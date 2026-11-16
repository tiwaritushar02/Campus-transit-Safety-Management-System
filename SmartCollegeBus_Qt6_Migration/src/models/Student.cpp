#include "Student.hpp"
#include <iostream>

// Constructor
Student::Student(const std::string& id, const std::string& name, const std::string& phone,
                 const std::string& email, const std::string& password,
                 const std::string& roll, const std::string& routeId,
                 const std::string& stopId, bool feePaid)
    : User(id, name, phone, email, password, UserRole::STUDENT),
      m_rollNumber(roll), m_assignedRouteId(routeId),
      m_assignedStopId(stopId), m_feeStatus(feePaid) {}

// Function Overriding
// Polymorphism
void Student::showDashboard(QWidget* container) {
    (void)container;
}

// Function Overriding
// Polymorphism
void Student::displayDetails() const {
    std::cout << "[STUDENT PROFILE] Roll: " << m_rollNumber
              << " | Name: " << m_fullName
              << " | Route: " << m_assignedRouteId
              << " | Boarding Stop: " << m_assignedStopId
              << " | Fee Status: " << (m_feeStatus ? "PAID" : "PENDING") << std::endl;
}
