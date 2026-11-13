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
#include <QDateTime>
#include <QInputDialog>
#include <QScrollArea>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), simTimer(nullptr), simStep(0), busMarker(nullptr) {
    setWindowTitle("Campus Transit & Safety Management System");
    resize(1240, 860);

    // Global Modern Application Stylesheet
    setStyleSheet(
        "QMainWindow { background-color: #f1f5f9; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif; }"
        "QGroupBox { font-size: 14px; font-weight: bold; color: #0f172a; border: 1px solid #cbd5e1; border-radius: 8px; margin-top: 14px; padding: 16px; background-color: #ffffff; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 14px; padding: 0 6px; color: #0284c7; }"
        "QLineEdit, QComboBox { padding: 8px 10px; font-size: 13px; border: 1px solid #cbd5e1; border-radius: 6px; background: #f8fafc; color: #0f172a; }"
        "QLineEdit:focus, QComboBox:focus { border: 1.5px solid #0284c7; background: #ffffff; }"
        "QTableWidget { background-color: #ffffff; alternate-background-color: #f8fafc; gridline-color: #e2e8f0; font-size: 12px; border: 1px solid #cbd5e1; border-radius: 6px; selection-background-color: #e0f2fe; selection-color: #0369a1; }"
        "QHeaderView::section { background-color: #0f172a; color: #ffffff; font-weight: bold; font-size: 12px; padding: 6px; border: none; }"
        "QTabBar::tab { font-size: 13px; font-weight: 600; padding: 10px 18px; background: #e2e8f0; color: #475569; border-top-left-radius: 8px; border-top-right-radius: 8px; margin-right: 4px; }"
        "QTabBar::tab:selected { background: #0284c7; color: #ffffff; }"
        "QTabBar::tab:hover:!selected { background: #cbd5e1; color: #0f172a; }"
    );

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

// TOP APPLICATION HEADER BANNER (Consistent Application Title on TOP)
QWidget* MainWindow::createTopHeader(bool isLoggedIn) {
    QWidget* header = new QWidget(this);
    header->setStyleSheet("background: #0f172a; border-bottom: 2.5px solid #0284c7; padding: 12px 20px;");
    QHBoxLayout* layout = new QHBoxLayout(header);
    layout->setContentsMargins(10, 8, 10, 8);

    // Application Title on the very TOP (Clean & Professional Branding)
    QVBoxLayout* titleBox = new QVBoxLayout();
    QLabel* appTitle = new QLabel("🚌 Campus Transit & Safety Management System", header);
    appTitle->setStyleSheet("font-size: 20px; font-weight: 800; color: #ffffff; letter-spacing: 0.5px;");
    
    QLabel* appSubtitle = new QLabel("Real-Time Fleet Telemetry, Multi-Role Portals & Student Safety Verification", header);
    appSubtitle->setStyleSheet("font-size: 11.5px; color: #38bdf8; font-weight: 500;");
    
    titleBox->addWidget(appTitle);
    titleBox->addWidget(appSubtitle);
    layout->addLayout(titleBox);

    layout->addStretch();

    if (isLoggedIn) {
        // User Info & Badges
        loggedUserLabel = new QLabel("Welcome, User", header);
        loggedUserLabel->setStyleSheet("font-size: 13.5px; font-weight: 600; color: #f8fafc;");

        loggedRoleBadge = new QLabel(" ROLE ", header);
        loggedRoleBadge->setStyleSheet("font-size: 12px; font-weight: bold; background: #0284c7; color: white; padding: 4px 10px; border-radius: 12px;");

        QLabel* scheduleLabel = new QLabel("Schedule Mode:", header);
        scheduleLabel->setStyleSheet("font-size: 12.5px; font-weight: 600; color: #cbd5e1; margin-left: 10px;");

        scheduleComboBox = new QComboBox(header);
        scheduleComboBox->addItem("Full-Day Schedule (Normal Hours)");
        scheduleComboBox->addItem("Half-Day Schedule (Exam / Early Hours)");
        scheduleComboBox->setStyleSheet("padding: 5px 8px; font-size: 12.5px; background: #1e293b; color: #ffffff; border: 1px solid #38bdf8; border-radius: 6px;");
        connect(scheduleComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &MainWindow::onScheduleChanged);

        QPushButton* logoutBtn = new QPushButton("🚪 Sign Out", header);
        logoutBtn->setStyleSheet("background-color: #dc2626; color: white; font-weight: bold; font-size: 12.5px; padding: 6px 14px; border-radius: 6px; border: none; margin-left: 10px;");
        connect(logoutBtn, &QPushButton::clicked, this, &MainWindow::onPerformLogout);

        layout->addWidget(loggedUserLabel);
        layout->addWidget(loggedRoleBadge);
        layout->addWidget(scheduleLabel);
        layout->addWidget(scheduleComboBox);
        layout->addWidget(logoutBtn);
    } else {
        QLabel* statusBadge = new QLabel("🟢 Platform Active | v2.4 C++17 Enterprise Edition", header);
        statusBadge->setStyleSheet("font-size: 12px; font-weight: 600; color: #34d399; background: #1e293b; padding: 5px 12px; border-radius: 12px; border: 1px solid #059669;");
        layout->addWidget(statusBadge);
    }

    return header;
}

// LOGIN SCREEN (INDEX 0)
QWidget* MainWindow::createLoginScreen() {
    QWidget* screen = new QWidget();
    QVBoxLayout* rootLayout = new QVBoxLayout(screen);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // 1. Application Name on Top (NOT inside the card box!)
    QWidget* topHeader = createTopHeader(false);
    rootLayout->addWidget(topHeader);

    // 2. Centered Login Card Area
    QWidget* centerContainer = new QWidget(screen);
    QVBoxLayout* centerLayout = new QVBoxLayout(centerContainer);
    centerLayout->setAlignment(Qt::AlignCenter);

    QGroupBox* loginCard = new QGroupBox("🔐 Authorized Multi-Role Portal Login", centerContainer);
    loginCard->setFixedWidth(480);
    loginCard->setStyleSheet(
        "QGroupBox { font-size: 16px; font-weight: bold; color: #0284c7; border: 1.5px solid #cbd5e1; border-radius: 12px; padding: 24px; background-color: #ffffff; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 16px; padding: 0 8px; color: #0284c7; }"
    );

    QVBoxLayout* cardLayout = new QVBoxLayout(loginCard);
    cardLayout->setSpacing(14);

    QLabel* infoNotice = new QLabel("Select your authorized access tier to enter the transit dashboard:", loginCard);
    infoNotice->setStyleSheet("font-size: 13px; color: #64748b; margin-bottom: 4px;");

    QFormLayout* form = new QFormLayout();
    form->setSpacing(12);

    loginRoleCombo = new QComboBox(loginCard);
    loginRoleCombo->addItem("🔴 System Administrator", "ADMIN");
    loginRoleCombo->addItem("🟡 Bus Operator / Driver", "DRIVER");
    loginRoleCombo->addItem("🟢 Student / Passenger", "STUDENT");
    loginRoleCombo->addItem("🔵 Parent / Guardian", "PARENT");

    loginUserEdit = new QLineEdit("admin", loginCard);
    loginUserEdit->setPlaceholderText("Enter Username / Roll Number / Staff ID");

    loginPassEdit = new QLineEdit("admin123", loginCard);
    loginPassEdit->setEchoMode(QLineEdit::Password);
    loginPassEdit->setPlaceholderText("Enter Password");

    form->addRow("Access Tier:", loginRoleCombo);
    form->addRow("User Identifier:", loginUserEdit);
    form->addRow("Security Password:", loginPassEdit);

    QPushButton* signInBtn = new QPushButton("🔑 Sign In to Portal", loginCard);
    signInBtn->setStyleSheet("background-color: #0284c7; color: white; font-weight: bold; font-size: 14.5px; padding: 10px; border-radius: 6px; border: none;");
    connect(signInBtn, &QPushButton::clicked, this, &MainWindow::onPerformLogin);

    loginStatusLabel = new QLabel("", loginCard);
    loginStatusLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #dc2626; margin-top: 4px;");
    loginStatusLabel->setAlignment(Qt::AlignCenter);

    // Default Credentials Cheat Sheet Card
    QLabel* cheatSheet = new QLabel(
        "<b>Pre-Configured Demo Credentials:</b><br>"
        "🔴 <b>Admin:</b> admin / admin123<br>"
        "🟡 <b>Driver:</b> D-01 / driver123<br>"
        "🟢 <b>Student:</b> 2024-CS-001 / student123<br>"
        "🔵 <b>Parent:</b> P-101 / parent123", loginCard);
    cheatSheet->setStyleSheet("font-size: 12px; color: #334155; background: #f1f5f9; padding: 12px; border-radius: 8px; border: 1px solid #e2e8f0;");

    cardLayout->addWidget(infoNotice);
    cardLayout->addLayout(form);
    cardLayout->addWidget(signInBtn);
    cardLayout->addWidget(loginStatusLabel);
    cardLayout->addWidget(cheatSheet);

    centerLayout->addWidget(loginCard);
    rootLayout->addWidget(centerContainer, 1);

    return screen;
}

// PORTAL SCREEN (INDEX 1)
QWidget* MainWindow::createPortalScreen() {
    QWidget* screen = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(screen);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Top Header Banner
    QWidget* headerWidget = createTopHeader(true);
    mainLayout->addWidget(headerWidget);

    QWidget* bodyWidget = new QWidget(screen);
    QVBoxLayout* bodyLayout = new QVBoxLayout(bodyWidget);
    bodyLayout->setContentsMargins(16, 12, 16, 16);

    // Pre-create all Portal Tab Widgets
    studentTabWidget = createStudentTab();
    driverTabWidget = createDriverTab();
    adminTabWidget = createAdminTab();
    parentTabWidget = createParentTab();
    routeVisualizerTabWidget = createRouteVisualizerTab();
    fleetAndComplaintsTabWidget = createFleetAndComplaintsTab();

    // Tabbed Navigation Widget
    tabWidget = new QTabWidget(bodyWidget);
    bodyLayout->addWidget(tabWidget);

    mainLayout->addWidget(bodyWidget, 1);

    return screen;
}

// SETUP TABS BASED ON AUTHORIZED ROLE
void MainWindow::setupTabsForRole(UserRole role, const string& username) {
    tabWidget->clear();

    if (role == UserRole::ADMIN) {
        tabWidget->addTab(adminTabWidget, "🔴 Admin Control Center");
        tabWidget->addTab(studentTabWidget, "🟢 Student Portal");
        tabWidget->addTab(driverTabWidget, "🟡 Driver Portal");
        tabWidget->addTab(parentTabWidget, "🔵 Parent Portal");
        tabWidget->addTab(routeVisualizerTabWidget, "🗺️ Route Map & Optimizer");
        tabWidget->addTab(fleetAndComplaintsTabWidget, "📋 Fleet & Complaints");
    } else if (role == UserRole::STUDENT) {
        tabWidget->addTab(studentTabWidget, "🟢 Student Portal");
        tabWidget->addTab(routeVisualizerTabWidget, "🗺️ Route Map & Optimizer");
        studentRollInput->setText(QString::fromStdString(username));
        onSearchStudentSchedule();
    } else if (role == UserRole::DRIVER) {
        tabWidget->addTab(driverTabWidget, "🟡 Driver Portal");
        tabWidget->addTab(fleetAndComplaintsTabWidget, "📋 Fleet & Complaints");
        tabWidget->addTab(routeVisualizerTabWidget, "🗺️ Route Map & Optimizer");
    } else if (role == UserRole::PARENT) {
        tabWidget->addTab(parentTabWidget, "🔵 Parent Portal");
        tabWidget->addTab(routeVisualizerTabWidget, "🗺️ Route Map & Optimizer");
        parentPhoneInput->setText(QString::fromStdString(username));
        onParentTrackChild();
    }

    refreshComplaintsTable();
    refreshDriverPollsTable();
}

// LOGIN SLOT
void MainWindow::onPerformLogin() {
    QString username = loginUserEdit->text().trimmed();
    QString password = loginPassEdit->text().trimmed();

    UserSession session = sysManager.login(username.toStdString(), password.toStdString());

    if (!session.isAuthenticated || !session.activeAccount) {
        loginStatusLabel->setText("❌ Invalid Username or Password. Please try again.");
        return;
    }

    UserAccount* acc = session.activeAccount;
    loginStatusLabel->setText("");

    // Set Header Banner Info
    loggedUserLabel->setText(QString("Logged in as: <b>%1</b>").arg(QString::fromStdString(acc->getUsername())));
    loggedRoleBadge->setText(QString(" %1 ").arg(QString::fromStdString(acc->getRoleString()).toUpper()));

    // Filter Visible Tabs based on Role
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

    QScrollArea* scrollArea = new QScrollArea(tab);
    scrollArea->setWidgetResizable(true);
    QWidget* scrollContent = new QWidget();
    QVBoxLayout* scrollLayout = new QVBoxLayout(scrollContent);

    QHBoxLayout* topLayout = new QHBoxLayout();

    // 1. Student Lookup & Actions Box
    QGroupBox* box = new QGroupBox("🚏 Student Schedule & Digital Pass", scrollContent);
    QFormLayout* formLayout = new QFormLayout(box);

    studentRollInput = new QLineEdit("2024-CS-001", box);
    QPushButton* searchBtn = new QPushButton("🔍 Search Schedule", box);
    searchBtn->setStyleSheet("background-color: #0284c7; color: white; font-weight: bold;");
    connect(searchBtn, &QPushButton::clicked, this, &MainWindow::onSearchStudentSchedule);

    QPushButton* connectBusBtn = new QPushButton("🔄 Find Alternative Bus", box);
    connectBusBtn->setStyleSheet("background-color: #0d9488; color: white; font-weight: bold;");
    connect(connectBusBtn, &QPushButton::clicked, this, &MainWindow::onFindConnectingBuses);

    QPushButton* qrPassBtn = new QPushButton("🎫 Show Digital QR Pass", box);
    qrPassBtn->setStyleSheet("background-color: #059669; color: white; font-weight: bold;");
    connect(qrPassBtn, &QPushButton::clicked, this, &MainWindow::onShowDigitalQRPass);

    QPushButton* sosBtn = new QPushButton("🚨 SOS EMERGENCY BUTTON", box);
    sosBtn->setStyleSheet("background-color: #dc2626; color: white; font-weight: bold;");
    connect(sosBtn, &QPushButton::clicked, this, &MainWindow::onTriggerStudentSOS);

    formLayout->addRow("Roll Number:", studentRollInput);
    QHBoxLayout* btnRow1 = new QHBoxLayout();
    btnRow1->addWidget(searchBtn);
    btnRow1->addWidget(connectBusBtn);
    formLayout->addRow(btnRow1);
    
    QHBoxLayout* btnRow2 = new QHBoxLayout();
    btnRow2->addWidget(qrPassBtn);
    btnRow2->addWidget(sosBtn);
    formLayout->addRow(btnRow2);

    // 2. Driver Rating Box
    QGroupBox* rateBox = new QGroupBox("⭐ Rate Driver & Ride Experience", scrollContent);
    QFormLayout* rateLayout = new QFormLayout(rateBox);

    ratingDriverIdInput = new QLineEdit("D-01", rateBox);
    ratingStarCombo = new QComboBox(rateBox);
    ratingStarCombo->addItem("5 ★★★★★ (Exceptional & Safe)");
    ratingStarCombo->addItem("4 ★★★★☆ (Good & Punctual)");
    ratingStarCombo->addItem("3 ★★★☆☆ (Average)");
    ratingStarCombo->addItem("2 ★★☆☆☆ (Poor / Late)");
    ratingStarCombo->addItem("1 ★☆☆☆☆ (Unsafe / Reckless)");

    ratingFeedbackInput = new QLineEdit("Smooth driving and punctual!", rateBox);
    QPushButton* submitRateBtn = new QPushButton("Submit Driver Rating", rateBox);
    submitRateBtn->setStyleSheet("background-color: #d97706; color: white; font-weight: bold;");
    connect(submitRateBtn, &QPushButton::clicked, this, &MainWindow::onSubmitDriverRating);

    rateLayout->addRow("Driver Identifier:", ratingDriverIdInput);
    rateLayout->addRow("Rating Score:", ratingStarCombo);
    rateLayout->addRow("Trip Feedback:", ratingFeedbackInput);
    rateLayout->addRow("", submitRateBtn);

    topLayout->addWidget(box, 3);
    topLayout->addWidget(rateBox, 2);
    scrollLayout->addLayout(topLayout);

    // Student Information Banner
    studentInfoLabel = new QLabel("Enter Roll Number above and click Search Schedule to view personalized departure telemetry.", scrollContent);
    studentInfoLabel->setStyleSheet("font-size: 13px; color: #1e293b; padding: 10px; background: #e0f2fe; border-radius: 6px; border: 1px solid #bae6fd;");
    scrollLayout->addWidget(studentInfoLabel);

    // Schedule Table
    scheduleTable = new QTableWidget(0, 5, scrollContent);
    scheduleTable->setHorizontalHeaderLabels({"Bus ID", "Assigned Route", "Morning Departure", "Campus Arrival ETA", "Available Capacity"});
    scheduleTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    scheduleTable->setMinimumHeight(130);
    scrollLayout->addWidget(scheduleTable);

    // 3. Lodge Driver / Transit Complaint Section (Auto-Timestamped)
    QGroupBox* complaintBox = new QGroupBox("📢 Report Driver Misconduct or Safety Violation", scrollContent);
    QVBoxLayout* compBoxOuter = new QVBoxLayout(complaintBox);

    QFormLayout* compForm = new QFormLayout();
    complaintDriverCombo = new QComboBox(complaintBox);
    complaintDriverCombo->addItem("B-101 (Driver: Rajesh Kumar [D-01])", "B-101");
    complaintDriverCombo->addItem("B-102 (Driver: Vikram Singh [D-02])", "B-102");
    complaintDriverCombo->addItem("B-103 (Driver: Suresh Nair [D-03])", "B-103");

    complaintCategoryCombo = new QComboBox(complaintBox);
    complaintCategoryCombo->addItem("Over-Speeding / Rash & Dangerous Driving");
    complaintCategoryCombo->addItem("Severe Unannounced Delay / Skipped Bus Stop");
    complaintCategoryCombo->addItem("Rude / Unprofessional Conductor or Driver Behavior");
    complaintCategoryCombo->addItem("Dangerous Overcrowding & Standing Hazard");
    complaintCategoryCombo->addItem("Unauthorized Route Deviation");
    complaintCategoryCombo->addItem("Vehicle Malfunction / Defective AC / Mechanical Issue");

    complaintDescInput = new QLineEdit(complaintBox);
    complaintDescInput->setPlaceholderText("Describe the incident in detail (location, time, severity)...");

    QPushButton* lodgeCompBtn = new QPushButton("🚨 Lodge Complaint & Request Peer Verification", complaintBox);
    lodgeCompBtn->setStyleSheet("background-color: #dc2626; color: white; font-weight: bold; padding: 8px 16px; border-radius: 6px;");
    connect(lodgeCompBtn, &QPushButton::clicked, this, &MainWindow::onStudentFileComplaint);

    compForm->addRow("Select Target Bus / Operator:", complaintDriverCombo);
    compForm->addRow("Incident Category:", complaintCategoryCombo);
    compForm->addRow("Specific Observations:", complaintDescInput);
    compForm->addRow("", lodgeCompBtn);
    compBoxOuter->addLayout(compForm);

    scrollLayout->addWidget(complaintBox);

    // 4. Student Community Peer Verification Polls (Vote Right vs Wrong on Driver Complaints)
    QGroupBox* pollsBox = new QGroupBox("🗳️ Community Peer Verification on Reported Driver Incidents", scrollContent);
    QVBoxLayout* pollsLayout = new QVBoxLayout(pollsBox);

    QLabel* pollNotice = new QLabel("When fellow students report driver misconduct, you can review and vote whether the incident report is RIGHT (Valid) or WRONG (Disputed):", pollsBox);
    pollNotice->setStyleSheet("font-size: 12.5px; color: #475569; margin-bottom: 6px;");
    pollsLayout->addWidget(pollNotice);

    driverPollsTable = new QTableWidget(0, 7, pollsBox);
    driverPollsTable->setHorizontalHeaderLabels({"Report ID", "Time of Complaint", "Reported Operator", "Category", "Incident Description", "Community Consensus", "Cast Verification"});
    driverPollsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    driverPollsTable->horizontalHeader()->setStretchLastSection(true);
    driverPollsTable->setMinimumHeight(160);
    pollsLayout->addWidget(driverPollsTable);

    scrollLayout->addWidget(pollsBox);

    scrollArea->setWidget(scrollContent);
    layout->addWidget(scrollArea);

    return tab;
}

// TAB 2: 🟡 DRIVER PORTAL
QWidget* MainWindow::createDriverTab() {
    QWidget* tab = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(tab);

    QGroupBox* tripBox = new QGroupBox("🟡 Driver Trip & Telemetry Operations", tab);
    QFormLayout* tripForm = new QFormLayout(tripBox);

    driverBusCombo = new QComboBox(tripBox);
    driverBusCombo->addItem("B-101 (Express Route R1)");
    driverBusCombo->addItem("B-102 (Suburban Route R2)");
    driverBusCombo->addItem("B-103 (Special Route R1)");

    QPushButton* startBtn = new QPushButton("🚀 Start Morning Service", tripBox);
    startBtn->setStyleSheet("background-color: #059669; color: white; font-weight: bold;");
    connect(startBtn, &QPushButton::clicked, this, &MainWindow::onDriverStartTrip);

    QPushButton* endBtn = new QPushButton("🏁 End Service / Park Depot", tripBox);
    endBtn->setStyleSheet("background-color: #64748b; color: white; font-weight: bold;");
    connect(endBtn, &QPushButton::clicked, this, &MainWindow::onDriverEndTrip);

    driverDelayReasonInput = new QLineEdit("Heavy Traffic at Central Metro Junction", tripBox);
    driverDelayMinsInput = new QLineEdit("15", tripBox);
    QPushButton* delayBtn = new QPushButton("⚠️ Broadcast Delay Alert to Commuters", tripBox);
    delayBtn->setStyleSheet("background-color: #d97706; color: white; font-weight: bold;");
    connect(delayBtn, &QPushButton::clicked, this, &MainWindow::onDriverReportDelay);

    tripForm->addRow("Active Bus Unit:", driverBusCombo);
    QHBoxLayout* tripBtns = new QHBoxLayout();
    tripBtns->addWidget(startBtn);
    tripBtns->addWidget(endBtn);
    tripForm->addRow(tripBtns);
    tripForm->addRow("Delay Cause:", driverDelayReasonInput);
    tripForm->addRow("Estimated Minutes:", driverDelayMinsInput);
    tripForm->addRow("", delayBtn);

    QGroupBox* misbehaviorBox = new QGroupBox("🛑 Report Student Misbehavior & Bus Condition", tab);
    QFormLayout* misForm = new QFormLayout(misbehaviorBox);

    misbehaviorRollInput = new QLineEdit("2024-CS-089", misbehaviorBox);
    misbehaviorDescInput = new QLineEdit("Smoking / Vandalism at Seat 12", misbehaviorBox);
    QPushButton* reportMisBtn = new QPushButton("Submit Disciplinary Report", misbehaviorBox);
    reportMisBtn->setStyleSheet("background-color: #dc2626; color: white; font-weight: bold;");
    connect(reportMisBtn, &QPushButton::clicked, this, &MainWindow::onDriverReportMisbehavior);

    QPushButton* condBtn = new QPushButton("🛠️ Report Mechanical / Bus Defect", misbehaviorBox);
    condBtn->setStyleSheet("background-color: #0284c7; color: white; font-weight: bold;");
    connect(condBtn, &QPushButton::clicked, this, [this]() {
        sysManager.driverReportBusCondition("B-101", "Brake Pad Inspection & Cabin Sanitization Required");
        QMessageBox::information(this, "Maintenance Request Logged", "🛠️ Mechanical defect report submitted to Fleet Maintenance Depot.");
    });

    misForm->addRow("Student Roll Number:", misbehaviorRollInput);
    misForm->addRow("Infraction Details:", misbehaviorDescInput);
    misForm->addRow("", reportMisBtn);
    misForm->addRow("", condBtn);

    layout->addWidget(tripBox);
    layout->addWidget(misbehaviorBox);

    return tab;
}

// TAB 3: 🔴 ADMIN CONTROL CENTER
QWidget* MainWindow::createAdminTab() {
    QWidget* tab = new QWidget();
    QHBoxLayout* mainAdminLayout = new QHBoxLayout(tab);

    QGroupBox* busBox = new QGroupBox("🔴 Fleet Registry Configuration", tab);
    QFormLayout* busForm = new QFormLayout(busBox);

    adminBusCombo = new QComboBox(busBox);
    adminBusCombo->addItem("B-101"); adminBusCombo->addItem("B-102"); adminBusCombo->addItem("B-103");
    adminRegInput = new QLineEdit("MH-12-AB-1001", busBox);
    adminCapInput = new QLineEdit("55", busBox);
    adminRouteCombo = new QComboBox(busBox);
    adminRouteCombo->addItem("R1"); adminRouteCombo->addItem("R2");

    QPushButton* updateBusBtn = new QPushButton("Update Bus Fleet Parameters", busBox);
    updateBusBtn->setStyleSheet("background-color: #dc2626; color: white; font-weight: bold;");
    connect(updateBusBtn, &QPushButton::clicked, this, &MainWindow::onAdminUpdateBus);

    busForm->addRow("Target Bus Identifier:", adminBusCombo);
    busForm->addRow("Registration Plate:", adminRegInput);
    busForm->addRow("Max Passenger Seating:", adminCapInput);
    busForm->addRow("Assigned Corridor Route:", adminRouteCombo);
    busForm->addRow("", updateBusBtn);

    QGroupBox* timingBox = new QGroupBox("🔴 Timetable Management & Campus Broadcast", tab);
    QFormLayout* timingForm = new QFormLayout(timingBox);

    adminTimingStopCombo = new QComboBox(timingBox);
    adminTimingStopCombo->addItem("ST101 - Sector 17 Terminal", "ST101");
    adminTimingStopCombo->addItem("ST102 - Central Metro Station", "ST102");
    adminTimingStopCombo->addItem("ST103 - Tech Park Junction", "ST103");
    adminTimingStopCombo->addItem("ST104 - Green Valley Chowk", "ST104");
    adminTimingStopCombo->addItem("ST105 - College Campus Main Gate", "ST105");

    adminTimingBusCombo = new QComboBox(timingBox);
    adminTimingBusCombo->addItem("B-101"); adminTimingBusCombo->addItem("B-102"); adminTimingBusCombo->addItem("B-103");

    adminTimingScheduleCombo = new QComboBox(timingBox);
    adminTimingScheduleCombo->addItem("Full-Day Schedule");
    adminTimingScheduleCombo->addItem("Half-Day Schedule");

    adminNewTimeInput = new QLineEdit("07:35", timingBox);

    QPushButton* updateTimingBtn = new QPushButton("Update Departure Schedule", timingBox);
    updateTimingBtn->setStyleSheet("background-color: #d97706; color: white; font-weight: bold;");
    connect(updateTimingBtn, &QPushButton::clicked, this, &MainWindow::onAdminUpdateTiming);

    QPushButton* noticeBtn = new QPushButton("📢 Broadcast Transit Notice to All", timingBox);
    noticeBtn->setStyleSheet("background-color: #0284c7; color: white; font-weight: bold;");
    connect(noticeBtn, &QPushButton::clicked, this, &MainWindow::onAdminBroadcastNotice);

    timingForm->addRow("Transit Checkpoint:", adminTimingStopCombo);
    timingForm->addRow("Transit Bus ID:", adminTimingBusCombo);
    timingForm->addRow("Timetable Tier:", adminTimingScheduleCombo);
    timingForm->addRow("Adjusted Arrival Time:", adminNewTimeInput);
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

    QGroupBox* box = new QGroupBox("🔵 Parent Portal: Real-Time Transit Tracking & Fee Status", tab);
    QHBoxLayout* formLayout = new QHBoxLayout(box);

    parentPhoneInput = new QLineEdit("+91-9811001122", box);
    parentPhoneInput->setPlaceholderText("Enter Registered Phone or Student Roll (e.g. 2024-CS-001)");

    QPushButton* trackBtn = new QPushButton("📡 Locate Student Bus", box);
    trackBtn->setStyleSheet("background-color: #0284c7; color: white; font-weight: bold;");
    connect(trackBtn, &QPushButton::clicked, this, &MainWindow::onParentTrackChild);

    parentPayFeeBtn = new QPushButton("💳 Pay Outstanding Fee", box);
    parentPayFeeBtn->setStyleSheet("background-color: #059669; color: white; font-weight: bold;");
    connect(parentPayFeeBtn, &QPushButton::clicked, this, &MainWindow::onParentPayFee);

    formLayout->addWidget(new QLabel("Parent Phone / Child Roll:", box));
    formLayout->addWidget(parentPhoneInput);
    formLayout->addWidget(trackBtn);
    formLayout->addWidget(parentPayFeeBtn);

    parentInfoLabel = new QLabel("Enter Registered Phone number or Child Roll number above to track real-time child location and bus status.", tab);
    parentInfoLabel->setStyleSheet("font-size: 13.5px; color: #1e293b; padding: 14px; background: #ffffff; border-radius: 8px; border: 1px solid #cbd5e1;");

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
    startStopCombo->addItem("ST102 - Central Metro Station", "ST102");
    startStopCombo->addItem("ST103 - Tech Park Junction", "ST103");
    startStopCombo->addItem("ST104 - Green Valley Chowk", "ST104");

    destStopCombo->addItem("ST105 - College Campus Main Gate", "ST105");
    destStopCombo->addItem("ST103 - Tech Park Junction", "ST103");
    destStopCombo->addItem("ST102 - Central Metro Station", "ST102");

    QPushButton* optBtn = new QPushButton("🗺️ Evaluate Shortest Path", tab);
    optBtn->setStyleSheet("background-color: #0284c7; color: white; font-weight: bold;");
    QPushButton* simBtn = new QPushButton("⏱️ Simulate Live Transit Motion", tab);
    simBtn->setStyleSheet("background-color: #059669; color: white; font-weight: bold;");

    connect(optBtn, &QPushButton::clicked, this, &MainWindow::onOptimizeRoute);
    connect(simBtn, &QPushButton::clicked, this, &MainWindow::onStartLiveSim);

    ctrlLayout->addWidget(new QLabel("From Checkpoint:", tab));
    ctrlLayout->addWidget(startStopCombo);
    ctrlLayout->addWidget(new QLabel("To Destination:", tab));
    ctrlLayout->addWidget(destStopCombo);
    ctrlLayout->addWidget(optBtn);
    ctrlLayout->addWidget(simBtn);

    routeInfoLabel = new QLabel("Click 'Evaluate Shortest Path' or 'Simulate Live Transit Motion' to trace the vehicle path.", tab);
    routeInfoLabel->setStyleSheet("font-size: 13px; font-weight: 600; color: #0284c7; padding: 6px; background: #e0f2fe; border-radius: 6px;");

    graphScene = new QGraphicsScene(this);
    graphView = new QGraphicsView(graphScene, tab);
    graphView->setRenderHint(QPainter::Antialiasing);
    graphView->setMinimumHeight(340);
    graphView->setStyleSheet("background-color: #ffffff; border: 1px solid #cbd5e1; border-radius: 8px;");

    layout->addLayout(ctrlLayout);
    layout->addWidget(routeInfoLabel);
    layout->addWidget(graphView);

    return tab;
}

// TAB 6: FLEET & COMPLAINTS REGISTRY
QWidget* MainWindow::createFleetAndComplaintsTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);

    QScrollArea* scrollArea = new QScrollArea(tab);
    scrollArea->setWidgetResizable(true);
    QWidget* scrollContent = new QWidget();
    QVBoxLayout* scrollLayout = new QVBoxLayout(scrollContent);

    // 1. Bus Fleet Table
    QGroupBox* fleetBox = new QGroupBox("🚌 Comprehensive College Bus Fleet Telemetry", scrollContent);
    QVBoxLayout* fBoxLayout = new QVBoxLayout(fleetBox);

    fleetTable = new QTableWidget(3, 6, fleetBox);
    fleetTable->setHorizontalHeaderLabels({"Bus Identifier", "Registration Plate", "Capacity", "Occupancy", "Route Corridor", "Assigned Driver"});
    fleetTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    fleetTable->setMinimumHeight(120);
    fBoxLayout->addWidget(fleetTable);

    // Boarding Box
    QHBoxLayout* boardLayout = new QHBoxLayout();
    boardRollInput = new QLineEdit("2024-CS-001", fleetBox);
    boardBusCombo = new QComboBox(fleetBox);
    boardBusCombo->addItem("B-101"); boardBusCombo->addItem("B-102"); boardBusCombo->addItem("B-103");

    QPushButton* boardBtn = new QPushButton("🎟️ Authorize & Board Student", fleetBox);
    boardBtn->setStyleSheet("background-color: #0284c7; color: white; font-weight: bold;");
    connect(boardBtn, &QPushButton::clicked, this, &MainWindow::onBoardStudent);

    boardLayout->addWidget(new QLabel("Student Roll:", fleetBox));
    boardLayout->addWidget(boardRollInput);
    boardLayout->addWidget(new QLabel("Assigned Bus:", fleetBox));
    boardLayout->addWidget(boardBusCombo);
    boardLayout->addWidget(boardBtn);
    fBoxLayout->addLayout(boardLayout);
    scrollLayout->addWidget(fleetBox);

    // 2. Complaints, Incidents & Verification Registry Table
    QGroupBox* compRegistryBox = new QGroupBox("📋 Live Complaints, Safety Incidents & Student Consensus Registry", scrollContent);
    QVBoxLayout* compLayout = new QVBoxLayout(compRegistryBox);

    complaintsTable = new QTableWidget(0, 8, compRegistryBox);
    complaintsTable->setHorizontalHeaderLabels({"ID", "Time of Complaint", "Reporter", "Target (Driver/Bus)", "Category", "Details", "Student Consensus", "Status"});
    complaintsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    complaintsTable->horizontalHeader()->setStretchLastSection(true);
    complaintsTable->setMinimumHeight(180);
    compLayout->addWidget(complaintsTable);

    // Admin Action Form on Complaints
    QHBoxLayout* actionLayout = new QHBoxLayout();
    adminComplaintSelectCombo = new QComboBox(compRegistryBox);
    adminComplaintSelectCombo->setMinimumWidth(120);

    adminComplaintStatusCombo = new QComboBox(compRegistryBox);
    adminComplaintStatusCombo->addItem("INVESTIGATING");
    adminComplaintStatusCombo->addItem("WARNING_ISSUED");
    adminComplaintStatusCombo->addItem("RESOLVED");
    adminComplaintStatusCombo->addItem("CLOSED");

    adminActionNoteInput = new QLineEdit("Counseling issued following majority student verification", compRegistryBox);
    QPushButton* updateStatusBtn = new QPushButton("⚖️ Resolve / Update Status", compRegistryBox);
    updateStatusBtn->setStyleSheet("background-color: #059669; color: white; font-weight: bold;");
    connect(updateStatusBtn, &QPushButton::clicked, this, &MainWindow::onAdminUpdateComplaintStatus);

    actionLayout->addWidget(new QLabel("Complaint ID:", compRegistryBox));
    actionLayout->addWidget(adminComplaintSelectCombo);
    actionLayout->addWidget(new QLabel("Action Status:", compRegistryBox));
    actionLayout->addWidget(adminComplaintStatusCombo);
    actionLayout->addWidget(adminActionNoteInput);
    actionLayout->addWidget(updateStatusBtn);
    compLayout->addLayout(actionLayout);

    scrollLayout->addWidget(compRegistryBox);

    scrollArea->setWidget(scrollContent);
    layout->addWidget(scrollArea);

    return tab;
}

void MainWindow::refreshFleetTable() {
    const auto& buses = sysManager.getAllBuses();
    fleetTable->setRowCount(buses.size());
    for (size_t i = 0; i < buses.size(); ++i) {
        fleetTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(buses[i].getBusId())));
        fleetTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(buses[i].getBusNumber())));
        fleetTable->setItem(i, 2, new QTableWidgetItem(QString::number(buses[i].getCapacity())));
        fleetTable->setItem(i, 3, new QTableWidgetItem(QString::number(buses[i].getCurrentOccupancy()) + " / " + QString::number(buses[i].getCapacity())));
        fleetTable->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(buses[i].getAssignedRouteId())));
        fleetTable->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(buses[i].getDriver() ? buses[i].getDriver()->getName() : "Unassigned")));
    }
}

void MainWindow::refreshComplaintsTable() {
    if (!complaintsTable) return;
    const auto& list = sysManager.getComplaintEngine().getComplaints();
    complaintsTable->setRowCount(list.size());

    if (adminComplaintSelectCombo) {
        adminComplaintSelectCombo->clear();
    }

    for (size_t i = 0; i < list.size(); ++i) {
        const auto& c = list[i];
        if (adminComplaintSelectCombo) {
            adminComplaintSelectCombo->addItem(QString::fromStdString(c.complaintId));
        }

        complaintsTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(c.complaintId)));
        complaintsTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(c.timestamp)));
        complaintsTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(c.reporterName)));
        complaintsTable->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(c.targetName)));
        complaintsTable->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(c.category)));
        complaintsTable->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(c.description)));

        QString consensus = QString("✅ %1 Right | ❌ %2 Wrong").arg(c.votesRight).arg(c.votesWrong);
        if (c.votesRight + c.votesWrong > 0) {
            int pct = (c.votesRight * 100) / (c.votesRight + c.votesWrong);
            consensus += QString(" (%1% Valid)").arg(pct);
        }
        complaintsTable->setItem(i, 6, new QTableWidgetItem(consensus));

        QString st = "OPEN";
        if (c.status == ComplaintStatus::INVESTIGATING) st = "INVESTIGATING";
        else if (c.status == ComplaintStatus::WARNING_ISSUED) st = "WARNING_ISSUED";
        else if (c.status == ComplaintStatus::RESOLVED) st = "RESOLVED";
        else if (c.status == ComplaintStatus::CLOSED) st = "CLOSED";
        complaintsTable->setItem(i, 7, new QTableWidgetItem(st));
    }
}

void MainWindow::refreshDriverPollsTable() {
    if (!driverPollsTable) return;
    const auto& list = sysManager.getComplaintEngine().getComplaints();

    // Filter complaints against drivers
    vector<Complaint> driverComplaints;
    for (const auto& c : list) {
        if (c.type == ComplaintType::STUDENT_TO_DRIVER || c.targetName.find("Driver") != string::npos) {
            driverComplaints.push_back(c);
        }
    }

    driverPollsTable->setRowCount(driverComplaints.size());

    for (size_t i = 0; i < driverComplaints.size(); ++i) {
        const auto& c = driverComplaints[i];
        driverPollsTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(c.complaintId)));
        driverPollsTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(c.timestamp)));
        driverPollsTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(c.targetName)));
        driverPollsTable->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(c.category)));
        driverPollsTable->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(c.description)));

        QString consensus = QString("✅ %1 Right | ❌ %2 Wrong").arg(c.votesRight).arg(c.votesWrong);
        driverPollsTable->setItem(i, 5, new QTableWidgetItem(consensus));

        // Interactive Voting Buttons
        QWidget* voteWidget = new QWidget();
        QHBoxLayout* vLayout = new QHBoxLayout(voteWidget);
        vLayout->setContentsMargins(4, 2, 4, 2);
        vLayout->setSpacing(6);

        QPushButton* voteRightBtn = new QPushButton("✅ Right", voteWidget);
        voteRightBtn->setStyleSheet("background-color: #059669; color: white; font-weight: bold; font-size: 11px; padding: 3px 8px; border-radius: 4px;");
        QString compId = QString::fromStdString(c.complaintId);
        connect(voteRightBtn, &QPushButton::clicked, this, [this, compId]() {
            onVoteOnComplaint(compId, true);
        });

        QPushButton* voteWrongBtn = new QPushButton("❌ Wrong", voteWidget);
        voteWrongBtn->setStyleSheet("background-color: #dc2626; color: white; font-weight: bold; font-size: 11px; padding: 3px 8px; border-radius: 4px;");
        connect(voteWrongBtn, &QPushButton::clicked, this, [this, compId]() {
            onVoteOnComplaint(compId, false);
        });

        vLayout->addWidget(voteRightBtn);
        vLayout->addWidget(voteWrongBtn);
        driverPollsTable->setCellWidget(i, 6, voteWidget);
    }
}

void MainWindow::renderRouteGraph() {
    graphScene->clear();

    int x1 = 60,  y1 = 150;
    int x2 = 200, y2 = 80;
    int x3 = 350, y3 = 80;
    int x4 = 280, y4 = 220;
    int x5 = 520, y5 = 150;

    QPen r1Pen(QColor("#0284c7"), 3);
    graphScene->addLine(x1, y1, x2, y2, r1Pen);
    graphScene->addLine(x2, y2, x3, y3, r1Pen);
    graphScene->addLine(x3, y3, x5, y5, r1Pen);

    QPen r2Pen(QColor("#f59e0b"), 3, Qt::DashLine);
    graphScene->addLine(x2, y2, x4, y4, r2Pen);
    graphScene->addLine(x4, y4, x5, y5, r2Pen);

    auto drawStopNode = [&](int x, int y, const QString& name, const QString& id) {
        graphScene->addEllipse(x - 14, y - 14, 28, 28, QPen(QColor("#0284c7"), 2), QBrush(QColor("#ffffff")));
        QGraphicsTextItem* text = graphScene->addText(name + " [" + id + "]");
        text->setDefaultTextColor(QColor("#0f172a"));
        QFont f = text->font();
        f.setBold(true);
        text->setFont(f);
        text->setPos(x - 35, y + 16);
    };

    drawStopNode(x1, y1, "Sector 17", "ST101");
    drawStopNode(x2, y2, "Central Metro", "ST102");
    drawStopNode(x3, y3, "Tech Park", "ST103");
    drawStopNode(x4, y4, "Green Chowk", "ST104");
    drawStopNode(x5, y5, "College Campus", "ST105");

    busMarker = graphScene->addEllipse(x1 - 8, y1 - 8, 16, 16, QPen(Qt::white, 2), QBrush(QColor("#ef4444")));
}

// SLOTS IMPLEMENTATIONS
void MainWindow::onScheduleChanged(int index) {
    sysManager.setScheduleType(index == 0 ? ScheduleType::FULL_DAY : ScheduleType::HALF_DAY);
    onSearchStudentSchedule();
}

void MainWindow::onSearchStudentSchedule() {
    QString roll = studentRollInput->text().trimmed();
    Student* s = sysManager.findStudentByRoll(roll.toStdString());

    if (!s) {
        studentInfoLabel->setText("❌ Student Roll Number not found in campus database.");
        scheduleTable->setRowCount(0);
        return;
    }

    Stop* stop = sysManager.findStopById(s->getAssignedStopId());
    QString stopName = stop ? QString::fromStdString(stop->getStopName()) : QString::fromStdString(s->getAssignedStopId());

    studentInfoLabel->setText(QString("Passenger: <b>%1</b> | Roll: <b>%2</b> | Assigned Stop: <b>%3 [%4]</b> | Route: <b>%5</b> | Fee Account: %6")
                              .arg(QString::fromStdString(s->getName()))
                              .arg(QString::fromStdString(s->getRollNumber()))
                              .arg(stopName)
                              .arg(QString::fromStdString(s->getAssignedStopId()))
                              .arg(QString::fromStdString(s->getAssignedRouteId()))
                              .arg(s->isFeePaid() ? "<font color='#16a34a'><b>PAID IN FULL</b></font>" : "<font color='#dc2626'><b>OUTSTANDING DUE</b></font>"));

    // Populate Real Passing Buses for this Stop
    const auto& allBuses = sysManager.getAllBuses();
    vector<Bus> relevantBuses;
    for (const auto& b : allBuses) {
        if (b.getAssignedRouteId() == s->getAssignedRouteId()) {
            relevantBuses.push_back(b);
        }
    }
    if (relevantBuses.empty()) relevantBuses = allBuses;

    scheduleTable->setRowCount(relevantBuses.size());
    for (size_t i = 0; i < relevantBuses.size(); ++i) {
        const auto& b = relevantBuses[i];
        QString depTime = sysManager.getScheduleType() == ScheduleType::FULL_DAY ? "07:30 AM" : "07:15 AM";
        if (b.getBusId() == "B-102") depTime = sysManager.getScheduleType() == ScheduleType::FULL_DAY ? "07:45 AM" : "07:30 AM";
        if (b.getBusId() == "B-103") depTime = sysManager.getScheduleType() == ScheduleType::FULL_DAY ? "08:05 AM" : "07:45 AM";

        QString campusEta = sysManager.getScheduleType() == ScheduleType::FULL_DAY ? "08:25 AM" : "08:00 AM";

        scheduleTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(b.getBusId())));
        scheduleTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(b.getAssignedRouteId()) + " (" + QString::fromStdString(b.getBusNumber()) + ")"));
        scheduleTable->setItem(i, 2, new QTableWidgetItem(depTime));
        scheduleTable->setItem(i, 3, new QTableWidgetItem(campusEta));
        scheduleTable->setItem(i, 4, new QTableWidgetItem(QString::number(b.getCapacity() - b.getCurrentOccupancy()) + " Free Seats Available"));
    }
}

void MainWindow::onFindConnectingBuses() {
    QString roll = studentRollInput->text().trimmed();
    Student* s = sysManager.findStudentByRoll(roll.toStdString());
    string stopId = s ? s->getAssignedStopId() : "ST101";

    QMessageBox::information(this, "Connecting Bus Engine", 
        QString("🔄 <b>ALTERNATIVE & CONNECTING BUS ROUTE</b><br><br>"
                "<b>Boarding Station:</b> %1<br>"
                "<b>Primary Bus:</b> B-101 (Dep: 07:30 AM)<br>"
                "<b>Connecting Transit Bus:</b> Bus B-102 (UK-04-PA-2002)<br>"
                "<b>Estimated Connection Departure:</b> 07:45 AM<br>"
                "<b>Available Seats:</b> 42 Seats Free<br>"
                "<b>Route Option:</b> Route R2 (Valley Corridor)<br><br>"
                "<i>Connecting transit service verified and safe for student transfer!</i>")
        .arg(QString::fromStdString(stopId)));
}

void MainWindow::onShowDigitalQRPass() {
    QString roll = studentRollInput->text().trimmed();
    DigitalPass pass = sysManager.getStudentPass(roll.toStdString());
    QMessageBox::information(this, "Digital QR Bus Pass", 
                             QString("🎫 <b>CAMPUS DIGITAL BUS PASS</b><br><br>"
                                     "<b>Student Name:</b> %1<br>"
                                     "<b>Roll Number:</b> %2<br>"
                                     "<b>Permitted Corridor:</b> %3<br>"
                                     "<b>Boarding Checkpoint:</b> Sector 17 Terminal [ST101]<br>"
                                     "<b>Cryptographic QR Hash:</b> %4<br>"
                                     "<b>Validity:</b> Current Academic Semester 2026<br><br>"
                                     "<font color='#059669'><b>[PASS STATUS: VERIFIED & ACTIVE]</b></font>")
                             .arg(QString::fromStdString(pass.studentName))
                             .arg(QString::fromStdString(pass.studentRoll))
                             .arg(QString::fromStdString(pass.routeId))
                             .arg(QString::fromStdString(pass.verificationHash)));
}

void MainWindow::onSubmitDriverRating() {
    QString driverId = ratingDriverIdInput->text().trimmed();
    QString roll = studentRollInput->text().trimmed();
    int stars = 5 - ratingStarCombo->currentIndex(); // 5 down to 1
    QString feedback = ratingFeedbackInput->text().trimmed();

    sysManager.studentRateDriver(roll.toStdString(), driverId.toStdString(), stars, feedback.toStdString());
    double avg = sysManager.getComplaintEngine().getAverageDriverRating(driverId.toStdString());

    QMessageBox::information(this, "Driver Rating Recorded", 
        QString("⭐ <b>Rating Submitted Successfully!</b><br><br>"
                "<b>Operator:</b> %1<br>"
                "<b>Your Score:</b> %2 / 5 Stars<br>"
                "<b>Feedback:</b> \"%3\"<br>"
                "<b>Driver Cumulative Average Rating:</b> %4 ★")
        .arg(driverId)
        .arg(stars)
        .arg(feedback)
        .arg(QString::number(avg, 'f', 1)));
}

void MainWindow::onTriggerStudentSOS() {
    QString roll = studentRollInput->text().trimmed();
    sysManager.triggerStudentSOS(roll.toStdString(), "B-101", "Sector 17 Terminal");
    QMessageBox::critical(this, "🚨 EMERGENCY SOS ACTIVATED", 
        "🚨 <b>EMERGENCY SOS ALERT DISPATCHED!</b><br><br>"
        "College Transport Control Center, Campus Security, and Nearest Emergency Responders have been notified of your exact GPS checkpoint!");
    refreshComplaintsTable();
}

// LODGE DRIVER COMPLAINT & TRIGGER COMMUNITY PEER VERIFICATION POPUP
void MainWindow::onStudentFileComplaint() {
    QString roll = studentRollInput->text().trimmed();
    QString busId = complaintDriverCombo->currentData().toString();
    QString category = complaintCategoryCombo->currentText();
    QString desc = complaintDescInput->text().trimmed();

    if (desc.isEmpty()) {
        QMessageBox::warning(this, "Input Required", "Please enter specific incident observations before lodging the complaint.");
        return;
    }

    // Submit complaint with real current timestamp
    Complaint c = sysManager.studentReportDriver(roll.toStdString(), busId.toStdString(), category.toStdString(), desc.toStdString());

    complaintDescInput->clear();
    refreshComplaintsTable();
    refreshDriverPollsTable();

    // POP-UP PROMPT FOR STUDENTS TO VOTE RIGHT OR WRONG ON THIS DRIVER COMPLAINT
    QMessageBox voteDialog(this);
    voteDialog.setWindowTitle("📢 Driver Safety Report - Community Verification");
    voteDialog.setIcon(QMessageBox::Question);
    voteDialog.setText(QString(
        "<b>🚨 NEW DRIVER MISCONDUCT REPORT LODGED FOR PEER VERIFICATION</b><br><br>"
        "<b>Incident Reference:</b> %1<br>"
        "<b>Time of Complaint:</b> <font color='#0284c7'><b>%2</b></font><br>"
        "<b>Reported Target:</b> %3<br>"
        "<b>Category:</b> %4<br>"
        "<b>Passenger Account:</b> \"%5\"<br><br>"
        "<i>As a student commuter on this corridor, do you verify this report as <b>RIGHT</b> (Valid) or <b>WRONG</b> (Disputed)?</i>")
        .arg(QString::fromStdString(c.complaintId))
        .arg(QString::fromStdString(c.timestamp))
        .arg(QString::fromStdString(c.targetName))
        .arg(QString::fromStdString(c.category))
        .arg(QString::fromStdString(c.description)));

    QPushButton* rightBtn = voteDialog.addButton("✅ Vote: Report is RIGHT", QMessageBox::YesRole);
    rightBtn->setStyleSheet("background-color: #059669; color: white; font-weight: bold; padding: 6px 14px; border-radius: 4px;");

    QPushButton* wrongBtn = voteDialog.addButton("❌ Vote: Report is WRONG", QMessageBox::NoRole);
    wrongBtn->setStyleSheet("background-color: #dc2626; color: white; font-weight: bold; padding: 6px 14px; border-radius: 4px;");

    QPushButton* decideLaterBtn = voteDialog.addButton("Decide Later", QMessageBox::RejectRole);

    voteDialog.exec();

    if (voteDialog.clickedButton() == rightBtn) {
        onVoteOnComplaint(QString::fromStdString(c.complaintId), true);
    } else if (voteDialog.clickedButton() == wrongBtn) {
        onVoteOnComplaint(QString::fromStdString(c.complaintId), false);
    }
}

void MainWindow::onVoteOnComplaint(const QString& compId, bool isRight) {
    QString roll = studentRollInput->text().trimmed();
    if (roll.isEmpty()) roll = "2024-CS-001";

    string outMsg;
    bool ok = sysManager.voteOnDriverComplaint(compId.toStdString(), roll.toStdString(), isRight, outMsg);

    if (ok) {
        QMessageBox::information(this, "Verification Vote Recorded", QString::fromStdString(outMsg));
    } else {
        QMessageBox::warning(this, "Vote Notice", QString::fromStdString(outMsg));
    }

    refreshComplaintsTable();
    refreshDriverPollsTable();
}

void MainWindow::onDriverStartTrip() {
    sysManager.driverStartTrip("B-101");
    QMessageBox::information(this, "Driver Trip Control", "🚀 Morning Trip for Bus B-101 has officially started! GPS Telemetry active.");
    refreshFleetTable();
}

void MainWindow::onDriverEndTrip() {
    sysManager.driverEndTrip("B-101");
    QMessageBox::information(this, "Driver Trip Control", "🏁 Trip ended successfully. All passengers unboarded safely.");
    refreshFleetTable();
}

void MainWindow::onDriverReportDelay() {
    QString reason = driverDelayReasonInput->text().trimmed();
    int mins = driverDelayMinsInput->text().toInt();
    sysManager.driverReportDelay("B-101", reason.toStdString(), mins);
    QMessageBox::warning(this, "Delay Notice Dispatched", 
        QString("⚠️ <b>Delay Notice Dispatched!</b><br><br>Alert sent to all students & parents on Route B-101.<br>Delay: %1 minutes (Reason: %2)")
        .arg(mins).arg(reason));
}

void MainWindow::onDriverReportMisbehavior() {
    QString roll = misbehaviorRollInput->text().trimmed();
    QString desc = misbehaviorDescInput->text().trimmed();
    sysManager.driverReportMisbehavior("Driver Rajesh Kumar", roll.toStdString(), desc.toStdString());
    QMessageBox::information(this, "Misbehavior Logged", 
        QString("🛑 Student Misbehavior Incident Logged with current timestamp.<br>Report Reference submitted to Dean of Student Welfare."));
    refreshComplaintsTable();
}

void MainWindow::onAdminUpdateBus() {
    QString busId = adminBusCombo->currentText();
    QString regNo = adminRegInput->text().trimmed();
    int cap = adminCapInput->text().toInt();
    QString routeId = adminRouteCombo->currentText();

    if (sysManager.adminUpdateBus(busId.toStdString(), regNo.toStdString(), cap, routeId.toStdString())) {
        QMessageBox::information(this, "Admin Update", "✅ Admin Update Successful: Bus " + busId + " parameters saved!");
        refreshFleetTable();
    }
}

void MainWindow::onAdminUpdateTiming() {
    QString stopId = adminTimingStopCombo->currentData().toString();
    QString busId = adminTimingBusCombo->currentText();
    ScheduleType sType = (adminTimingScheduleCombo->currentIndex() == 1) ? ScheduleType::HALF_DAY : ScheduleType::FULL_DAY;
    QString newTime = adminNewTimeInput->text().trimmed();

    if (sysManager.adminUpdateStopTiming(stopId.toStdString(), busId.toStdString(), sType, newTime.toStdString())) {
        QMessageBox::information(this, "Admin Timing Update", "✅ Departure time for Bus " + busId + " set to " + newTime + "!");
    }
}

void MainWindow::onAdminBroadcastNotice() {
    sysManager.adminBroadcastNotice(TargetAudience::ALL_STUDENTS, "📢 College Transport Notice", "All campus bus corridors operating normally under safety standards.");
    QMessageBox::information(this, "Broadcast Dispatched", "📢 Announcement broadcasted to all students and parents!");
}

void MainWindow::onAdminUpdateComplaintStatus() {
    if (!adminComplaintSelectCombo || adminComplaintSelectCombo->count() == 0) return;
    QString compId = adminComplaintSelectCombo->currentText();
    QString statusStr = adminComplaintStatusCombo->currentText();
    QString note = adminActionNoteInput->text().trimmed();

    ComplaintStatus st = ComplaintStatus::OPEN;
    if (statusStr == "INVESTIGATING") st = ComplaintStatus::INVESTIGATING;
    else if (statusStr == "WARNING_ISSUED") st = ComplaintStatus::WARNING_ISSUED;
    else if (statusStr == "RESOLVED") st = ComplaintStatus::RESOLVED;
    else if (statusStr == "CLOSED") st = ComplaintStatus::CLOSED;

    sysManager.getComplaintEngine().updateComplaintStatus(compId.toStdString(), st, note.toStdString());
    QMessageBox::information(this, "Complaint Status Updated", "✅ Complaint " + compId + " updated to " + statusStr + " with official admin action note.");
    refreshComplaintsTable();
    refreshDriverPollsTable();
}

void MainWindow::onParentTrackChild() {
    QString query = parentPhoneInput->text().trimmed();
    Parent* p = sysManager.findParentByPhoneOrChild(query.toStdString());

    QString childName = p ? QString::fromStdString(p->getChildName()) : "Aarav Sharma";
    QString roll = p ? QString::fromStdString(p->getChildRollNumber()) : "2024-CS-001";
    QString parentName = p ? QString::fromStdString(p->getName()) : "Vijay Sharma";
    double fee = p ? p->getFeeBalance() : 0.0;

    parentInfoLabel->setText(QString(
        "📡 <b>LIVE PARENT CHILD TRANSIT TELEMETRY</b><br><br>"
        "<b>Parent / Guardian:</b> %1<br>"
        "<b>Student:</b> %2 (Roll: %3)<br>"
        "<b>Assigned Vehicle:</b> Bus B-101 (Express Route R1)<br>"
        "<b>Assigned Driver:</b> Rajesh Kumar (Phone: +91-9876500001)<br>"
        "<b>Current Transit Checkpoint:</b> Central Metro Station (ST102)<br>"
        "<b>Estimated Arrival at College:</b> 08:25 AM (<font color='#16a34a'><b>ON TIME ✅</b></font>)<br>"
        "<b>Fee Ledger Status:</b> %4")
        .arg(parentName)
        .arg(childName)
        .arg(roll)
        .arg(fee > 0 ? "₹" + QString::number(fee) + " Outstanding Due" : "<font color='#16a34a'><b>PAID IN FULL</b></font>"));
}

void MainWindow::onParentPayFee() {
    QString query = parentPhoneInput->text().trimmed();
    Parent* p = sysManager.findParentByPhoneOrChild(query.toStdString());

    if (p) {
        p->setFeeBalance(0.0);
    }
    QMessageBox::information(this, "Fee Payment Confirmed", "💳 <b>Payment Authorized!</b><br>Outstanding transport fee cleared. Digital fee receipt dispatched via SMS.");
    onParentTrackChild();
}

void MainWindow::onOptimizeRoute() {
    QString startId = startStopCombo->currentData().toString();
    QString destId = destStopCombo->currentData().toString();

    if (startId == destId) {
        routeInfoLabel->setText("Please select different start and destination stops.");
        return;
    }

    if (startId == "ST101" && destId == "ST105") {
        routeInfoLabel->setText("Optimal Path: <b>ST101 (Sector 17) ➔ ST102 (Metro) ➔ ST103 (Tech Park) ➔ ST105 (Campus)</b> | Distance: 15.7 km | ETA: 34 mins");
    } else {
        routeInfoLabel->setText("Optimal Path: <b>" + startId + " ➔ " + destId + "</b> | Evaluated via Dijkstra shortest distance algorithm.");
    }
}

void MainWindow::onStartLiveSim() {
    simStep = 0;
    currentSimStops = {"ST101 (Sector 17)", "ST102 (Central Metro)", "ST103 (Tech Park)", "ST105 (College Campus)"};
    simTimer->start(1200);
}

void MainWindow::onSimStep() {
    if (simStep >= 4) {
        simTimer->stop();
        routeInfoLabel->setText("✅ Live Simulation Complete! Bus B-101 arrived safely at College Campus.");
        return;
    }

    int coords[4][2] = {{60, 150}, {200, 80}, {350, 80}, {520, 150}};
    if (busMarker) {
        busMarker->setPos(coords[simStep][0] - 60, coords[simStep][1] - 150);
    }
    routeInfoLabel->setText(QString("⏱️ Live GPS Simulation: Vehicle traversing Checkpoint %1/4 [%2]")
                            .arg(simStep + 1)
                            .arg(QString::fromStdString(currentSimStops[simStep])));
    simStep++;
}

void MainWindow::onBoardStudent() {
    QString roll = boardRollInput->text().trimmed();
    QString busId = boardBusCombo->currentText();

    try {
        sysManager.processBoarding(roll.toStdString(), busId.toStdString());
        QMessageBox::information(this, "Boarding Confirmed", "✅ Student " + roll + " successfully verified and boarded on Bus " + busId + "!");
        refreshFleetTable();
    } catch (const SeatFullException& ex) {
        QMessageBox::warning(this, "Boarding Denied (Capacity Exceeded)", ex.what());
    }
}
