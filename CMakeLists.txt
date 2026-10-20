cmake_minimum_required(VERSION 3.14)
project(smart_college_bus_system_gui LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core Gui)

include_directories(include)

set(SOURCES
    src/Student.cpp
    src/Driver.cpp
    src/Conductor.cpp
    src/Stop.cpp
    src/Route.cpp
    src/Bus.cpp
    src/ETAEngine.cpp
    src/RouteOptimizer.cpp
    src/ConnectingBusEngine.cpp
    src/AttendanceLogger.cpp
    src/FileHandler.cpp
    src/SystemManager.cpp
    src/MainWindow.cpp
    src/qt_main.cpp
)

set(HEADERS
    include/Person.hpp
    include/Student.hpp
    include/Driver.hpp
    include/Conductor.hpp
    include/Stop.hpp
    include/Route.hpp
    include/Bus.hpp
    include/ScheduleTypes.hpp
    include/LiveLocationSim.hpp
    include/ETAEngine.hpp
    include/RouteOptimizer.hpp
    include/ConnectingBusEngine.hpp
    include/AttendanceLogger.hpp
    include/Exceptions.hpp
    include/FileHandler.hpp
    include/SystemManager.hpp
    include/MainWindow.hpp
)

add_executable(smart_college_bus_system_gui ${SOURCES} ${HEADERS})

target_link_libraries(smart_college_bus_system_gui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui)
