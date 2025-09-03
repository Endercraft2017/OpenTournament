# Database Fixes Specification

This document outlines the technical approach for fixing the database-related issues in the OpenTournament application.

## Identified Issues

1. "No query Unable to fetch row" errors
2. "Parameter count mismatch" errors
3. Match ID generation starting from arbitrary numbers
4. Missing tournament-scoped match IDs

## Root Cause Analysis

Based on the error messages, these issues are likely caused by:

1. Incorrect SQL query construction
2. Parameter binding mismatches
3. Missing error handling in database operations
4. Improper database schema design for match IDs

## Proposed Solutions

### 1. Query Construction Fixes

#### Problem

Queries may be constructed incorrectly, leading to malformed SQL statements.

#### Solution

- Use prepared statements consistently for all database operations
- Validate query construction before execution
- Add comprehensive error logging for failed queries

```cpp
// Example of proper query construction
QSqlQuery query(db);
query.prepare("SELECT id, name, seed FROM players WHERE id = ?");
query.addBindValue(playerId);

if (!query.exec()) {
    qDebug() << "Failed to get player by ID:" << query.lastError().text();
    return Player(); // Return default player
}

if (query.next()) {
    // Process result
} else {
    qDebug() << "No player found with ID:" << playerId;
    return Player(); // Return default player
}
```

### 2. Parameter Binding Fixes

#### Problem

Mismatch between the number of placeholders (?) in SQL queries and the number of parameters bound.

#### Solution

- Ensure each placeholder has exactly one corresponding `addBindValue()` call
- Count placeholders and compare with bound parameters
- Add validation before query execution

```cpp
// Example of proper parameter binding
QSqlQuery query(db);
query.prepare("INSERT INTO matches (tournament_id, round, p1, p2) VALUES (?, ?, ?, ?)");
query.addBindValue(tournamentId);
query.addBindValue(round);
query.addBindValue(p1Id);
query.addBindValue(p2Id);

// Verify parameter count matches placeholder count
Q_ASSERT(query.boundValues().size() == 4); // 4 placeholders in query

if (!query.exec()) {
    qDebug() << "Failed to add match:" << query.lastError().text();
    return -1;
}
```

### 3. Error Handling Improvements

#### Problem

Missing or inadequate error handling leads to uninformative error messages.

#### Solution

- Add comprehensive error checking for all database operations
- Log detailed error information including query text and parameters
- Return meaningful error codes from database functions

```cpp
// Example of improved error handling
bool Database::updatePlayer(const Player &player)
{
    QSqlQuery query(db);
    query.prepare("UPDATE players SET name = ?, seed = ? WHERE id = ?");
    query.addBindValue(player.getName());
    query.addBindValue(player.getSeed());
    query.addBindValue(player.getId());

    if (!query.exec()) {
        qDebug() << "Failed to update player:" << query.lastError().text();
        qDebug() << "Query:" << query.lastQuery();
        qDebug() << "Bound values:" << query.boundValues();
        return false;
    }

    int rowsAffected = query.numRowsAffected();
    if (rowsAffected == 0) {
        qDebug() << "No rows affected when updating player with ID:" << player.getId();
        return false;
    }

    return true;
}
```

### 4. Match ID Generation Fixes

#### Problem

Match IDs start from arbitrary numbers instead of 1, and are not scoped to tournaments.

#### Solution

- Modify database schema to include proper auto-increment behavior
- Implement tournament-scoped ID generation
- Reset ID sequences when starting new tournaments

```sql
-- Improved matches table schema
CREATE TABLE IF NOT EXISTS matches (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    tournament_id INTEGER NOT NULL,
    round INTEGER NOT NULL,
    p1 INTEGER NOT NULL,
    p2 INTEGER NOT NULL,
    result TEXT DEFAULT NULL,
    locked INTEGER DEFAULT 0,
    FOREIGN KEY (tournament_id) REFERENCES tournaments(id),
    FOREIGN KEY (p1) REFERENCES players(id),
    FOREIGN KEY (p2) REFERENCES players(id)
);
```

```cpp
// Function to get next match ID for a tournament
int Database::getNextMatchIdForTournament(int tournamentId)
{
    QSqlQuery query(db);
    query.prepare("SELECT COALESCE(MAX(id), 0) + 1 FROM matches WHERE tournament_id = ?");
    query.addBindValue(tournamentId);

    if (!query.exec() || !query.next()) {
        qDebug() << "Failed to get next match ID for tournament:" << query.lastError().text();
        return -1;
    }

    return query.value(0).toInt();
}
```

### 5. Tournament-Scoped Match IDs

#### Problem

All matches share a global ID space, making it difficult to track matches within tournaments.

#### Solution

- Use tournament_id as part of match identification
- Implement functions to work with tournament-scoped matches
- Add indexes for performance optimization

```cpp
// Function to get matches for a specific tournament
QList<Match> Database::getMatchesForTournament(int tournamentId)
{
    QList<Match> matches;
    QSqlQuery query(db);
    query.prepare("SELECT id, tournament_id, round, p1, p2, result, locked FROM matches WHERE tournament_id = ? ORDER BY round, id");
    query.addBindValue(tournamentId);

    if (!query.exec()) {
        qDebug() << "Failed to get matches for tournament:" << query.lastError().text();
        return matches;
    }

    while (query.next()) {
        int id = query.value(0).toInt();
        int tournamentId = query.value(1).toInt();
        int round = query.value(2).toInt();
        int p1 = query.value(3).toInt();
        int p2 = query.value(4).toInt();
        QString result = query.value(5).toString();
        bool locked = query.value(6).toBool();
        matches.append(Match(id, tournamentId, round, p1, p2, result, locked));
    }

    return matches;
}
```

## Implementation Steps

### Step 1: Schema Validation and Updates

1. Verify current database schema
2. Apply necessary schema updates
3. Add indexes for performance optimization
4. Validate foreign key constraints

### Step 2: Query Construction Review

1. Audit all database functions for query construction issues
2. Fix any malformed queries
3. Ensure consistent use of prepared statements
4. Add query validation logic

### Step 3: Parameter Binding Verification

1. Audit all database functions for parameter binding issues
2. Ensure each placeholder has exactly one corresponding `addBindValue()` call
3. Add parameter count validation
4. Implement helper functions for common parameter binding patterns

### Step 4: Error Handling Enhancement

1. Add comprehensive error checking to all database functions
2. Implement detailed error logging
3. Return meaningful error codes
4. Add retry logic for transient failures

### Step 5: Match ID Generation Implementation

1. Modify database schema if necessary
2. Implement tournament-scoped ID generation functions
3. Update match creation functions to use new ID generation
4. Add functions to reset ID sequences when needed

### Step 6: Testing and Validation

1. Create unit tests for all database functions
2. Test with various edge cases (empty tables, large datasets, etc.)
3. Validate error handling with simulated failures
4. Perform performance testing with large datasets

## Testing Plan

### Unit Tests

1. Test query construction with various inputs
2. Test parameter binding with correct and incorrect counts
3. Test error handling with simulated database failures
4. Test match ID generation with various scenarios

### Integration Tests

1. Test complete tournament lifecycle with database operations
2. Test concurrent access to database
3. Test data integrity with various operations
4. Test performance with large datasets

### Validation Tests

1. Verify all existing functionality continues to work
2. Validate new error messages are informative
3. Confirm match IDs start from 1 and are tournament-scoped
4. Verify all database operations handle errors gracefully

## Rollout Plan

### Phase 1: Development Environment

1. Implement all fixes in development branch
2. Run comprehensive unit tests
3. Perform integration testing
4. Validate with sample tournaments

### Phase 2: Staging Environment

1. Deploy to staging environment
2. Test with realistic data sets
3. Validate performance with large tournaments
4. Conduct user acceptance testing

### Phase 3: Production Release

1. Deploy to production environment
2. Monitor for any issues
3. Provide rollback plan if needed
4. Update documentation and user guides

## Risk Mitigation

### Data Loss Prevention

1. Implement database backups before deployment
2. Add transaction support for critical operations
3. Implement data validation before writes
4. Provide recovery mechanisms for failed operations

### Performance Impact

1. Add database indexing for frequently queried columns
2. Optimize query execution plans
3. Implement caching for frequently accessed data
4. Monitor performance metrics after deployment

### Compatibility Issues

1. Maintain backward compatibility with existing databases
2. Implement migration scripts for schema changes
3. Test with various database versions
4. Provide clear upgrade instructions for users
