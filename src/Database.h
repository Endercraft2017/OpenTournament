#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QList>
#include <QString>
#include "Player.h"
#include "Match.h"
#include "Tournament.h"

class Database
{
public:
    // Constructor and destructor
    Database();
    ~Database();

    // Connection management
    bool connect();
    void disconnect();
    bool isConnected() const;

    // Player operations
    int addPlayer(const QString &name, int seed = 0);
    QList<Player> getAllPlayers();
    Player getPlayerById(int id);
    bool updatePlayer(const Player &player);
    bool deletePlayer(int id);

    // Match operations
    int addMatch(int round, int p1, int p2);
    QList<Match> getMatchesForRound(int round);
    QList<Match> getAllMatches();
    bool updateMatchResult(int matchId, const QString &result);
    bool lockMatch(int matchId);
    bool unlockMatch(int matchId);
    bool deleteMatch(int id);

    // Tournament operations
    int addTournament(const QString &name);
    QList<Tournament> getAllTournaments();
    Tournament getTournamentById(int id);
    bool updateTournament(const Tournament &tournament);
    bool deleteTournament(int id);

    // Utility methods
    bool initializeSchema();
    bool resetDatabase();

private:
    QSqlDatabase db;
    QString dbPath;
};

#endif // DATABASE_H