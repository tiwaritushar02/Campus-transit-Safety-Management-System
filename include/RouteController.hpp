#ifndef ROUTE_CONTROLLER_HPP
#define ROUTE_CONTROLLER_HPP

/**
 * ============================================================================
 * CLASS: RouteController
 * OOP CONCEPTS:
 * - Controller Pattern: Orchestrates route selection, fixed destination rules,
 *   mountain road waypoints generation, and simulation dispatching.
 * ============================================================================
 */

#include <QObject>
#include <QVector>
#include <QString>
#include "GPSLocation.hpp"
#include "BusSimulation.hpp"
#include "BusTrackingService.hpp"

class RouteController : public QObject {
    Q_OBJECT

private:
    QString m_selectedSourceId;
    QString m_selectedBusId;
    QVector<CheckpointStop> m_allStops;
    QVector<GPSLocation> m_fullCorridorRoad;

    BusSimulation* m_simulation;
    BusTrackingService m_trackingService;

    void initializeCorridorCoordinates();

public:
    explicit RouteController(QObject* parent = nullptr);
    ~RouteController() override;

    void setSource(const QString& sourceStopId);
    void setBus(const QString& busId);

    QString getSourceStopId() const { return m_selectedSourceId; }
    QString getDestinationStopId() const { return "ST106"; } // ALWAYS FIXED: Graphic Era Bhimtal
    QString getDestinationName() const { return "Graphic Era Hill University, Bhimtal Campus [1,420m ASL]"; }

    QVector<CheckpointStop> getActiveCheckpoints() const;
    QVector<GPSLocation> getActiveRoadCoordinates() const;

    BusSimulation* getSimulation() { return m_simulation; }
    BusTrackingService& getTrackingService() { return m_trackingService; }

    void prepareSimulation();
};

#endif // ROUTE_CONTROLLER_HPP
