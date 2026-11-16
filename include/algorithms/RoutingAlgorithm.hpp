#ifndef ROUTING_ALGORITHM_HPP
#define ROUTING_ALGORITHM_HPP

#include "Graph.hpp"
#include <string>
#include <vector>

struct RouteResult {
    bool found;
    std::vector<std::string> pathNodes;
    double totalDistanceKm;
    double totalElevationGainM;
    int estimatedDurationMins;
    std::string algorithmName;
    std::string explanation;
};

// Abstraction
class RoutingAlgorithm {
public:
    // Destructor
    virtual ~RoutingAlgorithm() = default;

    // Polymorphism
    virtual RouteResult calculateRoute(const Graph& graph,
                                       const std::string& startNode,
                                       const std::string& destNode) = 0;

    // Polymorphism
    virtual std::string getAlgorithmName() const = 0;
};

#endif // ROUTING_ALGORITHM_HPP
