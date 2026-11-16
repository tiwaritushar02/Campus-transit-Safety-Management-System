#ifndef TRIP_HPP
#define TRIP_HPP

#include <string>
#include <vector>

// Encapsulation
class Trip {
private:
    std::string m_tripId;
    std::string m_routeId;
    std::string m_sourceStopId;
    std::string m_sourceStopName;
    std::string m_destStopId;
    std::string m_destStopName;
    std::string m_departureTime;
    std::string m_assignedBusId;
    std::string m_assignedDriverId;
    std::string m_status;
    bool m_alertSent;

public:
    // Function Overloading
    // Constructor
    Trip();

    // Function Overloading
    // Constructor
    Trip(const std::string& tripId, const std::string& routeId,
         const std::string& sourceStopId, const std::string& sourceStopName,
         const std::string& destStopId, const std::string& destStopName,
         const std::string& departureTime, const std::string& assignedBusId,
         const std::string& assignedDriverId, const std::string& status,
         bool alertSent = false);

    // Encapsulation
    std::string getTripId() const { return m_tripId; }
    std::string getRouteId() const { return m_routeId; }
    std::string getSourceStopId() const { return m_sourceStopId; }
    std::string getSourceStopName() const { return m_sourceStopName; }
    std::string getDestStopId() const { return m_destStopId; }
    std::string getDestStopName() const { return m_destStopName; }
    std::string getDepartureTime() const { return m_departureTime; }
    std::string getAssignedBusId() const { return m_assignedBusId; }
    std::string getAssignedDriverId() const { return m_assignedDriverId; }
    std::string getStatus() const { return m_status; }
    bool isAlertSent() const { return m_alertSent; }

    bool isBusAssigned() const { return !m_assignedBusId.empty() && m_assignedBusId != "NONE"; }

    void setAssignedBusId(const std::string& busId) { m_assignedBusId = busId; }
    void setAssignedDriverId(const std::string& driverId) { m_assignedDriverId = driverId; }
    void setStatus(const std::string& status) { m_status = status; }
    void setAlertSent(bool sent) { m_alertSent = sent; }
    void setDepartureTime(const std::string& time) { m_departureTime = time; }
    void setSource(const std::string& stopId, const std::string& stopName) {
        m_sourceStopId = stopId;
        m_sourceStopName = stopName;
    }

    std::string toCsv() const;
    static Trip fromCsv(const std::vector<std::string>& tokens);
};

#endif // TRIP_HPP
