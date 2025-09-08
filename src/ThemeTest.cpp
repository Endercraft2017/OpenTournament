#include <QtTest/QtTest>
#include <QApplication>
#include <QFile>
#include <QSettings>
#include <QDir>
#include <QStandardPaths>

#include "ThemeTest.h"
#include "Settings.h"

// Test implementation
void ThemeTest::initTestCase()
{
    // Setup code before all tests
}

void ThemeTest::cleanupTestCase()
{
    // Cleanup code after all tests
}

void ThemeTest::testAbyssThemeFileExists()
{
    // Check that the abyss.qss file exists
    QFile themeFile(":/styles/abyss.qss");
    QVERIFY(themeFile.exists());
}

void ThemeTest::testDarkHighContrastThemeFileExists()
{
    // Check that the dark_high_contrast.qss file exists
    QFile themeFile(":/styles/dark_high_contrast.qss");
    QVERIFY(themeFile.exists());
}

void ThemeTest::testAbyssThemeFileReadable()
{
    // Check that the abyss.qss file can be opened and read
    QFile themeFile(":/styles/abyss.qss");
    QVERIFY(themeFile.open(QFile::ReadOnly));
    
    // Check that the file has content
    QByteArray content = themeFile.readAll();
    QVERIFY(!content.isEmpty());
}

void ThemeTest::testDarkHighContrastThemeFileReadable()
{
    // Check that the dark_high_contrast.qss file can be opened and read
    QFile themeFile(":/styles/dark_high_contrast.qss");
    QVERIFY(themeFile.open(QFile::ReadOnly));
    
    // Check that the file has content
    QByteArray content = themeFile.readAll();
    QVERIFY(!content.isEmpty());
}

void ThemeTest::testSettingsThemePersistence()
{
    // Test that theme settings can be saved and loaded
    Settings settings;
    
    // Test each theme option
    QStringList themes = {"fusion_dark", "fusion_light", "fusion_white", "abyss", "dark_high_contrast"};
    for (const QString& theme : themes) {
        // Set theme
        settings.setTheme(theme);
        
        // Save settings
        QVERIFY2(settings.save(), QString("Failed to save theme: %1").arg(theme).toStdString().c_str());
        
        // Create a new settings object to simulate fresh load
        Settings loadedSettings;
        QVERIFY2(loadedSettings.load(), QString("Failed to load settings for theme: %1").arg(theme).toStdString().c_str());
        
        // Check that the theme was loaded correctly
        QCOMPARE(loadedSettings.getTheme(), theme);
    }
}

void ThemeTest::testSettingsFileLocation()
{
    // Test that the settings file is created in the expected location
    Settings settings;
    settings.setTheme("abyss");
    QVERIFY(settings.save());
    
    // Get the expected file path
    QString settingsPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString settingsFile = settingsPath + "/settings.ini";
    
    // Check that the file exists
    QFile file(settingsFile);
    QVERIFY2(file.exists(), QString("Settings file does not exist: %1").arg(settingsFile).toStdString().c_str());
    
    // Check that the file is readable
    QVERIFY2(file.open(QIODevice::ReadOnly), QString("Cannot read settings file: %1").arg(settingsFile).toStdString().c_str());
    
    // Check that the file contains theme information
    QByteArray content = file.readAll();
    QVERIFY2(content.contains("abyss"), "Settings file does not contain theme information");
}

void ThemeTest::testThemeSelection()
{
    // Test that we can set different themes
    Settings settings;
    
    // Test setting each theme
    settings.setTheme("fusion_dark");
    QCOMPARE(settings.getTheme(), QString("fusion_dark"));
    
    settings.setTheme("fusion_light");
    QCOMPARE(settings.getTheme(), QString("fusion_light"));
    
    settings.setTheme("fusion_white");
    QCOMPARE(settings.getTheme(), QString("fusion_white"));
    
    settings.setTheme("abyss");
    QCOMPARE(settings.getTheme(), QString("abyss"));
    
    settings.setTheme("dark_high_contrast");
    QCOMPARE(settings.getTheme(), QString("dark_high_contrast"));
}

void ThemeTest::testApplicationStartupWithTheme()
{
    // Test that application can start with each theme
    Settings settings;
    
    // Test each theme
    QStringList themes = {"fusion_dark", "fusion_light", "fusion_white", "abyss", "dark_high_contrast"};
    for (const QString& theme : themes) {
        // Set theme
        settings.setTheme(theme);
        settings.save();
        
        // Note: We can't actually start the full application in a unit test,
        // but we can verify that the theme file exists and is readable
        if (theme == "abyss") {
            QFile themeFile(":/styles/abyss.qss");
            QVERIFY2(themeFile.exists(), "Abyss theme file does not exist");
            QVERIFY2(themeFile.open(QFile::ReadOnly), "Cannot read abyss theme file");
        }
        else if (theme == "dark_high_contrast") {
            QFile themeFile(":/styles/dark_high_contrast.qss");
            QVERIFY2(themeFile.exists(), "Dark High Contrast theme file does not exist");
            QVERIFY2(themeFile.open(QFile::ReadOnly), "Cannot read dark high contrast theme file");
        }
        // For other themes, they are built into Qt and don't have separate files
    }
}

QTEST_MAIN(ThemeTest)
#include "ThemeTest.moc"