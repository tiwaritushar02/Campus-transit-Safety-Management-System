#include "BusAssignmentMonitorService.hpp"
#include <QDateTime>
#include <iostream>
#include <algorithm>

// Constructor
BusAssignmentMonitorService::BusAssignmentMonitorService(FileManager& fm, QObject* parent)
    : QObject(parent), m_fileManager(fm) {
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &BusAssignmentMonitorService::onMonitorTick);
    m_trips = m_fileManager.loadTrips();
}

// Destructor
BusAssignmentMonitorService::~BusAssignmentMonitorService() {
    stopMonitoring();
}

void BusAssignmentMonitorService::startMonitoring(int intervalMs) {
    if (!m_timer->isActive()) {
        m_timer->start(intervalMs);
    }
    // Immediate initial check
    manualCheckNow();
}

void BusAssignmentMonitorService::stopMonitoring() {
    if (m_timer->isActive()) {
        m_timer->stop();
    }
}

std::vector<std::string> BusAssignmentMonitorService::getStudentsForTrip(const Trip& trip) {
    std::vector<std::string> affectedStudents;
    auto users = m_fileManager.loadUsers();
    for (const auto& u : users) {
        if (u && u->getRole() == UserRole::STUDENT) {
            auto s = std::dynamic_pointer_cast<Student>(u);
            if (s) {
                // If student is assigned to this route or boards at this stop
                if (s->getAssignedRouteId() == trip.getRouteId() || s->getAssignedStopId() == trip.getSourceStopId()) {
                    affectedStudents.push_back(s->getRollNumber().empty() ? s->getId() : s->getRollNumber());
                }
            }
        }
    }
    // If no specific match, default to registered student demo roll numbers
    if (affectedStudents.empty()) {
        affectedStudents.push_back("2024-CS-001");
        affectedStudents.push_back("2024-EC-045");
    }
    return affectedStudents;
}

void BusAssignmentMonitorService::onMonitorTick() {
    manualCheckNow();
}

void BusAssignmentMonitorService::manualCheckNow() {
    m_trips = m_fileManager.loadTrips();
    bool changed = false;

    for (auto& t : m_trips) {
        // Situation: Bus is not assigned
        if (!t.isBusAssigned() && t.getStatus() != "Cancelled") {
            // Check if alert was already sent to avoid duplicate spamming
            if (!t.isAlertSent()) {
                t.setAlertSent(true);
                t.setStatus("Bus Not Assigned");
                changed = true;

                QString nowStr = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

                // 1. Send Admin Notification
                std::string adminNote = QString("[%1] [ADMIN ALERT] ACTION REQUIRED: BUS NOT ASSIGNED - The scheduled trip %2 from %3 to %4 has no bus assigned. Please assign a bus immediately.")
                                        .arg(nowStr)
                                        .arg(QString::fromStdString(t.getTripId()))
                                        .arg(QString::fromStdString(t.getSourceStopName()))
                                        .arg(QString::fromStdString(t.getDestStopName()))
                                        .toStdString();
                m_fileManager.appendNotification(adminNote);

                // 2. Send Student Notification to registered students for this trip/route ONLY
                auto students = getStudentsForTrip(t);
                for (const auto& roll : students) {
                    std::string studentNote = QString("[%1] [STUDENT ALERT - %2] BUS NOT ASSIGNED - Your scheduled college bus from %3 to %4 (Departure: %5) has not been assigned yet. The transport administration has been notified. Please check for further updates.")
                                              .arg(nowStr)
                                              .arg(QString::fromStdString(roll))
                                              .arg(QString::fromStdString(t.getSourceStopName()))
                                              .arg(QString::fromStdString(t.getDestStopName()))
                                              .arg(QString::fromStdString(t.getDepartureTime()))
                                              .toStdString();
                    m_fileManager.appendNotification(studentNote);
                }

                emit unassignedTripAlert(QString::fromStdString(t.getTripId()),
                                        QString::fromStdString(t.getSourceStopName()),
                                        static_cast<int>(students.size()));
            }
        }
    }

    if (changed) {
        m_fileManager.saveTrips(m_trips);
        emit tripListUpdated();
    }
}

std::vector<Trip> BusAssignmentMonitorService::getAllTrips() {
    m_trips = m_fileManager.loadTrips();
    return m_trips;
}

std::vector<Trip> BusAssignmentMonitorService::getUnassignedTrips() {
    m_trips = m_fileManager.loadTrips();
    std::vector<Trip> unassigned;
    for (const auto& t : m_trips) {
        if (!t.isBusAssigned() && t.getStatus() != "Cancelled") {
            unassigned.push_back(t);
        }
    }
    return unassigned;
}

std::vector<Trip> BusAssignmentMonitorService::getAssignedTrips() {
    m_trips = m_fileManager.loadTrips();
    std::vector<Trip> assigned;
    for (const auto& t : m_trips) {
        if (t.isBusAssigned() && t.getStatus() != "Cancelled") {
            assigned.push_back(t);
        }
    }
    return assigned;
}

Trip BusAssignmentMonitorService::getTripById(const std::string& tripId) {
    for (const auto& t : m_trips) {
        if (t.getTripId() == tripId) return t;
    }
    return Trip();
}

bool BusAssignmentMonitorService::checkBusConflict(const std::string& busId, const std::string& depTime, const std::string& excludeTripId) {
    if (busId.empty() || busId == "NONE") return false;
    for (const auto& t : m_trips) {
        if (t.getTripId() != excludeTripId && t.getStatus() != "Cancelled") {
            if (t.getAssignedBusId() == busId && t.getDepartureTime() == depTime) {
                return true; // Conflict detected: Bus is already scheduled at this time
            }
        }
    }
    return false;
}

bool BusAssignmentMonitorService::checkDriverConflict(const std::string& driverId, const std::string& depTime, const std::string& excludeTripId) {
    if (driverId.empty() || driverId == "NONE") return false;
    for (const auto& t : m_trips) {
        if (t.getTripId() != excludeTripId && t.getStatus() != "Cancelled") {
            if (t.getAssignedDriverId() == driverId && t.getDepartureTime() == depTime) {
                return true; // Conflict detected: Driver is already assigned at this time
            }
        }
    }
    return false;
}

bool BusAssignmentMonitorService::assignBusAndDriver(const std::string& tripId, const std::string& busId,
                                                    const std::string& driverId, std::string& errorMsg) {
    auto trips = m_fileManager.loadTrips();
    bool found = false;

    for (auto& t : trips) {
        if (t.getTripId() == tripId) {
            found = true;

            // Check overlap conflicts
            if (checkBusConflict(busId, t.getDepartureTime(), tripId)) {
                errorMsg = "Conflict Detected: Bus " + busId + " is already assigned to another trip at " + t.getDepartureTime();
                return false;
            }
            if (checkDriverConflict(driverId, t.getDepartureTime(), tripId)) {
                errorMsg = "Conflict Detected: Driver " + driverId + " is already assigned to another trip at " + t.getDepartureTime();
                return false;
            }

            t.setAssignedBusId(busId);
            t.setAssignedDriverId(driverId);
            t.setStatus("Assigned");
            t.setAlertSent(false); // Reset alert state

            // Notify affected students about confirmed assignment
            QString nowStr = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
            auto students = getStudentsForTrip(t);
            for (const auto& roll : students) {
                std::string confNote = QString("[%1] [STUDENT ALERT - %2] BUS ASSIGNED - Bus %3 (Driver: %4) has been assigned to your scheduled trip from %5 to %6 (Departure: %7).")
                                       .arg(nowStr)
                                       .arg(QString::fromStdString(roll))
                                       .arg(QString::fromStdString(busId))
                                       .arg(QString::fromStdString(driverId))
                                       .arg(QString::fromStdString(t.getSourceStopName()))
                                       .arg(QString::fromStdString(t.getDestStopName()))
                                       .arg(QString::fromStdString(t.getDepartureTime()))
                                       .toStdString();
                m_fileManager.appendNotification(confNote);
            }

            // Also log Admin confirmation
            std::string adminConf = QString("[%1] [ADMIN NOTICE] Bus %2 & Driver %3 successfully assigned to trip %4.")
                                    .arg(nowStr)
                                    .arg(QString::fromStdString(busId))
                                    .arg(QString::fromStdString(driverId))
                                    .arg(QString::fromStdString(tripId))
                                    .toStdString();
            m_fileManager.appendNotification(adminConf);

            emit tripAssignedNotice(QString::fromStdString(tripId), QString::fromStdString(busId), QString::fromStdString(driverId));
            break;
        }
    }

    if (!found) {
        errorMsg = "Trip ID not found.";
        return false;
    }

    m_fileManager.saveTrips(trips);
    m_trips = trips;
    emit tripListUpdated();
    return true;
}

bool BusAssignmentMonitorService::createTrip(const std::string& routeId, const std::string& sourceStopId,
                                            const std::string& sourceStopName, const std::string& departureTime,
                                            const std::string& busId, const std::string& driverId) {
    auto trips = m_fileManager.loadTrips();
    int nextNum = 101 + static_cast<int>(trips.size());
    std::string newTripId = "TRIP-" + std::to_string(nextNum);
    std::string status = (busId.empty() || busId == "NONE") ? "Bus Not Assigned" : "Assigned";

    Trip newTrip(newTripId, routeId, sourceStopId, sourceStopName,
                 "ST106", "Graphic Era Hill University, Bhimtal Campus",
                 departureTime, busId.empty() ? "NONE" : busId,
                 driverId.empty() ? "NONE" : driverId, status, false);

    trips.push_back(newTrip);
    m_fileManager.saveTrips(trips);
    m_trips = trips;

    // Trigger immediate check to notify if unassigned
    manualCheckNow();
    emit tripListUpdated();
    return true;
}

bool BusAssignmentMonitorService::cancelTrip(const std::string& tripId) {
    auto trips = m_fileManager.loadTrips();
    bool found = false;
    for (auto& t : trips) {
        if (t.getTripId() == tripId) {
            t.setStatus("Cancelled");
            found = true;
            QString nowStr = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
            auto students = getStudentsForTrip(t);
            for (const auto& roll : students) {
                std::string cancelNote = QString("[%1] [STUDENT ALERT - %2] TRIP CANCELLED - Scheduled trip %3 from %4 has been cancelled by College Transport Administration.")
                                         .arg(nowStr).arg(QString::fromStdString(roll)).arg(QString::fromStdString(tripId))
                                         .arg(QString::fromStdString(t.getSourceStopName())).toStdString();
                m_fileManager.appendNotification(cancelNote);
            }
            break;
        }
    }
    if (found) {
        m_fileManager.saveTrips(trips);
        m_trips = trips;
        emit tripListUpdated();
    }
    return found;
}
