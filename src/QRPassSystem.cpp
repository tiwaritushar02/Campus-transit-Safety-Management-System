/**
 * ============================================================================
 * MODULE: Member 1 - Student & QR Pass Module
 * FILE: QRPassSystem.cpp
 * ============================================================================
 */

#include "QRPassSystem.hpp"
#include <iomanip>
#include <sstream>

using namespace std;

void DigitalPass::displayDigitalPassCard() const {
    cout << "┌─────────────────────────────────────────────────────────┐\n";
    cout << "│              🎫 DIGITAL STUDENT BUS PASS                │\n";
    cout << "├─────────────────────────────────────────────────────────┤\n";
    cout << "│ Pass ID:        " << left << setw(39) << passId << "│\n";
    cout << "│ Student Name:   " << left << setw(39) << studentName << "│\n";
    cout << "│ Roll Number:    " << left << setw(39) << studentRoll << "│\n";
    cout << "│ Authorized Route:" << left << setw(38) << routeId << "│\n";
    cout << "│ Boarding Stop:  " << left << setw(39) << stopId << "│\n";
    cout << "│ Valid Period:   " << left << setw(39) << (issueDate + " to " + expiryDate) << "│\n";
    cout << "│ Status:         " << left << setw(39) << (isValid ? "ACTIVE / VERIFIED" : "EXPIRED / INVALID") << "│\n";
    cout << "├─────────────────────────────────────────────────────────┤\n";
    cout << "│ QR TOKEN: [" << left << setw(43) << verificationHash << "]│\n";
    cout << "│ [█▀▀▀▀▀█ ▄ █▀▀▀▀▀█]  [SCAN AT ENTRANCE BOARDING]        │\n";
    cout << "│ [█ ███ █ █ █ ███ █]  [COLLEGE TRANSPORT AUTHORITY]      │\n";
    cout << "│ [█▄▄▄▄▄█ █ █▄▄▄▄▄█]                                     │\n";
    cout << "└─────────────────────────────────────────────────────────┘\n";
}

DigitalPass QRPassSystem::generateBusPass(const Student& student, const string& routeId, const string& stopId) {
    DigitalPass pass;
    pass.passId = "PASS-" + student.getRollNumber();
    pass.studentRoll = student.getRollNumber();
    pass.studentName = student.getName();
    pass.routeId = routeId;
    pass.stopId = stopId;
    pass.issueDate = "2026-08-01";
    pass.expiryDate = "2027-06-30";
    pass.isValid = student.isFeePaid();

    // Hash Token generation
    stringstream ss;
    ss << "QR-" << student.getRollNumber() << "-" << routeId << "-VERIFIED";
    pass.verificationHash = ss.str();

    return pass;
}

bool QRPassSystem::verifyPass(const DigitalPass& pass, const string& currentBusRouteId) {
    if (!pass.isValid) return false;
    if (pass.routeId != currentBusRouteId) return false;
    return true;
}
