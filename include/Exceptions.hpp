#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

/**
 * ============================================================================
 * MODULE: Member 4 - Connecting Bus Logic, Attendance & File Handling
 * FILE: Exceptions.hpp
 * 
 * OOP CONCEPTS DEMONSTRATED:
 * - Exception Handling (Pillar): Custom exception hierarchy inheriting from
 *   exception. Polymorphic exception handling via what() method.
 * ============================================================================
 */

#include <exception>
#include <string>

using namespace std;

// Base Custom Exception Class for the Bus Management System
class BusSystemException : public exception {
protected:
    string message;

public:
    explicit BusSystemException(const string& msg) : message(msg) {}

    // Overridden what() function from exception (Polymorphism in Exceptions)
    const char* what() const noexcept override {
        return message.c_str();
    }
};

// Exception thrown when a bus reaches maximum seating capacity
class SeatFullException : public BusSystemException {
public:
    explicit SeatFullException(const string& busId, int maxCapacity)
        : BusSystemException("Seat Full Error: Bus [" + busId + "] is at full capacity (" + 
                             to_string(maxCapacity) + " seats). Boarding denied!") {}
};

// Exception thrown when a requested route or stop is not found
class RouteNotFoundException : public BusSystemException {
public:
    explicit RouteNotFoundException(const string& routeOrStopId)
        : BusSystemException("Route/Stop Error: Route or Stop ID [" + routeOrStopId + "] does not exist in the system.") {}
};

// Exception thrown when no connecting bus is available for a missed bus
class NoConnectingBusException : public BusSystemException {
public:
    explicit NoConnectingBusException(const string& stopName, const string& timeStr)
        : BusSystemException("Search Result: No connecting bus found passing stop [" + stopName + 
                             "] after " + timeStr + ". Please contact emergency transport admin.") {}
};

// Exception thrown for invalid student credentials or missing fee status
class InvalidUserException : public BusSystemException {
public:
    explicit InvalidUserException(const string& details)
        : BusSystemException("User Error: " + details) {}
};

// Exception thrown during file reading or writing failure
class FileIOException : public BusSystemException {
public:
    explicit FileIOException(const string& filename, const string& reason)
        : BusSystemException("File I/O Error: Could not access file [" + filename + "]. Reason: " + reason) {}
};

#endif // EXCEPTIONS_HPP
