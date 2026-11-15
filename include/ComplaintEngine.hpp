#ifndef COMPLAINT_ENGINE_HPP
#define COMPLAINT_ENGINE_HPP

/**
 * ============================================================================
 * MODULE: Member 4 - Complaint, Parent Portal & Student Disciplinary Poll
 * FILE: ComplaintEngine.hpp
 * 
 * OOP CONCEPTS DEMONSTRATED:
 * 1. ENCAPSULATION & STRUCT DATA MODELING: Complaint, DriverRating, LostItem structs.
 * 2. COMPLAINT WORKFLOW ENGINE: Manages student/driver/parent complaints, SOS alerts,
 *    driver ratings, and student disciplinary voting polls with duplicate protection.
 * ============================================================================
 */

#include <string>
#include <vector>
#include <iostream>

using namespace std;

enum class ComplaintType {
    STUDENT_TO_DRIVER,
    DRIVER_TO_STUDENT,
    PARENT_TO_TRANSPORT,
    SOS_EMERGENCY
};

enum class ComplaintStatus {
    OPEN,
    INVESTIGATING,
    WARNING_ISSUED,
    RESOLVED,
    CLOSED
};

struct Complaint {
    string complaintId;          // 1. Complaint ID (e.g. "CMP-101")
    string category;             // 2. Complaint Category (e.g. "Rash Driving", "Student Misbehavior")
    string description;          // 3. Complaint Description
    string reporterName;         // 4. Name of person who submitted complaint
    string reporterRole;         // 5. Role of complainant (Student / Driver / Parent)
    string accusedName;          // 6. Name of person against whom complaint was submitted
    string accusedRole;          // 7. Role of accused person (Driver / Student / Authority)
    string busId;                // 8. Bus ID (e.g. "B-101")
    string submissionDate;       // 9. Complaint submission date (e.g. "2026-11-02")
    string submissionTime;       // 10. Exact complaint submission time (e.g. "08:35 AM")
    string status;               // 11. Status ("OPEN", "UNDER INVESTIGATION", "POLL ACTIVE", "RESOLVED", "REJECTED")
    string adminActionNote;      // Action note or recorded rejection reason
    
    // Student Peer Voting / Disciplinary Poll System
    bool isPollActive = false;
    int pollYesVotes = 0;
    int pollNoVotes = 0;
    vector<string> votedStudentRolls;
    string complainantRoll;
    string accusedRoll;

    // Backwards-compatible legacy fields
    ComplaintType type = ComplaintType::STUDENT_TO_DRIVER;
    string targetName;
    string timestamp;
};

struct DriverRating {
    string driverId;
    string studentRoll;
    int ratingStars; // 1 to 5
    string feedbackText;
    string timestamp;
};

struct LostItem {
    string itemId;
    string busId;
    string itemDescription;
    string reportedBy;
    string contactPhone;
    string status; // "REPORTED", "RECOVERED", "HANDED_OVER"
};

class ComplaintEngine {
private:
    vector<Complaint> complaints;
    vector<DriverRating> driverRatings;
    vector<LostItem> lostItems;

public:
    ComplaintEngine();

    // Core Complaint Workflow Methods
    void logComplaint(const Complaint& complaint);
    void submitDriverRating(const string& driverId, const string& studentRoll, int stars, const string& feedback);
    void reportLostItem(const string& busId, const string& itemDesc, const string& reportedBy, const string& phone);
    void triggerSOSAlert(const string& senderName, const string& busId, const string& location);

    // Advanced Comprehensive Complaint Creation
    string submitComprehensiveComplaint(
        const string& category,
        const string& description,
        const string& reporterName,
        const string& reporterRole,
        const string& accusedName,
        const string& accusedRole,
        const string& busId,
        const string& complainantRoll = "",
        const string& accusedRoll = ""
    );

    // Student Poll / Disciplinary Voting System
    bool castStudentPollVote(const string& complaintId, const string& studentRoll, bool voteYes, string& outMessage);

    // Admin Action Methods
    void updateComplaintStatus(const string& complaintId, ComplaintStatus newStatus, const string& actionNote);
    void investigateComplaint(const string& complaintId);
    void resolveComplaint(const string& complaintId, const string& resolutionNote);
    void rejectComplaint(const string& complaintId, const string& rejectionReason);

    // Getters & Analytics
    const vector<Complaint>& getComplaints() const { return complaints; }
    vector<Complaint>& getComplaintsMutable() { return complaints; }
    const vector<DriverRating>& getRatings() const { return driverRatings; }
    const vector<LostItem>& getLostItems() const { return lostItems; }

    Complaint* findComplaintById(const string& complaintId);

    double getAverageDriverRating(const string& driverId) const;
    void displayAllComplaints() const;
    void displayLostAndFound() const;
};

#endif // COMPLAINT_ENGINE_HPP
