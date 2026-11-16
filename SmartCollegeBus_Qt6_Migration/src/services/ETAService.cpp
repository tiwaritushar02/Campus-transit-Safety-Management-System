#include "ETAService.hpp"
#include <algorithm>

ETAService::ETAService(double baseSpeedKmh, int dwellPerStopSecs, double trafficFactor)
    : m_baseSpeedKmh(baseSpeedKmh), m_dwellTimePerStopSecs(dwellPerStopSecs), m_trafficDelayFactor(trafficFactor) {}

int ETAService::calculateEtaMinutes(double remainingDistanceKm, double elevationAscentM, int remainingStops) const {
    if (remainingDistanceKm <= 0.05) return 0;

    // Mountain road speed reduction: steep ascent lowers effective speed
    double effectiveSpeed = m_baseSpeedKmh;
    if (elevationAscentM > 300.0) {
        effectiveSpeed *= 0.82; // Steep ghat climb speed reduction
    }

    // Apply traffic delay factor
    effectiveSpeed /= std::max(0.5, m_trafficDelayFactor);

    // Driving time in hours
    double driveHours = remainingDistanceKm / effectiveSpeed;
    int driveMinutes = static_cast<int>(driveHours * 60.0);

    // Add dwell times for remaining checkpoints
    int dwellMinutes = (remainingStops * m_dwellTimePerStopSecs) / 60;

    return std::max(1, driveMinutes + dwellMinutes);
}

QString ETAService::calculateArrivalTime(int etaMinutes) const {
    QTime now = QTime::currentTime();
    QTime arrival = now.addSecs(etaMinutes * 60);
    return arrival.toString("hh:mm AP");
}

QString ETAService::getCorridorStatus() const {
    if (m_trafficDelayFactor <= 1.05) {
        return "CLEAR (Normal Mountain Transit)";
    } else if (m_trafficDelayFactor <= 1.25) {
        return "MODERATE (Kathgodam Curve Congestion)";
    } else {
        return "HEAVY DELAY (Ghat Narrow Pass / Weather)";
    }
}
