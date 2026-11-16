#ifndef BUS_HPP
#define BUS_HPP

#include <string>

// Encapsulation
class Bus {
private:
    std::string m_busId;
    std::string m_registrationNumber;
    int m_capacity;
    std::string m_assignedRouteId;
    int m_currentOccupancy;

public:
    // Constructor
    Bus(const std::string& id = "", const std::string& reg = "", int cap = 45, const std::string& routeId = "R1")
        : m_busId(id), m_registrationNumber(reg), m_capacity(cap),
          m_assignedRouteId(routeId), m_currentOccupancy(0) {}

    // Encapsulation
    std::string getBusId() const { return m_busId; }
    std::string getRegistrationNumber() const { return m_registrationNumber; }
    int getCapacity() const { return m_capacity; }
    std::string getAssignedRouteId() const { return m_assignedRouteId; }
    int getCurrentOccupancy() const { return m_currentOccupancy; }
    int getAvailableSeats() const { return m_capacity - m_currentOccupancy; }

    void setRegistrationNumber(const std::string& reg) { m_registrationNumber = reg; }
    void setCapacity(int cap) { m_capacity = cap; }
    void setAssignedRouteId(const std::string& rId) { m_assignedRouteId = rId; }
    bool boardPassenger() {
        if (m_currentOccupancy < m_capacity) {
            m_currentOccupancy++;
            return true;
        }
        return false;
    }
};

#endif // BUS_HPP
