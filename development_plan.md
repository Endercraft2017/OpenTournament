# OpenTournament Development Plan

## Project Structure

```
OpenTournament/
├── CMakeLists.txt
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
├── resources/
└── tournament.db
```

## Implementation Order

### 1. CMakeLists.txt

- Set up CMake configuration with Qt6
- Define project requirements
- Configure build targets

### 2. Core Data Models

- Player class
- Match class
- Tournament class

### 3. Database Integration

- SQLite database setup
- Schema implementation
- Query functions

### 4. Pairing Algorithm

- Round-robin implementation
- Player pairing logic

### 5. Main Window

- UI layout design
- Widget integration
- Event handling

### 6. Feature Implementation

- Player management
- Match result handling
- Leaderboard
- Export functionality

## File Implementation Details

### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(OpenTournament VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql)

add_executable(OpenTournament
    src/main.cpp
    src/MainWindow.cpp
    src/Database.cpp
    src/Player.cpp
    src/Match.cpp
    src/Tournament.cpp
)

target_link_libraries(OpenTournament PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql)
```

### Database Schema

```sql
CREATE TABLE players (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name TEXT NOT NULL UNIQUE,
  seed INTEGER DEFAULT 0
);

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

CREATE TABLE tournaments (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name TEXT,
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

### Round-Robin Pairing Algorithm

```cpp
#include <vector>
#include <string>
#include <optional>

struct Pair { std::string p1; std::string p2; };

std::vector<std::vector<Pair>> roundRobinPairings(std::vector<std::string> players) {
    int n = players.size();
    if (n % 2 == 1) {
        players.push_back("BYE");
        n++;
    }
    std::vector<std::vector<Pair>> rounds;
    for (int r = 0; r < n - 1; r++) {
        std::vector<Pair> round;
        for (int i = 0; i < n/2; i++) {
            std::string p1 = players[i];
            std::string p2 = players[n-1-i];
            if (p1 != "BYE" && p2 != "BYE")
                round.push_back({p1, p2});
        }
        rounds.push_back(round);
        // rotate
        std::rotate(players.begin()+1, players.end()-1, players.end());
    }
    return rounds;
}
```
