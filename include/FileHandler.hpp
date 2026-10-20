#ifndef FILE_HANDLER_HPP
#define FILE_HANDLER_HPP

/**
 * ============================================================================
 * MODULE: Member 4 - Connecting Bus Logic, Attendance & File Handling
 * FILE: FileHandler.hpp
 * 
 * OOP CONCEPTS DEMONSTRATED:
 * 1. FILE PERSISTENCE (Pillar Requirement): Reading and writing data using
 *    ifstream and ofstream so project data survives restarts.
 * 2. EXCEPTION HANDLING: Catches file open/parse errors and throws FileIOException.
 * ============================================================================
 */

#include "Student.hpp"
#include "Bus.hpp"
#include "Route.hpp"
#include "Driver.hpp"
#include "Conductor.hpp"
#include <string>
#include <vector>

using namespace std;

class FileHandler {
public:
    // Student CSV persistence
    static void saveStudents(const vector<Student>& students, const string& filename = "data/students.csv");
    static vector<Student> loadStudents(const string& filename = "data/students.csv");

    // Bus CSV persistence
    static void saveBuses(const vector<Bus>& buses, const string& filename = "data/buses.csv");

    // Sample data initializer if files don't exist yet
    static void initializeSampleData();
};

#endif // FILE_HANDLER_HPP
