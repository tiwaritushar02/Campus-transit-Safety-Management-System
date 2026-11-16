#include "BusSimulation.hpp"
#include <QDateTime>
#include <algorithm>

BusSimulation::BusSimulation(const QString& busId, QObject* parent)
    : QObject(parent), m_busId(busId), m_currentIndex(0), m_currentCheckpointIndex(0),
      m_isRunning(false), m_isPaused(false), m_speedMultiplier(1), m_etaService(36.0, 90, 1.0) {
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &BusSimulation::onTick);
}

BusSimulation::~BusSimulation() {
    if (m_timer && m_timer->isActive()) {
        m_timer->stop();
    }
}

void BusSimulation::setRouteWaypoints(const QVector<GPSLocation>& coords, const QVector<CheckpointStop>& stops) {
    m_roadCoordinates = coords;
    m_checkpoints = stops;
    resetSimulation();
}

void BusSimulation::startSimulation() {
    if (m_roadCoordinates.isEmpty()) return;
    m_isRunning = true;
    m_isPaused = false;
    m_timer->start(350 / m_speedMultiplier);
}

void BusSimulation::pauseSimulation() {
    if (!m_isRunning) return;
    m_isPaused = true;
    m_timer->stop();
}

void BusSimulation::resumeSimulation() {
    if (!m_isRunning || !m_isPaused) return;
    m_isPaused = false;
    m_timer->start(350 / m_speedMultiplier);
}

void BusSimulation::resetSimulation() {
    if (m_timer->isActive()) {
        m_timer->stop();
    }
    m_isRunning = false;
    m_isPaused = false;
    m_currentIndex = 0;
    m_currentCheckpointIndex = 0;
}

void BusSimulation::setSpeedMultiplier(int mult) {
    m_speedMultiplier = std::max(1, mult);
    if (m_isRunning && !m_isPaused) {
        m_timer->setInterval(350 / m_speedMultiplier);
    }
}

GPSLocation BusSimulation::getCurrentLocation() const {
    if (m_roadCoordinates.isEmpty()) return GPSLocation();
    if (m_currentIndex < m_roadCoordinates.size()) {
        return m_roadCoordinates[m_currentIndex];
    }
    return m_roadCoordinates.last();
}

double BusSimulation::calculateRemainingDistanceKm() const {
    if (m_currentIndex >= m_roadCoordinates.size() - 1) return 0.0;
    double dist = 0.0;
    for (int i = m_currentIndex; i < m_roadCoordinates.size() - 1; ++i) {
        dist += m_roadCoordinates[i].distanceTo(m_roadCoordinates[i + 1]);
    }
    return dist;
}

void BusSimulation::onTick() {
    if (m_roadCoordinates.isEmpty()) return;

    if (m_currentIndex >= m_roadCoordinates.size()) {
        m_timer->stop();
        m_isRunning = false;
        emit simulationCompleted(m_busId);
        return;
    }

    GPSLocation curLoc = m_roadCoordinates[m_currentIndex];
    curLoc.setTimestamp(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

    int progressPercent = static_cast<int>((m_currentIndex * 100.0) / std::max(1, static_cast<int>(m_roadCoordinates.size()) - 1));
    progressPercent = std::min(100, progressPercent);

    // Determine current and next checkpoints
    QString currentStopName = "En Route";
    QString nextStopName = "Graphic Era Hill University (Destination)";

    if (!m_checkpoints.isEmpty()) {
        for (int i = 0; i < m_checkpoints.size(); ++i) {
            if (m_currentIndex >= m_checkpoints[i].coordinateIndex) {
                currentStopName = m_checkpoints[i].name;
                if (i + 1 < m_checkpoints.size()) {
                    nextStopName = m_checkpoints[i + 1].name;
                } else {
                    nextStopName = "Destination Reached";
                }

                if (i > m_currentCheckpointIndex) {
                    m_currentCheckpointIndex = i;
                    emit checkpointReached(m_busId, m_checkpoints[i].name, QTime::currentTime().toString("hh:mm AP"));
                }
            }
        }
    }

    double remainingKm = calculateRemainingDistanceKm();
    int remainingStops = std::max(0, static_cast<int>(m_checkpoints.size()) - 1 - m_currentCheckpointIndex);
    int etaMinutes = m_etaService.calculateEtaMinutes(remainingKm, 900.0, remainingStops);

    emit telemetryUpdated(m_busId, curLoc, progressPercent, currentStopName, nextStopName, etaMinutes, remainingKm);

    m_currentIndex++;
}
