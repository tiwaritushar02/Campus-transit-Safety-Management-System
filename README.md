# Smart College Bus Management & Campus Transit Safety System

[![C++17](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Qt 6](https://img.shields.io/badge/Framework-Qt%206-green.svg)](https://www.qt.io/)
[![Architecture](https://img.shields.io/badge/Architecture-4%20Pillars%20OOP-orange.svg)]()
[![Campus](https://img.shields.io/badge/University-Graphic%20Era%20Hill%20University-crimson.svg)]()

A high-performance, native desktop application developed in **C++17 and Qt 6** for **Graphic Era Hill University (Bhimtal Campus)**. The system provides real-time GPS fleet telemetry, mountain corridor transit tracking, student attendance logging, a peer disciplinary incident voting system, and pathfinding route optimization.

---

## 🌟 Key Capabilities & Features

### 1. Multi-Role Authentication & Access Control
- **Administrator**: Complete fleet overview, trip scheduling, driver assignment, incident investigation, and resolution directives.
- **Student / Commuter**: Assigned bus route tracking, morning stop departure times, digital QR boarding pass, and confidential peer disciplinary poll voting.
- **Driver**: Assigned vehicle manifest, route stop checkpoints, mountain road advisories, and live trip status broadcasting.
- **Parent / Guardian**: Ward transit tracking, estimated time of arrival (ETA), and driver contact directory.

### 2. Native Embedded OpenStreetMap & Simulation Engine
- Embedded directly inside the Qt desktop window using native painting and slippy tile rendering (**no external web browser or WebEngine dependencies**).
- Real geographic topographic corridor: **Haldwani Bus Terminal ➔ Kathgodam Railway Station ➔ Ranibagh Junction ➔ Bhujiaghat Mountain Pass ➔ Bhimtal Lake ➔ Graphic Era Hill University (1,420m ASL)**.
- Real-time vehicle motion simulation with live GPS telemetry HUD, altitude profiles, speed indicators, and checkpoint triggers.

### 3. Route Optimization & Search Algorithms
- Real graph representation of mountain routes with weighted edge metrics (distance, elevation delta, road grade).
- **A\* Pathfinding Algorithm** with Euclidean/Haversine heuristic estimation.
- **Dijkstra's Algorithm** for shortest path baseline comparison.

### 4. Fleet Safety & Peer Disciplinary Voting System
- 11-field incident tracking (Complaint ID, Category, Reporter, Accused, Bus ID, Date, Time, Status, Admin History).
- **Confidential Student Peer Voting**: Automatically initiates a YES/NO poll for student incidents with strict eligibility verification (involved parties restricted, single vote per student roll number, real-time vote percentage tallies).
- **Investigation & Resolution History**: Permanently logs timestamped inquiry entries and administrative directives with Admin identity.

### 5. Flat-File Persistence (No SQL Database)
- Robust, portable flat-file storage using CSV and structured text (`data/*.csv`, `data/users.txt`).
- Automated thread-safe loading, validation, and data persistence.

---

## 🏛️ Demonstration of the Four OOP Pillars

1. **Encapsulation**:
   - Class state (coordinates, user credentials, bus telemetry, votes) encapsulated with private/protected member access and validated getters/setters across `User`, `Bus`, `Stop`, `Route`, `Complaint`, `Poll`, etc.
2. **Inheritance**:
   - Base `User` class inherited by `Admin`, `Student`, `Driver`, and `Parent`.
   - Abstract `RoutingAlgorithm` inherited by `AStarRoute` and `DijkstraRoute`.
   - `QWidget` base class extended by specialized modular widgets (`MainWindow`, `LoginWidget`, `ComplaintDeskWidget`, `NativeOsmMapWidget`).
3. **Polymorphism**:
   - Dynamic polymorphism via pure virtual methods (`getRoleString()`, `calculateShortestPath()`).
   - Virtual method overriding (`QWidget::paintEvent`, `QWidget::resizeEvent`).
   - Function and constructor overloading across service layers.
4. **Abstraction**:
   - Clean separation of interface and implementation through dedicated headers in `include/` and source implementations in `src/`.
   - Service layers (`AuthManager`, `FleetManager`, `ComplaintManager`, `RouteController`) hide low-level file I/O and data parsing details.

---

## 📂 Project Structure

```
├── CMakeLists.txt              # Primary CMake configuration (C++17, Qt6)
├── README.md                   # Project overview & documentation
├── README_VIVA_GUIDE.md        # Academic PBL viva evaluation guide
├── assets/                     # High-resolution campus bus imagery
│   └── campus_bus_bg.jpg
├── data/                       # CSV flat-file persistence
│   ├── attendance.csv
│   ├── buses.csv
│   ├── complaints.csv
│   ├── drivers.csv
│   ├── notifications.txt
│   ├── polls.csv
│   ├── routes.csv
│   ├── stops.csv
│   ├── students.csv
│   ├── trips.csv
│   ├── users.txt
│   └── votes.csv
├── include/                    # Header definitions
│   ├── algorithms/             # Graph, RoutingAlgorithm, AStar, Dijkstra
│   ├── models/                 # User, Admin, Student, Driver, Bus, Complaint, etc.
│   ├── services/               # Auth, Fleet, Complaint, ETA, File, RouteController
│   └── ui/                     # MainWindow, Dashboards, Map, ComplaintDesk
├── src/                        # C++ implementations
│   ├── algorithms/
│   ├── models/
│   ├── services/
│   ├── ui/
│   └── main.cpp
└── SmartCollegeBus_Qt6_Migration/ # Modular standalone Qt 6 migration project
```

---

## 🛠️ Build & Run Instructions

### Prerequisites
- **C++ Compiler**: Clang (macOS) / GCC (Linux) / MSVC (Windows) with **C++17** support
- **Qt 6**: `Qt6::Core`, `Qt6::Gui`, `Qt6::Widgets`, `Qt6::Network`
- **CMake**: Version 3.16 or higher

### Build via Command Line
```bash
# Create build directory
mkdir -p build && cd build

# Configure with CMake
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Compile executable
make -j4

# Run application
./SmartCollegeBusSystem
# or ./smart_college_bus_system_gui
```

### Build via Qt Creator
1. Open Qt Creator.
2. Select **File > Open File or Project...** and choose `CMakeLists.txt`.
3. Configure project with your Qt 6 Kit (e.g. Qt 6.11 macOS).
4. Click **Build** and **Run (⌘ + R)**.

---

## 🔑 Demo Login Profiles

| Role | Username / ID | Password | Notes |
|---|---|---|---|
| **Administrator** | `admin` | `admin123` | College Transport Head |
| **Student** | `2024-CS-001` | `student123` | Aarav Sharma (B.Tech CSE) |
| **Driver** | `D-01` | `driver123` | Rajesh Kumar (Bus B-101) |
| **Parent** | `P-101` | `parent123` | Vijay Sharma |

---

## 📜 License
Academic PBL Evaluation Project for **Graphic Era Hill University, Bhimtal Campus**.
All rights reserved.
