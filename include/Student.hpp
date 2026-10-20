#ifndef STUDENT_HPP
#define STUDENT_HPP

/**
 * ============================================================================
 * MODULE: Member 1 - Student & Route Management
 * FILE: Student.hpp
 * 
 * OOP CONCEPTS DEMONSTRATED:
 * 1. INHERITANCE (Pillar): Student inherits publicly from Person.
 * 2. POLYMORPHISM (Pillar): Overrides virtual methods `displayDetails()` 
 *    and `getRole()` from base class Person.
 * 3. OPERATOR OVERLOADING: Overloads `operator<<` for stream printing.
 * 4. ENCAPSULATION (Pillar): Fee status, roll number, assigned stop/route
 *    are private with validation getters/setters.
 * ============================================================================
 */

#include "Person.hpp"
#include <string>
#include <iostream>

using namespace std;

class Student : public Person {
private:
    string rollNumber;
    string assignedRouteId;
    string assignedStopId;
    bool feeStatus; // true = Paid, false = Pending

public:
    // Constructor calling Base Class Constructor (Person)
    Student(const string& sId, const string& sName, const string& sPhone,
            const string& sEmail, const string& roll, const string& routeId,
            const string& stopId, bool feePaid = true);

    // Virtual Method Overriding (Polymorphism)
    void displayDetails() const override;
    string getRole() const override;

    // Getters & Setters
    string getRollNumber() const { return rollNumber; }
    string getAssignedRouteId() const { return assignedRouteId; }
    string getAssignedStopId() const { return assignedStopId; }
    bool isFeePaid() const { return feeStatus; }

    void setAssignedRouteId(const string& routeId) { assignedRouteId = routeId; }
    void setAssignedStopId(const string& stopId) { assignedStopId = stopId; }
    void setFeeStatus(bool status) { feeStatus = status; }

    // Operator Overloading (`<<` insertion operator)
    friend ostream& operator<<(ostream& os, const Student& student);
};

#endif // STUDENT_HPP
