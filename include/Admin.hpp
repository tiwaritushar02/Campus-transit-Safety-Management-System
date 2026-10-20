#ifndef ADMIN_HPP
#define ADMIN_HPP

/**
 * ============================================================================
 * MODULE: Member 2 - Person Hierarchy & Admin Management
 * FILE: Admin.hpp
 * 
 * OOP CONCEPTS DEMONSTRATED:
 * 1. INHERITANCE: Admin inherits from Person.
 * 2. POLYMORPHISM: Overrides `displayDetails()` and `getRole()`.
 * 3. ROLE-BASED ACCESS CONTROL (RBAC): Encapsulates admin privileges for updating
 *    buses, timings, and routes.
 * ============================================================================
 */

#include "Person.hpp"
#include <string>

using namespace std;

class Admin : public Person {
private:
    string adminLevel; // "SUPER_ADMIN", "TRANSPORT_HEAD"

public:
    Admin(const string& aId, const string& aName, const string& aPhone,
          const string& aEmail, const string& level = "TRANSPORT_HEAD");

    void displayDetails() const override;
    string getRole() const override;

    string getAdminLevel() const { return adminLevel; }
};

#endif // ADMIN_HPP
