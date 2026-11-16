#include "AttendanceManager.hpp"
#include <chrono>
#include <ctime>

AttendanceManager::AttendanceManager(FileManager& fm) : m_fileManager(fm) {
    reload();
}

void AttendanceManager::reload() {
    m_records = m_fileManager.loadAttendance();
}

bool AttendanceManager::markBoarding(const std::string& roll, const std::string& name,
                                     const std::string& busId, const std::string& stopId, bool feePaid) {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char tsBuf[30];
    std::strftime(tsBuf, sizeof(tsBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    std::string feeStatus = feePaid ? "PAID" : "PENDING";
    std::string boardStatus = feePaid ? "BOARDED" : "DENIED";

    AttendanceRecord record(tsBuf, roll, name, busId, stopId, feeStatus, boardStatus);
    m_records.push_back(record);
    m_fileManager.appendAttendance(record);

    return feePaid;
}
