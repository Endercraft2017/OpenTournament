#include "Database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>

Database::Database()
{
    // Set database path to application directory
    QString appDir = QCoreApplication::applicationDirPath();
    dbPath = appDir + "/tournament.db";
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);
}

Database::~Database()
{
    if (db.isOpen())
    {
        db.close();
    }
}

bool Database::connect()
{
    if (!db.isOpen())
    {
        if (!db.open())
        {
            qDebug() << "Failed to open database:" << db.lastError().text();
            return false;
        }
    }

    // Initialize schema if needed
    return initializeSchema();
}

void Database::disconnect()
{
    if (db.isOpen())
    {
        db.close();
    }
}

bool Database::isConnected() const
{
    return db.isOpen();
}

bool Database::initializeSchema()
{
    QSqlQuery query(db);

    // Create players table
    QString createPlayersTable = R"(
        CREATE TABLE IF NOT EXISTS players (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL UNIQUE,
            seed INTEGER DEFAULT 0
        )
    )";

    if (!query.exec(createPlayersTable))
    {
        qDebug() << "Failed to create players table:" << query.lastError().text();
        return false;
    }

    // Create matches table
    QString createMatchesTable = R"(
        CREATE TABLE IF NOT EXISTS matches (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            round INTEGER NOT NULL,
            p1 INTEGER NOT NULL,
            p2 INTEGER NOT NULL,
            result TEXT DEFAULT NULL,
            locked INTEGER DEFAULT 0,
            FOREIGN KEY (p1) REFERENCES players(id),
            FOREIGN KEY (p2) REFERENCES players(id)
        )
    )";

    if (!query.exec(createMatchesTable))
    {
        qDebug() << "Failed to create matches table:" << query.lastError().text();
        return false;
    }

    // Create tournaments table
    QString createTournamentsTable = R"(
        CREATE TABLE IF NOT EXISTS tournaments (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )
    )";

    if (!query.exec(createTournamentsTable))
    {
        qDebug() << "Failed to create tournaments table:" << query.lastError().text();
        return false;
    }

    return true;
}

// Player operations
int Database::addPlayer(const QString &name, int seed)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO players (name, seed) VALUES (?, ?)");
    query.addBindValue(name);
    query.addBindValue(seed);

    if (!query.exec())
    {
        qDebug() << "Failed to add player:" << query.lastError().text();
        return -1;
    }

    return query.lastInsertId().toInt();
}

QList<Player> Database::getAllPlayers()
{
    QList<Player> players;
    QSqlQuery query(db);
    query.prepare("SELECT id, name, seed FROM players ORDER BY name");

    if (!query.exec())
    {
        qDebug() << "Failed to get players:" << query.lastError().text();
        return players;
    }

    while (query.next())
    {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        int seed = query.value(2).toInt();
        players.append(Player(id, name, seed));
    }

    return players;
}

Player Database::getPlayerById(int id)
{
    QSqlQuery query(db);
    query.prepare("SELECT id, name, seed FROM players WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec() || !query.next())
    {
        qDebug() << "Failed to get player by ID:" << query.lastError().text();
        return Player(); // Return default player
    }

    QString name = query.value(1).toString();
    int seed = query.value(2).toInt();
    return Player(id, name, seed);
}

bool Database::updatePlayer(const Player &player)
{
    QSqlQuery query(db);
    query.prepare("UPDATE players SET name = ?, seed = ? WHERE id = ?");
    query.addBindValue(player.getName());
    query.addBindValue(player.getSeed());
    query.addBindValue(player.getId());

    if (!query.exec())
    {
        qDebug() << "Failed to update player:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool Database::deletePlayer(int id)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM players WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec())
    {
        qDebug() << "Failed to delete player:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

// Match operations
int Database::addMatch(int round, int p1, int p2)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO matches (round, p1, p2) VALUES (?, ?, ?)");
    query.addBindValue(round);
    query.addBindValue(p1);
    query.addBindValue(p2);

    if (!query.exec())
    {
        qDebug() << "Failed to add match:" << query.lastError().text();
        return -1;
    }

    return query.lastInsertId().toInt();
}

QList<Match> Database::getMatchesForRound(int round)
{
    QList<Match> matches;
    QSqlQuery query(db);
    query.prepare("SELECT id, round, p1, p2, result, locked FROM matches WHERE round = ? ORDER BY id");
    query.addBindValue(round);

    if (!query.exec())
    {
        qDebug() << "Failed to get matches for round:" << query.lastError().text();
        return matches;
    }

    while (query.next())
    {
        int id = query.value(0).toInt();
        int round = query.value(1).toInt();
        int p1 = query.value(2).toInt();
        int p2 = query.value(3).toInt();
        QString result = query.value(4).toString();
        bool locked = query.value(5).toBool();
        matches.append(Match(id, round, p1, p2, result, locked));
    }

    return matches;
}

QList<Match> Database::getAllMatches()
{
    QList<Match> matches;
    QSqlQuery query(db);
    query.prepare("SELECT id, round, p1, p2, result, locked FROM matches ORDER BY round, id");

    if (!query.exec())
    {
        qDebug() << "Failed to get all matches:" << query.lastError().text();
        return matches;
    }

    while (query.next())
    {
        int id = query.value(0).toInt();
        int round = query.value(1).toInt();
        int p1 = query.value(2).toInt();
        int p2 = query.value(3).toInt();
        QString result = query.value(4).toString();
        bool locked = query.value(5).toBool();
        matches.append(Match(id, round, p1, p2, result, locked));
    }

    return matches;
}

bool Database::updateMatchResult(int matchId, const QString &result)
{
    QSqlQuery query(db);
    query.prepare("UPDATE matches SET result = ?, locked = 0 WHERE id = ?");
    query.addBindValue(result);
    query.addBindValue(matchId);

    if (!query.exec())
    {
        qDebug() << "Failed to update match result:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool Database::lockMatch(int matchId)
{
    QSqlQuery query(db);
    query.prepare("UPDATE matches SET locked = 1 WHERE id = ? AND result IS NOT NULL");
    query.addBindValue(matchId);

    if (!query.exec())
    {
        qDebug() << "Failed to lock match:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool Database::unlockMatch(int matchId)
{
    QSqlQuery query(db);
    query.prepare("UPDATE matches SET locked = 0 WHERE id = ?");
    query.addBindValue(matchId);

    if (!query.exec())
    {
        qDebug() << "Failed to unlock match:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool Database::deleteMatch(int id)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM matches WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec())
    {
        qDebug() << "Failed to delete match:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

// Tournament operations
int Database::addTournament(const QString &name)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO tournaments (name) VALUES (?)");
    query.addBindValue(name);

    if (!query.exec())
    {
        qDebug() << "Failed to add tournament:" << query.lastError().text();
        return -1;
    }

    return query.lastInsertId().toInt();
}

QList<Tournament> Database::getAllTournaments()
{
    QList<Tournament> tournaments;
    QSqlQuery query(db);
    query.prepare("SELECT id, name, created_at FROM tournaments ORDER BY created_at DESC");

    if (!query.exec())
    {
        qDebug() << "Failed to get tournaments:" << query.lastError().text();
        return tournaments;
    }

    while (query.next())
    {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        QDateTime createdAt = query.value(2).toDateTime();
        tournaments.append(Tournament(id, name, createdAt));
    }

    return tournaments;
}

Tournament Database::getTournamentById(int id)
{
    QSqlQuery query(db);
    query.prepare("SELECT id, name, created_at FROM tournaments WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec() || !query.next())
    {
        qDebug() << "Failed to get tournament by ID:" << query.lastError().text();
        return Tournament(); // Return default tournament
    }

    QString name = query.value(1).toString();
    QDateTime createdAt = query.value(2).toDateTime();
    return Tournament(id, name, createdAt);
}

bool Database::updateTournament(const Tournament &tournament)
{
    QSqlQuery query(db);
    query.prepare("UPDATE tournaments SET name = ?, created_at = ? WHERE id = ?");
    query.addBindValue(tournament.getName());
    query.addBindValue(tournament.getCreatedAt());
    query.addBindValue(tournament.getId());

    if (!query.exec())
    {
        qDebug() << "Failed to update tournament:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool Database::deleteTournament(int id)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM tournaments WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec())
    {
        qDebug() << "Failed to delete tournament:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

// Utility methods
bool Database::resetDatabase()
{
    QSqlQuery query(db);

    // Delete all matches
    if (!query.exec("DELETE FROM matches"))
    {
        qDebug() << "Failed to delete matches:" << query.lastError().text();
        return false;
    }

    // Delete all tournaments
    if (!query.exec("DELETE FROM tournaments"))
    {
        qDebug() << "Failed to delete tournaments:" << query.lastError().text();
        return false;
    }

    // Reset autoincrement counters
    if (!query.exec("DELETE FROM sqlite_sequence WHERE name = 'matches'"))
    {
        qDebug() << "Failed to reset matches sequence:" << query.lastError().text();
        return false;
    }

    if (!query.exec("DELETE FROM sqlite_sequence WHERE name = 'tournaments'"))
    {
        qDebug() << "Failed to reset tournaments sequence:" << query.lastError().text();
        return false;
    }

    return true;
}