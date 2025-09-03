# Round-Robin Pairing Algorithm Specification for OpenTournament

## Overview

This document specifies the implementation of the round-robin pairing algorithm for the OpenTournament application. The algorithm uses the circle method to generate pairings for all rounds.

## Algorithm Description

The round-robin algorithm ensures that each player plays against every other player exactly once. The implementation uses the circle method:

1. If the number of players is odd, add a "BYE" placeholder
2. For each round:
   - Pair players from opposite ends of the circle
   - Rotate all positions except the first one
3. Continue until all rounds are generated

## Implementation

### Function Signature

```cpp
#include <vector>
#include <string>
#include <algorithm>

struct Pair {
    std::string p1;
    std::string p2;
};

std::vector<std::vector<Pair>> roundRobinPairings(std::vector<std::string> players);
```

### Detailed Implementation

```cpp
std::vector<std::vector<Pair>> roundRobinPairings(std::vector<std::string> players) {
    int n = players.size();

    // Handle odd number of players by adding a BYE
    if (n % 2 == 1) {
        players.push_back("BYE");
        n++;
    }

    std::vector<std::vector<Pair>> rounds;

    // Generate n-1 rounds
    for (int r = 0; r < n - 1; r++) {
        std::vector<Pair> round;

        // Pair players from opposite ends
        for (int i = 0; i < n/2; i++) {
            std::string p1 = players[i];
            std::string p2 = players[n-1-i];

            // Skip BYE matches
            if (p1 != "BYE" && p2 != "BYE")
                round.push_back({p1, p2});
        }

        rounds.push_back(round);

        // Rotate all positions except the first one
        std::rotate(players.begin()+1, players.end()-1, players.end());
    }

    return rounds;
}
```

## Integration with Application

### Player ID Mapping

Since the algorithm works with player names, we need to map between player IDs and names:

```cpp
// Convert player IDs to names for pairing
std::vector<QString> playerNames;
for (const Player& player : players) {
    playerNames.push_back(player.getName());
}

// Generate pairings
std::vector<std::vector<Pair>> pairings = roundRobinPairings(playerNames);

// Convert back to match objects with player IDs
std::vector<Match> matches;
int roundNumber = 1;
for (const auto& round : pairings) {
    for (const auto& pair : round) {
        // Find player IDs by name
        int p1Id = findPlayerIdByName(pair.p1);
        int p2Id = findPlayerIdByName(pair.p2);

        // Create match object
        Match match(0, roundNumber, p1Id, p2Id);
        matches.push_back(match);
    }
    roundNumber++;
}
```

## Edge Cases

### Even Number of Players

- All players are paired in each round
- Number of rounds = n - 1
- Number of matches per round = n / 2

### Odd Number of Players

- One "BYE" is added to make the count even
- Players with a BYE in a round are considered to have a bye (automatic win or no match)
- Number of rounds = n (original count)
- Number of matches per round = (n - 1) / 2

## Example Usage

### With 4 Players

Players: ["Alice", "Bob", "Carol", "Dave"]

Round 1: Alice vs Dave, Bob vs Carol
Round 2: Alice vs Carol, Dave vs Bob
Round 3: Alice vs Bob, Carol vs Dave

### With 5 Players

Players: ["Alice", "Bob", "Carol", "Dave", "Eve"]

Round 1: Alice vs Eve, Bob vs Dave (Carol has BYE)
Round 2: Alice vs Dave, Eve vs Carol (Bob has BYE)
Round 3: Alice vs Carol, Dave vs Bob (Eve has BYE)
Round 4: Alice vs Bob, Carol vs Eve (Dave has BYE)
Round 5: Alice vs Eve, Bob vs Carol (Dave has BYE)

## Performance Considerations

- Time complexity: O(n²) where n is the number of players
- Space complexity: O(n²) for storing all pairings
- Suitable for tournaments with reasonable player counts (up to hundreds)

## Testing Scenarios

1. Verify correct pairings for even player counts (4, 6, 8 players)
2. Verify correct pairings for odd player counts (3, 5, 7 players)
3. Ensure each player plays every other player exactly once
4. Verify that BYE assignments are correct for odd player counts
5. Check that the rotation algorithm works correctly
