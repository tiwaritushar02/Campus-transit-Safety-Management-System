#ifndef FLEET_MANAGER_HPP
#define FLEET_MANAGER_HPP

#include "FileManager.hpp"
#include "Bus.hpp"
#include "Route.hpp"
#include "Stop.hpp"
#include <vector>
#include <string>

// Encapsulation
class FleetManager {
private:
    // Composition
    FileManager& m_fileManager;
    std::vector<Bus> m_buses;
    std::vector<Route> m_routes;
    std::vector<Stop> m_stops;

public:
    // Constructor
    explicit FleetManager(FileManager& fm);

    void reload();

    const std::vector<Bus>& getBuses() const { return m_buses; }
    const std::vector<Route>& getRoutes() const { return m_routes; }
    const std::vector<Stop>& getStops() const { return m_stops; }

    int getTotalFleetUnits() const { return static_cast<int>(m_buses.size()); }
    int getTotalAvailableSeats() const;

    bool updateBusConfig(const std::string& busId, const std::string& reg, int cap, const std::string& routeId);
    bool updateStopTiming(const std::string& stopId, const std::string& newTime, bool isFullDay);
};

#endif // FLEET_MANAGER_HPP
