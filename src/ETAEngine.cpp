/**
 * ============================================================================
 * MODULE: Member 3 - Scheduling, ETA & Route Optimization
 * FILE: ETAEngine.cpp
 * ============================================================================
 */

#include "ETAEngine.hpp"
#include <cmath>

using namespace std;

// Variant 1: Basic ETA based on distance
int ETAEngine::calculateETA(const Route& route, const string& currentStopId, const string& targetStopId) {
    int startIdx = route.getStopIndex(currentStopId);
    int targetIdx = route.getStopIndex(targetStopId);

    if (startIdx == -1 || targetIdx == -1 || startIdx > targetIdx) {
        return -1; // Target stop invalid or already passed
    }

    const auto& stops = route.getStops();
    double totalDistance = 0.0;
    for (int i = startIdx; i < targetIdx; ++i) {
        totalDistance += route.getSegmentDistance(stops[i].getStopId(), stops[i + 1].getStopId());
    }

    // Average bus speed in city: 30 km/h (0.5 km per min) + 1 min dwell time per stop
    double speedKmPerMin = 0.5;
    int dwellTime = (targetIdx - startIdx) * 1; 
    int travelMinutes = static_cast<int>(round(totalDistance / speedKmPerMin)) + dwellTime;

    return travelMinutes;
}

// Variant 2: Advanced ETA considering traffic factor
int ETAEngine::calculateETA(const Route& route, const string& currentStopId, const string& targetStopId,
                            ScheduleType scheduleType, double trafficFactor) {
    int baseMinutes = calculateETA(route, currentStopId, targetStopId);
    if (baseMinutes == -1) return -1;

    // Apply schedule multiplier: Half-Day usually has lighter/heavier peak hour traffic
    double scheduleMultiplier = (scheduleType == ScheduleType::HALF_DAY) ? 0.9 : 1.0;
    
    return static_cast<int>(round(baseMinutes * trafficFactor * scheduleMultiplier));
}

// Variant 3: Schedule-based exact arrival time
string ETAEngine::calculateScheduledETA(const Stop& stop, const string& busId, ScheduleType scheduleType) {
    return stop.getArrivalTime(busId, scheduleType);
}
