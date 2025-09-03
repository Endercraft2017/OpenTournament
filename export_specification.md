# CSV Export Functionality Specification

## Overview

This document specifies the implementation of the CSV export functionality for the OpenTournament application. This feature allows users to export tournament results to a CSV file for external use.

## Export Format

### File Format

- **Extension**: .csv
- **Encoding**: UTF-8
- **Delimiter**: Comma (,)
- **Line Ending**: CRLF (Windows) or LF (Unix)

### CSV Structure

```
Player,Points,Wins,Losses,Draws
Alice,2.5,2,0,1
Bob,1.0,1,2,0
Carol,0.5,0,2,1
Dave,0.0,0,3,0
```

### Column Definitions

1. **Player**: Player name
2. **Points**: Total points (wins + 0.5 \* draws)
3. **Wins**: Number of matches won
4. **Losses**: Number of matches lost
5. **Draws**: Number of matches drawn

## Database Queries

### Get Player Statistics

```sql
SELECT p.name,
       COALESCE(SUM(CASE
           WHEN m.result = 'p1' AND m.p1 = p.id THEN 1
           WHEN m.result = 'p2' AND m.p2 = p.id THEN 1
           WHEN m.result = 'draw' THEN 0.5
           ELSE 0
       END), 0) AS points,
       COALESCE(SUM(CASE
           WHEN m.result = 'p1' AND m.p1 = p.id THEN 1
           WHEN m.result = 'p2' AND m.p2 = p.id THEN 1
           ELSE 0
       END), 0) AS wins,
       COALESCE(SUM(CASE
           WHEN m.result = 'p1' AND m.p2 = p.id THEN 1
           WHEN m.result = 'p2' AND m.p1 = p.id THEN 1
           ELSE 0
       END), 0) AS losses,
       COALESCE(SUM(CASE
           WHEN m.result = 'draw' THEN 1
           ELSE 0
       END), 0) AS draws
FROM players p
LEFT JOIN matches m ON (p.id = m.p1 OR p.id = m.p2) AND m.result IS NOT NULL
GROUP BY p.id, p.name
ORDER BY points DESC, p.name ASC
```

## Implementation Details

### ExportManager Class

```cpp
class ExportManager {
public:
    struct PlayerStats {
        QString name;
        double points;
        int wins;
        int losses;
        int draws;
    };

    // Export tournament results to CSV
    bool exportToCSV(const QString& filename);

    // Get player statistics for export
    QList<PlayerStats> getPlayerStats();

private:
    Database* database;
    QString escapeCSVField(const QString& field);
};
```

### MainWindow Integration

```cpp
// In MainWindow class
private slots:
    void onExportResultsClicked();

private:
    void performCSVExport();
    QString getExportFileName();
```

### Helper Functions

```cpp
// Escape CSV fields to handle commas, quotes, and newlines
QString escapeCSVField(const QString& field) {
    if (field.contains(',') || field.contains('"') || field.contains('\n')) {
        QString escaped = field;
        escaped.replace("\"", "\"\"");
        return "\"" + escaped + "\"";
    }
    return field;
}
```

## Workflow

### User Interaction

1. User clicks "Export Results" button in toolbar
2. File save dialog appears with default name "tournament_results.csv"
3. User selects location and confirms filename
4. Application performs export operation
5. Success/failure message shown to user

### Export Process

1. Query database for player statistics
2. Open file for writing
3. Write CSV header
4. Write player data rows
5. Close file
6. Show success message

## UI Integration

### Toolbar Button

- **Icon**: Export/Save icon
- **Text**: "Export Results"
- **Position**: Right side of toolbar
- **State**: Enabled when tournament has data, disabled otherwise

### File Dialog

- **Type**: QFileDialog::getSaveFileName
- **Title**: "Export Tournament Results"
- **Filter**: "CSV Files (\*.csv)"
- **Default Name**: "tournament_results.csv"

### Status Messages

- **Success**: "Results exported successfully to [filename]"
- **Failure**: "Export failed: [error message]"
- **Cancel**: No message (user cancelled operation)

## Error Handling

### File Operations

- Check if file can be created/written
- Handle disk full errors
- Handle permission errors
- Handle invalid paths

### Database Errors

- Handle query failures
- Provide fallback data if possible
- Show meaningful error messages

### Edge Cases

- Empty tournament (no players)
- Tournament with players but no matches
- Invalid file paths
- File already exists (overwrite confirmation)

## Performance Considerations

### Large Datasets

- For tournaments with many players, show progress indicator
- Buffer file writes for efficiency
- Handle memory usage appropriately

### Data Validation

- Validate data before writing
- Handle NULL values appropriately
- Ensure consistent formatting

## Testing Scenarios

### Basic Export

1. Export tournament with data
2. Verify CSV file is created
3. Verify file contents match expected format
4. Verify data accuracy

### Edge Cases

1. Export empty tournament
2. Export tournament with special characters in names
3. Export tournament with commas in names
4. Export tournament with quotes in names

### Error Conditions

1. Attempt export to unwritable location
2. Simulate disk full during export
3. Handle database errors during export

### File Validation

1. Open exported CSV in spreadsheet application
2. Verify data imports correctly
3. Verify column headers are correct
4. Verify numerical data formats

## User Experience Considerations

### File Dialog

- Remember last export location
- Provide sensible default filename
- Show file size estimate if possible

### Progress Indication

- For large exports, show progress bar
- Disable UI during export
- Provide cancel option for long operations

### Feedback

- Clear success messages with file location
- Helpful error messages
- Option to open file location after export

### Accessibility

- Keyboard shortcut for export (Ctrl+E)
- Screen reader compatibility
- High contrast dialog elements
