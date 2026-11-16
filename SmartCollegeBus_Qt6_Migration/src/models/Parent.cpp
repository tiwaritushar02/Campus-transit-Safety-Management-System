#include "Parent.hpp"
#include <iostream>

// Constructor
Parent::Parent(const std::string& id, const std::string& name, const std::string& phone,
               const std::string& email, const std::string& password,
               const std::string& childRoll, const std::string& childName,
               double feeBalance, const std::string& dueDate)
    : User(id, name, phone, email, password, UserRole::PARENT),
      m_childRollNumber(childRoll), m_childName(childName),
      m_feeBalance(feeBalance), m_feeDueDate(dueDate) {}

// Function Overriding
// Polymorphism
void Parent::showDashboard(QWidget* container) {
    (void)container;
}

// Function Overriding
// Polymorphism
void Parent::displayDetails() const {
    std::cout << "[PARENT PROFILE] User: " << m_userId
              << " | Name: " << m_fullName
              << " | Ward: " << m_childName << " (" << m_childRollNumber << ")"
              << " | Transport Fee Balance: INR " << m_feeBalance << std::endl;
}
