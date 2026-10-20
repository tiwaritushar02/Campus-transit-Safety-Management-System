/**
 * ============================================================================
 * MODULE: Member 2 - Bus, Driver & Conductor Management
 * FILE: Bus.cpp
 * ============================================================================
 */

#include "Bus.hpp"
#include <iomanip>

using namespace std;

Bus::Bus(const string& bId, const string& bNumber, int cap,
         const string& routeId, Driver* d, Conductor* c)
    : busId(bId),
      busNumber(bNumber),
      capacity(cap),
      currentOccupancy(0),
      assignedRouteId(routeId),
      currentLocationStopId("Terminal"),
      driver(d),
      conductor(c) {}

void Bus::boardStudent() {
    if (currentOccupancy >= capacity) {
        // Exception Handling: Throw SeatFullException if bus is full
        throw SeatFullException(busId, capacity);
    }
    currentOccupancy++;
}

void Bus::unboardStudent() {
    if (currentOccupancy > 0) {
        currentOccupancy--;
    }
}

void Bus::displayBusStatus() const {
    cout << "┌─────────────────────────────────────────────────────────┐\n";
    cout << "│                    BUS STATUS LOG                       │\n";
    cout << "├─────────────────────────────────────────────────────────┤\n";
    cout << "│ Bus ID:         " << left << setw(39) << busId << "│\n";
    cout << "│ Registration:   " << left << setw(39) << busNumber << "│\n";
    cout << "│ Route Assigned: " << left << setw(39) << assignedRouteId << "│\n";
    cout << "│ Current Stop:   " << left << setw(39) << currentLocationStopId << "│\n";
    cout << "│ Seating:        " << left << setw(39) 
              << (to_string(currentOccupancy) + "/" + to_string(capacity) + " Seats Occupied") << "│\n";
    cout << "│ Driver:         " << left << setw(39) << (driver ? driver->getName() + " (" + driver->getPhone() + ")" : "Not Assigned") << "│\n";
    cout << "│ Conductor:      " << left << setw(39) << (conductor ? conductor->getName() + " (" + conductor->getPhone() + ")" : "Not Assigned") << "│\n";
    cout << "└─────────────────────────────────────────────────────────┘\n";
}

ostream& operator<<(ostream& os, const Bus& bus) {
    os << "[Bus " << bus.busId << "] No: " << bus.busNumber
       << " | Route: " << bus.assignedRouteId
       << " | Location: " << bus.currentLocationStopId
       << " | Seats: " << bus.currentOccupancy << "/" << bus.capacity;
    return os;
}
