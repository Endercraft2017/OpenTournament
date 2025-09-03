# OpenTournament — Full Build Instructions

This document is a **step‑by‑step build guide** for creating the OpenTournament desktop application using **Qt6 + CMake**. It merges the requirements from both planning docs and expands them into very detailed implementation instructions that Kilo Code (or any AI assistant IDE) can follow without ambiguity.

---

## 1. Project Setup

### 1.1 Create project structure

- Project name: `OpenTournament`
- Main source folder: `src/`
- Resources (icons, qml, styles): `resources/`
- Database file (runtime): `tournament.db`
- Build system: `CMake`

### 1.2 CMake configuration

- Minimum version: `cmake_minimum_required(VERSION 3.16)`
- Project declaration:
  ```cmake
  project(OpenTournament VERSION 0.1 LANGUAGES CXX)
  ```
- Set C++ standard:
  ```cmake
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  ```
- Find required Qt6 packages:
  ```cmake
  find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql)
  ```
- Add executable:
  ```cmake
  add_executable(OpenTournament
      src/main.cpp
      src/MainWindow.cpp
      src/MainWindow.h
      src/Database.cpp
      src/Database.h
      src/Player.cpp
      src/Player.h
      src/Match.cpp
      src/Match.h
      src/Tournament.cpp
      src/Tournament.h
  )
  ```
- Link Qt6 libraries:
  ```cmake
  target_link_libraries(OpenTournament PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql)
  ```

### 1.3 Build instructions

1. Configure the project:
   ```bash
   cmake -S . -B build
   ```
2. Build the project:
   ```bash
   cmake --build build
   ```
3. Run the binary:
   ```bash
   ./build/OpenTournament
   ```

---

## 2. Application Features

### MVP features

1. Player management (add/edit/remove players).
2. Auto‑generate round‑robin pairings using circle method.
3. Display rounds & matches in a table or tabbed layout.
4. Click‑to‑set winner (Player A / Player B / Draw).
5. Confirm result submission → locks match row.
6. Edit/unlock flow via explicit “Edit” button with confirm dialog.
7. Live leaderboard sorted by points.
8. Reset tournament with confirmation.
9. A button to Export results to CSV/XLSX.
10. Persistent data via SQLite.

### Nice‑to‑have (if time permits)

- Multiple tournaments in DB.
- Tie‑break algorithms (head‑to‑head, Sonneborn‑Berger).
- Printable final standings.
- An option for swiss pairing system

---

## 3. Database Schema (SQLite)

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

- Use Qt’s `QSqlDatabase` for connection.
- Use `QSqlQuery` for inserts/updates.
- Store DB in `tournament.db` (created if not exists).

---

## 4. Pairing Algorithm (C++)

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

---

## 5. UI / UX (Qt Widgets)

- **MainWindow** layout:
  - Left panel: Player list & leaderboard (QTableWidget).
  - Right panel: Tabs for rounds (QTabWidget).
  - Each tab: Matches in a table with 3 buttons: A wins / Draw / B wins.
- **Match row states:**
  - Default → active (buttons enabled).
  - After Confirm → locked (buttons disabled, show Edit).
  - Edit → unlocks with confirm dialog.
- **Toolbar / Menu:**
  - New Tournament (clears DB with confirm).
  - Export Results → CSV/XLSX.
  - Exit.

---

## 📐 UI Layout (Wireframe)

```
+-------------------------------------------------------------+
| OpenTournament                                   [File] [?] |
+-------------------------------------------------------------+
| [ Add Player ] [ Reset Tournament ] [ Export Results ]      |
+-------------------------------------------------------------+
| Players (ranked)          | Matches (round robin)           |
|----------------------------+--------------------------------|
| 1. Alice    (2 pts)        | Round 1: Alice vs Bob   [Win]  |
| 2. Bob      (1 pt)         | Round 1: Carol vs Dave  [Win]  |
| 3. Carol    (0 pts)        | Round 2: Alice vs Carol [Win]  |
| 4. Dave     (0 pts)        | Round 2: Bob vs Dave    [Win]  |
|                            |                                |
+-------------------------------------------------------------+
| Status: Ready                                               |
+-------------------------------------------------------------+
```

## 6. Export Logic

- Use `QTextStream` for CSV.
- Use `QtXlsx` (or a simple CSV import into Excel) for XLSX.

Example CSV snippet:

```cpp
QFile file("results.csv");
if (file.open(QIODevice::WriteOnly)) {
    QTextStream out(&file);
    out << "Player,Points,Wins,Losses,Draws\n";
    // loop players and compute stats
}
```

---

## 7. Packaging

- Use **CMake install rules**:
  ```cmake
  install(TARGETS OpenTournament DESTINATION bin)
  install(FILES tournament.db DESTINATION share/OpenTournament)
  ```
- On Windows: build `.exe` via CMake/MSVC or MinGW.
- Optionally use `windeployqt` to bundle required Qt DLLs.

---

## 8. Testing Checklist

- [ ] Add players and verify uniqueness.
- [ ] Generate pairings for even/odd counts.
- [ ] Submit results, confirm locks.
- [ ] Unlock/edit and verify update.
- [ ] Export CSV and verify correctness.
- [ ] Reset tournament clears DB.

---

## 9. Future Extensions

- Swiss system pairing.
- Elo rating updates.
- Online sync via WebSockets.
- Native window style with QML/QtQuick.

---

**End of Instructions.**
