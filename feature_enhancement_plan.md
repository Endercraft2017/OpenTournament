# OpenTournament Feature Enhancement Plan

## Overview

This document outlines the implementation plan for enhancing the OpenTournament application with the features requested by the user. The enhancements will be implemented systematically to avoid breaking existing functionality.

## Feature Categories

### 1. Player Management Enhancements

- Add delete player functionality
- Add edit player name functionality
- Implement proper player validation

### 2. Tournament Management Improvements

- Add "Remove players" option to Reset Tournament
- Modify "New Tournament" to remove players
- Implement proper tournament ID management
- Add tournament history viewing/deletion

### 3. UI/UX Improvements

- Fix tab navigation after match result submission
- Center-align points in the Points column
- Add "Start Tournament" button
- Add settings tab for pairing system selection

### 4. Advanced Pairing Systems

- Implement Swiss pairing algorithm
- Add option to choose between Round Robin and Swiss pairing

### 5. Database Schema Updates

- Improve match/tournament ID management
- Add proper tournament tracking
- Implement late player handling

## Detailed Implementation Plan

### Phase 1: Database Schema Updates

1. Modify database schema to support multiple tournaments
2. Add proper tournament tracking with IDs
3. Implement foreign key relationships between matches and tournaments
4. Add settings/preferences table

### Phase 2: Player Management Features

1. Add delete player functionality to Database class
2. Add edit player functionality to Database class
3. Update UI to include player edit/delete buttons
4. Implement player validation and conflict resolution

### Phase 3: Tournament Management Improvements

1. Modify reset tournament functionality with options
2. Update new tournament behavior to handle players
3. Implement tournament ID management
4. Add tournament history management

### Phase 4: UI/UX Improvements

1. Fix tab navigation after match submission
2. Center-align points column
3. Add "Start Tournament" button
4. Add settings tab with pairing options

### Phase 5: Advanced Pairing Systems

1. Implement Swiss pairing algorithm
2. Add pairing system selection in settings
3. Implement late player handling system

### Phase 6: Tournament History Management

1. Add tournament selection/viewing interface
2. Implement tournament deletion functionality
3. Add tournament reset functionality

## Database Schema Changes

### Updated tournaments table

```sql
CREATE TABLE IF NOT EXISTS tournaments (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT,
    pairing_system TEXT DEFAULT 'round_robin', -- 'round_robin' or 'swiss'
    status TEXT DEFAULT 'setup', -- 'setup', 'active', 'completed'
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    started_at TIMESTAMP NULL
);
```

### Updated matches table

```sql
CREATE TABLE IF NOT EXISTS matches (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    tournament_id INTEGER NOT NULL,
    round INTEGER NOT NULL,
    p1 INTEGER NOT NULL,
    p2 INTEGER NOT NULL,
    result TEXT DEFAULT NULL, -- 'p1','p2','draw'
    locked INTEGER DEFAULT 0,
    FOREIGN KEY (tournament_id) REFERENCES tournaments(id),
    FOREIGN KEY (p1) REFERENCES players(id),
    FOREIGN KEY (p2) REFERENCES players(id)
);
```

### New settings table

```sql
CREATE TABLE IF NOT EXISTS settings (
    id INTEGER PRIMARY KEY CHECK (id = 1), -- Singleton table
    default_pairing_system TEXT DEFAULT 'round_robin',
    auto_start_tournament INTEGER DEFAULT 0 -- 0 = false, 1 = true
);
```

## Class Modifications

### Tournament Class

- Add pairing_system field
- Add status field
- Add started_at field

### Database Class

- Add methods for tournament management
- Add methods for settings management
- Update existing methods to work with tournament IDs

### MainWindow Class

- Add player edit/delete functionality
- Add tournament selection interface
- Add settings tab
- Add pairing system selection
- Implement tab navigation fixes

## UI Improvements

### Player Management Panel

- Add context menu or buttons for edit/delete
- Add confirmation dialogs for destructive actions

### Tournament Management

- Add "Start Tournament" button
- Add options to reset tournament dialog
- Add tournament history viewer

### Settings Tab

- Add pairing system selection (Round Robin/Swiss)
- Add auto-start tournament option
- Add other configurable options

## Algorithm Implementations

### Swiss Pairing Algorithm

- Implement Swiss pairing logic
- Handle bye assignments
- Manage pairing history to avoid rematches

### Late Player Handling

- Automatically regenerate pairings when players are added/removed
- Handle ongoing tournaments with new players

## Implementation Order

1. Database schema updates
2. Tournament class modifications
3. Database class updates
4. Player management features
5. Tournament management improvements
6. UI/UX fixes
7. Swiss pairing implementation
8. Settings system
9. Tournament history management
10. Late player handling
11. Testing and refinement

## Testing Plan

### Unit Tests

- Database operations with multiple tournaments
- Player management functions
- Pairing algorithms
- Tournament state transitions

### Integration Tests

- Full tournament lifecycle
- Player add/remove during tournament
- Pairing system switching
- Tournament history management

### User Acceptance Tests

- All requested features work as expected
- Existing functionality remains intact
- UI/UX improvements enhance user experience
