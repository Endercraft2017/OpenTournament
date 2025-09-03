# Database Specification for OpenTournament

## Overview

OpenTournament uses SQLite for data persistence. The database will store information about players, matches, and tournaments.

## Database File

- File name: `tournament.db`
- Location: Application directory
- Created automatically if not exists

## Schema

### Players Table

```sql
CREATE TABLE players (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name TEXT NOT NULL UNIQUE,
  seed INTEGER DEFAULT 0
);
```

**Columns:**

- `id`: Unique identifier for each player
- `name`: Player's name (must be unique)
- `seed`: Seeding value for tournament ordering

### Matches Table

```sql
CREATE TABLE matches (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  round INTEGER NOT NULL,
  p1 INTEGER NOT NULL,
  p2 INTEGER NOT NULL,
  result TEXT DEFAULT NULL, -- 'p1','p2','draw'
  locked INTEGER DEFAULT 0,
  FOREIGN KEY (p1) REFERENCES players(id),
  FOREIGN KEY (p2) REFERENCES players(id)
);
```

**Columns:**

- `id`: Unique identifier for each match
- `round`: Round number in the tournament
- `p1`: Foreign key referencing player 1 (from players table)
- `p2`: Foreign key referencing player 2 (from players table)
- `result`: Match result ('p1', 'p2', 'draw', or NULL for unplayed)
- `locked`: Flag indicating if the match result is locked (0 = unlocked, 1 = locked)

### Tournaments Table

```sql
CREATE TABLE tournaments (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name TEXT,
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

**Columns:**

- `id`: Unique identifier for each tournament
- `name`: Tournament name
- `created_at`: Timestamp when the tournament was created

## Database Integration Approach

### Connection Management

- Use Qt's `QSqlDatabase` class for database connections
- Use SQLite driver
- Handle connection opening and closing properly
- Implement error handling for database operations

### Query Execution

- Use `QSqlQuery` for executing SQL statements
- Prepare statements for better performance and security
- Handle query results appropriately
- Implement proper error checking

### Data Access Patterns

1. **Player Operations:**

   - Insert new player
   - Retrieve all players
   - Retrieve player by ID
   - Update player information
   - Delete player

2. **Match Operations:**

   - Insert new match
   - Retrieve matches for a round
   - Retrieve all matches
   - Update match result
   - Lock/unlock match
   - Delete match

3. **Tournament Operations:**
   - Insert new tournament
   - Retrieve all tournaments
   - Retrieve tournament by ID
   - Update tournament information
   - Delete tournament

## Database Class Interface

### Database Class Methods

```cpp
class Database {
public:
    // Constructor and destructor
    Database();
    ~Database();

    // Connection management
    bool connect();
    void disconnect();
    bool isConnected() const;

    // Player operations
    int addPlayer(const QString& name, int seed = 0);
    QList<Player> getAllPlayers();
    Player getPlayerById(int id);
    bool updatePlayer(const Player& player);
    bool deletePlayer(int id);

    // Match operations
    int addMatch(int round, int p1, int p2);
    QList<Match> getMatchesForRound(int round);
    QList<Match> getAllMatches();
    bool updateMatchResult(int matchId, const QString& result);
    bool lockMatch(int matchId);
    bool unlockMatch(int matchId);
    bool deleteMatch(int id);

    // Tournament operations
    int addTournament(const QString& name);
    QList<Tournament> getAllTournaments();
    Tournament getTournamentById(int id);
    bool updateTournament(const Tournament& tournament);
    bool deleteTournament(int id);

    // Utility methods
    bool initializeSchema();
    bool resetDatabase();

private:
    QSqlDatabase db;
    QString dbPath;
};
```

## Error Handling

- Check database connection status before operations
- Validate query execution results
- Handle SQL errors appropriately
- Provide meaningful error messages to the UI
