/**
 * ============================================================================
 * MODULE: Member 2 - Bus, Driver & Conductor Management
 * FILE: Driver.cpp
 * ============================================================================
 */

#include "Driver.hpp"
#include <iomanip>

using namespace std;

Driver::Driver(const string& dId, const string& dName, const string& dPhone,
               const string& dEmail, const string& license, int exp,
               const string& busId)
    : Person(dId, dName, dPhone, dEmail),
      licenseNumber(license),
      experienceYears(exp),
      assignedBusId(busId) {}

void Driver::displayDetails() const {
    cout << "┌─────────────────────────────────────────────────────────┐\n";
    cout << "│                  DRIVER PROFILE                         │\n";
    cout << "├─────────────────────────────────────────────────────────┤\n";
    cout << "│ ID:             " << left << setw(39) << id << "│\n";
    cout << "│ Name:           " << left << setw(39) << name << "│\n";
    cout << "│ License No:     " << left << setw(39) << licenseNumber << "│\n";
    cout << "│ Phone:          " << left << setw(39) << phone << "│\n";
    cout << "│ Experience:     " << left << setw(39) << (to_string(experienceYears) + " Years") << "│\n";
    cout << "│ Assigned Bus:   " << left << setw(39) << assignedBusId << "│\n";
    cout << "└─────────────────────────────────────────────────────────┘\n";
}

string Driver::getRole() const {
    return "Driver";
}
