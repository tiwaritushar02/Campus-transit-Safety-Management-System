#ifndef PERSON_HPP
#define PERSON_HPP

/**
 * ============================================================================
 * MODULE: Member 2 - Bus, Driver & Conductor Management
 * FILE: Person.hpp
 * 
 * OOP CONCEPTS DEMONSTRATED:
 * 1. ABSTRACTION (Pillar): Person is an Abstract Base Class (ABC) containing
 *    a pure virtual function `displayDetails()`. It cannot be instantiated directly.
 * 2. ENCAPSULATION (Pillar): Protected member attributes (id, name, phone, email)
 *    accessed safely through public getter and setter methods.
 * 3. VIRTUAL DESTRUCTOR: Ensures proper clean-up of derived objects (Student,
 *    Driver, Conductor) when deleted via a base class pointer.
 * ============================================================================
 */

#include <string>
#include <iostream>

using namespace std;

class Person {
protected:
    // Encapsulation: Protected attributes accessible by derived classes
    string id;
    string name;
    string phone;
    string email;

public:
    // Parameterized Constructor
    Person(const string& pId, const string& pName, 
           const string& pPhone, const string& pEmail)
        : id(pId), name(pName), phone(pPhone), email(pEmail) {}

    // Virtual Destructor (Essential for Abstract Base Classes)
    virtual ~Person() = default;

    // Getters & Setters (Encapsulation)
    string getId() const { return id; }
    string getName() const { return name; }
    string getPhone() const { return phone; }
    string getEmail() const { return email; }

    void setName(const string& pName) { name = pName; }
    void setPhone(const string& pPhone) { phone = pPhone; }
    void setEmail(const string& pEmail) { email = pEmail; }

    // Pure Virtual Function (Abstraction & Polymorphism contract)
    // Every derived class (Student, Driver, Conductor) MUST override this method.
    virtual void displayDetails() const = 0;

    // Pure Virtual Function returning role description string
    virtual string getRole() const = 0;
};

#endif // PERSON_HPP
