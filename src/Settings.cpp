#include "Settings.h"
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>

Settings::Settings()
    : pairingSystem("round_robin"), autoStartTournament(false),
      useSeriesTiebreakers(false), singleTiebreaker(0)
{
}

bool Settings::getUseSeriesTiebreakers() const
{
    return useSeriesTiebreakers;
}

int Settings::getSingleTiebreaker() const
{
    return singleTiebreaker;
}

QList<int> Settings::getSeriesTiebreakers() const
{
    return seriesTiebreakers;
}

void Settings::setUseSeriesTiebreakers(bool useSeries)
{
    useSeriesTiebreakers = useSeries;
}

void Settings::setSingleTiebreaker(int tiebreaker)
{
    singleTiebreaker = tiebreaker;
}

void Settings::setSeriesTiebreakers(const QList<int> &tiebreakers)
{
    seriesTiebreakers = tiebreakers;
}

QString Settings::getPairingSystem() const
{
    return pairingSystem;
}

bool Settings::getAutoStartTournament() const
{
    return autoStartTournament;
}

void Settings::setPairingSystem(const QString &system)
{
    pairingSystem = system;
}

void Settings::setAutoStartTournament(bool autoStart)
{
    autoStartTournament = autoStart;
}

bool Settings::load()
{
    QSettings settings(getSettingsFilePath(), QSettings::IniFormat);

    // Load settings
    pairingSystem = settings.value("pairingSystem", "round_robin").toString();
    autoStartTournament = settings.value("autoStartTournament", false).toBool();

    // Load tiebreaker settings
    useSeriesTiebreakers = settings.value("useSeriesTiebreakers", false).toBool();
    singleTiebreaker = settings.value("singleTiebreaker", 0).toInt();
    seriesTiebreakers = settings.value("seriesTiebreakers", QVariant::fromValue(QList<int>())).value<QList<int>>();

    return true;
}

bool Settings::save() const
{
    QSettings settings(getSettingsFilePath(), QSettings::IniFormat);

    // Save settings
    settings.setValue("pairingSystem", pairingSystem);
    settings.setValue("autoStartTournament", autoStartTournament);

    // Save tiebreaker settings
    settings.setValue("useSeriesTiebreakers", useSeriesTiebreakers);
    settings.setValue("singleTiebreaker", singleTiebreaker);
    settings.setValue("seriesTiebreakers", QVariant::fromValue(seriesTiebreakers));

    return true;
}

QString Settings::getSettingsFilePath() const
{
    // Get application data directory
    QString appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    // Create directory if it doesn't exist
    QDir dir;
    if (!dir.exists(appDataDir))
    {
        dir.mkpath(appDataDir);
    }

    // Return settings file path
    return appDataDir + "/settings.ini";
}