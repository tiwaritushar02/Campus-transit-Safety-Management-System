#ifndef STUDENT_HPP
#define STUDENT_HPP

#include "User.hpp"

// Inheritance
class Student : public User {
private:
    // Encapsulation
    std::string m_rollNumber;
    std::string m_assignedRouteId;
    std::string m_assignedStopId;
    bool m_feeStatus;

public:
    // Constructor
    Student(const std::string& id, const std::string& name, const std::string& phone,
            const std::string& email, const std::string& password,
            const std::string& roll, const std::string& routeId,
            const std::string& stopId, bool feePaid = true);

    // Function Overriding
    // Polymorphism
    void showDashboard(QWidget* container) override;

    // Function Overriding
    // Polymorphism
    std::string getRoleString() const override { return "Student"; }

    // Function Overriding
    // Polymorphism
    void displayDetails() const override;

    // Encapsulation
    std::string getRollNumber() const { return m_rollNumber; }
    std::string getAssignedRouteId() const { return m_assignedRouteId; }
    std::string getAssignedStopId() const { return m_assignedStopId; }
    bool isFeePaid() const { return m_feeStatus; }

    void setAssignedRouteId(const std::string& rId) { m_assignedRouteId = rId; }
    void setAssignedStopId(const std::string& sId) { m_assignedStopId = sId; }
    void setFeeStatus(bool status) { m_feeStatus = status; }
};

#endif // STUDENT_HPP
