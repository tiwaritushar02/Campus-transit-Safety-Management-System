#ifndef BUS_ASSIGNMENT_MONITOR_SERVICE_HPP
#define BUS_ASSIGNMENT_MONITOR_SERVICE_HPP

#include <QObject>
#include <QTimer>
#include <vector>
#include <string>
#include "FileManager.hpp"
#include "Trip.hpp"

// Inheritance
class BusAssignmentMonitorService : public QObject {
    Q_OBJECT

private:
    // Composition
    FileManager& m_fileManager;
    QTimer* m_timer;
    std::vector<Trip> m_trips;

    std::vector<std::string> getStudentsForTrip(const Trip& trip);

private slots:
    void onMonitorTick();

signals:
    void unassignedTripAlert(const QString& tripId, const QString& sourceName, int affectedStudents);
    void tripAssignedNotice(const QString& tripId, const QString& busId, const QString& driverName);
    void tripListUpdated();

public:
    // Constructor
    explicit BusAssignmentMonitorService(FileManager& fm, QObject* parent = nullptr);

    // Destructor
    ~BusAssignmentMonitorService() override;

    void startMonitoring(int intervalMs = 5000);
    void stopMonitoring();

    std::vector<Trip> getAllTrips();
    std::vector<Trip> getUnassignedTrips();
    std::vector<Trip> getAssignedTrips();
    Trip getTripById(const std::string& tripId);

    bool createTrip(const std::string& routeId, const std::string& sourceStopId,
                    const std::string& sourceStopName, const std::string& departureTime,
                    const std::string& busId = "NONE", const std::string& driverId = "NONE");

    bool assignBusAndDriver(const std::string& tripId, const std::string& busId,
                            const std::string& driverId, std::string& errorMsg);

    bool cancelTrip(const std::string& tripId);

    bool checkBusConflict(const std::string& busId, const std::string& depTime, const std::string& excludeTripId);
    bool checkDriverConflict(const std::string& driverId, const std::string& depTime, const std::string& excludeTripId);

    void manualCheckNow();
};

#endif // BUS_ASSIGNMENT_MONITOR_SERVICE_HPP
