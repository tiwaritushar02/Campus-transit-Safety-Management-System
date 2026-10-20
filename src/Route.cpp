/**
 * ============================================================================
 * MODULE: Member 1 - Student & Route Management
 * FILE: Route.cpp
 * ============================================================================
 */

#include "Route.hpp"
#include <iomanip>

using namespace std;

Route::Route(const string& rId, const string& rName)
    : routeId(rId), routeName(rName) {}

void Route::addStop(const Stop& stop) {
    stops.push_back(stop);
}

void Route::setSegmentDistance(const string& fromStopId, const string& toStopId, double distanceKm) {
    stopDistances[{fromStopId, toStopId}] = distanceKm;
    stopDistances[{toStopId, fromStopId}] = distanceKm; // Bi-directional
}

double Route::getSegmentDistance(const string& fromStopId, const string& toStopId) const {
    auto it = stopDistances.find({fromStopId, toStopId});
    if (it != stopDistances.end()) return it->second;
    return 2.5; // Default distance fallback (2.5 km)
}

double Route::getTotalRouteDistance() const {
    double total = 0.0;
    for (size_t i = 0; i + 1 < stops.size(); ++i) {
        total += getSegmentDistance(stops[i].getStopId(), stops[i + 1].getStopId());
    }
    return total;
}

bool Route::hasStop(const string& stopId) const {
    for (const auto& s : stops) {
        if (s.getStopId() == stopId) return true;
    }
    return false;
}

int Route::getStopIndex(const string& stopId) const {
    for (size_t i = 0; i < stops.size(); ++i) {
        if (stops[i].getStopId() == stopId) return static_cast<int>(i);
    }
    return -1;
}

void Route::displayRouteDetails(ScheduleType scheduleType) const {
    cout << "===========================================================\n";
    cout << "ROUTE DETAILS: [" << routeId << "] " << routeName << "\n";
    cout << "Total Distance: " << getTotalRouteDistance() << " km | Total Stops: " << stops.size() << "\n";
    cout << "Schedule Active: " << ScheduleUtils::toString(scheduleType) << "\n";
    cout << "-----------------------------------------------------------\n";
    for (size_t i = 0; i < stops.size(); ++i) {
        cout << "  " << (i + 1) << ". " << left << setw(25) << stops[i].getStopName()
                  << " (ID: " << stops[i].getStopId() << ")\n";
    }
    cout << "===========================================================\n";
}

ostream& operator<<(ostream& os, const Route& route) {
    os << "[Route " << route.routeId << "] " << route.routeName << " (" << route.stops.size() << " stops)";
    return os;
}
