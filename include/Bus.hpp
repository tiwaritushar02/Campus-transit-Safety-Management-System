#ifndef BUS_HPP
#define BUS_HPP

/**
 * ============================================================================
 * MODULE: Member 2 - Bus, Driver & Conductor Management
 * FILE: Bus.hpp
 * ============================================================================
 */

#include "Driver.hpp"
#include "Conductor.hpp"
#include "Exceptions.hpp"
#include <string>
#include <iostream>

using namespace std;

class Bus {
private:
    string busId;
    string busNumber;
    int capacity;
    int currentOccupancy;
    string assignedRouteId;
    string currentLocationStopId;
    
    // Pointers to Driver & Conductor (Association)
    Driver* driver;
    Conductor* conductor;

public:
    Bus(const string& bId, const string& bNumber, int cap,
        const string& routeId = "Unassigned",
        Driver* d = nullptr, Conductor* c = nullptr);

    // Getters
    string getBusId() const { return busId; }
    string getBusNumber() const { return busNumber; }
    int getCapacity() const { return capacity; }
    int getCurrentOccupancy() const { return currentOccupancy; }
    int getAvailableSeats() const { return capacity - currentOccupancy; }
    string getAssignedRouteId() const { return assignedRouteId; }
    string getCurrentLocationStopId() const { return currentLocationStopId; }

    Driver* getDriver() const { return driver; }
    Conductor* getConductor() const { return conductor; }

    // Admin Mutators (Encapsulation)
    void setBusNumber(const string& number) { busNumber = number; }
    void setCapacity(int cap) { if (cap > 0) capacity = cap; }
    void setDriver(Driver* d) { driver = d; }
    void setConductor(Conductor* c) { conductor = c; }
    void setAssignedRouteId(const string& routeId) { assignedRouteId = routeId; }
    void setCurrentLocationStopId(const string& stopId) { currentLocationStopId = stopId; }

    // Boarding Logic with Exception Handling
    void boardStudent();
    void unboardStudent();
    bool isFull() const { return currentOccupancy >= capacity; }

    void displayBusStatus() const;

    // Operator Overloading
    friend ostream& operator<<(ostream& os, const Bus& bus);
};

#endif // BUS_HPP
