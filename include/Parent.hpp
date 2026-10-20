#ifndef PARENT_HPP
#define PARENT_HPP

/**
 * ============================================================================
 * MODULE: Member 4 - Complaint, Parent Portal & File Handling
 * FILE: Parent.hpp
 * 
 * OOP CONCEPTS DEMONSTRATED:
 * 1. INHERITANCE: Parent inherits from Person.
 * 2. POLYMORPHISM: Overrides `displayDetails()` and `getRole()`.
 * 3. ENCAPSULATION: Child roll number, linked bus, fee balance.
 * ============================================================================
 */

#include "Person.hpp"
#include <string>

using namespace std;

class Parent : public Person {
private:
    string childRollNumber;
    string childName;
    double feeBalance;
    string feeDueDate;

public:
    Parent(const string& pId, const string& pName, const string& pPhone,
           const string& pEmail, const string& childRoll, const string& cName = "Child Student",
           double fee = 0.0, const string& dueDate = "2026-09-01");

    void displayDetails() const override;
    string getRole() const override;

    // Getters & Setters
    string getChildRollNumber() const { return childRollNumber; }
    string getChildName() const { return childName; }
    double getFeeBalance() const { return feeBalance; }
    string getFeeDueDate() const { return feeDueDate; }

    void setFeeBalance(double balance) { feeBalance = balance; }
};

#endif // PARENT_HPP
