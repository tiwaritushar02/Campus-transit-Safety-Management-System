/**
 * ============================================================================
 * MODULE: Member 2 - Person Hierarchy, Bus & Maintenance Management
 * FILE: MaintenanceManager.cpp
 * ============================================================================
 */

#include "MaintenanceManager.hpp"
#include <iomanip>

using namespace std;

MaintenanceManager::MaintenanceManager() {
    initializeSampleMaintenanceData();
}

void MaintenanceManager::initializeSampleMaintenanceData() {
    records = {
        {"B-101", "2026-07-01", "2026-10-01", "2027-01-15", "2027-04-30", "Good - All Systems Nominal", true},
        {"B-102", "2026-06-15", "2026-09-15", "2026-12-01", "2027-03-20", "Scheduled AC Filter Cleaning", true},
        {"B-103", "2026-07-20", "2026-10-20", "2027-02-10", "2027-05-15", "Good - New Tyres Installed", true}
    };
}

void MaintenanceManager::logConditionReport(const string& busId, const string& issue) {
    for (auto& r : records) {
        if (r.busId == busId) {
            r.conditionIssues += " | Reported: " + issue;
            cout << "🔧 [MAINTENANCE LOGGED] Bus [" << busId << "] Issue logged: " << issue << "\n";
            return;
        }
    }
    records.push_back({busId, "2026-08-01", "2026-11-01", "2027-01-01", "2027-01-01", issue, true});
}

void MaintenanceManager::updateMaintenanceDates(const string& busId, const string& nextService,
                                                 const string& pucExp, const string& insExp) {
    for (auto& r : records) {
        if (r.busId == busId) {
            r.nextServiceDueDate = nextService;
            r.pucExpiryDate = pucExp;
            r.insuranceExpiryDate = insExp;
            cout << "✅ Maintenance dates updated for Bus " << busId << "\n";
            return;
        }
    }
}

void MaintenanceManager::verifyVehicleCompliance(const string& busId) const {
    const auto* rec = getRecord(busId);
    if (!rec) return;

    if (!rec->isOperational) {
        throw BusSystemException("Vehicle Out of Service: Bus [" + busId + "] is marked unfit for operation!");
    }
}

const BusMaintenanceRecord* MaintenanceManager::getRecord(const string& busId) const {
    for (const auto& r : records) {
        if (r.busId == busId) return &r;
    }
    return nullptr;
}

void MaintenanceManager::displayFleetMaintenanceOverview() const {
    cout << "\n===========================================================\n";
    cout << "           BUS FLEET MAINTENANCE & COMPLIANCE LOG         \n";
    cout << "===========================================================\n";
    cout << left << setw(10) << "BUS ID"
              << setw(15) << "LAST SERVICE"
              << setw(15) << "NEXT SERVICE"
              << setw(15) << "PUC EXPIRY"
              << "CONDITION STATUS\n";
    cout << "-----------------------------------------------------------\n";

    for (const auto& r : records) {
        cout << left << setw(10) << r.busId
                  << setw(15) << r.lastServiceDate
                  << setw(15) << r.nextServiceDueDate
                  << setw(15) << r.pucExpiryDate
                  << r.conditionIssues << "\n";
    }
    cout << "===========================================================\n\n";
}
