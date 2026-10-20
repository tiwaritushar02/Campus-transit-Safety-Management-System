/**
 * ============================================================================
 * MODULE: Member 1 - Student & Route Management
 * FILE: Stop.cpp
 * ============================================================================
 */

#include "Stop.hpp"
#include <iomanip>

using namespace std;

Stop::Stop(const string& sId, const string& sName)
    : stopId(sId), stopName(sName) {}

void Stop::addPassingBus(const string& busId, const string& fullDayTime, const string& halfDayTime) {
    passingBusIds.push_back(busId);
    fullDayArrivalTimes[busId] = fullDayTime;
    halfDayArrivalTimes[busId] = halfDayTime;
}

void Stop::updateArrivalTime(const string& busId, ScheduleType scheduleType, const string& newTime) {
    if (scheduleType == ScheduleType::FULL_DAY) {
        fullDayArrivalTimes[busId] = newTime;
    } else {
        halfDayArrivalTimes[busId] = newTime;
    }
}

string Stop::getArrivalTime(const string& busId, ScheduleType scheduleType) const {
    if (scheduleType == ScheduleType::FULL_DAY) {
        auto it = fullDayArrivalTimes.find(busId);
        if (it != fullDayArrivalTimes.end()) return it->second;
    } else {
        auto it = halfDayArrivalTimes.find(busId);
        if (it != halfDayArrivalTimes.end()) return it->second;
    }
    return "N/A";
}

int Stop::getArrivalTimeInMinutes(const string& busId, ScheduleType scheduleType) const {
    string timeStr = getArrivalTime(busId, scheduleType);
    if (timeStr == "N/A") return -1;
    return ScheduleUtils::timeStringToMinutes(timeStr);
}

void Stop::displayStopInfo(ScheduleType activeSchedule) const {
    cout << "Stop ID: [" << stopId << "] Name: " << stopName << "\n";
    cout << "  Schedule Mode: " << ScheduleUtils::toString(activeSchedule) << "\n";
    cout << "  Buses passing this stop:\n";
    for (const auto& busId : passingBusIds) {
        cout << "   - Bus [" << busId << "] Arrival Time: " 
                  << getArrivalTime(busId, activeSchedule) << "\n";
    }
}
