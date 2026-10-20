#ifndef NOTIFICATION_CENTER_HPP
#define NOTIFICATION_CENTER_HPP

/**
 * ============================================================================
 * MODULE: Member 3 - Live Tracking, Scheduling & Notification Center
 * FILE: NotificationCenter.hpp
 * 
 * OOP CONCEPTS DEMONSTRATED:
 * 1. OBSERVER / BROADCAST PATTERN: Dispatches push notifications to Students,
 *    Parents, and Drivers.
 * 2. STRUCT ENCAPSULATION: NotificationMessage data model.
 * ============================================================================
 */

#include <string>
#include <vector>
#include <iostream>

using namespace std;

enum class TargetAudience {
    ALL_STUDENTS,
    ALL_PARENTS,
    SPECIFIC_ROUTE,
    SPECIFIC_BUS,
    DRIVERS_ONLY
};

struct NotificationMessage {
    string notificationId;
    TargetAudience audience;
    string targetFilter; // Route ID or Bus ID if applicable
    string title;
    string messageText;
    string timestamp;
};

class NotificationCenter {
private:
    vector<NotificationMessage> notifications;

public:
    NotificationCenter();

    void initializeSampleNotifications();

    void sendBroadcast(TargetAudience audience, const string& title,
                       const string& messageText, const string& targetFilter = "ALL");

    void sendParentDropNotification(const string& parentPhone, const string& childName, const string& eventType, const string& timeStr);

    const vector<NotificationMessage>& getNotifications() const { return notifications; }
    void displayNotificationLog() const;
};

#endif // NOTIFICATION_CENTER_HPP
