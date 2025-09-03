#include "MainWindow.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QTextEdit>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QDateTime>
#include <QTextStream>
#include <QFile>
#include <QDir>
#include <algorithm>
#include <vector>
#include <string>
#include <numeric>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), database(new Database())
{
    // Connect to database
    if (!database->connect())
    {
        QMessageBox::critical(this, "Database Error",
                              "Failed to connect to database. The application will exit.");
        QApplication::exit(1);
    }

    setupUI();
    updatePlayerList();
    updateMatchTabs();

    // Set initial status
    mainStatusBar->showMessage("Ready");
}

MainWindow::~MainWindow()
{
    delete database;
}

void MainWindow::setupUI()
{
    setWindowTitle("OpenTournament");
    resize(800, 600);

    setupMenu();
    setupToolbar();
    setupPlayerPanel();
    setupMatchPanel();

    // Create central widget with splitter
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(playerTable);
    splitter->addWidget(roundTabs);
    splitter->setSizes(QList<int>() << 300 << 500);

    setCentralWidget(splitter);
    setStatusBar(mainStatusBar);
}

void MainWindow::setupMenu()
{
    menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    // File menu
    QMenu *fileMenu = menuBar->addMenu("&File");
    newTournamentAction = fileMenu->addAction("&New Tournament");
    exportResultsAction = fileMenu->addAction("&Export Results");
    fileMenu->addSeparator();
    exitAction = fileMenu->addAction("E&xit");

    connect(newTournamentAction, &QAction::triggered, this, &MainWindow::onResetTournamentClicked);
    connect(exportResultsAction, &QAction::triggered, this, &MainWindow::onExportResultsClicked);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);

    // Help menu
    QMenu *helpMenu = menuBar->addMenu("&Help");
    aboutAction = helpMenu->addAction("&About");
    connect(aboutAction, &QAction::triggered, [this]()
            { QMessageBox::about(this, "About OpenTournament",
                                 "OpenTournament v0.1\n\n"
                                 "A desktop application for managing tournament brackets using a round-robin pairing system."); });
}

void MainWindow::setupToolbar()
{
    toolBar = addToolBar("Main");

    addPlayerButton = new QPushButton("Add Player", this);
    resetTournamentButton = new QPushButton("Reset Tournament", this);
    exportResultsButton = new QPushButton("Export Results", this);

    toolBar->addWidget(addPlayerButton);
    toolBar->addWidget(resetTournamentButton);
    toolBar->addWidget(exportResultsButton);

    connect(addPlayerButton, &QPushButton::clicked, this, &MainWindow::onAddPlayerClicked);
    connect(resetTournamentButton, &QPushButton::clicked, this, &MainWindow::onResetTournamentClicked);
    connect(exportResultsButton, &QPushButton::clicked, this, &MainWindow::onExportResultsClicked);
}

void MainWindow::setupPlayerPanel()
{
    playerTable = new QTableWidget(0, 3, this);
    playerTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    playerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStringList headers;
    headers << "Rank" << "Player" << "Points";
    playerTable->setHorizontalHeaderLabels(headers);
    playerTable->horizontalHeader()->setStretchLastSection(true);
    playerTable->verticalHeader()->setVisible(false);

    connect(playerTable, &QTableWidget::itemSelectionChanged, this, &MainWindow::onPlayerSelectionChanged);
}

void MainWindow::setupMatchPanel()
{
    roundTabs = new QTabWidget(this);
    roundTabs->setTabsClosable(false);
    mainStatusBar = new QStatusBar(this);
}

void MainWindow::onAddPlayerClicked()
{
    showAddPlayerDialog();
}

void MainWindow::showAddPlayerDialog()
{
    bool ok;
    QString name = QInputDialog::getText(this, "Add Player",
                                         "Enter player name:", QLineEdit::Normal,
                                         "", &ok);

    if (ok && !name.isEmpty())
    {
        // Check if player already exists
        QList<Player> players = database->getAllPlayers();
        for (const Player &player : players)
        {
            if (player.getName() == name)
            {
                QMessageBox::warning(this, "Duplicate Player",
                                     "A player with this name already exists.");
                return;
            }
        }

        // Add player to database
        int playerId = database->addPlayer(name);
        if (playerId == -1)
        {
            QMessageBox::critical(this, "Error", "Failed to add player to database.");
            return;
        }

        updatePlayerList();
        generateRoundRobinPairings();
        updateMatchTabs();
        mainStatusBar->showMessage("Player added successfully");
    }
}

void MainWindow::onResetTournamentClicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Reset",
                                  "This will clear all match results. Players will not be deleted. Continue?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        if (database->resetDatabase())
        {
            updatePlayerList();
            updateMatchTabs();
            mainStatusBar->showMessage("Tournament reset successfully");
        }
        else
        {
            QMessageBox::critical(this, "Error", "Failed to reset tournament.");
        }
    }
}

void MainWindow::onExportResultsClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Export Tournament Results",
                                                    "tournament_results.csv",
                                                    "CSV Files (*.csv)");

    if (!fileName.isEmpty())
    {
        exportToCSV(fileName);
    }
}

void MainWindow::exportToCSV(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Error", "Failed to create export file.");
        return;
    }

    QTextStream out(&file);

    // Write header
    out << "Player,Points,Wins,Losses,Draws\n";

    // Get player statistics
    QList<Player> players = database->getAllPlayers();
    QList<Match> matches = database->getAllMatches();

    for (const Player &player : players)
    {
        int wins = 0, losses = 0, draws = 0;

        for (const Match &match : matches)
        {
            if (match.isPlayed())
            {
                if (match.getPlayer1Id() == player.getId())
                {
                    if (match.getResult() == "p1")
                        wins++;
                    else if (match.getResult() == "p2")
                        losses++;
                    else if (match.getResult() == "draw")
                        draws++;
                }
                else if (match.getPlayer2Id() == player.getId())
                {
                    if (match.getResult() == "p2")
                        wins++;
                    else if (match.getResult() == "p1")
                        losses++;
                    else if (match.getResult() == "draw")
                        draws++;
                }
            }
        }

        double points = wins + (draws * 0.5);
        out << "\"" << player.getName() << "\","
            << QString::number(points, 'f', 1) << ","
            << wins << "," << losses << "," << draws << "\n";
    }

    file.close();
    mainStatusBar->showMessage("Results exported successfully to " + filename);
}

void MainWindow::onMatchResultClicked()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (!button)
        return;

    // Extract match ID and result from button properties
    int matchId = button->property("matchId").toInt();
    QString result = button->property("result").toString();

    // Confirm result submission
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Result",
                                  "Confirm result submission?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        if (database->updateMatchResult(matchId, result))
        {
            if (database->lockMatch(matchId))
            {
                updateMatchTabs();
                updateLeaderboard();
                mainStatusBar->showMessage("Result submitted successfully");
            }
        }
        else
        {
            QMessageBox::critical(this, "Error", "Failed to submit result.");
        }
    }
}

void MainWindow::onEditMatchClicked()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (!button)
        return;

    // Extract match ID from button properties
    int matchId = button->property("matchId").toInt();

    // Confirm unlock
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Unlock Match",
                                  "Unlock this match for editing?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        if (database->unlockMatch(matchId))
        {
            updateMatchTabs();
            mainStatusBar->showMessage("Match unlocked for editing");
        }
        else
        {
            QMessageBox::critical(this, "Error", "Failed to unlock match.");
        }
    }
}

void MainWindow::onPlayerSelectionChanged()
{
    // Handle player selection changes if needed
}

void MainWindow::updatePlayerList()
{
    QList<Player> players = database->getAllPlayers();
    QList<Match> matches = database->getAllMatches();

    // Calculate points for each player
    struct PlayerStats
    {
        int id;
        QString name;
        double points;
        int wins;
        int losses;
        int draws;
    };

    QList<PlayerStats> playerStats;
    for (const Player &player : players)
    {
        PlayerStats stats;
        stats.id = player.getId();
        stats.name = player.getName();
        stats.points = 0.0;
        stats.wins = 0;
        stats.losses = 0;
        stats.draws = 0;

        for (const Match &match : matches)
        {
            if (match.isPlayed())
            {
                if (match.getPlayer1Id() == player.getId())
                {
                    if (match.getResult() == "p1")
                    {
                        stats.wins++;
                        stats.points += 1.0;
                    }
                    else if (match.getResult() == "p2")
                    {
                        stats.losses++;
                    }
                    else if (match.getResult() == "draw")
                    {
                        stats.draws++;
                        stats.points += 0.5;
                    }
                }
                else if (match.getPlayer2Id() == player.getId())
                {
                    if (match.getResult() == "p2")
                    {
                        stats.wins++;
                        stats.points += 1.0;
                    }
                    else if (match.getResult() == "p1")
                    {
                        stats.losses++;
                    }
                    else if (match.getResult() == "draw")
                    {
                        stats.draws++;
                        stats.points += 0.5;
                    }
                }
            }
        }

        playerStats.append(stats);
    }

    // Sort by points (descending) then by name (ascending)
    std::sort(playerStats.begin(), playerStats.end(), [](const PlayerStats &a, const PlayerStats &b)
              {
        if (a.points != b.points) {
            return a.points > b.points;
        }
        return a.name < b.name; });

    // Update player table
    playerTable->setRowCount(playerStats.size());
    for (int i = 0; i < playerStats.size(); ++i)
    {
        const PlayerStats &stats = playerStats[i];

        QTableWidgetItem *rankItem = new QTableWidgetItem(QString::number(i + 1));
        QTableWidgetItem *nameItem = new QTableWidgetItem(stats.name);
        QTableWidgetItem *pointsItem = new QTableWidgetItem(QString::number(stats.points, 'f', 1));

        playerTable->setItem(i, 0, rankItem);
        playerTable->setItem(i, 1, nameItem);
        playerTable->setItem(i, 2, pointsItem);
    }
}

void MainWindow::updateLeaderboard()
{
    updatePlayerList();
}

struct Pair
{
    std::string p1;
    std::string p2;
};

std::vector<std::vector<Pair>> roundRobinPairings(std::vector<std::string> players)
{
    int n = players.size();
    if (n % 2 == 1)
    {
        players.push_back("BYE");
        n++;
    }
    std::vector<std::vector<Pair>> rounds;
    for (int r = 0; r < n - 1; r++)
    {
        std::vector<Pair> round;
        for (int i = 0; i < n / 2; i++)
        {
            std::string p1 = players[i];
            std::string p2 = players[n - 1 - i];
            if (p1 != "BYE" && p2 != "BYE")
                round.push_back({p1, p2});
        }
        rounds.push_back(round);
        // rotate
        std::rotate(players.begin() + 1, players.end() - 1, players.end());
    }
    return rounds;
}

void MainWindow::generateRoundRobinPairings()
{
    QList<Player> players = database->getAllPlayers();

    // We need at least 3 players to generate pairings
    if (players.size() < 3)
    {
        return;
    }

    // Convert player names to std::string
    std::vector<std::string> playerNames;
    for (const Player &player : players)
    {
        playerNames.push_back(player.getName().toStdString());
    }

    // Generate pairings
    std::vector<std::vector<Pair>> pairings = roundRobinPairings(playerNames);

    // Clear existing matches
    QList<Match> existingMatches = database->getAllMatches();
    for (const Match &match : existingMatches)
    {
        database->deleteMatch(match.getId());
    }

    // Create matches in database
    int roundNumber = 1;
    for (const auto &round : pairings)
    {
        for (const auto &pair : round)
        {
            // Find player IDs by name
            int p1Id = -1, p2Id = -1;
            QString p1Name = QString::fromStdString(pair.p1);
            QString p2Name = QString::fromStdString(pair.p2);

            for (const Player &player : players)
            {
                if (player.getName() == p1Name)
                {
                    p1Id = player.getId();
                }
                if (player.getName() == p2Name)
                {
                    p2Id = player.getId();
                }
            }

            // Create match if both players found
            if (p1Id != -1 && p2Id != -1)
            {
                database->addMatch(roundNumber, p1Id, p2Id);
            }
        }
        roundNumber++;
    }
}

void MainWindow::updateMatchTabs()
{
    // Clear existing tabs
    while (roundTabs->count() > 0)
    {
        QWidget *widget = roundTabs->widget(0);
        roundTabs->removeTab(0);
        delete widget;
    }

    // Get all matches grouped by round
    QList<Match> matches = database->getAllMatches();

    // Group matches by round
    QMap<int, QList<Match>> matchesByRound;
    for (const Match &match : matches)
    {
        matchesByRound[match.getRound()].append(match);
    }

    // Create a tab for each round
    QList<int> rounds = matchesByRound.keys();
    std::sort(rounds.begin(), rounds.end());

    for (int round : rounds)
    {
        QList<Match> roundMatches = matchesByRound[round];

        // Create table for this round
        QTableWidget *table = new QTableWidget(roundMatches.size(), 5);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);

        QStringList headers;
        headers << "Match" << "Player 1" << "Player 2" << "Result" << "Actions";
        table->setHorizontalHeaderLabels(headers);
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->verticalHeader()->setVisible(false);

        // Populate table with matches
        for (int i = 0; i < roundMatches.size(); ++i)
        {
            const Match &match = roundMatches[i];

            // Get player names
            Player player1 = database->getPlayerById(match.getPlayer1Id());
            Player player2 = database->getPlayerById(match.getPlayer2Id());

            // Match ID
            QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(match.getId()));
            idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable);
            table->setItem(i, 0, idItem);

            // Player names
            QTableWidgetItem *p1Item = new QTableWidgetItem(player1.getName());
            p1Item->setFlags(p1Item->flags() & ~Qt::ItemIsEditable);
            table->setItem(i, 1, p1Item);

            QTableWidgetItem *p2Item = new QTableWidgetItem(player2.getName());
            p2Item->setFlags(p2Item->flags() & ~Qt::ItemIsEditable);
            table->setItem(i, 2, p2Item);

            // Result
            QString resultText;
            if (!match.isPlayed())
            {
                resultText = "Not Played";
            }
            else if (match.getResult() == "p1")
            {
                resultText = player1.getName() + " Wins";
            }
            else if (match.getResult() == "p2")
            {
                resultText = player2.getName() + " Wins";
            }
            else if (match.getResult() == "draw")
            {
                resultText = "Draw";
            }

            QTableWidgetItem *resultItem = new QTableWidgetItem(resultText);
            resultItem->setFlags(resultItem->flags() & ~Qt::ItemIsEditable);
            table->setItem(i, 3, resultItem);

            // Actions
            QWidget *actionsWidget = new QWidget();
            QHBoxLayout *actionsLayout = new QHBoxLayout(actionsWidget);
            actionsLayout->setContentsMargins(0, 0, 0, 0);

            if (!match.isLocked())
            {
                // Result buttons
                QPushButton *p1WinButton = new QPushButton("P1 Win");
                QPushButton *drawButton = new QPushButton("Draw");
                QPushButton *p2WinButton = new QPushButton("P2 Win");

                // Set properties for identification
                p1WinButton->setProperty("matchId", match.getId());
                p1WinButton->setProperty("result", "p1");
                drawButton->setProperty("matchId", match.getId());
                drawButton->setProperty("result", "draw");
                p2WinButton->setProperty("matchId", match.getId());
                p2WinButton->setProperty("result", "p2");

                // Connect signals
                connect(p1WinButton, &QPushButton::clicked, this, &MainWindow::onMatchResultClicked);
                connect(drawButton, &QPushButton::clicked, this, &MainWindow::onMatchResultClicked);
                connect(p2WinButton, &QPushButton::clicked, this, &MainWindow::onMatchResultClicked);

                actionsLayout->addWidget(p1WinButton);
                actionsLayout->addWidget(drawButton);
                actionsLayout->addWidget(p2WinButton);
            }
            else
            {
                // Edit button
                QPushButton *editButton = new QPushButton("Edit");
                editButton->setProperty("matchId", match.getId());
                connect(editButton, &QPushButton::clicked, this, &MainWindow::onEditMatchClicked);
                actionsLayout->addWidget(editButton);
            }

            actionsWidget->setLayout(actionsLayout);
            table->setCellWidget(i, 4, actionsWidget);
        }

        // Add tab
        roundTabs->addTab(table, QString("Round %1").arg(round));
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Exit",
                                  "Are you sure you want to exit?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}