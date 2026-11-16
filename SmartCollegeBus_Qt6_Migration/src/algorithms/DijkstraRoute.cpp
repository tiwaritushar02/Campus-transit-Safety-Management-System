#include "DijkstraRoute.hpp"
#include <queue>
#include <unordered_map>
#include <cmath>
#include <algorithm>

RouteResult DijkstraRoute::calculateRoute(const Graph& graph,
                                          const std::string& startNode,
                                          const std::string& destNode) {
    RouteResult res;
    res.found = false;
    res.algorithmName = getAlgorithmName();

    if (!graph.hasNode(startNode) || !graph.hasNode(destNode)) {
        res.explanation = "Invalid start or destination node in graph.";
        return res;
    }

    struct DistNode {
        std::string id;
        double dist;
        bool operator>(const DistNode& other) const { return dist > other.dist; }
    };

    std::priority_queue<DistNode, std::vector<DistNode>, std::greater<DistNode>> pq;
    std::unordered_map<std::string, double> distances;
    std::unordered_map<std::string, std::string> previous;

    distances[startNode] = 0.0;
    pq.push({startNode, 0.0});

    while (!pq.empty()) {
        auto [current, d] = pq.top();
        pq.pop();

        if (d > distances[current]) continue;

        if (current == destNode) {
            std::vector<std::string> path;
            std::string curr = destNode;
            while (curr != startNode) {
                path.push_back(curr);
                curr = previous[curr];
            }
            path.push_back(startNode);
            std::reverse(path.begin(), path.end());

            res.found = true;
            res.pathNodes = path;
            res.totalDistanceKm = distances[destNode];
            res.totalElevationGainM = std::max(0.0, graph.getElevation(destNode) - graph.getElevation(startNode));
            res.estimatedDurationMins = static_cast<int>(std::round(res.totalDistanceKm * 1.9));
            res.explanation = "Dijkstra uniform shortest path computed.";
            return res;
        }

        for (const auto& edge : graph.getNeighbors(current)) {
            double newDist = distances[current] + edge.distanceKm;
            if (distances.find(edge.destination) == distances.end() || newDist < distances[edge.destination]) {
                distances[edge.destination] = newDist;
                previous[edge.destination] = current;
                pq.push({edge.destination, newDist});
            }
        }
    }

    res.explanation = "No corridor connectivity between nodes.";
    return res;
}
