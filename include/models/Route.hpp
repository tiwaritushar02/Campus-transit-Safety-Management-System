#ifndef ROUTE_HPP
#define ROUTE_HPP

#include <string>
#include <vector>

// Encapsulation
class Route {
private:
    std::string m_routeId;
    std::string m_routeName;
    double m_totalDistanceKm;
    int m_stopsCount;

public:
    // Constructor
    Route(const std::string& id = "", const std::string& name = "", double dist = 0.0, int count = 0)
        : m_routeId(id), m_routeName(name), m_totalDistanceKm(dist), m_stopsCount(count) {}

    // Encapsulation
    std::string getRouteId() const { return m_routeId; }
    std::string getRouteName() const { return m_routeName; }
    double getTotalDistanceKm() const { return m_totalDistanceKm; }
    int getStopsCount() const { return m_stopsCount; }
};

#endif // ROUTE_HPP
