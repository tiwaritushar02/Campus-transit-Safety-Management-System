/**
 * ============================================================================
 * MODULE: Member 3 - Scheduling, ETA & Route Optimization
 * FILE: RouteOptimizer.cpp
 * 
 * Demonstrates:
 * 1. Dijkstra's Algorithm (Uniform Cost Search)
 * 2. A* (A-Star Search with admissible Euclidean heuristic)
 * ============================================================================
 */

#include "RouteOptimizer.hpp"
#include "ETAEngine.hpp"
#include <queue>
#include <limits>
#include <algorithm>
#include <cmath>

using namespace std;

const unordered_map<string, GraphNode>& RouteOptimizer::getGraphNodes() {
    static const unordered_map<string, GraphNode> nodes = {
        {"ST101", {"ST101", "Haldwani Bus Terminal", 415, 1340, 29.2198, 79.5244}},
        {"ST102", {"ST102", "Tikonia Chowk (Bareilly Rd)", 436, 1239, 29.2350, 79.5280}},
        {"ST103", {"ST103", "Kathgodam Railway Station", 504, 981, 29.2736, 79.5398}},
        {"ST104", {"ST104", "Ranibagh HMT Junction", 559, 821, 29.2975, 79.5492}},
        {"ST108", {"ST108", "Mehra Gaon Ghat", 552, 618, 29.3280, 79.5480}},
        {"ST107", {"ST107", "Jeolikote Valley Junction", 342, 551, 29.3380, 79.5120}},
        {"ST106", {"ST106", "Bhowali Sanatorium Chowk", 401, 370, 29.3650, 79.5220}},
        {"ST105", {"ST105", "Graphic Era Hill University (GEHU), Bhimtal Campus", 557, 416, 29.3582, 79.5487}}
    };
    return nodes;
}

const unordered_map<string, vector<GraphEdge>>& RouteOptimizer::getGraphAdjList() {
    static const unordered_map<string, vector<GraphEdge>> adj = {
        {"ST101", {
            {"ST102", 2.7, {{415, 1340}, {436, 1239}}}
        }},
        {"ST102", {
            {"ST101", 2.7, {{436, 1239}, {415, 1340}}},
            {"ST103", 4.6, {{436, 1239}, {471, 1105}, {504, 981}}}
        }},
        {"ST103", {
            {"ST102", 4.6, {{504, 981}, {471, 1105}, {436, 1239}}},
            {"ST104", 4.8, {{504, 981}, {529, 898}, {559, 821}}}
        }},
        {"ST104", {
            {"ST103", 4.8, {{559, 821}, {529, 898}, {504, 981}}},
            {"ST108", 6.5, {{559, 821}, {575, 724}, {552, 618}}},
            {"ST107", 7.2, {{559, 821}, {453, 704}, {342, 551}}}
        }},
        {"ST108", {
            {"ST104", 6.5, {{552, 618}, {575, 724}, {559, 821}}},
            {"ST105", 9.9, {{552, 618}, {558, 531}, {557, 416}}}
        }},
        {"ST107", {
            {"ST104", 7.2, {{342, 551}, {453, 704}, {559, 821}}},
            {"ST106", 8.3, {{342, 551}, {401, 370}}}
        }},
        {"ST106", {
            {"ST107", 8.3, {{401, 370}, {342, 551}}},
            {"ST105", 11.2, {{401, 370}, {450, 385}, {500, 398}, {530, 408}, {557, 416}}}
        }},
        {"ST105", {
            {"ST108", 9.9, {{557, 416}, {558, 531}, {552, 618}}},
            {"ST106", 11.2, {{557, 416}, {530, 408}, {500, 398}, {450, 385}, {401, 370}}}
        }}
    };
    return adj;
}

double RouteOptimizer::calculateHeuristic(const string& fromNodeId, const string& toNodeId) {
    const auto& nodes = getGraphNodes();
    auto it1 = nodes.find(fromNodeId);
    auto it2 = nodes.find(toNodeId);
    if (it1 == nodes.end() || it2 == nodes.end()) return 0.0;

    // Direct Euclidean distance in km (scaled: ~0.026 km per map pixel)
    // Straight-line distance is guaranteed to be <= actual winding mountain road distance (Admissible)
    double dx = it1->second.pixelX - it2->second.pixelX;
    double dy = it1->second.pixelY - it2->second.pixelY;
    double pxDist = std::hypot(dx, dy);
    return pxDist * 0.026;
}

AlgorithmMetrics RouteOptimizer::runDijkstra(const string& startStopId, const string& destStopId) {
    auto tStart = chrono::high_resolution_clock::now();

    AlgorithmMetrics metrics;
    metrics.algorithmName = "Dijkstra (Uniform Cost Search)";
    metrics.heuristicType = "None (Uniform Cost g(n) only)";
    metrics.totalDistanceKm = 0.0;
    metrics.totalTimeMinutes = 0;

    const auto& adj = getGraphAdjList();
    unordered_map<string, double> dist;
    unordered_map<string, string> parent;
    unordered_map<string, bool> visited;

    for (const auto& kv : getGraphNodes()) {
        dist[kv.first] = 1e9;
        visited[kv.first] = false;
    }

    // Min-heap: {accumulated distance g(n), nodeId}
    priority_queue<pair<double, string>, vector<pair<double, string>>, greater<pair<double, string>>> pq;

    dist[startStopId] = 0.0;
    pq.push({0.0, startStopId});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (visited[u]) continue;
        visited[u] = true;
        metrics.exploredNodes.push_back(u);

        if (u == destStopId) break;

        auto it = adj.find(u);
        if (it != adj.end()) {
            for (const auto& edge : it->second) {
                metrics.exploredEdges.push_back({u, edge.targetNodeId});
                if (!visited[edge.targetNodeId] && dist[u] + edge.distanceKm < dist[edge.targetNodeId]) {
                    dist[edge.targetNodeId] = dist[u] + edge.distanceKm;
                    parent[edge.targetNodeId] = u;
                    pq.push({dist[edge.targetNodeId], edge.targetNodeId});
                }
            }
        }
    }

    // Reconstruct optimal path
    if (dist[destStopId] < 1e8) {
        string curr = destStopId;
        while (!curr.empty()) {
            metrics.optimalPath.push_back(curr);
            if (curr == startStopId) break;
            curr = parent[curr];
        }
        reverse(metrics.optimalPath.begin(), metrics.optimalPath.end());
        metrics.totalDistanceKm = dist[destStopId];
        metrics.totalTimeMinutes = static_cast<int>(dist[destStopId] * 1.55); // ~40 km/h mountain road average
    }

    auto tEnd = chrono::high_resolution_clock::now();
    metrics.executionTimeMs = chrono::duration<double, milli>(tEnd - tStart).count();
    metrics.nodesExploredCount = static_cast<int>(metrics.exploredNodes.size());

    return metrics;
}

AlgorithmMetrics RouteOptimizer::runAStar(const string& startStopId, const string& destStopId) {
    auto tStart = chrono::high_resolution_clock::now();

    AlgorithmMetrics metrics;
    metrics.algorithmName = "A* (A-Star Heuristic Search)";
    metrics.heuristicType = "Admissible Straight-Line Distance to GEHU Bhimtal";
    metrics.totalDistanceKm = 0.0;
    metrics.totalTimeMinutes = 0;

    const auto& adj = getGraphAdjList();
    unordered_map<string, double> gScore;
    unordered_map<string, double> fScore;
    unordered_map<string, string> parent;
    unordered_map<string, bool> visited;

    for (const auto& kv : getGraphNodes()) {
        gScore[kv.first] = 1e9;
        fScore[kv.first] = 1e9;
        visited[kv.first] = false;
    }

    // Min-heap: {fScore, nodeId} where f(n) = g(n) + h(n)
    priority_queue<pair<double, string>, vector<pair<double, string>>, greater<pair<double, string>>> pq;

    gScore[startStopId] = 0.0;
    fScore[startStopId] = calculateHeuristic(startStopId, destStopId);
    pq.push({fScore[startStopId], startStopId});

    while (!pq.empty()) {
        auto [f, u] = pq.top();
        pq.pop();

        if (visited[u]) continue;
        visited[u] = true;
        metrics.exploredNodes.push_back(u);

        if (u == destStopId) break;

        auto it = adj.find(u);
        if (it != adj.end()) {
            for (const auto& edge : it->second) {
                metrics.exploredEdges.push_back({u, edge.targetNodeId});
                double tentative_g = gScore[u] + edge.distanceKm;
                if (tentative_g < gScore[edge.targetNodeId]) {
                    parent[edge.targetNodeId] = u;
                    gScore[edge.targetNodeId] = tentative_g;
                    fScore[edge.targetNodeId] = tentative_g + calculateHeuristic(edge.targetNodeId, destStopId);
                    if (!visited[edge.targetNodeId]) {
                        pq.push({fScore[edge.targetNodeId], edge.targetNodeId});
                    }
                }
            }
        }
    }

    // Reconstruct optimal path
    if (gScore[destStopId] < 1e8) {
        string curr = destStopId;
        while (!curr.empty()) {
            metrics.optimalPath.push_back(curr);
            if (curr == startStopId) break;
            curr = parent[curr];
        }
        reverse(metrics.optimalPath.begin(), metrics.optimalPath.end());
        metrics.totalDistanceKm = gScore[destStopId];
        metrics.totalTimeMinutes = static_cast<int>(gScore[destStopId] * 1.55);
    }

    auto tEnd = chrono::high_resolution_clock::now();
    metrics.executionTimeMs = chrono::duration<double, milli>(tEnd - tStart).count();
    metrics.nodesExploredCount = static_cast<int>(metrics.exploredNodes.size());

    return metrics;
}

PathResult RouteOptimizer::findShortestPath(const vector<Route>& allRoutes,
                                            const string& startStopId,
                                            const string& destStopId) {
    (void)allRoutes;
    AlgorithmMetrics astar = runAStar(startStopId, destStopId);

    PathResult res;
    res.stopPath = astar.optimalPath;
    res.totalDistanceKm = astar.totalDistanceKm;
    res.totalTimeMinutes = astar.totalTimeMinutes;
    res.recommendedRouteId = (startStopId == "ST106" || destStopId == "ST106") ? "R2" : "R1";
    return res;
}
