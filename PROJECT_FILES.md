# OpenTournament Project Files

## Project Structure

```
OpenTournament/
├── CMakeLists.txt
├── README.md
├── LICENSE
├── CHANGELOG.md
├── build.sh
├── build.bat
├── src/
│   ├── main.cpp
│   ├── MainWindow.cpp
│   ├── MainWindow.h
│   ├── Database.cpp
│   ├── Database.h
│   ├── Player.cpp
│   ├── Player.h
│   ├── Match.cpp
│   ├── Match.h
│   ├── Tournament.cpp
│   └── Tournament.h
└── tournament.db (created at runtime)
```

## Core Components

### Build System

- **CMakeLists.txt**: CMake configuration file for building the project
- **build.sh**: Unix/Linux build script
- **build.bat**: Windows build script

### Documentation

- **README.md**: Project overview and usage instructions
- **LICENSE**: MIT License file
- **CHANGELOG.md**: Version history

### Source Code

- **src/main.cpp**: Application entry point
- **src/MainWindow.cpp**: Main window implementation
- **src/MainWindow.h**: Main window header
- **src/Database.cpp**: Database operations implementation
- **src/Database.h**: Database operations header
- **src/Player.cpp**: Player class implementation
- **src/Player.h**: Player class header
- **src/Match.cpp**: Match class implementation
- **src/Match.h**: Match class header
- **src/Tournament.cpp**: Tournament class implementation
- **src/Tournament.h**: Tournament class header

### Runtime Files

- **tournament.db**: SQLite database file (created when the application first runs)

## Features Implemented

1. Player management (add players with unique names)
2. Round-robin pairing algorithm for match generation
3. Tabbed interface for viewing matches by round
4. Match result submission with confirmation
5. Match locking mechanism to prevent accidental changes
6. Match editing capability with unlock confirmation
7. Leaderboard with points calculation (1 for win, 0.5 for draw, 0 for loss)
8. Tournament reset functionality
9. CSV export of results
10. Persistent data storage using SQLite

## Dependencies

- Qt6 Core
- Qt6 Gui
- Qt6 Widgets
- Qt6 Sql
- C++17 compatible compiler
- CMake 3.16 or higher
