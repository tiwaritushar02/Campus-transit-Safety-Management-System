#ifndef STOP_HPP
#define STOP_HPP

/**
 * ============================================================================
 * MODULE: Member 1 - Student & Route Management
 * FILE: Stop.hpp
 * ============================================================================
 */

#include "ScheduleTypes.hpp"
#include <string>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

class Stop {
private:
    string stopId;
    string stopName;
    vector<string> passingBusIds;
    
    // Schedule-aware arrival times: map<busId, HH:MM time string>
    map<string, string> fullDayArrivalTimes;
    map<string, string> halfDayArrivalTimes;

public:
    Stop() = default;
    Stop(const string& sId, const string& sName);

    // Getters & Setters
    string getStopId() const { return stopId; }
    string getStopName() const { return stopName; }
    const vector<string>& getPassingBusIds() const { return passingBusIds; }

    void addPassingBus(const string& busId, const string& fullDayTime, const string& halfDayTime);
    
    // Admin Update Methods
    void updateArrivalTime(const string& busId, ScheduleType scheduleType, const string& newTime);

    // Get scheduled arrival time based on active ScheduleType (HALF_DAY vs FULL_DAY)
    string getArrivalTime(const string& busId, ScheduleType scheduleType) const;
    int getArrivalTimeInMinutes(const string& busId, ScheduleType scheduleType) const;

    void displayStopInfo(ScheduleType activeSchedule) const;
};

#endif // STOP_HPP
