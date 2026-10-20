/**
 * ============================================================================
 * MODULE: Member 4 - Complaint, Parent Portal & File Handling
 * FILE: Parent.cpp
 * ============================================================================
 */

#include "Parent.hpp"
#include <iomanip>

using namespace std;

Parent::Parent(const string& pId, const string& pName, const string& pPhone,
               const string& pEmail, const string& childRoll, const string& cName,
               double fee, const string& dueDate)
    : Person(pId, pName, pPhone, pEmail),
      childRollNumber(childRoll),
      childName(cName),
      feeBalance(fee),
      feeDueDate(dueDate) {}

void Parent::displayDetails() const {
    cout << "┌─────────────────────────────────────────────────────────┐\n";
    cout << "│                  PARENT PROFILE                         │\n";
    cout << "├─────────────────────────────────────────────────────────┤\n";
    cout << "│ Parent ID:      " << left << setw(39) << id << "│\n";
    cout << "│ Parent Name:    " << left << setw(39) << name << "│\n";
    cout << "│ Contact Phone:  " << left << setw(39) << phone << "│\n";
    cout << "│ Child Student:  " << left << setw(39) << (childName + " (" + childRollNumber + ")") << "│\n";
    cout << "│ Transport Fee:  " << left << setw(39) << (feeBalance == 0.0 ? "PAID IN FULL" : ("Balance ₹" + to_string(feeBalance) + " Due: " + feeDueDate)) << "│\n";
    cout << "└─────────────────────────────────────────────────────────┘\n";
}

string Parent::getRole() const {
    return "Parent";
}
