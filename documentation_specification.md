# Documentation Specification for OpenTournament

## Overview

This document specifies the documentation requirements for the OpenTournament application. It covers user documentation, developer documentation, and technical specifications.

## User Documentation

### README.md

```markdown
# OpenTournament

OpenTournament is a desktop application for managing tournament brackets using a round-robin pairing system. It allows you to organize tournaments, track match results, and determine final standings.

## Features

- Player management (add/edit/remove players)
- Auto-generate round-robin pairings using circle method
- Display rounds & matches in a tabbed layout
- Click-to-set winner (Player A / Player B / Draw)
- Confirm result submission → locks match row
- Edit/unlock flow via explicit "Edit" button with confirm dialog
- Live leaderboard sorted by points
- Reset tournament with confirmation
- Export results to CSV
- Persistent data via SQLite

## System Requirements

- Windows 7 or later, macOS 10.12 or later, or Linux
- Qt6 runtime libraries

## Installation

1. Download the latest release for your platform
2. Extract the archive
3. Run the OpenTournament executable

## Getting Started

1. Add players using the "Add Player" button
2. Generate pairings automatically
3. Record match results as they occur
4. View standings in the leaderboard
5. Export results when the tournament is complete

## Usage

### Adding Players

1. Click the "Add Player" button in the toolbar
2. Enter the player's name in the dialog
3. Click "OK" to add the player

### Recording Match Results

1. Navigate to the appropriate round tab
2. Click the result button (Player 1 Win, Draw, Player 2 Win) for the match
3. Confirm the result in the dialog
4. The match will be locked to prevent accidental changes

### Editing Match Results

1. Click the "Edit" button on a locked match
2. Confirm that you want to unlock the match
3. Change the result using the result buttons
4. Confirm the new result

### Exporting Results

1. Click the "Export Results" button in the toolbar
2. Choose a location and filename for the CSV file
3. Click "Save" to export the results

### Resetting the Tournament

1. Click the "Reset Tournament" button in the toolbar
2. Confirm that you want to clear all match data
3. The tournament will be reset, but players will be retained

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Support

For issues, feature requests, or questions, please visit our GitHub repository.
```

### User Manual

```markdown
# OpenTournament User Manual

## Table of Contents

1. Introduction
2. Installation
3. Getting Started
4. Player Management
5. Tournament Management
6. Match Management
7. Leaderboard
8. Exporting Results
9. Troubleshooting
10. FAQ

## 1. Introduction

OpenTournament is a desktop application designed to help you organize and manage round-robin tournaments. Whether you're running a chess tournament, a gaming competition, or any other type of competition where each participant plays every other participant, OpenTournament makes it easy to track results and determine final standings.

## 2. Installation

### Windows

1. Download the Windows installer or ZIP archive
2. Run the installer or extract the ZIP archive
3. Run OpenTournament.exe

### macOS

1. Download the DMG file
2. Open the DMG and drag OpenTournament to your Applications folder
3. Run OpenTournament from your Applications folder

### Linux

1. Download the tar.gz archive
2. Extract the archive
3. Run the OpenTournament executable

## 3. Getting Started

When you first run OpenTournament, you'll see an empty interface. To get started:

1. Add players using the "Add Player" button
2. Generate pairings (happens automatically when you have enough players)
3. Start recording match results

## 4. Player Management

### Adding Players

To add players:

1. Click the "Add Player" button in the toolbar
2. Enter the player's name in the dialog that appears
3. Click "OK"

Player names must be unique. If you try to add a player with a name that already exists, you'll see an error message.

### Viewing Players

All players are displayed in the left panel, sorted by their current ranking in the tournament.

### Removing Players

Currently, there is no direct way to remove players. Players remain in the system for the duration of the tournament.

## 5. Tournament Management

### Generating Pairings

Pairings are generated automatically when you have at least 3 players. The system uses the round-robin method to ensure each player plays every other player exactly once.

### Tournament Structure

- For an even number of players: (n-1) rounds with n/2 matches per round
- For an odd number of players: n rounds with (n-1)/2 matches per round (one player has a BYE each round)

## 6. Match Management

### Recording Results

To record a match result:

1. Navigate to the appropriate round tab
2. Click the appropriate result button (Player 1 Win, Draw, Player 2 Win)
3. Confirm the result in the dialog that appears

### Editing Results

To edit a recorded result:

1. Click the "Edit" button next to the locked match
2. Confirm that you want to unlock the match
3. Change the result using the result buttons
4. Confirm the new result

## 7. Leaderboard

The leaderboard in the left panel shows all players ranked by their points:

- Win: 1 point
- Draw: 0.5 points
- Loss: 0 points

Players are sorted by points in descending order. In case of a tie, players are sorted alphabetically by name.

## 8. Exporting Results

To export tournament results:

1. Click the "Export Results" button in the toolbar
2. Choose a location and filename for the CSV file
3. Click "Save"

The exported CSV file contains:

- Player name
- Total points
- Number of wins
- Number of losses
- Number of draws

## 9. Troubleshooting

### Common Issues

#### Application won't start

- Ensure all required Qt libraries are installed
- Check that your system meets the minimum requirements

#### Players not appearing

- Ensure you've clicked "OK" when adding players
- Check that player names are unique

#### Match results not saving

- Ensure you've confirmed the result in the dialog
- Check that the application has write permissions to its directory

### Reporting Issues

If you encounter issues not covered in this manual, please report them to our GitHub repository with:

- A description of the issue
- Steps to reproduce
- Your operating system and version
- Application version

## 10. FAQ

### Can I have multiple tournaments?

Currently, OpenTournament supports one active tournament at a time. You can reset the tournament to start a new one.

### How are ties handled in the leaderboard?

Players with the same number of points are sorted alphabetically by name.

### Can I print the standings?

Not directly from the application, but you can export to CSV and open the file in a spreadsheet application for printing.

### What happens if I close the application?

All tournament data is saved automatically. When you reopen the application, your tournament will be restored.

### Is my data secure?

Tournament data is stored locally on your computer in an SQLite database file. Only you have access to this data.
```

## Developer Documentation

### Development Guide

```markdown
# OpenTournament Development Guide

## Project Structure
```

OpenTournament/
├── CMakeLists.txt
├── README.md
├── src/
│ ├── main.cpp
│ ├── MainWindow.cpp
│ ├── MainWindow.h
│ ├── Database.cpp
│ ├── Database.h
│ ├── Player.cpp
│ ├── Player.h
│ ├── Match.cpp
│ ├── Match.h
│ ├── Tournament.cpp
│ └── Tournament.h
├── resources/
└── tournament.db

```

## Building from Source
### Prerequisites
- CMake 3.16 or higher
- Qt6 development libraries
- C++ compiler with C++17 support

### Build Steps
1. Clone the repository
2. Configure: `cmake -S . -B build`
3. Build: `cmake --build build`
4. Run: `./build/OpenTournament`

## Architecture
### Overview
OpenTournament follows a Model-View-Controller (MVC) pattern:
- **Model**: Player, Match, Tournament classes
- **View**: Qt Widgets UI
- **Controller**: MainWindow class and manager classes

### Components
1. **MainWindow**: Main application window and UI controller
2. **Database**: SQLite database interface
3. **PlayerManager**: Player-related operations
4. **MatchManager**: Match-related operations
5. **TournamentManager**: Tournament-level operations

## Code Style
- Follow Qt coding conventions
- Use camelCase for variables and functions
- Use PascalCase for classes
- Use ALL_CAPS for constants
- Comment public interfaces
- Keep functions focused and small

## Contributing
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Update documentation
6. Submit a pull request

## Testing
See testing_specification.md for detailed testing procedures.
```

### API Documentation

```markdown
# OpenTournament API Documentation

## Classes

### Player

Represents a tournament player.

#### Constructors

- `Player()`: Default constructor
- `Player(int id, const QString& name, int seed = 0)`: Constructor with parameters

#### Methods

- `int getId() const`: Get player ID
- `QString getName() const`: Get player name
- `int getSeed() const`: Get player seed
- `void setId(int id)`: Set player ID
- `void setName(const QString& name)`: Set player name
- `void setSeed(int seed)`: Set player seed

### Match

Represents a tournament match.

#### Constructors

- `Match()`: Default constructor
- `Match(int id, int round, int p1, int p2, const QString& result = QString(), bool locked = false)`: Constructor with parameters

#### Methods

- `int getId() const`: Get match ID
- `int getRound() const`: Get round number
- `int getPlayer1Id() const`: Get player 1 ID
- `int getPlayer2Id() const`: Get player 2 ID
- `QString getResult() const`: Get match result
- `bool isLocked() const`: Check if match is locked
- `void setId(int id)`: Set match ID
- `void setRound(int round)`: Set round number
- `void setPlayer1Id(int p1)`: Set player 1 ID
- `void setPlayer2Id(int p2)`: Set player 2 ID
- `void setResult(const QString& result)`: Set match result
- `void setLocked(bool locked)`: Set lock status
- `bool isPlayed() const`: Check if match has been played
- `bool isDraw() const`: Check if match is a draw
- `int getWinnerId() const`: Get winner's ID

### Tournament

Represents a tournament.

#### Constructors

- `Tournament()`: Default constructor
- `Tournament(int id, const QString& name, const QDateTime& createdAt = QDateTime::currentDateTime())`: Constructor with parameters

#### Methods

- `int getId() const`: Get tournament ID
- `QString getName() const`: Get tournament name
- `QDateTime getCreatedAt() const`: Get creation timestamp
- `void setId(int id)`: Set tournament ID
- `void setName(const QString& name)`: Set tournament name
- `void setCreatedAt(const QDateTime& createdAt)`: Set creation timestamp

### Database

Handles database operations.

#### Methods

- `bool connect()`: Connect to database
- `void disconnect()`: Disconnect from database
- `bool isConnected() const`: Check connection status
- `int addPlayer(const QString& name, int seed = 0)`: Add player
- `QList<Player> getAllPlayers()`: Get all players
- `Player getPlayerById(int id)`: Get player by ID
- `bool updatePlayer(const Player& player)`: Update player
- `bool deletePlayer(int id)`: Delete player
- `int addMatch(int round, int p1, int p2)`: Add match
- `QList<Match> getMatchesForRound(int round)`: Get matches for round
- `QList<Match> getAllMatches()`: Get all matches
- `bool updateMatchResult(int matchId, const QString& result)`: Update match result
- `bool lockMatch(int matchId)`: Lock match
- `bool unlockMatch(int matchId)`: Unlock match
- `bool deleteMatch(int id)`: Delete match
- `bool initializeSchema()`: Initialize database schema
- `bool resetDatabase()`: Reset database
```

## Technical Documentation

### Database Schema Documentation

```markdown
# Database Schema Documentation

## Overview

OpenTournament uses SQLite for data storage. The database contains three tables: players, matches, and tournaments.

## Tables

### players

Stores information about tournament players.

#### Columns

- `id` (INTEGER PRIMARY KEY AUTOINCREMENT): Unique player identifier
- `name` (TEXT NOT NULL UNIQUE): Player name (must be unique)
- `seed` (INTEGER DEFAULT 0): Player seeding value

#### Example Data

| id  | name  | seed |
| --- | ----- | ---- |
| 1   | Alice | 1    |
| 2   | Bob   | 2    |
| 3   | Carol | 0    |

### matches

Stores information about tournament matches.

#### Columns

- `id` (INTEGER PRIMARY KEY AUTOINCREMENT): Unique match identifier
- `round` (INTEGER NOT NULL): Round number
- `p1` (INTEGER NOT NULL): Player 1 ID (foreign key to players.id)
- `p2` (INTEGER NOT NULL): Player 2 ID (foreign key to players.id)
- `result` (TEXT DEFAULT NULL): Match result ('p1', 'p2', 'draw', or NULL)
- `locked` (INTEGER DEFAULT 0): Lock status (0 = unlocked, 1 = locked)

#### Example Data

| id  | round | p1  | p2  | result | locked |
| --- | ----- | --- | --- | ------ | ------ |
| 1   | 1     | 1   | 2   | p1     | 1      |
| 2   | 1     | 3   | 4   | draw   | 1      |

### tournaments

Stores information about tournaments.

#### Columns

- `id` (INTEGER PRIMARY KEY AUTOINCREMENT): Unique tournament identifier
- `name` (TEXT): Tournament name
- `created_at` (TIMESTAMP DEFAULT CURRENT_TIMESTAMP): Creation timestamp

#### Example Data

| id  | name              | created_at          |
| --- | ----------------- | ------------------- |
| 1   | Summer Tournament | 2023-06-01 10:00:00 |

## Relationships

- matches.p1 → players.id
- matches.p2 → players.id
```

### Pairing Algorithm Documentation

```markdown
# Pairing Algorithm Documentation

## Overview

OpenTournament uses the circle method to generate round-robin pairings. This ensures that each player plays every other player exactly once.

## Algorithm Description

The circle method works as follows:

1. If the number of players is odd, add a "BYE" placeholder
2. For each round:
   - Pair players from opposite ends of the circle
   - Rotate all positions except the first one
3. Continue until all rounds are generated

## Implementation Details

See pairing_algorithm_specification.md for detailed implementation.

## Example

For 4 players (A, B, C, D):

- Round 1: A-D, B-C
- Round 2: A-C, D-B
- Round 3: A-B, C-D

For 5 players (A, B, C, D, E):

- Round 1: A-E, B-D (C has BYE)
- Round 2: A-D, E-C (B has BYE)
- Round 3: A-C, D-B (E has BYE)
- Round 4: A-B, C-E (D has BYE)
- Round 5: A-E, B-C (D has BYE)
```

## Changelog

```markdown
# Changelog

## 0.1.0 (Initial Release)

- Player management (add/edit/remove players)
- Auto-generate round-robin pairings using circle method
- Display rounds & matches in a tabbed layout
- Click-to-set winner (Player A / Player B / Draw)
- Confirm result submission → locks match row
- Edit/unlock flow via explicit "Edit" button with confirm dialog
- Live leaderboard sorted by points
- Reset tournament with confirmation
- Export results to CSV
- Persistent data via SQLite
```

## License

```markdown
# License

MIT License

Copyright (c) 2025 OpenTournament Contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
