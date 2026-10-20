#ifndef COMPLAINT_ENGINE_HPP
#define COMPLAINT_ENGINE_HPP

/**
 * ============================================================================
 * MODULE: Member 4 - Complaint, Parent Portal & File Handling
 * FILE: ComplaintEngine.hpp
 * 
 * OOP CONCEPTS DEMONSTRATED:
 * 1. ENCAPSULATION & STRUCT DATA MODELING: Complaint, DriverRating, LostItem structs.
 * 2. COMPLAINT WORKFLOW ENGINE: Manages student/driver/parent complaints, SOS alerts,
 *    and driver ratings.
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
    string complaintId;
    ComplaintType type;
    string reporterName; // Student/Driver/Parent name
    string targetName;   // Driver/Student name
    string busId;
    string category;     // e.g. "Rash Driving", "Vandalism", "Over-speeding"
    string description;
    string timestamp;
    ComplaintStatus status;
    string adminActionNote;
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
    ComplaintEngine() = default;

    // File/Log Methods
    void logComplaint(const Complaint& complaint);
    void submitDriverRating(const string& driverId, const string& studentRoll, int stars, const string& feedback);
    void reportLostItem(const string& busId, const string& itemDesc, const string& reportedBy, const string& phone);
    void triggerSOSAlert(const string& senderName, const string& busId, const string& location);

    // Admin Action Methods
    void updateComplaintStatus(const string& complaintId, ComplaintStatus newStatus, const string& actionNote);

    // Getters & Analytics
    const vector<Complaint>& getComplaints() const { return complaints; }
    const vector<DriverRating>& getRatings() const { return driverRatings; }
    const vector<LostItem>& getLostItems() const { return lostItems; }

    double getAverageDriverRating(const string& driverId) const;
    void displayAllComplaints() const;
    void displayLostAndFound() const;
};

#endif // COMPLAINT_ENGINE_HPP
