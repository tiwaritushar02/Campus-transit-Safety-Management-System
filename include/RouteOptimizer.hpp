#ifndef ROUTE_OPTIMIZER_HPP
#define ROUTE_OPTIMIZER_HPP

/**
 * ============================================================================
 * MODULE: Member 3 - Scheduling, ETA & Route Optimization
 * FILE: RouteOptimizer.hpp
 * 
 * OOP CONCEPTS DEMONSTRATED:
 * - Graph Data Structure & Algorithm Abstraction: Uses Graph search (Dijkstra)
 *   to find the shortest path / minimum travel time between any two stops across
 *   the entire college bus network.
 * ============================================================================
 */

#include "Route.hpp"
#include "Stop.hpp"
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

struct PathResult {
    vector<string> stopPath;
    double totalDistanceKm;
    int totalTimeMinutes;
    string recommendedRouteId;
};

class RouteOptimizer {
public:
    // Finds shortest route path between start stop and destination stop across all available routes
    static PathResult findShortestPath(const vector<Route>& allRoutes,
                                       const string& startStopId,
                                       const string& destStopId);
};

#endif // ROUTE_OPTIMIZER_HPP
