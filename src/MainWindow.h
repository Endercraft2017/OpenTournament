#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QTabWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QPushButton>
#include <QAction>
#include <QList>
#include <QCloseEvent>
#include <QPoint>
#include <QMenu>
#include <QInputDialog>
#include <QComboBox>
#include <QDesktopServices>
#include <QUrl>

#include "Database.h"
#include "Player.h"
#include "Match.h"
#include "Tournament.h"
#include "Settings.h"
#include "Tiebreaker.h"

class QApplication; // Forward declaration

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddPlayerClicked();
    void onResetTournamentClicked();
    void onExportResultsClicked();
    void onMatchResultClicked();
    void onEditMatchClicked();
    void onPlayerSelectionChanged();
    void onPlayerContextMenuRequested(const QPoint &pos);
    void onEditPlayer();
    void onDeletePlayer();
    void onStartTournamentClicked();                          // New slot for start tournament button
    void onEndTournamentClicked();                            // New slot for end tournament button
    void onTiebreakerClicked();                               // New slot for tiebreaker button
    void onAddTournamentClicked();                            // New slot for add tournament button
    void onTournamentSelectionChanged(int index);             // New slot for tournament selection
    void onTournamentContextMenuRequested(const QPoint &pos); // New slot for tournament context menu
    void onEditTournamentClicked();                           // New slot for editing tournament
    void onDeleteTournamentClicked();                         // New slot for deleting tournament
    void onAddSwissRoundClicked();                            // New slot for adding Swiss round

    // Theme and icon slots
    void onFusionDarkSelected();
    void onFusionLightSelected();
    void onFusionWhiteSelected();
    void onAbyssThemeSelected();
    void onDarkHighContrastThemeSelected();
    void onDefaultIconsSelected();
    void onModernIconsSelected();

private:
    // UI components
    QMenuBar *menuBar;
    QToolBar *toolBar;
    QTableWidget *playerTable;
    QTabWidget *roundTabs;
    QStatusBar *mainStatusBar;
    QComboBox *tournamentSelector; // New tournament selector

    // Actions
    QAction *exportResultsAction;
    QAction *exitAction;
    QAction *aboutAction;
    QAction *settingsAction; // New settings action

    // Theme actions
    QAction *fusionDarkAction;
    QAction *fusionLightAction;
    QAction *fusionWhiteAction;
    QAction *abyssThemeAction;
    QAction *darkHighContrastThemeAction;
    QAction *defaultIconsAction;
    QAction *modernIconsAction;

    // Buttons
    QPushButton *addPlayerButton;
    QPushButton *resetTournamentButton;
    QPushButton *exportResultsButton;
    QPushButton *startTournamentButton; // New start tournament button
    QPushButton *endTournamentButton;   // New end tournament button
    QPushButton *tiebreakerButton;      // New tiebreaker button
    QPushButton *addTournamentButton;   // New add tournament button

    // Data managers
    Database *database;
    Settings *settings; // New settings manager

    // Application reference for theme changes
    QApplication *app;

    // Helper methods
    void setupUI();
    void setupMenu();
    void setupToolbar();
    void setupPlayerPanel();
    void setupMatchPanel();
    void updatePlayerList();
    void updateMatchTabs();
    void showAddPlayerDialog();
    void showConfirmationDialog(const QString &message);
    void generateRoundRobinPairings(const QList<Player> &players); // Modified to accept players parameter
    void generateSwissPairings(const QList<Player> &players);      // Modified to accept players parameter

    void updateLeaderboard();
    void exportToCSV(const QString &filename);
    void showSettingsDialog();             // New method for showing settings dialog
    void calculateAndDisplayTiebreakers(); // New method for calculating tiebreakers
    void showTiebreakerSettingsDialog();   // New method for showing tiebreaker settings dialog
    void populateTournamentSelector();     // New method for populating tournament selector

    // Theme and icon methods
    void setDefaultIcons();
    void setModernIcons();

    // Event handlers
    void closeEvent(QCloseEvent *event) override;

    // Helper methods
    int getCurrentTournamentId();
};

#endif // MAINWINDOW_H