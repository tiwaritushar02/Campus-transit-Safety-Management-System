#ifndef PARENT_HPP
#define PARENT_HPP

#include "User.hpp"

// Inheritance
class Parent : public User {
private:
    // Encapsulation
    std::string m_childRollNumber;
    std::string m_childName;
    double m_feeBalance;
    std::string m_feeDueDate;

public:
    // Constructor
    Parent(const std::string& id, const std::string& name, const std::string& phone,
           const std::string& email, const std::string& password,
           const std::string& childRoll, const std::string& childName = "Ward",
           double feeBalance = 0.0, const std::string& dueDate = "2026-12-31");

    // Function Overriding
    // Polymorphism
    void showDashboard(QWidget* container) override;

    // Function Overriding
    // Polymorphism
    std::string getRoleString() const override { return "Parent"; }

    // Function Overriding
    // Polymorphism
    void displayDetails() const override;

    // Encapsulation
    std::string getChildRollNumber() const { return m_childRollNumber; }
    std::string getChildName() const { return m_childName; }
    double getFeeBalance() const { return m_feeBalance; }
    std::string getFeeDueDate() const { return m_feeDueDate; }

    void setFeeBalance(double balance) { m_feeBalance = balance; }
};

#endif // PARENT_HPP
