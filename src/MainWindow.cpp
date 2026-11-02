/**
 * ============================================================================
 * QT GUI MODULE: MainWindow Implementation (Role-Based Visibility Filtering)
 * FILE: MainWindow.cpp
 * ============================================================================
 */

#include "MainWindow.hpp"
#include <QCoreApplication>
#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QWheelEvent>
#include <QMouseEvent>
#include <cmath>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QBrush>
#include <QPen>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), simTimer(nullptr), simStep(0), visTimer(nullptr), visStep(0),
      busMarker(nullptr), optimalRoutePathItem(nullptr), optimalRouteGlowItem(nullptr), sourceHighlightMarker(nullptr) {
    setWindowTitle("Smart College Bus Management System (Role Authorized Portals)");
    setMinimumSize(1020, 720);
    resize(1200, 820);

    simTimer = new QTimer(this);
    connect(simTimer, &QTimer::timeout, this, &MainWindow::onSimStep);

    visTimer = new QTimer(this);
    connect(visTimer, &QTimer::timeout, this, &MainWindow::onVisStep);

    setupUI();
    renderRouteGraph();
    refreshFleetTable();
    refreshAttendanceTable();
    refreshComplaintTable();
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
    QGroupBox* loginCard = new QGroupBox(" System Authorization & Multi-Role Login", screen);
    loginCard->setFixedWidth(460);
    loginCard->setStyleSheet(
        "QGroupBox { font-size: 16px; font-weight: bold; color: #0284c7; border: 2px solid #0284c7; border-radius: 10px; margin-top: 10px; padding: 20px; background-color: #f8fafc; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 15px; padding: 0 5px; }"
    );

    QVBoxLayout* cardLayout = new QVBoxLayout(loginCard);

    QLabel* logoLabel = new QLabel(" Smart Bus Management System", loginCard);
    logoLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #0f172a; margin-bottom: 10px;");
    logoLabel->setAlignment(Qt::AlignCenter);

    QFormLayout* form = new QFormLayout();
    form->setSpacing(12);

    loginRoleCombo = new QComboBox(loginCard);
    loginRoleCombo->addItem(" Admin Portal", "ADMIN");
    loginRoleCombo->addItem(" Driver Portal", "DRIVER");
    loginRoleCombo->addItem(" Student / User Portal", "STUDENT");
    loginRoleCombo->addItem(" Parent Portal", "PARENT");
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

    QPushButton* signInBtn = new QPushButton(" Sign In to Portal", loginCard);
    signInBtn->setStyleSheet("background-color: #0284c7; color: white; font-weight: bold; font-size: 15px; padding: 10px; border-radius: 6px;");
    connect(signInBtn, &QPushButton::clicked, this, &MainWindow::onPerformLogin);

    loginStatusLabel = new QLabel("", loginCard);
    loginStatusLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #dc2626; margin-top: 5px;");
    loginStatusLabel->setAlignment(Qt::AlignCenter);

    // Default Credentials Cheat Sheet Card
    QLabel* cheatSheet = new QLabel(
        "<b>Default Login Credentials:</b><br>"
        "<b>Admin:</b> admin / admin123<br>"
        "<b>Driver:</b> D-01 / driver123<br>"
        "<b>Students (10 Registered):</b> 2024-CS-001 (Haldwani) | 2024-EC-045 (Tikonia) | 2024-IT-027 (Kathgodam) | 2024-ME-012 (Ranibagh) | 2024-CS-089 (Bhowali) / pass: student123<br>"
        "<b>Parent:</b> P-101 / parent123", loginCard);
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
    headerLayout->setContentsMargins(4, 2, 4, 2);
    headerLayout->setSpacing(8);

    QLabel* titleLabel = new QLabel("Smart College Bus Ecosystem", screen);
    titleLabel->setStyleSheet("font-size: 17px; font-weight: bold; color: #0284c7;");

    loggedUserLabel = new QLabel("Welcome, User", screen);
    loggedUserLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #0f172a;");

    loggedRoleBadge = new QLabel(" [ROLE] ", screen);
    loggedRoleBadge->setStyleSheet("font-size: 12px; font-weight: bold; background: #0284c7; color: white; padding: 3px 8px; border-radius: 10px;");

    QLabel* scheduleLabel = new QLabel("Schedule:", screen);
    scheduleLabel->setStyleSheet("font-size: 12px; font-weight: 600; color: #334155;");

    scheduleComboBox = new QComboBox(screen);
    scheduleComboBox->addItem("Full-Day (Normal)");
    scheduleComboBox->addItem("Half-Day (Exam)");
    scheduleComboBox->setMaximumWidth(150);
    scheduleComboBox->setStyleSheet("padding: 4px 6px; font-size: 12px; border-radius: 4px;");
    connect(scheduleComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onScheduleChanged);

    QPushButton* logoutBtn = new QPushButton("Logout", screen);
    logoutBtn->setStyleSheet("background-color: #dc2626; color: white; font-weight: bold; padding: 5px 12px; border-radius: 5px;");
    logoutBtn->setMinimumWidth(75);
    connect(logoutBtn, &QPushButton::clicked, this, &MainWindow::onPerformLogout);

    headerLayout->addWidget(titleLabel);
    headerLayout->addWidget(loggedUserLabel);
    headerLayout->addWidget(loggedRoleBadge);
    headerLayout->addStretch(1);
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
    connect(tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
        if (tabWidget->widget(index) == routeVisualizerTabWidget) {
            QTimer::singleShot(60, this, &MainWindow::fitCorridorView);
        } else if (tabWidget->widget(index) == studentTabWidget) {
            checkAndShowDelayNotice();
        }
    });

    mainLayout->addWidget(tabWidget);

    return screen;
}

// SETUP TABS BASED ON AUTHORIZED ROLE
void MainWindow::setupTabsForRole(UserRole role, const string& username) {
    tabWidget->clear();

    if (role == UserRole::ADMIN) {
        // Admin sees all management portals
        tabWidget->addTab(adminTabWidget, " Admin Control Center");
        tabWidget->addTab(studentTabWidget, " Student Portal");
        tabWidget->addTab(driverTabWidget, " Driver Portal");
        tabWidget->addTab(parentTabWidget, " Parent Portal");
        tabWidget->addTab(routeVisualizerTabWidget, " Route Map & Optimizer");
        tabWidget->addTab(fleetAndComplaintsTabWidget, " Fleet & Complaints");
    } else if (role == UserRole::STUDENT) {
        // Student sees ONLY Student Portal and Route Map
        tabWidget->addTab(studentTabWidget, " Student Portal");
        tabWidget->addTab(routeVisualizerTabWidget, " Route Map & Optimizer");
        studentRollInput->setText(QString::fromStdString(username));
        onSearchStudentSchedule();
        checkAndShowDelayNotice();

        // Auto-select student's assigned stop on Route Map
        Student* s = sysManager.findStudentByRoll(username);
        if (s && startStopCombo) {
            for (int i = 0; i < startStopCombo->count(); ++i) {
                if (startStopCombo->itemData(i).toString() == QString::fromStdString(s->getAssignedStopId())) {
                    startStopCombo->setCurrentIndex(i);
                    break;
                }
            }
        }
    } else if (role == UserRole::DRIVER) {
        // Driver sees ONLY Driver Portal, Fleet Status, and Route Map
        tabWidget->addTab(driverTabWidget, " Driver Portal");
        tabWidget->addTab(fleetAndComplaintsTabWidget, " Fleet Status");
        tabWidget->addTab(routeVisualizerTabWidget, " Route Map & Optimizer");
    } else if (role == UserRole::PARENT) {
        // Parent sees ONLY Parent Portal and Route Map
        tabWidget->addTab(parentTabWidget, " Parent Portal");
        tabWidget->addTab(routeVisualizerTabWidget, " Route Map & Optimizer");
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
        loginStatusLabel->setText("[ERROR] Invalid Username or Password. Please try again.");
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

// TAB 1: STUDENT PORTAL
QWidget* MainWindow::createStudentTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(10);

    // Dynamic Bus Delay Warning Banner (Shown if Driver reports delay)
    studentDelayBannerLabel = new QLabel(tab);
    studentDelayBannerLabel->setStyleSheet("background-color: #78350f; color: #fef08a; border: 1px solid #ca8a04; border-radius: 6px; padding: 10px 14px; font-weight: bold; font-size: 13px;");
    studentDelayBannerLabel->setVisible(false);
    layout->addWidget(studentDelayBannerLabel);

    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->setSpacing(10);

    // Student Lookup & Profile Box
    QGroupBox* box = new QGroupBox("Student Portal - Profile & Assigned Stop Lookup", tab);
    box->setStyleSheet(
        "QGroupBox { font-size: 13px; font-weight: bold; color: #f8fafc; border: 1px solid #334155; border-radius: 8px; margin-top: 8px; padding-top: 14px; background-color: #1e293b; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 4px; }"
        "QLabel { color: #cbd5e1; font-size: 12px; font-weight: 600; }"
        "QLineEdit, QComboBox { background-color: #0f172a; color: #f8fafc; font-size: 12px; border: 1px solid #475569; border-radius: 4px; padding: 4px 8px; }"
    );
    QFormLayout* formLayout = new QFormLayout(box);
    formLayout->setContentsMargins(10, 10, 10, 10);
    formLayout->setSpacing(8);

    studentRollInput = new QLineEdit("2024-CS-001", box);
    studentRollInput->setPlaceholderText("Enter Student Roll Number");
    connect(studentRollInput, &QLineEdit::returnPressed, this, &MainWindow::onSearchStudentSchedule);

    QComboBox* studentQuickCombo = new QComboBox(box);
    studentQuickCombo->addItem("-- Select Registered Student (10 Available) --", "");
    for (const auto& st : sysManager.getAllStudents()) {
        QString txt = QString("%1 - %2 (Stop: %3)")
                      .arg(QString::fromStdString(st.getRollNumber()))
                      .arg(QString::fromStdString(st.getName()))
                      .arg(QString::fromStdString(st.getAssignedStopId()));
        studentQuickCombo->addItem(txt, QString::fromStdString(st.getRollNumber()));
    }

    connect(studentQuickCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this, studentQuickCombo](int idx) {
        if (idx > 0) {
            QString roll = studentQuickCombo->itemData(idx).toString();
            studentRollInput->setText(roll);
            onSearchStudentSchedule();
        }
    });

    QPushButton* searchBtn = new QPushButton("Search Schedule & Timings", box);
    searchBtn->setStyleSheet("background-color: #0284c7; color: white; font-weight: bold; padding: 6px 12px; border-radius: 5px;");
    connect(searchBtn, &QPushButton::clicked, this, &MainWindow::onSearchStudentSchedule);

    QPushButton* qrPassBtn = new QPushButton("Show Digital QR Pass", box);
    qrPassBtn->setStyleSheet("background-color: #059669; color: white; font-weight: bold; padding: 6px 12px; border-radius: 5px;");
    connect(qrPassBtn, &QPushButton::clicked, this, &MainWindow::onShowDigitalQRPass);

    QPushButton* viewOnMapBtn = new QPushButton("View Stop on Map", box);
    viewOnMapBtn->setStyleSheet("background-color: #7c3aed; color: white; font-weight: bold; padding: 6px 12px; border-radius: 5px;");
    connect(viewOnMapBtn, &QPushButton::clicked, this, [this]() {
        onSearchStudentSchedule();
        if (tabWidget) {
            tabWidget->setCurrentWidget(routeVisualizerTabWidget);
            QTimer::singleShot(60, this, &MainWindow::fitCorridorView);
        }
    });

    QPushButton* sosBtn = new QPushButton("[ALERT] Emergency SOS", box);
    sosBtn->setStyleSheet("background-color: #dc2626; color: white; font-weight: bold; padding: 6px 12px; border-radius: 5px;");
    connect(sosBtn, &QPushButton::clicked, this, &MainWindow::onTriggerStudentSOS);

    QHBoxLayout* btnRow = new QHBoxLayout();
    btnRow->addWidget(searchBtn);
    btnRow->addWidget(qrPassBtn);
    btnRow->addWidget(viewOnMapBtn);

    formLayout->addRow("Quick Select:", studentQuickCombo);
    formLayout->addRow("Roll Number:", studentRollInput);
    formLayout->addRow("", btnRow);
    formLayout->addRow("", sosBtn);

    // Driver Rating & Complaint Box
    QGroupBox* rateBox = new QGroupBox("Driver Rating & Incident Reporting", tab);
    rateBox->setStyleSheet(
        "QGroupBox { font-size: 13px; font-weight: bold; color: #f8fafc; border: 1px solid #334155; border-radius: 8px; margin-top: 8px; padding-top: 14px; background-color: #1e293b; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 4px; }"
        "QLabel { color: #cbd5e1; font-size: 12px; font-weight: 600; }"
        "QLineEdit, QComboBox { background-color: #0f172a; color: #f8fafc; font-size: 12px; border: 1px solid #475569; border-radius: 4px; padding: 4px 8px; }"
    );
    QFormLayout* rateLayout = new QFormLayout(rateBox);
    rateLayout->setContentsMargins(10, 10, 10, 10);
    rateLayout->setSpacing(8);

    ratingDriverIdInput = new QLineEdit("D-01", rateBox);
    ratingStarCombo = new QComboBox(rateBox);
    ratingStarCombo->addItem("5 Stars (Excellent)");
    ratingStarCombo->addItem("4 Stars (Good)");
    ratingStarCombo->addItem("3 Stars (Average)");
    ratingStarCombo->addItem("2 Stars (Poor)");
    ratingStarCombo->addItem("1 Star (Unsafe)");

    ratingFeedbackInput = new QLineEdit("Smooth driving and punctual!", rateBox);
    QPushButton* submitRateBtn = new QPushButton("Submit Rating", rateBox);
    submitRateBtn->setStyleSheet("background-color: #d97706; color: white; font-weight: bold; padding: 6px 12px; border-radius: 5px;");
    connect(submitRateBtn, &QPushButton::clicked, this, &MainWindow::onSubmitDriverRating);

    QPushButton* reportDriverBtn = new QPushButton("Report Driver / Incident", rateBox);
    reportDriverBtn->setStyleSheet("background-color: #dc2626; color: white; font-weight: bold; padding: 6px 12px; border-radius: 5px;");
    connect(reportDriverBtn, &QPushButton::clicked, this, &MainWindow::onStudentReportDriver);

    QHBoxLayout* rateBtnRow = new QHBoxLayout();
    rateBtnRow->addWidget(submitRateBtn);
    rateBtnRow->addWidget(reportDriverBtn);

    rateLayout->addRow("Driver ID:", ratingDriverIdInput);
    rateLayout->addRow("Rating:", ratingStarCombo);
    rateLayout->addRow("Feedback:", ratingFeedbackInput);
    rateLayout->addRow("", rateBtnRow);

    topLayout->addWidget(box, 3);
    topLayout->addWidget(rateBox, 2);

    studentInfoLabel = new QLabel("Select or enter a Student Roll Number above to view assigned pickup stop and schedule.", tab);
    studentInfoLabel->setStyleSheet("font-size: 13px; color: #38bdf8; padding: 10px; background: #0f172a; border: 1px solid #334155; border-radius: 8px;");

    scheduleTable = new QTableWidget(0, 5, tab);
    scheduleTable->setHorizontalHeaderLabels({"Bus ID", "Route Name", "Boarding / Campus Stop", "Scheduled Bus Time", "Transit Status"});
    scheduleTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    scheduleTable->setStyleSheet(
        "QTableWidget { background-color: #0f172a; color: #f8fafc; gridline-color: #334155; border: 1px solid #334155; border-radius: 6px; font-size: 13px; }"
        "QHeaderView::section { background-color: #1e293b; color: #94a3b8; font-weight: bold; border: 1px solid #334155; padding: 6px; }"
    );

    layout->addLayout(topLayout);
    layout->addWidget(studentInfoLabel);
    layout->addWidget(scheduleTable, 1);

    // Pre-populate with default student 2024-CS-001 so it is never blank!
    QTimer::singleShot(20, this, &MainWindow::onSearchStudentSchedule);

    return tab;
}

// TAB 2:  DRIVER PORTAL
QWidget* MainWindow::createDriverTab() {
    QWidget* tab = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(tab);

    QGroupBox* tripBox = new QGroupBox(" Driver Trip & Delay Control", tab);
    QFormLayout* tripForm = new QFormLayout(tripBox);

    driverBusCombo = new QComboBox(tripBox);
    driverBusCombo->addItem("B-101"); driverBusCombo->addItem("B-102"); driverBusCombo->addItem("B-103");

    QPushButton* startBtn = new QPushButton(" Start Trip", tripBox);
    startBtn->setStyleSheet("background-color: #059669; color: white; font-weight: bold;");
    connect(startBtn, &QPushButton::clicked, this, &MainWindow::onDriverStartTrip);

    QPushButton* endBtn = new QPushButton(" End Trip", tripBox);
    endBtn->setStyleSheet("background-color: #64748b; color: white; font-weight: bold;");
    connect(endBtn, &QPushButton::clicked, this, &MainWindow::onDriverEndTrip);

    driverDelayReasonInput = new QLineEdit("Expressway Traffic Jam", tripBox);
    driverDelayMinsInput = new QLineEdit("15", tripBox);
    QPushButton* delayBtn = new QPushButton("[WARNING] Report Delay to Students & Parents", tripBox);
    delayBtn->setStyleSheet("background-color: #d97706; color: white; font-weight: bold;");
    connect(delayBtn, &QPushButton::clicked, this, &MainWindow::onDriverReportDelay);

    tripForm->addRow("Select Bus ID:", driverBusCombo);
    tripForm->addRow(startBtn, endBtn);
    tripForm->addRow("Delay Reason:", driverDelayReasonInput);
    tripForm->addRow("Minutes Delayed:", driverDelayMinsInput);
    tripForm->addRow("", delayBtn);

    QGroupBox* misbehaviorBox = new QGroupBox(" Report Student Misbehavior & Bus Condition", tab);
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

// TAB 3: ADMIN CONTROL CENTER & STUDENT ATTENDANCE MODULE
QWidget* MainWindow::createAdminTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* mainAdminLayout = new QVBoxLayout(tab);
    mainAdminLayout->setContentsMargins(10, 10, 10, 10);
    mainAdminLayout->setSpacing(10);

    // 1. Top Modern Metric KPI Strip
    QHBoxLayout* statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(10);

    auto makeStatCard = [](const QString& title, const QString& val, const QString& color, QLabel** outLabel = nullptr) -> QFrame* {
        QFrame* card = new QFrame();
        card->setStyleSheet(QString(
            "QFrame { background-color: #1e293b; border: 1px solid #334155; border-radius: 8px; padding: 8px 14px; }"
            "QLabel#title { color: #94a3b8; font-size: 11px; font-weight: bold; text-transform: uppercase; }"
            "QLabel#val { color: %1; font-size: 16px; font-weight: bold; }").arg(color));
        QVBoxLayout* l = new QVBoxLayout(card);
        l->setContentsMargins(4, 4, 4, 4);
        l->setSpacing(2);
        QLabel* t = new QLabel(title, card); t->setObjectName("title");
        QLabel* v = new QLabel(val, card); v->setObjectName("val");
        if (outLabel) *outLabel = v;
        l->addWidget(t);
        l->addWidget(v);
        return card;
    };

    QFrame* c1 = makeStatCard("Fleet Status", "3 Buses (165 Seats)", "#38bdf8", &statFleetCountLabel);
    QFrame* c2 = makeStatCard("Operating Corridors", "Haldwani - GEHU Bhimtal", "#10b981", &statActiveRoutesLabel);
    QFrame* c3 = makeStatCard("Campus Schedule", "Full-Day (Normal Hours)", "#fbbf24");
    QFrame* c4 = makeStatCard("Student Attendance", "3 Verified Boardings", "#a855f7", &statAttendanceCountLabel);

    statsLayout->addWidget(c1);
    statsLayout->addWidget(c2);
    statsLayout->addWidget(c3);
    statsLayout->addWidget(c4);

    // 2. Middle Row: Fleet/Timing Config & Student Directory/Registration Desk in 2 Columns
    QHBoxLayout* midLayout = new QHBoxLayout();
    midLayout->setSpacing(10);

    // Left Column: Bus Fleet Configuration + Stop Timings
    QVBoxLayout* leftCol = new QVBoxLayout();
    leftCol->setSpacing(10);

    // Bus Fleet Configuration
    QGroupBox* busBox = new QGroupBox("Bus Fleet Configuration", tab);
    busBox->setStyleSheet(
        "QGroupBox { font-size: 13px; font-weight: bold; color: #f8fafc; border: 1px solid #334155; border-radius: 8px; margin-top: 8px; padding-top: 14px; background-color: #1e293b; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 4px; }"
        "QLabel { color: #cbd5e1; font-size: 12px; font-weight: 600; }"
        "QLineEdit, QComboBox { background-color: #0f172a; color: #f8fafc; font-size: 12px; border: 1px solid #475569; border-radius: 4px; padding: 4px 8px; }"
    );
    QFormLayout* busForm = new QFormLayout(busBox);
    busForm->setContentsMargins(10, 10, 10, 10);
    busForm->setSpacing(8);

    adminBusCombo = new QComboBox(busBox);
    adminBusCombo->addItem("B-101 (Express Route R1)", "B-101");
    adminBusCombo->addItem("B-102 (Suburban Route R2)", "B-102");
    adminBusCombo->addItem("B-103 (Standby Campus Shuttle)", "B-103");

    adminRegInput = new QLineEdit("MH-12-AB-9999", busBox);
    adminCapInput = new QLineEdit("55", busBox);

    adminRouteCombo = new QComboBox(busBox);
    adminRouteCombo->addItem("R1 - Haldwani Express Corridor", "R1");
    adminRouteCombo->addItem("R2 - Bhowali Mountain Route", "R2");

    QPushButton* updateBusBtn = new QPushButton("Save Bus Configuration", busBox);
    updateBusBtn->setStyleSheet("background-color: #2563eb; color: white; font-weight: bold; padding: 6px 14px; border-radius: 5px;");
    connect(updateBusBtn, &QPushButton::clicked, this, &MainWindow::onAdminUpdateBus);

    busForm->addRow("Select Bus ID:", adminBusCombo);
    busForm->addRow("Registration No:", adminRegInput);
    busForm->addRow("Seating Capacity:", adminCapInput);
    busForm->addRow("Assigned Route:", adminRouteCombo);
    busForm->addRow("", updateBusBtn);

    // Stop Timings & Broadcast Notice
    QGroupBox* timingBox = new QGroupBox("Campus Stop Timings & Broadcast Dispatch", tab);
    timingBox->setStyleSheet(
        "QGroupBox { font-size: 13px; font-weight: bold; color: #f8fafc; border: 1px solid #334155; border-radius: 8px; margin-top: 8px; padding-top: 14px; background-color: #1e293b; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 4px; }"
        "QLabel { color: #cbd5e1; font-size: 12px; font-weight: 600; }"
        "QLineEdit, QComboBox { background-color: #0f172a; color: #f8fafc; font-size: 12px; border: 1px solid #475569; border-radius: 4px; padding: 4px 8px; }"
    );
    QFormLayout* timingForm = new QFormLayout(timingBox);
    timingForm->setContentsMargins(10, 10, 10, 10);
    timingForm->setSpacing(8);

    adminTimingStopCombo = new QComboBox(timingBox);
    adminTimingStopCombo->addItem("ST101 - Haldwani Bus Terminal", "ST101");
    adminTimingStopCombo->addItem("ST102 - Tikonia Chowk", "ST102");
    adminTimingStopCombo->addItem("ST103 - Kathgodam Railway Station", "ST103");
    adminTimingStopCombo->addItem("ST104 - Ranibagh HMT Junction", "ST104");
    adminTimingStopCombo->addItem("ST106 - Bhowali Sanatorium Chowk", "ST106");
    adminTimingStopCombo->addItem("ST105 - GEHU Bhimtal Campus Main Gate", "ST105");

    adminTimingBusCombo = new QComboBox(timingBox);
    adminTimingBusCombo->addItem("B-101", "B-101");
    adminTimingBusCombo->addItem("B-102", "B-102");
    adminTimingBusCombo->addItem("B-103", "B-103");

    adminTimingScheduleCombo = new QComboBox(timingBox);
    adminTimingScheduleCombo->addItem("Full-Day Schedule");
    adminTimingScheduleCombo->addItem("Half-Day Schedule");

    adminNewTimeInput = new QLineEdit("07:35", timingBox);

    QHBoxLayout* timingBtnLayout = new QHBoxLayout();
    QPushButton* updateTimingBtn = new QPushButton("Update Timing", timingBox);
    updateTimingBtn->setStyleSheet("background-color: #d97706; color: white; font-weight: bold; padding: 6px 12px; border-radius: 5px;");
    connect(updateTimingBtn, &QPushButton::clicked, this, &MainWindow::onAdminUpdateTiming);

    QPushButton* noticeBtn = new QPushButton("Dispatch Notice", timingBox);
    noticeBtn->setStyleSheet("background-color: #0284c7; color: white; font-weight: bold; padding: 6px 12px; border-radius: 5px;");
    connect(noticeBtn, &QPushButton::clicked, this, &MainWindow::onAdminBroadcastNotice);

    timingBtnLayout->addWidget(updateTimingBtn);
    timingBtnLayout->addWidget(noticeBtn);

    timingForm->addRow("Select Stop ID:", adminTimingStopCombo);
    timingForm->addRow("Select Bus ID:", adminTimingBusCombo);
    timingForm->addRow("Schedule Type:", adminTimingScheduleCombo);
    timingForm->addRow("New Departure Time:", adminNewTimeInput);
    timingForm->addRow("", timingBtnLayout);

    leftCol->addWidget(busBox);
    leftCol->addWidget(timingBox);

    // Right Column: Student Directory Inspector & Registration Desk
    QGroupBox* studentDirBox = new QGroupBox("Student Transport Directory & Registration Desk", tab);
    studentDirBox->setStyleSheet(
        "QGroupBox { font-size: 13px; font-weight: bold; color: #f8fafc; border: 1px solid #334155; border-radius: 8px; margin-top: 8px; padding-top: 14px; background-color: #1e293b; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 4px; }"
        "QLabel { color: #cbd5e1; font-size: 12px; font-weight: 600; }"
        "QLineEdit, QComboBox { background-color: #0f172a; color: #f8fafc; font-size: 12px; border: 1px solid #475569; border-radius: 4px; padding: 4px 8px; }"
    );
    QVBoxLayout* dirLayout = new QVBoxLayout(studentDirBox);
    dirLayout->setContentsMargins(10, 10, 10, 10);
    dirLayout->setSpacing(8);

    // Student Lookup & Inspector
    QLabel* inspHeader = new QLabel("Inspect Registered Student Record:", studentDirBox);
    inspHeader->setStyleSheet("color: #38bdf8; font-weight: bold; font-size: 12px;");
    adminStudentLookupCombo = new QComboBox(studentDirBox);
    for (const auto& st : sysManager.getAllStudents()) {
        adminStudentLookupCombo->addItem(
            QString("%1 - %2 (%3)").arg(QString::fromStdString(st.getRollNumber()))
                                   .arg(QString::fromStdString(st.getName()))
                                   .arg(QString::fromStdString(st.getAssignedStopId())),
            QString::fromStdString(st.getRollNumber())
        );
    }
    connect(adminStudentLookupCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onAdminLookupStudentChanged);

    adminStudentLookupInfoLabel = new QLabel(studentDirBox);
    adminStudentLookupInfoLabel->setStyleSheet("background-color: #0f172a; border: 1px solid #334155; border-radius: 6px; padding: 8px; color: #f8fafc; font-size: 12px;");

    // Register New Student Form
    QGroupBox* regBox = new QGroupBox("Register New Student", studentDirBox);
    regBox->setStyleSheet("QGroupBox { border: 1px dashed #475569; border-radius: 6px; padding: 6px; margin-top: 4px; font-size: 11px; color: #94a3b8; }");
    QFormLayout* regForm = new QFormLayout(regBox);
    regForm->setSpacing(6);

    adminNewStudentNameInput = new QLineEdit("Kunal Sharma", regBox);
    adminNewStudentRollInput = new QLineEdit("2024-CS-099", regBox);
    adminNewStudentPhoneInput = new QLineEdit("+91-9876543210", regBox);

    adminNewStudentStopCombo = new QComboBox(regBox);
    adminNewStudentStopCombo->addItem("ST101 - Haldwani Bus Terminal", "ST101");
    adminNewStudentStopCombo->addItem("ST102 - Tikonia Chowk", "ST102");
    adminNewStudentStopCombo->addItem("ST103 - Kathgodam Station", "ST103");
    adminNewStudentStopCombo->addItem("ST104 - Ranibagh Junction", "ST104");
    adminNewStudentStopCombo->addItem("ST106 - Bhowali Chowk", "ST106");

    adminNewStudentBusCombo = new QComboBox(regBox);
    adminNewStudentBusCombo->addItem("B-101 (Express R1)", "B-101");
    adminNewStudentBusCombo->addItem("B-102 (Mountain R2)", "B-102");
    adminNewStudentBusCombo->addItem("B-103 (Standby Shuttle)", "B-103");

    adminNewStudentFeeCombo = new QComboBox(regBox);
    adminNewStudentFeeCombo->addItem("Fee Paid (Active Pass)", "PAID");
    adminNewStudentFeeCombo->addItem("Fee Pending", "PENDING");

    QPushButton* registerBtn = new QPushButton("Register & Save Student", regBox);
    registerBtn->setStyleSheet("background-color: #059669; color: white; font-weight: bold; padding: 6px 14px; border-radius: 5px;");
    connect(registerBtn, &QPushButton::clicked, this, &MainWindow::onAdminRegisterStudent);

    regForm->addRow("Full Name:", adminNewStudentNameInput);
    regForm->addRow("Roll Number:", adminNewStudentRollInput);
    regForm->addRow("Phone Number:", adminNewStudentPhoneInput);
    regForm->addRow("Boarding Stop:", adminNewStudentStopCombo);
    regForm->addRow("Assigned Bus:", adminNewStudentBusCombo);
    regForm->addRow("Semester Fee:", adminNewStudentFeeCombo);
    regForm->addRow("", registerBtn);

    dirLayout->addWidget(inspHeader);
    dirLayout->addWidget(adminStudentLookupCombo);
    dirLayout->addWidget(adminStudentLookupInfoLabel);
    dirLayout->addWidget(regBox);

    midLayout->addLayout(leftCol, 1);
    midLayout->addWidget(studentDirBox, 1);

    // 3. Bottom Section: Student Bus Attendance & Live Boarding Management Module
    QGroupBox* attBox = new QGroupBox("Student Bus Attendance & Boarding Management Module", tab);
    attBox->setStyleSheet(
        "QGroupBox { font-size: 13px; font-weight: bold; color: #f8fafc; border: 1px solid #334155; border-radius: 8px; margin-top: 8px; padding-top: 14px; background-color: #1e293b; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 4px; }"
        "QLabel { color: #cbd5e1; font-size: 12px; font-weight: 600; }"
        "QComboBox { background-color: #0f172a; color: #f8fafc; font-size: 12px; border: 1px solid #475569; border-radius: 4px; padding: 4px 8px; }"
        "QTableWidget { background-color: #0f172a; color: #f8fafc; gridline-color: #334155; border: 1px solid #334155; border-radius: 6px; }"
        "QHeaderView::section { background-color: #1e293b; color: #94a3b8; font-weight: bold; border: 1px solid #334155; padding: 4px; }"
    );
    QVBoxLayout* attLayout = new QVBoxLayout(attBox);
    attLayout->setContentsMargins(10, 10, 10, 10);
    attLayout->setSpacing(8);

    QHBoxLayout* attCtrlLayout = new QHBoxLayout();
    attCtrlLayout->setSpacing(8);

    adminBoardRollCombo = new QComboBox(attBox);
    for (const auto& st : sysManager.getAllStudents()) {
        QString txt = QString("%1 (%2) - %3")
                      .arg(QString::fromStdString(st.getRollNumber()))
                      .arg(QString::fromStdString(st.getName()))
                      .arg(QString::fromStdString(st.getAssignedStopId()));
        adminBoardRollCombo->addItem(txt, QString::fromStdString(st.getRollNumber()));
    }
    adminBoardRollCombo->setMaximumWidth(280);

    adminBoardBusCombo = new QComboBox(attBox);
    adminBoardBusCombo->addItem("B-101", "B-101");
    adminBoardBusCombo->addItem("B-102", "B-102");
    adminBoardBusCombo->addItem("B-103", "B-103");
    adminBoardBusCombo->setMaximumWidth(90);

    QPushButton* markAttBtn = new QPushButton("Mark Attendance / Board", attBox);
    markAttBtn->setStyleSheet("background-color: #059669; color: white; font-weight: bold; padding: 6px 14px; border-radius: 5px;");
    connect(markAttBtn, &QPushButton::clicked, this, &MainWindow::onAdminMarkAttendance);

    QPushButton* scanQrBtn = new QPushButton("Verify QR Pass", attBox);
    scanQrBtn->setStyleSheet("background-color: #7c3aed; color: white; font-weight: bold; padding: 6px 14px; border-radius: 5px;");
    connect(scanQrBtn, &QPushButton::clicked, this, &MainWindow::onAdminScanQRAttendance);

    QPushButton* refreshAttBtn = new QPushButton("Refresh Log", attBox);
    refreshAttBtn->setStyleSheet("background-color: #334155; color: #f8fafc; font-weight: bold; padding: 6px 12px; border-radius: 5px;");
    connect(refreshAttBtn, &QPushButton::clicked, this, &MainWindow::refreshAttendanceTable);

    adminAttendanceStatusLabel = new QLabel("Ready to log boarding events and student bus attendance.", attBox);
    adminAttendanceStatusLabel->setStyleSheet("color: #38bdf8; font-weight: 600; font-size: 12px;");

    attCtrlLayout->addWidget(new QLabel("Student Roll:", attBox));
    attCtrlLayout->addWidget(adminBoardRollCombo);
    attCtrlLayout->addWidget(new QLabel("Bus:", attBox));
    attCtrlLayout->addWidget(adminBoardBusCombo);
    attCtrlLayout->addWidget(markAttBtn);
    attCtrlLayout->addWidget(scanQrBtn);
    attCtrlLayout->addWidget(refreshAttBtn);
    attCtrlLayout->addStretch();

    adminAttendanceTable = new QTableWidget(0, 7, attBox);
    adminAttendanceTable->setHorizontalHeaderLabels({"Timestamp", "Roll Number", "Student Name", "Bus ID", "Pickup Stop", "Fee Status", "Boarding Status"});
    adminAttendanceTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    adminAttendanceTable->setMinimumHeight(170);

    attLayout->addLayout(attCtrlLayout);
    attLayout->addWidget(adminAttendanceStatusLabel);
    attLayout->addWidget(adminAttendanceTable, 1);

    mainAdminLayout->addLayout(statsLayout);
    mainAdminLayout->addLayout(midLayout);
    mainAdminLayout->addWidget(attBox, 1);

    // Initial student inspector record display
    onAdminLookupStudentChanged(0);

    return tab;
}

// TAB 4:  PARENT PORTAL
QWidget* MainWindow::createParentTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);

    QGroupBox* box = new QGroupBox(" Parent Live Child Tracking & Fee Status", tab);
    QHBoxLayout* formLayout = new QHBoxLayout(box);

    parentPhoneInput = new QLineEdit("+91-9811001122", box);
    QPushButton* trackBtn = new QPushButton(" Track Child's Bus & ETA", box);
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

// TAB 5: ROUTE OPTIMIZER & REAL GEOGRAPHICAL MAP (GRAPHIC ERA HILL UNIVERSITY - BHIMTAL CAMPUS)
QWidget* MainWindow::createRouteVisualizerTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(8);

    // Modern 2-Row Control Toolbar Card
    QFrame* ctrlCard = new QFrame(tab);
    ctrlCard->setObjectName("routeCtrlCard");
    ctrlCard->setStyleSheet(
        "#routeCtrlCard {"
        "  background-color: #1e293b;"
        "  border: 1px solid #334155;"
        "  border-radius: 8px;"
        "  padding: 4px;"
        "}"
        "QLabel { color: #cbd5e1; font-size: 12px; font-weight: 600; }"
        "QComboBox { background-color: #0f172a; color: #f8fafc; font-size: 12px; font-weight: 600; border: 1px solid #475569; border-radius: 5px; padding: 4px 8px; }"
        "QPushButton { font-size: 12px; font-weight: bold; border-radius: 5px; padding: 6px 12px; border: none; }"
    );

    QVBoxLayout* cardLayout = new QVBoxLayout(ctrlCard);
    cardLayout->setContentsMargins(8, 8, 8, 8);
    cardLayout->setSpacing(8);

    // Row 1: Source Pickup, College Destination, Algorithm, Execution Actions
    QHBoxLayout* row1 = new QHBoxLayout();
    row1->setSpacing(8);

    startStopCombo = new QComboBox(ctrlCard);
    startStopCombo->addItem("ST101 - Haldwani Bus Terminal", "ST101");
    startStopCombo->addItem("ST102 - Tikonia Chowk", "ST102");
    startStopCombo->addItem("ST103 - Kathgodam Station", "ST103");
    startStopCombo->addItem("ST104 - Ranibagh Junction", "ST104");
    startStopCombo->addItem("ST106 - Bhowali Chowk", "ST106");
    startStopCombo->setMaximumWidth(220);

    destStopCombo = new QComboBox(ctrlCard);
    destStopCombo->addItem("GEHU Bhimtal Campus", "GEHU");
    destStopCombo->setEnabled(false);
    destStopCombo->setMaximumWidth(170);
    destStopCombo->setStyleSheet("background-color: #3b0764; color: #fde047; font-weight: bold; border: 1px solid #7e22ce; border-radius: 5px; padding: 4px 8px;");

    algorithmCombo = new QComboBox(ctrlCard);
    algorithmCombo->addItem("A* (Guided Heuristic)", "ASTAR");
    algorithmCombo->addItem("Dijkstra (Uniform Cost)", "DIJKSTRA");
    algorithmCombo->setMaximumWidth(170);

    QPushButton* optBtn = new QPushButton("Calculate Shortest Path", ctrlCard);
    optBtn->setStyleSheet("background-color: #2563eb; color: white; font-weight: bold; padding: 6px 12px; border-radius: 5px;");

    QPushButton* visBtn = new QPushButton("Visualize Search", ctrlCard);
    visBtn->setStyleSheet("background-color: #7c3aed; color: white; font-weight: bold; padding: 6px 12px; border-radius: 5px;");

    QPushButton* simBtn = new QPushButton("Simulate Bus Motion", ctrlCard);
    simBtn->setStyleSheet("background-color: #059669; color: white; font-weight: bold; padding: 6px 12px; border-radius: 5px;");

    row1->addWidget(new QLabel("Pickup (Source):", ctrlCard));
    row1->addWidget(startStopCombo);
    row1->addWidget(new QLabel("Destination:", ctrlCard));
    row1->addWidget(destStopCombo);
    row1->addWidget(new QLabel("Algorithm:", ctrlCard));
    row1->addWidget(algorithmCombo);
    row1->addWidget(optBtn);
    row1->addWidget(visBtn);
    row1->addWidget(simBtn);
    row1->addStretch();

    // Row 2: Viewport controls (Fit, Zoom In, Zoom Out, Web Map) + Status Chip
    QHBoxLayout* row2 = new QHBoxLayout();
    row2->setSpacing(8);

    QPushButton* fitViewBtn = new QPushButton("Fit Corridor", ctrlCard);
    fitViewBtn->setStyleSheet("background-color: #334155; color: #f8fafc; font-weight: 600; padding: 5px 10px; border-radius: 5px; border: 1px solid #475569;");

    QPushButton* zoomInBtn = new QPushButton("Zoom (+)", ctrlCard);
    zoomInBtn->setStyleSheet("background-color: #334155; color: #f8fafc; font-weight: 600; padding: 5px 10px; border-radius: 5px; border: 1px solid #475569;");

    QPushButton* zoomOutBtn = new QPushButton("Zoom (-)", ctrlCard);
    zoomOutBtn->setStyleSheet("background-color: #334155; color: #f8fafc; font-weight: 600; padding: 5px 10px; border-radius: 5px; border: 1px solid #475569;");

    QPushButton* webMapBtn = new QPushButton("Launch Web Map (OSM)", ctrlCard);
    webMapBtn->setStyleSheet("background-color: #0284c7; color: white; font-weight: bold; padding: 5px 10px; border-radius: 5px;");

    routeInfoLabel = new QLabel("Select your pickup stop above or click any stop pin on the map to find shortest route to Graphic Era Hill University Bhimtal.", ctrlCard);
    routeInfoLabel->setStyleSheet("font-size: 12px; font-weight: 600; color: #38bdf8; padding: 5px 10px; background-color: #0f172a; border: 1px solid #0369a1; border-radius: 5px;");
    routeInfoLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    row2->addWidget(fitViewBtn);
    row2->addWidget(zoomInBtn);
    row2->addWidget(zoomOutBtn);
    row2->addWidget(webMapBtn);
    row2->addWidget(routeInfoLabel, 1);

    cardLayout->addLayout(row1);
    cardLayout->addLayout(row2);

    connect(optBtn, &QPushButton::clicked, this, &MainWindow::onOptimizeRoute);
    connect(visBtn, &QPushButton::clicked, this, &MainWindow::onVisualizeAlgorithmSearch);
    connect(simBtn, &QPushButton::clicked, this, &MainWindow::onStartLiveSim);
    connect(zoomInBtn, &QPushButton::clicked, this, &MainWindow::onZoomIn);
    connect(zoomOutBtn, &QPushButton::clicked, this, &MainWindow::onZoomOut);
    connect(fitViewBtn, &QPushButton::clicked, this, &MainWindow::onFitCorridor);
    connect(webMapBtn, &QPushButton::clicked, this, &MainWindow::onOpenWebMap);
    connect(startStopCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onOptimizeRoute);
    connect(algorithmCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onOptimizeRoute);

    graphScene = new QGraphicsScene(this);
    graphScene->setBackgroundBrush(QBrush(QColor("#0f172a")));

    graphView = new QGraphicsView(graphScene, tab);
    graphView->setRenderHint(QPainter::Antialiasing);
    graphView->setRenderHint(QPainter::SmoothPixmapTransform);
    graphView->setDragMode(QGraphicsView::ScrollHandDrag);
    graphView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    graphView->setMinimumHeight(480);
    graphView->setBackgroundBrush(QBrush(QColor("#0f172a")));
    graphView->setStyleSheet("QGraphicsView { border: 1px solid #334155; border-radius: 8px; background-color: #0f172a; }");
    graphView->viewport()->installEventFilter(this);

    layout->addWidget(ctrlCard);
    layout->addWidget(graphView, 1);

    return tab;
}

// TAB 6: FLEET & COMPLAINTS
QWidget* MainWindow::createFleetAndComplaintsTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(10);

    // Fleet Table
    QGroupBox* fleetBox = new QGroupBox("College Bus Fleet Status Overview", tab);
    fleetBox->setStyleSheet(
        "QGroupBox { font-size: 13px; font-weight: bold; color: #f8fafc; border: 1px solid #334155; border-radius: 8px; margin-top: 8px; padding-top: 14px; background-color: #1e293b; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 4px; }"
        "QTableWidget { background-color: #0f172a; color: #f8fafc; gridline-color: #334155; border: 1px solid #334155; border-radius: 6px; font-size: 12px; }"
        "QHeaderView::section { background-color: #1e293b; color: #94a3b8; font-weight: bold; border: 1px solid #334155; padding: 5px; }"
    );
    QVBoxLayout* fleetBoxLayout = new QVBoxLayout(fleetBox);
    fleetTable = new QTableWidget(3, 6, fleetBox);
    fleetTable->setHorizontalHeaderLabels({"Bus ID", "Registration", "Capacity", "Occupancy", "Route", "Driver"});
    fleetTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    fleetTable->setMinimumHeight(110);
    fleetBoxLayout->addWidget(fleetTable);

    // Boarding Control
    QGroupBox* boardBox = new QGroupBox("Student Boarding Control", tab);
    boardBox->setStyleSheet(
        "QGroupBox { font-size: 13px; font-weight: bold; color: #f8fafc; border: 1px solid #334155; border-radius: 8px; margin-top: 8px; padding-top: 14px; background-color: #1e293b; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 4px; }"
        "QLabel { color: #cbd5e1; font-size: 12px; font-weight: 600; }"
        "QLineEdit, QComboBox { background-color: #0f172a; color: #f8fafc; font-size: 12px; border: 1px solid #475569; border-radius: 4px; padding: 4px 8px; }"
    );
    QHBoxLayout* boardLayout = new QHBoxLayout(boardBox);
    boardRollInput = new QLineEdit("2024-CS-001", boardBox);
    boardBusCombo = new QComboBox(boardBox);
    boardBusCombo->addItem("B-101"); boardBusCombo->addItem("B-102"); boardBusCombo->addItem("B-103");

    QPushButton* boardBtn = new QPushButton("Board Student", boardBox);
    boardBtn->setStyleSheet("background-color: #059669; color: white; font-weight: bold; padding: 6px 14px; border-radius: 5px;");
    connect(boardBtn, &QPushButton::clicked, this, &MainWindow::onBoardStudent);

    boardLayout->addWidget(new QLabel("Roll No:", boardBox));
    boardLayout->addWidget(boardRollInput);
    boardLayout->addWidget(new QLabel("Bus ID:", boardBox));
    boardLayout->addWidget(boardBusCombo);
    boardLayout->addWidget(boardBtn);

    // Central Transport Complaints & Incident Management Desk
    QGroupBox* compBox = new QGroupBox("Transport Complaints & Safety Incidents Management Desk", tab);
    compBox->setStyleSheet(
        "QGroupBox { font-size: 13px; font-weight: bold; color: #f8fafc; border: 1px solid #334155; border-radius: 8px; margin-top: 8px; padding-top: 14px; background-color: #1e293b; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 4px; }"
        "QLabel { color: #cbd5e1; font-size: 12px; font-weight: 600; }"
        "QTableWidget { background-color: #0f172a; color: #f8fafc; gridline-color: #334155; border: 1px solid #334155; border-radius: 6px; font-size: 12px; }"
        "QHeaderView::section { background-color: #1e293b; color: #94a3b8; font-weight: bold; border: 1px solid #334155; padding: 5px; }"
    );
    QVBoxLayout* compLayout = new QVBoxLayout(compBox);
    compLayout->setContentsMargins(10, 10, 10, 10);
    compLayout->setSpacing(8);

    QHBoxLayout* compCtrl = new QHBoxLayout();
    QPushButton* resolveBtn = new QPushButton("Resolve Selected Complaint", compBox);
    resolveBtn->setStyleSheet("background-color: #059669; color: white; font-weight: bold; padding: 6px 14px; border-radius: 5px;");
    connect(resolveBtn, &QPushButton::clicked, this, &MainWindow::onAdminResolveComplaint);

    QPushButton* refreshCompBtn = new QPushButton("Refresh Complaints Log", compBox);
    refreshCompBtn->setStyleSheet("background-color: #334155; color: #f8fafc; font-weight: bold; padding: 6px 12px; border-radius: 5px;");
    connect(refreshCompBtn, &QPushButton::clicked, this, &MainWindow::refreshComplaintTable);

    compCtrl->addWidget(new QLabel("Live incidents & complaints reported by students, drivers, and parents:", compBox));
    compCtrl->addStretch();
    compCtrl->addWidget(resolveBtn);
    compCtrl->addWidget(refreshCompBtn);

    complaintTable = new QTableWidget(0, 8, compBox);
    complaintTable->setHorizontalHeaderLabels({"Complaint ID", "Type", "Reporter", "Target / Accused", "Bus ID", "Category", "Description", "Status"});
    complaintTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    complaintTable->setMinimumHeight(180);

    compLayout->addLayout(compCtrl);
    compLayout->addWidget(complaintTable);

    layout->addWidget(fleetBox);
    layout->addWidget(boardBox);
    layout->addWidget(compBox, 1);

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
    optimalRoutePathItem = nullptr;
    optimalRouteGlowItem = nullptr;
    sourceHighlightMarker = nullptr;
    busMarker = nullptr;

    // 1. Load Real OpenStreetMap Base Map of Haldwani & Bhimtal with multi-path resolution
    QStringList searchPaths = {
        ":/data/real_osm_basemap.png",
        "data/real_osm_basemap.png",
        QCoreApplication::applicationDirPath() + "/data/real_osm_basemap.png",
        QCoreApplication::applicationDirPath() + "/../Resources/real_osm_basemap.png",
        QCoreApplication::applicationDirPath() + "/../../../data/real_osm_basemap.png",
        "/Users/tushartiwari565/Documents/project copy/data/real_osm_basemap.png"
    };

    QPixmap osmPix;
    QString loadedFrom;
    for (const QString& path : searchPaths) {
        if (QFile::exists(path)) {
            osmPix.load(path);
            if (!osmPix.isNull()) {
                loadedFrom = path;
                break;
            }
        }
    }

    if (!osmPix.isNull()) {
        qDebug() << "Successfully loaded Real OpenStreetMap Basemap from:" << loadedFrom << "Size:" << osmPix.size();
        graphScene->setSceneRect(0, 0, osmPix.width(), osmPix.height());
        graphScene->setBackgroundBrush(QBrush(QColor("#0f172a")));
        QGraphicsPixmapItem* bgPixItem = graphScene->addPixmap(osmPix);
        bgPixItem->setZValue(0);
    } else {
        qWarning() << "Could not load Real OpenStreetMap Basemap from any search path!";
        graphScene->setSceneRect(0, 0, 1024, 1792);
        graphScene->setBackgroundBrush(QBrush(QColor("#0f172a")));
    }

    // Notice: NO pre-drawn lines! User selects their source stop to calculate shortest path!

    // 2. Draw Selectable Student Pickup Stop Pins
    auto drawStopMarker = [&](int x, int y, const QString& name, const QString& id, const QString& time) {
        // Outer pin halo ring
        auto halo = graphScene->addEllipse(x - 12, y - 12, 24, 24, QPen(Qt::white, 2), QBrush(QColor("#2563eb")));
        halo->setZValue(5);

        // Center dot
        auto dot = graphScene->addEllipse(x - 4, y - 4, 8, 8, QPen(Qt::NoPen), QBrush(Qt::white));
        dot->setZValue(6);

        // Label pill background
        QString labelStr = QString("%1 (%2) | %3").arg(name).arg(id).arg(time);
        QFont font("Inter", 9, QFont::Bold);
        QFontMetrics fm(font);
        int textW = fm.horizontalAdvance(labelStr) + 16;
        int textH = fm.height() + 8;

        QPainterPath pillPath;
        pillPath.addRoundedRect(x - textW / 2, y + 14, textW, textH, 6, 6);
        auto pill = graphScene->addPath(pillPath, QPen(QColor("#cbd5e1"), 1), QBrush(QColor(255, 255, 255, 240)));
        pill->setZValue(7);

        QGraphicsTextItem* text = graphScene->addText(labelStr, font);
        text->setDefaultTextColor(QColor("#0f172a"));
        text->setPos(x - textW / 2 + 8, y + 16);
        text->setZValue(8);
    };

    drawStopMarker(415, 1340, "Haldwani Bus Terminal", "ST101", "07:30 AM");
    drawStopMarker(436, 1239, "Tikonia Chowk (Bareilly Rd)", "ST102", "07:38 AM");
    drawStopMarker(504, 981, "Kathgodam Railway Station", "ST103", "07:48 AM");
    drawStopMarker(559, 821, "Ranibagh HMT Junction", "ST104", "08:02 AM");
    drawStopMarker(401, 370, "Bhowali Sanatorium Chowk", "ST106", "08:15 AM");

    // 3. Draw Fixed College Destination: Graphic Era Hill University (GEHU), Bhimtal Campus at (557, 416)
    int gehuX = 557, gehuY = 416;
    auto gehuHalo = graphScene->addEllipse(gehuX - 18, gehuY - 18, 36, 36, QPen(QColor("#fbbf24"), 3), QBrush(QColor("#dc2626")));
    gehuHalo->setZValue(25);

    auto gehuDot = graphScene->addEllipse(gehuX - 7, gehuY - 7, 14, 14, QPen(Qt::NoPen), QBrush(Qt::white));
    gehuDot->setZValue(26);

    QString collegeLabel = "GRAPHIC ERA HILL UNIVERSITY (GEHU) - BHIMTAL CAMPUS";
    QFont collegeFont("Inter", 10, QFont::Black);
    QFontMetrics colFm(collegeFont);
    int cW = colFm.horizontalAdvance(collegeLabel) + 24;
    int cH = colFm.height() + 12;

    QPainterPath colPillPath;
    colPillPath.addRoundedRect(gehuX - cW / 2, gehuY - cH - 20, cW, cH, 8, 8);
    auto colPill = graphScene->addPath(colPillPath, QPen(QColor("#dc2626"), 2), QBrush(QColor(255, 255, 255, 250)));
    colPill->setZValue(27);

    QGraphicsTextItem* colText = graphScene->addText(collegeLabel, collegeFont);
    colText->setDefaultTextColor(QColor("#dc2626"));
    colText->setPos(gehuX - cW / 2 + 12, gehuY - cH - 16);
    colText->setZValue(28);

    // 4. Pinned Geographical Description Cards at Haldwani & Bhimtal Locations
    // Haldwani Base Transit Hub Pinned Card at (415, 1340)
    int haldX = 220, haldY = 1430, haldW = 380, haldH = 68;
    QPainterPath haldCardPath;
    haldCardPath.addRoundedRect(haldX, haldY, haldW, haldH, 8, 8);
    auto haldCard = graphScene->addPath(haldCardPath, QPen(QColor("#0284c7"), 2), QBrush(QColor(15, 23, 42, 235)));
    haldCard->setZValue(22);

    QGraphicsTextItem* hTitle = graphScene->addText("HALDWANI TRANSIT BASE (FOOTHILLS HUB)", QFont("Inter", 9, QFont::Bold));
    hTitle->setDefaultTextColor(QColor("#38bdf8"));
    hTitle->setPos(haldX + 12, haldY + 8);
    hTitle->setZValue(23);

    QGraphicsTextItem* hSub = graphScene->addText("Origin Boarding Terminal • NH-109 Nainital Highway\nElevation: 424m • Regular GEHU Student Shuttles", QFont("Inter", 8));
    hSub->setDefaultTextColor(QColor("#cbd5e1"));
    hSub->setPos(haldX + 12, haldY + 28);
    hSub->setZValue(23);

    auto hLine = graphScene->addLine(415, 1340, haldX + haldW / 2, haldY, QPen(QColor("#0284c7"), 2, Qt::DashLine));
    hLine->setZValue(21);

    // Bhimtal Campus Pinned Card at (557, 416)
    int bhimX = 350, bhimY = 230, bhimW = 420, bhimH = 75;
    QPainterPath bhimCardPath;
    bhimCardPath.addRoundedRect(bhimX, bhimY, bhimW, bhimH, 8, 8);
    auto bhimCard = graphScene->addPath(bhimCardPath, QPen(QColor("#dc2626"), 2), QBrush(QColor(15, 23, 42, 240)));
    bhimCard->setZValue(22);

    QGraphicsTextItem* bTitle = graphScene->addText("GRAPHIC ERA HILL UNIVERSITY (GEHU)", QFont("Inter", 10, QFont::Black));
    bTitle->setDefaultTextColor(QColor("#f87171"));
    bTitle->setPos(bhimX + 12, bhimY + 8);
    bTitle->setZValue(23);

    QGraphicsTextItem* bSub = graphScene->addText("Bhimtal Campus Destination • Sattal Road Corridor\nAltitude: 1,370m • Engineering & Management Blocks\nOfficial Terminus for All Morning Transit Shuttles", QFont("Inter", 8));
    bSub->setDefaultTextColor(QColor("#f8fafc"));
    bSub->setPos(bhimX + 12, bhimY + 28);
    bSub->setZValue(23);

    auto bLine = graphScene->addLine(557, 416, bhimX + bhimW / 2, bhimY + bhimH, QPen(QColor("#dc2626"), 2, Qt::DashLine));
    bLine->setZValue(21);

    // 5. Initial View Framing on the Haldwani - GEHU Bhimtal Corridor
    QTimer::singleShot(80, this, &MainWindow::fitCorridorView);

    // 6. Compute initial shortest route for currently selected stop
    onOptimizeRoute();
}

// SLOTS
void MainWindow::onScheduleChanged(int index) {
    sysManager.setScheduleType(index == 0 ? ScheduleType::FULL_DAY : ScheduleType::HALF_DAY);
    onSearchStudentSchedule();
}

void MainWindow::onSearchStudentSchedule() {
    QString roll = studentRollInput->text().trimmed();
    Student* s = sysManager.findStudentByRoll(roll.toStdString());

    if (!s) {
        studentInfoLabel->setText("<font color='#ef4444'><b>[ERROR]</b> Student Roll Number not found in college registry.</font>");
        scheduleTable->setRowCount(0);
        return;
    }

    // Resolve Assigned Pickup Stop Name & Route Details
    QString stopId = QString::fromStdString(s->getAssignedStopId());
    QString stopName = stopId;
    QString morningPickupTime = "07:30 AM";
    QString busId = "B-101";
    QString routeName = "Route R1 (Haldwani Express Corridor)";

    if (stopId == "ST101") {
        stopName = "Haldwani Bus Terminal (ST101)";
        morningPickupTime = (sysManager.getScheduleType() == ScheduleType::FULL_DAY ? "07:30 AM" : "07:15 AM");
        busId = "B-101";
        routeName = "Route R1 (Express Corridor)";
    } else if (stopId == "ST102") {
        stopName = "Tikonia Chowk, Bareilly Rd (ST102)";
        morningPickupTime = (sysManager.getScheduleType() == ScheduleType::FULL_DAY ? "07:38 AM" : "07:22 AM");
        busId = "B-101";
        routeName = "Route R1 (Express Corridor)";
    } else if (stopId == "ST103") {
        stopName = "Kathgodam Railway Station (ST103)";
        morningPickupTime = (sysManager.getScheduleType() == ScheduleType::FULL_DAY ? "07:48 AM" : "07:32 AM");
        busId = "B-101";
        routeName = "Route R1 (Express Corridor)";
    } else if (stopId == "ST104") {
        stopName = "Ranibagh HMT Junction (ST104)";
        morningPickupTime = (sysManager.getScheduleType() == ScheduleType::FULL_DAY ? "08:02 AM" : "07:45 AM");
        busId = "B-102";
        routeName = "Route R2 (Suburban Mountain Corridor)";
    } else if (stopId == "ST106") {
        stopName = "Bhowali Sanatorium Chowk (ST106)";
        morningPickupTime = (sysManager.getScheduleType() == ScheduleType::FULL_DAY ? "08:15 AM" : "07:58 AM");
        busId = "B-102";
        routeName = "Route R2 (Suburban Mountain Corridor)";
    }

    studentInfoLabel->setText(
        QString("<div style='background-color: #1e293b; border: 1px solid #334155; border-radius: 8px; padding: 12px;'>"
                "<table width='100%' cellpadding='4'>"
                "<tr>"
                "<td><span style='color: #94a3b8; font-size: 11px;'>STUDENT</span><br><b style='color: #f8fafc; font-size: 14px;'>%1</b></td>"
                "<td><span style='color: #94a3b8; font-size: 11px;'>ROLL ID</span><br><b style='color: #38bdf8; font-size: 14px;'>%2</b></td>"
                "<td><span style='color: #94a3b8; font-size: 11px;'>ASSIGNED PICKUP STOP</span><br><b style='color: #10b981; font-size: 15px;'>%3</b></td>"
                "<td><span style='color: #94a3b8; font-size: 11px;'>PICKUP TIME</span><br><b style='color: #fbbf24; font-size: 14px;'>%4</b></td>"
                "<td><span style='color: #94a3b8; font-size: 11px;'>SEMESTER FEE</span><br>%5</td>"
                "</tr>"
                "</table>"
                "</div>")
        .arg(QString::fromStdString(s->getName()))
        .arg(QString::fromStdString(s->getRollNumber()))
        .arg(stopName)
        .arg(morningPickupTime)
        .arg(s->isFeePaid() ? "<span style='color: #10b981; font-weight: bold;'>PAID (ACTIVE)</span>" : "<span style='color: #ef4444; font-weight: bold;'>PENDING</span>"));

    scheduleTable->setRowCount(2);
    // Row 1: Student Pickup Stop
    scheduleTable->setItem(0, 0, new QTableWidgetItem(busId));
    scheduleTable->setItem(0, 1, new QTableWidgetItem(routeName));
    scheduleTable->setItem(0, 2, new QTableWidgetItem(QString("Pickup: %1").arg(stopName)));

    QTableWidgetItem* pickupTimeItem = new QTableWidgetItem(morningPickupTime);
    pickupTimeItem->setFont(QFont("Inter", 10, QFont::Bold));
    pickupTimeItem->setForeground(QColor("#fbbf24"));
    scheduleTable->setItem(0, 3, pickupTimeItem);

    QTableWidgetItem* statusItem1 = new QTableWidgetItem(sysManager.getActiveDelayNotice().hasDelay ? "DELAYED (Notice Issued)" : "ON SCHEDULE");
    statusItem1->setForeground(sysManager.getActiveDelayNotice().hasDelay ? QColor("#ef4444") : QColor("#10b981"));
    scheduleTable->setItem(0, 4, statusItem1);

    // Row 2: Campus Destination Arrival
    scheduleTable->setItem(1, 0, new QTableWidgetItem(busId));
    scheduleTable->setItem(1, 1, new QTableWidgetItem("Campus Arrival (GEHU)"));
    scheduleTable->setItem(1, 2, new QTableWidgetItem("Graphic Era Hill University, Bhimtal Campus"));

    QString arrivalTime = (sysManager.getScheduleType() == ScheduleType::FULL_DAY ? "08:25 AM" : "08:05 AM");
    QTableWidgetItem* arrTimeItem = new QTableWidgetItem(arrivalTime);
    arrTimeItem->setFont(QFont("Inter", 10, QFont::Bold));
    arrTimeItem->setForeground(QColor("#38bdf8"));
    scheduleTable->setItem(1, 3, arrTimeItem);

    QTableWidgetItem* statusItem2 = new QTableWidgetItem("CAMPUS DESTINATION");
    statusItem2->setForeground(QColor("#10b981"));
    scheduleTable->setItem(1, 4, statusItem2);

    // Auto-select this student's pickup stop on the Route Map!
    if (startStopCombo) {
        for (int i = 0; i < startStopCombo->count(); ++i) {
            if (startStopCombo->itemData(i).toString() == stopId) {
                startStopCombo->setCurrentIndex(i);
                break;
            }
        }
    }
}

void MainWindow::onFindConnectingBuses() {
    onSearchStudentSchedule();
}

void MainWindow::onShowDigitalQRPass() {
    QString roll = studentRollInput->text();
    DigitalPass pass = sysManager.getStudentPass(roll.toStdString());
    QMessageBox::information(this, "Digital QR Bus Pass", 
                             QString(" <b>DIGITAL BUS PASS</b><br><br>"
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
    QMessageBox::information(this, "Rating Submitted", "[SUCCESS] Thank you! Your rating of " + QString::number(stars) + " stars for Driver " + driverId + " has been recorded.");
}

void MainWindow::onTriggerStudentSOS() {
    QString roll = studentRollInput->text().trimmed();
    sysManager.triggerStudentSOS(roll.toStdString(), "B-101", "Haldwani Bus Terminal");
    refreshComplaintTable();
    QMessageBox::critical(this, "[ALERT] EMERGENCY SOS ACTIVATED", "[ALERT] EMERGENCY SOS ALERT HAS BEEN SENT TO COLLEGE TRANSPORT ADMIN OFFICE!");
}

void MainWindow::onDriverStartTrip() {
    sysManager.driverStartTrip("B-101");
    QMessageBox::information(this, "Driver Trip Control", "[TRIP STARTED] Morning Trip for Bus B-101 has officially started!");
}

void MainWindow::onDriverEndTrip() {
    sysManager.driverEndTrip("B-101");
    QMessageBox::information(this, "Driver Trip Control", "[TRIP COMPLETED] Trip ended successfully. All passengers unboarded.");
}

void MainWindow::onDriverReportDelay() {
    QString reason = driverDelayReasonInput->text().trimmed();
    int mins = driverDelayMinsInput->text().toInt();
    if (mins <= 0) mins = 15;
    sysManager.driverReportDelay("B-101", reason.toStdString(), mins);
    onSearchStudentSchedule();
    QMessageBox::warning(this, "Delay Notice Dispatched",
                         QString("[TRANSIT DELAY NOTICE DISPATCHED]\n\n"
                                 "Bus B-101 reported delayed by %1 mins.\n"
                                 "Reason: %2\n\n"
                                 "A delay alert banner and pop-up will appear on all student portals.")
                         .arg(mins).arg(reason));
}

void MainWindow::onDriverReportMisbehavior() {
    QString roll = misbehaviorRollInput->text().trimmed();
    QString desc = misbehaviorDescInput->text().trimmed();
    sysManager.driverReportMisbehavior("Driver Rajesh Kumar (B-101)", roll.toStdString(), desc.toStdString());
    refreshComplaintTable();
    QMessageBox::information(this, "Misbehavior Logged", "[INCIDENT LOGGED] Misbehavior report for student " + roll + " submitted directly to Admin Office.");
}

void MainWindow::onAdminUpdateBus() {
    QString busId = adminBusCombo->currentText();
    QString regNo = adminRegInput->text();
    int cap = adminCapInput->text().toInt();
    QString routeId = adminRouteCombo->currentText();

    if (sysManager.adminUpdateBus(busId.toStdString(), regNo.toStdString(), cap, routeId.toStdString())) {
        QMessageBox::information(this, "Admin Update", "[SUCCESS] Admin Update Successful: Bus " + busId + " details updated!");
        refreshFleetTable();
        refreshAttendanceTable();
    }
}

void MainWindow::onAdminUpdateTiming() {
    QString stopId = adminTimingStopCombo->currentData().toString();
    QString busId = adminTimingBusCombo->currentText();
    ScheduleType sType = (adminTimingScheduleCombo->currentIndex() == 1) ? ScheduleType::HALF_DAY : ScheduleType::FULL_DAY;
    QString newTime = adminNewTimeInput->text();

    if (sysManager.adminUpdateStopTiming(stopId.toStdString(), busId.toStdString(), sType, newTime.toStdString())) {
        QMessageBox::information(this, "Admin Timing Update", "[SUCCESS] Departure time for Bus " + busId + " set to " + newTime + "!");
    }
}

void MainWindow::onAdminBroadcastNotice() {
    sysManager.adminBroadcastNotice(TargetAudience::ALL_STUDENTS, "College Transport Notice", "All bus routes operating on schedule.");
    QMessageBox::information(this, "Broadcast Dispatched", "[NOTICE SENT] Announcement broadcasted to all students and parents!");
}

void MainWindow::refreshAttendanceTable() {
    if (!adminAttendanceTable) return;
    const auto& records = sysManager.getAttendanceLogger().getRecords();
    adminAttendanceTable->setRowCount(records.size());

    int successCount = 0;
    for (size_t i = 0; i < records.size(); ++i) {
        // Show newest records at the top
        const auto& rec = records[records.size() - 1 - i];
        if (rec.status.find("SUCCESS") != string::npos) successCount++;

        Student* s = sysManager.findStudentByRoll(rec.studentRoll);
        QString studentName = s ? QString::fromStdString(s->getName()) : "Registered Student";
        QString feeStatus = (s && s->isFeePaid()) ? "PAID" : "PENDING";

        adminAttendanceTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(rec.timestamp)));
        adminAttendanceTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(rec.studentRoll)));
        adminAttendanceTable->setItem(i, 2, new QTableWidgetItem(studentName));
        adminAttendanceTable->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(rec.busId)));
        adminAttendanceTable->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(rec.stopId)));

        QTableWidgetItem* feeItem = new QTableWidgetItem(feeStatus);
        feeItem->setForeground(feeStatus == "PAID" ? QColor("#10b981") : QColor("#ef4444"));
        adminAttendanceTable->setItem(i, 5, feeItem);

        QTableWidgetItem* statusItem = new QTableWidgetItem(QString::fromStdString(rec.status));
        if (rec.status.find("SUCCESS") != string::npos) {
            statusItem->setForeground(QColor("#10b981"));
        } else {
            statusItem->setForeground(QColor("#ef4444"));
        }
        adminAttendanceTable->setItem(i, 6, statusItem);
    }

    if (statAttendanceCountLabel) {
        statAttendanceCountLabel->setText(QString("%1 Verified Boardings").arg(successCount));
    }
}

void MainWindow::onAdminMarkAttendance() {
    QString roll = adminBoardRollCombo ? adminBoardRollCombo->currentData().toString() : "2024-CS-001";
    QString busId = adminBoardBusCombo ? adminBoardBusCombo->currentText() : "B-101";

    try {
        sysManager.processBoarding(roll.toStdString(), busId.toStdString());
        refreshAttendanceTable();
        refreshFleetTable();
        refreshAttendanceTable();
        if (adminAttendanceStatusLabel) {
            adminAttendanceStatusLabel->setText(QString("<font color='#10b981'><b>[ATTENDANCE RECORDED]</b> Student %1 checked into Bus %2 successfully.</font>").arg(roll).arg(busId));
        }
        QMessageBox::information(this, "Attendance Logged",
                                 QString("[SUCCESS] Student Attendance & Boarding Verified!\n\n"
                                         "Student Roll: %1\n"
                                         "Bus Assigned: %2\n"
                                         "Status: SUCCESS_BOARDED\n"
                                         "Parent SMS alert sent.").arg(roll).arg(busId));
    } catch (const SeatFullException& ex) {
        refreshAttendanceTable();
        if (adminAttendanceStatusLabel) {
            adminAttendanceStatusLabel->setText(QString("<font color='#ef4444'><b>[DENIED]</b> Bus %1 is full.</font>").arg(busId));
        }
        QMessageBox::warning(this, "Boarding Denied", ex.what());
    }
}

void MainWindow::onAdminScanQRAttendance() {
    QString roll = adminBoardRollCombo ? adminBoardRollCombo->currentData().toString() : "2024-CS-001";
    QString busId = adminBoardBusCombo ? adminBoardBusCombo->currentText() : "B-101";

    Student* s = sysManager.findStudentByRoll(roll.toStdString());
    if (!s) {
        QMessageBox::warning(this, "Scan Failed", "Student not found in college registry.");
        return;
    }

    DigitalPass pass = sysManager.getStudentPass(roll.toStdString());

    if (pass.isValid && s->isFeePaid()) {
        try {
            sysManager.processBoarding(roll.toStdString(), busId.toStdString());
            refreshAttendanceTable();
            refreshFleetTable();
            if (adminAttendanceStatusLabel) {
                adminAttendanceStatusLabel->setText(QString("<font color='#10b981'><b>[QR PASS VERIFIED]</b> Digital Pass authentic. Student %1 boarded Bus %2.</font>").arg(roll).arg(busId));
            }
            QMessageBox::information(this, "Digital QR Pass Verified",
                                     QString("[QR PASS VERIFIED & ATTENDANCE LOGGED]\n\n"
                                             "Digital Pass Hash: %1\n"
                                             "Student: %2 (%3)\n"
                                             "Fee Status: PAID (Valid for Semester)\n"
                                             "Assigned Bus: %4\n"
                                             "Attendance logged in database.")
                                     .arg(QString::fromStdString(pass.verificationHash))
                                     .arg(QString::fromStdString(s->getName()))
                                     .arg(roll)
                                     .arg(busId));
        } catch (const SeatFullException& ex) {
            QMessageBox::warning(this, "Boarding Denied", ex.what());
        }
    } else {
        if (adminAttendanceStatusLabel) {
            adminAttendanceStatusLabel->setText(QString("<font color='#ef4444'><b>[PASS INVALID]</b> Bus fee unpaid for student %1.</font>").arg(roll));
        }
        QMessageBox::warning(this, "QR Verification Denied", "Digital QR Pass is suspended: College bus transport fee is PENDING!");
    }
}

void MainWindow::onParentTrackChild() {
    QString phone = parentPhoneInput->text();
    Parent* p = sysManager.findParentByPhoneOrChild(phone.toStdString());

    parentInfoLabel->setText(QString("<b>LIVE PARENT CHILD TRACKING SYSTEM</b><br><br>"
                                     "<b>Parent Name:</b> %1<br>"
                                     "<b>Child Student:</b> %2 (%3)<br>"
                                     "<b>Bus Assigned:</b> Bus B-101 (Express Route R1)<br>"
                                     "<b>Current Location:</b> Kathgodam Railway Station (ST102)<br>"
                                     "<b>ETA to College:</b> 08:25 AM (<font color='green'>ON TIME [SUCCESS]</font>)<br>"
                                     "<b>Fee Balance:</b> %4")
                             .arg(p ? QString::fromStdString(p->getName()) : "Vijay Sharma")
                             .arg(p ? QString::fromStdString(p->getChildName()) : "Aarav Sharma")
                             .arg(p ? QString::fromStdString(p->getChildRollNumber()) : "2024-CS-001")
                             .arg(p && p->getFeeBalance() > 0 ? "INR " + QString::number(p->getFeeBalance()) + " Due" : "<font color='green'>PAID IN FULL</font>"));
}

void MainWindow::onOptimizeRoute() {
    QString startId = startStopCombo->currentData().toString();
    QString startName = startStopCombo->currentText();
    QString algo = algorithmCombo ? algorithmCombo->currentData().toString() : "ASTAR";

    // 1. Remove previous shortest route path and highlight markers
    if (optimalRoutePathItem) {
        graphScene->removeItem(optimalRoutePathItem);
        delete optimalRoutePathItem;
        optimalRoutePathItem = nullptr;
    }
    if (optimalRouteGlowItem) {
        graphScene->removeItem(optimalRouteGlowItem);
        delete optimalRouteGlowItem;
        optimalRouteGlowItem = nullptr;
    }
    if (sourceHighlightMarker) {
        graphScene->removeItem(sourceHighlightMarker);
        delete sourceHighlightMarker;
        sourceHighlightMarker = nullptr;
    }
    for (auto* item : searchVisItems) {
        graphScene->removeItem(item);
        delete item;
    }
    searchVisItems.clear();

    // 2. Run selected Algorithm (A* or Dijkstra)
    if (algo == "DIJKSTRA") {
        currentMetrics = RouteOptimizer::runDijkstra(startId.toStdString(), "ST105");
    } else {
        currentMetrics = RouteOptimizer::runAStar(startId.toStdString(), "ST105");
    }

    // 3. Extract road waypoints along optimal path
    const auto& adj = RouteOptimizer::getGraphAdjList();
    vector<pair<int, int>> waypoints;

    for (size_t i = 0; i + 1 < currentMetrics.optimalPath.size(); ++i) {
        string u = currentMetrics.optimalPath[i];
        string v = currentMetrics.optimalPath[i + 1];
        auto it = adj.find(u);
        if (it != adj.end()) {
            for (const auto& edge : it->second) {
                if (edge.targetNodeId == v) {
                    if (waypoints.empty()) {
                        waypoints = edge.waypoints;
                    } else {
                        waypoints.insert(waypoints.end(), edge.waypoints.begin() + 1, edge.waypoints.end());
                    }
                    break;
                }
            }
        }
    }

    if (waypoints.empty()) {
        waypoints = {{415, 1340}, {557, 416}};
    }
    activeSimWaypoints = waypoints;

    int srcX = waypoints.front().first;
    int srcY = waypoints.front().second;

    int etaMins = currentMetrics.totalTimeMinutes;
    if (scheduleComboBox && scheduleComboBox->currentIndex() == 1) {
        etaMins = static_cast<int>(etaMins * 0.85);
    }

    // 4. Highlight Selected Source Node with Pulsing Emerald Ring
    sourceHighlightMarker = graphScene->addEllipse(srcX - 18, srcY - 18, 36, 36, QPen(QColor("#10b981"), 4), QBrush(Qt::NoBrush));
    sourceHighlightMarker->setZValue(12);

    // 5. Draw Shortest Road Path with Glowing Underlay
    QPainterPath optPath;
    optPath.moveTo(waypoints[0].first, waypoints[0].second);
    for (size_t i = 1; i < waypoints.size(); ++i) {
        optPath.lineTo(waypoints[i].first, waypoints[i].second);
    }

    QPen glowPen(QColor(16, 185, 129, 90), 12, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    optimalRouteGlowItem = graphScene->addPath(optPath, glowPen);
    optimalRouteGlowItem->setZValue(9);

    QPen optPen(QColor("#10b981"), 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    optimalRoutePathItem = graphScene->addPath(optPath, optPen);
    optimalRoutePathItem->setZValue(10);

    // 6. Position Live Bus Marker at Selected Source Stop
    if (!busMarker) {
        busMarker = graphScene->addEllipse(srcX - 12, srcY - 12, 24, 24, QPen(Qt::white, 3), QBrush(QColor("#f59e0b")));
        busMarker->setZValue(20);
        QGraphicsTextItem* busLabel = graphScene->addText("BUS", QFont("Inter", 7, QFont::Black));
        busLabel->setDefaultTextColor(Qt::white);
        busLabel->setParentItem(busMarker);
        busLabel->setPos(3, 4);
    } else {
        busMarker->setRect(srcX - 12, srcY - 12, 24, 24);
        busMarker->setPos(0, 0);
        busMarker->setVisible(true);
    }

    // 7. Update Detailed Performance & Route Banner
    routeInfoLabel->setText(QString("[%1] Selected Source: <b>%2</b> -> Destination: <b>Graphic Era Hill University, Bhimtal</b> | Distance: <b>%3 km</b> | ETA: <b>%4 mins</b> | Nodes Evaluated: <b>%5</b> | Exec Time: <b>%6 ms</b>")
                            .arg(QString::fromStdString(currentMetrics.algorithmName))
                            .arg(startName)
                            .arg(currentMetrics.totalDistanceKm, 0, 'f', 1)
                            .arg(etaMins)
                            .arg(currentMetrics.nodesExploredCount)
                            .arg(currentMetrics.executionTimeMs, 0, 'f', 3));
}

void MainWindow::onVisualizeAlgorithmSearch() {
    QString startId = startStopCombo->currentData().toString();
    QString algo = algorithmCombo ? algorithmCombo->currentData().toString() : "ASTAR";

    // Run chosen algorithm
    if (algo == "DIJKSTRA") {
        currentMetrics = RouteOptimizer::runDijkstra(startId.toStdString(), "ST105");
    } else {
        currentMetrics = RouteOptimizer::runAStar(startId.toStdString(), "ST105");
    }

    // Clear previous visual items
    if (optimalRoutePathItem) {
        graphScene->removeItem(optimalRoutePathItem);
        delete optimalRoutePathItem;
        optimalRoutePathItem = nullptr;
    }
    if (optimalRouteGlowItem) {
        graphScene->removeItem(optimalRouteGlowItem);
        delete optimalRouteGlowItem;
        optimalRouteGlowItem = nullptr;
    }
    for (auto* item : searchVisItems) {
        graphScene->removeItem(item);
        delete item;
    }
    searchVisItems.clear();

    if (visTimer->isActive()) visTimer->stop();
    visStep = 0;
    visTimer->start(350); // 350ms per exploration step animation
}

void MainWindow::onVisStep() {
    const auto& nodes = RouteOptimizer::getGraphNodes();

    if (visStep < (int)currentMetrics.exploredNodes.size()) {
        string nodeId = currentMetrics.exploredNodes[visStep];
        auto it = nodes.find(nodeId);
        if (it != nodes.end()) {
            int nx = it->second.pixelX;
            int ny = it->second.pixelY;

            // Search exploration ring (Purple / Amber)
            auto ring = graphScene->addEllipse(nx - 20, ny - 20, 40, 40, QPen(QColor("#7c3aed"), 3, Qt::DashLine), QBrush(QColor(124, 58, 237, 45)));
            ring->setZValue(11);
            searchVisItems.push_back(ring);

            // Step number indicator pill
            QString stepStr = QString("Step %1").arg(visStep + 1);
            QFont sFont("Inter", 7, QFont::Bold);
            QFontMetrics fm(sFont);
            int sW = fm.horizontalAdvance(stepStr) + 8;
            int sH = fm.height() + 4;

            QPainterPath sPill;
            sPill.addRoundedRect(nx - sW / 2, ny - 28, sW, sH, 4, 4);
            auto sItem = graphScene->addPath(sPill, QPen(QColor("#7c3aed"), 1), QBrush(QColor(124, 58, 237)));
            sItem->setZValue(12);
            searchVisItems.push_back(sItem);

            auto sText = graphScene->addText(stepStr, sFont);
            sText->setDefaultTextColor(Qt::white);
            sText->setPos(nx - sW / 2 + 4, ny - 28 + 2);
            sText->setZValue(13);
            searchVisItems.push_back(sText);

            routeInfoLabel->setText(QString("[SEARCH VISUALIZATION (%1)] Step %2/%3: Exploring Node <b>%4 (%5)</b>...")
                                    .arg(QString::fromStdString(currentMetrics.algorithmName))
                                    .arg(visStep + 1)
                                    .arg(currentMetrics.exploredNodes.size())
                                    .arg(QString::fromStdString(it->second.name))
                                    .arg(QString::fromStdString(it->second.id)));
        }
        visStep++;
    } else {
        // Visualization complete: draw optimal path
        visTimer->stop();
        onOptimizeRoute();
        routeInfoLabel->setText(QString("[VISUALIZATION COMPLETE: %1] Explored %2 nodes. Optimal Shortest Road Path to GEHU Bhimtal highlighted in green!")
                                .arg(QString::fromStdString(currentMetrics.algorithmName))
                                .arg(currentMetrics.nodesExploredCount));
    }
}

void MainWindow::onStartLiveSim() {
    if (activeSimWaypoints.empty()) {
        onOptimizeRoute();
    }
    simStep = 0;
    if (simTimer->isActive()) simTimer->stop();
    simTimer->start(250);
}

void MainWindow::onSimStep() {
    if (activeSimWaypoints.empty()) {
        simTimer->stop();
        return;
    }

    if (simStep >= (int)activeSimWaypoints.size()) {
        simTimer->stop();
        routeInfoLabel->setText("[SUCCESS] Student safely arrived at Graphic Era Hill University (GEHU), Bhimtal Campus!");
        return;
    }

    int px = activeSimWaypoints[simStep].first;
    int py = activeSimWaypoints[simStep].second;

    if (busMarker) {
        busMarker->setRect(px - 12, py - 12, 24, 24);
        busMarker->setPos(0, 0);
        busMarker->setZValue(22);
    }

    int pct = (simStep + 1) * 100 / activeSimWaypoints.size();
    routeInfoLabel->setText(QString("[IN TRANSIT (%1%)] Live Road Simulation: Waypoint %2/%3 to Graphic Era Hill University Bhimtal | Speed: 42 km/h")
                            .arg(pct)
                            .arg(simStep + 1)
                            .arg(activeSimWaypoints.size()));

    graphView->centerOn(px, py);
    simStep++;
}

void MainWindow::fitCorridorView() {
    if (!graphView || !graphScene) return;
    int vpW = graphView->viewport()->width();
    int vpH = graphView->viewport()->height();
    if (vpW < 80 || vpH < 80) {
        QTimer::singleShot(80, this, &MainWindow::fitCorridorView);
        return;
    }

    graphView->resetTransform();

    // Full real map: 1024 x 1792.
    // Road corridor spans Haldwani (415, 1340) to GEHU Bhimtal (557, 416).
    // Framing rect across full width to eliminate black side bars:
    QRectF corridorRect(0, 200, 1024, 1380);
    graphView->fitInView(corridorRect, Qt::KeepAspectRatioByExpanding);
    graphView->centerOn(512, 890);
}

void MainWindow::onFitCorridor() {
    fitCorridorView();
}

void MainWindow::onZoomIn() {
    graphView->scale(1.2, 1.2);
}

void MainWindow::onZoomOut() {
    graphView->scale(1.0 / 1.2, 1.0 / 1.2);
}

void MainWindow::onOpenWebMap() {
    QString webPath = QDir::current().filePath("web/index.html");
    QDesktopServices::openUrl(QUrl::fromLocalFile(webPath));
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (graphView && watched == graphView->viewport()) {
        if (event->type() == QEvent::Wheel) {
            QWheelEvent* we = static_cast<QWheelEvent*>(event);
            double factor = we->angleDelta().y() > 0 ? 1.15 : (1.0 / 1.15);
            graphView->scale(factor, factor);
            return true;
        }
        if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent* me = static_cast<QMouseEvent*>(event);
            if (me->button() == Qt::LeftButton) {
                QPointF p = graphView->mapToScene(me->pos());
                struct StopPoint { QString id; int x; int y; };
                vector<StopPoint> pts = {
                    {"ST101", 415, 1340},
                    {"ST102", 436, 1239},
                    {"ST103", 504, 981},
                    {"ST104", 559, 821},
                    {"ST106", 401, 370}
                };
                for (const auto& sp : pts) {
                    double dist = std::hypot(p.x() - sp.x, p.y() - sp.y);
                    double dx = std::abs(p.x() - sp.x);
                    double dy = p.y() - sp.y;
                    bool inCircle = (dist <= 50.0);
                    bool inPill = (dx <= 95.0 && dy >= 0.0 && dy <= 50.0);
                    if (inCircle || inPill) {
                        for (int i = 0; i < startStopCombo->count(); ++i) {
                            if (startStopCombo->itemData(i).toString() == sp.id) {
                                if (startStopCombo->currentIndex() != i) {
                                    startStopCombo->setCurrentIndex(i);
                                } else {
                                    onOptimizeRoute();
                                }
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::onBoardStudent() {
    QString roll = boardRollInput->text().trimmed();
    QString busId = boardBusCombo->currentText();

    try {
        sysManager.processBoarding(roll.toStdString(), busId.toStdString());
        QMessageBox::information(this, "Boarding Status", "[SUCCESS] Student successfully boarded Bus " + busId + "!");
        refreshFleetTable();
        refreshAttendanceTable();
    } catch (const SeatFullException& ex) {
        QMessageBox::warning(this, "Boarding Denied (Exception Caught)", ex.what());
    }
}

void MainWindow::refreshComplaintTable() {
    if (!complaintTable) return;
    const auto& list = sysManager.getComplaintEngine().getComplaints();
    complaintTable->setRowCount(list.size());

    for (size_t i = 0; i < list.size(); ++i) {
        const auto& c = list[list.size() - 1 - i];

        QString typeStr = "STUDENT COMPLAINT";
        if (c.type == ComplaintType::DRIVER_TO_STUDENT) typeStr = "DRIVER MISBEHAVIOR REPORT";
        else if (c.type == ComplaintType::PARENT_TO_TRANSPORT) typeStr = "PARENT INQUIRY";
        else if (c.type == ComplaintType::SOS_EMERGENCY) typeStr = "[SOS] EMERGENCY ALERT";

        QString statusStr = "OPEN";
        QColor statusColor = QColor("#fbbf24");
        if (c.status == ComplaintStatus::INVESTIGATING) {
            statusStr = "INVESTIGATING";
            statusColor = QColor("#38bdf8");
        } else if (c.status == ComplaintStatus::WARNING_ISSUED) {
            statusStr = "WARNING ISSUED";
            statusColor = QColor("#f97316");
        } else if (c.status == ComplaintStatus::RESOLVED || c.status == ComplaintStatus::CLOSED) {
            statusStr = "RESOLVED";
            statusColor = QColor("#10b981");
        }

        complaintTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(c.complaintId)));

        QTableWidgetItem* tItem = new QTableWidgetItem(typeStr);
        if (c.type == ComplaintType::SOS_EMERGENCY) tItem->setForeground(QColor("#ef4444"));
        complaintTable->setItem(i, 1, tItem);

        complaintTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(c.reporterName)));
        complaintTable->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(c.targetName)));
        complaintTable->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(c.busId)));
        complaintTable->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(c.category)));
        complaintTable->setItem(i, 6, new QTableWidgetItem(QString::fromStdString(c.description)));

        QTableWidgetItem* sItem = new QTableWidgetItem(statusStr);
        sItem->setFont(QFont("Inter", 10, QFont::Bold));
        sItem->setForeground(statusColor);
        complaintTable->setItem(i, 7, sItem);
    }
}

void MainWindow::checkAndShowDelayNotice() {
    const auto& notice = sysManager.getActiveDelayNotice();
    if (notice.hasDelay) {
        if (studentDelayBannerLabel) {
            studentDelayBannerLabel->setText(
                QString("<b>[ATTENTION: BUS DELAY NOTICE]</b> Bus %1 is running <b>%2 minutes late</b> due to '%3'. Scheduled pickup times have been adjusted.")
                .arg(QString::fromStdString(notice.busId))
                .arg(notice.minutesDelayed)
                .arg(QString::fromStdString(notice.reason))
            );
            studentDelayBannerLabel->setVisible(true);
        }
        QMessageBox::warning(this, "BUS DELAY ALERT",
            QString("<b>[COLLEGE TRANSPORT DELAY NOTICE]</b><br><br>"
                    "<b>Bus ID:</b> %1<br>"
                    "<b>Delay Duration:</b> %2 Minutes<br>"
                    "<b>Reason:</b> %3<br><br>"
                    "<i>Please adjust your arrival time at your pickup stop accordingly.</i>")
            .arg(QString::fromStdString(notice.busId))
            .arg(notice.minutesDelayed)
            .arg(QString::fromStdString(notice.reason)));
    } else {
        if (studentDelayBannerLabel) {
            studentDelayBannerLabel->setVisible(false);
        }
    }
}

void MainWindow::onStudentReportDriver() {
    QString roll = studentRollInput->text().trimmed();

    bool ok;
    QStringList categories = {
        "Rash / Reckless Driving",
        "Overspeeding on Mountain Bend",
        "Unscheduled Halt / Skipping Stop",
        "Rude Behavior / Argument",
        "AC / Seating / Cleanliness Issue"
    };
    QString category = QInputDialog::getItem(this, "Report Driver / Incident", "Select Complaint Category:", categories, 0, false, &ok);
    if (!ok || category.isEmpty()) return;

    QString details = QInputDialog::getText(this, "Incident Details", "Enter specific incident details:", QLineEdit::Normal, "Driver was driving recklessly near curve.", &ok);
    if (!ok || details.isEmpty()) return;

    sysManager.studentReportDriver(roll.toStdString(), "B-101", category.toStdString(), details.toStdString());
    refreshComplaintTable();
    QMessageBox::information(this, "Complaint Registered",
        QString("[COMPLAINT REGISTERED]\n\n"
                "Your incident report against Driver of Bus B-101 has been logged and forwarded directly to the Transport Admin Control Center.\n\n"
                "Category: %1\n"
                "Details: %2")
        .arg(category).arg(details));
}

void MainWindow::onAdminRegisterStudent() {
    QString name = adminNewStudentNameInput->text().trimmed();
    QString roll = adminNewStudentRollInput->text().trimmed();
    QString phone = adminNewStudentPhoneInput->text().trimmed();
    QString stopId = adminNewStudentStopCombo->currentData().toString();
    QString busId = adminNewStudentBusCombo->currentData().toString();
    bool feePaid = (adminNewStudentFeeCombo->currentData().toString() == "PAID");

    if (name.isEmpty() || roll.isEmpty()) {
        QMessageBox::warning(this, "Registration Incomplete", "Please enter both student name and roll number.");
        return;
    }

    string routeId = (stopId == "ST104" || stopId == "ST106") ? "R2" : "R1";
    string email = roll.toLower().toStdString() + "@gehu.ac.in";
    string id = "S" + to_string(rand() % 900 + 100);

    Student newStudent(id, name.toStdString(), phone.toStdString(), email, roll.toStdString(), routeId, stopId.toStdString(), feePaid);
    sysManager.adminRegisterNewStudent(newStudent);

    QString entry = QString("%1 - %2 (%3)").arg(roll).arg(name).arg(stopId);
    if (adminStudentLookupCombo) {
        adminStudentLookupCombo->addItem(entry, roll);
        adminStudentLookupCombo->setCurrentIndex(adminStudentLookupCombo->count() - 1);
    }
    if (adminBoardRollCombo) {
        adminBoardRollCombo->addItem(entry, roll);
    }

    refreshAttendanceTable();
    QMessageBox::information(this, "Student Registered",
        QString("[SUCCESS] Student <b>%1</b> (%2) successfully registered in transport system!<br><br>"
                "<b>Assigned Stop:</b> %3<br>"
                "<b>Assigned Bus:</b> %4<br>"
                "<b>Fee Status:</b> %5<br>"
                "<b>Credentials:</b> Username: %2 | Password: student123")
        .arg(name).arg(roll).arg(stopId).arg(busId).arg(feePaid ? "PAID" : "PENDING"));
}

void MainWindow::onAdminLookupStudentChanged(int idx) {
    if (!adminStudentLookupCombo || idx < 0) return;
    QString roll = adminStudentLookupCombo->itemData(idx).toString();
    Student* s = sysManager.findStudentByRoll(roll.toStdString());
    if (!s) return;

    DigitalPass pass = sysManager.getStudentPass(roll.toStdString());
    QString feeStr = s->isFeePaid() ? "<span style='color: #10b981; font-weight: bold;'>PAID (ACTIVE PASS)</span>" : "<span style='color: #ef4444; font-weight: bold;'>FEE PENDING</span>";

    adminStudentLookupInfoLabel->setText(
        QString("<table width='100%' cellpadding='3'>"
                "<tr><td><b>Name:</b> %1</td><td><b>Roll No:</b> %2</td><td><b>Phone:</b> %3</td></tr>"
                "<tr><td><b>Boarding Stop:</b> %4</td><td><b>Assigned Bus:</b> %5</td><td><b>Fee Status:</b> %6</td></tr>"
                "<tr><td colspan='3'><b>Digital Pass Hash:</b> <code style='color: #38bdf8;'>%7</code></td></tr>"
                "</table>")
        .arg(QString::fromStdString(s->getName()))
        .arg(QString::fromStdString(s->getRollNumber()))
        .arg(QString::fromStdString(s->getPhone()))
        .arg(QString::fromStdString(s->getAssignedStopId()))
        .arg(QString::fromStdString(s->getAssignedRouteId() == "R2" ? "B-102" : "B-101"))
        .arg(feeStr)
        .arg(QString::fromStdString(pass.verificationHash))
    );
}

void MainWindow::onAdminResolveComplaint() {
    if (!complaintTable) return;
    int row = complaintTable->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Select Complaint", "Please select a complaint row from the table to resolve.");
        return;
    }
    QString cid = complaintTable->item(row, 0)->text();
    sysManager.getComplaintEngine().updateComplaintStatus(cid.toStdString(), ComplaintStatus::RESOLVED, "Resolved and logged by Transport Admin");
    refreshComplaintTable();
    QMessageBox::information(this, "Complaint Resolved", QString("[SUCCESS] Complaint %1 has been marked as RESOLVED.").arg(cid));
}
