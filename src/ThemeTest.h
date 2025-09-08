#ifndef THEMETEST_H
#define THEMETEST_H

#include <QObject>

class ThemeTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testAbyssThemeFileExists();
    void testAbyssThemeFileReadable();
    void testDarkHighContrastThemeFileExists();
    void testDarkHighContrastThemeFileReadable();
    void testSettingsThemePersistence();
    void testSettingsFileLocation();
    void testThemeSelection();
    void testApplicationStartupWithTheme();
};

#endif // THEMETEST_H