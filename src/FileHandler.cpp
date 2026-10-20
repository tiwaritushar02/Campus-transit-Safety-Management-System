/**
 * ============================================================================
 * MODULE: Member 4 - Connecting Bus Logic, Attendance & File Handling
 * FILE: FileHandler.cpp
 * ============================================================================
 */

#include "FileHandler.hpp"
#include "Exceptions.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

using namespace std;

void FileHandler::saveStudents(const vector<Student>& students, const string& filename) {
    try {
        filesystem::path filePath(filename);
        if (filePath.has_parent_path()) {
            filesystem::create_directories(filePath.parent_path());
        }
    } catch (...) {}

    ofstream outFile(filename, ios::trunc);
    if (!outFile.is_open()) {
        cerr << "Warning: Could not open file " << filename << " for writing. Continuing in-memory.\n";
        return;
    }

    for (const auto& s : students) {
        outFile << s.getId() << ","
                << s.getName() << ","
                << s.getPhone() << ","
                << s.getEmail() << ","
                << s.getRollNumber() << ","
                << s.getAssignedRouteId() << ","
                << s.getAssignedStopId() << ","
                << (s.isFeePaid() ? "1" : "0") << "\n";
    }
}

vector<Student> FileHandler::loadStudents(const string& filename) {
    vector<Student> students;
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        return students; // Return empty if file does not exist yet
    }

    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string id, name, phone, email, roll, route, stop, feeStr;

        if (getline(ss, id, ',') &&
            getline(ss, name, ',') &&
            getline(ss, phone, ',') &&
            getline(ss, email, ',') &&
            getline(ss, roll, ',') &&
            getline(ss, route, ',') &&
            getline(ss, stop, ',') &&
            getline(ss, feeStr)) {
            
            bool fee = (feeStr == "1" || feeStr == "true");
            students.emplace_back(id, name, phone, email, roll, route, stop, fee);
        }
    }
    return students;
}

void FileHandler::saveBuses(const vector<Bus>& buses, const string& filename) {
    try {
        filesystem::path filePath(filename);
        if (filePath.has_parent_path()) {
            filesystem::create_directories(filePath.parent_path());
        }
    } catch (...) {}

    ofstream outFile(filename, ios::trunc);
    if (!outFile.is_open()) {
        cerr << "Warning: Could not open file " << filename << " for writing. Continuing in-memory.\n";
        return;
    }

    for (const auto& b : buses) {
        outFile << b.getBusId() << ","
                << b.getBusNumber() << ","
                << b.getCapacity() << ","
                << b.getAssignedRouteId() << "\n";
    }
}

void FileHandler::initializeSampleData() {
    try {
        filesystem::create_directories("data");
    } catch (...) {}

    ifstream testFile("data/students.csv");
    if (!testFile.is_open()) {
        vector<Student> samples = {
            Student("S101", "Aarav Sharma", "+91-9811223344", "aarav@college.edu", "2024-CS-001", "R1", "ST101", true),
            Student("S102", "Priya Verma", "+91-9822334455", "priya@college.edu", "2024-EC-045", "R1", "ST102", true),
            Student("S103", "Rohan Patel", "+91-9833445566", "rohan@college.edu", "2024-ME-012", "R2", "ST103", false),
            Student("S104", "Ananya Iyer", "+91-9844556677", "ananya@college.edu", "2024-CS-089", "R2", "ST104", true)
        };
        saveStudents(samples);
    }
}

