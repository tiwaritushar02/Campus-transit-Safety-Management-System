/**
 * ============================================================================
 * MODULE: Member 2 - Bus, Driver & Conductor Management
 * FILE: Conductor.cpp
 * ============================================================================
 */

#include "Conductor.hpp"
#include <iomanip>

using namespace std;

Conductor::Conductor(const string& cId, const string& cName, const string& cPhone,
                     const string& cEmail, const string& busId)
    : Person(cId, cName, cPhone, cEmail),
      assignedBusId(busId) {}

void Conductor::displayDetails() const {
    cout << "┌─────────────────────────────────────────────────────────┐\n";
    cout << "│                 CONDUCTOR PROFILE                       │\n";
    cout << "├─────────────────────────────────────────────────────────┤\n";
    cout << "│ ID:             " << left << setw(39) << id << "│\n";
    cout << "│ Name:           " << left << setw(39) << name << "│\n";
    cout << "│ Phone:          " << left << setw(39) << phone << "│\n";
    cout << "│ Assigned Bus:   " << left << setw(39) << assignedBusId << "│\n";
    cout << "└─────────────────────────────────────────────────────────┘\n";
}

string Conductor::getRole() const {
    return "Conductor";
}
