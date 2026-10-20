#ifndef MAINTENANCE_MANAGER_HPP
#define MAINTENANCE_MANAGER_HPP

/**
 * ============================================================================
 * MODULE: Member 2 - Person Hierarchy, Bus & Maintenance Management
 * FILE: MaintenanceManager.hpp
 * 
 * OOP CONCEPTS DEMONSTRATED:
 * 1. STRUCT ENCAPSULATION: BusMaintenanceRecord encapsulates vehicle health parameters.
 * 2. EXCEPTION HANDLING INTEGRATION: Detects expired PUC/Insurance and throws
 *    `MaintenanceOverdueException`.
 * ============================================================================
 */

#include "Exceptions.hpp"
#include <string>
#include <vector>
#include <iostream>

using namespace std;

struct BusMaintenanceRecord {
    string busId;
    string lastServiceDate;
    string nextServiceDueDate;
    string pucExpiryDate;
    string insuranceExpiryDate;
    string conditionIssues; // e.g. "Low Fuel", "Tyre Issue", "AC Problem"
    bool isOperational;
};

class MaintenanceManager {
private:
    vector<BusMaintenanceRecord> records;

public:
    MaintenanceManager();

    void initializeSampleMaintenanceData();

    void logConditionReport(const string& busId, const string& issue);
    void updateMaintenanceDates(const string& busId, const string& nextService,
                                const string& pucExp, const string& insExp);

    void verifyVehicleCompliance(const string& busId) const;
    const BusMaintenanceRecord* getRecord(const string& busId) const;

    void displayFleetMaintenanceOverview() const;
};

#endif // MAINTENANCE_MANAGER_HPP
