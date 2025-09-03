#include "Tiebreaker.h"
#include <algorithm>
#include <vector>

Tiebreaker::Tiebreaker()
{
}

double Tiebreaker::calculateBuchholzCutOne(const Player &player, const QList<Player> &players, const QList<Match> &matches)
{
    // Get opponents
    QList<Player> opponents = getOpponents(player, players, matches);

    // Calculate scores of all opponents
    QList<double> opponentScores;
    for (const Player &opponent : opponents)
    {
        opponentScores.append(getPlayerScore(opponent, matches));
    }

    // Sort scores in descending order
    std::sort(opponentScores.begin(), opponentScores.end(), std::greater<double>());

    // Sum all scores except the lowest one
    double sum = 0.0;
    for (int i = 0; i < opponentScores.size() - 1; i++)
    {
        sum += opponentScores[i];
    }

    return sum;
}

double Tiebreaker::calculateSonnebornBerger(const Player &player, const QList<Player> &players, const QList<Match> &matches)
{
    double score = 0.0;

    // For each match the player has played
    for (const Match &match : matches)
    {
        if (match.isPlayed())
        {
            // Check if player was player 1
            if (match.getPlayer1Id() == player.getId())
            {
                // Get opponent (player 2)
                Player opponent = Player(); // Default player
                for (const Player &p : players)
                {
                    if (p.getId() == match.getPlayer2Id())
                    {
                        opponent = p;
                        break;
                    }
                }

                // Add opponent's score if player won or drew
                if (match.getResult() == "p1")
                {
                    score += getPlayerScore(opponent, matches);
                }
                else if (match.getResult() == "draw")
                {
                    score += getPlayerScore(opponent, matches) / 2.0;
                }
            }
            // Check if player was player 2
            else if (match.getPlayer2Id() == player.getId())
            {
                // Get opponent (player 1)
                Player opponent = Player(); // Default player
                for (const Player &p : players)
                {
                    if (p.getId() == match.getPlayer1Id())
                    {
                        opponent = p;
                        break;
                    }
                }

                // Add opponent's score if player won or drew
                if (match.getResult() == "p2")
                {
                    score += getPlayerScore(opponent, matches);
                }
                else if (match.getResult() == "draw")
                {
                    score += getPlayerScore(opponent, matches) / 2.0;
                }
            }
        }
    }

    return score;
}

double Tiebreaker::calculateDirectEncounter(const Player &player1, const Player &player2, const QList<Match> &matches)
{
    // Check if these two players have played against each other
    for (const Match &match : matches)
    {
        if (match.isPlayed())
        {
            if ((match.getPlayer1Id() == player1.getId() && match.getPlayer2Id() == player2.getId()) ||
                (match.getPlayer1Id() == player2.getId() && match.getPlayer2Id() == player1.getId()))
            {
                // They have played against each other
                if (match.getResult() == "p1")
                {
                    return (match.getPlayer1Id() == player1.getId()) ? 1.0 : 0.0;
                }
                else if (match.getResult() == "p2")
                {
                    return (match.getPlayer2Id() == player1.getId()) ? 1.0 : 0.0;
                }
                else if (match.getResult() == "draw")
                {
                    return 0.5;
                }
            }
        }
    }

    // They haven't played against each other
    return 0.0;
}

int Tiebreaker::calculateNumberOfWins(const Player &player, const QList<Match> &matches)
{
    int wins = 0;

    for (const Match &match : matches)
    {
        if (match.isPlayed())
        {
            if (match.getPlayer1Id() == player.getId() && match.getResult() == "p1")
            {
                wins++;
            }
            else if (match.getPlayer2Id() == player.getId() && match.getResult() == "p2")
            {
                wins++;
            }
        }
    }

    return wins;
}

double Tiebreaker::calculateCumulativeOpponentScore(const Player &player, const QList<Player> &players, const QList<Match> &matches)
{
    double score = 0.0;

    // Get opponents
    QList<Player> opponents = getOpponents(player, players, matches);

    // Sum scores of all opponents
    for (const Player &opponent : opponents)
    {
        score += getPlayerScore(opponent, matches);
    }

    return score;
}

QString Tiebreaker::getTiebreakerName(Type type)
{
    switch (type)
    {
    case BuchholzCutOne:
        return "Buchholz Cut One";
    case SonnebornBerger:
        return "Sonneborn–Berger Score";
    case DirectEncounter:
        return "Direct Encounter";
    case NumberOfWins:
        return "Number of Wins";
    case CumulativeOpponentScore:
        return "Cumulative Opponent's Score";
    default:
        return "Unknown";
    }
}

QString Tiebreaker::getTiebreakerDescription(Type type)
{
    switch (type)
    {
    case BuchholzCutOne:
        return "This system adds up the scores of all your opponents and then subtracts the score of the lowest-scoring opponent. It rewards players who have faced tougher opponents.";
    case SonnebornBerger:
        return "This method gives more credit to a player for wins or draws against strong opponents.";
    case DirectEncounter:
        return "If tied players have already played each other, the results of that game(s) can break the tie, with the winner being ranked higher.";
    case NumberOfWins:
        return "The player with the higher total number of wins, or wins with the black pieces, may be ranked higher.";
    case CumulativeOpponentScore:
        return "Sums up the total scores of all opponents a player has faced.";
    default:
        return "Unknown tiebreaker";
    }
}

double Tiebreaker::getPlayerScore(const Player &player, const QList<Match> &matches)
{
    double score = 0.0;

    for (const Match &match : matches)
    {
        if (match.isPlayed())
        {
            if (match.getPlayer1Id() == player.getId())
            {
                if (match.getResult() == "p1")
                {
                    score += 1.0;
                }
                else if (match.getResult() == "draw")
                {
                    score += 0.5;
                }
            }
            else if (match.getPlayer2Id() == player.getId())
            {
                if (match.getResult() == "p2")
                {
                    score += 1.0;
                }
                else if (match.getResult() == "draw")
                {
                    score += 0.5;
                }
            }
        }
    }

    return score;
}

QList<Player> Tiebreaker::getOpponents(const Player &player, const QList<Player> &players, const QList<Match> &matches)
{
    QList<Player> opponents;

    for (const Match &match : matches)
    {
        if (match.isPlayed())
        {
            if (match.getPlayer1Id() == player.getId())
            {
                // Player was player 1, so opponent is player 2
                for (const Player &p : players)
                {
                    if (p.getId() == match.getPlayer2Id())
                    {
                        opponents.append(p);
                        break;
                    }
                }
            }
            else if (match.getPlayer2Id() == player.getId())
            {
                // Player was player 2, so opponent is player 1
                for (const Player &p : players)
                {
                    if (p.getId() == match.getPlayer1Id())
                    {
                        opponents.append(p);
                        break;
                    }
                }
            }
        }
    }

    return opponents;
}

double Tiebreaker::getOpponentScore(const Player &opponent, const QList<Match> &matches)
{
    return getPlayerScore(opponent, matches);
}