#ifndef ADMIN_HPP
#define ADMIN_HPP

#include "User.hpp"

// Inheritance
class Admin : public User {
private:
    // Encapsulation
    std::string m_adminLevel;

public:
    // Constructor
    Admin(const std::string& id, const std::string& name, const std::string& phone,
          const std::string& email, const std::string& password,
          const std::string& level = "SUPER_ADMIN");

    // Function Overriding
    // Polymorphism
    void showDashboard(QWidget* container) override;

    // Function Overriding
    // Polymorphism
    std::string getRoleString() const override { return "Admin"; }

    // Function Overriding
    // Polymorphism
    void displayDetails() const override;

    // Encapsulation
    std::string getAdminLevel() const { return m_adminLevel; }
};

#endif // ADMIN_HPP
