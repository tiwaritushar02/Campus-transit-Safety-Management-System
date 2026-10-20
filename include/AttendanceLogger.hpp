#ifndef ATTENDANCE_LOGGER_HPP
#define ATTENDANCE_LOGGER_HPP

/**
 * ============================================================================
 * MODULE: Member 4 - Connecting Bus Logic, Attendance & File Handling
 * FILE: AttendanceLogger.hpp
 * 
 * OOP CONCEPTS DEMONSTRATED:
 * 1. ENCAPSULATION & STRUCT DATA MODELING: AttendanceRecord encapsulates
 *    boarding events.
 * 2. PERSISTENCE & FILE HANDLING: Logs student boarding events to text file.
 * ============================================================================
 */

#include <string>
#include <vector>
#include <iostream>

using namespace std;

struct AttendanceRecord {
    string timestamp;
    string studentRoll;
    string busId;
    string stopId;
    string status; // "SUCCESS", "OVERFLOW_DENIED", "EXPIRED_PASS"
};

class AttendanceLogger {
private:
    vector<AttendanceRecord> records;
    string logFilePath;

public:
    explicit AttendanceLogger(const string& filePath = "data/attendance.log");

    void logBoarding(const string& studentRoll, const string& busId,
                     const string& stopId, const string& status);

    void displayLogs() const;
    const vector<AttendanceRecord>& getRecords() const { return records; }

    void saveToFile() const;
    void loadFromFile();
};

#endif // ATTENDANCE_LOGGER_HPP
