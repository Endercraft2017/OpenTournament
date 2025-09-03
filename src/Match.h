#ifndef MATCH_H
#define MATCH_H

#include <QString>

class Match
{
public:
    // Constructors
    Match();
    Match(int id, int tournamentId, int round, int p1, int p2, const QString &result = QString(), bool locked = false);

    // Getters
    int getId() const;
    int getTournamentId() const;
    int getRound() const;
    int getPlayer1Id() const;
    int getPlayer2Id() const;
    QString getResult() const;
    bool isLocked() const;

    // Setters
    void setId(int id);
    void setTournamentId(int tournamentId);
    void setRound(int round);
    void setPlayer1Id(int p1);
    void setPlayer2Id(int p2);
    void setResult(const QString &result);
    void setLocked(bool locked);

    // Helper methods
    bool isPlayed() const;
    bool isDraw() const;
    int getWinnerId() const; // Returns ID of winner, -1 for draw or unplayed

private:
    int id;
    int tournamentId;
    int round;
    int p1;         // Player 1 ID
    int p2;         // Player 2 ID
    QString result; // "p1", "p2", "draw", or empty for unplayed
    bool locked;
};

#endif // MATCH_H