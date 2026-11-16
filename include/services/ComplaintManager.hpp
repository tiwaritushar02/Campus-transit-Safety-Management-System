#ifndef COMPLAINT_MANAGER_HPP
#define COMPLAINT_MANAGER_HPP

#include "FileManager.hpp"
#include "Complaint.hpp"
#include "Poll.hpp"
#include <vector>
#include <string>

// Encapsulation
class ComplaintManager {
private:
    // Composition
    FileManager& m_fileManager;
    std::vector<Complaint> m_complaints;
    std::vector<Poll> m_polls;

public:
    // Constructor
    explicit ComplaintManager(FileManager& fm);

    void reload();

    const std::vector<Complaint>& getComplaints() const { return m_complaints; }
    const std::vector<Poll>& getPolls() const { return m_polls; }

    std::string submitComplaint(const std::string& cat, const std::string& desc,
                                const std::string& repName, const std::string& repRole,
                                const std::string& accName, const std::string& accRole,
                                const std::string& bus);

    bool castPollVote(const std::string& complaintId, const std::string& voterRoll, bool isYes);

    bool investigateComplaint(const std::string& id, const std::string& note, const std::string& adminUser = "Admin");
    bool resolveComplaint(const std::string& id, const std::string& note, const std::string& adminUser = "Admin");
    bool rejectComplaint(const std::string& id, const std::string& note, const std::string& adminUser = "Admin");

    const Poll* findPollForComplaint(const std::string& complaintId) const;
};

#endif // COMPLAINT_MANAGER_HPP
