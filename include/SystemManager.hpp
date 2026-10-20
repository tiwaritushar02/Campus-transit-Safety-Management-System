#ifndef SYSTEM_MANAGER_HPP
#define SYSTEM_MANAGER_HPP

/**
 * ============================================================================
 * ORCHESTRATOR: SystemManager (Integrates 4 Portals & AuthManager)
 * FILE: SystemManager.hpp
 * ============================================================================
 */

#include "Student.hpp"
#include "Driver.hpp"
#include "Conductor.hpp"
#include "Admin.hpp"
#include "Parent.hpp"
#include "Bus.hpp"
#include "Route.hpp"
#include "Stop.hpp"
#include "ScheduleTypes.hpp"
#include "AttendanceLogger.hpp"
#include "ConnectingBusEngine.hpp"
#include "RouteOptimizer.hpp"
#include "ETAEngine.hpp"
#include "QRPassSystem.hpp"
#include "ComplaintEngine.hpp"
#include "MaintenanceManager.hpp"
#include "NotificationCenter.hpp"
#include "AuthManager.hpp"
#include <vector>
#include <memory>

using namespace std;

class SystemManager {
private:
    ScheduleType activeSchedule;
    
    // Polymorphic Person vector storing base pointers to Student/Driver/Conductor/Admin/Parent
    vector<shared_ptr<Person>> people;
    
    vector<Student> students;
    vector<Driver> drivers;
    vector<Conductor> conductors;
    vector<Admin> admins;
    vector<Parent> parents;
    vector<Bus> buses;
    vector<Route> routes;
    vector<Stop> stops;
    
    AttendanceLogger attendanceLogger;
    ComplaintEngine complaintEngine;
    MaintenanceManager maintenanceManager;
    NotificationCenter notificationCenter;
    AuthManager authManager;

public:
    SystemManager();

    void initializeSystemData();

    // Authentication & Session Management
    AuthManager& getAuthManager() { return authManager; }
    UserSession login(const string& username, const string& password);
    void logout();
    UserRole authenticateUser(const string& username, const string& password);

    // Subsystems
    ComplaintEngine& getComplaintEngine() { return complaintEngine; }
    MaintenanceManager& getMaintenanceManager() { return maintenanceManager; }
    NotificationCenter& getNotificationCenter() { return notificationCenter; }

    // Admin Operations (Role 1: Admin)
    bool adminUpdateBus(const string& busId, const string& newNumber, int newCapacity, const string& newRouteId);
    bool adminUpdateStopTiming(const string& stopId, const string& busId, ScheduleType schedule, const string& newTime);
    bool adminAddNewBus(const string& busId, const string& busNumber, int capacity, const string& routeId);
    void adminBroadcastNotice(TargetAudience audience, const string& title, const string& msg);
    void displayAnalyticsDashboard() const;

    // Driver Operations (Role 2: Driver)
    void driverStartTrip(const string& busId);
    void driverEndTrip(const string& busId);
    void driverReportDelay(const string& busId, const string& reason, int minutesDelayed);
    void driverReportMisbehavior(const string& driverName, const string& studentRoll, const string& issue);
    void driverReportBusCondition(const string& busId, const string& issue);

    // Student Operations (Role 3: Student)
    DigitalPass getStudentPass(const string& rollNumber);
    void studentRateDriver(const string& studentRoll, const string& driverId, int stars, const string& feedback);
    void studentReportDriver(const string& studentRoll, const string& busId, const string& issueCategory, const string& details);
    void triggerStudentSOS(const string& studentRoll, const string& busId, const string& stopId);

    // Parent Operations (Role 4: Parent)
    Parent* findParentByPhoneOrChild(const string& query);
    void checkChildStatusForParent(const string& parentPhone);

    // Schedule Management
    void setScheduleType(ScheduleType schedule) { activeSchedule = schedule; }
    ScheduleType getScheduleType() const { return activeSchedule; }

    // Lookups
    Student* findStudentByRoll(const string& rollNumber);
    Driver* findDriverById(const string& driverId);
    Stop* findStopById(const string& stopId);
    Route* findRouteById(const string& routeId);
    Bus* findBusById(const string& busId);

    const vector<Bus>& getAllBuses() const { return buses; }
    const vector<Stop>& getAllStops() const { return stops; }
    const vector<Driver>& getAllDrivers() const { return drivers; }
    const vector<Student>& getAllStudents() const { return students; }

    void displayFleetOverview() const;

    // Core Features
    void checkStudentBusTiming(const string& rollNumber) const;
    void searchConnectingBuses(const string& rollNumber, const string& stopId, const string& currentTimeStr);
    void simulateLiveBusTrip(const string& busId);
    void optimizeRouteBetweenStops(const string& startStopId, const string& destStopId) const;
    void processBoarding(const string& rollNumber, const string& busId);

    // Logs
    void displayAttendanceLogs() const { attendanceLogger.displayLogs(); }
    void runAcademicSelfTest();
};

#endif // SYSTEM_MANAGER_HPP
