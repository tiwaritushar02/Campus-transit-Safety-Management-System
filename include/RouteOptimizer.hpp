#ifndef ROUTE_OPTIMIZER_HPP
#define ROUTE_OPTIMIZER_HPP

/**
 * ============================================================================
 * MODULE: Member 3 - Scheduling, ETA & Route Optimization
 * FILE: RouteOptimizer.hpp
 * 
 * OOP CONCEPTS DEMONSTRATED:
 * - Graph Data Structure & Algorithm Abstraction: Implements both Dijkstra's
 *   Algorithm (Uniform Cost Search) and A* (A-Star Heuristic Search) to find
 *   the optimal shortest path from any source pickup stop to Graphic Era Hill
 *   University (GEHU), Bhimtal Campus.
 * ============================================================================
 */

#include "Route.hpp"
#include "Stop.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>

using namespace std;

struct PathResult {
    vector<string> stopPath;
    double totalDistanceKm;
    int totalTimeMinutes;
    string recommendedRouteId;
};

struct AlgorithmMetrics {
    string algorithmName; // "Dijkstra" or "A*"
    vector<string> exploredNodes; // In order of visit
    vector<pair<string, string>> exploredEdges; // In order of relaxation
    vector<string> optimalPath;
    double totalDistanceKm;
    int totalTimeMinutes;
    double executionTimeMs;
    int nodesExploredCount;
    string heuristicType;
};

struct GraphNode {
    string id;
    string name;
    int pixelX;
    int pixelY;
    double lat;
    double lon;
};

struct GraphEdge {
    string targetNodeId;
    double distanceKm;
    vector<pair<int, int>> waypoints;
};

class RouteOptimizer {
public:
    // Core Graph nodes and adjacency list
    static const unordered_map<string, GraphNode>& getGraphNodes();
    static const unordered_map<string, vector<GraphEdge>>& getGraphAdjList();

    // Dijkstra Algorithm (Uniform Cost Search based on road distance)
    static AlgorithmMetrics runDijkstra(const string& startStopId, const string& destStopId = "ST105");

    // A* (A-Star) Algorithm (Using Road Distance + Admissible Euclidean Heuristic to GEHU Bhimtal)
    static AlgorithmMetrics runAStar(const string& startStopId, const string& destStopId = "ST105");

    // Heuristic function: Straight-line distance estimate in km to destination
    static double calculateHeuristic(const string& fromNodeId, const string& toNodeId = "ST105");

    // Backward compatibility for existing CLI and viva test suite
    static PathResult findShortestPath(const vector<Route>& allRoutes,
                                       const string& startStopId,
                                       const string& destStopId);
};

#endif // ROUTE_OPTIMIZER_HPP
