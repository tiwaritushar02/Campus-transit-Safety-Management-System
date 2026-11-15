/**
 * ============================================================================
 * MODULE: Member 4 - Complaint, Parent Portal & Student Disciplinary Poll
 * FILE: ComplaintEngine.cpp
 * ============================================================================
 */

#include "ComplaintEngine.hpp"
#include <iomanip>
#include <chrono>
#include <ctime>
#include <sstream>
#include <algorithm>

using namespace std;

ComplaintEngine::ComplaintEngine() {
    // 1. Initial Default Complaint 1 (Master Prompt Requirement)
    Complaint c1;
    c1.complaintId = "CMP-101";
    c1.category = "Rash Driving";
    c1.description = "Bus was over-speeding near Kathgodam curves and overtaking abruptly on mountain corridor.";
    c1.reporterName = "Aarav Sharma";
    c1.reporterRole = "Student";
    c1.complainantRoll = "2024-CS-001";
    c1.accusedName = "Rajesh Kumar";
    c1.accusedRole = "Driver";
    c1.accusedRoll = "D-01";
    c1.busId = "B-101";
    c1.submissionDate = "2026-11-02";
    c1.submissionTime = "08:35 AM";
    c1.status = "UNDER INVESTIGATION";
    c1.adminActionNote = "Assigned for telemetry speed governor verification and operator safety review.";
    c1.isPollActive = false;
    c1.type = ComplaintType::STUDENT_TO_DRIVER;
    c1.targetName = "Rajesh Kumar";
    c1.timestamp = "2026-11-02 08:35 AM";
    complaints.push_back(c1);

    // 2. Initial Default Complaint 2 (Master Prompt Student Misbehavior with Active Poll)
    Complaint c2;
    c2.complaintId = "CMP-102";
    c2.category = "Student Misbehavior";
    c2.description = "Loud altercation, rowdy disruption, and refusal to follow safety instructions during mountain ascent.";
    c2.reporterName = "Rajesh Kumar";
    c2.reporterRole = "Driver";
    c2.complainantRoll = "D-01";
    c2.accusedName = "Rohan Patel";
    c2.accusedRole = "Student";
    c2.accusedRoll = "2024-ME-012";
    c2.busId = "B-101";
    c2.submissionDate = "2026-11-02";
    c2.submissionTime = "09:15 AM";
    c2.status = "POLL ACTIVE";
    c2.adminActionNote = "Student peer disciplinary poll active for eligible bus route commuters.";
    c2.isPollActive = true;
    c2.pollYesVotes = 3;
    c2.pollNoVotes = 1;
    c2.votedStudentRolls = {"2024-EC-045", "2024-CS-089", "2613219", "2613220"};
    c2.type = ComplaintType::DRIVER_TO_STUDENT;
    c2.targetName = "Rohan Patel";
    c2.timestamp = "2026-11-02 09:15 AM";
    complaints.push_back(c2);

    // 3. Initial Default Complaint 3 (Parent Service Feedback)
    Complaint c3;
    c3.complaintId = "CMP-103";
    c3.category = "Schedule Delay Notice";
    c3.description = "Broadcast alert was received 20 minutes late regarding the mountain valley bypass road diversion.";
    c3.reporterName = "Vijay Sharma";
    c3.reporterRole = "Parent";
    c3.complainantRoll = "+91-9811001122";
    c3.accusedName = "Campus Transport Office";
    c3.accusedRole = "Authority";
    c3.accusedRoll = "ADMIN";
    c3.busId = "B-102";
    c3.submissionDate = "2026-11-01";
    c3.submissionTime = "07:50 AM";
    c3.status = "RESOLVED";
    c3.adminActionNote = "SMS gateway automated failover protocol configured for instant delay broadcasting.";
    c3.isPollActive = false;
    c3.type = ComplaintType::PARENT_TO_TRANSPORT;
    c3.targetName = "Campus Transport Office";
    c3.timestamp = "2026-11-01 07:50 AM";
    complaints.push_back(c3);
}

void ComplaintEngine::logComplaint(const Complaint& complaint) {
    complaints.push_back(complaint);
    cout << "🚨 [COMPLAINT LOGGED] ID: " << complaint.complaintId 
         << " | Reporter: " << complaint.reporterName 
         << " | Accused: " << complaint.accusedName 
         << " | Category: " << complaint.category << "\n";
}

string ComplaintEngine::submitComprehensiveComplaint(
    const string& category,
    const string& description,
    const string& reporterName,
    const string& reporterRole,
    const string& accusedName,
    const string& accusedRole,
    const string& busId,
    const string& complainantRoll,
    const string& accusedRoll
) {
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    char dateBuf[32];
    char timeBuf[32];
    strftime(dateBuf, sizeof(dateBuf), "%Y-%m-%d", localtime(&now_c));
    strftime(timeBuf, sizeof(timeBuf), "%I:%M %p", localtime(&now_c));

    string newId = "CMP-" + to_string(complaints.size() + 101);

    Complaint c;
    c.complaintId = newId;
    c.category = category;
    c.description = description;
    c.reporterName = reporterName;
    c.reporterRole = reporterRole;
    c.accusedName = accusedName;
    c.accusedRole = accusedRole;
    c.busId = busId.empty() ? "B-101" : busId;
    c.submissionDate = string(dateBuf);
    c.submissionTime = string(timeBuf);
    c.complainantRoll = complainantRoll;
    c.accusedRoll = accusedRoll;
    c.timestamp = c.submissionDate + " " + c.submissionTime;
    c.targetName = accusedName;

    // Determine Poll Activation:
    // If student vs student OR driver reports student misbehavior, activate Student Disciplinary Poll
    if (accusedRole == "Student" || category == "Student Misbehavior") {
        c.status = "POLL ACTIVE";
        c.isPollActive = true;
        c.pollYesVotes = 0;
        c.pollNoVotes = 0;
        c.adminActionNote = "Peer disciplinary poll initiated for eligible student commuters.";
    } else {
        c.status = "OPEN";
        c.isPollActive = false;
        c.adminActionNote = "Pending transport administration preliminary review.";
    }

    complaints.push_back(c);
    logComplaint(c);
    return newId;
}

bool ComplaintEngine::castStudentPollVote(
    const string& complaintId,
    const string& studentRoll,
    bool voteYes,
    string& outMessage
) {
    Complaint* c = findComplaintById(complaintId);
    if (!c) {
        outMessage = "Complaint record not found.";
        return false;
    }

    if (!c->isPollActive || c->status != "POLL ACTIVE") {
        outMessage = "This complaint poll is no longer active for voting.";
        return false;
    }

    // Rule 1: Complainant cannot vote in their own complaint
    if (!c->complainantRoll.empty() && c->complainantRoll == studentRoll) {
        outMessage = "Access Denied: As the reporting student, you cannot vote in your own complaint.";
        return false;
    }

    // Rule 2: Accused student cannot vote in their own disciplinary poll
    if (!c->accusedRoll.empty() && c->accusedRoll == studentRoll) {
        outMessage = "Access Denied: As the accused student, you cannot vote in your own disciplinary poll.";
        return false;
    }

    // Rule 3: Check for duplicate vote
    auto it = find(c->votedStudentRolls.begin(), c->votedStudentRolls.end(), studentRoll);
    if (it != c->votedStudentRolls.end()) {
        outMessage = "Duplicate Vote Blocked: You have already cast your vote for this complaint poll.";
        return false;
    }

    // Record valid vote
    c->votedStudentRolls.push_back(studentRoll);
    if (voteYes) {
        c->pollYesVotes++;
    } else {
        c->pollNoVotes++;
    }

    outMessage = "Your vote (" + string(voteYes ? "YES" : "NO") + ") has been securely recorded and verified.";
    return true;
}

void ComplaintEngine::investigateComplaint(const string& complaintId) {
    Complaint* c = findComplaintById(complaintId);
    if (c) {
        c->status = "UNDER INVESTIGATION";
        c->adminActionNote = "Official enquiry commenced by Campus Transport Disciplinary Committee.";
    }
}

void ComplaintEngine::resolveComplaint(const string& complaintId, const string& resolutionNote) {
    Complaint* c = findComplaintById(complaintId);
    if (c) {
        c->status = "RESOLVED";
        c->isPollActive = false;
        c->adminActionNote = resolutionNote.empty() ? "Case resolved and closed with remedial directives." : resolutionNote;
    }
}

void ComplaintEngine::rejectComplaint(const string& complaintId, const string& rejectionReason) {
    Complaint* c = findComplaintById(complaintId);
    if (c) {
        c->status = "REJECTED";
        c->isPollActive = false;
        c->adminActionNote = rejectionReason.empty() ? "Complaint dismissed upon evidentiary review." : rejectionReason;
    }
}

Complaint* ComplaintEngine::findComplaintById(const string& complaintId) {
    for (auto& c : complaints) {
        if (c.complaintId == complaintId) return &c;
    }
    return nullptr;
}

void ComplaintEngine::submitDriverRating(const string& driverId, const string& studentRoll, int stars, const string& feedback) {
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", localtime(&now_c));

    driverRatings.push_back({driverId, studentRoll, stars, feedback, string(buf)});
    cout << "⭐ [RATING SUBMITTED] Driver " << driverId << " received " << stars << " stars!\n";
}

void ComplaintEngine::reportLostItem(const string& busId, const string& itemDesc, const string& reportedBy, const string& phone) {
    string itemId = "LOST-" + to_string(lostItems.size() + 101);
    lostItems.push_back({itemId, busId, itemDesc, reportedBy, phone, "REPORTED"});
    cout << "🎒 [LOST & FOUND] Item [" << itemDesc << "] reported on Bus " << busId << " (Ref ID: " << itemId << ")\n";
}

void ComplaintEngine::triggerSOSAlert(const string& senderName, const string& busId, const string& location) {
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    char dateBuf[32];
    char timeBuf[32];
    strftime(dateBuf, sizeof(dateBuf), "%Y-%m-%d", localtime(&now_c));
    strftime(timeBuf, sizeof(timeBuf), "%I:%M:%S %p", localtime(&now_c));

    Complaint sos;
    sos.complaintId = "SOS-" + to_string(complaints.size() + 9001);
    sos.type = ComplaintType::SOS_EMERGENCY;
    sos.reporterName = senderName;
    sos.reporterRole = "Student";
    sos.accusedName = "HIGHWAY EMERGENCY SERVICES";
    sos.accusedRole = "Emergency Service";
    sos.busId = busId;
    sos.category = "EMERGENCY_SOS_BUTTON";
    sos.description = "URGENT SOS Emergency Alert from " + senderName + " on Bus " + busId + " near " + location;
    sos.submissionDate = string(dateBuf);
    sos.submissionTime = string(timeBuf);
    sos.timestamp = sos.submissionDate + " " + sos.submissionTime;
    sos.status = "OPEN";
    sos.adminActionNote = "IMMEDIATE DISPATCH: Security squad alerted to vehicle telemetry location.";
    sos.isPollActive = false;
    sos.targetName = "TRANSPORT_ADMIN_OFFICE";

    complaints.push_back(sos);
    cout << "\033[1;31m🚨 🚨 🚨 [EMERGENCY SOS ALERT ACTIVATED] 🚨 🚨 🚨\033[0m\n";
    cout << "  Location: " << location << " | Bus: " << busId << " | Sender: " << senderName << "\n";
}

void ComplaintEngine::updateComplaintStatus(const string& complaintId, ComplaintStatus newStatus, const string& actionNote) {
    for (auto& c : complaints) {
        if (c.complaintId == complaintId) {
            if (newStatus == ComplaintStatus::INVESTIGATING) c.status = "UNDER INVESTIGATION";
            else if (newStatus == ComplaintStatus::RESOLVED) c.status = "RESOLVED";
            else if (newStatus == ComplaintStatus::CLOSED) c.status = "REJECTED";
            else c.status = "OPEN";
            c.adminActionNote = actionNote;
            return;
        }
    }
}

double ComplaintEngine::getAverageDriverRating(const string& driverId) const {
    double total = 0.0;
    int count = 0;
    for (const auto& r : driverRatings) {
        if (r.driverId == driverId) {
            total += r.ratingStars;
            count++;
        }
    }
    if (count == 0) return 4.5;
    return total / count;
}

void ComplaintEngine::displayAllComplaints() const {
    cout << "\n===========================================================\n";
    cout << "            COLLEGE COMPLAINTS & INCIDENT LOG              \n";
    cout << "===========================================================\n";
    for (const auto& c : complaints) {
        cout << "ID: [" << c.complaintId << "] | Status: " << c.status << " | Category: " << c.category << "\n";
        cout << "  Reporter: " << c.reporterName << " (" << c.reporterRole << ") -> Accused: " 
             << c.accusedName << " (" << c.accusedRole << ")\n";
        cout << "  Timestamp: " << c.submissionDate << " " << c.submissionTime << " | Bus: " << c.busId << "\n";
        cout << "  Description: " << c.description << "\n";
        if (c.isPollActive) {
            cout << "  [STUDENT POLL ACTIVE] YES: " << c.pollYesVotes << " | NO: " << c.pollNoVotes << "\n";
        }
        cout << "  Admin Action Note: " << (c.adminActionNote.empty() ? "Pending" : c.adminActionNote) << "\n";
        cout << "-----------------------------------------------------------\n";
    }
}

void ComplaintEngine::displayLostAndFound() const {
    cout << "\n===========================================================\n";
    cout << "               LOST & FOUND REPOSITORY                     \n";
    cout << "===========================================================\n";
    for (const auto& item : lostItems) {
        cout << "ID: [" << item.itemId << "] | Bus: " << item.busId << " | Status: " << item.status << "\n";
        cout << "  Item: " << item.itemDescription << " | Reported By: " << item.reportedBy << " (" << item.contactPhone << ")\n";
        cout << "-----------------------------------------------------------\n";
    }
}
