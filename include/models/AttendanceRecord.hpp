#ifndef ATTENDANCE_RECORD_HPP
#define ATTENDANCE_RECORD_HPP

#include <string>

// Encapsulation
class AttendanceRecord {
private:
    std::string m_timestamp;
    std::string m_rollNumber;
    std::string m_studentName;
    std::string m_busId;
    std::string m_stopId;
    std::string m_feeStatus;
    std::string m_boardingStatus;

public:
    // Constructor
    AttendanceRecord(const std::string& ts = "", const std::string& roll = "",
                     const std::string& name = "", const std::string& bus = "",
                     const std::string& stop = "", const std::string& fee = "PAID",
                     const std::string& boarding = "BOARDED")
        : m_timestamp(ts), m_rollNumber(roll), m_studentName(name),
          m_busId(bus), m_stopId(stop), m_feeStatus(fee),
          m_boardingStatus(boarding) {}

    // Encapsulation
    std::string getTimestamp() const { return m_timestamp; }
    std::string getRollNumber() const { return m_rollNumber; }
    std::string getStudentName() const { return m_studentName; }
    std::string getBusId() const { return m_busId; }
    std::string getStopId() const { return m_stopId; }
    std::string getFeeStatus() const { return m_feeStatus; }
    std::string getBoardingStatus() const { return m_boardingStatus; }
};

#endif // ATTENDANCE_RECORD_HPP
