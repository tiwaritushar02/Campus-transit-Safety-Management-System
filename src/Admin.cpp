/**
 * ============================================================================
 * MODULE: Member 2 - Person Hierarchy & Admin Management
 * FILE: Admin.cpp
 * ============================================================================
 */

#include "Admin.hpp"
#include <iomanip>

using namespace std;

Admin::Admin(const string& aId, const string& aName, const string& aPhone,
             const string& aEmail, const string& level)
    : Person(aId, aName, aPhone, aEmail), adminLevel(level) {}

void Admin::displayDetails() const {
    cout << "┌─────────────────────────────────────────────────────────┐\n";
    cout << "│                  ADMIN PROFILE                          │\n";
    cout << "├─────────────────────────────────────────────────────────┤\n";
    cout << "│ ID:             " << left << setw(39) << id << "│\n";
    cout << "│ Name:           " << left << setw(39) << name << "│\n";
    cout << "│ Level:          " << left << setw(39) << adminLevel << "│\n";
    cout << "│ Phone:          " << left << setw(39) << phone << "│\n";
    cout << "│ Email:          " << left << setw(39) << email << "│\n";
    cout << "└─────────────────────────────────────────────────────────┘\n";
}

string Admin::getRole() const {
    return "Admin";
}
