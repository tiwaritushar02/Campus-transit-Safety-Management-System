#ifndef DRIVER_HPP
#define DRIVER_HPP

#include "User.hpp"

// Inheritance
class Driver : public User {
private:
    // Encapsulation
    std::string m_licenseNumber;
    int m_experienceYears;
    std::string m_assignedBusId;

public:
    // Constructor
    Driver(const std::string& id, const std::string& name, const std::string& phone,
           const std::string& email, const std::string& password,
           const std::string& license, int exp, const std::string& busId = "Unassigned");

    // Function Overriding
    // Polymorphism
    void showDashboard(QWidget* container) override;

    // Function Overriding
    // Polymorphism
    std::string getRoleString() const override { return "Driver"; }

    // Function Overriding
    // Polymorphism
    void displayDetails() const override;

    // Encapsulation
    std::string getLicenseNumber() const { return m_licenseNumber; }
    int getExperienceYears() const { return m_experienceYears; }
    std::string getAssignedBusId() const { return m_assignedBusId; }

    void setAssignedBusId(const std::string& busId) { m_assignedBusId = busId; }
};

#endif // DRIVER_HPP
