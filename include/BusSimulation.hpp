#ifndef BUS_SIMULATION_HPP
#define BUS_SIMULATION_HPP

/**
 * ============================================================================
 * CLASS: BusSimulation
 * OOP CONCEPTS:
 * - Encapsulation & Event-driven Abstraction: Uses QTimer to simulate vehicle
 *   motion along predefined geographic road coordinates.
 * - Demo Mode Transparency: Explicitly tags all coordinates as SIMULATED.
 * ============================================================================
 */

#include <QObject>
#include <QTimer>
#include <QVector>
#include "GPSLocation.hpp"
#include "ETAService.hpp"

struct CheckpointStop {
    QString id;
    QString name;
    GPSLocation location;
    int coordinateIndex;
};

class BusSimulation : public QObject {
    Q_OBJECT

private:
    QString m_busId;
    QVector<GPSLocation> m_roadCoordinates;
    QVector<CheckpointStop> m_checkpoints;
    int m_currentIndex;
    int m_currentCheckpointIndex;

    QTimer* m_timer;
    bool m_isRunning;
    bool m_isPaused;
    int m_speedMultiplier; // 1x, 2x, 5x

    ETAService m_etaService;

    double calculateRemainingDistanceKm() const;

private slots:
    void onTick();

signals:
    void telemetryUpdated(const QString& busId, const GPSLocation& location, 
                          int progressPercent, const QString& currentStop, 
                          const QString& nextStop, int etaMinutes, double remainingKm);
    void checkpointReached(const QString& busId, const QString& stopName, const QString& arrivalTime);
    void simulationCompleted(const QString& busId);

public:
    explicit BusSimulation(const QString& busId = "BUS101", QObject* parent = nullptr);
    ~BusSimulation() override;

    void setRouteWaypoints(const QVector<GPSLocation>& coords, const QVector<CheckpointStop>& stops);
    void startSimulation();
    void pauseSimulation();
    void resumeSimulation();
    void resetSimulation();
    void setSpeedMultiplier(int mult);

    bool isRunning() const { return m_isRunning; }
    bool isPaused() const { return m_isPaused; }
    int getCurrentIndex() const { return m_currentIndex; }
    GPSLocation getCurrentLocation() const;
    QString getBusId() const { return m_busId; }
};

#endif // BUS_SIMULATION_HPP
