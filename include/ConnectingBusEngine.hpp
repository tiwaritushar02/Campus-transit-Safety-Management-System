#ifndef CONNECTING_BUS_ENGINE_HPP
#define CONNECTING_BUS_ENGINE_HPP

/**
 * ============================================================================
 * MODULE: Member 4 - Connecting Bus Logic, Attendance & File Handling
 * FILE: ConnectingBusEngine.hpp
 * 
 * CORE INNOVATIVE FEATURE & PROBLEM SOLVER:
 * Solves Problem #3 from Brief: When a student misses their primary assigned bus,
 * this engine searches all other buses passing through that exact stop after the
 * missed departure time, checks seat availability, and sorts them chronologically.
 * 
 * OOP CONCEPTS DEMONSTRATED:
 * 1. ALGORITHMIC SEARCH: Time-stamped edge lookup across bus schedules.
 * 2. EXCEPTION HANDLING: Throws `NoConnectingBusException` if no alternate bus exists.
 * 3. STL CONTAINERS & ALGORITHMS: `vector`, `sort`, Lambda functions.
 * ============================================================================
 */

#include "Bus.hpp"
#include "Stop.hpp"
#include "ScheduleTypes.hpp"
#include "Exceptions.hpp"
#include <string>
#include <vector>

using namespace std;

// Struct representing a suggested connecting bus option
struct ConnectingBusOption {
    string busId;
    string busNumber;
    string routeId;
    string scheduledArrivalTimeStr;
    int arrivalTimeMinutes;
    int availableSeats;
    string driverName;
    string driverPhone;
};

class ConnectingBusEngine {
public:
    // Finds all connecting/alternate buses passing through stopId AFTER missedTimeMinutes
    static vector<ConnectingBusOption> findConnectingBuses(
        const vector<Stop>& allStops,
        const vector<Bus>& allBuses,
        const string& stopId,
        int missedTimeMinutes,
        ScheduleType activeSchedule);
};

#endif // CONNECTING_BUS_ENGINE_HPP
