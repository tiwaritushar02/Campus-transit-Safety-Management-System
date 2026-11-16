#include "AStarRoute.hpp"
#include <queue>
#include <unordered_map>
#include <cmath>
#include <algorithm>

// Euclidean distance + elevation gradient heuristic
double AStarRoute::calculateHeuristic(const Graph& graph, const std::string& current, const std::string& target) const {
    auto [lat1, lon1] = graph.getCoordinates(current);
    auto [lat2, lon2] = graph.getCoordinates(target);

    // Approximate distance in km using equirectangular projection
    double x = (lon2 - lon1) * std::cos((lat1 + lat2) * M_PI / 360.0) * 111.32;
    double y = (lat2 - lat1) * 110.57;
    double distKm = std::sqrt(x * x + y * y);

    // Mountain hill climb cost heuristic (0.005 km penalty per meter of elevation ascent)
    double elev1 = graph.getElevation(current);
    double elev2 = graph.getElevation(target);
    double elevDiffKm = std::max(0.0, (elev2 - elev1) * 0.002);

    return distKm + elevDiffKm;
}

RouteResult AStarRoute::calculateRoute(const Graph& graph,
                                      const std::string& startNode,
                                      const std::string& destNode) {
    RouteResult res;
    res.found = false;
    res.algorithmName = getAlgorithmName();

    if (!graph.hasNode(startNode) || !graph.hasNode(destNode)) {
        res.explanation = "Invalid start or destination node in graph.";
        return res;
    }

    if (startNode == destNode) {
        res.found = true;
        res.pathNodes = {startNode};
        res.totalDistanceKm = 0.0;
        res.totalElevationGainM = 0.0;
        res.estimatedDurationMins = 0;
        res.explanation = "Start and destination are identical.";
        return res;
    }

    struct QueueNode {
        std::string id;
        double fCost; // g + h
        bool operator>(const QueueNode& other) const { return fCost > other.fCost; }
    };

    std::priority_queue<QueueNode, std::vector<QueueNode>, std::greater<QueueNode>> openSet;
    std::unordered_map<std::string, double> gScore;
    std::unordered_map<std::string, std::string> cameFrom;

    gScore[startNode] = 0.0;
    openSet.push({startNode, calculateHeuristic(graph, startNode, destNode)});

    while (!openSet.empty()) {
        std::string current = openSet.top().id;
        openSet.pop();

        if (current == destNode) {
            // Reconstruct path
            std::vector<std::string> path;
            std::string curr = destNode;
            while (curr != startNode) {
                path.push_back(curr);
                curr = cameFrom[curr];
            }
            path.push_back(startNode);
            std::reverse(path.begin(), path.end());

            res.found = true;
            res.pathNodes = path;
            res.totalDistanceKm = gScore[destNode];
            res.totalElevationGainM = std::max(0.0, graph.getElevation(destNode) - graph.getElevation(startNode));
            res.estimatedDurationMins = static_cast<int>(std::round(res.totalDistanceKm * 1.85)); // Hill driving speed ~32-40 km/h
            res.explanation = "A* shortest path computed with elevation heuristic optimization.";
            return res;
        }

        for (const auto& edge : graph.getNeighbors(current)) {
            double tentativeG = gScore[current] + edge.distanceKm;
            if (gScore.find(edge.destination) == gScore.end() || tentativeG < gScore[edge.destination]) {
                cameFrom[edge.destination] = current;
                gScore[edge.destination] = tentativeG;
                double fScore = tentativeG + calculateHeuristic(graph, edge.destination, destNode);
                openSet.push({edge.destination, fScore});
            }
        }
    }

    res.explanation = "No corridor connectivity between nodes.";
    return res;
}
