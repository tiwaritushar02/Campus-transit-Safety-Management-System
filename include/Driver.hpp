#ifndef DRIVER_HPP
#define DRIVER_HPP

/**
 * ============================================================================
 * MODULE: Member 2 - Bus, Driver & Conductor Management
 * FILE: Driver.hpp
 * 
 * OOP CONCEPTS DEMONSTRATED:
 * 1. INHERITANCE: Driver inherits from Person.
 * 2. POLYMORPHISM: Overrides `displayDetails()` and `getRole()`.
 * 3. ENCAPSULATION: License number, experience, assigned bus ID.
 * ============================================================================
 */

#include "Person.hpp"
#include <string>

using namespace std;

class Driver : public Person {
private:
    string licenseNumber;
    int experienceYears;
    string assignedBusId;

public:
    Driver(const string& dId, const string& dName, const string& dPhone,
           const string& dEmail, const string& license, int exp,
           const string& busId = "Unassigned");

    void displayDetails() const override;
    string getRole() const override;

    // Getters & Setters
    string getLicenseNumber() const { return licenseNumber; }
    int getExperienceYears() const { return experienceYears; }
    string getAssignedBusId() const { return assignedBusId; }

    void setAssignedBusId(const string& busId) { assignedBusId = busId; }
};

#endif // DRIVER_HPP
