# UI Specification for OpenTournament

## Overview

This document specifies the user interface design for the OpenTournament application using Qt Widgets. The UI will consist of a main window with multiple panels for player management, match display, and leaderboard.

## Main Window Layout

### Overall Structure

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

### Main Window Components

1. **Menu Bar**

   - File menu: New Tournament, Export Results, Exit
   - Help menu: About

2. **Toolbar**

   - Add Player button
   - Reset Tournament button
   - Export Results button

3. **Main Content Area**

   - Left panel: Player list and leaderboard
   - Right panel: Tabbed match display

4. **Status Bar**
   - Application status messages

## UI Components

### 1. Player Management Panel

#### Player List Widget

- **Type**: QTableWidget
- **Columns**:
  - Rank (integer)
  - Player Name (string)
  - Points (integer)
- **Features**:
  - Sorted by points (descending)
  - Context menu for player actions (edit, remove)
  - Double-click to edit player name

#### Add Player Button

- **Type**: QPushButton
- **Functionality**:
  - Opens dialog to enter player name
  - Validates unique player names
  - Adds player to database and UI

### 2. Match Management Panel

#### Round Tabs

- **Type**: QTabWidget
- **Tabs**: One for each round
- **Content**: Matches for that round in a table

#### Match Table

- **Type**: QTableWidget
- **Columns**:
  - Match ID (hidden)
  - Player 1 Name (string)
  - Player 2 Name (string)
- Result (string: "Not Played", "Player 1 Wins", "Player 2 Wins", "Draw")
- Actions (buttons: Player 1 Win, Draw, Player 2 Win, Edit)
- **Features**:
  - Buttons disabled when match is locked
  - Edit button appears when match is locked
  - Color coding for match states

### 3. Match Action Buttons

#### Result Buttons

- **Player 1 Win Button**: QPushButton

  - Sets match result to "p1"
  - Enabled when match is unlocked
  - Triggers confirmation dialog

- **Draw Button**: QPushButton

  - Sets match result to "draw"
  - Enabled when match is unlocked
  - Triggers confirmation dialog

- **Player 2 Win Button**: QPushButton
  - Sets match result to "p2"
  - Enabled when match is unlocked
  - Triggers confirmation dialog

#### Edit Button

- **Type**: QPushButton
- **Functionality**:
  - Appears when match is locked
  - Triggers confirmation dialog before unlocking
  - Changes to "Confirm" after unlocking

### 4. Toolbar Buttons

#### Add Player Button

- **Icon**: Plus sign
- **Functionality**: Opens add player dialog

#### Reset Tournament Button

- **Icon**: Refresh/Reset icon
- **Functionality**:
  - Triggers confirmation dialog
  - Clears all tournament data
  - Resets UI to initial state

#### Export Results Button

- **Icon**: Export/Save icon
- **Functionality**:
  - Exports tournament results to CSV
  - Opens file save dialog

## Dialogs

### 1. Add Player Dialog

- **Type**: QDialog
- **Components**:
  - QLabel: "Enter player name:"
  - QLineEdit: Player name input
  - QPushButton: OK
  - QPushButton: Cancel
- **Validation**:
  - Check for empty name
  - Check for unique name
  - Show error messages

### 2. Confirmation Dialogs

- **Type**: QMessageBox
- **Types**:
  - Result confirmation: "Confirm result submission?"
  - Edit confirmation: "Unlock this match for editing?"
  - Reset confirmation: "Clear all tournament data?"

### 3. Export Dialog

- **Type**: QFileDialog
- **Functionality**:
  - Choose file location and name
  - Default to .csv extension

## UI State Management

### Match States

1. **Unplayed**:

   - Result buttons enabled
   - Edit button hidden
   - Background: default

2. **Played (Unlocked)**:

   - Result buttons enabled
   - Edit button hidden
   - Background: light yellow

3. **Played (Locked)**:
   - Result buttons disabled
   - Edit button visible
   - Background: light green

### Player States

1. **Active**:

   - Visible in leaderboard
   - Can be selected for matches

2. **Removed**:
   - Not visible in leaderboard
   - Matches handled appropriately

## UI Class Design

### MainWindow Class

```cpp
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddPlayerClicked();
    void onResetTournamentClicked();
    void onExportResultsClicked();
    void onMatchResultClicked();
    void onEditMatchClicked();

private:
    // UI components
    QMenuBar *menuBar;
    QToolBar *toolBar;
    QTableWidget *playerTable;
    QTabWidget *roundTabs;
    QStatusBar *statusBar;

    // Actions
    QAction *newTournamentAction;
    QAction *exportResultsAction;
    QAction *exitAction;
    QAction *aboutAction;

    // Buttons
    QPushButton *addPlayerButton;
    QPushButton *resetTournamentButton;
    QPushButton *exportResultsButton;

    // Data managers
    Database *database;
    PlayerManager *playerManager;
    MatchManager *matchManager;
    TournamentManager *tournamentManager;

    // Helper methods
    void setupUI();
    void setupMenu();
    void setupToolbar();
    void setupPlayerPanel();
    void setupMatchPanel();
    void updatePlayerList();
    void updateMatchTabs();
    void showAddPlayerDialog();
    void showConfirmationDialog(const QString &message);
};
```

## Styling and Appearance

### Color Scheme

- **Primary**: Tournament blue (#2E86AB)
- **Secondary**: Light gray (#F2F2F2)
- **Accent**: Tournament green (#1FAB89)
- **Warning**: Tournament red (#FF6B6B)

### Fonts

- **Main**: System default, 10-12pt
- **Headers**: Bold, 12-14pt
- **Status**: Italic, 9pt

### Widget Styling

- **Buttons**: Rounded corners, consistent padding
- **Tables**: Alternating row colors, clear headers
- **Tabs**: Modern styling with clear indicators

## Responsive Design

- Main window resizable
- Columns in tables resize appropriately
- Scroll areas for long lists
- Properly sized dialogs

## Accessibility

- Keyboard navigation support
- Clear visual feedback for interactions
- Sufficient contrast for readability
- Tooltips for all interactive elements
