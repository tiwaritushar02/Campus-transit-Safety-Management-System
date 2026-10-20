/**
 * ============================================================================
 * MODULE: Member 4 - Connecting Bus Logic, Attendance & File Handling
 * FILE: AttendanceLogger.cpp
 * ============================================================================
 */

#include "AttendanceLogger.hpp"
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <filesystem>

using namespace std;

AttendanceLogger::AttendanceLogger(const string& filePath)
    : logFilePath(filePath) {
    loadFromFile();
}

void AttendanceLogger::logBoarding(const string& studentRoll, const string& busId,
                                   const string& stopId, const string& status) {
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    
    char timeBuf[64];
    strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", localtime(&now_c));

    AttendanceRecord rec{string(timeBuf), studentRoll, busId, stopId, status};
    records.push_back(rec);

    try {
        filesystem::path p(logFilePath);
        if (p.has_parent_path()) filesystem::create_directories(p.parent_path());
    } catch (...) {}

    // Immediately append to log file (File I/O)
    ofstream outFile(logFilePath, ios::app);
    if (outFile.is_open()) {
        outFile << rec.timestamp << "," << rec.studentRoll << ","
                << rec.busId << "," << rec.stopId << "," << rec.status << "\n";
    }
}

void AttendanceLogger::displayLogs() const {
    cout << "===========================================================\n";
    cout << "             STUDENT ATTENDANCE / BOARDING LOG             \n";
    cout << "===========================================================\n";
    cout << left << setw(22) << "TIMESTAMP"
              << setw(15) << "ROLL NO"
              << setw(10) << "BUS ID"
              << setw(12) << "STOP ID"
              << "STATUS\n";
    cout << "-----------------------------------------------------------\n";

    if (records.empty()) {
        cout << "  [No attendance records logged yet]\n";
    } else {
        for (const auto& r : records) {
            cout << left << setw(22) << r.timestamp
                      << setw(15) << r.studentRoll
                      << setw(10) << r.busId
                      << setw(12) << r.stopId
                      << r.status << "\n";
        }
    }
    cout << "===========================================================\n";
}

void AttendanceLogger::saveToFile() const {
    ofstream outFile(logFilePath, ios::trunc);
    if (outFile.is_open()) {
        for (const auto& r : records) {
            outFile << r.timestamp << "," << r.studentRoll << ","
                    << r.busId << "," << r.stopId << "," << r.status << "\n";
        }
    }
}

void AttendanceLogger::loadFromFile() {
    records.clear();
    ifstream inFile(logFilePath);
    if (!inFile.is_open()) return;

    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string time, roll, bus, stop, stat;

        if (getline(ss, time, ',') &&
            getline(ss, roll, ',') &&
            getline(ss, bus, ',') &&
            getline(ss, stop, ',') &&
            getline(ss, stat)) {
            records.push_back({time, roll, bus, stop, stat});
        }
    }
}
