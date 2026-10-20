/**
 * ============================================================================
 * MODULE: Member 1 - Student & Route Management
 * FILE: Student.cpp
 * ============================================================================
 */

#include "Student.hpp"
#include <iomanip>

using namespace std;

Student::Student(const string& sId, const string& sName, const string& sPhone,
                 const string& sEmail, const string& roll, const string& routeId,
                 const string& stopId, bool feePaid)
    : Person(sId, sName, sPhone, sEmail),
      rollNumber(roll),
      assignedRouteId(routeId),
      assignedStopId(stopId),
      feeStatus(feePaid) {}

// Polymorphic function implementation
void Student::displayDetails() const {
    cout << "┌─────────────────────────────────────────────────────────┐\n";
    cout << "│                 STUDENT PROFILE                         │\n";
    cout << "├─────────────────────────────────────────────────────────┤\n";
    cout << "│ ID:             " << left << setw(39) << id << "│\n";
    cout << "│ Name:           " << left << setw(39) << name << "│\n";
    cout << "│ Roll Number:    " << left << setw(39) << rollNumber << "│\n";
    cout << "│ Contact:        " << left << setw(39) << phone << "│\n";
    cout << "│ Email:          " << left << setw(39) << email << "│\n";
    cout << "│ Assigned Route: " << left << setw(39) << assignedRouteId << "│\n";
    cout << "│ Assigned Stop:  " << left << setw(39) << assignedStopId << "│\n";
    cout << "│ Bus Fee Status: " << left << setw(39) << (feeStatus ? "PAID (Active Pass)" : "PENDING (Pass On Hold)") << "│\n";
    cout << "└─────────────────────────────────────────────────────────┘\n";
}

string Student::getRole() const {
    return "Student";
}

// Operator Overloading Implementation
ostream& operator<<(ostream& os, const Student& student) {
    os << "[Student] Roll: " << student.rollNumber 
       << " | Name: " << student.name 
       << " | Route: " << student.assignedRouteId 
       << " | Stop: " << student.assignedStopId
       << " | Fee: " << (student.feeStatus ? "Paid" : "Pending");
    return os;
}
