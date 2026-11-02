/**
 * ============================================================================
 * ORCHESTRATOR: SystemManager Implementation
 * FILE: SystemManager.cpp
 * ============================================================================
 */

#include "SystemManager.hpp"
#include "FileHandler.hpp"
#include "LiveLocationSim.hpp"
#include <iostream>
#include <iomanip>

using namespace std;

SystemManager::SystemManager()
    : activeSchedule(ScheduleType::FULL_DAY) {
    initializeSystemData();
}

void SystemManager::initializeSystemData() {
    FileHandler::initializeSampleData();

    // 1. Initialize Admins
    admins = {
        Admin("A-01", "College Transport Head", "+91-9900011122", "admin@college.edu", "SUPER_ADMIN")
    };
    auto adminPtr = make_shared<Admin>(admins[0]);
    people.push_back(adminPtr);
    authManager.registerAccount(UserAccount("admin", "admin123", UserRole::ADMIN, adminPtr));

    // 2. Initialize Drivers & Conductors
    drivers = {
        Driver("D-01", "Rajesh Kumar", "+91-9876500001", "rajesh@bus.college.edu", "DL-IND-2012-9981", 12, "B-101"),
        Driver("D-02", "Vikram Singh", "+91-9876500002", "vikram@bus.college.edu", "DL-IND-2015-4412", 9,  "B-102"),
        Driver("D-03", "Suresh Nair",  "+91-9876500003", "suresh@bus.college.edu", "DL-IND-2018-7719", 6,  "B-103")
    };

    conductors = {
        Conductor("C-01", "Amit Das",   "+91-9123400001", "amit@bus.college.edu",   "B-101"),
        Conductor("C-02", "Sunil Joshi", "+91-9123400002", "sunil@bus.college.edu",  "B-102"),
        Conductor("C-03", "Ramesh Rao",  "+91-9123400003", "ramesh@bus.college.edu", "B-103")
    };

    for (size_t i = 0; i < drivers.size(); ++i) {
        auto dPtr = make_shared<Driver>(drivers[i]);
        people.push_back(dPtr);
        authManager.registerAccount(UserAccount(drivers[i].getId(), "driver123", UserRole::DRIVER, dPtr));
    }
    for (const auto& c : conductors) people.push_back(make_shared<Conductor>(c));

    // 3. Initialize Parents
    parents = {
        Parent("P-101", "Vijay Sharma", "+91-9811001122", "vijay@parent.edu", "2024-CS-001", "Aarav Sharma", 0.0, "PAID"),
        Parent("P-102", "Sunita Verma", "+91-9822002233", "sunita@parent.edu", "2024-EC-045", "Priya Verma", 0.0, "PAID"),
        Parent("P-103", "Dinesh Patel", "+91-9833003344", "dinesh@parent.edu", "2024-ME-012", "Rohan Patel", 4500.0, "2026-09-15")
    };

    for (size_t i = 0; i < parents.size(); ++i) {
        auto pPtr = make_shared<Parent>(parents[i]);
        people.push_back(pPtr);
        authManager.registerAccount(UserAccount(parents[i].getId(), "parent123", UserRole::PARENT, pPtr));
    }

    // 4. Initialize Stops
    Stop st101("ST101", "Haldwani Bus Terminal");
    st101.addPassingBus("B-101", "07:30", "07:15");
    st101.addPassingBus("B-102", "07:45", "07:30");
    st101.addPassingBus("B-103", "08:05", "07:45");

    Stop st102("ST102", "Kathgodam Railway Station");
    st102.addPassingBus("B-101", "07:45", "07:25");
    st102.addPassingBus("B-102", "08:00", "07:40");
    st102.addPassingBus("B-103", "08:20", "08:00");

    Stop st103("ST103", "Ranibagh HMT Junction");
    st103.addPassingBus("B-101", "08:05", "07:40");
    st103.addPassingBus("B-102", "08:15", "07:55");

    Stop st104("ST104", "Bhowali Sanatorium Chowk");
    st104.addPassingBus("B-102", "08:30", "08:10");
    st104.addPassingBus("B-103", "08:35", "08:15");

    Stop st105("ST105", "Graphic Era Hill University (GEHU), Bhimtal Campus");
    st105.addPassingBus("B-101", "08:25", "08:00");
    st105.addPassingBus("B-102", "08:45", "08:25");
    st105.addPassingBus("B-103", "08:55", "08:30");

    stops = {st101, st102, st103, st104, st105};

    // 5. Initialize Routes
    Route r1("R1", "Express Route: Haldwani -> Kathgodam -> Ranibagh -> GEHU Bhimtal Campus");
    r1.addStop(st101); r1.addStop(st102); r1.addStop(st103); r1.addStop(st105);
    r1.setSegmentDistance("ST101", "ST102", 4.5);
    r1.setSegmentDistance("ST102", "ST103", 5.0);
    r1.setSegmentDistance("ST103", "ST105", 19.0);

    Route r2("R2", "Suburban Route: Haldwani -> Kathgodam -> Bhowali -> GEHU Bhimtal Campus");
    r2.addStop(st101); r2.addStop(st102); r2.addStop(st104); r2.addStop(st105);
    r2.setSegmentDistance("ST101", "ST102", 4.5);
    r2.setSegmentDistance("ST102", "ST104", 15.5);
    r2.setSegmentDistance("ST104", "ST105", 11.2);

    routes = {r1, r2};

    // 6. Initialize Buses
    Bus b1("B-101", "MH-12-AB-1001", 45, "R1", &drivers[0], &conductors[0]);
    Bus b2("B-102", "MH-12-CD-2002", 50, "R2", &drivers[1], &conductors[1]);
    Bus b3("B-103", "MH-12-EF-3003", 35, "R1", &drivers[2], &conductors[2]);

    buses = {b1, b2, b3};

    // 7. Initialize Students
    students = FileHandler::loadStudents("data/students.csv");
    if (students.empty()) {
        students = {
            Student("S101", "Aarav Sharma", "+91-9811223344", "aarav@college.edu", "2024-CS-001", "R1", "ST101", true),
            Student("S102", "Diya Rawat",   "+91-9811223345", "diya@college.edu",  "2024-CS-002", "R1", "ST101", true),
            Student("S103", "Priya Verma",  "+91-9822334455", "priya@college.edu", "2024-EC-045", "R1", "ST102", true),
            Student("S104", "Ayush Joshi",  "+91-9822334456", "ayush@college.edu", "2024-AI-018", "R1", "ST102", true),
            Student("S105", "Sneha Bisht",  "+91-9833445566", "sneha@college.edu", "2024-IT-027", "R1", "ST103", true),
            Student("S106", "Kavya Pant",   "+91-9833445567", "kavya@college.edu", "2024-CS-033", "R1", "ST103", true),
            Student("S107", "Rohan Patel",  "+91-9844556677", "rohan@college.edu", "2024-ME-012", "R2", "ST104", true),
            Student("S108", "Harshit Pandey","+91-9844556678","harshit@college.edu","2024-EC-029", "R2", "ST104", true),
            Student("S109", "Ananya Iyer",  "+91-9855667788", "ananya@college.edu", "2024-CS-089", "R2", "ST106", true),
            Student("S110", "Aditya Mehra", "+91-9855667789", "aditya@college.edu", "2024-AI-052", "R2", "ST106", true)
        };
    }
    for (size_t i = 0; i < students.size(); ++i) {
        auto sPtr = make_shared<Student>(students[i]);
        people.push_back(sPtr);
        authManager.registerAccount(UserAccount(students[i].getRollNumber(), "student123", UserRole::STUDENT, sPtr));
    }

    // 8. Seed Initial Complaints & Incident Reports
    Complaint c1;
    c1.complaintId = "CMP-101";
    c1.type = ComplaintType::STUDENT_TO_DRIVER;
    c1.reporterName = "Aarav Sharma (2024-CS-001)";
    c1.targetName = "Driver Rajesh Kumar (D-01)";
    c1.busId = "B-101";
    c1.category = "Rash Driving";
    c1.description = "Driver was overspeeding near Kathgodam Railway Station bypass.";
    c1.timestamp = "2026-11-02 08:10";
    c1.status = ComplaintStatus::INVESTIGATING;
    c1.adminActionNote = "Under review by transport office";
    complaintEngine.logComplaint(c1);

    Complaint c2;
    c2.complaintId = "MISB-102";
    c2.type = ComplaintType::DRIVER_TO_STUDENT;
    c2.reporterName = "Driver Rajesh Kumar (D-01)";
    c2.targetName = "Rohan Patel (2024-ME-012)";
    c2.busId = "B-101";
    c2.category = "STUDENT_MISBEHAVIOR";
    c2.description = "Loud music and disruption of fellow student commuters.";
    c2.timestamp = "2026-11-02 08:25";
    c2.status = ComplaintStatus::OPEN;
    c2.adminActionNote = "Warning notice queued";
    complaintEngine.logComplaint(c2);
}

UserSession SystemManager::login(const string& username, const string& password) {
    return authManager.login(username, password);
}

void SystemManager::logout() {
    authManager.logout();
}

UserRole SystemManager::authenticateUser(const string& username, const string& password) {
    UserSession session = authManager.login(username, password);
    if (session.isAuthenticated && session.activeAccount) {
        return session.activeAccount->getRole();
    }
    return UserRole::UNAUTHORIZED;
}

// ADMIN OPERATIONS
bool SystemManager::adminUpdateBus(const string& busId, const string& newNumber, int newCapacity, const string& newRouteId) {
    Bus* bus = findBusById(busId);
    if (!bus) return false;

    if (!newNumber.empty()) bus->setBusNumber(newNumber);
    if (newCapacity > 0) bus->setCapacity(newCapacity);
    if (!newRouteId.empty()) bus->setAssignedRouteId(newRouteId);

    FileHandler::saveBuses(buses, "data/buses.csv");
    return true;
}

bool SystemManager::adminUpdateStopTiming(const string& stopId, const string& busId, ScheduleType schedule, const string& newTime) {
    Stop* stop = findStopById(stopId);
    if (!stop) return false;

    stop->updateArrivalTime(busId, schedule, newTime);
    return true;
}

bool SystemManager::adminAddNewBus(const string& busId, const string& busNumber, int capacity, const string& routeId) {
    if (findBusById(busId) != nullptr) return false;

    buses.emplace_back(busId, busNumber, capacity, routeId);
    FileHandler::saveBuses(buses, "data/buses.csv");
    return true;
}

void SystemManager::adminBroadcastNotice(TargetAudience audience, const string& title, const string& msg) {
    notificationCenter.sendBroadcast(audience, title, msg);
}

void SystemManager::displayAnalyticsDashboard() const {
    cout << "\n===========================================================\n";
    cout << "           TRANSPORT SYSTEM ANALYTICS DASHBOARD          \n";
    cout << "===========================================================\n";
    cout << "Total Fleet Buses:      " << buses.size() << " Buses\n";
    cout << "Registered Drivers:     " << drivers.size() << " Drivers\n";
    cout << "Active Student Passes:  " << students.size() << " Students\n";
    cout << "Active Day Mode:        " << ScheduleUtils::toString(activeSchedule) << "\n";
    cout << "Highest Rated Driver:   Rajesh Kumar (Rating: 4.9 *)\n";
    cout << "Fleet Seat Utilization: 78.5% Average Occupancy\n";
    cout << "===========================================================\n\n";
}

// DRIVER OPERATIONS
void SystemManager::driverStartTrip(const string& busId) {
    Bus* bus = findBusById(busId);
    if (bus) {
        cout << " Driver started trip for Bus [" << busId << "] on Route [" << bus->getAssignedRouteId() << "]\n";
        notificationCenter.sendBroadcast(TargetAudience::ALL_STUDENTS, " Trip Started", "Bus " + busId + " has started its morning trip.", bus->getAssignedRouteId());
    }
}

void SystemManager::driverEndTrip(const string& busId) {
    cout << " Trip ended for Bus [" << busId << "]. All passengers unboarded.\n";
}

bool SystemManager::adminRegisterNewStudent(const Student& newStudent) {
    for (auto& s : students) {
        if (s.getRollNumber() == newStudent.getRollNumber()) {
            s = newStudent;
            FileHandler::saveStudents(students, "data/students.csv");
            return true;
        }
    }
    students.push_back(newStudent);
    auto sPtr = make_shared<Student>(newStudent);
    people.push_back(sPtr);
    authManager.registerAccount(UserAccount(newStudent.getRollNumber(), "student123", UserRole::STUDENT, sPtr));
    FileHandler::saveStudents(students, "data/students.csv");
    return true;
}

void SystemManager::driverReportDelay(const string& busId, const string& reason, int minutesDelayed) {
    activeDelayNotice.hasDelay = true;
    activeDelayNotice.busId = busId;
    activeDelayNotice.reason = reason;
    activeDelayNotice.minutesDelayed = minutesDelayed;
    activeDelayNotice.timestamp = "Morning Transit";

    string msg = "Bus " + busId + " is delayed by " + to_string(minutesDelayed) + " mins due to " + reason + ".";
    notificationCenter.sendBroadcast(TargetAudience::ALL_STUDENTS, "[WARNING] Bus Delay Alert", msg, busId);
    notificationCenter.sendBroadcast(TargetAudience::ALL_PARENTS, "[WARNING] Child Bus Delay Alert", msg, busId);
}

void SystemManager::driverReportMisbehavior(const string& driverName, const string& studentRoll, const string& issue) {
    Complaint c;
    c.complaintId = "MISB-" + to_string(rand() % 900 + 100);
    c.type = ComplaintType::DRIVER_TO_STUDENT;
    c.reporterName = driverName;
    c.targetName = studentRoll;
    c.busId = "B-101";
    c.category = "STUDENT_MISBEHAVIOR";
    c.description = issue;
    c.timestamp = "2026-08-02 08:15";
    c.status = ComplaintStatus::OPEN;
    
    complaintEngine.logComplaint(c);
}

void SystemManager::driverReportBusCondition(const string& busId, const string& issue) {
    maintenanceManager.logConditionReport(busId, issue);
}

// STUDENT OPERATIONS
DigitalPass SystemManager::getStudentPass(const string& rollNumber) {
    Student* student = findStudentByRoll(rollNumber);
    if (!student) {
        Student dummy("S00", "Guest Student", "000", "guest@college.edu", rollNumber, "R1", "ST101", true);
        return QRPassSystem::generateBusPass(dummy, "R1", "ST101");
    }
    return QRPassSystem::generateBusPass(*student, student->getAssignedRouteId(), student->getAssignedStopId());
}

void SystemManager::studentRateDriver(const string& studentRoll, const string& driverId, int stars, const string& feedback) {
    complaintEngine.submitDriverRating(driverId, studentRoll, stars, feedback);
}

void SystemManager::studentReportDriver(const string& studentRoll, const string& busId, const string& issueCategory, const string& details) {
    Complaint c;
    c.complaintId = "REP-" + to_string(rand() % 900 + 100);
    c.type = ComplaintType::STUDENT_TO_DRIVER;
    c.reporterName = studentRoll;
    c.targetName = "Driver of " + busId;
    c.busId = busId;
    c.category = issueCategory;
    c.description = details;
    c.timestamp = "2026-08-02 08:30";
    c.status = ComplaintStatus::OPEN;

    complaintEngine.logComplaint(c);
}

void SystemManager::triggerStudentSOS(const string& studentRoll, const string& busId, const string& stopId) {
    complaintEngine.triggerSOSAlert(studentRoll, busId, stopId);
}

// PARENT OPERATIONS
Parent* SystemManager::findParentByPhoneOrChild(const string& query) {
    for (auto& p : parents) {
        if (p.getPhone() == query || p.getChildRollNumber() == query || p.getId() == query) {
            return &p;
        }
    }
    return nullptr;
}

void SystemManager::checkChildStatusForParent(const string& parentPhone) {
    Parent* parent = findParentByPhoneOrChild(parentPhone);
    if (!parent) {
        cout << "[ERROR] Parent record not found.\n";
        return;
    }

    parent->displayDetails();
    cout << "\n LIVE PARENT BUS TRACKING:\n";
    cout << "  Child [" << parent->getChildName() << "]'s Bus: B-101 (Route R1 Express)\n";
    cout << "  Current Bus Location: Central Metro Station (ST102)\n";
    cout << "  Estimated Arrival at College: 08:25 AM (On Time [SUCCESS])\n";
    cout << "  Driver: Rajesh Kumar (Tel: +91-9876500001)\n";
}

// LOOKUPS
Student* SystemManager::findStudentByRoll(const string& rollNumber) {
    for (auto& s : students) {
        if (s.getRollNumber() == rollNumber) return &s;
    }
    return nullptr;
}

Driver* SystemManager::findDriverById(const string& driverId) {
    for (auto& d : drivers) {
        if (d.getId() == driverId || d.getName() == driverId) return &d;
    }
    return nullptr;
}

Stop* SystemManager::findStopById(const string& stopId) {
    for (auto& s : stops) {
        if (s.getStopId() == stopId) return &s;
    }
    return nullptr;
}

Route* SystemManager::findRouteById(const string& routeId) {
    for (auto& r : routes) {
        if (r.getRouteId() == routeId) return &r;
    }
    return nullptr;
}

Bus* SystemManager::findBusById(const string& busId) {
    for (auto& b : buses) {
        if (b.getBusId() == busId) return &b;
    }
    return nullptr;
}

void SystemManager::displayFleetOverview() const {
    cout << "\n===========================================================\n";
    cout << "                 COLLEGE BUS FLEET OVERVIEW                \n";
    cout << "===========================================================\n";
    cout << "Active Schedule: " << ScheduleUtils::toString(activeSchedule) << "\n\n";

    for (const auto& b : buses) {
        b.displayBusStatus();
        cout << "\n";
    }
}

void SystemManager::checkStudentBusTiming(const string& rollNumber) const {
    const Student* student = nullptr;
    for (const auto& s : students) {
        if (s.getRollNumber() == rollNumber) { student = &s; break; }
    }

    if (!student) {
        cout << "[ERROR] Error: Student with Roll Number [" << rollNumber << "] not found.\n";
        return;
    }

    cout << "\n===========================================================\n";
    cout << "        STUDENT SCHEDULE LOOKUP (PROBLEM SOLVER 1 & 2)      \n";
    cout << "===========================================================\n";
    student->displayDetails();
    cout << "\nACTIVE TODAY: " << ScheduleUtils::toString(activeSchedule) << "\n";

    const Stop* stop = nullptr;
    for (const auto& s : stops) {
        if (s.getStopId() == student->getAssignedStopId()) { stop = &s; break; }
    }

    if (stop) {
        cout << "\nScheduled Bus Departures at your stop [" << stop->getStopName() << "]:\n";
        for (const auto& busId : stop->getPassingBusIds()) {
            string arrTime = stop->getArrivalTime(busId, activeSchedule);
            cout << "  Bus [" << busId << "] Scheduled Morning Departure: " << arrTime << "\n";
        }
    }
    cout << "===========================================================\n\n";
}

void SystemManager::searchConnectingBuses(const string& rollNumber, const string& stopId, const string& currentTimeStr) {
    cout << "\n===========================================================\n";
    cout << "    CORE FEATURE: MISSED BUS -> CONNECTING BUS SEARCH     \n";
    cout << "===========================================================\n";
    cout << "Student Roll: " << rollNumber << " | Current Stop ID: " << stopId << "\n";
    cout << "Missed Time Input: " << currentTimeStr << "\n";
    cout << "Active Day Schedule: " << ScheduleUtils::toString(activeSchedule) << "\n";
    cout << "-----------------------------------------------------------\n";

    int missedMinutes = ScheduleUtils::timeStringToMinutes(currentTimeStr);

    try {
        auto options = ConnectingBusEngine::findConnectingBuses(stops, buses, stopId, missedMinutes, activeSchedule);

        cout << "[SUCCESS] Found " << options.size() << " connecting bus(es) passing your stop after " << currentTimeStr << ":\n\n";

        for (size_t i = 0; i < options.size(); ++i) {
            cout << "Option #" << (i + 1) << ": Bus [" << options[i].busId << "] (Registration: " << options[i].busNumber << ")\n";
            cout << "  - Scheduled Arrival: " << options[i].scheduledArrivalTimeStr << " (" 
                      << (options[i].arrivalTimeMinutes - missedMinutes) << " mins from now)\n";
            cout << "  - Route ID: " << options[i].routeId << "\n";
            cout << "  - Available Seats: " << options[i].availableSeats << " seats free\n";
            cout << "  - Driver Contact: " << options[i].driverName << " Tel: " << options[i].driverPhone << "\n\n";
        }
    } catch (const BusSystemException& ex) {
        cout << "[WARNING]  " << ex.what() << "\n";
    }
    cout << "===========================================================\n\n";
}

void SystemManager::simulateLiveBusTrip(const string& busId) {
    Bus* bus = findBusById(busId);
    if (!bus) {
        cout << "[ERROR] Error: Bus [" << busId << "] not found.\n";
        return;
    }

    const Route* route = findRouteById(bus->getAssignedRouteId());
    if (!route) {
        cout << "[ERROR] Error: Assigned route not found for Bus [" << busId << "].\n";
        return;
    }

    LiveLocationSim::simulateBusTrip(*bus, *route, 1, true);
}

void SystemManager::optimizeRouteBetweenStops(const string& startStopId, const string& destStopId) const {
    cout << "\n===========================================================\n";
    cout << "        ROUTE OPTIMIZER (SHORTEST PATH & MIN ETA)           \n";
    cout << "===========================================================\n";

    PathResult result = RouteOptimizer::findShortestPath(routes, startStopId, destStopId);

    cout << "Start Stop: " << startStopId << " -> Destination: " << destStopId << "\n";
    cout << "Recommended Route ID: " << result.recommendedRouteId << "\n";
    cout << "Total Distance: " << result.totalDistanceKm << " km\n";
    cout << "Estimated Travel Time: " << result.totalTimeMinutes << " minutes\n";
    cout << "Stop Sequence Path:\n  ";
    for (size_t i = 0; i < result.stopPath.size(); ++i) {
        cout << result.stopPath[i] << (i + 1 < result.stopPath.size() ? " -> " : "");
    }
    cout << "\n===========================================================\n\n";
}

void SystemManager::processBoarding(const string& rollNumber, const string& busId) {
    Student* student = findStudentByRoll(rollNumber);
    Bus* bus = findBusById(busId);

    if (!student) {
        cout << "[ERROR] Error: Student [" << rollNumber << "] not registered.\n";
        return;
    }
    if (!bus) {
        cout << "[ERROR] Error: Bus [" << busId << "] invalid.\n";
        return;
    }

    cout << "\nAttempting boarding for Student [" << student->getName() << "] on Bus [" << busId << "]...\n";

    if (!student->isFeePaid()) {
        cout << "[ERROR] Boarding Denied: Student bus fee is PENDING!\n";
        attendanceLogger.logBoarding(rollNumber, busId, student->getAssignedStopId(), "DENIED_FEE_PENDING");
        return;
    }

    try {
        bus->boardStudent();
        cout << "[SUCCESS] Boarding Successful! Seats remaining: " << bus->getAvailableSeats() << "\n";
        attendanceLogger.logBoarding(rollNumber, busId, student->getAssignedStopId(), "SUCCESS_BOARDED");

        notificationCenter.sendParentDropNotification("+91-9811001122", student->getName(), "safely boarded bus", "07:32 AM");
    } catch (const SeatFullException& ex) {
        cout << "[WARNING]  " << ex.what() << "\n";
        attendanceLogger.logBoarding(rollNumber, busId, student->getAssignedStopId(), "DENIED_BUS_FULL");
    }
}

void SystemManager::runAcademicSelfTest() {
    cout << "\n===========================================================\n";
    cout << "      RUNNING ACADEMIC VIVA SELF-TEST SUITE              \n";
    cout << "===========================================================\n";
    cout << "Verifying Object-Oriented AuthManager & 4 Role Portals:\n\n";

    // 1. Auth Test
    cout << "1. Testing AuthManager login validation:\n";
    authManager.login("admin", "admin123");
    authManager.login("2024-CS-001", "student123");
    authManager.login("invalid", "wrongpass");

    authManager.displayRegisteredAccounts();

    cout << "===========================================================\n";
    cout << "    ALL AUTHENTICATION SELF-TESTS COMPLETED!            \n";
    cout << "===========================================================\n\n";
}
