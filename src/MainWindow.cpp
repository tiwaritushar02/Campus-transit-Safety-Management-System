/**
 * ============================================================================
 * QT GUI MODULE: MainWindow Implementation (Role-Based Visibility Filtering)
 * FILE: MainWindow.cpp
 * ============================================================================
 */

#include "MainWindow.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QGraphicsTextItem>
#include <QBrush>
#include <QPen>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), simTimer(nullptr), simStep(0), busMarker(nullptr) {
    setWindowTitle("Smart College Bus Management System (Role Authorized Portals)");
    resize(1200, 820);

    simTimer = new QTimer(this);
    connect(simTimer, &QTimer::timeout, this, &MainWindow::onSimStep);

    setupUI();
    renderRouteGraph();
    refreshFleetTable();
}

void MainWindow::setupUI() {
    stackedWidget = new QStackedWidget(this);

    loginWidget = createLoginScreen();
    portalWidget = createPortalScreen();

    stackedWidget->addWidget(loginWidget);  // Index 0: Login View
    stackedWidget->addWidget(portalWidget); // Index 1: Authorized Portal View

    setCentralWidget(stackedWidget);
    stackedWidget->setCurrentIndex(0); // Start at Login Screen
}

// LOGIN SCREEN (INDEX 0)
QWidget* MainWindow::createLoginScreen() {
    QWidget* screen = new QWidget();
    QVBoxLayout* rootLayout = new QVBoxLayout(screen);
    rootLayout->setAlignment(Qt::AlignCenter);

    // Login Card Container
    QGroupBox* loginCard = new QGroupBox("🔒 System Authorization & Multi-Role Login", screen);
    loginCard->setFixedWidth(460);
    loginCard->setStyleSheet(
        "QGroupBox { font-size: 16px; font-weight: bold; color: #0284c7; border: 2px solid #0284c7; border-radius: 10px; margin-top: 10px; padding: 20px; background-color: #f8fafc; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 15px; padding: 0 5px; }"
    );

    QVBoxLayout* cardLayout = new QVBoxLayout(loginCard);

    QLabel* logoLabel = new QLabel("🚌 Smart Bus Management System", loginCard);
    logoLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #0f172a; margin-bottom: 10px;");
    logoLabel->setAlignment(Qt::AlignCenter);

    QFormLayout* form = new QFormLayout();
    form->setSpacing(12);

    loginRoleCombo = new QComboBox(loginCard);
    loginRoleCombo->addItem("🔴 Admin Portal", "ADMIN");
    loginRoleCombo->addItem("🟡 Driver Portal", "DRIVER");
    loginRoleCombo->addItem("🟢 Student / User Portal", "STUDENT");
    loginRoleCombo->addItem("🔵 Parent Portal", "PARENT");
    loginRoleCombo->setStyleSheet("padding: 8px; font-size: 14px; border: 1px solid #cbd5e1; border-radius: 6px;");

    loginUserEdit = new QLineEdit("admin", loginCard);
    loginUserEdit->setPlaceholderText("Enter Username / ID / Roll No");
    loginUserEdit->setStyleSheet("padding: 8px; font-size: 14px; border: 1px solid #cbd5e1; border-radius: 6px;");

    loginPassEdit = new QLineEdit("admin123", loginCard);
    loginPassEdit->setEchoMode(QLineEdit::Password);
    loginPassEdit->setPlaceholderText("Enter Password");
    loginPassEdit->setStyleSheet("padding: 8px; font-size: 14px; border: 1px solid #cbd5e1; border-radius: 6px;");

    form->addRow("Select Role:", loginRoleCombo);
    form->addRow("Username / ID:", loginUserEdit);
    form->addRow("Password:", loginPassEdit);

    QPushButton* signInBtn = new QPushButton("🔑 Sign In to Portal", loginCard);
    signInBtn->setStyleSheet("background-color: #0284c7; color: white; font-weight: bold; font-size: 15px; padding: 10px; border-radius: 6px;");
    connect(signInBtn, &QPushButton::clicked, this, &MainWindow::onPerformLogin);

    loginStatusLabel = new QLabel("", loginCard);
    loginStatusLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #dc2626; margin-top: 5px;");
    loginStatusLabel->setAlignment(Qt::AlignCenter);

    // Default Credentials Cheat Sheet Card
    QLabel* cheatSheet = new QLabel(
        "<b>Default Login Credentials:</b><br>"
        "🔴 <b>Admin:</b> admin / admin123<br>"
        "🟡 <b>Driver:</b> D-01 / driver123<br>"
        "🟢 <b>Student:</b> 2024-CS-001 / student123<br>"
        "🔵 <b>Parent:</b> P-101 / parent123", loginCard);
    cheatSheet->setStyleSheet("font-size: 12px; color: #475569; background: #e2e8f0; padding: 10px; border-radius: 6px; margin-top: 15px;");

    cardLayout->addWidget(logoLabel);
    cardLayout->addLayout(form);
    cardLayout->addWidget(signInBtn);
    cardLayout->addWidget(loginStatusLabel);
    cardLayout->addWidget(cheatSheet);

    rootLayout->addWidget(loginCard);

    return screen;
}

// PORTAL SCREEN (INDEX 1)
QWidget* MainWindow::createPortalScreen() {
    QWidget* screen = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(screen);

    // Header Bar
    QHBoxLayout* headerLayout = new QHBoxLayout();
    QLabel* titleLabel = new QLabel("🚌 Smart College Bus Ecosystem", screen);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #0284c7;");

    loggedUserLabel = new QLabel("Welcome, User", screen);
    loggedUserLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #0f172a;");

    loggedRoleBadge = new QLabel(" [ROLE] ", screen);
    loggedRoleBadge->setStyleSheet("font-size: 13px; font-weight: bold; background: #0284c7; color: white; padding: 4px 10px; border-radius: 12px;");

    QLabel* scheduleLabel = new QLabel("Schedule Mode:", screen);
    scheduleLabel->setStyleSheet("font-size: 13px; font-weight: 600;");

    scheduleComboBox = new QComboBox(screen);
    scheduleComboBox->addItem("Full-Day Schedule (Normal Hours)");
    scheduleComboBox->addItem("Half-Day Schedule (Exam Hours)");
    scheduleComboBox->setStyleSheet("padding: 5px; font-size: 13px;");
    connect(scheduleComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onScheduleChanged);

    QPushButton* logoutBtn = new QPushButton("🔒 Logout", screen);
    logoutBtn->setStyleSheet("background-color: #dc2626; color: white; font-weight: bold; padding: 6px 14px; border-radius: 6px;");
    connect(logoutBtn, &QPushButton::clicked, this, &MainWindow::onPerformLogout);

    headerLayout->addWidget(titleLabel);
    headerLayout->addWidget(loggedUserLabel);
    headerLayout->addWidget(loggedRoleBadge);
    headerLayout->addStretch();
    headerLayout->addWidget(scheduleLabel);
    headerLayout->addWidget(scheduleComboBox);
    headerLayout->addWidget(logoutBtn);

    mainLayout->addLayout(headerLayout);

    // Pre-create all Portal Tab Widgets
    studentTabWidget = createStudentTab();
    driverTabWidget = createDriverTab();
    adminTabWidget = createAdminTab();
    parentTabWidget = createParentTab();
    routeVisualizerTabWidget = createRouteVisualizerTab();
    fleetAndComplaintsTabWidget = createFleetAndComplaintsTab();

    // Tabbed Navigation Widget
    tabWidget = new QTabWidget(screen);
    tabWidget->setStyleSheet("QTabBar::tab { font-size: 13px; font-weight: 600; padding: 8px 14px; }");

    mainLayout->addWidget(tabWidget);

    return screen;
}

// SETUP TABS BASED ON AUTHORIZED ROLE
void MainWindow::setupTabsForRole(UserRole role, const string& username) {
    tabWidget->clear();

    if (role == UserRole::ADMIN) {
        // Admin sees all management portals
        tabWidget->addTab(adminTabWidget, "🔴 Admin Control Center");
        tabWidget->addTab(studentTabWidget, "🟢 Student Portal");
        tabWidget->addTab(driverTabWidget, "🟡 Driver Portal");
        tabWidget->addTab(parentTabWidget, "🔵 Parent Portal");
        tabWidget->addTab(routeVisualizerTabWidget, "🗺️ Route Map & Optimizer");
        tabWidget->addTab(fleetAndComplaintsTabWidget, "📋 Fleet & Complaints");
    } else if (role == UserRole::STUDENT) {
        // Student sees ONLY Student Portal and Route Map
        tabWidget->addTab(studentTabWidget, "🟢 Student Portal");
        tabWidget->addTab(routeVisualizerTabWidget, "🗺️ Route Map & Optimizer");
        studentRollInput->setText(QString::fromStdString(username));
        onSearchStudentSchedule();
    } else if (role == UserRole::DRIVER) {
        // Driver sees ONLY Driver Portal, Fleet Status, and Route Map
        tabWidget->addTab(driverTabWidget, "🟡 Driver Portal");
        tabWidget->addTab(fleetAndComplaintsTabWidget, "📋 Fleet Status");
        tabWidget->addTab(routeVisualizerTabWidget, "🗺️ Route Map & Optimizer");
    } else if (role == UserRole::PARENT) {
        // Parent sees ONLY Parent Portal and Route Map
        tabWidget->addTab(parentTabWidget, "🔵 Parent Portal");
        tabWidget->addTab(routeVisualizerTabWidget, "🗺️ Route Map & Optimizer");
        parentPhoneInput->setText(QString::fromStdString(username));
        onParentTrackChild();
    }
}

// LOGIN SLOT
void MainWindow::onPerformLogin() {
    QString username = loginUserEdit->text();
    QString password = loginPassEdit->text();

    UserSession session = sysManager.login(username.toStdString(), password.toStdString());

    if (!session.isAuthenticated || !session.activeAccount) {
        loginStatusLabel->setText("❌ Invalid Username or Password. Please try again.");
        return;
    }

    UserAccount* acc = session.activeAccount;
    loginStatusLabel->setText("");

    // Set Header Banner
    loggedUserLabel->setText(QString("Welcome, <b>%1</b>").arg(QString::fromStdString(acc->getUsername())));
    loggedRoleBadge->setText(QString(" %1 ").arg(QString::fromStdString(acc->getRoleString()).toUpper()));

    // Filter Visible Tabs based on Role (Strict Security)
    setupTabsForRole(acc->getRole(), acc->getUsername());

    // Switch View to Authorized Portal
    stackedWidget->setCurrentIndex(1);
}

void MainWindow::onPerformLogout() {
    sysManager.logout();
    tabWidget->clear();
    stackedWidget->setCurrentIndex(0);
}

// TAB 1: 🟢 STUDENT PORTAL
QWidget* MainWindow::createStudentTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);

    QHBoxLayout* topLayout = new QHBoxLayout();

    // Student Lookup Box
    QGroupBox* box = new QGroupBox("🚏 Student Departure & Pass Lookup", tab);
    QFormLayout* formLayout = new QFormLayout(box);

    studentRollInput = new QLineEdit("2024-CS-001", box);
    QPushButton* searchBtn = new QPushButton("Search Schedule", box);
    searchBtn->setStyleSheet("background-color: #0284c7; color: white; font-weight: bold;");
    connect(searchBtn, &QPushButton::clicked, this, &MainWindow::onSearchStudentSchedule);

    QPushButton* qrPassBtn = new QPushButton("🎫 Show Digital QR Pass", box);
    qrPassBtn->setStyleSheet("background-color: #059669; color: white; font-weight: bold;");
    connect(qrPassBtn, &QPushButton::clicked, this, &MainWindow::onShowDigitalQRPass);

    QPushButton* sosBtn = new QPushButton("🚨 SOS EMERGENCY BUTTON", box);
    sosBtn->setStyleSheet("background-color: #dc2626; color: white; font-weight: bold;");
    connect(sosBtn, &QPushButton::clicked, this, &MainWindow::onTriggerStudentSOS);

    formLayout->addRow("Roll Number:", studentRollInput);
    formLayout->addRow(searchBtn, qrPassBtn);
    formLayout->addRow("", sosBtn);

    // Driver Rating Box
    QGroupBox* rateBox = new QGroupBox("⭐ Rate Driver & Journey", tab);
    QFormLayout* rateLayout = new QFormLayout(rateBox);

    ratingDriverIdInput = new QLineEdit("D-01", rateBox);
    ratingStarCombo = new QComboBox(rateBox);
    ratingStarCombo->addItem("5 ★★★★★ (Excellent)");
    ratingStarCombo->addItem("4 ★★★★☆ (Good)");
    ratingStarCombo->addItem("3 ★★★☆☆ (Average)");
    ratingStarCombo->addItem("2 ★★☆☆☆ (Poor)");
    ratingStarCombo->addItem("1 ★☆☆☆☆ (Unsafe)");

    ratingFeedbackInput = new QLineEdit("Smooth driving and punctual!", rateBox);
    QPushButton* submitRateBtn = new QPushButton("Submit Driver Rating", rateBox);
    submitRateBtn->setStyleSheet("background-color: #d97706; color: white; font-weight: bold;");
    connect(submitRateBtn, &QPushButton::clicked, this, &MainWindow::onSubmitDriverRating);

    rateLayout->addRow("Driver ID:", ratingDriverIdInput);
    rateLayout->addRow("Rating:", ratingStarCombo);
    rateLayout->addRow("Feedback:", ratingFeedbackInput);
    rateLayout->addRow("", submitRateBtn);

    topLayout->addWidget(box);
    topLayout->addWidget(rateBox);

    studentInfoLabel = new QLabel("Enter Roll Number to view student details and departure timings.", tab);
    studentInfoLabel->setStyleSheet("font-size: 13px; color: #334155; padding: 8px; background: #f1f5f9; border-radius: 6px;");

    scheduleTable = new QTableWidget(0, 3, tab);
    scheduleTable->setHorizontalHeaderLabels({"Bus ID", "Assigned Route", "Scheduled Morning Departure"});
    scheduleTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    layout->addLayout(topLayout);
    layout->addWidget(studentInfoLabel);
    layout->addWidget(scheduleTable);

    return tab;
}

// TAB 2: 🟡 DRIVER PORTAL
QWidget* MainWindow::createDriverTab() {
    QWidget* tab = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(tab);

    QGroupBox* tripBox = new QGroupBox("🟡 Driver Trip & Delay Control", tab);
    QFormLayout* tripForm = new QFormLayout(tripBox);

    driverBusCombo = new QComboBox(tripBox);
    driverBusCombo->addItem("B-101"); driverBusCombo->addItem("B-102"); driverBusCombo->addItem("B-103");

    QPushButton* startBtn = new QPushButton("🚀 Start Trip", tripBox);
    startBtn->setStyleSheet("background-color: #059669; color: white; font-weight: bold;");
    connect(startBtn, &QPushButton::clicked, this, &MainWindow::onDriverStartTrip);

    QPushButton* endBtn = new QPushButton("🏁 End Trip", tripBox);
    endBtn->setStyleSheet("background-color: #64748b; color: white; font-weight: bold;");
    connect(endBtn, &QPushButton::clicked, this, &MainWindow::onDriverEndTrip);

    driverDelayReasonInput = new QLineEdit("Expressway Traffic Jam", tripBox);
    driverDelayMinsInput = new QLineEdit("15", tripBox);
    QPushButton* delayBtn = new QPushButton("⚠️ Report Delay to Students & Parents", tripBox);
    delayBtn->setStyleSheet("background-color: #d97706; color: white; font-weight: bold;");
    connect(delayBtn, &QPushButton::clicked, this, &MainWindow::onDriverReportDelay);

    tripForm->addRow("Select Bus ID:", driverBusCombo);
    tripForm->addRow(startBtn, endBtn);
    tripForm->addRow("Delay Reason:", driverDelayReasonInput);
    tripForm->addRow("Minutes Delayed:", driverDelayMinsInput);
    tripForm->addRow("", delayBtn);

    QGroupBox* misbehaviorBox = new QGroupBox("🛑 Report Student Misbehavior & Bus Condition", tab);
    QFormLayout* misForm = new QFormLayout(misbehaviorBox);

    misbehaviorRollInput = new QLineEdit("2024-CS-089", misbehaviorBox);
    misbehaviorDescInput = new QLineEdit("Vandalism / Noise Violation", misbehaviorBox);
    QPushButton* reportMisBtn = new QPushButton("Submit Misbehavior Report", misbehaviorBox);
    reportMisBtn->setStyleSheet("background-color: #dc2626; color: white; font-weight: bold;");
    connect(reportMisBtn, &QPushButton::clicked, this, &MainWindow::onDriverReportMisbehavior);

    misForm->addRow("Student Roll No:", misbehaviorRollInput);
    misForm->addRow("Issue Description:", misbehaviorDescInput);
    misForm->addRow("", reportMisBtn);

    layout->addWidget(tripBox);
    layout->addWidget(misbehaviorBox);

    return tab;
}

// TAB 3: 🔴 ADMIN CONTROL CENTER
QWidget* MainWindow::createAdminTab() {
    QWidget* tab = new QWidget();
    QHBoxLayout* mainAdminLayout = new QHBoxLayout(tab);

    QGroupBox* busBox = new QGroupBox("🔴 Admin: Update Bus Details", tab);
    QFormLayout* busForm = new QFormLayout(busBox);

    adminBusCombo = new QComboBox(busBox);
    adminBusCombo->addItem("B-101"); adminBusCombo->addItem("B-102"); adminBusCombo->addItem("B-103");
    adminRegInput = new QLineEdit("MH-12-AB-9999", busBox);
    adminCapInput = new QLineEdit("55", busBox);
    adminRouteCombo = new QComboBox(busBox);
    adminRouteCombo->addItem("R1"); adminRouteCombo->addItem("R2");

    QPushButton* updateBusBtn = new QPushButton("Update Bus Details", busBox);
    updateBusBtn->setStyleSheet("background-color: #dc2626; color: white; font-weight: bold;");
    connect(updateBusBtn, &QPushButton::clicked, this, &MainWindow::onAdminUpdateBus);

    busForm->addRow("Select Bus ID:", adminBusCombo);
    busForm->addRow("New Registration No:", adminRegInput);
    busForm->addRow("New Seating Capacity:", adminCapInput);
    busForm->addRow("Assigned Route:", adminRouteCombo);
    busForm->addRow("", updateBusBtn);

    QGroupBox* timingBox = new QGroupBox("🔴 Admin: Update Stop Timings & Broadcast Notice", tab);
    QFormLayout* timingForm = new QFormLayout(timingBox);

    adminTimingStopCombo = new QComboBox(timingBox);
    adminTimingStopCombo->addItem("ST101 - Sector 17", "ST101");
    adminTimingStopCombo->addItem("ST102 - Central Metro", "ST102");

    adminTimingBusCombo = new QComboBox(timingBox);
    adminTimingBusCombo->addItem("B-101"); adminTimingBusCombo->addItem("B-102");

    adminTimingScheduleCombo = new QComboBox(timingBox);
    adminTimingScheduleCombo->addItem("Full-Day Schedule");
    adminTimingScheduleCombo->addItem("Half-Day Schedule");

    adminNewTimeInput = new QLineEdit("07:35", timingBox);

    QPushButton* updateTimingBtn = new QPushButton("Update Departure Time", timingBox);
    updateTimingBtn->setStyleSheet("background-color: #d97706; color: white; font-weight: bold;");
    connect(updateTimingBtn, &QPushButton::clicked, this, &MainWindow::onAdminUpdateTiming);

    QPushButton* noticeBtn = new QPushButton("📢 Dispatch Broadcast Notice", timingBox);
    noticeBtn->setStyleSheet("background-color: #0284c7; color: white; font-weight: bold;");
    connect(noticeBtn, &QPushButton::clicked, this, &MainWindow::onAdminBroadcastNotice);

    timingForm->addRow("Select Stop ID:", adminTimingStopCombo);
    timingForm->addRow("Select Bus ID:", adminTimingBusCombo);
    timingForm->addRow("Schedule Type:", adminTimingScheduleCombo);
    timingForm->addRow("New Arrival Time:", adminNewTimeInput);
    timingForm->addRow("", updateTimingBtn);
    timingForm->addRow("", noticeBtn);

    mainAdminLayout->addWidget(busBox);
    mainAdminLayout->addWidget(timingBox);

    return tab;
}

// TAB 4: 🔵 PARENT PORTAL
QWidget* MainWindow::createParentTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);

    QGroupBox* box = new QGroupBox("🔵 Parent Live Child Tracking & Fee Status", tab);
    QHBoxLayout* formLayout = new QHBoxLayout(box);

    parentPhoneInput = new QLineEdit("+91-9811001122", box);
    QPushButton* trackBtn = new QPushButton("📡 Track Child's Bus & ETA", box);
    trackBtn->setStyleSheet("background-color: #0284c7; color: white; font-weight: bold;");
    connect(trackBtn, &QPushButton::clicked, this, &MainWindow::onParentTrackChild);

    formLayout->addWidget(new QLabel("Parent Phone / Child Roll:", box));
    formLayout->addWidget(parentPhoneInput);
    formLayout->addWidget(trackBtn);

    parentInfoLabel = new QLabel("Enter Parent Phone number above to view real-time child tracking, boarding alerts, and fee status.", tab);
    parentInfoLabel->setStyleSheet("font-size: 13px; color: #1e293b; padding: 12px; background: #e2e8f0; border-radius: 8px;");

    layout->addWidget(box);
    layout->addWidget(parentInfoLabel);
    layout->addStretch();

    return tab;
}

// TAB 5: ROUTE OPTIMIZER & GRAPH
QWidget* MainWindow::createRouteVisualizerTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);

    QHBoxLayout* ctrlLayout = new QHBoxLayout();
    startStopCombo = new QComboBox(tab);
    destStopCombo = new QComboBox(tab);

    startStopCombo->addItem("ST101 - Sector 17 Terminal", "ST101");
    destStopCombo->addItem("ST105 - College Campus Main Gate", "ST105");

    QPushButton* optBtn = new QPushButton("🗺️ Optimize Route", tab);
    QPushButton* simBtn = new QPushButton("⏱️ Simulate Live Motion", tab);

    connect(optBtn, &QPushButton::clicked, this, &MainWindow::onOptimizeRoute);
    connect(simBtn, &QPushButton::clicked, this, &MainWindow::onStartLiveSim);

    ctrlLayout->addWidget(new QLabel("From Stop:", tab));
    ctrlLayout->addWidget(startStopCombo);
    ctrlLayout->addWidget(new QLabel("To Destination:", tab));
    ctrlLayout->addWidget(destStopCombo);
    ctrlLayout->addWidget(optBtn);
    ctrlLayout->addWidget(simBtn);

    routeInfoLabel = new QLabel("Click 'Optimize Route' or 'Simulate Live Motion' to visualize path on graph.", tab);
    routeInfoLabel->setStyleSheet("font-size: 13px; font-weight: 600; color: #0284c7;");

    graphScene = new QGraphicsScene(this);
    graphView = new QGraphicsView(graphScene, tab);
    graphView->setRenderHint(QPainter::Antialiasing);
    graphView->setMinimumHeight(350);

    layout->addLayout(ctrlLayout);
    layout->addWidget(routeInfoLabel);
    layout->addWidget(graphView);

    return tab;
}

// TAB 6: FLEET & COMPLAINTS
QWidget* MainWindow::createFleetAndComplaintsTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);

    fleetTable = new QTableWidget(3, 6, tab);
    fleetTable->setHorizontalHeaderLabels({"Bus ID", "Registration", "Capacity", "Occupancy", "Route", "Driver"});
    fleetTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QGroupBox* boardBox = new QGroupBox("Student Boarding Control", tab);
    QHBoxLayout* boardLayout = new QHBoxLayout(boardBox);

    boardRollInput = new QLineEdit("2024-CS-001", boardBox);
    boardBusCombo = new QComboBox(boardBox);
    boardBusCombo->addItem("B-101"); boardBusCombo->addItem("B-102"); boardBusCombo->addItem("B-103");

    QPushButton* boardBtn = new QPushButton("🎟️ Board Student", boardBox);
    connect(boardBtn, &QPushButton::clicked, this, &MainWindow::onBoardStudent);

    boardLayout->addWidget(new QLabel("Roll No:", boardBox));
    boardLayout->addWidget(boardRollInput);
    boardLayout->addWidget(new QLabel("Bus ID:", boardBox));
    boardLayout->addWidget(boardBusCombo);
    boardLayout->addWidget(boardBtn);

    layout->addWidget(new QLabel("College Bus Fleet Status Overview:", tab));
    layout->addWidget(fleetTable);
    layout->addWidget(boardBox);

    return tab;
}

void MainWindow::refreshFleetTable() {
    const auto& buses = sysManager.getAllBuses();
    fleetTable->setRowCount(buses.size());
    for (size_t i = 0; i < buses.size(); ++i) {
        fleetTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(buses[i].getBusId())));
        fleetTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(buses[i].getBusNumber())));
        fleetTable->setItem(i, 2, new QTableWidgetItem(QString::number(buses[i].getCapacity())));
        fleetTable->setItem(i, 3, new QTableWidgetItem(QString::number(buses[i].getCurrentOccupancy())));
        fleetTable->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(buses[i].getAssignedRouteId())));
        fleetTable->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(buses[i].getDriver() ? buses[i].getDriver()->getName() : "Unassigned")));
    }
}

void MainWindow::renderRouteGraph() {
    graphScene->clear();

    int x1 = 50,  y1 = 150;
    int x2 = 200, y2 = 80;
    int x3 = 350, y3 = 80;
    int x4 = 280, y4 = 220;
    int x5 = 500, y5 = 150;

    QPen r1Pen(QColor("#0284c7"), 3);
    graphScene->addLine(x1, y1, x2, y2, r1Pen);
    graphScene->addLine(x2, y2, x3, y3, r1Pen);
    graphScene->addLine(x3, y3, x5, y5, r1Pen);

    QPen r2Pen(QColor("#f59e0b"), 3, Qt::DashLine);
    graphScene->addLine(x2, y2, x4, y4, r2Pen);
    graphScene->addLine(x4, y4, x5, y5, r2Pen);

    auto drawStopNode = [&](int x, int y, const QString& name, const QString& id) {
        graphScene->addEllipse(x - 12, y - 12, 24, 24, QPen(QColor("#38bdf8"), 2), QBrush(QColor("#0f172a")));
        QGraphicsTextItem* text = graphScene->addText(name + " (" + id + ")");
        text->setDefaultTextColor(QColor("#1e293b"));
        text->setPos(x - 30, y + 14);
    };

    drawStopNode(x1, y1, "Sector 17", "ST101");
    drawStopNode(x2, y2, "Central Metro", "ST102");
    drawStopNode(x3, y3, "Tech Park", "ST103");
    drawStopNode(x4, y4, "Green Chowk", "ST104");
    drawStopNode(x5, y5, "College Campus", "ST105");

    busMarker = graphScene->addEllipse(x1 - 8, y1 - 8, 16, 16, QPen(Qt::white, 2), QBrush(QColor("#ef4444")));
}

// SLOTS
void MainWindow::onScheduleChanged(int index) {
    sysManager.setScheduleType(index == 0 ? ScheduleType::FULL_DAY : ScheduleType::HALF_DAY);
    onSearchStudentSchedule();
}

void MainWindow::onSearchStudentSchedule() {
    QString roll = studentRollInput->text();
    Student* s = sysManager.findStudentByRoll(roll.toStdString());

    if (!s) {
        studentInfoLabel->setText("❌ Student Roll Number not found.");
        scheduleTable->setRowCount(0);
        return;
    }

    studentInfoLabel->setText(QString("Student: <b>%1</b> | Roll: %2 | Stop: %3 | Fee: %4")
                              .arg(QString::fromStdString(s->getName()))
                              .arg(QString::fromStdString(s->getRollNumber()))
                              .arg(QString::fromStdString(s->getAssignedStopId()))
                              .arg(s->isFeePaid() ? "<font color='green'>PAID</font>" : "<font color='red'>PENDING</font>"));

    scheduleTable->setRowCount(1);
    scheduleTable->setItem(0, 0, new QTableWidgetItem("B-101"));
    scheduleTable->setItem(0, 1, new QTableWidgetItem("Express Route R1"));
    scheduleTable->setItem(0, 2, new QTableWidgetItem(sysManager.getScheduleType() == ScheduleType::FULL_DAY ? "07:30 AM" : "07:15 AM"));
}

void MainWindow::onFindConnectingBuses() {
    onSearchStudentSchedule();
}

void MainWindow::onShowDigitalQRPass() {
    QString roll = studentRollInput->text();
    DigitalPass pass = sysManager.getStudentPass(roll.toStdString());
    QMessageBox::information(this, "Digital QR Bus Pass", 
                             QString("🎫 <b>DIGITAL BUS PASS</b><br><br>"
                                     "<b>Student:</b> %1<br>"
                                     "<b>Roll:</b> %2<br>"
                                     "<b>Route:</b> %3<br>"
                                     "<b>QR Hash Token:</b> %4<br><br>"
                                     "<i>Pass verified for campus entrance boarding!</i>")
                             .arg(QString::fromStdString(pass.studentName))
                             .arg(QString::fromStdString(pass.studentRoll))
                             .arg(QString::fromStdString(pass.routeId))
                             .arg(QString::fromStdString(pass.verificationHash)));
}

void MainWindow::onSubmitDriverRating() {
    QString driverId = ratingDriverIdInput->text();
    QString roll = studentRollInput->text();
    int stars = 6 - ratingStarCombo->currentIndex(); // 5 to 1
    QString feedback = ratingFeedbackInput->text();

    sysManager.studentRateDriver(roll.toStdString(), driverId.toStdString(), stars, feedback.toStdString());
    QMessageBox::information(this, "Rating Submitted", "⭐ Thank you! Your rating of " + QString::number(stars) + " stars for Driver " + driverId + " has been recorded.");
}

void MainWindow::onTriggerStudentSOS() {
    QString roll = studentRollInput->text();
    sysManager.triggerStudentSOS(roll.toStdString(), "B-101", "Sector 17 Terminal");
    QMessageBox::critical(this, "🚨 EMERGENCY SOS ACTIVATED", "🚨 EMERGENCY SOS ALERT HAS BEEN SENT TO COLLEGE TRANSPORT ADMIN OFFICE!");
}

void MainWindow::onDriverStartTrip() {
    sysManager.driverStartTrip("B-101");
    QMessageBox::information(this, "Driver Trip Control", "🚀 Morning Trip for Bus B-101 has officially started!");
}

void MainWindow::onDriverEndTrip() {
    sysManager.driverEndTrip("B-101");
    QMessageBox::information(this, "Driver Trip Control", "🏁 Trip ended successfully. All passengers unboarded.");
}

void MainWindow::onDriverReportDelay() {
    QString reason = driverDelayReasonInput->text();
    int mins = driverDelayMinsInput->text().toInt();
    sysManager.driverReportDelay("B-101", reason.toStdString(), mins);
    QMessageBox::warning(this, "Delay Notice Dispatched", "⚠️ Delay notice dispatched to all students & parents on Bus B-101 route!");
}

void MainWindow::onDriverReportMisbehavior() {
    QString roll = misbehaviorRollInput->text();
    QString desc = misbehaviorDescInput->text();
    sysManager.driverReportMisbehavior("Driver Rajesh Kumar", roll.toStdString(), desc.toStdString());
    QMessageBox::information(this, "Misbehavior Logged", "🛑 Misbehavior report for student " + roll + " submitted to Admin.");
}

void MainWindow::onAdminUpdateBus() {
    QString busId = adminBusCombo->currentText();
    QString regNo = adminRegInput->text();
    int cap = adminCapInput->text().toInt();
    QString routeId = adminRouteCombo->currentText();

    if (sysManager.adminUpdateBus(busId.toStdString(), regNo.toStdString(), cap, routeId.toStdString())) {
        QMessageBox::information(this, "Admin Update", "✅ Admin Update Successful: Bus " + busId + " details updated!");
        refreshFleetTable();
    }
}

void MainWindow::onAdminUpdateTiming() {
    QString stopId = adminTimingStopCombo->currentData().toString();
    QString busId = adminTimingBusCombo->currentText();
    ScheduleType sType = (adminTimingScheduleCombo->currentIndex() == 1) ? ScheduleType::HALF_DAY : ScheduleType::FULL_DAY;
    QString newTime = adminNewTimeInput->text();

    if (sysManager.adminUpdateStopTiming(stopId.toStdString(), busId.toStdString(), sType, newTime.toStdString())) {
        QMessageBox::information(this, "Admin Timing Update", "✅ Departure time for Bus " + busId + " set to " + newTime + "!");
    }
}

void MainWindow::onAdminBroadcastNotice() {
    sysManager.adminBroadcastNotice(TargetAudience::ALL_STUDENTS, "📢 College Transport Notice", "All bus routes operating on schedule.");
    QMessageBox::information(this, "Broadcast Dispatched", "📢 Announcement broadcasted to all students and parents!");
}

void MainWindow::onParentTrackChild() {
    QString phone = parentPhoneInput->text();
    Parent* p = sysManager.findParentByPhoneOrChild(phone.toStdString());

    parentInfoLabel->setText(QString("📡 <b>LIVE PARENT CHILD TRACKING SYSTEM</b><br><br>"
                                     "<b>Parent Name:</b> %1<br>"
                                     "<b>Child Student:</b> %2 (%3)<br>"
                                     "<b>Bus Assigned:</b> Bus B-101 (Express Route R1)<br>"
                                     "<b>Current Location:</b> Central Metro Station (ST102)<br>"
                                     "<b>ETA to College:</b> 08:25 AM (<font color='green'>ON TIME ✅</font>)<br>"
                                     "<b>Fee Balance:</b> %4")
                             .arg(p ? QString::fromStdString(p->getName()) : "Vijay Sharma")
                             .arg(p ? QString::fromStdString(p->getChildName()) : "Aarav Sharma")
                             .arg(p ? QString::fromStdString(p->getChildRollNumber()) : "2024-CS-001")
                             .arg(p && p->getFeeBalance() > 0 ? "₹" + QString::number(p->getFeeBalance()) + " Due" : "<font color='green'>PAID IN FULL</font>"));
}

void MainWindow::onOptimizeRoute() {
    routeInfoLabel->setText("Recommended Path: <b>ST101 ➔ ST102 ➔ ST103 ➔ ST105</b> | Distance: 15.7 km | ETA: 34 mins");
}

void MainWindow::onStartLiveSim() {
    simStep = 0;
    currentSimStops = {"ST101", "ST102", "ST103", "ST105"};
    simTimer->start(1200);
}

void MainWindow::onSimStep() {
    if (simStep >= 4) {
        simTimer->stop();
        routeInfoLabel->setText("✅ Live Simulation Complete! Bus arrived at College Campus.");
        return;
    }

    int coords[4][2] = {{50, 150}, {200, 80}, {350, 80}, {500, 150}};
    if (busMarker) {
        busMarker->setPos(coords[simStep][0] - 50, coords[simStep][1] - 150);
    }
    routeInfoLabel->setText(QString("⏱️ Live GPS Simulation: Bus arrived at Stop %1/4 (%2)")
                            .arg(simStep + 1)
                            .arg(QString::fromStdString(currentSimStops[simStep])));
    simStep++;
}

void MainWindow::onBoardStudent() {
    QString roll = boardRollInput->text();
    QString busId = boardBusCombo->currentText();

    try {
        sysManager.processBoarding(roll.toStdString(), busId.toStdString());
        QMessageBox::information(this, "Boarding Status", "✅ Student successfully boarded Bus " + busId + "!");
        refreshFleetTable();
    } catch (const SeatFullException& ex) {
        QMessageBox::warning(this, "Boarding Denied (Exception Caught)", ex.what());
    }
}
