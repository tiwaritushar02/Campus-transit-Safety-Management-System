/**
 * ============================================================================
 * MODULE: Member 4 - Complaint, Parent Portal & File Handling
 * FILE: ComplaintEngine.cpp
 * ============================================================================
 */

#include "ComplaintEngine.hpp"
#include <iomanip>
#include <chrono>
#include <ctime>

using namespace std;

void ComplaintEngine::logComplaint(const Complaint& complaint) {
    complaints.push_back(complaint);
    cout << "🚨 [COMPLAINT LOGGED] ID: " << complaint.complaintId 
              << " | Reporter: " << complaint.reporterName 
              << " | Category: " << complaint.category << "\n";
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
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now_c));

    Complaint sos;
    sos.complaintId = "SOS-" + to_string(complaints.size() + 9001);
    sos.type = ComplaintType::SOS_EMERGENCY;
    sos.reporterName = senderName;
    sos.targetName = "TRANSPORT_ADMIN_OFFICE";
    sos.busId = busId;
    sos.category = "EMERGENCY_SOS_BUTTON";
    sos.description = "URGENT SOS Emergency Alert from " + senderName + " on Bus " + busId + " near " + location;
    sos.timestamp = string(buf);
    sos.status = ComplaintStatus::OPEN;
    sos.adminActionNote = "DISPATCHING EMERGENCY ASSISTANCE";

    complaints.push_back(sos);
    cout << "\033[1;31m🚨 🚨 🚨 [EMERGENCY SOS ALERT ACTIVATED] 🚨 🚨 🚨\033[0m\n";
    cout << "  Location: " << location << " | Bus: " << busId << " | Sender: " << senderName << "\n";
}

void ComplaintEngine::updateComplaintStatus(const string& complaintId, ComplaintStatus newStatus, const string& actionNote) {
    for (auto& c : complaints) {
        if (c.complaintId == complaintId) {
            c.status = newStatus;
            c.adminActionNote = actionNote;
            cout << "✅ Complaint [" << complaintId << "] updated by Admin to Status Code: " << static_cast<int>(newStatus) << "\n";
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
    if (count == 0) return 4.5; // Default initial rating
    return total / count;
}

void ComplaintEngine::displayAllComplaints() const {
    cout << "\n===========================================================\n";
    cout << "            COLLEGE COMPLAINTS & INCIDENT LOG              \n";
    cout << "===========================================================\n";
    if (complaints.empty()) {
        cout << "  [No complaints currently logged]\n";
    } else {
        for (const auto& c : complaints) {
            cout << "ID: [" << c.complaintId << "] | Type: " 
                      << (c.type == ComplaintType::SOS_EMERGENCY ? "🚨 SOS EMERGENCY" : "STANDARD")
                      << " | Category: " << left << setw(20) << c.category << "\n";
            cout << "  Reporter: " << c.reporterName << " -> Target: " << c.targetName << "\n";
            cout << "  Description: " << c.description << "\n";
            cout << "  Admin Action Note: " << (c.adminActionNote.empty() ? "Pending Investigation" : c.adminActionNote) << "\n";
            cout << "-----------------------------------------------------------\n";
        }
    }
    cout << "===========================================================\n\n";
}

void ComplaintEngine::displayLostAndFound() const {
    cout << "\n===========================================================\n";
    cout << "              LOST & FOUND RECOVERED ITEMS                 \n";
    cout << "===========================================================\n";
    if (lostItems.empty()) {
        cout << "  [No lost items currently reported]\n";
    } else {
        for (const auto& item : lostItems) {
            cout << "Item Ref: [" << item.itemId << "] Bus: " << item.busId << " | Item: " << item.itemDescription
                      << " | Reported By: " << item.reportedBy << " (📞 " << item.contactPhone << ") | Status: " << item.status << "\n";
        }
    }
    cout << "===========================================================\n\n";
}
