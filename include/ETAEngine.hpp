#ifndef ETA_ENGINE_HPP
#define ETA_ENGINE_HPP

/**
 * ============================================================================
 * MODULE: Member 3 - Scheduling, ETA & Route Optimization
 * FILE: ETAEngine.hpp
 * 
 * OOP CONCEPTS DEMONSTRATED:
 * 1. POLYMORPHISM (Pillar - Compile-time / Static): Function Overloading
 *    for `calculateETA()` with different signature parameter sets.
 * 2. ALGORITHMIC CALCULATION: Distance-based ETA computation considering speed
 *    and schedule-specific arrival times.
 * ============================================================================
 */

#include "Route.hpp"
#include "Bus.hpp"
#include "ScheduleTypes.hpp"
#include <string>

using namespace std;

class ETAEngine {
public:
    // Variant 1 (Function Overloading): Basic ETA calculation based on distance and average speed (30 km/h)
    static int calculateETA(const Route& route, const string& currentStopId, const string& targetStopId);

    // Variant 2 (Function Overloading): Advanced ETA calculation considering traffic factor and active ScheduleType
    static int calculateETA(const Route& route, const string& currentStopId, const string& targetStopId,
                            ScheduleType scheduleType, double trafficFactor);

    // Variant 3 (Function Overloading): Schedule-based exact arrival time difference
    static string calculateScheduledETA(const Stop& stop, const string& busId, ScheduleType scheduleType);
};

#endif // ETA_ENGINE_HPP
