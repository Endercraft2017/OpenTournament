#include "Settings.h"
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>

Settings::Settings()
    : pairingSystem("round_robin"), autoStartTournament(false),
      swissRounds(5), // Default to 5 rounds for Swiss pairing
      theme("abyss"), iconSet("default"),
      useSeriesTiebreakers(false), singleTiebreaker(0)
{
}

// Theme settings getters
QString Settings::getTheme() const
{
    return theme;
}

QString Settings::getIconSet() const
{
    return iconSet;
}

// Theme settings setters
void Settings::setTheme(const QString &theme)
{
    this->theme = theme;
}

void Settings::setIconSet(const QString &iconSet)
{
    this->iconSet = iconSet;
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

int Settings::getSwissRounds() const
{
    return swissRounds;
}

void Settings::setPairingSystem(const QString &system)
{
    pairingSystem = system;
}

void Settings::setAutoStartTournament(bool autoStart)
{
    autoStartTournament = autoStart;
}

void Settings::setSwissRounds(int rounds)
{
    swissRounds = rounds;
}

bool Settings::load()
{
    QSettings settings(getSettingsFilePath(), QSettings::IniFormat);

    // Load settings
    pairingSystem = settings.value("pairingSystem", "round_robin").toString();
    autoStartTournament = settings.value("autoStartTournament", false).toBool();
    swissRounds = settings.value("swissRounds", 5).toInt(); // Load Swiss rounds setting

    // Load theme settings
    theme = settings.value("theme", "abyss").toString();
    iconSet = settings.value("iconSet", "default").toString();

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
    settings.setValue("swissRounds", swissRounds); // Save Swiss rounds setting

    // Save theme settings
    settings.setValue("theme", theme);
    settings.setValue("iconSet", iconSet);

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