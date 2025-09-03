# Testing Specification for OpenTournament

## Overview

This document specifies the testing procedures for the OpenTournament application based on the provided checklist. It covers functional testing scenarios to ensure all features work correctly.

## Testing Checklist Coverage

### 1. Add players and verify uniqueness

- **Test Case**: Add player with unique name
- **Expected Result**: Player is added successfully
- **Test Case**: Add player with existing name
- **Expected Result**: Error message displayed, player not added

### 2. Generate pairings for even/odd counts

- **Test Case**: Generate pairings for 4 players (even count)
- **Expected Result**: 3 rounds with 2 matches each
- **Test Case**: Generate pairings for 5 players (odd count)
- **Expected Result**: 5 rounds with 2 matches each, one player has BYE each round

### 3. Submit results, confirm locks

- **Test Case**: Submit result for unplayed match
- **Expected Result**: Result is saved, match is locked, UI updates
- **Test Case**: Attempt to change result for locked match
- **Expected Result**: Edit button appears, result cannot be changed without unlock

### 4. Unlock/edit and verify update

- **Test Case**: Unlock locked match
- **Expected Result**: Confirmation dialog appears, match becomes editable
- **Test Case**: Change result of unlocked match
- **Expected Result**: New result is saved, match is locked again

### 5. Export CSV and verify correctness

- **Test Case**: Export results to CSV
- **Expected Result**: CSV file is created with correct data format
- **Test Case**: Verify CSV content
- **Expected Result**: Data matches leaderboard, proper formatting

### 6. Reset tournament clears DB

- **Test Case**: Reset tournament with data
- **Expected Result**: All matches cleared, players remain, leaderboard resets

## Detailed Test Cases

### Player Management Tests

#### TC-001: Add Unique Player

- **Preconditions**: Application is running, no player with name "Alice" exists
- **Steps**:
  1. Click "Add Player" button
  2. Enter "Alice" in name field
  3. Click "OK"
- **Expected Results**:
  - Player "Alice" appears in player list
  - No error messages
  - Leaderboard updates

#### TC-02: Add Duplicate Player

- **Preconditions**: Application is running, player "Alice" already exists
- **Steps**:
  1. Click "Add Player" button
  2. Enter "Alice" in name field
  3. Click "OK"
- **Expected Results**:
  - Error message: "Player name must be unique"
  - Dialog remains open
  - Player list unchanged

#### TC-003: Add Player with Empty Name

- **Preconditions**: Application is running
- **Steps**:
  1. Click "Add Player" button
  2. Leave name field empty
  3. Click "OK"
- **Expected Results**:
  - Error message: "Player name cannot be empty"
  - Dialog remains open
  - Player list unchanged

### Pairing Algorithm Tests

#### TC-004: Generate Pairings for Even Players

- **Preconditions**: 4 players exist (Alice, Bob, Carol, Dave)
- **Steps**:
  1. Generate pairings
- **Expected Results**:
  - 3 rounds created
  - Round 1: Alice vs Dave, Bob vs Carol
  - Round 2: Alice vs Carol, Dave vs Bob
  - Round 3: Alice vs Bob, Carol vs Dave

#### TC-005: Generate Pairings for Odd Players

- **Preconditions**: 5 players exist (Alice, Bob, Carol, Dave, Eve)
- **Steps**:
  1. Generate pairings
- **Expected Results**:
  - 5 rounds created
  - Each round has one BYE
  - Each player has a BYE exactly once
  - Each player plays every other player exactly once

### Match Result Tests

#### TC-006: Submit Player 1 Win

- **Preconditions**: Match between Alice and Bob exists, unplayed
- **Steps**:
  1. Click "Alice Wins" button

2.  Confirm submission

- **Expected Results**:
  - Match result shows "Alice Wins"
  - Match is locked (buttons disabled)
  - Edit button appears
  - Leaderboard updates (Alice: 1 point, Bob: 0 points)

#### TC-007: Submit Draw

- **Preconditions**: Match between Alice and Bob exists, unplayed
- **Steps**:
  1. Click "Draw" button
  2. Confirm submission
- **Expected Results**:
  - Match result shows "Draw"
  - Match is locked (buttons disabled)
  - Edit button appears
- Leaderboard updates (Alice: 0.5 points, Bob: 0.5 points)

#### TC-008: Submit Player 2 Win

- **Preconditions**: Match between Alice and Bob exists, unplayed
- **Steps**:
  1. Click "Bob Wins" button
  2. Confirm submission
- **Expected Results**:
  - Match result shows "Bob Wins"
  - Match is locked (buttons disabled)
  - Edit button appears
  - Leaderboard updates (Alice: 0 points, Bob: 1 point)

#### TC-009: Edit Locked Match

- **Preconditions**: Match between Alice and Bob is locked with result "Alice Wins"
- **Steps**:
  1. Click "Edit" button
  2. Confirm unlock
  3. Click "Draw" button
  4. Confirm submission
- **Expected Results**:
  - Match unlocks after step 2
  - Match result changes to "Draw" after step 4
  - Match locks again
  - Leaderboard updates (Alice: 0.5 points, Bob: 0.5 points)

### Export Tests

#### TC-010: Export to CSV

- **Preconditions**: Tournament with players and results exists
- **Steps**:
  1. Click "Export Results" button
  2. Choose file location and name
  3. Save file
- **Expected Results**:
  - CSV file is created
  - File contains header row
  - File contains player data rows
  - Data matches leaderboard

#### TC-011: Verify CSV Content

- **Preconditions**: CSV file exported in TC-010
- **Steps**:
  1. Open CSV file in text editor
  2. Verify format
- **Expected Results**:
  - First line: "Player,Points,Wins,Losses,Draws"
  - Subsequent lines: Player data in correct format
  - Numerical values are properly formatted
  - No special characters cause parsing issues

### Reset Tests

#### TC-012: Reset Tournament

- **Preconditions**: Tournament with players and matches exists
- **Steps**:
  1. Click "Reset Tournament" button
  2. Confirm reset
- **Expected Results**:
  - Confirmation dialog appears
  - All matches cleared from database
  - Match tables empty
  - Leaderboard shows all players with 0 points
  - Players remain in player list

## Automated Testing

### Unit Tests

- **Player Class**: Test constructors, getters, setters, operators
- **Match Class**: Test constructors, getters, setters, result logic
- **Tournament Class**: Test constructors, getters, setters
- **Pairing Algorithm**: Test round-robin logic with various player counts
- **Database Operations**: Test CRUD operations for all entities

### Integration Tests

- **UI to Data Models**: Test that UI actions correctly update data models
- **Data Models to Database**: Test that data model changes persist to database
- **Database to UI**: Test that database changes update the UI

### System Tests

- **End-to-End Workflows**: Test complete user journeys
- **Error Handling**: Test how the application handles various error conditions
- **Performance**: Test application performance with large datasets

## Manual Testing

### UI Testing

- **Layout**: Verify UI layout matches design specifications
- **Responsiveness**: Test UI responsiveness during operations
- **Accessibility**: Test keyboard navigation and screen reader compatibility

### User Experience Testing

- **Workflow**: Test that user workflows are intuitive
- **Error Messages**: Verify that error messages are clear and helpful
- **Performance**: Test that the application feels responsive to users

## Test Environment

### Platforms

- **Windows**: Test on Windows 10/11
- **Linux**: Test on Ubuntu 20.04+
- **macOS**: Test on macOS 10.15+

### Configurations

- **Debug Build**: For development testing
- **Release Build**: For final testing

### Tools

- **Manual Testing**: Direct application usage
- **Automated Testing**: Unit test framework (if available in Qt)
- **UI Testing**: Manual verification

## Test Data

### Sample Players

- Alice
- Bob
- Carol
- Dave
- Eve
- Frank
- Grace
- Henry

### Sample Tournaments

- Small tournament: 4 players
- Medium tournament: 8 players
- Large tournament: 16 players

## Reporting

### Test Results

- **Pass**: Functionality works as expected
- **Fail**: Functionality does not work as expected
- **Blocked**: Unable to test due to dependency issues

### Defect Tracking

- **Severity**: Critical, High, Medium, Low
- **Priority**: Immediate, High, Medium, Low
- **Status**: Open, In Progress, Resolved, Closed

## Regression Testing

After each fix or enhancement:

1. Re-run affected test cases
2. Re-run core functionality tests
3. Perform sanity check of overall application
