#include "Match.h"

Match::Match() : id(0), tournamentId(0), round(0), p1(0), p2(0), result(""), locked(false) {}

Match::Match(int id, int tournamentId, int round, int p1, int p2, const QString &result, bool locked)
    : id(id), tournamentId(tournamentId), round(round), p1(p1), p2(p2), result(result), locked(locked) {}

// Getters
int Match::getId() const
{
    return id;
}

int Match::getTournamentId() const
{
    return tournamentId;
}

int Match::getRound() const
{
    return round;
}

int Match::getPlayer1Id() const
{
    return p1;
}

int Match::getPlayer2Id() const
{
    return p2;
}

QString Match::getResult() const
{
    return result;
}

bool Match::isLocked() const
{
    return locked;
}

// Setters
void Match::setId(int id)
{
    this->id = id;
}

void Match::setTournamentId(int tournamentId)
{
    this->tournamentId = tournamentId;
}

void Match::setRound(int round)
{
    this->round = round;
}

void Match::setPlayer1Id(int p1)
{
    this->p1 = p1;
}

void Match::setPlayer2Id(int p2)
{
    this->p2 = p2;
}

void Match::setResult(const QString &result)
{
    this->result = result;
}

void Match::setLocked(bool locked)
{
    this->locked = locked;
}

// Helper methods
bool Match::isPlayed() const
{
    return !result.isEmpty();
}

bool Match::isDraw() const
{
    return result == "draw";
}

int Match::getWinnerId() const
{
    if (!isPlayed() || isDraw())
    {
        return -1; // -1 for unplayed or draw
    }

    if (result == "p1")
    {
        return p1;
    }
    else if (result == "p2")
    {
        return p2;
    }

    return -1; // Should not happen
}