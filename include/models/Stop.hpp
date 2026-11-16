#ifndef STOP_HPP
#define STOP_HPP

#include <string>
#include <vector>

// Encapsulation
class Stop {
private:
    std::string m_stopId;
    std::string m_stopName;
    std::string m_passingBuses;
    std::string m_fullDayTimes;
    std::string m_halfDayTimes;
    double m_lat;
    double m_lon;
    std::string m_altitude;

public:
    // Constructor
    Stop(const std::string& id = "", const std::string& name = "", const std::string& buses = "",
         const std::string& fullTimes = "", const std::string& halfTimes = "",
         double lat = 0.0, double lon = 0.0, const std::string& alt = "424m ASL")
        : m_stopId(id), m_stopName(name), m_passingBuses(buses),
          m_fullDayTimes(fullTimes), m_halfDayTimes(halfTimes),
          m_lat(lat), m_lon(lon), m_altitude(alt) {}

    // Encapsulation
    std::string getStopId() const { return m_stopId; }
    std::string getStopName() const { return m_stopName; }
    std::string getPassingBuses() const { return m_passingBuses; }
    std::string getFullDayTimes() const { return m_fullDayTimes; }
    std::string getHalfDayTimes() const { return m_halfDayTimes; }
    double getLat() const { return m_lat; }
    double getLon() const { return m_lon; }
    std::string getAltitude() const { return m_altitude; }

    void setFullDayTimes(const std::string& t) { m_fullDayTimes = t; }
    void setHalfDayTimes(const std::string& t) { m_halfDayTimes = t; }
};

#endif // STOP_HPP
