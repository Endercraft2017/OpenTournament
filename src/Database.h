#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QList>
#include <QString>
#include "Player.h"
#include "Match.h"
#include "Tournament.h"

// Structure to hold tournament results data
struct TournamentResult
{
    int playerId;
    QString playerName;
    int finalRank;
    double points;
    int wins;
    int losses;
    int draws;
    double buchholzCutOne;
    double sonnebornBerger;
    int numberOfWins;
    double cumulativeOpponentScore;
};

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
    int addMatch(int tournamentId, int round, int p1, int p2);
    QList<Match> getMatchesForRound(int tournamentId, int round);
    QList<Match> getAllMatches(int tournamentId = -1); // -1 for all matches
    bool updateMatchResult(int matchId, const QString &result);
    bool lockMatch(int matchId);
    bool unlockMatch(int matchId);
    bool deleteMatch(int id);

    // Tournament match operations
    QList<Match> getMatchesForTournament(int tournamentId);
    bool deleteMatchesForTournament(int tournamentId);
    bool resetMatchIdSequence(int tournamentId);

    // Tournament operations
    int addTournament(const QString &name);
    QList<Tournament> getAllTournaments();
    Tournament getTournamentById(int id);
    bool updateTournament(const Tournament &tournament);
    bool deleteTournament(int id);
    bool startTournament(int id);
    bool completeTournament(int id);

    // Tournament results operations
    bool saveTournamentResults(int tournamentId, const QList<TournamentResult> &results);
    QList<TournamentResult> getTournamentResults(int tournamentId);

    // Utility methods
    bool initializeSchema();
    bool resetDatabase();

private:
    QSqlDatabase db;
    QString dbPath;
};

#endif // DATABASE_H