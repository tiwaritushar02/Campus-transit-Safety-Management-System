/**
 * ============================================================================
 * QT GUI MODULE: Main Application Entry Point
 * FILE: qt_main.cpp
 * ============================================================================
 */

#include "MainWindow.hpp"
#include <QApplication>
#include <QDir>
#include <QCoreApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Ensure working directory is project root when launched via macOS Finder or bundle
    QDir projectDir(QCoreApplication::applicationDirPath());
    if (projectDir.dirName() == "MacOS") {
        projectDir.cdUp(); // Contents
        projectDir.cdUp(); // smart_college_bus_system_gui.app
        projectDir.cdUp(); // bin
        projectDir.cdUp(); // project root
        QDir::setCurrent(projectDir.absolutePath());
    }

    // Apply custom Qt Dark Theme palette
    app.setStyle("Fusion");

    MainWindow window;
    window.show();

    return app.exec();
}
