# Data Models Specification for OpenTournament

## Overview

This document specifies the core data models for the OpenTournament application: Player, Match, and Tournament.

## Player Model

### Header File (Player.h)

```cpp
#ifndef PLAYER_H
#define PLAYER_H

#include <QString>

class Player {
public:
    // Constructors
    Player();
    Player(int id, const QString& name, int seed = 0);

    // Getters
    int getId() const;
    QString getName() const;
    int getSeed() const;

    // Setters
    void setId(int id);
    void setName(const QString& name);
    void setSeed(int seed);

    // Comparison operators
    bool operator==(const Player& other) const;
    bool operator!=(const Player& other) const;

private:
    int id;
    QString name;
    int seed;
};

#endif // PLAYER_H
```

### Implementation Details

- **id**: Unique identifier for the player (integer)
- **name**: Player's name (QString), must be unique
- **seed**: Seeding value for tournament ordering (integer, default 0)

## Match Model

### Header File (Match.h)

```cpp
#ifndef MATCH_H
#define MATCH_H

#include <QString>

class Match {
public:
    // Constructors
    Match();
    Match(int id, int round, int p1, int p2, const QString& result = QString(), bool locked = false);

    // Getters
    int getId() const;
    int getRound() const;
    int getPlayer1Id() const;
    int getPlayer2Id() const;
    QString getResult() const;
    bool isLocked() const;

    // Setters
    void setId(int id);
    void setRound(int round);
    void setPlayer1Id(int p1);
    void setPlayer2Id(int p2);
    void setResult(const QString& result);
    void setLocked(bool locked);

    // Helper methods
    bool isPlayed() const;
    bool isDraw() const;
    int getWinnerId() const; // Returns ID of winner, -1 for draw or unplayed

private:
    int id;
    int round;
    int p1; // Player 1 ID
    int p2; // Player 2 ID
    QString result; // "p1", "p2", "draw", or empty for unplayed
    bool locked;
};

#endif // MATCH_H
```

### Implementation Details

- **id**: Unique identifier for the match (integer)
- **round**: Round number in the tournament (integer)
- **p1**: Player 1 ID (integer, foreign key to players table)
- **p2**: Player 2 ID (integer, foreign key to players table)
- **result**: Match result ("p1", "p2", "draw", or empty for unplayed)
- **locked**: Flag indicating if the match result is locked (boolean)

## Tournament Model

### Header File (Tournament.h)

```cpp
#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include <QString>
#include <QDateTime>

class Tournament {
public:
    // Constructors
    Tournament();
    Tournament(int id, const QString& name, const QDateTime& createdAt = QDateTime::currentDateTime());

    // Getters
    int getId() const;
    QString getName() const;
    QDateTime getCreatedAt() const;

    // Setters
    void setId(int id);
    void setName(const QString& name);
    void setCreatedAt(const QDateTime& createdAt);

    // Comparison operators
    bool operator==(const Tournament& other) const;
    bool operator!=(const Tournament& other) const;

private:
    int id;
    QString name;
    QDateTime createdAt;
};

#endif // TOURNAMENT_H
```

### Implementation Details

- **id**: Unique identifier for the tournament (integer)
- **name**: Tournament name (QString)
- **createdAt**: Timestamp when the tournament was created (QDateTime)

## Relationships

### Player-Match Relationship

- Each match references two players (p1 and p2)
- Players can participate in multiple matches
- Foreign key constraints ensure data integrity

### Tournament-Player Relationship

- In the current implementation, the relationship is minimal
- Players belong to the overall system rather than specific tournaments
- Future extensions could link players to specific tournaments

### Tournament-Match Relationship

- Matches are associated with a specific tournament
- Future extensions could include a tournament_id field in matches

## Usage Examples

### Creating a Player

```cpp
Player player(1, "Alice", 1);
// or
Player player;
player.setId(1);
player.setName("Alice");
player.setSeed(1);
```

### Creating a Match

```cpp
Match match(1, 1, 1, 2, "p1", true);
// or
Match match;
match.setId(1);
match.setRound(1);
match.setPlayer1Id(1);
match.setPlayer2Id(2);
match.setResult("p1");
match.setLocked(true);
```

### Creating a Tournament

```cpp
QDateTime now = QDateTime::currentDateTime();
Tournament tournament(1, "Summer Championship", now);
// or
Tournament tournament;
tournament.setId(1);
tournament.setName("Summer Championship");
tournament.setCreatedAt(now);
```
