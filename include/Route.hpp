#ifndef ROUTE_HPP
#define ROUTE_HPP

/**
 * ============================================================================
 * MODULE: Member 1 - Student & Route Management
 * FILE: Route.hpp
 * 
 * OOP CONCEPTS DEMONSTRATED:
 * 1. ENCAPSULATION: Route properties and stop sequencing.
 * 2. OPERATOR OVERLOADING: `operator<<` for printing complete route details.
 * 3. STL USAGE: `vector<Stop>`, `map<pair<string,string>, double>`
 * ============================================================================
 */

#include "Stop.hpp"
#include <string>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

class Route {
private:
    string routeId;
    string routeName;
    vector<Stop> stops;
    
    // Map storing distance in KM between adjacent stops: key = pair(stopA_Id, stopB_Id)
    map<pair<string, string>, double> stopDistances;

public:
    Route() = default;
    Route(const string& rId, const string& rName);

    // Getters & Setters
    string getRouteId() const { return routeId; }
    string getRouteName() const { return routeName; }
    const vector<Stop>& getStops() const { return stops; }

    void addStop(const Stop& stop);
    void setSegmentDistance(const string& fromStopId, const string& toStopId, double distanceKm);

    double getSegmentDistance(const string& fromStopId, const string& toStopId) const;
    double getTotalRouteDistance() const;

    bool hasStop(const string& stopId) const;
    int getStopIndex(const string& stopId) const;

    void displayRouteDetails(ScheduleType scheduleType) const;

    // Operator Overloading
    friend ostream& operator<<(ostream& os, const Route& route);
};

#endif // ROUTE_HPP
