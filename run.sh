#!/bin/bash
# Convenient launcher script for Smart College Bus System (CLI & Qt GUI)

echo "==========================================================="
echo " 🚌 SMART COLLEGE BUS SYSTEM LAUNCHER (C++ OOP & Qt GUI)"
echo "==========================================================="
echo "  1. Run Interactive C++ Console Application"
echo "  2. Run Academic Viva Self-Test Suite"
echo "  3. Build Qt GUI Project via qmake / CMake"
echo "  4. Open Interactive Web Visualizer Dashboard"
echo "  5. Recompile CLI System (make clean && make)"
echo "==========================================================="
read -p "Select option (1-5): " choice

case $choice in
    1)
        make && ./smart_bus_system
        ;;
    2)
        make && ./smart_bus_system --test
        ;;
    3)
        echo "Building Qt GUI..."
        if command -v qmake &> /dev/null; then
            qmake smart_college_bus_system.pro && make
            ./bin/smart_college_bus_system_gui
        elif command -v cmake &> /dev/null; then
            mkdir -p build && cd build && cmake .. && make
            ./smart_college_bus_system_gui
        else
            echo "Qt tools (qmake/cmake) not found in PATH."
            echo "Please open 'smart_college_bus_system.pro' in Qt Creator IDE!"
        fi
        ;;
    4)
        echo "Opening web visualizer..."
        open web/index.html || xdg-open web/index.html || echo "Please open file:///Users/tushartiwari565/.gemini/antigravity/scratch/smart_college_bus_system/web/index.html in your browser."
        ;;
    5)
        make clean && make
        ;;
    *)
        echo "Invalid option."
        ;;
esac
