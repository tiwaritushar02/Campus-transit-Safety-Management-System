#include "FileManager.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <ctime>

namespace fs = std::filesystem;

FileManager::FileManager(const std::string& dataDir) : m_dataDir(dataDir) {
    std::vector<std::string> candidates = {
        dataDir,
        "data",
        "../data",
        "../../data",
        "/Users/tushartiwari565/Documents/SmartCollegeBus_Qt6_Migration/data"
    };
    for (const auto& c : candidates) {
        if (fs::exists(c) && fs::exists(c + "/users.txt")) {
            m_dataDir = c;
            return;
        }
    }
    if (!fs::exists(m_dataDir)) {
        fs::create_directories(m_dataDir);
    }
}

std::vector<std::string> FileManager::splitCsv(const std::string& line) const {
    std::vector<std::string> tokens;
    std::string token;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            tokens.push_back(token);
            token.clear();
        } else {
            token += c;
        }
    }
    tokens.push_back(token);
    return tokens;
}

std::string FileManager::escapeCsv(const std::string& field) const {
    if (field.find(',') != std::string::npos || field.find('"') != std::string::npos) {
        std::string res = "\"";
        for (char c : field) {
            if (c == '"') res += "\"\"";
            else res += c;
        }
        res += "\"";
        return res;
    }
    return field;
}

std::vector<std::shared_ptr<User>> FileManager::loadUsers() {
    std::vector<std::shared_ptr<User>> users;

    std::vector<std::string> searchPaths = {
        m_dataDir + "/users.txt",
        "data/users.txt",
        "../data/users.txt",
        "../../data/users.txt",
        "/Users/tushartiwari565/Documents/SmartCollegeBus_Qt6_Migration/data/users.txt"
    };

    std::ifstream file;
    for (const auto& p : searchPaths) {
        file.open(p);
        if (file.is_open()) break;
    }

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::stringstream ss(line);
            std::string uName, pass, roleStr, pId;
            std::getline(ss, uName, ':');
            std::getline(ss, pass, ':');
            std::getline(ss, roleStr, ':');
            std::getline(ss, pId, ':');

            if (roleStr == "ADMIN") {
                users.push_back(std::make_shared<Admin>(uName, "College Transport Head", "+91-9876543210", "admin@gehu.ac.in", pass));
            } else if (roleStr == "STUDENT") {
                users.push_back(std::make_shared<Student>(uName, "Student " + uName, "+91-9811223344", uName + "@college.edu", pass, uName, "R1", "ST101", true));
            } else if (roleStr == "DRIVER") {
                users.push_back(std::make_shared<Driver>(uName, "Driver " + uName, "+91-9876500001", uName + "@gehu.ac.in", pass, "UK-04-DL-8888", 10, "B-101"));
            } else if (roleStr == "PARENT") {
                users.push_back(std::make_shared<Parent>(uName, "Parent " + uName, "+91-9811001122", uName + "@parents.edu", pass, "2024-CS-001", "Aarav Sharma", 0.0));
            }
        }
        file.close();
    }

    // Reliable fallback default users so login NEVER fails even if text file is missing
    if (users.empty()) {
        users.push_back(std::make_shared<Admin>("admin", "College Transport Head", "+91-9876543210", "admin@gehu.ac.in", "admin123"));
        users.push_back(std::make_shared<Student>("2024-CS-001", "Aarav Sharma", "+91-9811223344", "aarav@college.edu", "student123", "2024-CS-001", "R1", "ST101", true));
        users.push_back(std::make_shared<Driver>("D-01", "Rajesh Kumar", "+91-9876500001", "rajesh@gehu.ac.in", "driver123", "UK-04-DL-8888", 10, "B-101"));
        users.push_back(std::make_shared<Parent>("P-101", "Vijay Sharma", "+91-9811001122", "vijay@parents.edu", "parent123", "2024-CS-001", "Aarav Sharma", 0.0));
    }

    return users;
}

std::vector<Bus> FileManager::loadBuses() {
    std::vector<Bus> buses;
    std::ifstream f(m_dataDir + "/buses.csv");
    if (!f.is_open()) return buses;

    std::string line;
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        auto tokens = splitCsv(line);
        if (tokens.size() >= 4) {
            buses.emplace_back(tokens[0], tokens[1], std::stoi(tokens[2]), tokens[3]);
        }
    }
    return buses;
}

bool FileManager::saveBuses(const std::vector<Bus>& buses) {
    std::ofstream f(m_dataDir + "/buses.csv");
    if (!f.is_open()) return false;
    for (const auto& b : buses) {
        f << b.getBusId() << "," << b.getRegistrationNumber() << ","
          << b.getCapacity() << "," << b.getAssignedRouteId() << "\n";
    }
    return true;
}

std::vector<Route> FileManager::loadRoutes() {
    std::vector<Route> routes;
    std::ifstream f(m_dataDir + "/routes.csv");
    if (!f.is_open()) return routes;

    std::string line;
    bool header = true;
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        if (header) { header = false; continue; }
        auto tokens = splitCsv(line);
        if (tokens.size() >= 4) {
            routes.emplace_back(tokens[0], tokens[1], std::stod(tokens[2]), std::stoi(tokens[3]));
        }
    }
    return routes;
}

std::vector<Stop> FileManager::loadStops() {
    std::vector<Stop> stops;
    std::ifstream f(m_dataDir + "/stops.csv");
    if (!f.is_open()) return stops;

    std::string line;
    bool header = true;
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        if (header) { header = false; continue; }
        auto tokens = splitCsv(line);
        if (tokens.size() >= 5) {
            double lat = 29.28, lon = 79.54;
            std::string alt = "424m ASL";
            if (tokens[0] == "ST101") { lat = 29.2183; lon = 79.5130; alt = "424m ASL"; }
            else if (tokens[0] == "ST102") { lat = 29.2715; lon = 79.5312; alt = "554m ASL"; }
            else if (tokens[0] == "ST103") { lat = 29.2942; lon = 79.5445; alt = "680m ASL"; }
            else if (tokens[0] == "ST104") { lat = 29.3210; lon = 79.5510; alt = "950m ASL"; }
            else if (tokens[0] == "ST105") { lat = 29.3458; lon = 79.5583; alt = "1,370m ASL"; }
            else if (tokens[0] == "ST106") { lat = 29.3565; lon = 79.5620; alt = "1,420m ASL"; }

            stops.emplace_back(tokens[0], tokens[1], tokens[2], tokens[3], tokens[4], lat, lon, alt);
        }
    }
    return stops;
}

bool FileManager::saveStops(const std::vector<Stop>& stops) {
    std::ofstream f(m_dataDir + "/stops.csv");
    if (!f.is_open()) return false;
    f << "stopId,stopName,passingBuses,fullDayTimes,halfDayTimes\n";
    for (const auto& s : stops) {
        f << s.getStopId() << "," << escapeCsv(s.getStopName()) << ","
          << escapeCsv(s.getPassingBuses()) << "," << escapeCsv(s.getFullDayTimes()) << ","
          << escapeCsv(s.getHalfDayTimes()) << "\n";
    }
    return true;
}

std::vector<AttendanceRecord> FileManager::loadAttendance() {
    std::vector<AttendanceRecord> records;
    std::ifstream f(m_dataDir + "/attendance.csv");
    if (!f.is_open()) return records;

    std::string line;
    bool header = true;
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        if (header) { header = false; continue; }
        auto t = splitCsv(line);
        if (t.size() >= 7) {
            records.emplace_back(t[0], t[1], t[2], t[3], t[4], t[5], t[6]);
        }
    }
    return records;
}

bool FileManager::appendAttendance(const AttendanceRecord& r) {
    std::ofstream f(m_dataDir + "/attendance.csv", std::ios::app);
    if (!f.is_open()) return false;
    f << r.getTimestamp() << "," << r.getRollNumber() << "," << r.getStudentName() << ","
      << r.getBusId() << "," << r.getStopId() << "," << r.getFeeStatus() << ","
      << r.getBoardingStatus() << "\n";
    return true;
}

std::vector<Complaint> FileManager::loadComplaints() {
    std::vector<Complaint> complaints;
    std::ifstream f(m_dataDir + "/complaints.csv");
    if (!f.is_open()) return complaints;

    std::string line;
    bool header = true;
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        if (header) { header = false; continue; }
        auto t = splitCsv(line);
        if (t.size() >= 11) {
            std::string notes = (t.size() >= 12) ? t[11] : "";
            complaints.emplace_back(t[0], t[1], t[2], t[3], t[4], t[5], t[6], t[7], t[8], t[9], t[10], notes);
        }
    }
    return complaints;
}

bool FileManager::saveComplaints(const std::vector<Complaint>& complaints) {
    std::ofstream f(m_dataDir + "/complaints.csv");
    if (!f.is_open()) return false;
    f << "id,category,description,reporterName,reporterRole,accusedName,accusedRole,busId,date,time,status,adminNotes\n";
    for (const auto& c : complaints) {
        f << c.getId() << "," << escapeCsv(c.getCategory()) << "," << escapeCsv(c.getDescription()) << ","
          << escapeCsv(c.getReporterName()) << "," << c.getReporterRole() << ","
          << escapeCsv(c.getAccusedName()) << "," << c.getAccusedRole() << ","
          << c.getBusId() << "," << c.getDate() << "," << c.getTime() << ","
          << c.getStatus() << "," << escapeCsv(c.getAdminNotes()) << "\n";
    }
    return true;
}

bool FileManager::appendComplaint(const Complaint& c) {
    std::ofstream f(m_dataDir + "/complaints.csv", std::ios::app);
    if (!f.is_open()) return false;
    f << c.getId() << "," << escapeCsv(c.getCategory()) << "," << escapeCsv(c.getDescription()) << ","
      << escapeCsv(c.getReporterName()) << "," << c.getReporterRole() << ","
      << escapeCsv(c.getAccusedName()) << "," << c.getAccusedRole() << ","
      << c.getBusId() << "," << c.getDate() << "," << c.getTime() << ","
      << c.getStatus() << "," << escapeCsv(c.getAdminNotes()) << "\n";
    return true;
}

std::vector<Poll> FileManager::loadPolls() {
    std::vector<Poll> polls;
    std::ifstream f(m_dataDir + "/polls.csv");
    if (!f.is_open()) return polls;

    std::string line;
    bool header = true;
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        if (header) { header = false; continue; }
        auto t = splitCsv(line);
        if (t.size() >= 7) {
            std::string comp = (t.size() >= 8) ? t[7] : "";
            std::string acc = (t.size() >= 9) ? t[8] : "";
            Poll p(t[0], t[1], t[2], t[3] == "1", std::stoi(t[4]), std::stoi(t[5]), std::stoi(t[6]), comp, acc);
            polls.push_back(p);
        }
    }

    // Load recorded votes to populate voted rolls
    std::ifstream vf(m_dataDir + "/votes.csv");
    if (vf.is_open()) {
        std::string vline;
        bool vhead = true;
        while (std::getline(vf, vline)) {
            if (vline.empty()) continue;
            if (vhead) { vhead = false; continue; }
            auto vt = splitCsv(vline);
            if (vt.size() >= 2) {
                for (auto& p : polls) {
                    if (p.getPollId() == vt[0]) {
                        p.addVotedRoll(vt[1]);
                    }
                }
            }
        }
    }

    return polls;
}

bool FileManager::savePolls(const std::vector<Poll>& polls) {
    std::ofstream f(m_dataDir + "/polls.csv");
    if (!f.is_open()) return false;
    f << "pollId,complaintId,question,active,totalVotes,yesVotes,noVotes,complainantRoll,accusedRoll\n";
    for (const auto& p : polls) {
        f << p.getPollId() << "," << p.getComplaintId() << "," << escapeCsv(p.getQuestion()) << ","
          << (p.isActive() ? "1" : "0") << "," << p.getTotalVotes() << "," << p.getYesVotes() << ","
          << p.getNoVotes() << "," << p.getComplainantRoll() << "," << p.getAccusedRoll() << "\n";
    }
    return true;
}

bool FileManager::appendVote(const std::string& pollId, const std::string& roll, bool isYes) {
    std::ofstream f(m_dataDir + "/votes.csv", std::ios::app);
    if (!f.is_open()) return false;
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char buf[30];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    f << pollId << "," << roll << "," << (isYes ? "YES" : "NO") << "," << buf << "\n";
    return true;
}

std::vector<std::string> FileManager::loadNotifications() {
    std::vector<std::string> notes;
    std::ifstream f(m_dataDir + "/notifications.txt");
    if (!f.is_open()) return notes;
    std::string line;
    while (std::getline(f, line)) {
        if (!line.empty()) notes.push_back(line);
    }
    return notes;
}

bool FileManager::appendNotification(const std::string& note) {
    std::ofstream f(m_dataDir + "/notifications.txt", std::ios::app);
    if (!f.is_open()) return false;
    f << note << "\n";
    return true;
}

std::vector<Trip> FileManager::loadTrips() {
    std::vector<Trip> trips;
    std::string path = m_dataDir + "/trips.csv";
    std::ifstream f(path);
    if (!f.is_open()) {
        // Fallback default sample trips if file doesn't exist yet
        trips.push_back(Trip("TRIP-101", "R1", "ST101", "Haldwani Bus Terminal (Tikonia)", "ST106", "Graphic Era Hill University, Bhimtal Campus", "07:30 AM", "B-101", "D101", "Assigned", false));
        trips.push_back(Trip("TRIP-102", "R1", "ST102", "Kathgodam Railway Station", "ST106", "Graphic Era Hill University, Bhimtal Campus", "07:45 AM", "B-102", "D102", "Assigned", false));
        trips.push_back(Trip("TRIP-103", "R1", "ST101", "Haldwani Bus Terminal (Tikonia)", "ST106", "Graphic Era Hill University, Bhimtal Campus", "08:00 AM", "NONE", "NONE", "Bus Not Assigned", false));
        trips.push_back(Trip("TRIP-104", "R2", "ST103", "Ranibagh Junction", "ST106", "Graphic Era Hill University, Bhimtal Campus", "08:15 AM", "NONE", "NONE", "Bus Not Assigned", false));
        saveTrips(trips);
        return trips;
    }

    std::string line;
    bool isHeader = true;
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        if (isHeader) {
            isHeader = false;
            if (line.find("tripId") != std::string::npos) continue;
        }
        auto tokens = splitCsv(line);
        if (tokens.size() >= 10) {
            trips.push_back(Trip::fromCsv(tokens));
        }
    }
    f.close();
    return trips;
}

bool FileManager::saveTrips(const std::vector<Trip>& trips) {
    std::string path = m_dataDir + "/trips.csv";
    std::ofstream f(path);
    if (!f.is_open()) return false;
    f << "tripId,routeId,sourceStopId,sourceStopName,destStopId,destStopName,departureTime,assignedBusId,assignedDriverId,status,alertSent\n";
    for (const auto& t : trips) {
        f << t.toCsv() << "\n";
    }
    f.close();
    return true;
}

bool FileManager::appendTrip(const Trip& trip) {
    std::string path = m_dataDir + "/trips.csv";
    bool needHeader = !fs::exists(path);
    std::ofstream f(path, std::ios::app);
    if (!f.is_open()) return false;
    if (needHeader) {
        f << "tripId,routeId,sourceStopId,sourceStopName,destStopId,destStopName,departureTime,assignedBusId,assignedDriverId,status,alertSent\n";
    }
    f << trip.toCsv() << "\n";
    f.close();
    return true;
}
