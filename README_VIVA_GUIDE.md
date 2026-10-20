# Smart College Bus Management & Route Optimization System
## Academic Project Documentation & Viva Evaluation Guide (C++ OOPs PBL)

---

## 📌 Project Overview
This project is an Object-Oriented C++17 application designed for college **Project-Based Learning (PBL)** evaluation for a **team of 4 students**. It solves the real-world problem of student college bus schedule confusion:

1. **Schedule-Type Awareness**: Differentiates timing sets between **Half-Day (Exam / Early Hours)** and **Full-Day (Normal Hours)** schedules so students always know which bus timing applies today.
2. **Morning Departure Clarity**: Explicitly provides students with exact morning departure times at their assigned stop.
3. **Connecting Bus Engine (Core Differentiating Logic)**: If a student misses their assigned bus, the system searches all other buses passing through that same stop after the missed time, checks seat availability, sorts them chronologically, and provides driver contact details.

---

## 👥 Module Division & Member Ownership Table

| Member | Technical Ownership | Key Class Files | Specific OOP Concepts Demonstrated |
|---|---|---|---|
| **Member 1** | **Student & Route Management** | `Student.hpp/cpp`, `Route.hpp/cpp`, `Stop.hpp/cpp` | **Inheritance** (`Student` from `Person`), **Encapsulation**, **Operator Overloading** (`operator<<`), **STL** (`std::vector`, `std::map`) |
| **Member 2** | **Person Hierarchy & Bus Management** | `Person.hpp` (ABC), `Driver.hpp/cpp`, `Conductor.hpp/cpp`, `Bus.hpp/cpp` | **Abstraction** (Pure virtual `displayDetails()`), **Polymorphism** (Virtual function overriding), **Composition / Association** (`Bus` holding `Driver*` and `Conductor*`) |
| **Member 3** | **Scheduling, ETA & Route Optimization** | `ScheduleTypes.hpp`, `LiveLocationSim.hpp`, `ETAEngine.hpp/cpp`, `RouteOptimizer.hpp/cpp` | **Function Overloading** (Compile-time Polymorphism for `calculateETA()`), **Threading & Chrono** (`std::this_thread::sleep_for`), **Dijkstra Graph Search** |
| **Member 4** | **Connecting Bus Logic, Attendance & File Handling** | `ConnectingBusEngine.hpp/cpp`, `AttendanceLogger.hpp/cpp`, `Exceptions.hpp`, `FileHandler.hpp/cpp` | **Exception Handling** (`SeatFullException`, `NoConnectingBusException`), **File I/O Persistence** (`std::ifstream`, `std::ofstream`), **Algorithm Filtering & Lambda Functions** |

---

## 🧠 Breakdown of the 4 Pillars of OOPs in Code

### 1. Abstraction
- **File**: [`include/Person.hpp`](file:///Users/tushartiwari565/.gemini/antigravity/scratch/smart_college_bus_system/include/Person.hpp)
- **Code Reference**:
  ```cpp
  class Person {
  public:
      virtual void displayDetails() const = 0; // Pure Virtual Function
      virtual std::string getRole() const = 0;
  };
  ```
- **Explanation for Viva**: `Person` is an Abstract Base Class (ABC) that defines a blueprint for all system individuals (Student, Driver, Conductor). It cannot be instantiated directly.

### 2. Inheritance
- **Files**: [`include/Student.hpp`](file:///Users/tushartiwari565/.gemini/antigravity/scratch/smart_college_bus_system/include/Student.hpp), [`include/Driver.hpp`](file:///Users/tushartiwari565/.gemini/antigravity/scratch/smart_college_bus_system/include/Driver.hpp), [`include/Conductor.hpp`](file:///Users/tushartiwari565/.gemini/antigravity/scratch/smart_college_bus_system/include/Conductor.hpp)
- **Code Reference**:
  ```cpp
  class Student : public Person { ... };
  class Driver : public Person { ... };
  class Conductor : public Person { ... };
  ```
- **Explanation for Viva**: `Student`, `Driver`, and `Conductor` inherit common attributes (`id`, `name`, `phone`, `email`) from `Person`, avoiding code duplication and establishing an IS-A relationship.

### 3. Polymorphism
- **Runtime Polymorphism (Virtual Function Overriding)**:
  - Base class pointer vector `std::vector<std::shared_ptr<Person>> people` in `SystemManager`. Calling `personPtr->displayDetails()` invokes the specific derived version (`Student`, `Driver`, or `Conductor`).
- **Compile-time Polymorphism (Function Overloading)**:
  - In `ETAEngine`:
    ```cpp
    static int calculateETA(const Route& route, const std::string& currentStopId, const std::string& targetStopId);
    static int calculateETA(const Route& route, const std::string& currentStopId, const std::string& targetStopId, ScheduleType scheduleType, double trafficFactor);
    ```

### 4. Encapsulation
- **Files**: All header files (`Bus.hpp`, `Student.hpp`, etc.)
- **Code Reference**:
  - `currentOccupancy` and `capacity` in `Bus` are `private`. They can only be modified through `boardStudent()` which validates that occupancy does not exceed capacity.

---

## 🛠️ Build & Run Commands

```bash
# Navigate to project directory
cd /Users/tushartiwari565/.gemini/antigravity/scratch/smart_college_bus_system

# Compile the application
make

# Run the interactive console application
./smart_bus_system

# Run automated academic self-test suite (for quick viva demonstration)
./smart_bus_system --test
```

---

## 🎓 Viva Questions & Answers Guide for Team Members

### Member 1 Questions (Student & Route Management):
- **Q: How does `Student` inherit from `Person`?**
  - *Answer*: `Student` derives publicly from `Person` using `class Student : public Person`. The constructor passes `sId`, `sName`, `sPhone`, `sEmail` up to `Person(sId, sName, sPhone, sEmail)`.
- **Q: Where is operator overloading used?**
  - *Answer*: In `Student.cpp` and `Route.cpp`, we overloaded `operator<<` so that `Student` and `Route` objects can be printed cleanly directly using `std::cout << student;`.

### Member 2 Questions (Person & Bus Management):
- **Q: Why is `Person` destructor virtual?**
  - *Answer*: If we delete a derived object (like `Student`) through a `Person*` base pointer, a non-virtual destructor causes undefined behavior. A virtual destructor ensures the derived class destructor is called properly.
- **Q: How are Driver and Conductor linked to Bus?**
  - *Answer*: `Bus` contains raw/shared pointers `Driver* driver` and `Conductor* conductor` (Composition/Association).

### Member 3 Questions (Scheduling, Live Sim & Route Optimization):
- **Q: How does the system handle Half-Day vs Full-Day schedules?**
  - *Answer*: `ScheduleType` is an enum (`HALF_DAY`, `FULL_DAY`). `Stop` maintains two separate arrival time maps (`fullDayArrivalTimes` and `halfDayArrivalTimes`). The active schedule toggles which map is queried.
- **Q: What algorithm is used for route optimization?**
  - *Answer*: `RouteOptimizer` uses a graph search algorithm to traverse stops across routes, calculate cumulative segment distances, and compute estimated travel time.

### Member 4 Questions (Connecting Bus Engine & File Handling):
- **Q: How does the Missed Bus feature work?**
  - *Answer*: In `ConnectingBusEngine.cpp`, `findConnectingBuses()` takes the student's stop ID and missed departure time in minutes. It iterates through all buses passing that stop, filters out those arriving after the missed time, sorts them chronologically using `std::sort` with a Lambda expression, and returns the best options.
- **Q: How are exceptions used?**
  - *Answer*: Custom exception hierarchy in `Exceptions.hpp` derived from `std::exception`. For example, `SeatFullException` is thrown when `bus.boardStudent()` is called on a full bus.
