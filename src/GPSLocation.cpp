#include "GPSLocation.hpp"
#include <QDateTime>
#include <QStringList>

GPSLocation::GPSLocation()
    : m_latitude(29.2183), m_longitude(79.5130), m_altitudeMeters(424.0),
      m_speedKmh(0.0), m_headingDegrees(0.0),
      m_timestamp(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")),
      m_isSimulated(true) {}

GPSLocation::GPSLocation(double lat, double lon, double alt, double speed, 
                         double heading, const QString& timestamp, bool isSimulated)
    : m_latitude(lat), m_longitude(lon), m_altitudeMeters(alt),
      m_speedKmh(speed), m_headingDegrees(heading),
      m_timestamp(timestamp.isEmpty() ? QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") : timestamp),
      m_isSimulated(isSimulated) {}

double GPSLocation::distanceTo(const GPSLocation& other) const {
    const double R = 6371.0; // Earth's radius in km
    double lat1Rad = m_latitude * M_PI / 180.0;
    double lat2Rad = other.m_latitude * M_PI / 180.0;
    double deltaLat = (other.m_latitude - m_latitude) * M_PI / 180.0;
    double deltaLon = (other.m_longitude - m_longitude) * M_PI / 180.0;

    double a = std::sin(deltaLat / 2.0) * std::sin(deltaLat / 2.0) +
               std::cos(lat1Rad) * std::cos(lat2Rad) *
               std::sin(deltaLon / 2.0) * std::sin(deltaLon / 2.0);
    double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
    return R * c;
}

QString GPSLocation::toString() const {
    return QString("%1° N, %2° E (Alt: %3m)")
        .arg(m_latitude, 0, 'f', 5)
        .arg(m_longitude, 0, 'f', 5)
        .arg(m_altitudeMeters, 0, 'f', 1);
}

QString GPSLocation::toCsv() const {
    return QString("%1,%2,%3,%4,%5,%6,%7")
        .arg(m_latitude, 0, 'f', 6)
        .arg(m_longitude, 0, 'f', 6)
        .arg(m_altitudeMeters, 0, 'f', 1)
        .arg(m_speedKmh, 0, 'f', 1)
        .arg(m_headingDegrees, 0, 'f', 1)
        .arg(m_timestamp)
        .arg(m_isSimulated ? "SIMULATED" : "LIVE_GPS");
}

GPSLocation GPSLocation::fromCsv(const QString& line) {
    QStringList parts = line.split(',');
    if (parts.size() < 7) {
        return GPSLocation();
    }
    double lat = parts[0].trimmed().toDouble();
    double lon = parts[1].trimmed().toDouble();
    double alt = parts[2].trimmed().toDouble();
    double speed = parts[3].trimmed().toDouble();
    double heading = parts[4].trimmed().toDouble();
    QString ts = parts[5].trimmed();
    bool isSim = (parts[6].trimmed() == "SIMULATED");
    return GPSLocation(lat, lon, alt, speed, heading, ts, isSim);
}
