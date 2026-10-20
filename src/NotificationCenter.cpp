/**
 * ============================================================================
 * MODULE: Member 3 - Live Tracking, Scheduling & Notification Center
 * FILE: NotificationCenter.cpp
 * ============================================================================
 */

#include "NotificationCenter.hpp"
#include <iomanip>
#include <chrono>
#include <ctime>

using namespace std;

NotificationCenter::NotificationCenter() {
    initializeSampleNotifications();
}

void NotificationCenter::initializeSampleNotifications() {
    notifications = {
        {"NOTIF-101", TargetAudience::ALL_STUDENTS, "ALL", "🗓️ Schedule Notice", "College will follow Half-Day Schedule on Friday for Mid-Term Exams.", "2026-08-01 09:00"},
        {"NOTIF-102", TargetAudience::SPECIFIC_BUS, "B-101", "⚠️ Delay Alert", "Bus B-101 is delayed by 15 minutes due to heavy traffic on Expressway.", "2026-08-02 07:40"}
    };
}

void NotificationCenter::sendBroadcast(TargetAudience audience, const string& title,
                                       const string& messageText, const string& targetFilter) {
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", localtime(&now_c));

    string id = "NOTIF-" + to_string(notifications.size() + 101);
    notifications.push_back({id, audience, targetFilter, title, messageText, string(buf)});

    cout << "📢 [BROADCAST DISPATCHED] Title: " << title << " | Target: " << targetFilter << "\n";
}

void NotificationCenter::sendParentDropNotification(const string& parentPhone, const string& childName, const string& eventType, const string& timeStr) {
    string msg = "SMS/App Notification to Parent (" + parentPhone + "): 'Your child " + childName + " " + eventType + " at " + timeStr + ".'";
    sendBroadcast(TargetAudience::ALL_PARENTS, "👨‍👩‍👧 Parent Alert", msg, childName);
}

void NotificationCenter::displayNotificationLog() const {
    cout << "\n===========================================================\n";
    cout << "             SYSTEM BROADCAST & NOTIFICATION CENTER        \n";
    cout << "===========================================================\n";
    for (const auto& n : notifications) {
        cout << "ID: [" << n.notificationId << "] | Date: " << n.timestamp << "\n";
        cout << "  Title: " << n.title << " (Filter: " << n.targetFilter << ")\n";
        cout << "  Message: " << n.messageText << "\n";
        cout << "-----------------------------------------------------------\n";
    }
    cout << "===========================================================\n\n";
}
