# OpenTournament Build Instructions

## Prerequisites

- CMake 3.16 or higher
- Qt6 development libraries
- C++ compiler with C++17 support

## Build Process

### 1. Configure the project

```bash
cmake -S . -B build
```

### 2. Build the project

```bash
cmake --build build
```

### 3. Run the application

```bash
./build/OpenTournament
```

## Directory Structure

```
OpenTournament/
├── CMakeLists.txt          # Build configuration
├── README.md               # Project overview
├── build_instructions.md   # This file
├── project_plan.md         # Architecture and planning
├── development_plan.md     # Implementation details
├── src/                    # Source code
│   ├── main.cpp           # Application entry point
│   ├── MainWindow.cpp     # Main window implementation
│   ├── MainWindow.h       # Main window header
│   ├── Database.cpp       # Database operations
│   ├── Database.h         # Database header
│   ├── Player.cpp         # Player class implementation
│   ├── Player.h           # Player class header
│   ├── Match.cpp          # Match class implementation
│   ├── Match.h            # Match class header
│   ├── Tournament.cpp     # Tournament class implementation
│   └── Tournament.h       # Tournament class header
├── resources/             # Application resources (icons, styles, etc.)
└── tournament.db          # SQLite database file
```

## Implementation Steps

### Step 1: Create CMakeLists.txt

- Set minimum CMake version to 3.16
- Define project with C++17 standard
- Find required Qt6 components (Core, Gui, Widgets, Sql)
- Add executable with all source files
- Link Qt6 libraries

### Step 2: Implement Core Classes

- Player class with id, name, and seed
- Match class with id, round, players, result, and locked status
- Tournament class with id, name, and creation timestamp

### Step 3: Database Integration

- Create SQLite database connection
- Implement schema creation
- Add functions for CRUD operations
- Handle foreign key relationships

### Step 4: Pairing Algorithm

- Implement round-robin pairing using circle method
- Handle both even and odd number of players
- Generate matches for each round

### Step 5: UI Implementation

- Create main window with Qt Widgets
- Implement player management interface
- Create match display with tabbed layout
- Add leaderboard display
- Implement toolbar and menu

### Step 6: Feature Implementation

- Add player management (add/edit/remove)
- Implement match result submission
- Create match locking mechanism
- Implement leaderboard sorting
- Add tournament reset functionality
- Implement CSV export

### Step 7: Testing

- Verify player uniqueness
- Test pairing algorithm with different player counts
- Verify match result submission and locking
- Test unlock/edit functionality
- Verify CSV export
- Test tournament reset

## Packaging

- Use CMake install rules
- Bundle required Qt DLLs on Windows
- Create distributable package

## Troubleshooting

- Ensure Qt6 is properly installed and in PATH
- Check CMake version compatibility
- Verify database permissions
- Check for missing dependencies
