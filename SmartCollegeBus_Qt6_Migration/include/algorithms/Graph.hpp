#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <string>
#include <vector>
#include <unordered_map>

struct Edge {
    std::string destination;
    double distanceKm;
    double elevationChangeM;
    std::string corridorName;
};

// Encapsulation
class Graph {
private:
    std::unordered_map<std::string, std::vector<Edge>> m_adjacencyList;
    std::unordered_map<std::string, std::pair<double, double>> m_nodeCoordinates;
    std::unordered_map<std::string, double> m_nodeElevations;

public:
    void addNode(const std::string& id, double lat, double lon, double elevM);
    void addEdge(const std::string& from, const std::string& to, double distKm, double elevM, const std::string& corridor);

    const std::vector<Edge>& getNeighbors(const std::string& nodeId) const;
    bool hasNode(const std::string& nodeId) const;
    std::pair<double, double> getCoordinates(const std::string& nodeId) const;
    double getElevation(const std::string& nodeId) const;

    static Graph createHaldwaniBhimtalNetwork();
};

#endif // GRAPH_HPP
