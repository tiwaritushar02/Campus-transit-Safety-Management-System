#include "FleetManager.hpp"

FleetManager::FleetManager(FileManager& fm) : m_fileManager(fm) {
    reload();
}

void FleetManager::reload() {
    m_buses = m_fileManager.loadBuses();
    m_routes = m_fileManager.loadRoutes();
    m_stops = m_fileManager.loadStops();
}

int FleetManager::getTotalAvailableSeats() const {
    int total = 0;
    for (const auto& b : m_buses) {
        total += b.getAvailableSeats();
    }
    return total;
}

bool FleetManager::updateBusConfig(const std::string& busId, const std::string& reg, int cap, const std::string& routeId) {
    for (auto& b : m_buses) {
        if (b.getBusId() == busId) {
            b.setRegistrationNumber(reg);
            b.setCapacity(cap);
            b.setAssignedRouteId(routeId);
            m_fileManager.saveBuses(m_buses);
            return true;
        }
    }
    return false;
}

bool FleetManager::updateStopTiming(const std::string& stopId, const std::string& newTime, bool isFullDay) {
    for (auto& s : m_stops) {
        if (s.getStopId() == stopId) {
            if (isFullDay) s.setFullDayTimes(newTime);
            else s.setHalfDayTimes(newTime);
            m_fileManager.saveStops(m_stops);
            return true;
        }
    }
    return false;
}
