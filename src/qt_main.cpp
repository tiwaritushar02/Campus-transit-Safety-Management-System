/**
 * ============================================================================
 * QT GUI MODULE: Main Application Entry Point
 * FILE: qt_main.cpp
 * ============================================================================
 */

#include "MainWindow.hpp"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Apply custom Qt Dark Theme palette
    app.setStyle("Fusion");

    MainWindow window;
    window.show();

    return app.exec();
}
