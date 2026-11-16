#ifndef FILE_MANAGER_HPP
#define FILE_MANAGER_HPP

#include <string>
#include <vector>
#include <memory>
#include "User.hpp"
#include "Admin.hpp"
#include "Student.hpp"
#include "Driver.hpp"
#include "Parent.hpp"
#include "Bus.hpp"
#include "Stop.hpp"
#include "Route.hpp"
#include "Complaint.hpp"
#include "AttendanceRecord.hpp"
#include "Poll.hpp"
#include "Trip.hpp"

// Encapsulation
class FileManager {
private:
    std::string m_dataDir;

    std::vector<std::string> splitCsv(const std::string& line) const;
    std::string escapeCsv(const std::string& field) const;

public:
    // Constructor
    explicit FileManager(const std::string& dataDir = "data");

    // Account & User Persistence (users.txt, students.csv, drivers.csv)
    std::vector<std::shared_ptr<User>> loadUsers();
    bool saveUsers(const std::vector<std::shared_ptr<User>>& users);

    // Fleet & Corridors (buses.csv, routes.csv, stops.csv)
    std::vector<Bus> loadBuses();
    bool saveBuses(const std::vector<Bus>& buses);

    std::vector<Route> loadRoutes();
    bool saveRoutes(const std::vector<Route>& routes);

    std::vector<Stop> loadStops();
    bool saveStops(const std::vector<Stop>& stops);

    // Attendance (attendance.csv)
    std::vector<AttendanceRecord> loadAttendance();
    bool appendAttendance(const AttendanceRecord& record);

    // Complaints & Polling (complaints.csv, polls.csv, votes.csv)
    std::vector<Complaint> loadComplaints();
    bool saveComplaints(const std::vector<Complaint>& complaints);
    bool appendComplaint(const Complaint& complaint);

    std::vector<Poll> loadPolls();
    bool savePolls(const std::vector<Poll>& polls);
    bool appendVote(const std::string& pollId, const std::string& roll, bool isYes);

    // Notifications (notifications.txt)
    std::vector<std::string> loadNotifications();
    bool appendNotification(const std::string& note);

    // Scheduled Trips & Bus Assignments (trips.csv)
    std::vector<Trip> loadTrips();
    bool saveTrips(const std::vector<Trip>& trips);
    bool appendTrip(const Trip& trip);
};

#endif // FILE_MANAGER_HPP
