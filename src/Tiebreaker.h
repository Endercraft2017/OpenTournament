#ifndef TIEBREAKER_H
#define TIEBREAKER_H

#include <QString>
#include <QList>
#include <QMap>
#include "Player.h"
#include "Match.h"

class Tiebreaker
{
public:
    // Tiebreaker types
    enum Type
    {
        BuchholzCutOne,
        SonnebornBerger,
        DirectEncounter,
        NumberOfWins,
        CumulativeOpponentScore
    };

    // Constructor
    Tiebreaker();

    // Calculate tiebreaker scores
    static double calculateBuchholzCutOne(const Player &player, const QList<Player> &players, const QList<Match> &matches);
    static double calculateSonnebornBerger(const Player &player, const QList<Player> &players, const QList<Match> &matches);
    static double calculateDirectEncounter(const Player &player1, const Player &player2, const QList<Match> &matches);
    static int calculateNumberOfWins(const Player &player, const QList<Match> &matches);
    static double calculateCumulativeOpponentScore(const Player &player, const QList<Player> &players, const QList<Match> &matches);

    // Get tiebreaker name
    static QString getTiebreakerName(Type type);

    // Get tiebreaker description
    static QString getTiebreakerDescription(Type type);

private:
    // Helper methods
    static double getPlayerScore(const Player &player, const QList<Match> &matches);
    static QList<Player> getOpponents(const Player &player, const QList<Player> &players, const QList<Match> &matches);
    static double getOpponentScore(const Player &opponent, const QList<Match> &matches);
};

#endif // TIEBREAKER_H