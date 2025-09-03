# Tiebreaker System Specification

This document outlines the implementation plan for the tiebreaker system in the OpenTournament application.

## Current State

The application already has a basic tiebreaker implementation with the following tiebreakers:

1. Buchholz Cut One
2. Sonneborn–Berger Score
3. Direct Encounter
4. Number of Wins
5. Cumulative Opponent's Score

However, the implementation lacks:

1. User configuration options
2. Series tiebreaker with priority ordering
3. Tooltips with definitions
4. Computation button that only appears when needed
5. Proper integration with the leaderboard

## Requirements

### Functional Requirements

1. **Tiebreaker Selection**

   - Users should be able to choose between single tiebreaker or series with priority
   - Users should be able to reorder tiebreaker priorities
   - Settings should be saved and loaded persistently

2. **Tiebreaker Definitions**

   - Each tiebreaker should have a tooltip with its definition
   - Definitions should be accessible through hover tooltips
   - Definitions should be clear and understandable

3. **Computation Trigger**

   - A button should appear only when there are ties in the standings
   - The button should trigger a confirmation dialog before computing tiebreakers
   - Tiebreaker computation should update the leaderboard with adjusted rankings

4. **Series Implementation**
   - Support for applying tiebreakers in a specific order
   - Automatic fallback to next tiebreaker when previous one results in ties
   - Visual indication of which tiebreaker broke the tie

### Non-Functional Requirements

1. **Performance**

   - Tiebreaker computation should be fast even with large tournaments
   - Caching of intermediate results to avoid recomputation

2. **Usability**

   - Intuitive configuration interface
   - Clear visual feedback during computation
   - Helpful error messages for invalid configurations

3. **Reliability**
   - Proper error handling for edge cases
   - Data integrity during tiebreaker computation
   - Recovery mechanisms for failed computations

## Design

### Data Structures

```cpp
// Tiebreaker configuration structure
struct TiebreakerConfig {
    enum Type {
        BuchholzCutOne,
        SonnebornBerger,
        DirectEncounter,
        NumberOfWins,
        CumulativeOpponentScore
    };

    Type type;
    int priority; // Lower number means higher priority
    bool enabled;
};

// Tiebreaker settings class
class TiebreakerSettings {
private:
    QList<TiebreakerConfig> configs;

public:
    void addTiebreaker(TiebreakerConfig config);
    void removeTiebreaker(TiebreakerConfig::Type type);
    void setPriority(TiebreakerConfig::Type type, int priority);
    void enableTiebreaker(TiebreakerConfig::Type type, bool enabled);
    QList<TiebreakerConfig> getEnabledTiebreakersOrdered() const;
    QString getDefinition(TiebreakerConfig::Type type) const;
};
```

### UI Components

1. **Configuration Dialog**

   - List of available tiebreakers with checkboxes
   - Priority ordering through drag-and-drop or numeric input
   - Tooltips with definitions
   - Preview of tiebreaker order

2. **Leaderboard Integration**

   - Tiebreaker computation button that appears conditionally
   - Visual indicators for tiebreaker-applied rankings
   - Hover tooltips showing tiebreaker scores

3. **Settings Integration**
   - Tiebreaker settings section in preferences dialog
   - Persistent storage of user preferences
   - Default configuration fallback

### Algorithms

#### Buchholz Cut One

```
Algorithm BuchholzCutOne(player):
1. Calculate sum of all opponent scores
2. Subtract the score of the lowest-scoring opponent
3. Return result
```

#### Sonneborn–Berger Score

```
Algorithm SonnebornBerger(player):
1. For each win, add opponent's score to total
2. For each draw, add half of opponent's score to total
3. Return total
```

#### Direct Encounter

```
Algorithm DirectEncounter(player1, player2):
1. If players haven't played each other, return 0
2. If player1 won, return 1
3. If player2 won, return 0
4. If draw, return 0.5
```

#### Number of Wins

```
Algorithm NumberOfWins(player):
1. Count number of wins
2. Return count
```

#### Cumulative Opponent's Score

```
Algorithm CumulativeOpponentScore(player):
1. Calculate sum of all opponent scores
2. Return result
```

### Tiebreaker Application

```
Algorithm ApplyTiebreakers(players, tiebreakerConfigs):
1. For each tiebreaker in priority order:
   a. Apply tiebreaker to all players
   b. Sort players by tiebreaker score (descending)
   c. If no more ties, return sorted players
2. Return players sorted by last applied tiebreaker
```

## Implementation Plan

### Phase 1: Data Model and Core Logic

1. **Enhance Tiebreaker Class**

   - Add configuration management methods
   - Implement tiebreaker definitions
   - Add series computation logic

2. **Update Database Schema**

   - Add tiebreaker settings table
   - Add tiebreaker results storage
   - Add indexes for performance

3. **Implement Core Algorithms**
   - Complete all five tiebreaker algorithms
   - Add proper error handling
   - Add unit tests

### Phase 2: UI Components

1. **Configuration Dialog**

   - Design and implement tiebreaker selection UI
   - Add drag-and-drop priority ordering
   - Implement tooltips with definitions

2. **Leaderboard Integration**

   - Add conditional tiebreaker computation button
   - Implement hover tooltips for tiebreaker scores
   - Update leaderboard rendering logic

3. **Settings Integration**
   - Add tiebreaker settings to preferences dialog
   - Implement persistent storage
   - Add default configuration loading

### Phase 3: Integration and Testing

1. **Integration Testing**

   - Test tiebreaker computation with various scenarios
   - Validate UI behavior with different configurations
   - Verify data persistence

2. **Performance Optimization**

   - Profile tiebreaker computation performance
   - Implement caching for intermediate results
   - Optimize database queries

3. **User Acceptance Testing**
   - Conduct usability testing with sample users
   - Gather feedback on configuration interface
   - Validate tiebreaker computation accuracy

## Detailed Implementation

### Tiebreaker Class Enhancement

```cpp
class Tiebreaker {
public:
    enum Type {
        BuchholzCutOne,
        SonnebornBerger,
        DirectEncounter,
        NumberOfWins,
        CumulativeOpponentScore
    };

    // Enhanced methods
    static double calculateBuchholzCutOne(const Player &player, const QList<Player> &players, const QList<Match> &matches);
    static double calculateSonnebornBerger(const Player &player, const QList<Player> &players, const QList<Match> &matches);
    static double calculateDirectEncounter(const Player &player1, const Player &player2, const QList<Match> &matches);
    static int calculateNumberOfWins(const Player &player, const QList<Match> &matches);
    static double calculateCumulativeOpponentScore(const Player &player, const QList<Player> &players, const QList<Match> &matches);

    // New methods
    static QString getDefinition(Type type);
    static QString getName(Type type);
    static QList<Player> applyTiebreakers(const QList<Player> &players, const QList<Type> &tiebreakerOrder, const QList<Match> &matches);

private:
    static double getPlayerScore(const Player &player, const QList<Match> &matches);
    static QList<Player> getOpponents(const Player &player, const QList<Player> &players, const QList<Match> &matches);
    static double getOpponentScore(const Player &opponent, const QList<Match> &matches);
};
```

### Tiebreaker Settings Class

```cpp
class TiebreakerSettings {
private:
    QList<Tiebreaker::Type> tiebreakerOrder;
    QMap<Tiebreaker::Type, bool> enabledTiebreakers;

public:
    TiebreakerSettings();

    void setTiebreakerOrder(const QList<Tiebreaker::Type> &order);
    QList<Tiebreaker::Type> getTiebreakerOrder() const;

    void enableTiebreaker(Tiebreaker::Type type, bool enabled);
    bool isTiebreakerEnabled(Tiebreaker::Type type) const;

    QList<Tiebreaker::Type> getEnabledTiebreakers() const;

    void save();
    void load();

    static QString getDefinition(Tiebreaker::Type type);
};
```

### Database Schema Updates

```sql
-- Tiebreaker settings table
CREATE TABLE IF NOT EXISTS tiebreaker_settings (
    id INTEGER PRIMARY KEY CHECK (id = 1), -- Singleton table
    tiebreaker_order TEXT, -- JSON array of tiebreaker types in order
    enabled_tiebreakers TEXT -- JSON object mapping tiebreaker types to boolean
);

-- Tiebreaker results table
CREATE TABLE IF NOT EXISTS tiebreaker_results (
    player_id INTEGER NOT NULL,
    tournament_id INTEGER NOT NULL,
    tiebreaker_type TEXT NOT NULL,
    score REAL NOT NULL,
    PRIMARY KEY (player_id, tournament_id, tiebreaker_type),
    FOREIGN KEY (player_id) REFERENCES players(id),
    FOREIGN KEY (tournament_id) REFERENCES tournaments(id)
);
```

### UI Component Implementation

#### Configuration Dialog

```cpp
class TiebreakerConfigDialog : public QDialog {
    Q_OBJECT

private:
    QListWidget *tiebreakerList;
    QPushButton *moveUpButton;
    QPushButton *moveDownButton;
    QPushButton *enableButton;
    QPushButton *disableButton;

    TiebreakerSettings *settings;

public:
    TiebreakerConfigDialog(TiebreakerSettings *settings, QWidget *parent = nullptr);

private slots:
    void onMoveUpClicked();
    void onMoveDownClicked();
    void onEnableClicked();
    void onDisableClicked();
    void onSaveClicked();

private:
    void updateTiebreakerList();
    void showTiebreakerTooltip(QListWidgetItem *item);
};
```

#### Leaderboard Integration

```cpp
class LeaderboardWidget : public QTableWidget {
    Q_OBJECT

private:
    QPushButton *computeTiebreakersButton;
    TiebreakerSettings *settings;

public:
    LeaderboardWidget(TiebreakerSettings *settings, QWidget *parent = nullptr);

    void updateLeaderboard(const QList<Player> &players, const QList<Match> &matches);
    void checkForTies(const QList<Player> &players);

private slots:
    void onComputeTiebreakersClicked();

private:
    void showTiebreakerScores(const Player &player, const QPoint &pos);
    bool hasTies(const QList<Player> &players) const;
};
```

## Testing Strategy

### Unit Tests

1. **Tiebreaker Calculation Tests**

   - Test each tiebreaker algorithm with known inputs
   - Validate edge cases (no opponents, no matches, etc.)
   - Verify mathematical accuracy

2. **Series Application Tests**

   - Test tiebreaker application order
   - Validate fallback behavior when tiebreakers don't resolve ties
   - Verify correct player ranking after tiebreaker application

3. **Configuration Tests**
   - Test tiebreaker enabling/disabling
   - Validate priority ordering
   - Verify persistence of settings

### Integration Tests

1. **UI Integration Tests**

   - Test configuration dialog functionality
   - Validate leaderboard integration
   - Verify conditional button visibility

2. **Database Integration Tests**

   - Test tiebreaker settings persistence
   - Validate tiebreaker results storage
   - Verify data integrity during operations

3. **Performance Tests**
   - Measure tiebreaker computation time
   - Test with large player counts
   - Validate memory usage

### User Acceptance Tests

1. **Usability Tests**

   - Evaluate configuration dialog ease of use
   - Assess clarity of tiebreaker definitions
   - Gather feedback on visual presentation

2. **Accuracy Tests**
   - Verify tiebreaker computation correctness
   - Validate leaderboard ranking adjustments
   - Confirm proper handling of edge cases

## Deployment Plan

### Pre-deployment

1. **Code Review**

   - Review tiebreaker implementation for correctness
   - Validate database schema changes
   - Ensure proper error handling

2. **Testing**

   - Run complete test suite
   - Perform performance benchmarking
   - Conduct user acceptance testing

3. **Documentation**
   - Update user guides with tiebreaker features
   - Create developer documentation for implementation details
   - Prepare release notes

### Deployment

1. **Staged Rollout**

   - Deploy to beta testers first
   - Monitor for issues and bugs
   - Gather user feedback

2. **Full Release**
   - Deploy to production environment
   - Monitor application performance
   - Provide support for user questions

### Post-deployment

1. **Monitoring**

   - Track tiebreaker usage patterns
   - Monitor for performance issues
   - Collect user feedback

2. **Maintenance**
   - Address reported bugs
   - Implement requested enhancements
   - Update documentation as needed

## Risk Mitigation

### Technical Risks

1. **Performance Issues**

   - Implement caching for tiebreaker results
   - Optimize database queries
   - Profile and optimize critical paths

2. **Data Integrity**

   - Implement transaction support for critical operations
   - Add data validation before writes
   - Provide recovery mechanisms for failed operations

3. **Compatibility**
   - Maintain backward compatibility with existing databases
   - Implement migration scripts for schema changes
   - Test with various database versions

### User Experience Risks

1. **Complexity**

   - Provide clear tooltips and definitions
   - Implement intuitive UI controls
   - Add guided configuration options

2. **Confusion**

   - Provide clear visual feedback during computation
   - Add helpful error messages
   - Implement undo functionality where appropriate

3. **Misconfiguration**
   - Add validation for tiebreaker configurations
   - Provide sensible defaults
   - Implement warnings for potentially problematic configurations
