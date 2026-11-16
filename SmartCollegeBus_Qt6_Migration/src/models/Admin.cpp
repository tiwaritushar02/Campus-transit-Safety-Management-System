#include "Admin.hpp"
#include <iostream>

// Constructor
Admin::Admin(const std::string& id, const std::string& name, const std::string& phone,
             const std::string& email, const std::string& password,
             const std::string& level)
    : User(id, name, phone, email, password, UserRole::ADMIN), m_adminLevel(level) {}

// Function Overriding
// Polymorphism
void Admin::showDashboard(QWidget* container) {
    (void)container;
}

// Function Overriding
// Polymorphism
void Admin::displayDetails() const {
    std::cout << "[ADMIN PROFILE] ID: " << m_userId 
              << " | Name: " << m_fullName 
              << " | Authority Level: " << m_adminLevel << std::endl;
}
