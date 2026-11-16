#include "Driver.hpp"
#include <iostream>

// Constructor
Driver::Driver(const std::string& id, const std::string& name, const std::string& phone,
               const std::string& email, const std::string& password,
               const std::string& license, int exp, const std::string& busId)
    : User(id, name, phone, email, password, UserRole::DRIVER),
      m_licenseNumber(license), m_experienceYears(exp), m_assignedBusId(busId) {}

// Function Overriding
// Polymorphism
void Driver::showDashboard(QWidget* container) {
    (void)container;
}

// Function Overriding
// Polymorphism
void Driver::displayDetails() const {
    std::cout << "[DRIVER PROFILE] ID: " << m_userId
              << " | Name: " << m_fullName
              << " | License: " << m_licenseNumber
              << " | Experience: " << m_experienceYears << " yrs"
              << " | Assigned Bus: " << m_assignedBusId << std::endl;
}
