#include "Trip.hpp"
#include <sstream>

Trip::Trip()
    : m_tripId("TRIP-000"), m_routeId("R1"),
      m_sourceStopId("ST101"), m_sourceStopName("Haldwani Bus Terminal (Tikonia)"),
      m_destStopId("ST106"), m_destStopName("Graphic Era Hill University, Bhimtal Campus"),
      m_departureTime("08:00 AM"), m_assignedBusId("NONE"), m_assignedDriverId("NONE"),
      m_status("Bus Not Assigned"), m_alertSent(false) {}

Trip::Trip(const std::string& tripId, const std::string& routeId,
           const std::string& sourceStopId, const std::string& sourceStopName,
           const std::string& destStopId, const std::string& destStopName,
           const std::string& departureTime, const std::string& assignedBusId,
           const std::string& assignedDriverId, const std::string& status,
           bool alertSent)
    : m_tripId(tripId), m_routeId(routeId),
      m_sourceStopId(sourceStopId), m_sourceStopName(sourceStopName),
      m_destStopId(destStopId), m_destStopName(destStopName),
      m_departureTime(departureTime), m_assignedBusId(assignedBusId),
      m_assignedDriverId(assignedDriverId), m_status(status),
      m_alertSent(alertSent) {}

std::string Trip::toCsv() const {
    auto escape = [](const std::string& str) {
        if (str.find(',') != std::string::npos || str.find('"') != std::string::npos) {
            std::string res = "\"";
            for (char c : str) {
                if (c == '"') res += "\"\"";
                else res += c;
            }
            res += "\"";
            return res;
        }
        return str;
    };

    std::stringstream ss;
    ss << escape(m_tripId) << ","
       << escape(m_routeId) << ","
       << escape(m_sourceStopId) << ","
       << escape(m_sourceStopName) << ","
       << escape(m_destStopId) << ","
       << escape(m_destStopName) << ","
       << escape(m_departureTime) << ","
       << escape(m_assignedBusId) << ","
       << escape(m_assignedDriverId) << ","
       << escape(m_status) << ","
       << (m_alertSent ? "1" : "0");
    return ss.str();
}

Trip Trip::fromCsv(const std::vector<std::string>& tokens) {
    if (tokens.size() < 10) {
        return Trip();
    }
    std::string tripId = tokens[0];
    std::string routeId = tokens[1];
    std::string srcId = tokens[2];
    std::string srcName = tokens[3];
    std::string dstId = tokens[4];
    std::string dstName = tokens[5];
    std::string depTime = tokens[6];
    std::string busId = tokens[7];
    std::string driverId = tokens[8];
    std::string status = tokens[9];
    bool alertSent = (tokens.size() >= 11 && (tokens[10] == "1" || tokens[10] == "true"));

    return Trip(tripId, routeId, srcId, srcName, dstId, dstName, depTime, busId, driverId, status, alertSent);
}
