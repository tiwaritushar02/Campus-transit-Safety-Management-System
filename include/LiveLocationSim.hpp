#ifndef LIVE_LOCATION_SIM_HPP
#define LIVE_LOCATION_SIM_HPP

/**
 * ============================================================================
 * MODULE: Member 3 - Scheduling, ETA & Route Optimization
 * FILE: LiveLocationSim.hpp
 * 
 * OOP CONCEPTS DEMONSTRATED:
 * - Thread & Timer Abstraction: Simulates real-time GPS movement of buses
 *   across route stops using `<chrono>` and `<thread>` in C++11/17.
 * ============================================================================
 */

#include "Bus.hpp"
#include "Route.hpp"
#include <chrono>
#include <thread>
#include <iostream>

using namespace std;

class LiveLocationSim {
public:
    // Simulates live bus movement step-by-step across all stops on a route
    static void simulateBusTrip(Bus& bus, const Route& route, int delayPerStopSeconds = 1, bool verbose = true) {
        const auto& stops = route.getStops();
        if (stops.empty()) return;

        if (verbose) {
            cout << "\n===========================================================\n";
            cout << "  🚀 SIMULATION STARTED: Bus [" << bus.getBusId() << "] on Route [" << route.getRouteName() << "]\n";
            cout << "===========================================================\n";
        }

        for (size_t i = 0; i < stops.size(); ++i) {
            bus.setCurrentLocationStopId(stops[i].getStopId());

            if (verbose) {
                cout << "  ⏱️  [LIVE GPS] Bus " << bus.getBusId() 
                          << " arrived at Stop (" << (i + 1) << "/" << stops.size() << "): "
                          << stops[i].getStopName() << " [ID: " << stops[i].getStopId() << "]\n";
            }

            // Simulate movement delay between stops
            if (delayPerStopSeconds > 0 && i + 1 < stops.size()) {
                this_thread::sleep_for(chrono::seconds(delayPerStopSeconds));
            }
        }

        if (verbose) {
            cout << "  ✅ SIMULATION COMPLETE: Bus [" << bus.getBusId() << "] reached final destination!\n";
            cout << "===========================================================\n\n";
        }
    }
};

#endif // LIVE_LOCATION_SIM_HPP
