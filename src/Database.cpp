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

    // Create tournaments table
    QString createTournamentsTable = R"(
        CREATE TABLE IF NOT EXISTS tournaments (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT,
            pairing_system TEXT DEFAULT NULL,
            status TEXT DEFAULT 'setup',
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            started_at TIMESTAMP NULL,
            completed_at TIMESTAMP NULL
        )
    )";

    if (!query.exec(createTournamentsTable))
    {
        qDebug() << "Failed to create tournaments table:" << query.lastError().text();
        return false;
    }

    // Create matches table with tournament_id
    QString createMatchesTable = R"(
        CREATE TABLE IF NOT EXISTS matches (
            id INTEGER NOT NULL,
            tournament_id INTEGER NOT NULL,
            round INTEGER NOT NULL,
            p1 INTEGER NOT NULL,
            p2 INTEGER NOT NULL,
            result TEXT DEFAULT NULL,
            locked INTEGER DEFAULT 0,
            PRIMARY KEY (id, tournament_id),
            FOREIGN KEY (tournament_id) REFERENCES tournaments(id),
            FOREIGN KEY (p1) REFERENCES players(id),
            FOREIGN KEY (p2) REFERENCES players(id)
        )
    )";

    if (!query.exec(createMatchesTable))
    {
        qDebug() << "Failed to create matches table:" << query.lastError().text();
        return false;
    }

    // Create match_id_sequence table to track next match ID for each tournament
    QString createMatchIdSequenceTable = R"(
        CREATE TABLE IF NOT EXISTS match_id_sequence (
            tournament_id INTEGER PRIMARY KEY,
            next_id INTEGER NOT NULL DEFAULT 1,
            FOREIGN KEY (tournament_id) REFERENCES tournaments(id)
        )
    )";

    if (!query.exec(createMatchIdSequenceTable))
    {
        qDebug() << "Failed to create match_id_sequence table:" << query.lastError().text();
        return false;
    }

    // Create tournament_results table to store completed tournament data
    QString createTournamentResultsTable = R"(
        CREATE TABLE IF NOT EXISTS tournament_results (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            tournament_id INTEGER NOT NULL,
            player_id INTEGER NOT NULL,
            player_name TEXT NOT NULL,
            final_rank INTEGER NOT NULL,
            points REAL NOT NULL,
            wins INTEGER NOT NULL,
            losses INTEGER NOT NULL,
            draws INTEGER NOT NULL,
            buchholz_cut_one REAL,
            sonneborn_berger REAL,
            number_of_wins INTEGER,
            cumulative_opponent_score REAL,
            FOREIGN KEY (tournament_id) REFERENCES tournaments(id),
            FOREIGN KEY (player_id) REFERENCES players(id)
        )
    )";

    if (!query.exec(createTournamentResultsTable))
    {
        qDebug() << "Failed to create tournament_results table:" << query.lastError().text();
        return false;
    }

    // Create player_tournaments linking table
    QString createPlayerTournamentsTable = R"(
        CREATE TABLE IF NOT EXISTS player_tournaments (
            player_id INTEGER NOT NULL,
            tournament_id INTEGER NOT NULL,
            PRIMARY KEY (player_id, tournament_id),
            FOREIGN KEY (player_id) REFERENCES players(id) ON DELETE CASCADE,
            FOREIGN KEY (tournament_id) REFERENCES tournaments(id) ON DELETE CASCADE
        )
    )";

    if (!query.exec(createPlayerTournamentsTable))
    {
        qDebug() << "Failed to create player_tournaments table:" << query.lastError().text();
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

int Database::addPlayerToTournament(const QString &name, int tournamentId, int seed)
{
    // Begin transaction for atomicity
    db.transaction();

    // Add player to players table
    QSqlQuery query(db);
    query.prepare("INSERT OR IGNORE INTO players (name, seed) VALUES (?, ?)");
    query.addBindValue(name);
    query.addBindValue(seed);

    if (!query.exec())
    {
        qDebug() << "Failed to add player:" << query.lastError().text();
        db.rollback();
        return -1;
    }

    // Get the player ID (either newly inserted or existing)
    query.prepare("SELECT id FROM players WHERE name = ?");
    query.addBindValue(name);

    if (!query.exec() || !query.next())
    {
        qDebug() << "Failed to get player ID:" << query.lastError().text();
        db.rollback();
        return -1;
    }

    int playerId = query.value(0).toInt();

    // Associate player with tournament
    query.prepare("INSERT OR IGNORE INTO player_tournaments (player_id, tournament_id) VALUES (?, ?)");
    query.addBindValue(playerId);
    query.addBindValue(tournamentId);

    if (!query.exec())
    {
        qDebug() << "Failed to associate player with tournament:" << query.lastError().text();
        db.rollback();
        return -1;
    }

    // Commit transaction
    if (!db.commit())
    {
        qDebug() << "Failed to commit add player transaction:" << db.lastError().text();
        return -1;
    }

    return playerId;
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

    // Begin transaction for atomicity
    db.transaction();

    // Check how many tournaments this player is in
    int tournamentCount = getPlayerTournamentCount(id);

    // Delete player_tournaments associations (handled by CASCADE, but we'll do it explicitly)
    query.prepare("DELETE FROM player_tournaments WHERE player_id = ?");
    query.addBindValue(id);

    if (!query.exec())
    {
        qDebug() << "Failed to delete player tournament associations:" << query.lastError().text();
        db.rollback();
        return false;
    }

    // If player is not in any tournaments, delete the player itself
    // Note: With CASCADE DELETE, the player would be automatically deleted when removed from all tournaments
    // But we're not using CASCADE DELETE on players to allow players to exist without tournaments
    if (tournamentCount == 0)
    {
        query.prepare("DELETE FROM players WHERE id = ?");
        query.addBindValue(id);

        if (!query.exec())
        {
            qDebug() << "Failed to delete player:" << query.lastError().text();
            db.rollback();
            return false;
        }
    }

    // Commit transaction
    if (!db.commit())
    {
        qDebug() << "Failed to commit player deletion transaction:" << db.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

// Player-tournament operations
bool Database::addPlayerToTournament(int playerId, int tournamentId)
{
    QSqlQuery query(db);
    query.prepare("INSERT OR IGNORE INTO player_tournaments (player_id, tournament_id) VALUES (?, ?)");
    query.addBindValue(playerId);
    query.addBindValue(tournamentId);

    if (!query.exec())
    {
        qDebug() << "Failed to add player tournament:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool Database::removePlayerFromTournament(int playerId, int tournamentId)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM player_tournaments WHERE player_id = ? AND tournament_id = ?");
    query.addBindValue(playerId);
    query.addBindValue(tournamentId);

    if (!query.exec())
    {
        qDebug() << "Failed to remove player from tournament:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

QList<Player> Database::getPlayersForTournament(int tournamentId)
{
    QList<Player> players;
    QSqlQuery query(db);
    query.prepare("SELECT p.id, p.name, p.seed FROM players p "
                  "JOIN player_tournaments pt ON p.id = pt.player_id "
                  "WHERE pt.tournament_id = ? ORDER BY p.name");
    query.addBindValue(tournamentId);

    if (!query.exec())
    {
        qDebug() << "Failed to get players for tournament:" << query.lastError().text();
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

bool Database::isPlayerInTournament(int playerId, int tournamentId)
{
    QSqlQuery query(db);
    query.prepare("SELECT 1 FROM player_tournaments WHERE player_id = ? AND tournament_id = ?");
    query.addBindValue(playerId);
    query.addBindValue(tournamentId);

    if (!query.exec())
    {
        qDebug() << "Failed to check if player is in tournament:" << query.lastError().text();
        return false;
    }

    return query.next(); // Returns true if a row was found
}

int Database::getPlayerTournamentCount(int playerId)
{
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM player_tournaments WHERE player_id = ?");
    query.addBindValue(playerId);

    if (!query.exec() || !query.next())
    {
        qDebug() << "Failed to get player tournament count:" << query.lastError().text();
        return 0;
    }

    return query.value(0).toInt();
}

// Match operations
int Database::addMatch(int tournamentId, int round, int p1, int p2)
{
    QSqlQuery query(db);

    // Begin transaction for atomicity
    db.transaction();

    // Get the next match ID for this tournament
    int nextMatchId = 1;
    query.prepare("SELECT next_id FROM match_id_sequence WHERE tournament_id = ?");
    query.addBindValue(tournamentId);

    if (query.exec() && query.next())
    {
        // Tournament exists in sequence table, get next ID
        nextMatchId = query.value(0).toInt();
    }
    else
    {
        // Tournament doesn't exist in sequence table, check if there are existing matches
        query.prepare("SELECT COALESCE(MAX(id), 0) + 1 FROM matches WHERE tournament_id = ?");
        query.addBindValue(tournamentId);

        if (query.exec() && query.next())
        {
            nextMatchId = query.value(0).toInt();
        }
    }

    // Insert the match with the calculated ID
    query.prepare("INSERT INTO matches (id, tournament_id, round, p1, p2) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(nextMatchId);
    query.addBindValue(tournamentId);
    query.addBindValue(round);
    query.addBindValue(p1);
    query.addBindValue(p2);

    if (!query.exec())
    {
        qDebug() << "Failed to add match:" << query.lastError().text();
        db.rollback();
        return -1;
    }

    // Update the next ID for this tournament
    query.prepare("INSERT OR REPLACE INTO match_id_sequence (tournament_id, next_id) VALUES (?, ?)");
    query.addBindValue(tournamentId);
    query.addBindValue(nextMatchId + 1);

    if (!query.exec())
    {
        qDebug() << "Failed to update match ID sequence:" << query.lastError().text();
        db.rollback();
        return -1;
    }

    // Commit transaction
    if (!db.commit())
    {
        qDebug() << "Failed to commit transaction:" << db.lastError().text();
        return -1;
    }

    return nextMatchId;
}

QList<Match> Database::getMatchesForRound(int tournamentId, int round)
{
    QList<Match> matches;
    QSqlQuery query(db);
    query.prepare("SELECT id, tournament_id, round, p1, p2, result, locked FROM matches WHERE tournament_id = ? AND round = ? ORDER BY id");
    query.addBindValue(tournamentId);
    query.addBindValue(round);

    if (!query.exec())
    {
        qDebug() << "Failed to get matches for round:" << query.lastError().text();
        return matches;
    }

    while (query.next())
    {
        int id = query.value(0).toInt();
        int tournamentId = query.value(1).toInt();
        int round = query.value(2).toInt();
        int p1 = query.value(3).toInt();
        int p2 = query.value(4).toInt();
        QString result = query.value(5).toString();
        bool locked = query.value(6).toBool();
        matches.append(Match(id, tournamentId, round, p1, p2, result, locked));
    }

    return matches;
}

QList<Match> Database::getAllMatches(int tournamentId)
{
    QList<Match> matches;
    QSqlQuery query(db);

    if (tournamentId == -1)
    {
        // Get all matches
        query.prepare("SELECT id, tournament_id, round, p1, p2, result, locked FROM matches ORDER BY round, id");
    }
    else
    {
        // Get matches for specific tournament
        query.prepare("SELECT id, tournament_id, round, p1, p2, result, locked FROM matches WHERE tournament_id = ? ORDER BY round, id");
        query.addBindValue(tournamentId);
    }

    if (!query.exec())
    {
        qDebug() << "Failed to get all matches:" << query.lastError().text();
        return matches;
    }

    while (query.next())
    {
        int id = query.value(0).toInt();
        int tournamentId = query.value(1).toInt();
        int round = query.value(2).toInt();
        int p1 = query.value(3).toInt();
        int p2 = query.value(4).toInt();
        QString result = query.value(5).toString();
        bool locked = query.value(6).toBool();
        matches.append(Match(id, tournamentId, round, p1, p2, result, locked));
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
    query.prepare("INSERT INTO tournaments (name, pairing_system, status) VALUES (?, ?, ?)");
    query.addBindValue(name);
    query.addBindValue("round_robin"); // default pairing system
    query.addBindValue("setup");       // default status

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
    query.prepare("SELECT id, name, pairing_system, status, created_at, started_at, completed_at FROM tournaments ORDER BY created_at DESC");

    if (!query.exec())
    {
        qDebug() << "Failed to get tournaments:" << query.lastError().text();
        return tournaments;
    }

    while (query.next())
    {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        QString pairingSystem = query.value(2).toString();
        QString statusString = query.value(3).toString();
        QDateTime createdAt = query.value(4).toDateTime();
        QDateTime startedAt = query.value(5).toDateTime();
        QDateTime completedAt = query.value(6).toDateTime();

        Tournament tournament(id, name, createdAt);
        tournament.setPairingSystem(pairingSystem);
        tournament.setStatusFromString(statusString);
        tournament.setStartedAt(startedAt);
        tournament.setCompletedAt(completedAt);

        tournaments.append(tournament);
    }

    return tournaments;
}

Tournament Database::getTournamentById(int id)
{
    QSqlQuery query(db);
    query.prepare("SELECT id, name, pairing_system, status, created_at, started_at, completed_at FROM tournaments WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec() || !query.next())
    {
        qDebug() << "Failed to get tournament by ID:" << query.lastError().text();
        return Tournament(); // Return default tournament
    }

    QString name = query.value(1).toString();
    QString pairingSystem = query.value(2).toString();
    QString statusString = query.value(3).toString();
    QDateTime createdAt = query.value(4).toDateTime();
    QDateTime startedAt = query.value(5).toDateTime();
    QDateTime completedAt = query.value(6).toDateTime();

    Tournament tournament(id, name, createdAt);
    tournament.setPairingSystem(pairingSystem);
    tournament.setStatusFromString(statusString);
    tournament.setStartedAt(startedAt);
    tournament.setCompletedAt(completedAt);

    return tournament;
}

bool Database::updateTournament(const Tournament &tournament)
{
    QSqlQuery query(db);
    query.prepare("UPDATE tournaments SET name = ?, pairing_system = ?, status = ?, created_at = ?, started_at = ?, completed_at = ? WHERE id = ?");
    query.addBindValue(tournament.getName());
    query.addBindValue(tournament.getPairingSystem());
    query.addBindValue(tournament.getStatusString());
    query.addBindValue(tournament.getCreatedAt());
    query.addBindValue(tournament.getStartedAt());
    query.addBindValue(tournament.getCompletedAt());
    query.addBindValue(tournament.getId());

    if (!query.exec())
    {
        qDebug() << "Failed to update tournament:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool Database::startTournament(int id)
{
    QSqlQuery query(db);
    query.prepare("UPDATE tournaments SET status = ?, started_at = datetime('now') WHERE id = ?");
    query.addBindValue("active");
    query.addBindValue(id);

    if (!query.exec())
    {
        qDebug() << "Failed to start tournament:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool Database::completeTournament(int id)
{
    QSqlQuery query(db);
    query.prepare("UPDATE tournaments SET status = ?, completed_at = datetime('now') WHERE id = ?");
    query.addBindValue("completed");
    query.addBindValue(id);

    if (!query.exec())
    {
        qDebug() << "Failed to complete tournament:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool Database::deleteTournament(int id)
{
    QSqlQuery query(db);

    // Begin transaction for atomicity
    db.transaction();

    // Delete matches for this tournament
    query.prepare("DELETE FROM matches WHERE tournament_id = ?");
    query.addBindValue(id);

    if (!query.exec())
    {
        qDebug() << "Failed to delete matches for tournament:" << query.lastError().text();
        db.rollback();
        return false;
    }

    // Delete tournament results for this tournament
    query.prepare("DELETE FROM tournament_results WHERE tournament_id = ?");
    query.addBindValue(id);

    if (!query.exec())
    {
        qDebug() << "Failed to delete tournament results:" << query.lastError().text();
        db.rollback();
        return false;
    }

    // Delete player_tournaments associations (handled by CASCADE, but we'll do it explicitly for clarity)
    query.prepare("DELETE FROM player_tournaments WHERE tournament_id = ?");
    query.addBindValue(id);

    if (!query.exec())
    {
        qDebug() << "Failed to delete player tournament associations:" << query.lastError().text();
        db.rollback();
        return false;
    }

    // Delete match_id_sequence entries for this tournament
    query.prepare("DELETE FROM match_id_sequence WHERE tournament_id = ?");
    query.addBindValue(id);

    if (!query.exec())
    {
        qDebug() << "Failed to delete match ID sequence entries:" << query.lastError().text();
        db.rollback();
        return false;
    }

    // Delete the tournament itself
    query.prepare("DELETE FROM tournaments WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec())
    {
        qDebug() << "Failed to delete tournament:" << query.lastError().text();
        db.rollback();
        return false;
    }

    // Commit transaction
    if (!db.commit())
    {
        qDebug() << "Failed to commit tournament deletion transaction:" << db.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

// Utility methods
bool Database::removeTournament(int tournamentId)
{
    QSqlQuery query(db);

    // Delete the tournament itself
    query.prepare("DELETE FROM tournaments WHERE id = :tournamentId");
    query.bindValue(":tournamentId", tournamentId);
    if (!query.exec())
    {
        qDebug() << "Failed to delete tournament:" << query.lastError().text();
        return false;
    }

    if (!query.exec("DELETE FROM sqlite_sequence WHERE name = 'tournaments'"))
    {
        qDebug() << "Failed to reset tournaments sequence:" << query.lastError().text();
        return false;
    }

    return true;
}

bool Database::resetDatabase(int tournamentId)
{
    QSqlQuery query(db);

    // Delete all matches for this tournament
    query.prepare("DELETE FROM matches WHERE tournament_id = :tournamentId");
    query.bindValue(":tournamentId", tournamentId);
    if (!query.exec())
    {
        qDebug() << "Failed to delete matches:" << query.lastError().text();
        return false;
    }

    // Reset autoincrement counters
    if (!query.exec("DELETE FROM sqlite_sequence WHERE name = 'matches'"))
    {
        qDebug() << "Failed to reset matches sequence:" << query.lastError().text();
        return false;
    }

    return true;
}

QList<Match> Database::getMatchesForTournament(int tournamentId)
{
    QList<Match> matches;
    QSqlQuery query(db);
    query.prepare("SELECT id, tournament_id, round, p1, p2, result, locked FROM matches WHERE tournament_id = ? ORDER BY round, id");
    query.addBindValue(tournamentId);

    if (!query.exec())
    {
        qDebug() << "Failed to get matches for tournament:" << query.lastError().text();
        return matches;
    }

    while (query.next())
    {
        int id = query.value(0).toInt();
        int tournamentId = query.value(1).toInt();
        int round = query.value(2).toInt();
        int p1 = query.value(3).toInt();
        int p2 = query.value(4).toInt();
        QString result = query.value(5).toString();
        bool locked = query.value(6).toBool();
        matches.append(Match(id, tournamentId, round, p1, p2, result, locked));
    }

    return matches;
}

bool Database::deleteMatchesForTournament(int tournamentId)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM matches WHERE tournament_id = ?");
    query.addBindValue(tournamentId);

    if (!query.exec())
    {
        qDebug() << "Failed to delete matches for tournament:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool Database::resetMatchIdSequence(int tournamentId)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM match_id_sequence WHERE tournament_id = ?");
    query.addBindValue(tournamentId);

    if (!query.exec())
    {
        qDebug() << "Failed to reset match ID sequence for tournament:" << query.lastError().text();
        return false;
    }

    return true;
}

bool Database::saveTournamentResults(int tournamentId, const QList<TournamentResult> &results)
{
    QSqlQuery query(db);

    // Begin transaction for atomicity
    db.transaction();

    // Delete any existing results for this tournament
    query.prepare("DELETE FROM tournament_results WHERE tournament_id = ?");
    query.addBindValue(tournamentId);

    if (!query.exec())
    {
        qDebug() << "Failed to delete existing tournament results:" << query.lastError().text();
        db.rollback();
        return false;
    }

    // Insert new results
    query.prepare("INSERT INTO tournament_results (tournament_id, player_id, player_name, final_rank, points, wins, losses, draws, buchholz_cut_one, sonneborn_berger, number_of_wins, cumulative_opponent_score) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

    for (const TournamentResult &result : results)
    {
        query.addBindValue(tournamentId);
        query.addBindValue(result.playerId);
        query.addBindValue(result.playerName);
        query.addBindValue(result.finalRank);
        query.addBindValue(result.points);
        query.addBindValue(result.wins);
        query.addBindValue(result.losses);
        query.addBindValue(result.draws);
        query.addBindValue(result.buchholzCutOne);
        query.addBindValue(result.sonnebornBerger);
        query.addBindValue(result.numberOfWins);
        query.addBindValue(result.cumulativeOpponentScore);

        if (!query.exec())
        {
            qDebug() << "Failed to insert tournament result:" << query.lastError().text();
            db.rollback();
            return false;
        }

        // Prepare for next iteration
        query.prepare("INSERT INTO tournament_results (tournament_id, player_id, player_name, final_rank, points, wins, losses, draws, buchholz_cut_one, sonneborn_berger, number_of_wins, cumulative_opponent_score) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    }

    // Commit transaction
    if (!db.commit())
    {
        qDebug() << "Failed to commit tournament results transaction:" << db.lastError().text();
        return false;
    }

    return true;
}

QList<TournamentResult> Database::getTournamentResults(int tournamentId)
{
    QList<TournamentResult> results;
    QSqlQuery query(db);
    query.prepare("SELECT player_id, player_name, final_rank, points, wins, losses, draws, buchholz_cut_one, sonneborn_berger, number_of_wins, cumulative_opponent_score FROM tournament_results WHERE tournament_id = ? ORDER BY final_rank");
    query.addBindValue(tournamentId);

    if (!query.exec())
    {
        qDebug() << "Failed to get tournament results:" << query.lastError().text();
        return results;
    }

    while (query.next())
    {
        TournamentResult result;
        result.playerId = query.value(0).toInt();
        result.playerName = query.value(1).toString();
        result.finalRank = query.value(2).toInt();
        result.points = query.value(3).toDouble();
        result.wins = query.value(4).toInt();
        result.losses = query.value(5).toInt();
        result.draws = query.value(6).toInt();
        result.buchholzCutOne = query.value(7).toDouble();
        result.sonnebornBerger = query.value(8).toDouble();
        result.numberOfWins = query.value(9).toInt();
        result.cumulativeOpponentScore = query.value(10).toDouble();

        results.append(result);
    }

    return results;
}