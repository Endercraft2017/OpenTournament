#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QMap>
#include <QList>

class Settings
{
public:
    // Constructor
    Settings();

    // Getters
    QString getPairingSystem() const;
    bool getAutoStartTournament() const;

    // Tiebreaker settings
    bool getUseSeriesTiebreakers() const;
    int getSingleTiebreaker() const;
    QList<int> getSeriesTiebreakers() const;

    // Setters
    void setPairingSystem(const QString &system);
    void setAutoStartTournament(bool autoStart);

    // Tiebreaker settings
    void setUseSeriesTiebreakers(bool useSeries);
    void setSingleTiebreaker(int tiebreaker);
    void setSeriesTiebreakers(const QList<int> &tiebreakers);

    // Load/save settings
    bool load();
    bool save() const;

private:
    QString pairingSystem; // "round_robin" or "swiss"
    bool autoStartTournament;

    // Tiebreaker settings
    bool useSeriesTiebreakers;
    int singleTiebreaker;
    QList<int> seriesTiebreakers;

    // Helper methods
    QString getSettingsFilePath() const;
};

#endif // SETTINGS_H