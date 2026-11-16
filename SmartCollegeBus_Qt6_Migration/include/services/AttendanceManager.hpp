#ifndef ATTENDANCE_MANAGER_HPP
#define ATTENDANCE_MANAGER_HPP

#include "FileManager.hpp"
#include "AttendanceRecord.hpp"
#include <vector>
#include <string>

// Encapsulation
class AttendanceManager {
private:
    // Composition
    FileManager& m_fileManager;
    std::vector<AttendanceRecord> m_records;

public:
    // Constructor
    explicit AttendanceManager(FileManager& fm);

    void reload();
    const std::vector<AttendanceRecord>& getRecords() const { return m_records; }

    bool markBoarding(const std::string& roll, const std::string& name, const std::string& busId,
                      const std::string& stopId, bool feePaid);
};

#endif // ATTENDANCE_MANAGER_HPP
