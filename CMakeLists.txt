cmake_minimum_required(VERSION 3.16)
project(SmartCollegeBusSystem VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Search Qt 6 Components
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

# Header directories
include_directories(
    include
    include/models
    include/algorithms
    include/services
    include/ui
)

# Source collection
set(SOURCES
    # Entry Point
    src/main.cpp

    # Models (Demonstrating Encapsulation & Inheritance)
    src/models/User.cpp
    src/models/Admin.cpp
    src/models/Student.cpp
    src/models/Driver.cpp
    src/models/Parent.cpp
    src/models/Bus.cpp
    src/models/Stop.cpp
    src/models/Route.cpp
    src/models/Complaint.cpp
    src/models/AttendanceRecord.cpp
    src/models/Poll.cpp
    src/models/GPSLocation.cpp
    src/models/Trip.cpp

    # Algorithms (Demonstrating Abstraction & Strategy Pattern)
    src/algorithms/Graph.cpp
    src/algorithms/RoutingAlgorithm.cpp
    src/algorithms/AStarRoute.cpp
    src/algorithms/DijkstraRoute.cpp

    # Services (Demonstrating File Handling - No Database)
    src/services/FileManager.cpp
    src/services/AuthManager.cpp
    src/services/ComplaintManager.cpp
    src/services/AttendanceManager.cpp
    src/services/FleetManager.cpp
    src/services/ETAService.cpp
    src/services/BusSimulation.cpp
    src/services/BusTrackingService.cpp
    src/services/RouteController.cpp
    src/services/BusAssignmentMonitorService.cpp

    # UI Widgets (Demonstrating Polymorphism & Native Embedded Map)
    src/ui/MainWindow.cpp
    src/ui/LoginWidget.cpp
    src/ui/AdminDashboardWidget.cpp
    src/ui/StudentDashboardWidget.cpp
    src/ui/DriverDashboardWidget.cpp
    src/ui/ParentDashboardWidget.cpp
    src/ui/ComplaintDeskWidget.cpp
    src/ui/NativeOsmMapWidget.cpp
)

set(HEADERS
    include/models/User.hpp
    include/models/Admin.hpp
    include/models/Student.hpp
    include/models/Driver.hpp
    include/models/Parent.hpp
    include/models/Bus.hpp
    include/models/Stop.hpp
    include/models/Route.hpp
    include/models/Complaint.hpp
    include/models/AttendanceRecord.hpp
    include/models/Poll.hpp
    include/models/GPSLocation.hpp
    include/models/Trip.hpp

    include/algorithms/Graph.hpp
    include/algorithms/RoutingAlgorithm.hpp
    include/algorithms/AStarRoute.hpp
    include/algorithms/DijkstraRoute.hpp

    include/services/FileManager.hpp
    include/services/AuthManager.hpp
    include/services/ComplaintManager.hpp
    include/services/AttendanceManager.hpp
    include/services/FleetManager.hpp
    include/services/ETAService.hpp
    include/services/BusSimulation.hpp
    include/services/BusTrackingService.hpp
    include/services/RouteController.hpp
    include/services/BusAssignmentMonitorService.hpp

    include/ui/MainWindow.hpp
    include/ui/LoginWidget.hpp
    include/ui/AdminDashboardWidget.hpp
    include/ui/StudentDashboardWidget.hpp
    include/ui/DriverDashboardWidget.hpp
    include/ui/ParentDashboardWidget.hpp
    include/ui/ComplaintDeskWidget.hpp
    include/ui/NativeOsmMapWidget.hpp
)

add_executable(SmartCollegeBusSystem ${SOURCES} ${HEADERS})

target_link_libraries(SmartCollegeBusSystem PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
)

# Automatically copy data and assets to build output directory
add_custom_command(TARGET SmartCollegeBusSystem POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/data
        ${CMAKE_CURRENT_BINARY_DIR}/data
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        ${CMAKE_CURRENT_BINARY_DIR}/assets
)
