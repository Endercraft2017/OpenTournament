# OpenTournament Implementation Plan

This document outlines the implementation plan for addressing the user's requests for the OpenTournament application.

## Current State Analysis

Based on the code review, the application already has many of the requested features implemented:

1. Player management (add, edit, delete)
2. Tournament management (start, end)
3. Round-robin pairing system
4. Swiss pairing system
5. Match result submission
6. Match locking/unlocking
7. Leaderboard with points calculation
8. CSV export functionality
9. Settings dialog with pairing system selection

However, there are still some features that need to be implemented or improved.

## Required Implementation Tasks

### 1. Database Fixes

**Issue**: "No query Unable to fetch row" and "Parameter count mismatch" errors
**Solution**:

- Review all database queries to ensure proper parameter binding
- Add error handling and logging for database operations
- Verify that all database operations return proper error codes
- Check that all queries have the correct number of parameters

### 2. Match ID Generation

**Issue**: Match IDs should start from 1 and be tournament-scoped
**Solution**:

- Modify the database schema to reset match IDs for each tournament
- Implement a function to get the next match ID for a specific tournament
- Update all match creation functions to use tournament-scoped IDs

### 3. Generate Pairings Button

**Issue**: Missing button to manually generate pairings
**Solution**:

- Add a "Generate Pairings" button to the toolbar
- Connect the button to a function that generates pairings based on the current pairing system setting
- Disable the button when no players are available or when tournament is not active

### 4. Tiebreaker Configuration

**Issue**: Missing tiebreaker choices in preferences
**Solution**:

- Add tiebreaker selection options to the settings dialog
- Implement support for both single tiebreaker and series with priority
- Add tooltips with definitions for each tiebreaker
- Save user preferences to settings

### 5. Tab Navigation Fix

**Issue**: Tab navigation after match submission
**Solution**:

- Store the current tab index before updating tabs
- Restore the tab index after updating tabs
- Ensure the tab restoration works correctly across all update operations

### 6. Tournament Start/End Functionality

**Issue**: Start/end tournament button functionality
**Solution**:

- Implement proper tournament start functionality:
  - Generate initial pairings
  - Update UI to disable player addition/removal
  - Enable tournament end button
- Implement proper tournament end functionality:
  - Calculate final standings
  - Save tournament to permanent database
  - Offer to export results
  - Reset UI for new tournament

### 7. Manual Rounds Generation

**Issue**: Rounds generation should be manual with button
**Solution**:

- Remove automatic round generation
- Add a button to manually generate rounds
- Implement functionality to generate rounds on demand

### 8. Tiebreaker Computation Button

**Issue**: Missing button to compute tiebreakers
**Solution**:

- Add a button that appears only when there are ties in the standings
- Implement confirmation dialog before computing tiebreakers
- Update leaderboard with tiebreaker-adjusted rankings

### 9. Dual Database System

**Issue**: Need current vs. permanent database system
**Solution**:

- Implement two database handlers:
  - Current tournament database (in-memory or temporary file)
  - Permanent database (persistent storage)
- Add functionality to save completed tournaments to permanent database
- Implement tournament history viewer

### 10. Tournament History Viewer

**Issue**: Missing tournament history viewer
**Solution**:

- Create a dialog to view past tournament records
- Implement functionality to view who went against whom
- Show winners, scores, and other details
- Make the history viewer non-editable

### 11. Tournament Record Deletion

**Issue**: Missing option to delete tournament records
**Solution**:

- Add functionality to remove tournament records from permanent database
- Implement confirmation dialogs for deletion
- Add backup mechanism before deletion

## Implementation Priority

1. Database fixes (highest priority - blocking other features)
2. Match ID generation and tournament-scoped IDs
3. Generate pairings button
4. Tournament start/end functionality
5. Tiebreaker configuration and computation
6. Tab navigation fix
7. Manual rounds generation
8. Dual database system
9. Tournament history viewer
10. Tournament record deletion

## Technical Considerations

### Database Schema Changes

- Modify matches table to include proper tournament scoping
- Add fields for tiebreaker preferences
- Implement proper indexing for performance

### UI/UX Improvements

- Add tooltips with definitions for tiebreakers
- Implement priority ordering for tiebreaker series
- Improve error messages and user feedback
- Add confirmation dialogs for destructive actions

### Code Organization

- Separate concerns between current tournament and permanent storage
- Implement proper error handling throughout the application
- Add logging for debugging purposes
- Ensure thread safety for database operations

## Testing Plan

1. Unit tests for database operations
2. Integration tests for pairing algorithms
3. UI tests for all new functionality
4. Performance tests for large tournaments
5. Regression tests for existing functionality

## Deployment Considerations

1. Database migration strategy for existing users
2. Backward compatibility with older tournament files
3. Documentation updates for new features
4. User training materials for new functionality
