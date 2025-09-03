# Tournament Reset Functionality Specification

## Overview

This document specifies the implementation of the tournament reset functionality for the OpenTournament application. This feature allows users to clear all tournament data and start fresh.

## Reset Scope

### What Gets Reset

- All matches in the database
- All players remain (they are not deleted)
- Tournament information remains
- Leaderboard is cleared (shows 0 points for all players)

### What Is Preserved

- Player list (names, IDs, seeds)
- Tournament structure
- Application settings (if any)

## Workflow

### User Interaction

1. User clicks "Reset Tournament" button in toolbar
2. Confirmation dialog appears: "Clear all tournament data?"
3. User confirms by clicking "Yes"
4. Application performs reset operation
5. UI updates to reflect reset state

### Reset Process

1. Show progress indicator (if needed)
2. Clear all matches from database
3. Reset match tables in UI
4. Reset leaderboard in UI
5. Update status bar: "Tournament reset successfully"
6. Hide progress indicator

## Database Operations

### Clear Matches

```sql
DELETE FROM matches;
```

### Reset Match IDs (optional, for clean sequencing)

```sql
DELETE FROM sqlite_sequence WHERE name = 'matches';
```

### Verification Query

```sql
SELECT COUNT(*) FROM matches;
-- Should return 0 after reset
```

## Implementation Details

### TournamentManager Class

```cpp
class TournamentManager {
public:
    // Reset tournament data
    bool resetTournament();

    // Check if tournament has data
    bool hasData();

    // Get tournament statistics
    struct TournamentStats {
        int playerCount;
        int matchCount;
        int completedMatches;
    };
    TournamentStats getStats();

private:
    Database* database;
};
```

### MainWindow Integration

```cpp
// In MainWindow class
private slots:
    void onResetTournamentClicked();

private:
    void confirmTournamentReset();
    void performTournamentReset();
    void updateUIAfterReset();
```

### Event Flow

1. User clicks Reset Tournament button
2. `onResetTournamentClicked()` slot triggered
3. `confirmTournamentReset()` called
4. Confirmation dialog shown
5. If user confirms:
   - `performTournamentReset()` called
   - Database operations executed
   - `updateUIAfterReset()` called
   - UI refreshed

## UI Updates After Reset

### Match Tables

- Clear all rows from match tables
- Reset tab labels to show rounds with no matches
- Disable all result buttons
- Hide all Edit buttons

### Leaderboard

- Update all player points to 0.0
- Maintain player order (alphabetical or by ID)
- Reset rank numbers

### Status Bar

- Show message: "Tournament reset successfully"

### Toolbar

- Reset Tournament button remains enabled
- Export Results button may be disabled if no data exists

## Error Handling

### Database Errors

- Log error messages
- Show user-friendly error dialog
- Maintain UI consistency
- Prevent partial resets

### Edge Cases

- Attempting to reset when no data exists
- Database connection failures during reset
- Insufficient permissions for database operations

## Security Considerations

### Data Loss Prevention

- Always require confirmation before reset
- Use clear, unambiguous language in confirmation dialog
- Provide option to cancel operation

### Undo Functionality

- No undo for reset operation (by design)
- Consider export functionality before reset

## Performance Considerations

### Large Datasets

- For tournaments with many matches, show progress indicator
- Perform database operations efficiently
- Update UI in batches if needed

### Transaction Management

- Use database transactions for consistency
- Rollback on error
- Commit on success

```sql
BEGIN TRANSACTION;
DELETE FROM matches;
COMMIT;
```

## Testing Scenarios

### Basic Reset

1. Perform reset on tournament with data
2. Verify all matches are deleted
3. Verify players remain
4. Verify UI updates correctly

### Empty Tournament Reset

1. Perform reset on tournament with no matches
2. Verify operation completes successfully
3. Verify no errors occur

### Error Conditions

1. Simulate database error during reset
2. Verify error is handled gracefully
3. Verify partial resets do not occur

### UI Updates

1. Verify match tables clear correctly
2. Verify leaderboard updates correctly
3. Verify status messages display correctly

## User Experience Considerations

### Confirmation Dialog

- Title: "Confirm Reset"
- Message: "This will clear all match results. Players will not be deleted. Continue?"
- Buttons: "Yes, Reset" and "Cancel"
- Default button: "Cancel"

### Progress Indication

- For large tournaments, show progress bar or spinner
- Disable UI during reset operation
- Provide cancel option for long operations (if applicable)

### Feedback

- Clear success message after reset
- Error messages if reset fails
- Status updates during operation
