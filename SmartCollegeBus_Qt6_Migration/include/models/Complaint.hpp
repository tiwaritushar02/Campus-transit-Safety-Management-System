#ifndef COMPLAINT_HPP
#define COMPLAINT_HPP

#include <string>

// Encapsulation
class Complaint {
private:
    std::string m_id;
    std::string m_category;
    std::string m_description;
    std::string m_reporterName;
    std::string m_reporterRole;
    std::string m_accusedName;
    std::string m_accusedRole;
    std::string m_busId;
    std::string m_date;
    std::string m_time;
    std::string m_status;
    std::string m_adminNotes;

public:
    // Constructor
    Complaint(const std::string& id = "", const std::string& cat = "", const std::string& desc = "",
              const std::string& repName = "", const std::string& repRole = "",
              const std::string& accName = "", const std::string& accRole = "",
              const std::string& bus = "", const std::string& date = "",
              const std::string& time = "", const std::string& status = "OPEN",
              const std::string& notes = "")
        : m_id(id), m_category(cat), m_description(desc),
          m_reporterName(repName), m_reporterRole(repRole),
          m_accusedName(accName), m_accusedRole(accRole),
          m_busId(bus), m_date(date), m_time(time),
          m_status(status), m_adminNotes(notes) {}

    // Encapsulation
    std::string getId() const { return m_id; }
    std::string getCategory() const { return m_category; }
    std::string getDescription() const { return m_description; }
    std::string getReporterName() const { return m_reporterName; }
    std::string getReporterRole() const { return m_reporterRole; }
    std::string getAccusedName() const { return m_accusedName; }
    std::string getAccusedRole() const { return m_accusedRole; }
    std::string getBusId() const { return m_busId; }
    std::string getDate() const { return m_date; }
    std::string getTime() const { return m_time; }
    std::string getStatus() const { return m_status; }
    std::string getAdminNotes() const { return m_adminNotes; }

    void setStatus(const std::string& s) { m_status = s; }
    void setAdminNotes(const std::string& n) { m_adminNotes = n; }
};

#endif // COMPLAINT_HPP
