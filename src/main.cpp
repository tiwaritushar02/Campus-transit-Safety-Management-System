/**
 * ============================================================================
 * PROJECT: Smart College Bus Management & Route Optimization System
 * ENTRY POINT: main.cpp (Class & Object-Based Multi-Role Login System)
 * ============================================================================
 */

#include "SystemManager.hpp"
#include "ScheduleTypes.hpp"
#include <iostream>
#include <string>
#include <limits>

using namespace std;

void displayBanner() {
    cout << "\033[1;36m";
    cout << "===========================================================================\n";
    cout << "  🚌 SMART COLLEGE BUS MANAGEMENT & ROUTE OPTIMIZATION SYSTEM (C++ OOP)   \n";
    cout << "  🔒 Object-Oriented Multi-Role Authorization & Login System               \n";
    cout << "===========================================================================\n";
    cout << "\033[0m";
}

void runAdminPortal(SystemManager& sysManager) {
    int choice = -1;
    while (choice != 0) {
        cout << "\n\033[1;31m--- 🔴 ADMIN CONTROL PORTAL [Active Mode: " 
                  << ScheduleUtils::toString(sysManager.getScheduleType()) << "] ---\033[0m\n";
        cout << " [1] 📊 View Transport System Analytics Dashboard\n";
        cout << " [2] 🚌 Update Bus Details (Registration No, Seating Capacity, Route)\n";
        cout << " [3] ⏰ Update Stop Departure Timings (Half-Day / Full-Day)\n";
        cout << " [4] ➕ Add New Bus to Fleet\n";
        cout << " [5] 📢 Dispatch Broadcast Notice (Students/Parents/Drivers)\n";
        cout << " [6] 🚨 View Complaints, SOS Alerts & Lost Items\n";
        cout << " [7] 🔧 View Fleet Maintenance & Compliance Schedule\n";
        cout << " [8] 🗓️  Switch System Schedule Mode (Full-Day vs Half-Day)\n";
        cout << " [0] 🔒 Logout from Admin Portal\n";
        cout << "Enter Admin Choice (0-8): ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1:
                sysManager.displayAnalyticsDashboard();
                break;
            case 2: {
                cout << "Enter Bus ID to update (e.g. B-101): ";
                string busId; cin >> busId;
                cout << "Enter New Registration Number: ";
                string regNo; cin >> regNo;
                cout << "Enter New Capacity (or 0): ";
                int cap; cin >> cap;
                cout << "Enter Assigned Route ID: ";
                string routeId; cin >> routeId;

                sysManager.adminUpdateBus(busId, regNo, cap, routeId);
                break;
            }
            case 3: {
                cout << "Enter Stop ID (ST101, ST102): ";
                string stopId; cin >> stopId;
                cout << "Enter Bus ID (B-101): ";
                string busId; cin >> busId;
                cout << "Schedule (1. Full-Day, 2. Half-Day): ";
                int sOpt; cin >> sOpt;
                cout << "New Time (HH:MM): ";
                string newTime; cin >> newTime;

                sysManager.adminUpdateStopTiming(stopId, busId, (sOpt == 2 ? ScheduleType::HALF_DAY : ScheduleType::FULL_DAY), newTime);
                break;
            }
            case 4: {
                cout << "New Bus ID: "; string bId; cin >> bId;
                cout << "Reg No: "; string reg; cin >> reg;
                cout << "Capacity: "; int cap; cin >> cap;
                cout << "Route ID: "; string rId; cin >> rId;
                sysManager.adminAddNewBus(bId, reg, cap, rId);
                break;
            }
            case 5: {
                cout << "Target (1. All Students, 2. All Parents, 3. Drivers): ";
                int tOpt; cin >> tOpt;
                cin.ignore();
                cout << "Notice Title: "; string title; getline(cin, title);
                cout << "Message Body: "; string msg; getline(cin, msg);
                sysManager.adminBroadcastNotice(tOpt == 2 ? TargetAudience::ALL_PARENTS : TargetAudience::ALL_STUDENTS, title, msg);
                break;
            }
            case 6:
                sysManager.getComplaintEngine().displayAllComplaints();
                sysManager.getComplaintEngine().displayLostAndFound();
                break;
            case 7:
                sysManager.getMaintenanceManager().displayFleetMaintenanceOverview();
                break;
            case 8: {
                cout << "Select Mode (1. Full-Day, 2. Half-Day): ";
                int mode; cin >> mode;
                sysManager.setScheduleType(mode == 2 ? ScheduleType::HALF_DAY : ScheduleType::FULL_DAY);
                break;
            }
            case 0:
                sysManager.logout();
                break;
            default:
                break;
        }
    }
}

void runDriverPortal(SystemManager& sysManager, const string& driverId) {
    int choice = -1;
    while (choice != 0) {
        cout << "\n\033[1;33m--- 🟡 DRIVER PORTAL [Driver: " << driverId << "] ---\033[0m\n";
        cout << " [1] 🚀 Start Trip\n";
        cout << " [2] 🏁 End Trip\n";
        cout << " [3] ⚠️ Report Delay (Traffic, Weather, Breakdown)\n";
        cout << " [4] 🛑 Report Student Misbehavior (Vandalism, Fighting)\n";
        cout << " [5] 🔧 Report Bus Condition Issue (Tyre, Engine, AC)\n";
        cout << " [0] 🔒 Logout\n";
        cout << "Enter Choice (0-5): ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1:
                sysManager.driverStartTrip("B-101");
                break;
            case 2:
                sysManager.driverEndTrip("B-101");
                break;
            case 3: {
                cout << "Reason (Traffic/Breakdown/Weather): ";
                string reason; cin >> reason;
                cout << "Minutes Delayed: ";
                int mins; cin >> mins;
                sysManager.driverReportDelay("B-101", reason, mins);
                break;
            }
            case 4: {
                cout << "Student Roll No: "; string roll; cin >> roll;
                cin.ignore();
                cout << "Description of Issue: "; string issue; getline(cin, issue);
                sysManager.driverReportMisbehavior(driverId, roll, issue);
                break;
            }
            case 5: {
                cout << "Issue (Low Fuel/Tyre/AC/Engine): "; string issue; cin >> issue;
                sysManager.driverReportBusCondition("B-101", issue);
                break;
            }
            case 0:
                sysManager.logout();
                break;
        }
    }
}

void runStudentPortal(SystemManager& sysManager, const string& studentRoll) {
    int choice = -1;
    while (choice != 0) {
        cout << "\n\033[1;32m--- 🟢 STUDENT / USER PORTAL [Roll: " << studentRoll << "] ---\033[0m\n";
        cout << " [1] 🎫 View My Digital QR Bus Pass\n";
        cout << " [2] 🚏 View Morning Departure Time\n";
        cout << " [3] 🚨 Missed Bus? Search Connecting Bus\n";
        cout << " [4] ⭐ Rate Driver (1-5 Stars) & Journey Feedback\n";
        cout << " [5] 📢 Report Driver Issue (Rash driving, Overspeeding, Phone)\n";
        cout << " [6] 🚨 SOS EMERGENCY BUTTON (Instant Admin Alert)\n";
        cout << " [0] 🔒 Logout\n";
        cout << "Enter Choice (0-6): ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1: {
                DigitalPass pass = sysManager.getStudentPass(studentRoll);
                pass.displayDigitalPassCard();
                break;
            }
            case 2:
                sysManager.checkStudentBusTiming(studentRoll);
                break;
            case 3: {
                cout << "Stop ID (ST101): "; string stopId; cin >> stopId;
                cout << "Missed Time (HH:MM): "; string timeStr; cin >> timeStr;
                sysManager.searchConnectingBuses(studentRoll, stopId, timeStr);
                break;
            }
            case 4: {
                cout << "Driver ID (D-01): "; string dId; cin >> dId;
                cout << "Rating Stars (1-5): "; int stars; cin >> stars;
                cin.ignore();
                cout << "Feedback: "; string fb; getline(cin, fb);
                sysManager.studentRateDriver(studentRoll, dId, stars, fb);
                break;
            }
            case 5: {
                cout << "Category (Rash Driving / Phone Usage / Overspeeding): "; string cat; cin >> cat;
                cin.ignore();
                cout << "Details: "; string det; getline(cin, det);
                sysManager.studentReportDriver(studentRoll, "B-101", cat, det);
                break;
            }
            case 6:
                sysManager.triggerStudentSOS(studentRoll, "B-101", "Sector 17 Terminal");
                break;
            case 0:
                sysManager.logout();
                break;
        }
    }
}

void runParentPortal(SystemManager& sysManager, const string& parentQuery) {
    int choice = -1;
    while (choice != 0) {
        cout << "\n\033[1;34m--- 🔵 PARENT PORTAL ---\033[0m\n";
        cout << " [1] 📡 Track Child's Bus & Live ETA\n";
        cout << " [2] 🔔 View Boarding & Drop Notifications\n";
        cout << " [3] 💳 View Transport Fee Status & Due Date\n";
        cout << " [0] 🔒 Logout\n";
        cout << "Enter Choice (0-3): ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1:
                sysManager.checkChildStatusForParent(parentQuery);
                break;
            case 2:
                sysManager.getNotificationCenter().displayNotificationLog();
                break;
            case 3: {
                Parent* p = sysManager.findParentByPhoneOrChild(parentQuery);
                if (p) p->displayDetails();
                break;
            }
            case 0:
                sysManager.logout();
                break;
        }
    }
}

int main(int argc, char* argv[]) {
    SystemManager sysManager;

    if (argc > 1 && string(argv[1]) == "--test") {
        sysManager.runAcademicSelfTest();
        return 0;
    }

    displayBanner();

    bool running = true;
    while (running) {
        cout << "\n\033[1;35m--- 🔒 LOGIN & AUTHORIZATION SCREEN ---\033[0m\n";
        cout << "Default Credentials:\n";
        cout << "  🔴 Admin:   admin       / admin123\n";
        cout << "  🟡 Driver:  D-01        / driver123\n";
        cout << "  🟢 Student: 2024-CS-001 / student123\n";
        cout << "  🔵 Parent:  P-101       / parent123\n";
        cout << "-----------------------------------------------------------\n";
        cout << "Enter Username / ID (or 'exit' to quit, 'test' for self-test): ";
        
        string username;
        if (!(cin >> username)) break;

        if (username == "exit" || username == "0") {
            cout << "Exiting system. Have a great day!\n";
            break;
        }
        if (username == "test") {
            sysManager.runAcademicSelfTest();
            continue;
        }

        cout << "Enter Password: ";
        string password;
        cin >> password;

        UserSession session = sysManager.login(username, password);
        if (!session.isAuthenticated || !session.activeAccount) {
            cout << "❌ Login failed! Please check credentials.\n";
            continue;
        }

        UserAccount* acc = session.activeAccount;
        UserRole role = acc->getRole();

        switch (role) {
            case UserRole::ADMIN:
                runAdminPortal(sysManager);
                break;
            case UserRole::DRIVER:
                runDriverPortal(sysManager, acc->getUsername());
                break;
            case UserRole::STUDENT:
                runStudentPortal(sysManager, acc->getUsername());
                break;
            case UserRole::PARENT:
                runParentPortal(sysManager, acc->getUsername());
                break;
            default:
                break;
        }
    }

    return 0;
}
