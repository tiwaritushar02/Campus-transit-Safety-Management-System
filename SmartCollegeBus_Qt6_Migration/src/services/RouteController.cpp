#include "RouteController.hpp"

RouteController::RouteController(QObject* parent)
    : QObject(parent), m_selectedSourceId("ST101"), m_selectedBusId("BUS101") {
    m_simulation = new BusSimulation(m_selectedBusId, this);
    initializeCorridorCoordinates();
    prepareSimulation();
}

RouteController::~RouteController() {}

void RouteController::initializeCorridorCoordinates() {
    m_allStops.clear();
    m_fullCorridorRoad.clear();

    // Checkpoint Stops
    m_allStops.append({"ST101", "Haldwani Bus Terminal [424m ASL]", GPSLocation(29.2183, 79.5130, 424.0), 0});
    m_allStops.append({"ST102", "Kathgodam Railway Station [554m ASL]", GPSLocation(29.2715, 79.5312, 554.0), 12});
    m_allStops.append({"ST103", "Ranibagh Junction [620m ASL]", GPSLocation(29.2974, 79.5447, 620.0), 20});
    m_allStops.append({"ST104", "Bhujiaghat Mountain Pass [910m ASL]", GPSLocation(29.3210, 79.5510, 910.0), 30});
    m_allStops.append({"ST105", "Bhimtal Lake Mall Road [1,370m ASL]", GPSLocation(29.3500, 79.5530, 1370.0), 42});
    m_allStops.append({"ST106", "Graphic Era Hill University, Bhimtal Campus [1,420m ASL]", GPSLocation(29.3600, 79.5580, 1420.0), 50});

    // High density road points along NH 109 & Bhimtal Highway
    // 0: Haldwani
    m_fullCorridorRoad.append(GPSLocation(29.2183, 79.5130, 424.0));
    m_fullCorridorRoad.append(GPSLocation(29.2240, 79.5165, 435.0));
    m_fullCorridorRoad.append(GPSLocation(29.2315, 79.5200, 452.0));
    m_fullCorridorRoad.append(GPSLocation(29.2390, 79.5235, 470.0));
    m_fullCorridorRoad.append(GPSLocation(29.2465, 79.5260, 490.0));
    m_fullCorridorRoad.append(GPSLocation(29.2530, 79.5280, 508.0));
    m_fullCorridorRoad.append(GPSLocation(29.2595, 79.5295, 524.0));
    m_fullCorridorRoad.append(GPSLocation(29.2650, 79.5305, 540.0));
    m_fullCorridorRoad.append(GPSLocation(29.2715, 79.5312, 554.0)); // 12: Kathgodam

    m_fullCorridorRoad.append(GPSLocation(29.2760, 79.5335, 570.0));
    m_fullCorridorRoad.append(GPSLocation(29.2815, 79.5365, 585.0));
    m_fullCorridorRoad.append(GPSLocation(29.2870, 79.5395, 600.0));
    m_fullCorridorRoad.append(GPSLocation(29.2925, 79.5420, 612.0));
    m_fullCorridorRoad.append(GPSLocation(29.2974, 79.5447, 620.0)); // 20: Ranibagh

    m_fullCorridorRoad.append(GPSLocation(29.3015, 79.5460, 660.0));
    m_fullCorridorRoad.append(GPSLocation(29.3060, 79.5475, 710.0));
    m_fullCorridorRoad.append(GPSLocation(29.3110, 79.5490, 770.0));
    m_fullCorridorRoad.append(GPSLocation(29.3160, 79.5502, 840.0));
    m_fullCorridorRoad.append(GPSLocation(29.3210, 79.5510, 910.0)); // 30: Bhujiaghat

    m_fullCorridorRoad.append(GPSLocation(29.3265, 79.5515, 980.0));
    m_fullCorridorRoad.append(GPSLocation(29.3320, 79.5520, 1060.0));
    m_fullCorridorRoad.append(GPSLocation(29.3375, 79.5522, 1140.0));
    m_fullCorridorRoad.append(GPSLocation(29.3420, 79.5525, 1220.0));
    m_fullCorridorRoad.append(GPSLocation(29.3465, 79.5528, 1310.0));
    m_fullCorridorRoad.append(GPSLocation(29.3500, 79.5530, 1370.0)); // 42: Bhimtal Lake

    m_fullCorridorRoad.append(GPSLocation(29.3525, 79.5540, 1385.0));
    m_fullCorridorRoad.append(GPSLocation(29.3550, 79.5555, 1400.0));
    m_fullCorridorRoad.append(GPSLocation(29.3575, 79.5570, 1412.0));
    m_fullCorridorRoad.append(GPSLocation(29.3600, 79.5580, 1420.0)); // 50: Graphic Era Bhimtal
}

void RouteController::setSource(const QString& sourceStopId) {
    m_selectedSourceId = sourceStopId;
    prepareSimulation();
}

void RouteController::setBus(const QString& busId) {
    m_selectedBusId = busId;
    prepareSimulation();
}

QVector<CheckpointStop> RouteController::getActiveCheckpoints() const {
    int startIdx = 0;
    for (int i = 0; i < m_allStops.size(); ++i) {
        if (m_allStops[i].id == m_selectedSourceId) {
            startIdx = i;
            break;
        }
    }

    QVector<CheckpointStop> active;
    int baseCoordOffset = m_allStops[startIdx].coordinateIndex;
    for (int i = startIdx; i < m_allStops.size(); ++i) {
        CheckpointStop s = m_allStops[i];
        s.coordinateIndex = std::max(0, s.coordinateIndex - baseCoordOffset);
        active.append(s);
    }
    return active;
}

QVector<GPSLocation> RouteController::getActiveRoadCoordinates() const {
    int startIdx = 0;
    for (int i = 0; i < m_allStops.size(); ++i) {
        if (m_allStops[i].id == m_selectedSourceId) {
            startIdx = m_allStops[i].coordinateIndex;
            break;
        }
    }

    QVector<GPSLocation> activeRoad;
    for (int i = startIdx; i < m_fullCorridorRoad.size(); ++i) {
        activeRoad.append(m_fullCorridorRoad[i]);
    }
    return activeRoad;
}

void RouteController::prepareSimulation() {
    auto activeCoords = getActiveRoadCoordinates();
    auto activeStops = getActiveCheckpoints();
    m_simulation->setRouteWaypoints(activeCoords, activeStops);
}
