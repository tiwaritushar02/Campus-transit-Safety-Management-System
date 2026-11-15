#ifndef ETA_SERVICE_HPP
#define ETA_SERVICE_HPP

/**
 * ============================================================================
 * CLASS: ETAService
 * OOP CONCEPTS:
 * - Abstraction & Single Responsibility: Encapsulates travel time calculations,
 *   mountain elevation penalties, and traffic factor computation.
 * ============================================================================
 */

#include <QString>
#include <QTime>

class ETAService {
private:
    double m_baseSpeedKmh;
    int m_dwellTimePerStopSecs;
    double m_trafficDelayFactor;

public:
    ETAService(double baseSpeedKmh = 35.0, int dwellPerStopSecs = 120, double trafficFactor = 1.0);

    // Calculate estimated minutes remaining to destination
    int calculateEtaMinutes(double remainingDistanceKm, double elevationAscentM = 0.0, int remainingStops = 0) const;

    // Calculate expected arrival timestamp
    QString calculateArrivalTime(int etaMinutes) const;

    // Status description based on traffic delay factor
    QString getCorridorStatus() const;

    // Setters
    void setBaseSpeed(double speed) { m_baseSpeedKmh = speed; }
    void setTrafficFactor(double factor) { m_trafficDelayFactor = factor; }
};

#endif // ETA_SERVICE_HPP
