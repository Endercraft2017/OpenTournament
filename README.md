# OpenTournament

OpenTournament is a desktop application for managing tournament brackets with support for multiple tournaments, various pairing systems, and comprehensive result tracking.

---

## Features

### Multi-Tournament Handling

- Create and manage multiple tournaments simultaneously.
- Each tournament maintains its own player list, matches, and results.
- Switch between tournaments using the tournament selector in the toolbar.
- Edit or delete existing tournaments (except the last remaining one).

### Tournament Pairing Systems

- **Round Robin**: Every player plays against every other player.
- **Swiss System**: Players are paired based on performance in previous rounds.
- Configurable number of rounds for Swiss tournaments.

#### Swiss Pairing System Details

- First round pairings are random.
- Subsequent rounds pair players with similar scores.
- Efforts are made to avoid repeat pairings.
- Add a new round by clicking the "+" tab after all current round matches are completed.

### Comprehensive Result Tracking

- Track wins, losses, and draws for each player.
- Automatic leaderboard calculation based on points:
  - Win = 1
  - Draw = 0.5
  - Loss = 0
- Export tournament results to CSV for external analysis.

### Advanced Tiebreaker System

- Multiple tiebreaker methods:
  - Buchholz Cut One
  - Sonneborn–Berger Score
  - Number of Wins
  - Cumulative Opponent's Score
- Configurable tiebreaker preferences.
- Automatic calculation when needed.

#### Tiebreaker Method Details

- **Buchholz Cut One**: Adds scores of all opponents minus the lowest-scoring opponent.
- **Sonneborn–Berger Score**: Rewards wins or draws against stronger opponents.
- **Number of Wins**: Player with more wins ranks higher.
- **Cumulative Opponent's Score**: Total score of all opponents faced.

### Customization Options

- Multiple themes (Fusion Dark, Fusion Light, Fusion White, Abyss, Dark High Contrast).
- Two icon sets (Default or Modern).
- Configurable pairing system and tiebreaker preferences.

### Settings and Preferences

- Access via **Settings → Preferences**:
  - Choose pairing system and number of Swiss rounds.
  - Select tiebreaker methods.
  - Choose themes and icon sets.
  - Enable/disable auto-start tournament.

### About Dialog

- Access via **Help → About**.
- Provides application info and link to this README.

---

## Installation

### Prerequisites

- Windows OS
- Qt6 libraries
- MinGW compiler (for building from source)

### Building from Source

```bash
git clone https://github.com/Endercraft2017/OpenTournament.git
cd OpenTournament
build.bat
build\OpenTournament.exe


### Running Pre-built Executable
- If you have downloaded the installer from GitHub releases, simply run the installer and follow the prompts.
- After installation, launch OpenTournament from the desktop icon or Start Menu.

---

## Usage

### Creating a Tournament
1. Click "Add Tournament" in the toolbar.
2. Enter a tournament name.
3. The tournament is automatically selected.

### Managing Tournaments
- **Switching Tournaments**: Use the dropdown selector.
- **Editing Tournaments**: Right-click the tournament selector → "Edit Tournament".
- **Deleting Tournaments**: Right-click the tournament selector → "Delete Tournament" (cannot delete last tournament).

### Adding Players
1. Ensure the correct tournament is selected.
2. Click "Add Player".
3. Enter the player's name.

### Starting a Tournament
1. Add at least 3 players.
2. Click "Start Tournament".
3. Matches are generated automatically according to the chosen pairing system.

### Recording Match Results
1. Navigate to the round tab.
2. Click the result button (P1 Win, Draw, P2 Win).
3. Confirm results.
4. Completed matches are locked to prevent accidental changes.

### Adding Swiss Rounds
1. Ensure all matches in the current round have results.
2. Click the "+" tab to add a new round.
3. Pairings are automatically generated.

### Ending a Tournament
1. Click "End Tournament".
2. Confirm action.
3. Final standings are calculated with tiebreakers applied.
4. Optionally export results to CSV.

### Exporting Results
1. Click "Export Results" in the toolbar.
2. Choose a location and filename for your CSV file.
3. The exported CSV contains player names, points, wins, losses, and draws.

### Settings and Preferences
- Access through **Settings → Preferences**.
- Choose pairing system, number of Swiss rounds, tiebreaker methods.
- Select themes and icon sets.
- Enable/disable auto-start tournament when enough players are added.

---

## Data Storage
- Tournament data is stored in an SQLite database file named `tournament.db` in the application directory.
- Contains all information about players, tournaments, matches, and results.
- Ensure the database is writable; otherwise, you may encounter errors like "attempt to write a readonly database."

---

## GitHub Releases
- Pre-built installer `.exe` files will be uploaded to the [Releases](https://github.com/Endercraft2017/OpenTournament/releases) section.
- Users can download and install OpenTournament without building from source.

**Tip:** If your installer is large, consider compressing the installer using Inno Setup options or a zip archive.

---

## Screenshots & Videos (Recommended)
Prepare media to showcase your project on GitHub:

### Screenshots
- Main window with active tournament.
- Tournament creation dialog.
- Adding players interface.
- Match results interface.
- Swiss pairing round addition.
- Settings dialog (themes, tiebreakers, icons).

### Videos (Optional)
- Short demo showing tournament creation → match results → CSV export.
- Theme switching in action.

---

## License
MIT License – see [LICENSE](LICENSE) file for details.

---

## Contributing
- Contributions are welcome via Pull Requests.
- Suggest new features, report bugs, or improve documentation.

---

## Acknowledgments
- Built with **Qt6**.
- Uses **SQLite** for data storage.
```
