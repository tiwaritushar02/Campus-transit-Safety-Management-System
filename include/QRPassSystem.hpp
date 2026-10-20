#ifndef QR_PASS_SYSTEM_HPP
#define QR_PASS_SYSTEM_HPP

/**
 * ============================================================================
 * MODULE: Member 1 - Student & QR Pass Module
 * FILE: QRPassSystem.hpp
 * 
 * OOP CONCEPTS DEMONSTRATED:
 * 1. ENCAPSULATION & STRUCT DATA MODELING: DigitalPass encapsulates ticket metadata.
 * 2. HASH GENERATION & VALIDATION: Generates unique digital bus pass hash & QR code.
 * ============================================================================
 */

#include "Student.hpp"
#include <string>
#include <iostream>

using namespace std;

struct DigitalPass {
    string passId;
    string studentRoll;
    string studentName;
    string routeId;
    string stopId;
    string issueDate;
    string expiryDate;
    string verificationHash;
    bool isValid;

    void displayDigitalPassCard() const;
};

class QRPassSystem {
public:
    // Generate digital bus pass for a student
    static DigitalPass generateBusPass(const Student& student, const string& routeId, const string& stopId);

    // Verify QR pass token at bus entrance
    static bool verifyPass(const DigitalPass& pass, const string& currentBusRouteId);
};

#endif // QR_PASS_SYSTEM_HPP
