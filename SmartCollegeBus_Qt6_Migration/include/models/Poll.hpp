#ifndef POLL_HPP
#define POLL_HPP

#include <string>
#include <vector>
#include <algorithm>

// Encapsulation
class Poll {
private:
    std::string m_pollId;
    std::string m_complaintId;
    std::string m_question;
    bool m_active;
    int m_totalVotes;
    int m_yesVotes;
    int m_noVotes;
    std::string m_complainantRoll;
    std::string m_accusedRoll;
    std::vector<std::string> m_votedRolls;

public:
    // Constructor
    Poll(const std::string& pId = "", const std::string& cId = "",
         const std::string& q = "Do you agree that this incident requires disciplinary action?",
         bool act = true, int total = 0, int yes = 0, int no = 0,
         const std::string& compRoll = "", const std::string& accRoll = "")
        : m_pollId(pId), m_complaintId(cId), m_question(q),
          m_active(act), m_totalVotes(total), m_yesVotes(yes),
          m_noVotes(no), m_complainantRoll(compRoll), m_accusedRoll(accRoll) {}

    // Encapsulation
    std::string getPollId() const { return m_pollId; }
    std::string getComplaintId() const { return m_complaintId; }
    std::string getQuestion() const { return m_question; }
    bool isActive() const { return m_active; }
    int getTotalVotes() const { return m_totalVotes; }
    int getYesVotes() const { return m_yesVotes; }
    int getNoVotes() const { return m_noVotes; }
    int getYesPercentage() const { return m_totalVotes > 0 ? (m_yesVotes * 100 / m_totalVotes) : 0; }
    int getNoPercentage() const { return m_totalVotes > 0 ? (m_noVotes * 100 / m_totalVotes) : 0; }
    std::string getComplainantRoll() const { return m_complainantRoll; }
    std::string getAccusedRoll() const { return m_accusedRoll; }

    bool hasVoted(const std::string& roll) const {
        return std::find(m_votedRolls.begin(), m_votedRolls.end(), roll) != m_votedRolls.end();
    }

    void addVotedRoll(const std::string& roll) {
        if (!hasVoted(roll)) m_votedRolls.push_back(roll);
    }

    bool castVote(const std::string& roll, bool isYes) {
        if (hasVoted(roll)) return false;
        m_votedRolls.push_back(roll);
        m_totalVotes++;
        if (isYes) m_yesVotes++;
        else m_noVotes++;
        return true;
    }

    void setActive(bool a) { m_active = a; }
};

#endif // POLL_HPP
