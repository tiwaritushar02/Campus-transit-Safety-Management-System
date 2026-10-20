#ifndef SCHEDULE_TYPES_HPP
#define SCHEDULE_TYPES_HPP

/**
 * ============================================================================
 * MODULE: Member 3 - Scheduling, ETA & Route Optimization
 * FILE: ScheduleTypes.hpp
 * 
 * OOP CONCEPT DEMONSTRATED:
 * - Enumeration & Helper Abstraction: Provides clear type-safe schedules for
 *   Half-Day vs Full-Day college timings.
 * ============================================================================
 */

#include <string>
#include <iostream>
#include <cstdio>

using namespace std;

// Enum representing the college operational day type
enum class ScheduleType {
    FULL_DAY,
    HALF_DAY
};

// Helper utilities for schedule conversions
class ScheduleUtils {
public:
    // Convert ScheduleType enum to string description
    static string toString(ScheduleType type) {
        switch (type) {
            case ScheduleType::FULL_DAY:
                return "Full-Day Schedule (Normal College Hours)";
            case ScheduleType::HALF_DAY:
                return "Half-Day Schedule (Early Departure / Exam Hours)";
            default:
                return "Unknown Schedule";
        }
    }

    // Convert minutes from midnight (e.g. 480 -> 08:00 AM) to HH:MM format string
    static string minutesToTimeString(int minutesFromMidnight) {
        int hours = (minutesFromMidnight / 60) % 24;
        int mins = minutesFromMidnight % 60;
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%02d:%02d", hours, mins);
        return string(buffer);
    }

    // Convert HH:MM format string (e.g. "08:30") to minutes from midnight
    static int timeStringToMinutes(const string& timeStr) {
        int hours = 0, mins = 0;
        if (sscanf(timeStr.c_str(), "%d:%d", &hours, &mins) == 2) {
            return hours * 60 + mins;
        }
        return 0;
    }
};

#endif // SCHEDULE_TYPES_HPP
