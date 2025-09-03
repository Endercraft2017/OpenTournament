# Leaderboard Functionality Specification

## Overview

This document specifies the implementation of the leaderboard functionality for the OpenTournament application. The leaderboard displays players ranked by their points in descending order.

## Scoring System

### Points Calculation

- **Win**: 1 point
- **Draw**: 0.5 points
- **Loss**: 0 points
- **BYE**: 1 point (automatic win)

### Example

- Alice: 2 wins, 1 draw = 2.5 points
- Bob: 1 win, 2 losses = 1 point
- Carol: 1 draw, 2 losses = 0.5 points
- Dave: 3 losses = 0 points

## Database Queries

### Get Player Points

```sql
SELECT p.id, p.name,
       COALESCE(SUM(CASE
           WHEN m.result = 'p1' AND m.p1 = p.id THEN 1
           WHEN m.result = 'p2' AND m.p2 = p.id THEN 1
           WHEN m.result = 'draw' THEN 0.5
           ELSE 0
       END), 0) AS points
FROM players p
LEFT JOIN matches m ON (p.id = m.p1 OR p.id = m.p2) AND m.result IS NOT NULL
GROUP BY p.id, p.name
ORDER BY points DESC, p.name ASC
```

## Implementation Details

### PlayerManager Class

```cpp
class PlayerManager {
public:
    struct PlayerRanking {
        int id;
        QString name;
        double points;
    };

    // Get ranked players
    QList<PlayerRanking> getRankedPlayers();

    // Calculate points for a specific player
    double calculatePlayerPoints(int playerId);

    // Update leaderboard UI
    void updateLeaderboard();

private:
    Database* database;
};
```

### MainWindow Integration

```cpp
// In MainWindow class
private:
    void updatePlayerTable();
    void refreshLeaderboard();
```

## UI Implementation

### Player Table Structure

```
+------+----------------+--------+
| Rank | Player Name    | Points |
+------+----------------+--------+
| 1    | Alice          | 2.5    |
| 2    | Bob            | 1.0    |
| 3    | Carol          | 0.5    |
| 4    | Dave           | 0.0    |
+------+----------------+--------+
```

### Table Properties

- **Type**: QTableWidget
- **Columns**:
  - Rank (integer, 1-based)
  - Player Name (string)
  - Points (double, formatted to 1 decimal place)
- **Sorting**: By points descending, then by name ascending
- **Read-only**: Yes (no editing directly in table)

### Update Mechanism

The leaderboard should update automatically when:

1. A new player is added
2. A match result is submitted
3. A match result is edited
4. The tournament is reset

## Ranking Algorithm

### Steps

1. Retrieve all players from database
2. For each player, calculate total points:
   - Count wins (1 point each)
   - Count draws (0.5 points each)
   - Losses = 0 points
3. Sort players by points (descending)
4. Assign ranks based on sorted order
5. Handle ties by alphabetical order of player names

### Tie-Breaking

If two or more players have the same points:

1. Sort by head-to-head results (if applicable)
2. Sort alphabetically by player name

## Performance Considerations

### Caching

- Cache player points to avoid repeated database queries
- Invalidate cache when match results change
- Update cache incrementally when possible

### Database Optimization

- Use efficient SQL queries with proper JOINs
- Consider indexing on frequently queried columns
- Limit result sets when possible

## Edge Cases

### No Players

- Display empty table with appropriate message
- Hide leaderboard if no players exist

### No Matches Played

- All players have 0 points
- Rank by alphabetical order

### New Players Added Mid-Tournament

- New players start with 0 points
- Recalculate rankings immediately

## Testing Scenarios

### Basic Functionality

1. Verify correct points calculation for wins/draws/losses
2. Verify correct ranking order
3. Verify tie-breaking works correctly

### Dynamic Updates

1. Add new player and verify leaderboard updates
2. Submit match result and verify points update
3. Edit match result and verify points recalculate
4. Reset tournament and verify leaderboard resets

### Edge Cases

1. Test with no players
2. Test with no matches played
3. Test with tied points
4. Test with large number of players

## UI/UX Considerations

### Visual Design

- Clear column headers
- Alternating row colors for readability
- Highlight top players (e.g., gold for 1st, silver for 2nd, bronze for 3rd)
- Proper number formatting (1 decimal place)

### Responsiveness

- Update leaderboard quickly after changes
- Show loading indicator during calculations if needed
- Handle large datasets efficiently

### Accessibility

- Keyboard navigation support
- Screen reader compatibility
- Sufficient color contrast
