#include <QApplication>
#include <QFile>
#include <QStyleFactory>
#include <QPalette>
#include <QIcon>
#include <QDebug>
#include "MainWindow.h"

void setFusionDark(QApplication &app)
{
    app.setStyle(QStyleFactory::create("Fusion"));
    QPalette darkPalette;

    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));

    // ✅ Fixed: was QColor(53, 53) → now QColor(53, 53, 53)
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));

    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);

    app.setPalette(darkPalette);
    app.setStyleSheet("");
}

void setFusionLight(QApplication &app)
{
    app.setStyle(QStyleFactory::create("Fusion"));
    app.setPalette(QApplication::style()->standardPalette());
    app.setStyleSheet("");
}

void setFusionWhite(QApplication &app)
{
    app.setStyle(QStyleFactory::create("Fusion"));
    QPalette whitePalette;

    whitePalette.setColor(QPalette::Window, Qt::white);
    whitePalette.setColor(QPalette::WindowText, Qt::black);
    whitePalette.setColor(QPalette::Base, QColor(245, 245, 245));
    whitePalette.setColor(QPalette::AlternateBase, Qt::white);
    whitePalette.setColor(QPalette::ToolTipBase, Qt::white);
    whitePalette.setColor(QPalette::ToolTipText, Qt::black);
    whitePalette.setColor(QPalette::Text, Qt::black);
    whitePalette.setColor(QPalette::Button, Qt::white);
    whitePalette.setColor(QPalette::ButtonText, Qt::black);
    whitePalette.setColor(QPalette::BrightText, Qt::red);

    app.setPalette(whitePalette);
    app.setStyleSheet("");
}


int main(int argc, char *argv[])
{
    qDebug() << "Starting application...";
    QApplication app(argc, argv);
    qDebug() << "QApplication created";

    // Set application icon
    app.setWindowIcon(QIcon(":/icons/app_icon.png"));
    qDebug() << "Application icon set";

    // Load settings to get the saved theme
    // For now, we'll use Fusion Dark as default, but in a real implementation
    // we would load the saved theme from settings
    setFusionDark(app); // default theme
    qDebug() << "Theme set";

    qDebug() << "Creating MainWindow...";
    MainWindow window;
    qDebug() << "MainWindow created";
    window.show();
    qDebug() << "MainWindow shown";

    return app.exec();
}
