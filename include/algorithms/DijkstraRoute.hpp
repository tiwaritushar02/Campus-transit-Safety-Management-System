#ifndef DIJKSTRA_ROUTE_HPP
#define DIJKSTRA_ROUTE_HPP

#include "RoutingAlgorithm.hpp"

// Inheritance
class DijkstraRoute : public RoutingAlgorithm {
public:
    // Function Overriding
    // Polymorphism
    RouteResult calculateRoute(const Graph& graph,
                               const std::string& startNode,
                               const std::string& destNode) override;

    // Function Overriding
    // Polymorphism
    std::string getAlgorithmName() const override {
        return "Dijkstra's Shortest Path Algorithm";
    }
};

#endif // DIJKSTRA_ROUTE_HPP
