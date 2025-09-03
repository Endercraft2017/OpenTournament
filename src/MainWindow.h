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

#include "Database.h"
#include "Player.h"
#include "Match.h"
#include "Tournament.h"

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

private:
    // UI components
    QMenuBar *menuBar;
    QToolBar *toolBar;
    QTableWidget *playerTable;
    QTabWidget *roundTabs;
    QStatusBar *mainStatusBar;

    // Actions
    QAction *newTournamentAction;
    QAction *exportResultsAction;
    QAction *exitAction;
    QAction *aboutAction;

    // Buttons
    QPushButton *addPlayerButton;
    QPushButton *resetTournamentButton;
    QPushButton *exportResultsButton;

    // Data managers
    Database *database;

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
    void generateRoundRobinPairings();
    void updateLeaderboard();
    void exportToCSV(const QString &filename);

    // Event handlers
    void closeEvent(QCloseEvent *event) override;
};

#endif // MAINWINDOW_H