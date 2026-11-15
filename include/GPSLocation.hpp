#ifndef GPS_LOCATION_HPP
#define GPS_LOCATION_HPP

/**
 * ============================================================================
 * CLASS: GPSLocation
 * OOP CONCEPTS:
 * - Encapsulation: Coordinates, altitude, speed, timestamp, heading are private.
 * - Validation & Serialization: CSV/string conversion and distance calculations.
 * ============================================================================
 */

#include <QString>
#include <cmath>

class GPSLocation {
private:
    double m_latitude;
    double m_longitude;
    double m_altitudeMeters;
    double m_speedKmh;
    double m_headingDegrees;
    QString m_timestamp;
    bool m_isSimulated;

public:
    GPSLocation();
    GPSLocation(double lat, double lon, double alt = 0.0, double speed = 35.0, 
                double heading = 0.0, const QString& timestamp = "", bool isSimulated = true);

    // Getters
    double getLatitude() const { return m_latitude; }
    double getLongitude() const { return m_longitude; }
    double getAltitude() const { return m_altitudeMeters; }
    double getSpeed() const { return m_speedKmh; }
    double getHeading() const { return m_headingDegrees; }
    QString getTimestamp() const { return m_timestamp; }
    bool isSimulated() const { return m_isSimulated; }

    // Setters
    void setLatitude(double lat) { m_latitude = lat; }
    void setLongitude(double lon) { m_longitude = lon; }
    void setAltitude(double alt) { m_altitudeMeters = alt; }
    void setSpeed(double speed) { m_speedKmh = speed; }
    void setHeading(double heading) { m_headingDegrees = heading; }
    void setTimestamp(const QString& ts) { m_timestamp = ts; }
    void setSimulated(bool sim) { m_isSimulated = sim; }

    // Great circle distance (Haversine formula) in kilometers
    double distanceTo(const GPSLocation& other) const;

    // Formatting & Serialization (C++ File Handling)
    QString toString() const;
    QString toCsv() const;
    static GPSLocation fromCsv(const QString& line);
};

#endif // GPS_LOCATION_HPP
