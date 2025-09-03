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

## Development Setup

### Prerequisites

- CMake 3.16 or higher
- Qt6 development libraries
- C++ compiler with C++17 support

### VS Code Configuration

This project includes configuration files for Visual Studio Code to help with development:

- `.vscode/c_cpp_properties.json` - Configures IntelliSense for Qt headers
- `.vscode/settings.json` - Configures C++ settings
- `.vscode/tasks.json` - Defines build tasks
- `.vscode/launch.json` - Configures debugging

To use these configurations:

1. Install the C/C++ extension for VS Code
2. Install Qt6 development libraries
3. Adjust the paths in `.vscode/c_cpp_properties.json` to match your Qt installation

### Building from Source

#### Using the provided scripts

- Linux/macOS: `./build.sh`
- Windows: `build.bat`

#### Manual build steps

1. Clone the repository
2. Configure: `cmake -S . -B build`
3. Build: `cmake --build build`
4. Run: `./build/OpenTournament`

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
