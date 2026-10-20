#ifndef USER_ACCOUNT_HPP
#define USER_ACCOUNT_HPP

/**
 * ============================================================================
 * MODULE: Authentication System
 * FILE: UserAccount.hpp
 * 
 * OOP CONCEPTS DEMONSTRATED:
 * 1. ENCAPSULATION (Pillar): Username, password hash, role, and linked Person pointer
 *    are private. Verified via authentication accessor methods.
 * 2. COMPOSITION / ASSOCIATION: UserAccount links to a Person object.
 * ============================================================================
 */

#include "Person.hpp"
#include <string>
#include <memory>

using namespace std;

enum class UserRole {
    ADMIN,
    DRIVER,
    STUDENT,
    PARENT,
    UNAUTHORIZED
};

class UserAccount {
private:
    string username;
    string password;
    UserRole role;
    shared_ptr<Person> personDetails;

public:
    UserAccount(const string& user, const string& pass, UserRole r, shared_ptr<Person> person = nullptr);

    // Getters
    string getUsername() const { return username; }
    UserRole getRole() const { return role; }
    shared_ptr<Person> getPersonDetails() const { return personDetails; }

    string getRoleString() const {
        switch (role) {
            case UserRole::ADMIN:   return "Admin";
            case UserRole::DRIVER:  return "Driver";
            case UserRole::STUDENT: return "Student";
            case UserRole::PARENT:  return "Parent";
            default:                return "Unauthorized";
        }
    }

    // Password verification
    bool validatePassword(const string& inputPassword) const {
        return password == inputPassword;
    }
};

#endif // USER_ACCOUNT_HPP
