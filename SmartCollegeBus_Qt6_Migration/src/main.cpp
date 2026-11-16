#include <QApplication>
#include "MainWindow.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("SmartCollegeBusManagementSystem");
    app.setOrganizationName("GraphicEraHillUniversity");

    MainWindow window;
    window.show();

    return app.exec();
}
