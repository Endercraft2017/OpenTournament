# Match Result Submission and Locking Mechanism Specification

## Overview

This document specifies the implementation of the match result submission and locking mechanism for the OpenTournament application. This feature allows users to submit match results and lock them to prevent accidental changes.

## Match States

### 1. Unplayed

- **Result**: NULL in database
- **Locked**: 0 (false)
- **UI**: Result buttons enabled, Edit button hidden
- **Background**: Default color

### 2. Played (Unlocked)

- **Result**: "p1", "p2", or "draw" in database
- **Locked**: 0 (false)
- **UI**: Result buttons enabled, Edit button hidden
- **Background**: Light yellow

### 3. Played (Locked)

- **Result**: "p1", "p2", or "draw" in database
- **Locked**: 1 (true)
- **UI**: Result buttons disabled, Edit button visible
- **Background**: Light green

## Workflow

### Submitting a Result

1. User clicks a result button (Player 1 Win, Draw, Player 2 Win)
2. Confirmation dialog appears: "Confirm result submission?"
3. User confirms by clicking "Yes"
4. Application:
   - Updates match result in database
   - Sets match state to Played (Unlocked)
   - Updates UI to reflect new state
   - Triggers leaderboard update

### Locking a Match

1. When a match is in Played (Unlocked) state
2. User navigates away from the match or after a timeout
3. Application automatically locks the match:
   - Sets locked flag to 1 in database
   - Updates UI to reflect Locked state
   - Disables result buttons
   - Shows Edit button

### Editing a Locked Match

1. User clicks Edit button on a locked match
2. Confirmation dialog appears: "Unlock this match for editing?"
3. User confirms by clicking "Yes"
4. Application:
   - Sets locked flag to 0 in database
   - Updates UI to reflect Unlocked state
   - Enables result buttons
   - Hides Edit button
5. User can now change the result
6. After changing result, match automatically locks again

## Database Operations

### Update Match Result

```sql
UPDATE matches
SET result = ?, locked = 0
WHERE id = ?
```

### Lock Match

```sql
UPDATE matches
SET locked = 1
WHERE id = ? AND result IS NOT NULL
```

### Unlock Match

```sql
UPDATE matches
SET locked = 0
WHERE id = ?
```

## UI Implementation

### Match Table Cell Structure

Each match row in the table will have the following structure:

```
+----+------------+------------+------------------+------------------+
| ID | Player 1   | Player 2   | Result           | Actions          |
+----+------------+------------------+------------------+
| 1  | Alice      | Bob        | Not Played       | [P1] [D] [P2]    |
| 2 | Carol      | Dave       | Alice Wins       | [P1] [D] [P2]    |
| 3  | Eve        | Frank      | Draw             | [Edit]           |
+----+------------+------------+------------------+------------------+
```

### Button States

- **Result Buttons** (P1 Win, Draw, P2 Win):

  - Enabled when match is Unplayed or Played (Unlocked)
  - Disabled when match is Played (Locked)

- **Edit Button**:
  - Hidden when match is Unplayed or Played (Unlocked)
  - Visible when match is Played (Locked)

### Color Coding

- **Unplayed**: Default background
- **Played (Unlocked)**: Light yellow background
- **Played (Locked)**: Light green background

## Implementation Details

### MatchManager Class

```cpp
class MatchManager {
public:
    // Submit a match result
    bool submitResult(int matchId, const QString& result);

    // Lock a match
    bool lockMatch(int matchId);

    // Unlock a match
    bool unlockMatch(int matchId);

    // Check if a match is locked
    bool isLocked(int matchId);

    // Get match result
    QString getResult(int matchId);

    // Update match table UI
    void updateMatchTable();

private:
    Database* database;
};
```

### MainWindow Integration

```cpp
// In MainWindow class
private slots:
    void onPlayer1WinClicked();
    void onDrawClicked();
    void onPlayer2WinClicked();
    void onEditMatchClicked();

private:
    void confirmResultSubmission(int matchId, const QString& result);
    void confirmMatchUnlock(int matchId);
    void updateMatchRow(int matchId);
```

### Event Flow

#### Result Submission Flow

1. User clicks result button
2. `onPlayer1WinClicked()` / `onDrawClicked()` / `onPlayer2WinClicked()` slot triggered
3. `confirmResultSubmission()` called with match ID and result
4. Confirmation dialog shown
5. If user confirms:
   - `MatchManager::submitResult()` called
   - Database updated
   - `updateMatchRow()` called to update UI
   - Leaderboard updated

#### Match Unlocking Flow

1. User clicks Edit button
2. `onEditMatchClicked()` slot triggered
3. `confirmMatchUnlock()` called with match ID
4. Confirmation dialog shown
5. If user confirms:
   - `MatchManager::unlockMatch()` called
   - Database updated
   - `updateMatchRow()` called to update UI

## Error Handling

### Database Errors

- Log error messages
- Show user-friendly error dialog
- Maintain UI consistency

### Validation

- Ensure match ID is valid
- Ensure result is one of "p1", "p2", "draw"
- Check match state before operations

### Edge Cases

- Attempting to submit result for locked match
- Attempting to unlock unplayed match
- Database connection failures

## Testing Scenarios

### Result Submission

1. Submit result for unplayed match
2. Submit result for unlocked match
3. Attempt to submit result for locked match (should be prevented)

### Match Locking

1. Verify automatic locking after result submission
2. Manual locking of matches
3. Verify locked matches cannot be modified

### Match Unlocking

1. Unlock locked match via Edit button
2. Modify result of unlocked match
3. Verify match locks again after modification

### UI Updates

1. Verify color coding changes with match states
2. Verify button states change with match states
3. Verify result display updates correctly
