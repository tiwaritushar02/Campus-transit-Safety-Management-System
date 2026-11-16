#ifndef BUS_TRACKING_SERVICE_HPP
#define BUS_TRACKING_SERVICE_HPP

/**
 * ============================================================================
 * CLASS: BusTrackingService
 * OOP CONCEPTS:
 * - Service Layer & Persistence Abstraction: Uses C++ File Handling (TXT, CSV, DAT)
 *   to store and retrieve tracking telemetry, historical logs, and arrival records.
 * - Strictly NO SQL databases.
 * ============================================================================
 */

#include <QString>
#include <QVector>
#include <QStringList>
#include "GPSLocation.hpp"

struct FleetBusInfo {
    QString busId;
    QString regNo;
    QString driverName;
    QString driverPhone;
    int capacity;
    QString currentStatus; // IDLE, EN ROUTE, DELAYED, COMPLETED
};

struct TrackingLogEntry {
    QString timestamp;
    QString busId;
    QString event;
    QString location;
    QString status;
};

// Encapsulation
class BusTrackingService {
private:
    QString m_gpsDatPath;
    QString m_historyCsvPath;
    QString m_arrivalCsvPath;
    QVector<FleetBusInfo> m_fleet;

    void ensureDataDirectory() const;

public:
    // Constructor
    BusTrackingService();

    // Fleet management
    QVector<FleetBusInfo> getFleet() const { return m_fleet; }
    FleetBusInfo getBusInfo(const QString& busId) const;
    void updateBusStatus(const QString& busId, const QString& status);

    // C++ File Handling: Log simulated GPS coordinates to .DAT file
    void appendGpsRecord(const QString& busId, const GPSLocation& location);

    // C++ File Handling: Append event to tracking_history.csv
    void logTrackingEvent(const QString& busId, const QString& event, const QString& location, const QString& status);

    // C++ File Handling: Append arrival record to arrival_records.csv
    void recordStopArrival(const QString& busId, const QString& stopName, const QString& arrivalTime);

    // Read history logs from CSV
    QVector<TrackingLogEntry> readTrackingHistory(int maxEntries = 50) const;

    // Load initial schedules / arrival records
    QVector<QStringList> getScheduleTable(const QString& busId) const;
};

#endif // BUS_TRACKING_SERVICE_HPP
