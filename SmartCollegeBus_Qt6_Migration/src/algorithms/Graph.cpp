#include "Graph.hpp"

void Graph::addNode(const std::string& id, double lat, double lon, double elevM) {
    m_nodeCoordinates[id] = {lat, lon};
    m_nodeElevations[id] = elevM;
    if (m_adjacencyList.find(id) == m_adjacencyList.end()) {
        m_adjacencyList[id] = {};
    }
}

void Graph::addEdge(const std::string& from, const std::string& to, double distKm, double elevM, const std::string& corridor) {
    m_adjacencyList[from].push_back({to, distKm, elevM, corridor});
}

const std::vector<Edge>& Graph::getNeighbors(const std::string& nodeId) const {
    static const std::vector<Edge> empty;
    auto it = m_adjacencyList.find(nodeId);
    if (it != m_adjacencyList.end()) return it->second;
    return empty;
}

bool Graph::hasNode(const std::string& nodeId) const {
    return m_nodeCoordinates.find(nodeId) != m_nodeCoordinates.end();
}

std::pair<double, double> Graph::getCoordinates(const std::string& nodeId) const {
    auto it = m_nodeCoordinates.find(nodeId);
    if (it != m_nodeCoordinates.end()) return it->second;
    return {0.0, 0.0};
}

double Graph::getElevation(const std::string& nodeId) const {
    auto it = m_nodeElevations.find(nodeId);
    if (it != m_nodeElevations.end()) return it->second;
    return 0.0;
}

Graph Graph::createHaldwaniBhimtalNetwork() {
    Graph g;
    // Haldwani to Graphic Era Hill University Bhimtal Corridor
    g.addNode("ST101", 29.2183, 79.5130, 424.0);  // Haldwani Bus Terminal
    g.addNode("ST102", 29.2715, 79.5312, 554.0);  // Kathgodam Railway Station
    g.addNode("ST103", 29.2942, 79.5445, 680.0);  // Ranibagh Junction
    g.addNode("ST104", 29.3210, 79.5510, 950.0);  // Bhujiaghat Bypass
    g.addNode("ST105", 29.3458, 79.5583, 1370.0); // Bhimtal Lake Mall Road
    g.addNode("ST106", 29.3565, 79.5620, 1420.0); // Graphic Era Hill University Bhimtal Campus

    // R1: Express Corridor (Haldwani -> Kathgodam -> Ranibagh -> Bhimtal Lake -> Graphic Era Campus)
    g.addEdge("ST101", "ST102", 5.6, 130.0, "R1-Express");
    g.addEdge("ST102", "ST103", 4.7, 126.0, "R1-Express");
    g.addEdge("ST103", "ST105", 13.9, 690.0, "R1-Express");
    g.addEdge("ST105", "ST106", 4.0, 50.0, "R1-Express");

    // R2: Valley Bypass via Bhujiaghat (Haldwani -> Kathgodam -> Ranibagh -> Bhujiaghat -> Bhimtal Lake -> Graphic Era Campus)
    g.addEdge("ST103", "ST104", 5.5, 270.0, "R2-Bypass");
    g.addEdge("ST104", "ST105", 10.6, 420.0, "R2-Bypass");

    return g;
}
