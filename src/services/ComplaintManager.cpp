#include "ComplaintManager.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

ComplaintManager::ComplaintManager(FileManager& fm) : m_fileManager(fm) {
    reload();
}

void ComplaintManager::reload() {
    m_complaints = m_fileManager.loadComplaints();
    m_polls = m_fileManager.loadPolls();
}

std::string ComplaintManager::submitComplaint(const std::string& cat, const std::string& desc,
                                             const std::string& repName, const std::string& repRole,
                                             const std::string& accName, const std::string& accRole,
                                             const std::string& bus) {
    std::string newId = "CMP-" + std::to_string(101 + m_complaints.size());

    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char dateBuf[20], timeBuf[20];
    std::strftime(dateBuf, sizeof(dateBuf), "%Y-%m-%d", std::localtime(&now));
    std::strftime(timeBuf, sizeof(timeBuf), "%I:%M %p", std::localtime(&now));

    std::string initialStatus = "OPEN";
    bool shouldTriggerPoll = (repRole == "Student" && accRole == "Student");

    if (shouldTriggerPoll) {
        initialStatus = "POLL ACTIVE";
    }

    Complaint c(newId, cat, desc, repName, repRole, accName, accRole, bus, dateBuf, timeBuf, initialStatus,
                shouldTriggerPoll ? "Peer disciplinary voting initiated across eligible students." : "Registered on portal.");

    m_complaints.push_back(c);
    m_fileManager.saveComplaints(m_complaints);

    if (shouldTriggerPoll) {
        std::string pollId = "POL-" + std::to_string(101 + m_polls.size());
        Poll p(pollId, newId, "Do you agree that this incident requires disciplinary action?", true, 0, 0, 0, repName, accName);
        m_polls.push_back(p);
        m_fileManager.savePolls(m_polls);
    }

    return newId;
}

bool ComplaintManager::castPollVote(const std::string& complaintId, const std::string& voterRoll, bool isYes) {
    for (auto& p : m_polls) {
        if (p.getComplaintId() == complaintId) {
            if (p.castVote(voterRoll, isYes)) {
                m_fileManager.savePolls(m_polls);
                m_fileManager.appendVote(p.getPollId(), voterRoll, isYes);
                return true;
            }
            return false;
        }
    }
    return false;
}

bool ComplaintManager::investigateComplaint(const std::string& id, const std::string& note, const std::string& adminUser) {
    for (auto& c : m_complaints) {
        if (c.getId() == id) {
            c.setStatus("UNDER INVESTIGATION");
            auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            char dtBuf[30];
            std::strftime(dtBuf, sizeof(dtBuf), "%Y-%m-%d %H:%M", std::localtime(&now));
            std::string entry = "[INVESTIGATION - " + std::string(dtBuf) + " | " + adminUser + "] " + note;
            std::string existing = c.getAdminNotes();
            c.setAdminNotes(existing.empty() ? entry : (existing + " | " + entry));
            m_fileManager.saveComplaints(m_complaints);
            return true;
        }
    }
    return false;
}

bool ComplaintManager::resolveComplaint(const std::string& id, const std::string& note, const std::string& adminUser) {
    for (auto& c : m_complaints) {
        if (c.getId() == id) {
            c.setStatus("RESOLVED");
            auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            char dtBuf[30];
            std::strftime(dtBuf, sizeof(dtBuf), "%Y-%m-%d %H:%M", std::localtime(&now));
            std::string entry = "[RESOLVED - " + std::string(dtBuf) + " | " + adminUser + "] " + note;
            std::string existing = c.getAdminNotes();
            c.setAdminNotes(existing.empty() ? entry : (existing + " | " + entry));
            m_fileManager.saveComplaints(m_complaints);
            for (auto& p : m_polls) {
                if (p.getComplaintId() == id) {
                    p.setActive(false);
                    m_fileManager.savePolls(m_polls);
                    break;
                }
            }
            return true;
        }
    }
    return false;
}

bool ComplaintManager::rejectComplaint(const std::string& id, const std::string& note, const std::string& adminUser) {
    for (auto& c : m_complaints) {
        if (c.getId() == id) {
            c.setStatus("REJECTED");
            auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            char dtBuf[30];
            std::strftime(dtBuf, sizeof(dtBuf), "%Y-%m-%d %H:%M", std::localtime(&now));
            std::string entry = "[REJECTED - " + std::string(dtBuf) + " | " + adminUser + "] Reason: " + note;
            std::string existing = c.getAdminNotes();
            c.setAdminNotes(existing.empty() ? entry : (existing + " | " + entry));
            m_fileManager.saveComplaints(m_complaints);
            for (auto& p : m_polls) {
                if (p.getComplaintId() == id) {
                    p.setActive(false);
                    m_fileManager.savePolls(m_polls);
                    break;
                }
            }
            return true;
        }
    }
    return false;
}

const Poll* ComplaintManager::findPollForComplaint(const std::string& complaintId) const {
    for (const auto& p : m_polls) {
        if (p.getComplaintId() == complaintId) return &p;
    }
    return nullptr;
}
