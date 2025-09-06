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
#include <QGroupBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>
#include <algorithm>
#include <vector>
#include <string>
#include <numeric>
#include <iostream>

// Forward declarations for theme functions
void setFusionDark(QApplication &app);
void setFusionLight(QApplication &app);
void setFusionWhite(QApplication &app);
void setCustomQss(QApplication &app);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), database(new Database()), settings(new Settings()), app(qApp)
{
    // Connect to database
    if (!database->connect())
    {
        QMessageBox::critical(this, "Database Error",
                              "Failed to connect to database. The application will exit.");
        QApplication::exit(1);
    }

    // Load settings
    settings->load();

    setupUI();
    populateTournamentSelector();
    updatePlayerList();
    updateMatchTabs();

    // Set initial status
    mainStatusBar->showMessage("Ready");
}

MainWindow::~MainWindow()
{
    delete database;
    delete settings;
}

void MainWindow::setupUI()
{
    setWindowTitle("OpenTournament");
    resize(800, 600);

    setupMenu();
    setupToolbar();
    setupPlayerPanel();
    setupMatchPanel();

    // Initialize mainStatusBar after setupMatchPanel
    mainStatusBar = new QStatusBar(this);

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

    connect(newTournamentAction, &QAction::triggered, this, &MainWindow::onNewTournamentClicked);
    connect(exportResultsAction, &QAction::triggered, this, &MainWindow::onExportResultsClicked);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);

    // Themes menu
    QMenu *themeMenu = menuBar->addMenu("&Themes");
    fusionDarkAction = themeMenu->addAction("Fusion Dark");
    fusionLightAction = themeMenu->addAction("Fusion Light");
    fusionWhiteAction = themeMenu->addAction("Fusion White");
    customQssAction = themeMenu->addAction("Custom QSS");

    connect(fusionDarkAction, &QAction::triggered, this, &MainWindow::onFusionDarkSelected);
    connect(fusionLightAction, &QAction::triggered, this, &MainWindow::onFusionLightSelected);
    connect(fusionWhiteAction, &QAction::triggered, this, &MainWindow::onFusionWhiteSelected);
    connect(customQssAction, &QAction::triggered, this, &MainWindow::onCustomQssSelected);

    // Icons menu
    QMenu *iconsMenu = menuBar->addMenu("&Icons");
    defaultIconsAction = iconsMenu->addAction("Default");
    modernIconsAction = iconsMenu->addAction("Modern");

    connect(defaultIconsAction, &QAction::triggered, this, &MainWindow::onDefaultIconsSelected);
    connect(modernIconsAction, &QAction::triggered, this, &MainWindow::onModernIconsSelected);

    // Settings menu
    QMenu *settingsMenu = menuBar->addMenu("&Settings");
    settingsAction = settingsMenu->addAction("&Preferences");
    connect(settingsAction, &QAction::triggered, this, &MainWindow::showSettingsDialog);

    // Help menu
    QMenu *helpMenu = menuBar->addMenu("&Help");
    aboutAction = helpMenu->addAction("&About");
    connect(aboutAction, &QAction::triggered, [this]()
            { QMessageBox::about(this, "About OpenTournament",
                                 "OpenTournament v0.1\n\n"
                                 "A desktop application for managing tournament brackets using a round-robin pairing system."); });
}

void MainWindow::onFusionDarkSelected()
{
    setFusionDark(*app);
    settings->setTheme("fusion_dark");
    settings->save();
    mainStatusBar->showMessage("Theme set to Fusion Dark");
}

void MainWindow::onFusionLightSelected()
{
    setFusionLight(*app);
    settings->setTheme("fusion_light");
    settings->save();
    mainStatusBar->showMessage("Theme set to Fusion Light");
}

void MainWindow::onFusionWhiteSelected()
{
    setFusionWhite(*app);
    settings->setTheme("fusion_white");
    settings->save();
    mainStatusBar->showMessage("Theme set to Fusion White");
}

void MainWindow::onCustomQssSelected()
{
    setCustomQss(*app);
    settings->setTheme("custom");
    settings->save();
    mainStatusBar->showMessage("Theme set to Custom QSS");
}

void MainWindow::onDefaultIconsSelected()
{
    setDefaultIcons();
    settings->setIconSet("default");
    settings->save();
    mainStatusBar->showMessage("Icons set to Default");
}

void MainWindow::onModernIconsSelected()
{
    setModernIcons();
    settings->setIconSet("modern");
    settings->save();
    mainStatusBar->showMessage("Icons set to Modern");
}

void MainWindow::setDefaultIcons()
{
    // Revert to Qt standard icons for all buttons
    addPlayerButton->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    resetTournamentButton->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
    exportResultsButton->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    startTournamentButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    endTournamentButton->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
    tiebreakerButton->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
}

void MainWindow::setModernIcons()
{
    // Load custom icons from icons/ directory
    addPlayerButton->setIcon(QIcon("icons/add.png"));
    resetTournamentButton->setIcon(QIcon("icons/reset.png"));
    exportResultsButton->setIcon(QIcon("icons/export.png"));
    startTournamentButton->setIcon(QIcon("icons/start.png"));
    endTournamentButton->setIcon(QIcon("icons/end.png"));
    tiebreakerButton->setIcon(QIcon("icons/tiebreaker.png"));
}

void MainWindow::onNewTournamentClicked()
{
    // Show input dialog for tournament name
    bool ok;
    QString name = QInputDialog::getText(this, "Add Tournament",
                                         "Enter tournament name:", QLineEdit::Normal,
                                         "", &ok);

    if (ok && !name.isEmpty())
    {
        // Add tournament to database
        int tournamentId = database->addTournament(name);
        if (tournamentId == -1)
        {
            QMessageBox::critical(this, "Error", "Failed to add tournament to database.");
            return;
        }

        // Update tournament selector
        populateTournamentSelector();

        // Select the newly added tournament
        for (int i = 0; i < tournamentSelector->count(); ++i)
        {
            if (tournamentSelector->itemData(i).toInt() == tournamentId)
            {
                tournamentSelector->setCurrentIndex(i);
                break;
            }
        }

        mainStatusBar->showMessage("Tournament added successfully");
    }
}

void MainWindow::setupToolbar()
{
    toolBar = addToolBar("Main");

    // Tournament selector
    tournamentSelector = new QComboBox(this);
    tournamentSelector->setMinimumWidth(200);
    populateTournamentSelector();

    // Enable context menu for tournament selector
    tournamentSelector->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tournamentSelector, &QComboBox::customContextMenuRequested, this, &MainWindow::onTournamentContextMenuRequested);

    // Add tournament button
    addTournamentButton = new QPushButton("Add Tournament", this);

    addPlayerButton = new QPushButton("Add Player", this);
    resetTournamentButton = new QPushButton("Reset Tournament", this);
    exportResultsButton = new QPushButton("Export Results", this);
    startTournamentButton = new QPushButton("Start Tournament", this); // New button
    endTournamentButton = new QPushButton("End Tournament", this);     // New button
    tiebreakerButton = new QPushButton("Compute Tiebreakers", this);   // New button for tiebreakers
    tiebreakerButton->setVisible(false);                               // Hide by default

    toolBar->addWidget(tournamentSelector);
    toolBar->addWidget(addTournamentButton);
    toolBar->addSeparator();
    toolBar->addWidget(addPlayerButton);
    toolBar->addWidget(resetTournamentButton);
    toolBar->addWidget(exportResultsButton);
    toolBar->addWidget(startTournamentButton); // Add start tournament button
    toolBar->addWidget(endTournamentButton);   // Add end tournament button
    toolBar->addWidget(tiebreakerButton);      // Add tiebreaker button

    connect(tournamentSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onTournamentSelectionChanged);
    connect(addTournamentButton, &QPushButton::clicked, this, &MainWindow::onAddTournamentClicked);
    connect(addPlayerButton, &QPushButton::clicked, this, &MainWindow::onAddPlayerClicked);
    connect(resetTournamentButton, &QPushButton::clicked, this, &MainWindow::onResetTournamentClicked);
    connect(exportResultsButton, &QPushButton::clicked, this, &MainWindow::onExportResultsClicked);
    connect(startTournamentButton, &QPushButton::clicked, this, &MainWindow::onStartTournamentClicked); // Connect start tournament button
    connect(endTournamentButton, &QPushButton::clicked, this, &MainWindow::onEndTournamentClicked);     // Connect end tournament button
    connect(tiebreakerButton, &QPushButton::clicked, this, &MainWindow::onTiebreakerClicked);           // Connect tiebreaker button
}

void MainWindow::onStartTournamentClicked()
{
    // Get current tournament ID
    int tournamentId = getCurrentTournamentId();

    // Check if there are enough players in the current tournament
    QList<Player> players = database->getPlayersForTournament(tournamentId);
    if (players.size() < 3)
    {
        QMessageBox::warning(this, "Insufficient Players",
                             "At least 3 players are required to start a tournament.");
    }
    // Debug output
    qDebug() << "Starting tournament with" << players.size() << "players for tournament ID:" << tournamentId;
    for (const Player &player : players)
    {
        qDebug() << "  Player:" << player.getName();
    }

    // Update tournament status to "active"
    Tournament tournament = database->getTournamentById(tournamentId);
    tournament.setStatus(Tournament::Active);
    tournament.setStartedAt(QDateTime::currentDateTime());

    if (database->updateTournament(tournament))
    {
        // Generate initial pairings based on selected pairing system
        if (settings->getPairingSystem() == "swiss")
        {
            generateSwissPairings(players);
        }
        else
        {
            generateRoundRobinPairings(players);
        }

        // Update UI
        updateMatchTabs();
        updateLeaderboard();

        // Enable tournament end button
        endTournamentButton->setEnabled(true);

        mainStatusBar->showMessage("Tournament started successfully");
    }
    else
    {
        QMessageBox::critical(this, "Error", "Failed to start tournament.");
    }
}

void MainWindow::onEndTournamentClicked()
{
    // Confirm with user before ending tournament
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "End Tournament",
                                  "Are you sure you want to end the tournament? This will finalize all results and save the tournament.",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No)
        return;

    // Get current tournament ID
    int tournamentId = getCurrentTournamentId();

    // Calculate final standings with tiebreakers
    updateLeaderboard();

    // Prepare tournament results data
    QList<TournamentResult> results;
    QList<Player> players = database->getAllPlayers();
    QList<Match> matches = database->getAllMatches(tournamentId);

    // Calculate player statistics
    struct PlayerStats
    {
        int id;
        QString name;
        double points;
        int wins;
        int losses;
        int draws;
        double buchholzCutOne;
        double sonnebornBerger;
        int numberOfWins;
        double cumulativeOpponentScore;
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

        // Calculate basic statistics
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

        // Calculate tiebreaker scores
        stats.buchholzCutOne = Tiebreaker::calculateBuchholzCutOne(player, players, matches);
        stats.sonnebornBerger = Tiebreaker::calculateSonnebornBerger(player, players, matches);
        stats.numberOfWins = Tiebreaker::calculateNumberOfWins(player, matches);
        stats.cumulativeOpponentScore = Tiebreaker::calculateCumulativeOpponentScore(player, players, matches);

        playerStats.append(stats);
    }

    // Sort by points (descending), then by tiebreakers
    std::sort(playerStats.begin(), playerStats.end(), [this](const PlayerStats &a, const PlayerStats &b)
              {
        if (a.points != b.points) {
            return a.points > b.points;
        }
        
        // Apply tiebreaker based on settings
        if (settings->getUseSeriesTiebreakers()) {
            // Use series of tiebreakers
            QList<int> tiebreakers = settings->getSeriesTiebreakers();
            for (int tiebreakerType : tiebreakers) {
                switch (tiebreakerType) {
                    case Tiebreaker::BuchholzCutOne:
                        if (a.buchholzCutOne != b.buchholzCutOne) {
                            return a.buchholzCutOne > b.buchholzCutOne;
                        }
                        break;
                    case Tiebreaker::SonnebornBerger:
                        if (a.sonnebornBerger != b.sonnebornBerger) {
                            return a.sonnebornBerger > b.sonnebornBerger;
                        }
                        break;
                    case Tiebreaker::NumberOfWins:
                        if (a.numberOfWins != b.numberOfWins) {
                            return a.numberOfWins > b.numberOfWins;
                        }
                        break;
                    case Tiebreaker::CumulativeOpponentScore:
                        if (a.cumulativeOpponentScore != b.cumulativeOpponentScore) {
                            return a.cumulativeOpponentScore > b.cumulativeOpponentScore;
                        }
                        break;
                }
            }
        } else {
            // Use single tiebreaker
            int tiebreakerType = settings->getSingleTiebreaker();
            switch (tiebreakerType) {
                case Tiebreaker::BuchholzCutOne:
                    return a.buchholzCutOne > b.buchholzCutOne;
                case Tiebreaker::SonnebornBerger:
                    return a.sonnebornBerger > b.sonnebornBerger;
                case Tiebreaker::NumberOfWins:
                    return a.numberOfWins > b.numberOfWins;
                case Tiebreaker::CumulativeOpponentScore:
                    return a.cumulativeOpponentScore > b.cumulativeOpponentScore;
            }
        }
        
        // If still tied, sort by name
        return a.name < b.name; });

    // Convert to TournamentResult objects
    for (int i = 0; i < playerStats.size(); ++i)
    {
        const PlayerStats &stats = playerStats[i];
        TournamentResult result;
        result.playerId = stats.id;
        result.playerName = stats.name;
        result.finalRank = i + 1;
        result.points = stats.points;
        result.wins = stats.wins;
        result.losses = stats.losses;
        result.draws = stats.draws;
        result.buchholzCutOne = stats.buchholzCutOne;
        result.sonnebornBerger = stats.sonnebornBerger;
        result.numberOfWins = stats.numberOfWins;
        result.cumulativeOpponentScore = stats.cumulativeOpponentScore;
        results.append(result);
    }

    // Save tournament results to permanent table
    if (!database->saveTournamentResults(tournamentId, results))
    {
        QMessageBox::critical(this, "Error", "Failed to save tournament results.");
        return;
    }

    // Update tournament status to "completed"
    Tournament tournament = database->getTournamentById(tournamentId);
    tournament.setStatus(Tournament::Completed);
    tournament.setCompletedAt(QDateTime::currentDateTime());

    if (!database->updateTournament(tournament))
    {
        QMessageBox::critical(this, "Error", "Failed to update tournament status.");
        return;
    }

    // Show final results
    QMessageBox::information(this, "Tournament Completed",
                             "The tournament has been completed successfully.\n\n"
                             "Final results have been calculated and saved.\n"
                             "You can now export the results or start a new tournament.");

    // Offer to export results
    reply = QMessageBox::question(this, "Export Results",
                                  "Would you like to export the tournament results to a CSV file?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
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

    // Disable tournament end button
    endTournamentButton->setEnabled(false);

    // Re-enable player addition/removal
    addPlayerButton->setEnabled(true);
    resetTournamentButton->setEnabled(true);

    // Update status bar
    mainStatusBar->showMessage("Tournament completed successfully");
}

void MainWindow::calculateAndDisplayTiebreakers()
{
    // Get players and matches for current tournament
    QList<Player> players = database->getAllPlayers();
    int tournamentId = getCurrentTournamentId();
    QList<Match> matches = database->getAllMatches(tournamentId);

    // Calculate tiebreaker scores for each player
    struct PlayerTiebreakerStats
    {
        int id;
        QString name;
        double points;
        double buchholzCutOne;
        double sonnebornBerger;
        int numberOfWins;
        double cumulativeOpponentScore;
    };

    QList<PlayerTiebreakerStats> playerStats;
    for (const Player &player : players)
    {
        PlayerTiebreakerStats stats;
        stats.id = player.getId();
        stats.name = player.getName();

        // Calculate basic points
        stats.points = 0.0;
        for (const Match &match : matches)
        {
            if (match.isPlayed())
            {
                if (match.getPlayer1Id() == player.getId())
                {
                    if (match.getResult() == "p1")
                        stats.points += 1.0;
                    else if (match.getResult() == "draw")
                        stats.points += 0.5;
                }
                else if (match.getPlayer2Id() == player.getId())
                {
                    if (match.getResult() == "p2")
                        stats.points += 1.0;
                    else if (match.getResult() == "draw")
                        stats.points += 0.5;
                }
            }
        }

        // Calculate tiebreaker scores
        stats.buchholzCutOne = Tiebreaker::calculateBuchholzCutOne(player, players, matches);
        stats.sonnebornBerger = Tiebreaker::calculateSonnebornBerger(player, players, matches);
        stats.numberOfWins = Tiebreaker::calculateNumberOfWins(player, matches);
        stats.cumulativeOpponentScore = Tiebreaker::calculateCumulativeOpponentScore(player, players, matches);

        playerStats.append(stats);
    }

    // Sort by points (descending), then by selected tiebreaker(s)
    std::sort(playerStats.begin(), playerStats.end(), [this](const PlayerTiebreakerStats &a, const PlayerTiebreakerStats &b)
              {
        if (a.points != b.points) {
            return a.points > b.points;
        }
        
        // Apply tiebreaker based on settings
        if (settings->getUseSeriesTiebreakers()) {
            // Use series of tiebreakers
            QList<int> tiebreakers = settings->getSeriesTiebreakers();
            for (int tiebreakerType : tiebreakers) {
                switch (tiebreakerType) {
                    case Tiebreaker::BuchholzCutOne:
                        if (a.buchholzCutOne != b.buchholzCutOne) {
                            return a.buchholzCutOne > b.buchholzCutOne;
                        }
                        break;
                    case Tiebreaker::SonnebornBerger:
                        if (a.sonnebornBerger != b.sonnebornBerger) {
                            return a.sonnebornBerger > b.sonnebornBerger;
                        }
                        break;
                    case Tiebreaker::NumberOfWins:
                        if (a.numberOfWins != b.numberOfWins) {
                            return a.numberOfWins > b.numberOfWins;
                        }
                        break;
                    case Tiebreaker::CumulativeOpponentScore:
                        if (a.cumulativeOpponentScore != b.cumulativeOpponentScore) {
                            return a.cumulativeOpponentScore > b.cumulativeOpponentScore;
                        }
                        break;
                }
            }
        } else {
            // Use single tiebreaker
            int tiebreakerType = settings->getSingleTiebreaker();
            switch (tiebreakerType) {
                case Tiebreaker::BuchholzCutOne:
                    return a.buchholzCutOne > b.buchholzCutOne;
                case Tiebreaker::SonnebornBerger:
                    return a.sonnebornBerger > b.sonnebornBerger;
                case Tiebreaker::NumberOfWins:
                    return a.numberOfWins > b.numberOfWins;
                case Tiebreaker::CumulativeOpponentScore:
                    return a.cumulativeOpponentScore > b.cumulativeOpponentScore;
            }
        }
        
        // If still tied, sort by name
        return a.name < b.name; });

    // Update player table with tiebreaker-adjusted rankings
    playerTable->setRowCount(playerStats.size());
    for (int i = 0; i < playerStats.size(); ++i)
    {
        const PlayerTiebreakerStats &stats = playerStats[i];

        QTableWidgetItem *rankItem = new QTableWidgetItem(QString::number(i + 1));
        QTableWidgetItem *nameItem = new QTableWidgetItem(stats.name);
        QTableWidgetItem *pointsItem = new QTableWidgetItem(QString::number(stats.points, 'f', 1));

        // Store player ID in the rank item's data
        rankItem->setData(Qt::UserRole, stats.id);

        // Center-align the points column
        pointsItem->setTextAlignment(Qt::AlignCenter);

        playerTable->setItem(i, 0, rankItem);
        playerTable->setItem(i, 1, nameItem);
        playerTable->setItem(i, 2, pointsItem);
    }

    mainStatusBar->showMessage("Tiebreakers calculated and applied");
}

void MainWindow::showTiebreakerSettingsDialog()
{
    // For now, just show a message
    // In a real implementation, this would show a dialog for configuring tiebreakers
    mainStatusBar->showMessage("Showing tiebreaker settings...");

    // In a real implementation, this would:
    // 1. Create a dialog with options for tiebreaker selection
    // 2. Allow user to choose single tiebreaker or series with priority
    // 3. Show tooltips with definitions for each tiebreaker
    // 4. Allow reordering of tiebreaker priorities
    // 5. Save user preferences to settings
    // 6. Include definitions as hover tooltips
    // 7. Have a numbering system for priority ordering
}

void MainWindow::setupPlayerPanel()
{
    playerTable = new QTableWidget(0, 3, this);
    playerTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    playerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Enable context menu
    playerTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(playerTable, &QTableWidget::customContextMenuRequested, this, &MainWindow::onPlayerContextMenuRequested);

    QStringList headers;
    headers << "Rank" << "Player" << "Points";
    playerTable->setHorizontalHeaderLabels(headers);
    playerTable->horizontalHeader()->setStretchLastSection(true);
    playerTable->verticalHeader()->setVisible(false);

    // Center-align the points column header
    playerTable->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignCenter);

    connect(playerTable, &QTableWidget::itemSelectionChanged, this, &MainWindow::onPlayerSelectionChanged);
}

void MainWindow::onPlayerContextMenuRequested(const QPoint &pos)
{
    // Get the item at the position
    QTableWidgetItem *item = playerTable->itemAt(pos);
    if (!item)
        return;

    // Get the row of the selected item
    int row = item->row();

    // Create context menu
    QMenu *contextMenu = new QMenu(this);

    // Add edit action
    QAction *editAction = contextMenu->addAction("Edit Player");
    connect(editAction, &QAction::triggered, this, &MainWindow::onEditPlayer);

    // Add delete action
    QAction *deleteAction = contextMenu->addAction("Delete Player");
    connect(deleteAction, &QAction::triggered, this, &MainWindow::onDeletePlayer);

    // Store the row for use in the action handlers
    editAction->setData(row);
    deleteAction->setData(row);

    // Show context menu
    contextMenu->exec(playerTable->viewport()->mapToGlobal(pos));

    // Clean up
    contextMenu->deleteLater();
}

void MainWindow::onEditPlayer()
{
    // Get the sender action
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action)
        return;

    // Get the row from the action data
    int row = action->data().toInt();

    // Get the player ID from the first column of the row
    QTableWidgetItem *idItem = playerTable->item(row, 0);
    if (!idItem)
        return;

    // Get player ID (stored in the item's data)
    int playerId = idItem->data(Qt::UserRole).toInt();

    // Get current player name
    QTableWidgetItem *nameItem = playerTable->item(row, 1);
    if (!nameItem)
        return;

    QString currentName = nameItem->text();

    // Show input dialog for new name
    bool ok;
    QString newName = QInputDialog::getText(this, "Edit Player",
                                            "Enter new player name:",
                                            QLineEdit::Normal, currentName, &ok);

    if (ok && !newName.isEmpty() && newName != currentName)
    {
        // Check if new name already exists
        QList<Player> players = database->getAllPlayers();
        for (const Player &player : players)
        {
            if (player.getId() != playerId && player.getName() == newName)
            {
                QMessageBox::warning(this, "Duplicate Player",
                                     "A player with this name already exists.");
                return;
            }
        }

        // Update player in database
        Player player = database->getPlayerById(playerId);
        player.setName(newName);

        if (database->updatePlayer(player))
        {
            updatePlayerList();
            updateMatchTabs();
            mainStatusBar->showMessage("Player updated successfully");
        }
        else
        {
            QMessageBox::critical(this, "Error", "Failed to update player.");
        }
    }
}

void MainWindow::onDeletePlayer()
{
    // Get the sender action
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action)
        return;

    // Get the row from the action data
    int row = action->data().toInt();

    // Get the player ID from the first column of the row
    QTableWidgetItem *idItem = playerTable->item(row, 0);
    if (!idItem)
        return;

    // Get player ID (stored in the item's data)
    int playerId = idItem->data(Qt::UserRole).toInt();

    // Get player name
    QTableWidgetItem *nameItem = playerTable->item(row, 1);
    if (!nameItem)
        return;

    QString playerName = nameItem->text();

    // Confirm deletion
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Delete",
                                  QString("Are you sure you want to delete player '%1'?").arg(playerName),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        // Delete player from database
        if (database->deletePlayer(playerId))
        {
            updatePlayerList();
            updateMatchTabs();
            mainStatusBar->showMessage("Player deleted successfully");
        }
        else
        {
            QMessageBox::critical(this, "Error", "Failed to delete player.");
        }
    }
}

void MainWindow::setupMatchPanel()
{
    roundTabs = new QTabWidget(this);
    roundTabs->setTabsClosable(false);
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
        // Check if player already exists in the current tournament
        int tournamentId = getCurrentTournamentId();
        QList<Player> players = database->getPlayersForTournament(tournamentId);
        for (const Player &player : players)
        {
            if (player.getName() == name)
            {
                QMessageBox::warning(this, "Duplicate Player",
                                     "A player with this name already exists in this tournament.");
                return;
            }
        }

        // Add player to database and associate with current tournament
        int playerId = database->addPlayerToTournament(name, tournamentId);
        if (playerId == -1)
        {
            QMessageBox::critical(this, "Error", "Failed to add player to database.");
            return;
        }

        updatePlayerList();
        updateMatchTabs();
        mainStatusBar->showMessage("Player added successfully");
    }
}

void MainWindow::onResetTournamentClicked()
{
    // Create a dialog with options
    QMessageBox msgBox;
    msgBox.setWindowTitle("Reset Tournament");
    msgBox.setText("This will clear all match results. What would you like to do?");
    msgBox.setIcon(QMessageBox::Question);

    QPushButton *resetOnlyButton = msgBox.addButton("Reset Tournament Only", QMessageBox::ActionRole);
    QPushButton *resetAndRemoveButton = msgBox.addButton("Reset and Remove All Players", QMessageBox::ActionRole);
    QPushButton *cancelButton = msgBox.addButton(QMessageBox::Cancel);

    msgBox.exec();

    QPushButton *clickedButton = qobject_cast<QPushButton *>(msgBox.clickedButton());

    int tournamentId = getCurrentTournamentId();

    if (clickedButton == resetOnlyButton)
    {
        // Reset tournament only
        if (database->resetDatabase(tournamentId))
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
    else if (clickedButton == resetAndRemoveButton)
    {
        // Reset tournament and remove all players
        QMessageBox::StandardButton confirmReply;
        confirmReply = QMessageBox::question(this, "Confirm Reset and Remove",
                                             "This will clear all match results AND remove all players. Continue?",
                                             QMessageBox::Yes | QMessageBox::No);

        if (confirmReply == QMessageBox::Yes)
        {
            // Reset database
            if (database->resetDatabase(tournamentId))
            {
                // Delete all players
                QList<Player> players = database->getAllPlayers();
                for (const Player &player : players)
                {
                    database->deletePlayer(player.getId());
                }

                updatePlayerList();
                updateMatchTabs();
                mainStatusBar->showMessage("Tournament and players reset successfully");
            }
            else
            {
                QMessageBox::critical(this, "Error", "Failed to reset tournament.");
            }
        }
    }
    // If cancel button is clicked, do nothing
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

    // Get player statistics for current tournament
    QList<Player> players = database->getAllPlayers();
    int tournamentId = getCurrentTournamentId();
    QList<Match> matches = database->getAllMatches(tournamentId);

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
                // Store the current tab index before updating tabs
                int currentTabIndex = roundTabs->currentIndex();

                updateMatchTabs();
                updateLeaderboard();

                // Restore the tab index after updating tabs
                if (currentTabIndex >= 0 && currentTabIndex < roundTabs->count())
                {
                    roundTabs->setCurrentIndex(currentTabIndex);
                }

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
    int tournamentId = getCurrentTournamentId();
    QList<Player> players = database->getPlayersForTournament(tournamentId);
    QList<Match> matches = database->getAllMatches(tournamentId);

    // Calculate points for each player
    struct PlayerStats
    {
        int id;
        QString name;
        double points;
        int wins;
        int losses;
        int draws;
        // Tiebreaker scores
        double buchholzCutOne;
        double sonnebornBerger;
        int numberOfWins;
        double cumulativeOpponentScore;
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
        // Initialize tiebreaker scores
        stats.buchholzCutOne = 0.0;
        stats.sonnebornBerger = 0.0;
        stats.numberOfWins = 0;
        stats.cumulativeOpponentScore = 0.0;

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

        // Calculate tiebreaker scores if there are ties
        // For now, we'll just set them to the player's points
        stats.buchholzCutOne = stats.points;
        stats.sonnebornBerger = stats.points;
        stats.numberOfWins = stats.wins;
        stats.cumulativeOpponentScore = stats.points;

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

        // Store player ID in the rank item's data
        rankItem->setData(Qt::UserRole, stats.id);

        // Center-align the points column
        pointsItem->setTextAlignment(Qt::AlignCenter);

        playerTable->setItem(i, 0, rankItem);
        playerTable->setItem(i, 1, nameItem);
        playerTable->setItem(i, 2, pointsItem);
    }
}

void MainWindow::updateLeaderboard()
{
    updatePlayerList();

    // Check for ties and show/hide tiebreaker button
    QList<Player> players = database->getAllPlayers();
    int tournamentId = getCurrentTournamentId();
    QList<Match> matches = database->getAllMatches(tournamentId);

    // Calculate points for each player
    struct PlayerStats
    {
        int id;
        QString name;
        double points;
    };

    QList<PlayerStats> playerStats;
    for (const Player &player : players)
    {
        PlayerStats stats;
        stats.id = player.getId();
        stats.name = player.getName();
        stats.points = 0.0;

        for (const Match &match : matches)
        {
            if (match.isPlayed())
            {
                if (match.getPlayer1Id() == player.getId())
                {
                    if (match.getResult() == "p1")
                    {
                        stats.points += 1.0;
                    }
                    else if (match.getResult() == "draw")
                    {
                        stats.points += 0.5;
                    }
                }
                else if (match.getPlayer2Id() == player.getId())
                {
                    if (match.getResult() == "p2")
                    {
                        stats.points += 1.0;
                    }
                    else if (match.getResult() == "draw")
                    {
                        stats.points += 0.5;
                    }
                }
            }
        }

        playerStats.append(stats);
    }

    // Check if there are ties
    bool hasTies = false;
    for (int i = 0; i < playerStats.size() - 1; i++)
    {
        for (int j = i + 1; j < playerStats.size(); j++)
        {
            if (playerStats[i].points == playerStats[j].points)
            {
                hasTies = true;
                break;
            }
        }
        if (hasTies)
            break;
    }

    // Show/hide tiebreaker button
    if (tiebreakerButton)
    {
        tiebreakerButton->setVisible(hasTies);
    }
}

// Swiss Pairing structures and functions
struct SwissPlayer
{
    int id;
    QString name;
    double points;
    int wins;
    int losses;
    int draws;
    QList<int> opponents; // List of opponent IDs
};

struct SwissPair
{
    int p1Id;
    int p2Id;
    QString p1Name;
    QString p2Name;
};

// Calculate player standings for Swiss pairing
QList<SwissPlayer> calculateStandings(const QList<Player> &players, const QList<Match> &matches)
{
    QList<SwissPlayer> standings;

    // Initialize players with zero stats
    for (const Player &player : players)
    {
        SwissPlayer swissPlayer;
        swissPlayer.id = player.getId();
        swissPlayer.name = player.getName();
        swissPlayer.points = 0.0;
        swissPlayer.wins = 0;
        swissPlayer.losses = 0;
        swissPlayer.draws = 0;

        // Calculate stats from matches
        for (const Match &match : matches)
        {
            if (match.isPlayed())
            {
                if (match.getPlayer1Id() == player.getId())
                {
                    swissPlayer.opponents.append(match.getPlayer2Id());
                    if (match.getResult() == "p1")
                    {
                        swissPlayer.wins++;
                        swissPlayer.points += 1.0;
                    }
                    else if (match.getResult() == "p2")
                    {
                        swissPlayer.losses++;
                    }
                    else if (match.getResult() == "draw")
                    {
                        swissPlayer.draws++;
                        swissPlayer.points += 0.5;
                    }
                }
                else if (match.getPlayer2Id() == player.getId())
                {
                    swissPlayer.opponents.append(match.getPlayer1Id());
                    if (match.getResult() == "p2")
                    {
                        swissPlayer.wins++;
                        swissPlayer.points += 1.0;
                    }
                    else if (match.getResult() == "p1")
                    {
                        swissPlayer.losses++;
                    }
                    else if (match.getResult() == "draw")
                    {
                        swissPlayer.draws++;
                        swissPlayer.points += 0.5;
                    }
                }
            }
        }

        standings.append(swissPlayer);
    }

    // Sort by points (descending), then by name (ascending)
    std::sort(standings.begin(), standings.end(), [](const SwissPlayer &a, const SwissPlayer &b)
              {
        if (a.points != b.points)
        {
            return a.points > b.points;
        }
        return a.name < b.name; });

    return standings;
}

// Check if two players have already played against each other
bool havePlayed(const SwissPlayer &p1, const SwissPlayer &p2)
{
    return p1.opponents.contains(p2.id) || p2.opponents.contains(p1.id);
}

// Generate Swiss pairings for one round
QList<SwissPair> generateSwissRound(QList<SwissPlayer> &players)
{
    QList<SwissPair> pairs;

    // Pair players from top to bottom, avoiding rematches
    for (int i = 0; i < players.size(); ++i)
    {
        // Skip if player is already paired
        if (players[i].id == -1)
            continue;

        // Look for an opponent who hasn't played this player
        for (int j = i + 1; j < players.size(); ++j)
        {
            // Skip if player is already paired
            if (players[j].id == -1)
                continue;

            // Check if they've already played
            if (!havePlayed(players[i], players[j]))
            {
                // Create pair
                SwissPair pair;
                pair.p1Id = players[i].id;
                pair.p2Id = players[j].id;
                pair.p1Name = players[i].name;
                pair.p2Name = players[j].name;
                pairs.append(pair);

                // Mark players as paired
                players[i].id = -1;
                players[j].id = -1;
                break;
            }
        }
    }

    return pairs;
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

void MainWindow::generateRoundRobinPairings(const QList<Player> &players)
{
    int tournamentId = getCurrentTournamentId();
    QList<Match> matches = database->getAllMatches(tournamentId);

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

    // Clear existing matches for this tournament
    database->deleteMatchesForTournament(tournamentId);

    // Reset match ID sequence for this tournament to ensure match IDs start at 1
    database->resetMatchIdSequence(tournamentId);

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
                int tournamentId = getCurrentTournamentId();
                database->addMatch(tournamentId, roundNumber, p1Id, p2Id);
            }
        }
        roundNumber++;
    }
}

void MainWindow::generateSwissPairings(const QList<Player> &players)
{
    int tournamentId = getCurrentTournamentId();
    QList<Match> matches = database->getAllMatches(tournamentId);

    // We need at least 3 players to generate pairings
    if (players.size() < 3)
    {
        return;
    }

    // Calculate current standings
    QList<SwissPlayer> standings = calculateStandings(players, matches);

    // Determine round number (number of matches each player has played + 1)
    int maxRounds = 0;
    for (const SwissPlayer &player : standings)
    {
        int playerRounds = player.wins + player.losses + player.draws;
        if (playerRounds > maxRounds)
        {
            maxRounds = playerRounds;
        }
    }
    int currentRound = maxRounds + 1;

    // Ensure we don't exceed the maximum number of rounds
    if (currentRound > settings->getSwissRounds())
    {
        mainStatusBar->showMessage(QString("Maximum number of Swiss rounds (%1) reached").arg(settings->getSwissRounds()));
        return;
    }

    // Generate pairings for current round
    QList<SwissPair> pairs = generateSwissRound(standings);

    // Clear existing matches for this tournament
    database->deleteMatchesForTournament(tournamentId);

    // Reset match ID sequence for this tournament to ensure match IDs start at 1
    database->resetMatchIdSequence(tournamentId);

    // Create matches in database
    for (const SwissPair &pair : pairs)
    {
        database->addMatch(tournamentId, currentRound, pair.p1Id, pair.p2Id);
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

    // Get all matches for the current tournament grouped by round
    int tournamentId = getCurrentTournamentId();
    QList<Match> matches = database->getAllMatches(tournamentId);

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
    
    // Add "+" button for Swiss tournaments if conditions are met
    Tournament tournament = database->getTournamentById(tournamentId);
    if (tournament.getPairingSystem() == "swiss" && tournament.getStatus() == Tournament::Active)
    {
        // Check if we can add another round:
        // 1. Current round count is within the limit set in swissRounds
        // 2. All matches in the last completed round have results (or it's the first round)
        bool canAddRound = true;
        int maxRound = 0;
        
        // Find the maximum round number
        if (!rounds.isEmpty())
        {
            maxRound = rounds.last();
        }
        
        // Check if we've reached the Swiss rounds limit
        if (maxRound >= settings->getSwissRounds())
        {
            canAddRound = false;
        }
        else if (maxRound > 0)
        {
            // Check if all matches in the last completed round have results
            QList<Match> lastRoundMatches = matchesByRound[maxRound];
            for (const Match &match : lastRoundMatches)
            {
                if (!match.isPlayed())
                {
                    canAddRound = false;
                    break;
                }
            }
        }
        
        // Add "+" button if conditions are met
        if (canAddRound)
        {
            QWidget *plusWidget = new QWidget();
            QVBoxLayout *layout = new QVBoxLayout(plusWidget);
            layout->setAlignment(Qt::AlignCenter);
            
            QPushButton *plusButton = new QPushButton("+");
            plusButton->setFixedSize(50, 50);
            plusButton->setStyleSheet("font-size: 20px; font-weight: bold;");
            connect(plusButton, &QPushButton::clicked, this, &MainWindow::onAddSwissRoundClicked);
            
            layout->addWidget(plusButton);
            roundTabs->addTab(plusWidget, "+");
        }
    }
}

int MainWindow::getCurrentTournamentId()
{
    // Get selected tournament ID from combobox
    int currentIndex = tournamentSelector->currentIndex();
    if (currentIndex >= 0)
    {
        return tournamentSelector->itemData(currentIndex).toInt();
    }

    // If no tournament is selected, get all tournaments
    QList<Tournament> tournaments = database->getAllTournaments();

    // If there are no tournaments, create a new one
    if (tournaments.isEmpty())
    {
        int tournamentId = database->addTournament("Default Tournament");
        return tournamentId;
    }

    // Return the ID of the first tournament
    return tournaments.first().getId();
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

void MainWindow::showSettingsDialog()
{
    // Create settings dialog
    QDialog settingsDialog(this);
    settingsDialog.setWindowTitle("Preferences");
    settingsDialog.resize(500, 400);

    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(&settingsDialog);

    // Create pairing system group box
    QGroupBox *pairingGroupBox = new QGroupBox("Pairing System", &settingsDialog);
    QVBoxLayout *pairingLayout = new QVBoxLayout(pairingGroupBox);

    // Create radio buttons for pairing systems
    QRadioButton *roundRobinRadio = new QRadioButton("Round Robin", pairingGroupBox);
    QRadioButton *swissRadio = new QRadioButton("Swiss System", pairingGroupBox);

    // Create spin box for Swiss rounds
    QSpinBox *swissRoundsSpinBox = new QSpinBox(pairingGroupBox);
    swissRoundsSpinBox->setRange(1, 20); // Allow 1-20 rounds
    swissRoundsSpinBox->setValue(settings->getSwissRounds());
    swissRoundsSpinBox->setPrefix("Rounds: ");

    // Set current selection based on settings
    if (settings->getPairingSystem() == "round_robin")
    {
        roundRobinRadio->setChecked(true);
        swissRoundsSpinBox->setEnabled(false); // Disable rounds setting for round robin
    }
    else
    {
        swissRadio->setChecked(true);
        swissRoundsSpinBox->setEnabled(true); // Enable rounds setting for Swiss
    }

    // Connect radio buttons to enable/disable Swiss rounds setting
    connect(roundRobinRadio, &QRadioButton::toggled, [swissRoundsSpinBox](bool checked)
            { swissRoundsSpinBox->setEnabled(!checked); });

    pairingLayout->addWidget(roundRobinRadio);
    pairingLayout->addWidget(swissRadio);
    pairingLayout->addWidget(swissRoundsSpinBox);
    pairingLayout->addStretch();

    // Create tiebreaker system group box
    QGroupBox *tiebreakerGroupBox = new QGroupBox("Tiebreaker System", &settingsDialog);
    QVBoxLayout *tiebreakerLayout = new QVBoxLayout(tiebreakerGroupBox);

    // Create radio buttons for tiebreaker systems
    QRadioButton *singleTiebreakerRadio = new QRadioButton("Single Tiebreaker", tiebreakerGroupBox);
    QRadioButton *seriesTiebreakerRadio = new QRadioButton("Series of Tiebreakers", tiebreakerGroupBox);

    // Set current selection based on settings
    if (settings->getUseSeriesTiebreakers())
    {
        seriesTiebreakerRadio->setChecked(true);
    }
    else
    {
        singleTiebreakerRadio->setChecked(true);
    }

    // Create combo box for single tiebreaker selection
    QComboBox *tiebreakerComboBox = new QComboBox(tiebreakerGroupBox);
    tiebreakerComboBox->addItem("Buchholz Cut One", Tiebreaker::BuchholzCutOne);
    tiebreakerComboBox->setItemData(0, "This system adds up the scores of all your opponents and then subtracts the score of the lowest-scoring opponent. It rewards players who have faced tougher opponents.", Qt::ToolTipRole);

    tiebreakerComboBox->addItem("Sonneborn–Berger Score", Tiebreaker::SonnebornBerger);
    tiebreakerComboBox->setItemData(1, "This method gives more credit to a player for wins or draws against strong opponents.", Qt::ToolTipRole);

    tiebreakerComboBox->addItem("Direct Encounter", Tiebreaker::DirectEncounter);
    tiebreakerComboBox->setItemData(2, "If tied players have already played each other, the results of that game(s) can break the tie, with the winner being ranked higher.", Qt::ToolTipRole);

    tiebreakerComboBox->addItem("Number of Wins", Tiebreaker::NumberOfWins);
    tiebreakerComboBox->setItemData(3, "The player with the higher total number of wins, or wins with the black pieces, may be ranked higher.", Qt::ToolTipRole);

    tiebreakerComboBox->addItem("Cumulative Opponent's Score", Tiebreaker::CumulativeOpponentScore);
    tiebreakerComboBox->setItemData(4, "Sums up the total scores of all opponents a player has faced.", Qt::ToolTipRole);

    // Set current selection based on settings
    int currentTiebreaker = settings->getSingleTiebreaker();
    for (int i = 0; i < tiebreakerComboBox->count(); ++i)
    {
        if (tiebreakerComboBox->itemData(i).toInt() == currentTiebreaker)
        {
            tiebreakerComboBox->setCurrentIndex(i);
            break;
        }
    }

    // Create label with tiebreaker definitions as tooltips
    QLabel *tiebreakerInfoLabel = new QLabel("<a href='#'>Tiebreaker Definitions</a>", tiebreakerGroupBox);
    tiebreakerInfoLabel->setOpenExternalLinks(false);

    // Connect label click to show definitions
    connect(tiebreakerInfoLabel, &QLabel::linkActivated, [this]()
            { QMessageBox::information(this, "Tiebreaker Definitions",
                                       "<b>Buchholz Cut One:</b> This system adds up the scores of all your opponents and then subtracts the score of the lowest-scoring opponent. It rewards players who have faced tougher opponents.\n\n"
                                       "<b>Sonneborn–Berger Score:</b> This method gives more credit to a player for wins or draws against strong opponents.\n\n"
                                       "<b>Direct Encounter:</b> If tied players have already played each other, the results of that game(s) can break the tie, with the winner being ranked higher.\n\n"
                                       "<b>Number of Wins:</b> The player with the higher total number of wins, or wins with the black pieces, may be ranked higher.\n\n"
                                       "<b>Cumulative Opponent's Score:</b> Sums up the total scores of all opponents a player has faced."); });

    tiebreakerLayout->addWidget(singleTiebreakerRadio);
    tiebreakerLayout->addWidget(tiebreakerComboBox);
    tiebreakerLayout->addWidget(seriesTiebreakerRadio);
    tiebreakerLayout->addWidget(tiebreakerInfoLabel);
    tiebreakerLayout->addStretch();

    // Create theme group box
    QGroupBox *themeGroupBox = new QGroupBox("Theme", &settingsDialog);
    QVBoxLayout *themeLayout = new QVBoxLayout(themeGroupBox);

    // Create combo box for themes
    QComboBox *themeComboBox = new QComboBox(themeGroupBox);
    themeComboBox->addItem("Fusion Dark", "fusion_dark");
    themeComboBox->addItem("Fusion Light", "fusion_light");
    themeComboBox->addItem("Fusion White", "fusion_white");
    themeComboBox->addItem("Custom QSS", "custom");

    // Set current selection based on settings
    QString currentTheme = settings->getTheme();
    for (int i = 0; i < themeComboBox->count(); ++i)
    {
        if (themeComboBox->itemData(i).toString() == currentTheme)
        {
            themeComboBox->setCurrentIndex(i);
            break;
        }
    }

    themeLayout->addWidget(themeComboBox);
    themeLayout->addStretch();

    // Create icon set group box
    QGroupBox *iconGroupBox = new QGroupBox("Icon Set", &settingsDialog);
    QVBoxLayout *iconLayout = new QVBoxLayout(iconGroupBox);

    // Create radio buttons for icon sets
    QRadioButton *defaultIconsRadio = new QRadioButton("Default", iconGroupBox);
    QRadioButton *modernIconsRadio = new QRadioButton("Modern", iconGroupBox);

    // Set current selection based on settings
    QString currentIconSet = settings->getIconSet();
    if (currentIconSet == "default")
    {
        defaultIconsRadio->setChecked(true);
    }
    else if (currentIconSet == "modern")
    {
        modernIconsRadio->setChecked(true);
    }

    iconLayout->addWidget(defaultIconsRadio);
    iconLayout->addWidget(modernIconsRadio);
    iconLayout->addStretch();

    // Create auto-start tournament checkbox
    QCheckBox *autoStartCheckbox = new QCheckBox("Auto-start tournament when enough players are added", &settingsDialog);
    autoStartCheckbox->setChecked(settings->getAutoStartTournament());

    // Add widgets to main layout
    layout->addWidget(pairingGroupBox);
    layout->addWidget(tiebreakerGroupBox);
    layout->addWidget(themeGroupBox);
    layout->addWidget(iconGroupBox);
    layout->addWidget(autoStartCheckbox);
    layout->addStretch();

    // Create buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                       Qt::Horizontal, &settingsDialog);
    layout->addWidget(buttonBox);

    // Connect buttons
    connect(buttonBox, &QDialogButtonBox::accepted, &settingsDialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &settingsDialog, &QDialog::reject);

    // Show dialog and process result
    if (settingsDialog.exec() == QDialog::Accepted)
    {
        // Update settings based on user selections
        if (roundRobinRadio->isChecked())
        {
            settings->setPairingSystem("round_robin");
        }
        else
        {
            settings->setPairingSystem("swiss");
            settings->setSwissRounds(swissRoundsSpinBox->value()); // Save Swiss rounds setting
        }

        // Update tiebreaker settings
        settings->setUseSeriesTiebreakers(seriesTiebreakerRadio->isChecked());
        if (singleTiebreakerRadio->isChecked())
        {
            int selectedTiebreaker = tiebreakerComboBox->itemData(tiebreakerComboBox->currentIndex()).toInt();
            settings->setSingleTiebreaker(selectedTiebreaker);
        }

        // Update theme settings
        QString selectedTheme = themeComboBox->itemData(themeComboBox->currentIndex()).toString();
        settings->setTheme(selectedTheme);

        if (selectedTheme == "fusion_dark")
        {
            setFusionDark(*app);
        }
        else if (selectedTheme == "fusion_light")
        {
            setFusionLight(*app);
        }
        else if (selectedTheme == "fusion_white")
        {
            setFusionWhite(*app);
        }
        else if (selectedTheme == "custom")
        {
            setCustomQss(*app);
        }

        // Update icon settings
        if (defaultIconsRadio->isChecked())
        {
            settings->setIconSet("default");
            setDefaultIcons();
        }
        else if (modernIconsRadio->isChecked())
        {
            settings->setIconSet("modern");
            setModernIcons();
        }

        settings->setAutoStartTournament(autoStartCheckbox->isChecked());

        // Save settings
        settings->save();

        mainStatusBar->showMessage("Settings saved successfully");
    }
}

void MainWindow::onTiebreakerClicked()
{
    calculateAndDisplayTiebreakers();
}

void MainWindow::onAddTournamentClicked()
{
    // Show input dialog for tournament name
    bool ok;
    QString name = QInputDialog::getText(this, "Add Tournament",
                                         "Enter tournament name:", QLineEdit::Normal,
                                         "", &ok);

    if (ok && !name.isEmpty())
    {
        // Add tournament to database
        int tournamentId = database->addTournament(name);
        if (tournamentId == -1)
        {
            QMessageBox::critical(this, "Error", "Failed to add tournament to database.");
            return;
        }

        // Update tournament selector
        populateTournamentSelector();

        // Select the newly added tournament
        for (int i = 0; i < tournamentSelector->count(); ++i)
        {
            if (tournamentSelector->itemData(i).toInt() == tournamentId)
            {
                tournamentSelector->setCurrentIndex(i);
                break;
            }
        }

        mainStatusBar->showMessage("Tournament added successfully");
    }
}

void MainWindow::onTournamentSelectionChanged(int index)
{
    // Get selected tournament ID
    int tournamentId = tournamentSelector->itemData(index).toInt();

    // Update UI based on tournament selection
    updatePlayerList();
    updateMatchTabs();

    // Update tournament status in UI
    Tournament tournament = database->getTournamentById(tournamentId);
    if (tournament.getStatus() == Tournament::Active)
    {
        endTournamentButton->setEnabled(true);
        addPlayerButton->setEnabled(false);
        resetTournamentButton->setEnabled(false);
    }
    else if (tournament.getStatus() == Tournament::Completed)
    {
        endTournamentButton->setEnabled(false);
        addPlayerButton->setEnabled(true);
        resetTournamentButton->setEnabled(true);
    }
    else
    {
        endTournamentButton->setEnabled(false);
        addPlayerButton->setEnabled(true);
        resetTournamentButton->setEnabled(true);
    }

    mainStatusBar->showMessage(QString("Selected tournament: %1").arg(tournament.getName()));
}

void MainWindow::onTournamentContextMenuRequested(const QPoint &pos)
{
    // Get the item at the position
    int index = tournamentSelector->currentIndex();
    if (index < 0)
        return;

    // Get the tournament ID
    int tournamentId = tournamentSelector->itemData(index).toInt();

    // Don't allow editing/deleting the last tournament
    QList<Tournament> tournaments = database->getAllTournaments();
    if (tournaments.size() <= 1)
    {
        QMessageBox::information(this, "Cannot Modify Tournament",
                                 "You cannot edit or delete the last tournament. At least one tournament must exist.");
        return;
    }

    // Create context menu
    QMenu *contextMenu = new QMenu(this);

    // Add edit action
    QAction *editAction = contextMenu->addAction("Edit Tournament");
    connect(editAction, &QAction::triggered, this, &MainWindow::onEditTournamentClicked);

    // Add delete action
    QAction *deleteAction = contextMenu->addAction("Delete Tournament");
    connect(deleteAction, &QAction::triggered, this, &MainWindow::onDeleteTournamentClicked);

    // Store the tournament ID for use in the action handlers
    editAction->setData(tournamentId);
    deleteAction->setData(tournamentId);

    // Show context menu
    contextMenu->exec(tournamentSelector->mapToGlobal(pos));

    // Clean up
    contextMenu->deleteLater();
}

void MainWindow::onEditTournamentClicked()
{
    // Get the sender action
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action)
        return;

    // Get the tournament ID from the action data
    int tournamentId = action->data().toInt();

    // Get current tournament name
    Tournament tournament = database->getTournamentById(tournamentId);
    QString currentName = tournament.getName();

    // Show input dialog for new name
    bool ok;
    QString newName = QInputDialog::getText(this, "Edit Tournament",
                                            "Enter new tournament name:",
                                            QLineEdit::Normal, currentName, &ok);

    if (ok && !newName.isEmpty() && newName != currentName)
    {
        // Check if new name already exists
        QList<Tournament> tournaments = database->getAllTournaments();
        for (const Tournament &t : tournaments)
        {
            if (t.getId() != tournamentId && t.getName() == newName)
            {
                QMessageBox::warning(this, "Duplicate Tournament",
                                     "A tournament with this name already exists.");
                return;
            }
        }

        // Update tournament in database
        tournament.setName(newName);
        if (database->updateTournament(tournament))
        {
            // Update tournament selector
            populateTournamentSelector();

            // Select the edited tournament
            for (int i = 0; i < tournamentSelector->count(); ++i)
            {
                if (tournamentSelector->itemData(i).toInt() == tournamentId)
                {
                    tournamentSelector->setCurrentIndex(i);
                    break;
                }
            }

            mainStatusBar->showMessage("Tournament updated successfully");
        }
        else
        {
            QMessageBox::critical(this, "Error", "Failed to update tournament.");
        }
    }
}

void MainWindow::onDeleteTournamentClicked()
{
    // Get the sender action
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action)
        return;

    // Get the tournament ID from the action data
    int tournamentId = action->data().toInt();

    // Get tournament name
    Tournament tournament = database->getTournamentById(tournamentId);
    QString tournamentName = tournament.getName();

    // Confirm deletion
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Delete",
                                  QString("Are you sure you want to delete tournament '%1'? This will also delete all associated players, matches, and results.").arg(tournamentName),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        // Delete tournament from database
        if (database->deleteTournament(tournamentId))
        {
            // Update tournament selector
            populateTournamentSelector();

            // Select the first tournament if any exist
            if (tournamentSelector->count() > 0)
            {
                tournamentSelector->setCurrentIndex(0);
                onTournamentSelectionChanged(0);
            }

            mainStatusBar->showMessage("Tournament deleted successfully");
        }
        else
        {
            QMessageBox::critical(this, "Error", "Failed to delete tournament.");
        }
    }
}

void MainWindow::populateTournamentSelector()
{
    // Clear existing items
    tournamentSelector->clear();

    // Get all tournaments from database
    QList<Tournament> tournaments = database->getAllTournaments();

    // Add tournaments to selector
    for (const Tournament &tournament : tournaments)
    {
        tournamentSelector->addItem(tournament.getName(), tournament.getId());
    }

    // Select the first tournament if any exist
    if (!tournaments.isEmpty())
    {
        tournamentSelector->setCurrentIndex(0);
    }
}

void MainWindow::onAddSwissRoundClicked()
{
    // Get current tournament ID
    int tournamentId = getCurrentTournamentId();
    
    // Get players for this tournament
    QList<Player> players = database->getPlayersForTournament(tournamentId);
    
    // Generate next Swiss round
    generateSwissPairings(players);
    
    // Update UI
    updateMatchTabs();
    updateLeaderboard();
    
    mainStatusBar->showMessage("Added new Swiss round");
}