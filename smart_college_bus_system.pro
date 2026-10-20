# Qt Creator Project File (.pro)
# Extended 4-Portal Smart College Bus System (Student, Driver, Admin, Parent)

QT += core gui widgets network

CONFIG += c++17

TARGET = smart_college_bus_system_gui
TEMPLATE = app

INCLUDEPATH += include

HEADERS += \
    include/UserAccount.hpp \
    include/AuthManager.hpp \
    include/Person.hpp \
    include/Student.hpp \
    include/Driver.hpp \
    include/Conductor.hpp \
    include/Admin.hpp \
    include/Parent.hpp \
    include/Stop.hpp \
    include/Route.hpp \
    include/Bus.hpp \
    include/ScheduleTypes.hpp \
    include/LiveLocationSim.hpp \
    include/ETAEngine.hpp \
    include/RouteOptimizer.hpp \
    include/ConnectingBusEngine.hpp \
    include/AttendanceLogger.hpp \
    include/QRPassSystem.hpp \
    include/ComplaintEngine.hpp \
    include/MaintenanceManager.hpp \
    include/NotificationCenter.hpp \
    include/Exceptions.hpp \
    include/FileHandler.hpp \
    include/SystemManager.hpp \
    include/MainWindow.hpp

SOURCES += \
    src/UserAccount.cpp \
    src/AuthManager.cpp \
    src/Student.cpp \
    src/Driver.cpp \
    src/Conductor.cpp \
    src/Admin.cpp \
    src/Parent.cpp \
    src/Stop.cpp \
    src/Route.cpp \
    src/Bus.cpp \
    src/ETAEngine.cpp \
    src/RouteOptimizer.cpp \
    src/ConnectingBusEngine.cpp \
    src/AttendanceLogger.cpp \
    src/QRPassSystem.cpp \
    src/ComplaintEngine.cpp \
    src/MaintenanceManager.cpp \
    src/NotificationCenter.cpp \
    src/FileHandler.cpp \
    src/SystemManager.cpp \
    src/MainWindow.cpp \
    src/qt_main.cpp

DESTDIR = bin
