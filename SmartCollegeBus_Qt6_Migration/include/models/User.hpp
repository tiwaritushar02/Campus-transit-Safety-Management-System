#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <iostream>

class QWidget;

enum class UserRole {
    ADMIN,
    STUDENT,
    DRIVER,
    PARENT,
    UNAUTHORIZED
};

// Abstraction
class User {
protected:
    // Encapsulation
    std::string m_userId;
    std::string m_fullName;
    std::string m_phone;
    std::string m_email;
    std::string m_password;
    UserRole m_role;

public:
    // Constructor
    User(const std::string& id, const std::string& name, const std::string& phone,
         const std::string& email, const std::string& password, UserRole role);

    // Destructor
    virtual ~User() = default;

    // Encapsulation
    std::string getId() const { return m_userId; }
    std::string getFullName() const { return m_fullName; }
    std::string getPhone() const { return m_phone; }
    std::string getEmail() const { return m_email; }
    UserRole getRole() const { return m_role; }

    void setPhone(const std::string& phone) { m_phone = phone; }
    void setEmail(const std::string& email) { m_email = email; }
    void setFullName(const std::string& name) { m_fullName = name; }

    bool validatePassword(const std::string& inputPassword) const {
        return m_password == inputPassword;
    }

    // Polymorphism
    virtual void showDashboard(QWidget* container) = 0;
    virtual std::string getRoleString() const = 0;
    virtual void displayDetails() const = 0;
};

#endif // USER_HPP
