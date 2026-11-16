#ifndef A_STAR_ROUTE_HPP
#define A_STAR_ROUTE_HPP

#include "RoutingAlgorithm.hpp"

// Inheritance
class AStarRoute : public RoutingAlgorithm {
private:
    double calculateHeuristic(const Graph& graph, const std::string& current, const std::string& target) const;

public:
    // Function Overriding
    // Polymorphism
    RouteResult calculateRoute(const Graph& graph,
                               const std::string& startNode,
                               const std::string& destNode) override;

    // Function Overriding
    // Polymorphism
    std::string getAlgorithmName() const override {
        return "A* Heuristic Search (Topographic & Elevation Profile)";
    }
};

#endif // A_STAR_ROUTE_HPP
