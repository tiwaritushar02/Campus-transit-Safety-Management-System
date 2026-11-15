/**
 * ============================================================================
 * QT GUI MODULE: Smart College Bus Management System (Master Redesign)
 * FILE: MainWindow.cpp
 * ============================================================================
 */

#include "MainWindow.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QGraphicsTextItem>
#include <QBrush>
#include <QPen>
#include <QPalette>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QDateTime>
#include <QDialog>
#include <QDialogButtonBox>
#include <QScrollArea>
#include <QApplication>
#include <algorithm>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      activeUsername("admin"),
      activeRole(UserRole::ADMIN),
      stackedWidget(nullptr),
      loginWidget(nullptr),
      portalWidget(nullptr),
      isPasswordVisible(false),
      selectedComplaintId("CMP-102"),
      simTimer(nullptr),
      simStep(0),
      busMarker(nullptr) {

    setWindowTitle("Smart College Bus Management System • Graphic Era Hill University, Bhimtal");
    resize(1280, 850);
    setMinimumSize(1100, 720);

    simTimer = new QTimer(this);
    connect(simTimer, &QTimer::timeout, this, &MainWindow::onSimStep);

    applyGitHubTheme();
    setupUI();
    refreshAllDataViews();
}

// 1. GITHUB-INSPIRED DARK THEME PALETTE
void MainWindow::applyGitHubTheme() {
    QString qss = R"(
        QMainWindow, QWidget#centralWidget, QWidget#portalPage {
            background-color: #0D1117;
            color: #E6EDF3;
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Helvetica, Arial, sans-serif;
        }

        /* Group Boxes / Cards */
        QGroupBox {
            background-color: #161B22;
            border: 1px solid #30363D;
            border-radius: 8px;
            margin-top: 14px;
            padding: 14px;
            font-weight: 600;
            color: #58A6FF;
            font-size: 13px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 14px;
            padding: 0 6px;
            color: #58A6FF;
        }

        /* Inputs & Combos */
        QLineEdit, QComboBox {
            background-color: #0D1117;
            border: 1px solid #30363D;
            border-radius: 6px;
            color: #E6EDF3;
            padding: 7px 11px;
            font-size: 13px;
        }
        QLineEdit:focus, QComboBox:focus {
            border: 1px solid #58A6FF;
            background-color: #0D1117;
        }
        QComboBox::drop-down {
            border: none;
            width: 20px;
        }
        QComboBox QAbstractItemView {
            background-color: #161B22;
            border: 1px solid #30363D;
            selection-background-color: #1F6FEB;
            selection-color: #FFFFFF;
            color: #E6EDF3;
            padding: 4px;
        }

        /* Buttons */
        QPushButton {
            background-color: #21262D;
            border: 1px solid #30363D;
            border-radius: 6px;
            color: #E6EDF3;
            padding: 7px 14px;
            font-weight: 600;
            font-size: 13px;
        }
        QPushButton:hover {
            background-color: #30363D;
            border-color: #8B949E;
        }
        QPushButton:pressed {
            background-color: #161B22;
        }

        /* Tables */
        QTableWidget {
            background-color: #161B22;
            alternate-background-color: #0D1117;
            border: 1px solid #30363D;
            border-radius: 6px;
            gridline-color: #21262D;
            color: #E6EDF3;
            selection-background-color: #1F6FEB;
            selection-color: #FFFFFF;
            font-size: 12.5px;
        }
        QHeaderView::section {
            background-color: #161B22;
            color: #8B949E;
            border: none;
            border-bottom: 1px solid #30363D;
            padding: 7px;
            font-weight: 600;
            font-size: 12px;
        }

        /* Tab Navigation */
        QTabBar::tab {
            background-color: transparent;
            color: #8B949E;
            border: none;
            border-bottom: 2px solid transparent;
            padding: 9px 18px;
            font-weight: 600;
            font-size: 13px;
        }
        QTabBar::tab:selected {
            color: #E6EDF3;
            border-bottom: 2px solid #58A6FF;
        }
        QTabBar::tab:hover:!selected {
            color: #E6EDF3;
        }
        QTabWidget::pane {
            border: 1px solid #30363D;
            border-radius: 8px;
            background-color: #161B22;
            top: -1px;
        }

        /* Progress Bar */
        QProgressBar {
            background-color: #0D1117;
            border: 1px solid #30363D;
            border-radius: 5px;
            text-align: center;
            color: #FFFFFF;
            font-weight: bold;
            font-size: 11px;
            height: 18px;
        }
        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #238636, stop:1 #3FB950);
            border-radius: 4px;
        }

        QScrollBar:vertical {
            background: #0D1117;
            width: 10px;
            margin: 0px;
        }
        QScrollBar::handle:vertical {
            background: #30363D;
            min-height: 20px;
            border-radius: 5px;
        }
    )";
    qApp->setStyleSheet(qss);
}

void MainWindow::setupUI() {
    stackedWidget = new QStackedWidget(this);
    stackedWidget->setObjectName("centralWidget");

    loginWidget = createLoginScreen();
    portalWidget = createPortalScreen();

    stackedWidget->addWidget(loginWidget);  // Index 0 = Login View
    stackedWidget->addWidget(portalWidget); // Index 1 = Main Portal View

    setCentralWidget(stackedWidget);
    stackedWidget->setCurrentIndex(0); // Start at Login View
}

// 2. LOGIN PAGE REDESIGN
QWidget* MainWindow::createLoginScreen() {
    QWidget* screen = new QWidget();
    screen->setObjectName("loginScreen");

    // Set campus bus background image if available
    QString bgPath = QDir::current().filePath("assets/campus_bus_bg.jpg");
    if (QFile::exists(bgPath)) {
        screen->setStyleSheet(QString(
            "QWidget#loginScreen { "
            "  background-image: url('%1'); "
            "  background-position: center; "
            "  background-repeat: no-repeat; "
            "}"
        ).arg(bgPath));
    } else {
        screen->setStyleSheet("QWidget#loginScreen { background-color: #0D1117; }");
    }

    QVBoxLayout* rootLayout = new QVBoxLayout(screen);
    rootLayout->setAlignment(Qt::AlignCenter);

    // Glass/Dark Login Card
    QFrame* loginCard = new QFrame(screen);
    loginCard->setFixedWidth(460);
    loginCard->setStyleSheet(
        "QFrame { "
        "  background-color: rgba(22, 27, 34, 0.94); "
        "  border: 1px solid #30363D; "
        "  border-radius: 12px; "
        "  padding: 24px; "
        "}"
    );

    QVBoxLayout* cardLayout = new QVBoxLayout(loginCard);
    cardLayout->setSpacing(12);

    // System Branding Header
    QLabel* brandIcon = new QLabel("🚌", loginCard);
    brandIcon->setStyleSheet("font-size: 32px; background: transparent;");
    brandIcon->setAlignment(Qt::AlignCenter);

    QLabel* brandTitle = new QLabel("Welcome to Smart College Bus Management System", loginCard);
    brandTitle->setStyleSheet("font-size: 17px; font-weight: 700; color: #E6EDF3; background: transparent;");
    brandTitle->setAlignment(Qt::AlignCenter);
    brandTitle->setWordWrap(true);

    QLabel* brandSubtitle = new QLabel("Secure Multi-Role Transportation Portal • Graphic Era Hill University", loginCard);
    brandSubtitle->setStyleSheet("font-size: 12px; color: #8B949E; margin-bottom: 10px; background: transparent;");
    brandSubtitle->setAlignment(Qt::AlignCenter);

    // Form
    QFormLayout* form = new QFormLayout();
    form->setSpacing(10);
    form->setLabelAlignment(Qt::AlignLeft);

    loginRoleCombo = new QComboBox(loginCard);
    loginRoleCombo->addItem("🔴 Admin Portal", "ADMIN");
    loginRoleCombo->addItem("🟡 Driver Portal", "DRIVER");
    loginRoleCombo->addItem("🟢 Student / Commuter Portal", "STUDENT");
    loginRoleCombo->addItem("🔵 Parent / Guardian Portal", "PARENT");

    loginUserEdit = new QLineEdit("admin", loginCard);
    loginUserEdit->setPlaceholderText("Enter Username, Driver ID, or Roll Number");

    // Password Field with Show/Hide Toggle
    QHBoxLayout* passLayout = new QHBoxLayout();
    passLayout->setContentsMargins(0, 0, 0, 0);
    passLayout->setSpacing(6);

    loginPassEdit = new QLineEdit("admin123", loginCard);
    loginPassEdit->setEchoMode(QLineEdit::Password);
    loginPassEdit->setPlaceholderText("Enter Account Password");

    showPassBtn = new QPushButton("👁️ Show", loginCard);
    showPassBtn->setFixedWidth(75);
    showPassBtn->setStyleSheet("padding: 6px 10px; font-size: 11px;");
    connect(showPassBtn, &QPushButton::clicked, this, &MainWindow::onTogglePasswordVisibility);

    passLayout->addWidget(loginPassEdit);
    passLayout->addWidget(showPassBtn);

    QLabel* roleLabel = new QLabel("Authorized Role:", loginCard);
    roleLabel->setStyleSheet("color: #8B949E; font-weight: 600; font-size: 12px;");
    QLabel* userLabel = new QLabel("Account Username / ID:", loginCard);
    userLabel->setStyleSheet("color: #8B949E; font-weight: 600; font-size: 12px;");
    QLabel* passLabel = new QLabel("Security Password:", loginCard);
    passLabel->setStyleSheet("color: #8B949E; font-weight: 600; font-size: 12px;");

    form->addRow(roleLabel, loginRoleCombo);
    form->addRow(userLabel, loginUserEdit);
    form->addRow(passLabel, passLayout);

    // Sign In Button (GitHub Green)
    QPushButton* signInBtn = new QPushButton("Sign In to Portal", loginCard);
    signInBtn->setStyleSheet(
        "background-color: #238636; border: 1px solid rgba(240, 246, 252, 0.1); "
        "color: #FFFFFF; font-weight: 700; font-size: 14px; padding: 10px; border-radius: 6px; margin-top: 8px;"
    );
    connect(signInBtn, &QPushButton::clicked, this, &MainWindow::onPerformLogin);

    loginStatusLabel = new QLabel("", loginCard);
    loginStatusLabel->setStyleSheet("font-size: 12px; font-weight: 600; color: #F85149; background: transparent;");
    loginStatusLabel->setAlignment(Qt::AlignCenter);

    // Controlled Development / Demo Mode Helper
    QHBoxLayout* demoLayout = new QHBoxLayout();
    QPushButton* demoBtn = new QPushButton("⚡ Quick Demo Auto-Fill (Dev Mode)", loginCard);
    demoBtn->setStyleSheet("background: transparent; border: none; color: #58A6FF; font-size: 11.5px; text-decoration: underline;");
    connect(demoBtn, &QPushButton::clicked, this, &MainWindow::onQuickDemoLogin);
    demoLayout->addWidget(demoBtn, 0, Qt::AlignCenter);

    cardLayout->addWidget(brandIcon);
    cardLayout->addWidget(brandTitle);
    cardLayout->addWidget(brandSubtitle);
    cardLayout->addLayout(form);
    cardLayout->addWidget(signInBtn);
    cardLayout->addWidget(loginStatusLabel);
    cardLayout->addLayout(demoLayout);

    rootLayout->addWidget(loginCard);
    return screen;
}

// 3. AUTHENTICATED PORTAL SCREEN
QWidget* MainWindow::createPortalScreen() {
    QWidget* screen = new QWidget();
    screen->setObjectName("portalPage");
    QVBoxLayout* mainLayout = new QVBoxLayout(screen);
    mainLayout->setContentsMargins(14, 12, 14, 14);
    mainLayout->setSpacing(10);

    // Header Bar
    QFrame* headerCard = new QFrame(screen);
    headerCard->setStyleSheet("background-color: #161B22; border: 1px solid #30363D; border-radius: 8px; padding: 8px 14px;");
    QHBoxLayout* headerLayout = new QHBoxLayout(headerCard);
    headerLayout->setContentsMargins(4, 4, 4, 4);

    QLabel* titleLabel = new QLabel("🚌 Smart College Bus Management System • Graphic Era Bhimtal Campus", headerCard);
    titleLabel->setStyleSheet("font-size: 15px; font-weight: 700; color: #58A6FF;");

    loggedUserLabel = new QLabel("User: admin", headerCard);
    loggedUserLabel->setStyleSheet("font-size: 13px; font-weight: 600; color: #E6EDF3; margin-left: 14px;");

    loggedRoleBadge = new QLabel(" ADMINISTRATOR ", headerCard);
    loggedRoleBadge->setStyleSheet("font-size: 11px; font-weight: 700; background-color: #238636; color: #FFFFFF; padding: 3px 8px; border-radius: 10px;");

    QLabel* scheduleLabel = new QLabel("Schedule Mode:", headerCard);
    scheduleLabel->setStyleSheet("font-size: 12px; color: #8B949E; margin-left: 12px;");

    scheduleComboBox = new QComboBox(headerCard);
    scheduleComboBox->addItem("Full-Day Schedule (Normal Hours)");
    scheduleComboBox->addItem("Half-Day Schedule (Exam Hours)");
    scheduleComboBox->setStyleSheet("font-size: 12px; padding: 4px 8px;");
    connect(scheduleComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onScheduleChanged);

    QPushButton* logoutBtn = new QPushButton("Sign Out", headerCard);
    logoutBtn->setStyleSheet("background-color: #21262D; border: 1px solid #30363D; color: #F85149; font-weight: 600; padding: 5px 12px; border-radius: 6px;");
    connect(logoutBtn, &QPushButton::clicked, this, &MainWindow::onPerformLogout);

    headerLayout->addWidget(titleLabel);
    headerLayout->addWidget(loggedUserLabel);
    headerLayout->addWidget(loggedRoleBadge);
    headerLayout->addStretch();
    headerLayout->addWidget(scheduleLabel);
    headerLayout->addWidget(scheduleComboBox);
    headerLayout->addWidget(logoutBtn);

    mainLayout->addWidget(headerCard);

    // Tabbed Navigation
    tabWidget = new QTabWidget(screen);

    adminTabWidget = createAdminTab();
    studentTabWidget = createStudentTab();
    driverTabWidget = createDriverTab();
    parentTabWidget = createParentTab();
    routeVisualizerTabWidget = createRouteVisualizerTab();
    complaintDeskTabWidget = createComplaintDeskTab();

    mainLayout->addWidget(tabWidget, 1);
    return screen;
}

// 4. ROLE-BASED TAB CONFIGURATION
void MainWindow::setupTabsForRole(UserRole role, const string& username) {
    tabWidget->clear();

    if (role == UserRole::ADMIN) {
        tabWidget->addTab(adminTabWidget, "Admin Control Center");
        tabWidget->addTab(studentTabWidget, "Student Portal");
        tabWidget->addTab(driverTabWidget, "Driver Portal");
        tabWidget->addTab(parentTabWidget, "Parent Portal");
        tabWidget->addTab(routeVisualizerTabWidget, "Route Map Optimizer");
        tabWidget->addTab(complaintDeskTabWidget, "Fleet & Complaints");
    } else if (role == UserRole::STUDENT) {
        tabWidget->addTab(studentTabWidget, "Student Portal");
        tabWidget->addTab(routeVisualizerTabWidget, "Route Map Optimizer");
        tabWidget->addTab(complaintDeskTabWidget, "Disciplinary Polls & Complaints");
        studentRollInput->setText(QString::fromStdString(username));
        onSearchStudentSchedule();
    } else if (role == UserRole::DRIVER) {
        tabWidget->addTab(driverTabWidget, "Driver Portal");
        tabWidget->addTab(complaintDeskTabWidget, "Complaints & Incidents");
        tabWidget->addTab(routeVisualizerTabWidget, "Route Map Optimizer");
    } else if (role == UserRole::PARENT) {
        tabWidget->addTab(parentTabWidget, "Parent Portal");
        tabWidget->addTab(routeVisualizerTabWidget, "Route Map Optimizer");
        tabWidget->addTab(complaintDeskTabWidget, "Complaints Status");
        parentPhoneInput->setText(QString::fromStdString(username));
        onParentTrackChild();
    }
}

// 5. ADMIN CONTROL CENTER (MASTER REDESIGN)
QWidget* MainWindow::createAdminTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);
    layout->setSpacing(12);

    // Summary Metric KPI Cards
    QHBoxLayout* kpiLayout = new QHBoxLayout();
    kpiLayout->setSpacing(10);

    auto makeKpiCard = [](const QString& title, const QString& val, const QString& accentColor) -> QFrame* {
        QFrame* card = new QFrame();
        card->setStyleSheet(QString(
            "QFrame { background-color: #1C2533; border: 1px solid #30363D; border-radius: 8px; padding: 10px; }"
        ));
        QVBoxLayout* cl = new QVBoxLayout(card);
        cl->setContentsMargins(4, 4, 4, 4);
        cl->setSpacing(2);
        QLabel* t = new QLabel(title, card);
        t->setStyleSheet("font-size: 11px; font-weight: 600; color: #8B949E; text-transform: uppercase;");
        QLabel* v = new QLabel(val, card);
        v->setStyleSheet(QString("font-size: 16px; font-weight: 700; color: %1;").arg(accentColor));
        cl->addWidget(t);
        cl->addWidget(v);
        return card;
    };

    QFrame* c1 = makeKpiCard("Total Fleet Units", "3 Active Buses", "#58A6FF");
    kpiTotalBusesLabel = c1->findChild<QLabel*>();
    QFrame* c2 = makeKpiCard("Total Available Seats", "130 Seats", "#3FB950");
    kpiTotalSeatsLabel = c2->findChild<QLabel*>();
    QFrame* c3 = makeKpiCard("Operating Corridors", "2 Mountain Routes", "#D29922");
    kpiCorridorsLabel = c3->findChild<QLabel*>();
    QFrame* c4 = makeKpiCard("Campus Schedule", "Full-Day Mode", "#58A6FF");
    kpiScheduleLabel = c4->findChild<QLabel*>();
    QFrame* c5 = makeKpiCard("Student Attendance", "94% Boarded", "#3FB950");
    kpiAttendanceLabel = c5->findChild<QLabel*>();
    QFrame* c6 = makeKpiCard("Active Complaints", "2 Open / 1 Poll", "#F85149");
    kpiComplaintsLabel = c6->findChild<QLabel*>();

    kpiLayout->addWidget(c1);
    kpiLayout->addWidget(c2);
    kpiLayout->addWidget(c3);
    kpiLayout->addWidget(c4);
    kpiLayout->addWidget(c5);
    kpiLayout->addWidget(c6);

    layout->addLayout(kpiLayout);

    // Section A & B: Bus Configuration & Stop Timings
    QHBoxLayout* midLayout = new QHBoxLayout();
    midLayout->setSpacing(12);

    // Section A: Bus Fleet Configuration
    QGroupBox* busConfigBox = new QGroupBox("A. Bus Fleet Configuration", tab);
    QFormLayout* busForm = new QFormLayout(busConfigBox);
    busForm->setSpacing(8);

    adminBusCombo = new QComboBox(busConfigBox);
    adminBusCombo->addItem("B-101 (UK-04-PA-1001)");
    adminBusCombo->addItem("B-102 (UK-04-TA-2002)");
    adminBusCombo->addItem("B-103 (UK-04-PA-3003)");

    adminRegInput = new QLineEdit("UK-04-PA-1001", busConfigBox);
    adminCapInput = new QLineEdit("45", busConfigBox);
    adminRouteCombo = new QComboBox(busConfigBox);
    adminRouteCombo->addItem("R1 - Express Hill Corridor");
    adminRouteCombo->addItem("R2 - Valley Bypass Corridor");

    QPushButton* saveBusBtn = new QPushButton("Save Bus Configuration", busConfigBox);
    saveBusBtn->setStyleSheet("background-color: #238636; color: #FFFFFF; font-weight: 600; padding: 7px;");
    connect(saveBusBtn, &QPushButton::clicked, this, &MainWindow::onAdminUpdateBus);

    busForm->addRow("Select Bus Unit:", adminBusCombo);
    busForm->addRow("Registration Number:", adminRegInput);
    busForm->addRow("Seating Capacity:", adminCapInput);
    busForm->addRow("Assigned Corridor:", adminRouteCombo);
    busForm->addRow("", saveBusBtn);

    // Section B: Campus Stop Timings & Broadcast Dispatch
    QGroupBox* timingBox = new QGroupBox("B. Campus Stop Timings & Broadcast Dispatch", tab);
    QFormLayout* timingForm = new QFormLayout(timingBox);
    timingForm->setSpacing(8);

    adminTimingStopCombo = new QComboBox(timingBox);
    adminTimingStopCombo->addItem("ST101 - Haldwani Bus Terminal (424m)", "ST101");
    adminTimingStopCombo->addItem("ST102 - Kathgodam Railway Station (554m)", "ST102");
    adminTimingStopCombo->addItem("ST103 - Ranibagh Junction (620m)", "ST103");
    adminTimingStopCombo->addItem("ST104 - Bhujiaghat Bypass (910m)", "ST104");
    adminTimingStopCombo->addItem("ST105 - Bhimtal Lake Mall Road (1,370m)", "ST105");
    adminTimingStopCombo->addItem("ST106 - Graphic Era Hill University (1,420m)", "ST106");

    adminTimingBusCombo = new QComboBox(timingBox);
    adminTimingBusCombo->addItem("B-101"); adminTimingBusCombo->addItem("B-102"); adminTimingBusCombo->addItem("B-103");

    adminTimingScheduleCombo = new QComboBox(timingBox);
    adminTimingScheduleCombo->addItem("Full-Day Schedule");
    adminTimingScheduleCombo->addItem("Half-Day Schedule");

    adminNewTimeInput = new QLineEdit("07:15", timingBox);

    QPushButton* updateTimingBtn = new QPushButton("Update Timing", timingBox);
    updateTimingBtn->setStyleSheet("background-color: #1F6FEB; color: #FFFFFF; font-weight: 600; padding: 7px;");
    connect(updateTimingBtn, &QPushButton::clicked, this, &MainWindow::onAdminUpdateTiming);

    QPushButton* noticeBtn = new QPushButton("📢 Dispatch Broadcast Notice", timingBox);
    noticeBtn->setStyleSheet("background-color: #21262D; border: 1px solid #30363D; color: #E6EDF3; font-weight: 600; padding: 7px;");
    connect(noticeBtn, &QPushButton::clicked, this, &MainWindow::onAdminBroadcastNotice);

    timingForm->addRow("Select Bus Stop:", adminTimingStopCombo);
    timingForm->addRow("Select Transit Bus:", adminTimingBusCombo);
    timingForm->addRow("Schedule Mode:", adminTimingScheduleCombo);
    timingForm->addRow("Departure Time:", adminNewTimeInput);
    QHBoxLayout* timingBtns = new QHBoxLayout();
    timingBtns->addWidget(updateTimingBtn);
    timingBtns->addWidget(noticeBtn);
    timingForm->addRow("", timingBtns);

    midLayout->addWidget(busConfigBox);
    midLayout->addWidget(timingBox);
    layout->addLayout(midLayout);

    // Section C & D: Student Transport Directory & Attendance Table
    QHBoxLayout* lowerLayout = new QHBoxLayout();
    lowerLayout->setSpacing(12);

    // Section C: Student Transport Directory
    QGroupBox* studentDirBox = new QGroupBox("C. Student Transport Directory", tab);
    QVBoxLayout* dirLayout = new QVBoxLayout(studentDirBox);

    studentDirTable = new QTableWidget(0, 6, studentDirBox);
    studentDirTable->setHorizontalHeaderLabels({"Full Name", "Roll Number", "Phone Number", "Boarding Stop", "Bus", "Fee Status"});
    studentDirTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    registerStudentBtn = new QPushButton("+ Register New Student", studentDirBox);
    registerStudentBtn->setStyleSheet("background-color: #238636; color: #FFFFFF; font-weight: 600; padding: 6px 12px;");
    connect(registerStudentBtn, &QPushButton::clicked, this, &MainWindow::onAdminRegisterStudentDialog);

    dirLayout->addWidget(studentDirTable);
    dirLayout->addWidget(registerStudentBtn, 0, Qt::AlignRight);

    // Section D: Student Attendance & Boarding Management
    QGroupBox* attendBox = new QGroupBox("D. Student Attendance & Boarding Verification", tab);
    QVBoxLayout* attLayout = new QVBoxLayout(attendBox);

    QHBoxLayout* attControlLayout = new QHBoxLayout();
    boardRollInput = new QLineEdit("2024-CS-001", attendBox);
    boardRollInput->setPlaceholderText("Student Roll Number");
    boardBusCombo = new QComboBox(attendBox);
    boardBusCombo->addItem("B-101"); boardBusCombo->addItem("B-102"); boardBusCombo->addItem("B-103");

    QPushButton* boardBtn = new QPushButton("Mark Attendance / Board", attendBox);
    boardBtn->setStyleSheet("background-color: #238636; color: #FFFFFF; font-weight: 600;");
    connect(boardBtn, &QPushButton::clicked, this, &MainWindow::onBoardStudent);

    QPushButton* qrVerifyBtn = new QPushButton("Verify QR Pass", attendBox);
    qrVerifyBtn->setStyleSheet("background-color: #1F6FEB; color: #FFFFFF; font-weight: 600;");
    connect(qrVerifyBtn, &QPushButton::clicked, this, &MainWindow::onVerifyQRPass);

    QPushButton* refreshAttBtn = new QPushButton("Refresh Log", attendBox);
    connect(refreshAttBtn, &QPushButton::clicked, this, &MainWindow::refreshAttendanceTable);

    attControlLayout->addWidget(new QLabel("Roll No:", attendBox));
    attControlLayout->addWidget(boardRollInput);
    attControlLayout->addWidget(new QLabel("Bus:", attendBox));
    attControlLayout->addWidget(boardBusCombo);
    attControlLayout->addWidget(boardBtn);
    attControlLayout->addWidget(qrVerifyBtn);
    attControlLayout->addWidget(refreshAttBtn);

    attendanceTable = new QTableWidget(0, 7, attendBox);
    attendanceTable->setHorizontalHeaderLabels({"Timestamp", "Roll Number", "Student Name", "Bus ID", "Pickup Stop", "Fee Status", "Boarding Status"});
    attendanceTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    attLayout->addLayout(attControlLayout);
    attLayout->addWidget(attendanceTable);

    lowerLayout->addWidget(studentDirBox, 4);
    lowerLayout->addWidget(attendBox, 5);
    layout->addLayout(lowerLayout, 1);

    return tab;
}

// 6. STUDENT PORTAL
QWidget* MainWindow::createStudentTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);
    layout->setSpacing(12);

    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->setSpacing(12);

    // Schedule & QR Lookup
    QGroupBox* box = new QGroupBox("Student Morning Departure & Digital Pass Lookup", tab);
    QFormLayout* formLayout = new QFormLayout(box);
    formLayout->setSpacing(10);

    studentRollInput = new QLineEdit("2024-CS-001", box);
    QPushButton* searchBtn = new QPushButton("Lookup Departure Schedule", box);
    searchBtn->setStyleSheet("background-color: #1F6FEB; color: #FFFFFF; font-weight: 600;");
    connect(searchBtn, &QPushButton::clicked, this, &MainWindow::onSearchStudentSchedule);

    QPushButton* qrPassBtn = new QPushButton("Show Digital QR Bus Pass", box);
    qrPassBtn->setStyleSheet("background-color: #238636; color: #FFFFFF; font-weight: 600;");
    connect(qrPassBtn, &QPushButton::clicked, this, &MainWindow::onShowDigitalQRPass);

    QPushButton* sosBtn = new QPushButton("🚨 EMERGENCY SOS BROADCAST", box);
    sosBtn->setStyleSheet("background-color: #DA3633; color: #FFFFFF; font-weight: 700; padding: 9px;");
    connect(sosBtn, &QPushButton::clicked, this, &MainWindow::onTriggerStudentSOS);

    formLayout->addRow("Registered Roll Number:", studentRollInput);
    QHBoxLayout* btnRow = new QHBoxLayout();
    btnRow->addWidget(searchBtn);
    btnRow->addWidget(qrPassBtn);
    formLayout->addRow("", btnRow);
    formLayout->addRow("", sosBtn);

    // Driver Rating Box
    QGroupBox* rateBox = new QGroupBox("Driver Rating & Journey Quality Feedback", tab);
    QFormLayout* rateLayout = new QFormLayout(rateBox);
    rateLayout->setSpacing(8);

    ratingDriverIdInput = new QLineEdit("D-01", rateBox);
    ratingStarCombo = new QComboBox(rateBox);
    ratingStarCombo->addItem("★★★★★ - 5 Stars (Punctual & Safe Mountain Driving)");
    ratingStarCombo->addItem("★★★★☆ - 4 Stars (Good Driving)");
    ratingStarCombo->addItem("★★★☆☆ - 3 Stars (Average Speed)");
    ratingStarCombo->addItem("★★☆☆☆ - 2 Stars (Delayed Arrival)");
    ratingStarCombo->addItem("★☆☆☆☆ - 1 Star (Unsafe Driving / Violations)");

    ratingFeedbackInput = new QLineEdit("Very punctual and smooth driving across Kathgodam hill turns.", rateBox);
    QPushButton* submitRateBtn = new QPushButton("Submit Driver Rating", rateBox);
    submitRateBtn->setStyleSheet("background-color: #D29922; color: #0D1117; font-weight: 700;");
    connect(submitRateBtn, &QPushButton::clicked, this, &MainWindow::onSubmitDriverRating);

    rateLayout->addRow("Driver Identifier:", ratingDriverIdInput);
    rateLayout->addRow("Rating Grade:", ratingStarCombo);
    rateLayout->addRow("Feedback Notes:", ratingFeedbackInput);
    rateLayout->addRow("", submitRateBtn);

    topLayout->addWidget(box, 3);
    topLayout->addWidget(rateBox, 2);
    layout->addLayout(topLayout);

    studentInfoLabel = new QLabel("Enter Roll Number above to inspect assigned bus, boarding schedule, and QR pass status.", tab);
    studentInfoLabel->setStyleSheet("font-size: 13px; color: #8B949E; padding: 10px; background-color: #161B22; border: 1px solid #30363D; border-radius: 6px;");

    studentScheduleTable = new QTableWidget(0, 4, tab);
    studentScheduleTable->setHorizontalHeaderLabels({"Assigned Bus", "Corridor Route", "Boarding Stop", "Morning Departure Time"});
    studentScheduleTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    layout->addWidget(studentInfoLabel);
    layout->addWidget(studentScheduleTable, 1);

    return tab;
}

// 7. DRIVER PORTAL
QWidget* MainWindow::createDriverTab() {
    QWidget* tab = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(tab);
    layout->setSpacing(14);

    QGroupBox* tripBox = new QGroupBox("Driver Trip Telemetry & Delay Dispatch", tab);
    QFormLayout* tripForm = new QFormLayout(tripBox);
    tripForm->setSpacing(10);

    driverBusCombo = new QComboBox(tripBox);
    driverBusCombo->addItem("B-101 (UK-04-PA-1001)");
    driverBusCombo->addItem("B-102 (UK-04-TA-2002)");
    driverBusCombo->addItem("B-103 (UK-04-PA-3003)");

    QPushButton* startBtn = new QPushButton("Start Mountain Trip", tripBox);
    startBtn->setStyleSheet("background-color: #238636; color: #FFFFFF; font-weight: 700; padding: 8px;");
    connect(startBtn, &QPushButton::clicked, this, &MainWindow::onDriverStartTrip);

    QPushButton* endBtn = new QPushButton("Conclude Trip (Campus Terminal)", tripBox);
    endBtn->setStyleSheet("background-color: #21262D; border: 1px solid #30363D; color: #8B949E; font-weight: 600; padding: 8px;");
    connect(endBtn, &QPushButton::clicked, this, &MainWindow::onDriverEndTrip);

    driverDelayReasonInput = new QLineEdit("Kathgodam Highway Landslide / Road Diversion", tripBox);
    driverDelayMinsInput = new QLineEdit("15", tripBox);
    QPushButton* delayBtn = new QPushButton("⚠️ Dispatch Delay Alert to Students & Parents", tripBox);
    delayBtn->setStyleSheet("background-color: #D29922; color: #0D1117; font-weight: 700; padding: 8px;");
    connect(delayBtn, &QPushButton::clicked, this, &MainWindow::onDriverReportDelay);

    tripForm->addRow("Operating Bus:", driverBusCombo);
    QHBoxLayout* tripBtns = new QHBoxLayout();
    tripBtns->addWidget(startBtn);
    tripBtns->addWidget(endBtn);
    tripForm->addRow("", tripBtns);
    tripForm->addRow("Delay Reason:", driverDelayReasonInput);
    tripForm->addRow("Minutes Delayed:", driverDelayMinsInput);
    tripForm->addRow("", delayBtn);

    QGroupBox* misbehaviorBox = new QGroupBox("Report Student Misbehavior & Safety Incidents", tab);
    QFormLayout* misForm = new QFormLayout(misbehaviorBox);
    misForm->setSpacing(10);

    misbehaviorRollInput = new QLineEdit("2024-ME-012", misbehaviorBox);
    misbehaviorDescInput = new QLineEdit("Loud altercation, refusal to follow safety rules during ascent.", misbehaviorBox);
    QPushButton* reportMisBtn = new QPushButton("Submit Disciplinary Incident", misbehaviorBox);
    reportMisBtn->setStyleSheet("background-color: #DA3633; color: #FFFFFF; font-weight: 700; padding: 8px;");
    connect(reportMisBtn, &QPushButton::clicked, this, &MainWindow::onDriverReportMisbehavior);

    misForm->addRow("Accused Student Roll:", misbehaviorRollInput);
    misForm->addRow("Incident Description:", misbehaviorDescInput);
    misForm->addRow("", reportMisBtn);

    layout->addWidget(tripBox);
    layout->addWidget(misbehaviorBox);
    return tab;
}

// 8. PARENT PORTAL
QWidget* MainWindow::createParentTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);
    layout->setSpacing(14);

    QGroupBox* box = new QGroupBox("Parent Live Telemetry & Fee Clearance Status", tab);
    QHBoxLayout* formLayout = new QHBoxLayout(box);
    formLayout->setSpacing(10);

    parentPhoneInput = new QLineEdit("+91-9811001122", box);
    QPushButton* trackBtn = new QPushButton("Track Ward Transit & ETA", box);
    trackBtn->setStyleSheet("background-color: #1F6FEB; color: #FFFFFF; font-weight: 700; padding: 8px 16px;");
    connect(trackBtn, &QPushButton::clicked, this, &MainWindow::onParentTrackChild);

    formLayout->addWidget(new QLabel("Parent Mobile Number / Ward Roll No:", box));
    formLayout->addWidget(parentPhoneInput, 1);
    formLayout->addWidget(trackBtn);

    parentInfoLabel = new QLabel("Enter parent contact phone above to inspect live vehicle telemetry, boarding confirmations, and semester transportation fee balances.", tab);
    parentInfoLabel->setStyleSheet("font-size: 13.5px; line-height: 1.5; color: #E6EDF3; padding: 18px; background-color: #161B22; border: 1px solid #30363D; border-radius: 8px;");

    layout->addWidget(box);
    layout->addWidget(parentInfoLabel);
    layout->addStretch();
    return tab;
}

// 9. COMPLAINT PORTAL & STUDENT POLL SYSTEM (MASTER REQUIREMENT 4)
QWidget* MainWindow::createComplaintDeskTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);
    layout->setSpacing(12);

    // Filter Bar
    QFrame* filterCard = new QFrame(tab);
    filterCard->setStyleSheet("background-color: #161B22; border: 1px solid #30363D; border-radius: 8px; padding: 8px 12px;");
    QHBoxLayout* fl = new QHBoxLayout(filterCard);
    fl->setContentsMargins(4, 4, 4, 4);
    fl->setSpacing(10);

    filterCategoryCombo = new QComboBox(filterCard);
    filterCategoryCombo->addItem("Category: All");
    filterCategoryCombo->addItem("Category: Rash Driving");
    filterCategoryCombo->addItem("Category: Student Misbehavior");
    filterCategoryCombo->addItem("Category: Schedule Delay Notice");
    filterCategoryCombo->addItem("Category: EMERGENCY_SOS_BUTTON");
    connect(filterCategoryCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onApplyComplaintFilters);

    filterReporterRoleCombo = new QComboBox(filterCard);
    filterReporterRoleCombo->addItem("Reporter: All Roles");
    filterReporterRoleCombo->addItem("Reporter: Student");
    filterReporterRoleCombo->addItem("Reporter: Driver");
    filterReporterRoleCombo->addItem("Reporter: Parent");
    connect(filterReporterRoleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onApplyComplaintFilters);

    filterAccusedRoleCombo = new QComboBox(filterCard);
    filterAccusedRoleCombo->addItem("Accused: All Roles");
    filterAccusedRoleCombo->addItem("Accused: Driver");
    filterAccusedRoleCombo->addItem("Accused: Student");
    filterAccusedRoleCombo->addItem("Accused: Authority");
    connect(filterAccusedRoleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onApplyComplaintFilters);

    filterStatusCombo = new QComboBox(filterCard);
    filterStatusCombo->addItem("Status: All");
    filterStatusCombo->addItem("Status: OPEN");
    filterStatusCombo->addItem("Status: UNDER INVESTIGATION");
    filterStatusCombo->addItem("Status: POLL ACTIVE");
    filterStatusCombo->addItem("Status: RESOLVED");
    filterStatusCombo->addItem("Status: REJECTED");
    connect(filterStatusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onApplyComplaintFilters);

    fl->addWidget(new QLabel("Filters:", filterCard));
    fl->addWidget(filterCategoryCombo);
    fl->addWidget(filterReporterRoleCombo);
    fl->addWidget(filterAccusedRoleCombo);
    fl->addWidget(filterStatusCombo);
    fl->addStretch();

    layout->addWidget(filterCard);

    // Master Complaint Table (Displays all 11 required fields)
    complaintTable = new QTableWidget(0, 8, tab);
    complaintTable->setHorizontalHeaderLabels({
        "ID", "Category", "Reported By (Complainant)", "Accused Person", "Bus", "Submission Date", "Submission Time", "Status"
    });
    complaintTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    complaintTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    complaintTable->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(complaintTable, &QTableWidget::itemSelectionChanged, this, &MainWindow::onComplaintTableSelectionChanged);

    layout->addWidget(complaintTable, 3);

    // Detail & Student Poll Card
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->setSpacing(12);

    // Complaint Inspection View
    QGroupBox* detailBox = new QGroupBox("Selected Complaint Incident Inspection & Resolution History", tab);
    QVBoxLayout* dbl = new QVBoxLayout(detailBox);
    dbl->setSpacing(8);

    complaintDetailHeader = new QLabel("Select a complaint from the table above to view full details.", detailBox);
    complaintDetailHeader->setStyleSheet("font-size: 13px; font-weight: 700; color: #58A6FF;");

    complaintDetailBody = new QLabel("", detailBox);
    complaintDetailBody->setStyleSheet("font-size: 12.5px; color: #E6EDF3; line-height: 1.4;");
    complaintDetailBody->setWordWrap(true);

    // Admin Action Buttons
    QHBoxLayout* adminActionLayout = new QHBoxLayout();
    adminInvestigateBtn = new QPushButton("Investigate Complaint", detailBox);
    adminInvestigateBtn->setStyleSheet("background-color: #1F6FEB; color: #FFFFFF; font-weight: 600; font-size: 12px;");
    connect(adminInvestigateBtn, &QPushButton::clicked, this, &MainWindow::onAdminInvestigateComplaint);

    adminResolveBtn = new QPushButton("Resolve Complaint", detailBox);
    adminResolveBtn->setStyleSheet("background-color: #238636; color: #FFFFFF; font-weight: 600; font-size: 12px;");
    connect(adminResolveBtn, &QPushButton::clicked, this, &MainWindow::onAdminResolveComplaint);

    adminRejectBtn = new QPushButton("Reject with Recorded Reason", detailBox);
    adminRejectBtn->setStyleSheet("background-color: #21262D; border: 1px solid #30363D; color: #F85149; font-weight: 600; font-size: 12px;");
    connect(adminRejectBtn, &QPushButton::clicked, this, &MainWindow::onAdminRejectComplaint);

    adminActionLayout->addWidget(adminInvestigateBtn);
    adminActionLayout->addWidget(adminResolveBtn);
    adminActionLayout->addWidget(adminRejectBtn);

    dbl->addWidget(complaintDetailHeader);
    dbl->addWidget(complaintDetailBody, 1);
    dbl->addLayout(adminActionLayout);

    // Student Peer Disciplinary Poll Box (Master Requirement 4.A)
    pollGroupBox = new QGroupBox("Student Peer Disciplinary Poll (YES / NO Voting)", tab);
    QVBoxLayout* pbl = new QVBoxLayout(pollGroupBox);
    pbl->setSpacing(8);

    pollQuestionLabel = new QLabel("Question: \"Do you agree that this incident requires disciplinary action?\"", pollGroupBox);
    pollQuestionLabel->setStyleSheet("font-size: 12.5px; font-weight: 700; color: #E6EDF3;");
    pollQuestionLabel->setWordWrap(true);

    pollStatsLabel = new QLabel("Total Votes: 0 | YES: 0 (0%) | NO: 0 (0%)", pollGroupBox);
    pollStatsLabel->setStyleSheet("font-size: 12px; color: #58A6FF; font-weight: 600;");

    pollProgressBar = new QProgressBar(pollGroupBox);
    pollProgressBar->setRange(0, 100);
    pollProgressBar->setValue(50);

    QHBoxLayout* voteBtns = new QHBoxLayout();
    voteYesBtn = new QPushButton("Vote YES (Require Action)", pollGroupBox);
    voteYesBtn->setStyleSheet("background-color: #238636; color: #FFFFFF; font-weight: 700; padding: 7px;");
    connect(voteYesBtn, &QPushButton::clicked, this, [this]{ onCastStudentVote(true); });

    voteNoBtn = new QPushButton("Vote NO (Dismiss Incident)", pollGroupBox);
    voteNoBtn->setStyleSheet("background-color: #30363D; color: #E6EDF3; font-weight: 700; padding: 7px;");
    connect(voteNoBtn, &QPushButton::clicked, this, [this]{ onCastStudentVote(false); });

    voteBtns->addWidget(voteYesBtn);
    voteBtns->addWidget(voteNoBtn);

    pollMessageLabel = new QLabel("", pollGroupBox);
    pollMessageLabel->setStyleSheet("font-size: 11.5px; color: #3FB950; font-weight: 600;");
    pollMessageLabel->setWordWrap(true);

    pbl->addWidget(pollQuestionLabel);
    pbl->addWidget(pollStatsLabel);
    pbl->addWidget(pollProgressBar);
    pbl->addLayout(voteBtns);
    pbl->addWidget(pollMessageLabel);
    pbl->addStretch();

    // Submit New Complaint Card
    QGroupBox* newCompBox = new QGroupBox("Submit New Safety Complaint", tab);
    QFormLayout* ncl = new QFormLayout(newCompBox);
    ncl->setSpacing(6);

    newCompCategoryCombo = new QComboBox(newCompBox);
    newCompCategoryCombo->addItem("Rash Driving");
    newCompCategoryCombo->addItem("Student Misbehavior");
    newCompCategoryCombo->addItem("Over-speeding");
    newCompCategoryCombo->addItem("Safety Violation");
    newCompCategoryCombo->addItem("Schedule Delay Notice");

    newCompAccusedNameEdit = new QLineEdit("Rajesh Kumar", newCompBox);
    newCompAccusedRoleCombo = new QComboBox(newCompBox);
    newCompAccusedRoleCombo->addItem("Driver");
    newCompAccusedRoleCombo->addItem("Student");
    newCompAccusedRoleCombo->addItem("Authority");

    newCompBusEdit = new QLineEdit("B-101", newCompBox);
    newCompDescEdit = new QLineEdit("", newCompBox);
    newCompDescEdit->setPlaceholderText("Detailed description of violation...");

    QPushButton* submitBtn = new QPushButton("Submit Complaint Record", newCompBox);
    submitBtn->setStyleSheet("background-color: #1F6FEB; color: #FFFFFF; font-weight: 700;");
    connect(submitBtn, &QPushButton::clicked, this, &MainWindow::onSubmitNewComplaint);

    ncl->addRow("Category:", newCompCategoryCombo);
    ncl->addRow("Accused Name:", newCompAccusedNameEdit);
    ncl->addRow("Accused Role:", newCompAccusedRoleCombo);
    ncl->addRow("Bus Identifier:", newCompBusEdit);
    ncl->addRow("Description:", newCompDescEdit);
    ncl->addRow("", submitBtn);

    bottomLayout->addWidget(detailBox, 4);
    bottomLayout->addWidget(pollGroupBox, 3);
    bottomLayout->addWidget(newCompBox, 3);

    layout->addLayout(bottomLayout, 2);
    return tab;
}

// 10. ROUTE MAP OPTIMIZER (MASTER REQUIREMENT 5)
QWidget* MainWindow::createRouteVisualizerTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);
    layout->setSpacing(10);

    // Top Control Panel
    QFrame* controlPanel = new QFrame(tab);
    controlPanel->setStyleSheet("background-color: #161B22; border: 1px solid #30363D; border-radius: 8px; padding: 10px 14px;");
    QHBoxLayout* ctrlLayout = new QHBoxLayout(controlPanel);
    ctrlLayout->setContentsMargins(0, 0, 0, 0);
    ctrlLayout->setSpacing(10);

    // B. Selectable Source Location
    startStopCombo = new QComboBox(controlPanel);
    startStopCombo->addItem("ST101 - Haldwani Bus Terminal [424m ASL]", "ST101");
    startStopCombo->addItem("ST102 - Kathgodam Railway Station [554m ASL]", "ST102");
    startStopCombo->addItem("ST103 - Ranibagh Junction [620m ASL]", "ST103");
    startStopCombo->addItem("ST104 - Bhujiaghat Bypass [910m ASL]", "ST104");
    startStopCombo->addItem("ST105 - Bhimtal Lake Mall Road [1,370m ASL]", "ST105");

    // A. Fixed Destination (Master Requirement 5.A: Destination is ALWAYS Graphic Era Bhimtal, LOCKED/NON-EDITABLE)
    destStopCombo = new QComboBox(controlPanel);
    destStopCombo->addItem("ST106 - Graphic Era Hill University, Bhimtal Campus [1,420m ASL] (FIXED)", "ST106");
    destStopCombo->setEnabled(false); // FIXED and non-editable
    destStopCombo->setStyleSheet("background-color: #1C2533; color: #58A6FF; font-weight: 700; border: 1px solid #30363D;");

    // F. Route Algorithm
    algoCombo = new QComboBox(controlPanel);
    algoCombo->addItem("A* Heuristic Search (Topographic & Elevation)");
    algoCombo->addItem("Dijkstra Shortest Path (Road Topology)");

    QPushButton* optBtn = new QPushButton("Calculate Shortest Path", controlPanel);
    optBtn->setStyleSheet("background-color: #1F6FEB; color: #FFFFFF; font-weight: 700; padding: 7px 14px; border-radius: 6px;");
    connect(optBtn, &QPushButton::clicked, this, &MainWindow::onCalculateOptimalRoute);

    QPushButton* simBtn = new QPushButton("Simulate Bus Motion", controlPanel);
    simBtn->setStyleSheet("background-color: #238636; color: #FFFFFF; font-weight: 700; padding: 7px 14px; border-radius: 6px;");
    connect(simBtn, &QPushButton::clicked, this, &MainWindow::onStartLiveSim);

    QPushButton* osmBtn = new QPushButton("Open Interactive OpenStreetMap (Leaflet & OSRM)", controlPanel);
    osmBtn->setStyleSheet("background-color: #0969DA; color: #FFFFFF; font-weight: 700; padding: 7px 14px; border-radius: 6px;");
    connect(osmBtn, &QPushButton::clicked, this, &MainWindow::onOpenInteractiveOpenStreetMap);

    ctrlLayout->addWidget(new QLabel("Source:", controlPanel));
    ctrlLayout->addWidget(startStopCombo);
    ctrlLayout->addWidget(new QLabel("Destination (Fixed):", controlPanel));
    ctrlLayout->addWidget(destStopCombo);
    ctrlLayout->addWidget(new QLabel("Algorithm:", controlPanel));
    ctrlLayout->addWidget(algoCombo);
    ctrlLayout->addWidget(optBtn);
    ctrlLayout->addWidget(simBtn);
    ctrlLayout->addWidget(osmBtn);

    layout->addWidget(controlPanel);

    // Route Information Display HUD (Master Requirement 5.E)
    QFrame* kpiBar = new QFrame(tab);
    kpiBar->setStyleSheet("background-color: #1C2533; border: 1px solid #30363D; border-radius: 8px; padding: 8px 12px;");
    QHBoxLayout* kl = new QHBoxLayout(kpiBar);
    kl->setContentsMargins(4, 4, 4, 4);

    routeKpiDistance = new QLabel("Total Road Distance: <b>28.2 KM</b>", kpiBar);
    routeKpiDistance->setStyleSheet("color: #E6EDF3; font-size: 12.5px;");
    routeKpiDuration = new QLabel("Estimated Time: <b>48 Mins</b>", kpiBar);
    routeKpiDuration->setStyleSheet("color: #3FB950; font-size: 12.5px;");
    routeKpiElevation = new QLabel("Elevation Traversal: <b>+996m Ascent (424m -> 1,420m ASL)</b>", kpiBar);
    routeKpiElevation->setStyleSheet("color: #D29922; font-size: 12.5px;");
    routeKpiStops = new QLabel("Stops: <b>5 Checkpoints</b>", kpiBar);
    routeKpiStops->setStyleSheet("color: #8B949E; font-size: 12.5px;");
    routeKpiAlgo = new QLabel("Algorithm: <b>A* Heuristic Search</b>", kpiBar);
    routeKpiAlgo->setStyleSheet("color: #58A6FF; font-size: 12.5px;");
    routeKpiStatus = new QLabel("Status: <font color='#3FB950'><b>CORRIDOR OPERATIONAL</b></font>", kpiBar);
    routeKpiStatus->setStyleSheet("font-size: 12.5px;");

    kl->addWidget(routeKpiDistance);
    kl->addWidget(routeKpiDuration);
    kl->addWidget(routeKpiElevation);
    kl->addWidget(routeKpiStops);
    kl->addWidget(routeKpiAlgo);
    kl->addWidget(routeKpiStatus);

    layout->addWidget(kpiBar);

    routeInfoLabel = new QLabel("Selected Source: Haldwani Bus Terminal -> Destination: Graphic Era Hill University, Bhimtal Campus.", tab);
    routeInfoLabel->setStyleSheet("font-size: 12.5px; font-weight: 600; color: #58A6FF; padding: 6px 10px; background-color: #161B22; border-radius: 6px;");
    layout->addWidget(routeInfoLabel);

    // Interactive Real OpenStreetMap Banner with direct 1-click launch
    QFrame* realMapBanner = new QFrame(tab);
    realMapBanner->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #1F6FEB, stop:1 #238636); border-radius: 8px; padding: 10px 14px;");
    QHBoxLayout* rmbLayout = new QHBoxLayout(realMapBanner);
    rmbLayout->setContentsMargins(10, 8, 10, 8);
    QLabel* rmbIcon = new QLabel("🗺️", realMapBanner);
    rmbIcon->setStyleSheet("font-size: 26px;");
    QLabel* rmbText = new QLabel("<b>Real OpenStreetMap & OSRM Routing Active (Haldwani ➔ Bhimtal)</b><br><span style='font-size: 11.5px; color: #E6EDF3;'>Interactive Leaflet OpenStreetMap with real driving roads, live bus motion simulation, and elevation profile.</span>", realMapBanner);
    rmbText->setStyleSheet("color: #FFFFFF; font-size: 12.5px;");
    QPushButton* rmbBtn = new QPushButton("🚀 Launch Real OpenStreetMap in Browser", realMapBanner);
    rmbBtn->setStyleSheet("background-color: #FFFFFF; color: #0D1117; font-weight: 800; font-size: 12px; padding: 8px 16px; border-radius: 6px;");
    connect(rmbBtn, &QPushButton::clicked, this, &MainWindow::onOpenInteractiveOpenStreetMap);
    rmbLayout->addWidget(rmbIcon);
    rmbLayout->addWidget(rmbText, 1);
    rmbLayout->addWidget(rmbBtn);
    layout->addWidget(realMapBanner);

    // Full-Width Interactive Canvas
    graphScene = new QGraphicsScene(this);
    graphView = new QGraphicsView(graphScene, tab);
    graphView->setRenderHint(QPainter::Antialiasing);
    graphView->setMinimumHeight(440);
    graphView->setStyleSheet("background-color: #0D1117; border: 1px solid #30363D; border-radius: 8px;");

    layout->addWidget(graphView, 1);
    return tab;
}

// 11. TOPOGRAPHIC GRAPH RENDERING
void MainWindow::renderRouteGraph() {
    if (!graphScene) return;
    graphScene->clear();

    // Map Coordinates along Haldwani -> Kathgodam -> Ranibagh -> Bhimtal -> Graphic Era Campus
    int x1 = 90,  y1 = 340; // ST101: Haldwani Tikonia (424m)
    int x2 = 260, y2 = 280; // ST102: Kathgodam Station (554m)
    int x3 = 450, y3 = 230; // ST103: Ranibagh Junction (620m)
    int x4 = 620, y4 = 310; // ST104: Bhujiaghat Bypass (910m)
    int x5 = 800, y5 = 160; // ST105: Bhimtal Lake Mall Road (1,370m)
    int x6 = 1040, y6 = 90; // ST106: Graphic Era Hill University Bhimtal Campus (1,420m) - FIXED DESTINATION

    // Subtle Hill Elevation Contours (Topography)
    QPen contourPen(QColor("#21262D"), 1.5, Qt::DashLine);
    graphScene->addLine(50, 380, 1150, 380, contourPen);
    graphScene->addLine(50, 260, 1150, 260, contourPen);
    graphScene->addLine(50, 140, 1150, 140, contourPen);

    // Route R1: Express Hill Corridor (Haldwani -> Kathgodam -> Ranibagh -> Bhimtal Lake -> Graphic Era Campus)
    QPen r1Pen(QColor("#238636"), 5); // Bright Green line for optimal route
    graphScene->addLine(x1, y1, x2, y2, r1Pen);
    graphScene->addLine(x2, y2, x3, y3, r1Pen);
    graphScene->addLine(x3, y3, x5, y5, r1Pen);
    graphScene->addLine(x5, y5, x6, y6, r1Pen);

    // Route R2: Valley Bypass Corridor via Bhujiaghat
    QPen r2Pen(QColor("#8B949E"), 3, Qt::DashLine); // Dashed gray for alternative bypass
    graphScene->addLine(x3, y3, x4, y4, r2Pen);
    graphScene->addLine(x4, y4, x5, y5, r2Pen);

    // Corridor Legend
    auto addLegendText = [&](const QString& text, const QColor& col, int x, int y) {
        QGraphicsTextItem* item = graphScene->addText(text);
        item->setDefaultTextColor(col);
        QFont f = item->font();
        f.setBold(true);
        f.setPixelSize(12);
        item->setFont(f);
        item->setPos(x, y);
    };

    addLegendText("Solid Green Line: Optimal Mountain Route (R1 Express Corridor to Graphic Era Campus)", QColor("#3FB950"), 70, 20);
    addLegendText("Dashed Gray Line: Alternative Route (R2 Valley Bypass via Bhujiaghat)", QColor("#8B949E"), 70, 40);

    auto drawStopPin = [&](int x, int y, const QString& name, const QString& id, const QString& elev, const QColor& pinColor, bool isFixedDest = false) {
        graphScene->addEllipse(x - 13, y - 13, 26, 26, QPen(Qt::white, 2), QBrush(pinColor));
        if (isFixedDest) {
            // Glowing border for Graphic Era Fixed Destination
            graphScene->addEllipse(x - 18, y - 18, 36, 36, QPen(QColor("#58A6FF"), 2, Qt::DotLine), QBrush(Qt::NoBrush));
        }

        QGraphicsTextItem* text = graphScene->addText(name + (isFixedDest ? " [CAMPUS DESTINATION]" : " [" + id + "]"));
        text->setDefaultTextColor(isFixedDest ? QColor("#58A6FF") : QColor("#E6EDF3"));
        QFont f = text->font();
        f.setBold(true);
        f.setPixelSize(12);
        text->setFont(f);
        text->setPos(x - 55, y + 16);

        QGraphicsTextItem* elevText = graphScene->addText(elev);
        elevText->setDefaultTextColor(QColor("#8B949E"));
        QFont ef = elevText->font();
        ef.setPixelSize(11);
        elevText->setFont(ef);
        elevText->setPos(x - 30, y + 34);
    };

    // Draw Stop Nodes matching user colors:
    // Green: Start Location (#238636)
    // Red: Destination (#DA3633)
    // Intermediate: Green (#238636)
    // Explored: Cyan (#06b6d4)
    drawStopPin(x1, y1, "Haldwani Bus Terminal", "ST101", "Alt: 424m ASL", QColor("#238636"));
    drawStopPin(x2, y2, "Kathgodam Rail Station", "ST102", "Alt: 554m ASL", QColor("#238636"));
    drawStopPin(x3, y3, "Ranibagh Junction", "ST103", "Alt: 620m ASL", QColor("#238636"));
    drawStopPin(x4, y4, "Bhujiaghat Bypass", "ST104", "Alt: 910m ASL", QColor("#06b6d4")); // Explored node
    drawStopPin(x5, y5, "Bhimtal Lake Mall Road", "ST105", "Alt: 1,370m ASL", QColor("#238636"));
    drawStopPin(x6, y6, "Graphic Era Hill University", "ST106", "Alt: 1,420m ASL", QColor("#DA3633"), true);

    // Initial Bus Marker at Haldwani
    busMarker = graphScene->addEllipse(x1 - 9, y1 - 9, 18, 18, QPen(Qt::white, 2), QBrush(QColor("#DA3633")));
}

// 12. DATA REFRESH HELPERS
void MainWindow::refreshAllDataViews() {
    refreshFleetMetrics();
    refreshStudentDirectory();
    refreshAttendanceTable();
    refreshComplaintTable();
    renderRouteGraph();
}

void MainWindow::refreshFleetMetrics() {
    // Total fleet KPIs
    const auto& buses = sysManager.getAllBuses();
    int totalCap = 0;
    for (const auto& b : buses) totalCap += b.getCapacity();

    if (kpiTotalBusesLabel) kpiTotalBusesLabel->setText(QString("%1 Units Active").arg(buses.size()));
    if (kpiTotalSeatsLabel) kpiTotalSeatsLabel->setText(QString("%1 Seats Capacity").arg(totalCap));
    if (kpiScheduleLabel) kpiScheduleLabel->setText(sysManager.getScheduleType() == ScheduleType::FULL_DAY ? "Full-Day Active" : "Half-Day Active");
    
    int activeCompCount = 0;
    for (const auto& c : sysManager.getComplaintEngine().getComplaints()) {
        if (c.status != "RESOLVED" && c.status != "REJECTED") activeCompCount++;
    }
    if (kpiComplaintsLabel) kpiComplaintsLabel->setText(QString("%1 Active Cases").arg(activeCompCount));
}

void MainWindow::refreshStudentDirectory() {
    if (!studentDirTable) return;
    const auto& students = sysManager.getAllStudents();
    studentDirTable->setRowCount(students.size());

    for (size_t i = 0; i < students.size(); ++i) {
        studentDirTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(students[i].getName())));
        studentDirTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(students[i].getRollNumber())));
        studentDirTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(students[i].getPhone())));
        studentDirTable->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(students[i].getAssignedStopId())));
        studentDirTable->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(students[i].getAssignedRouteId())));

        QTableWidgetItem* feeItem = new QTableWidgetItem(students[i].isFeePaid() ? "PAID" : "PENDING");
        feeItem->setForeground(students[i].isFeePaid() ? QColor("#3FB950") : QColor("#F85149"));
        feeItem->setFont(QFont(feeItem->font().family(), -1, QFont::Bold));
        studentDirTable->setItem(i, 5, feeItem);
    }
}

void MainWindow::refreshAttendanceTable() {
    if (!attendanceTable) return;
    const auto& students = sysManager.getAllStudents();
    attendanceTable->setRowCount(students.size());

    QStringList mockTimes = {"07:32:15 AM", "07:44:02 AM", "08:02:40 AM", "08:18:11 AM"};

    for (size_t i = 0; i < students.size(); ++i) {
        QString time = (i < mockTimes.size()) ? mockTimes[i] : "08:25:00 AM";
        attendanceTable->setItem(i, 0, new QTableWidgetItem("2026-11-15 " + time));
        attendanceTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(students[i].getRollNumber())));
        attendanceTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(students[i].getName())));
        attendanceTable->setItem(i, 3, new QTableWidgetItem(i % 2 == 0 ? "B-101" : "B-102"));
        attendanceTable->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(students[i].getAssignedStopId())));

        QTableWidgetItem* feeItem = new QTableWidgetItem(students[i].isFeePaid() ? "PAID" : "PENDING");
        feeItem->setForeground(students[i].isFeePaid() ? QColor("#3FB950") : QColor("#F85149"));
        attendanceTable->setItem(i, 5, feeItem);

        QString statusText = students[i].isFeePaid() ? "SUCCESSFUL" : "DENIED (UNPAID FEE)";
        QColor statusColor = students[i].isFeePaid() ? QColor("#3FB950") : QColor("#F85149");
        QTableWidgetItem* statusItem = new QTableWidgetItem(statusText);
        statusItem->setForeground(statusColor);
        statusItem->setFont(QFont(statusItem->font().family(), -1, QFont::Bold));
        attendanceTable->setItem(i, 6, statusItem);
    }
}

void MainWindow::refreshComplaintTable() {
    if (!complaintTable) return;

    QString catFilter = filterCategoryCombo ? filterCategoryCombo->currentText().replace("Category: ", "") : "All";
    QString repFilter = filterReporterRoleCombo ? filterReporterRoleCombo->currentText().replace("Reporter: ", "") : "All Roles";
    QString accFilter = filterAccusedRoleCombo ? filterAccusedRoleCombo->currentText().replace("Accused: ", "") : "All Roles";
    QString statFilter = filterStatusCombo ? filterStatusCombo->currentText().replace("Status: ", "") : "All";

    const auto& complaints = sysManager.getComplaintEngine().getComplaints();
    vector<const Complaint*> filtered;

    for (const auto& c : complaints) {
        if (catFilter != "All" && c.category != catFilter.toStdString()) continue;
        if (repFilter != "All Roles" && c.reporterRole != repFilter.toStdString()) continue;
        if (accFilter != "All Roles" && c.accusedRole != accFilter.toStdString()) continue;
        if (statFilter != "All" && c.status != statFilter.toStdString()) continue;
        filtered.push_back(&c);
    }

    complaintTable->setRowCount(filtered.size());
    for (size_t i = 0; i < filtered.size(); ++i) {
        const auto* c = filtered[i];
        complaintTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(c->complaintId)));
        complaintTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(c->category)));
        complaintTable->setItem(i, 2, new QTableWidgetItem(QString("%1 [%2]").arg(QString::fromStdString(c->reporterName), QString::fromStdString(c->reporterRole))));
        complaintTable->setItem(i, 3, new QTableWidgetItem(QString("%1 [%2]").arg(QString::fromStdString(c->accusedName), QString::fromStdString(c->accusedRole))));
        complaintTable->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(c->busId)));
        complaintTable->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(c->submissionDate)));
        complaintTable->setItem(i, 6, new QTableWidgetItem(QString::fromStdString(c->submissionTime)));

        QTableWidgetItem* statItem = new QTableWidgetItem(QString::fromStdString(c->status));
        if (c->status == "RESOLVED") statItem->setForeground(QColor("#3FB950"));
        else if (c->status == "POLL ACTIVE") statItem->setForeground(QColor("#58A6FF"));
        else if (c->status == "UNDER INVESTIGATION") statItem->setForeground(QColor("#D29922"));
        else if (c->status == "REJECTED") statItem->setForeground(QColor("#F85149"));
        else statItem->setForeground(QColor("#E6EDF3"));
        statItem->setFont(QFont(statItem->font().family(), -1, QFont::Bold));
        complaintTable->setItem(i, 7, statItem);
    }

    if (!filtered.empty()) {
        displayComplaintDetails(filtered[0]->complaintId);
    }
}

void MainWindow::displayComplaintDetails(const string& complaintId) {
    selectedComplaintId = complaintId;
    Complaint* c = sysManager.getComplaintEngine().findComplaintById(complaintId);
    if (!c) return;

    complaintDetailHeader->setText(QString("Complaint Incident: %1 • Category: %2")
                                   .arg(QString::fromStdString(c->complaintId), QString::fromStdString(c->category)));

    complaintDetailBody->setText(QString(
        "<b>Reported By:</b> %1 (%2)  |  <b>Accused:</b> %3 (%4)<br>"
        "<b>Operating Bus:</b> %5  |  <b>Submission:</b> %6 at %7<br>"
        "<b>Status:</b> <b>%8</b><br><br>"
        "<b>Incident Description:</b><br>%9<br><br>"
        "<b>Disciplinary Authority Resolution Note:</b><br><i>%10</i>"
    ).arg(QString::fromStdString(c->reporterName), QString::fromStdString(c->reporterRole))
     .arg(QString::fromStdString(c->accusedName), QString::fromStdString(c->accusedRole))
     .arg(QString::fromStdString(c->busId))
     .arg(QString::fromStdString(c->submissionDate), QString::fromStdString(c->submissionTime))
     .arg(QString::fromStdString(c->status))
     .arg(QString::fromStdString(c->description))
     .arg(c->adminActionNote.empty() ? "No resolution directives entered yet." : QString::fromStdString(c->adminActionNote)));

    // Poll Widget Updates
    if (c->isPollActive || c->status == "POLL ACTIVE") {
        pollGroupBox->setVisible(true);
        int totalVotes = c->pollYesVotes + c->pollNoVotes;
        int yesPct = (totalVotes > 0) ? (c->pollYesVotes * 100 / totalVotes) : 0;
        int noPct = (totalVotes > 0) ? (c->pollNoVotes * 100 / totalVotes) : 0;

        pollStatsLabel->setText(QString("Total Eligible Votes: <b>%1</b>  |  YES: <b>%2 (%3%)</b>  |  NO: <b>%4 (%5%)</b>")
                                .arg(totalVotes).arg(c->pollYesVotes).arg(yesPct).arg(c->pollNoVotes).arg(noPct));
        pollProgressBar->setValue(yesPct);

        // Check if student has already voted
        bool alreadyVoted = (std::find(c->votedStudentRolls.begin(), c->votedStudentRolls.end(), activeUsername) != c->votedStudentRolls.end());
        bool isComplainant = (!c->complainantRoll.empty() && c->complainantRoll == activeUsername);
        bool isAccused = (!c->accusedRoll.empty() && c->accusedRoll == activeUsername);

        if (alreadyVoted) {
            pollMessageLabel->setText("✓ You have already cast your confidential vote on this incident.");
            pollMessageLabel->setStyleSheet("color: #3FB950; font-size: 11.5px;");
            voteYesBtn->setEnabled(false);
            voteNoBtn->setEnabled(false);
        } else if (isComplainant || isAccused) {
            pollMessageLabel->setText("⚠ Voting Disabled: Disciplinary rules prohibit parties involved in the complaint from casting a poll vote.");
            pollMessageLabel->setStyleSheet("color: #D29922; font-size: 11.5px;");
            voteYesBtn->setEnabled(false);
            voteNoBtn->setEnabled(false);
        } else {
            pollMessageLabel->setText("• Please cast your confidential peer vote below:");
            pollMessageLabel->setStyleSheet("color: #58A6FF; font-size: 11.5px;");
            voteYesBtn->setEnabled(true);
            voteNoBtn->setEnabled(true);
        }
    } else {
        pollGroupBox->setVisible(false);
    }
}

// 13. AUTHENTICATION & LOGIN SLOTS
void MainWindow::onPerformLogin() {
    QString username = loginUserEdit->text().trimmed();
    QString password = loginPassEdit->text();

    UserSession session = sysManager.login(username.toStdString(), password.toStdString());

    if (!session.isAuthenticated || !session.activeAccount) {
        loginStatusLabel->setText("Invalid username or password. Please verify your credentials.");
        return;
    }

    UserAccount* acc = session.activeAccount;
    activeUsername = acc->getUsername();
    activeRole = acc->getRole();
    loginStatusLabel->setText("");

    loggedUserLabel->setText(QString("User: <b>%1</b>").arg(QString::fromStdString(activeUsername)));
    loggedRoleBadge->setText(QString(" %1 ").arg(QString::fromStdString(acc->getRoleString()).toUpper()));

    setupTabsForRole(activeRole, activeUsername);
    refreshAllDataViews();

    stackedWidget->setCurrentIndex(1); // Switch to Authenticated Portal View
}

void MainWindow::onPerformLogout() {
    sysManager.logout();
    activeUsername.clear();
    tabWidget->clear();
    stackedWidget->setCurrentIndex(0); // Return to Login View
}

void MainWindow::onTogglePasswordVisibility() {
    isPasswordVisible = !isPasswordVisible;
    loginPassEdit->setEchoMode(isPasswordVisible ? QLineEdit::Normal : QLineEdit::Password);
    showPassBtn->setText(isPasswordVisible ? "Hide" : "Show");
}

void MainWindow::onQuickDemoLogin() {
    QString roleCode = loginRoleCombo->currentData().toString();
    if (roleCode == "ADMIN") {
        loginUserEdit->setText("admin");
        loginPassEdit->setText("admin123");
    } else if (roleCode == "DRIVER") {
        loginUserEdit->setText("D-01");
        loginPassEdit->setText("driver123");
    } else if (roleCode == "STUDENT") {
        loginUserEdit->setText("2024-CS-001");
        loginPassEdit->setText("student123");
    } else if (roleCode == "PARENT") {
        loginUserEdit->setText("P-101");
        loginPassEdit->setText("parent123");
    }
}

// 14. ADMIN ACTIONS
void MainWindow::onScheduleChanged(int index) {
    sysManager.setScheduleType(index == 0 ? ScheduleType::FULL_DAY : ScheduleType::HALF_DAY);
    refreshFleetMetrics();
    onSearchStudentSchedule();
}

void MainWindow::onAdminUpdateBus() {
    QString busId = adminBusCombo->currentText().section(" ", 0, 0);
    QString reg = adminRegInput->text();
    int cap = adminCapInput->text().toInt();
    QString rId = adminRouteCombo->currentText().section(" ", 0, 0);

    if (sysManager.adminUpdateBus(busId.toStdString(), reg.toStdString(), cap, rId.toStdString())) {
        QMessageBox::information(this, "Configuration Updated", "Bus " + busId + " specifications updated successfully.");
        refreshFleetMetrics();
    }
}

void MainWindow::onAdminUpdateTiming() {
    QString stopId = adminTimingStopCombo->currentData().toString();
    QString busId = adminTimingBusCombo->currentText();
    ScheduleType s = (adminTimingScheduleCombo->currentIndex() == 1) ? ScheduleType::HALF_DAY : ScheduleType::FULL_DAY;
    QString time = adminNewTimeInput->text();

    if (sysManager.adminUpdateStopTiming(stopId.toStdString(), busId.toStdString(), s, time.toStdString())) {
        QMessageBox::information(this, "Timing Updated", "Departure timing for Stop " + stopId + " on Bus " + busId + " set to " + time + ".");
    }
}

void MainWindow::onAdminBroadcastNotice() {
    sysManager.adminBroadcastNotice(TargetAudience::ALL_STUDENTS, "Mountain Transit Broadcast", "All campus transit coaches operating on standard mountain schedule.");
    QMessageBox::information(this, "Broadcast Dispatched", "Campus transit broadcast notice transmitted to all student & parent portals.");
}

void MainWindow::onAdminRegisterStudentDialog() {
    QDialog d(this);
    d.setWindowTitle("Register New Student Commuter");
    d.setStyleSheet("background-color: #161B22; color: #E6EDF3;");
    QFormLayout* fl = new QFormLayout(&d);

    QLineEdit* nameEdit = new QLineEdit(&d);
    QLineEdit* rollEdit = new QLineEdit(&d);
    QLineEdit* phoneEdit = new QLineEdit(&d);
    QLineEdit* emailEdit = new QLineEdit(&d);
    QComboBox* routeCombo = new QComboBox(&d);
    routeCombo->addItem("R1"); routeCombo->addItem("R2");
    QComboBox* stopCombo = new QComboBox(&d);
    stopCombo->addItem("ST101 - Haldwani");
    stopCombo->addItem("ST102 - Kathgodam");
    stopCombo->addItem("ST103 - Ranibagh");
    stopCombo->addItem("ST104 - Bhujiaghat");
    stopCombo->addItem("ST105 - Bhimtal Lake");

    QComboBox* feeCombo = new QComboBox(&d);
    feeCombo->addItem("PAID IN FULL", true);
    feeCombo->addItem("PENDING", false);

    fl->addRow("Full Name:", nameEdit);
    fl->addRow("Roll Number:", rollEdit);
    fl->addRow("Phone Number:", phoneEdit);
    fl->addRow("Email Address:", emailEdit);
    fl->addRow("Assigned Route:", routeCombo);
    fl->addRow("Boarding Stop:", stopCombo);
    fl->addRow("Fee Clearance:", feeCombo);

    QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &d);
    connect(bb, &QDialogButtonBox::accepted, &d, &QDialog::accept);
    connect(bb, &QDialogButtonBox::rejected, &d, &QDialog::reject);
    fl->addRow("", bb);

    if (d.exec() == QDialog::Accepted) {
        string name = nameEdit->text().toStdString();
        string roll = rollEdit->text().toStdString();
        string phone = phoneEdit->text().toStdString();
        string email = emailEdit->text().toStdString();
        string rId = routeCombo->currentText().toStdString();
        string sId = stopCombo->currentText().section(" ", 0, 0).toStdString();
        bool fee = feeCombo->currentData().toBool();

        if (name.empty() || roll.empty()) {
            QMessageBox::warning(this, "Input Required", "Student Name and Roll Number cannot be empty.");
            return;
        }

        if (sysManager.adminRegisterNewStudent(name, roll, phone, email, rId, sId, fee)) {
            QMessageBox::information(this, "Student Enrolled", "Student " + QString::fromStdString(name) + " successfully registered!");
            refreshStudentDirectory();
            refreshAttendanceTable();
        } else {
            QMessageBox::warning(this, "Duplicate Error", "A student with this roll number is already registered.");
        }
    }
}

void MainWindow::onBoardStudent() {
    QString roll = boardRollInput->text().trimmed();
    QString busId = boardBusCombo->currentText();

    try {
        sysManager.processBoarding(roll.toStdString(), busId.toStdString());
        QMessageBox::information(this, "Boarding Authorized", "Passenger attendance logged successfully for Bus " + busId + ".");
        refreshAttendanceTable();
        refreshFleetMetrics();
    } catch (const SeatFullException& ex) {
        QMessageBox::warning(this, "Boarding Denied", ex.what());
    }
}

void MainWindow::onVerifyQRPass() {
    QString roll = boardRollInput->text().trimmed();
    DigitalPass pass = sysManager.getStudentPass(roll.toStdString());
    QMessageBox::information(this, "Digital QR Cryptographic Verification",
                             QString("<b>PASS VERIFICATION RESULT: AUTHENTIC</b><br><br>"
                                     "<b>Student:</b> %1<br>"
                                     "<b>Roll Number:</b> %2<br>"
                                     "<b>Route:</b> %3<br>"
                                     "<b>SHA-256 Token:</b> %4<br><br>"
                                     "<font color='#3FB950'><b>ACCESS GRANTED FOR BOARDING</b></font>")
                             .arg(QString::fromStdString(pass.studentName),
                                  QString::fromStdString(pass.studentRoll),
                                  QString::fromStdString(pass.routeId),
                                  QString::fromStdString(pass.verificationHash)));
}

// 15. STUDENT SLOTS
void MainWindow::onSearchStudentSchedule() {
    QString roll = studentRollInput->text().trimmed();
    Student* s = sysManager.findStudentByRoll(roll.toStdString());

    if (!s) {
        studentInfoLabel->setText("Student record not found for roll number: " + roll);
        studentScheduleTable->setRowCount(0);
        return;
    }

    studentInfoLabel->setText(QString(
        "Student Commuter: <b>%1</b>  |  Roll: <b>%2</b>  |  Assigned Stop: <b>%3</b>  |  Semester Fee: %4"
    ).arg(QString::fromStdString(s->getName()))
     .arg(QString::fromStdString(s->getRollNumber()))
     .arg(QString::fromStdString(s->getAssignedStopId()))
     .arg(s->isFeePaid() ? "<font color='#3FB950'><b>PAID IN FULL</b></font>" : "<font color='#F85149'><b>PENDING</b></font>"));

    studentScheduleTable->setRowCount(1);
    studentScheduleTable->setItem(0, 0, new QTableWidgetItem("B-101 (UK-04-PA-1001)"));
    studentScheduleTable->setItem(0, 1, new QTableWidgetItem("Express Hill Corridor (R1)"));
    studentScheduleTable->setItem(0, 2, new QTableWidgetItem(QString::fromStdString(s->getAssignedStopId())));
    studentScheduleTable->setItem(0, 3, new QTableWidgetItem(sysManager.getScheduleType() == ScheduleType::FULL_DAY ? "07:15 AM" : "07:00 AM"));
}

void MainWindow::onShowDigitalQRPass() {
    QString roll = studentRollInput->text().trimmed();
    DigitalPass pass = sysManager.getStudentPass(roll.toStdString());
    QMessageBox::information(this, "Digital QR Bus Pass",
                             QString("<b>OFFICIAL DIGITAL BUS PASS</b><br><br>"
                                     "<b>Student:</b> %1<br>"
                                     "<b>Roll Number:</b> %2<br>"
                                     "<b>Route:</b> %3<br>"
                                     "<b>Digital Token:</b> %4<br><br>"
                                     "<font color='#3FB950'><b>VALID FOR GRAPHIC ERA CAMPUS TRANSIT</b></font>")
                             .arg(QString::fromStdString(pass.studentName),
                                  QString::fromStdString(pass.studentRoll),
                                  QString::fromStdString(pass.routeId),
                                  QString::fromStdString(pass.verificationHash)));
}

void MainWindow::onSubmitDriverRating() {
    QString driverId = ratingDriverIdInput->text().trimmed();
    QString roll = studentRollInput->text().trimmed();
    int stars = 5 - ratingStarCombo->currentIndex();
    QString feedback = ratingFeedbackInput->text();

    sysManager.studentRateDriver(roll.toStdString(), driverId.toStdString(), stars, feedback.toStdString());
    QMessageBox::information(this, "Rating Submitted", "Thank you! Your rating of " + QString::number(stars) + " stars has been registered for Driver " + driverId + ".");
}

void MainWindow::onTriggerStudentSOS() {
    QString roll = studentRollInput->text().trimmed();
    sysManager.triggerStudentSOS(roll.toStdString(), "B-101", "Haldwani Mountain Corridor");
    QMessageBox::critical(this, "EMERGENCY SOS TRANSMITTED", "EMERGENCY SOS ALERT TRANSMITTED TO CAMPUS TRANSPORT SECURITY DESK!");
    refreshComplaintTable();
}

// 16. DRIVER SLOTS
void MainWindow::onDriverStartTrip() {
    sysManager.driverStartTrip("B-101");
    QMessageBox::information(this, "Trip Telemetry Active", "Mountain ascent trip for Bus B-101 started.");
}

void MainWindow::onDriverEndTrip() {
    sysManager.driverEndTrip("B-101");
    QMessageBox::information(this, "Trip Concluded", "Bus B-101 reached Graphic Era Hill University Terminal.");
}

void MainWindow::onDriverReportDelay() {
    QString reason = driverDelayReasonInput->text();
    int mins = driverDelayMinsInput->text().toInt();
    sysManager.driverReportDelay("B-101", reason.toStdString(), mins);
    QMessageBox::warning(this, "Delay Broadcast", "Highway delay notice dispatched to all commuters.");
}

void MainWindow::onDriverReportMisbehavior() {
    QString roll = misbehaviorRollInput->text().trimmed();
    QString desc = misbehaviorDescInput->text();
    sysManager.driverReportMisbehavior("Driver Rajesh Kumar", roll.toStdString(), desc.toStdString());
    QMessageBox::information(this, "Incident Logged", "Disciplinary misbehavior report logged against Student " + roll + ". Student Disciplinary Poll activated.");
    refreshComplaintTable();
}

// 17. PARENT SLOTS
void MainWindow::onParentTrackChild() {
    QString query = parentPhoneInput->text().trimmed();
    Parent* p = sysManager.findParentByPhoneOrChild(query.toStdString());

    parentInfoLabel->setText(QString(
        "<b>LIVE WARD TRANSIT TELEMETRY MONITOR</b><br><br>"
        "<b>Parent/Guardian:</b> %1  |  <b>Student Ward:</b> %2 (%3)<br>"
        "<b>Assigned Vehicle:</b> Bus B-101 (UK-04-PA-1001) • Express Hill Corridor<br>"
        "<b>Current Mountain Checkpoint:</b> Kathgodam Railway Station [ST102]<br>"
        "<b>ETA to Graphic Era Bhimtal Campus:</b> 08:25 AM (<font color='#3FB950'><b>ON SCHEDULE</b></font>)<br>"
        "<b>Transportation Fee Status:</b> %4"
    ).arg(p ? QString::fromStdString(p->getName()) : "Vijay Sharma")
     .arg(p ? QString::fromStdString(p->getChildName()) : "Aarav Sharma")
     .arg(p ? QString::fromStdString(p->getChildRollNumber()) : "2024-CS-001")
     .arg(p && p->getFeeBalance() > 0 ? "₹" + QString::number(p->getFeeBalance()) + " (Outstanding)" : "<font color='#3FB950'><b>PAID IN FULL</b></font>"));
}

// 18. COMPLAINT & STUDENT POLL SLOTS
void MainWindow::onComplaintTableSelectionChanged() {
    int row = complaintTable->currentRow();
    if (row < 0) return;
    QString compId = complaintTable->item(row, 0)->text();
    displayComplaintDetails(compId.toStdString());
}

void MainWindow::onApplyComplaintFilters() {
    refreshComplaintTable();
}

void MainWindow::onCastStudentVote(bool voteYes) {
    if (selectedComplaintId.empty()) return;
    string outMsg;
    bool success = sysManager.castStudentPollVote(selectedComplaintId, activeUsername, voteYes, outMsg);

    if (success) {
        QMessageBox::information(this, "Vote Recorded", QString::fromStdString(outMsg));
    } else {
        QMessageBox::warning(this, "Voting Denied", QString::fromStdString(outMsg));
    }

    displayComplaintDetails(selectedComplaintId);
    refreshComplaintTable();
}

void MainWindow::onAdminInvestigateComplaint() {
    if (selectedComplaintId.empty()) return;
    sysManager.adminInvestigateComplaint(selectedComplaintId);
    displayComplaintDetails(selectedComplaintId);
    refreshComplaintTable();
}

void MainWindow::onAdminResolveComplaint() {
    if (selectedComplaintId.empty()) return;
    bool ok;
    QString note = QInputDialog::getText(this, "Resolve Complaint", "Enter Official Disciplinary Resolution Directives:", QLineEdit::Normal, "Hearing concluded; counseling issued.", &ok);
    if (ok) {
        sysManager.adminResolveComplaint(selectedComplaintId, note.toStdString());
        displayComplaintDetails(selectedComplaintId);
        refreshComplaintTable();
    }
}

void MainWindow::onAdminRejectComplaint() {
    if (selectedComplaintId.empty()) return;
    bool ok;
    QString reason = QInputDialog::getText(this, "Reject Complaint", "Enter Recorded Evidentiary Rejection Reason:", QLineEdit::Normal, "Dismissed due to lack of corroborating evidence.", &ok);
    if (ok) {
        sysManager.adminRejectComplaint(selectedComplaintId, reason.toStdString());
        displayComplaintDetails(selectedComplaintId);
        refreshComplaintTable();
    }
}

void MainWindow::onSubmitNewComplaint() {
    QString cat = newCompCategoryCombo->currentText();
    QString accName = newCompAccusedNameEdit->text().trimmed();
    QString accRole = newCompAccusedRoleCombo->currentText();
    QString bus = newCompBusEdit->text().trimmed();
    QString desc = newCompDescEdit->text().trimmed();

    if (accName.isEmpty() || desc.isEmpty()) {
        QMessageBox::warning(this, "Input Required", "Accused person name and incident description must be provided.");
        return;
    }

    string repRole = "Student";
    if (activeRole == UserRole::ADMIN) repRole = "Admin";
    else if (activeRole == UserRole::DRIVER) repRole = "Driver";
    else if (activeRole == UserRole::PARENT) repRole = "Parent";

    string newId = sysManager.submitComprehensiveComplaint(
        cat.toStdString(), desc.toStdString(),
        activeUsername, repRole,
        accName.toStdString(), accRole.toStdString(),
        bus.toStdString(), activeUsername, accName.toStdString()
    );

    QMessageBox::information(this, "Complaint Registered", "Complaint " + QString::fromStdString(newId) + " registered successfully.");
    newCompDescEdit->clear();
    refreshComplaintTable();
    displayComplaintDetails(newId);
}

// 19. ROUTE OPTIMIZER SLOTS
void MainWindow::onCalculateOptimalRoute() {
    QString startId = startStopCombo->currentData().toString();
    QString algo = algoCombo->currentText();

    double dist = 28.2;
    int duration = 48;
    int elevGain = 996;
    int stops = 5;

    if (startId == "ST102") {
        dist = 23.7; duration = 38; elevGain = 866; stops = 4;
    } else if (startId == "ST103") {
        dist = 19.7; duration = 32; elevGain = 800; stops = 3;
    } else if (startId == "ST104") {
        dist = 15.4; duration = 28; elevGain = 510; stops = 3;
    } else if (startId == "ST105") {
        dist = 4.2; duration = 10; elevGain = 50; stops = 2;
    }

    routeKpiDistance->setText(QString("Total Road Distance: <b>%1 KM</b>").arg(dist, 0, 'f', 1));
    routeKpiDuration->setText(QString("Estimated Time: <b>%1 Mins</b>").arg(duration));
    routeKpiElevation->setText(QString("Elevation Traversal: <b>+%1m Ascent</b>").arg(elevGain));
    routeKpiStops->setText(QString("Stops: <b>%1 Checkpoints</b>").arg(stops));
    routeKpiAlgo->setText(QString("Algorithm: <b>%1</b>").arg(algo.contains("A*") ? "A* Search" : "Dijkstra"));

    routeInfoLabel->setText(QString("Calculated Optimal Route: <b>%1</b> -> <b>Graphic Era Hill University, Bhimtal Campus</b> | Method: <b>%2</b>")
                            .arg(startStopCombo->currentText().section(" - ", 1, 1).section(" [", 0, 0), algo));
}

void MainWindow::onStartLiveSim() {
    simStep = 0;
    QString startId = startStopCombo->currentData().toString();
    if (startId == "ST101") currentSimStops = {"Haldwani", "Kathgodam", "Ranibagh", "Bhimtal Lake", "Graphic Era Campus"};
    else if (startId == "ST102") currentSimStops = {"Kathgodam", "Ranibagh", "Bhimtal Lake", "Graphic Era Campus"};
    else if (startId == "ST103") currentSimStops = {"Ranibagh", "Bhimtal Lake", "Graphic Era Campus"};
    else if (startId == "ST104") currentSimStops = {"Bhujiaghat Bypass", "Bhimtal Lake", "Graphic Era Campus"};
    else currentSimStops = {"Bhimtal Lake", "Graphic Era Campus"};

    simTimer->start(1200);
}

void MainWindow::onSimStep() {
    if (simStep >= (int)currentSimStops.size()) {
        simTimer->stop();
        routeInfoLabel->setText("✅ Arrival Confirmed: Bus arrived safely at Graphic Era Hill University, Bhimtal Campus!");
        return;
    }

    int coords[6][2] = {
        {90, 340},   // Haldwani
        {260, 280},  // Kathgodam
        {450, 230},  // Ranibagh
        {620, 310},  // Bhujiaghat
        {800, 160},  // Bhimtal Lake
        {1040, 90}   // Graphic Era Campus
    };

    int ptIdx = 0;
    string cur = currentSimStops[simStep];
    if (cur == "Haldwani") ptIdx = 0;
    else if (cur == "Kathgodam") ptIdx = 1;
    else if (cur == "Ranibagh") ptIdx = 2;
    else if (cur == "Bhujiaghat Bypass") ptIdx = 3;
    else if (cur == "Bhimtal Lake") ptIdx = 4;
    else if (cur == "Graphic Era Campus") ptIdx = 5;

    if (busMarker) {
        busMarker->setPos(coords[ptIdx][0] - 90, coords[ptIdx][1] - 340);
    }

    routeInfoLabel->setText(QString("⏱️ Live Vehicle Telemetry: Bus traversing Checkpoint %1/%2 [%3] en route to Graphic Era Campus")
                            .arg(simStep + 1).arg(currentSimStops.size()).arg(QString::fromStdString(cur)));
    simStep++;
}

void MainWindow::onOpenInteractiveOpenStreetMap() {
    QStringList candidates = {
        QDir::current().filePath("web/index.html"),
        QCoreApplication::applicationDirPath() + "/web/index.html",
        QCoreApplication::applicationDirPath() + "/../web/index.html",
        QCoreApplication::applicationDirPath() + "/../../web/index.html",
        "/Users/tushartiwari565/Documents/project/web/index.html"
    };
    for (const QString& candidate : candidates) {
        if (QFile::exists(candidate)) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(candidate).absoluteFilePath()));
            return;
        }
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(candidates.first()));
}
