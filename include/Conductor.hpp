#ifndef CONDUCTOR_HPP
#define CONDUCTOR_HPP

/**
 * ============================================================================
 * MODULE: Member 2 - Bus, Driver & Conductor Management
 * FILE: Conductor.hpp
 * 
 * OOP CONCEPTS DEMONSTRATED:
 * 1. INHERITANCE: Conductor inherits from Person.
 * 2. POLYMORPHISM: Overrides `displayDetails()` and `getRole()`.
 * ============================================================================
 */

#include "Person.hpp"
#include <string>

using namespace std;

class Conductor : public Person {
private:
    string assignedBusId;

public:
    Conductor(const string& cId, const string& cName, const string& cPhone,
              const string& cEmail, const string& busId = "Unassigned");

    void displayDetails() const override;
    string getRole() const override;

    string getAssignedBusId() const { return assignedBusId; }
    void setAssignedBusId(const string& busId) { assignedBusId = busId; }
};

#endif // CONDUCTOR_HPP
