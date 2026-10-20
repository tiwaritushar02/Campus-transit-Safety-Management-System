/**
 * ============================================================================
 * MODULE: Member 4 - Connecting Bus Logic, Attendance & File Handling
 * FILE: ConnectingBusEngine.cpp
 * ============================================================================
 */

#include "ConnectingBusEngine.hpp"
#include <algorithm>

using namespace std;

vector<ConnectingBusOption> ConnectingBusEngine::findConnectingBuses(
    const vector<Stop>& allStops,
    const vector<Bus>& allBuses,
    const string& stopId,
    int missedTimeMinutes,
    ScheduleType activeSchedule) {

    // 1. Locate the target stop object
    const Stop* targetStop = nullptr;
    for (const auto& s : allStops) {
        if (s.getStopId() == stopId) {
            targetStop = &s;
            break;
        }
    }

    if (!targetStop) {
        throw RouteNotFoundException(stopId);
    }

    vector<ConnectingBusOption> options;

    // 2. Iterate through all buses registered as passing this stop
    for (const auto& busId : targetStop->getPassingBusIds()) {
        int arrivalMinutes = targetStop->getArrivalTimeInMinutes(busId, activeSchedule);

        // Filter: Bus must arrive AFTER the missed time
        if (arrivalMinutes > missedTimeMinutes) {
            // Locate bus object to check seating and driver contact
            const Bus* busPtr = nullptr;
            for (const auto& b : allBuses) {
                if (b.getBusId() == busId) {
                    busPtr = &b;
                    break;
                }
            }

            ConnectingBusOption opt;
            opt.busId = busId;
            opt.scheduledArrivalTimeStr = targetStop->getArrivalTime(busId, activeSchedule);
            opt.arrivalTimeMinutes = arrivalMinutes;

            if (busPtr) {
                opt.busNumber = busPtr->getBusNumber();
                opt.routeId = busPtr->getAssignedRouteId();
                opt.availableSeats = busPtr->getAvailableSeats();
                opt.driverName = busPtr->getDriver() ? busPtr->getDriver()->getName() : "Unassigned";
                opt.driverPhone = busPtr->getDriver() ? busPtr->getDriver()->getPhone() : "N/A";
            } else {
                opt.busNumber = "BUS-" + busId;
                opt.routeId = "Route-1";
                opt.availableSeats = 15;
                opt.driverName = "College Dispatch";
                opt.driverPhone = "+91-9876543210";
            }

            options.push_back(opt);
        }
    }

    // 3. Sort options chronologically by arrival time (STL Algorithm: sort with Lambda)
    sort(options.begin(), options.end(), [](const ConnectingBusOption& a, const ConnectingBusOption& b) {
        return a.arrivalTimeMinutes < b.arrivalTimeMinutes;
    });

    // 4. Exception handling if no connecting bus exists
    if (options.empty()) {
        throw NoConnectingBusException(targetStop->getStopName(), ScheduleUtils::minutesToTimeString(missedTimeMinutes));
    }

    return options;
}
