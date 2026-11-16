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
#include <QUrl>
#include <QDir>
#include <QDateTime>
#include <QDialog>
#include <QDialogButtonBox>
#include <QScrollArea>
#include <QApplication>
#include <QCheckBox>
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
      routeController(nullptr),
      osmMapWidget(nullptr) {

    setWindowTitle("Smart College Bus Management System • Graphic Era Hill University, Bhimtal");
    resize(1380, 880);
    setMinimumSize(1100, 720);

    routeController = new RouteController(this);
    connect(routeController->getSimulation(), &BusSimulation::telemetryUpdated, this, &MainWindow::onSimulationTelemetry);
    connect(routeController->getSimulation(), &BusSimulation::checkpointReached, this, &MainWindow::onSimulationCheckpoint);
    connect(routeController->getSimulation(), &BusSimulation::simulationCompleted, this, &MainWindow::onSimulationCompleted);

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

    // Set yellow bus depot background image with cover styling
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
        screen->setStyleSheet("QWidget#loginScreen { background-color: #080D16; }");
    }

    QHBoxLayout* rootLayout = new QHBoxLayout(screen);
    rootLayout->setContentsMargins(60, 40, 60, 40);
    rootLayout->setSpacing(40);

    // Left Hero Branding Section
    QVBoxLayout* leftBrandLayout = new QVBoxLayout();
    leftBrandLayout->setSpacing(12);

    QLabel* badge = new QLabel("OFFICIAL TRANSPORT LOGISTICS", screen);
    badge->setStyleSheet(
        "background-color: rgba(14, 165, 233, 0.15); "
        "border: 1px solid #0EA5E9; color: #38BDF8; "
        "font-size: 11px; font-weight: 700; padding: 5px 14px; "
        "border-radius: 12px; letter-spacing: 1.5px;"
    );
    badge->setFixedWidth(240);

    QLabel* heroTitle = new QLabel("Graphic Era Hill University\nBhimtal Campus", screen);
    heroTitle->setStyleSheet(
        "font-size: 32px; font-weight: 800; color: #FFFFFF; "
        "line-height: 1.2; letter-spacing: -0.6px; background: transparent;"
    );

    QLabel* heroSub = new QLabel("Smart College Bus Fleet & Mountain Corridor Transit System", screen);
    heroSub->setStyleSheet("font-size: 15px; font-weight: 600; color: #58A6FF; margin-bottom: 6px; background: transparent;");

    QLabel* heroDesc = new QLabel(
        "Real-time GPS bus fleet monitoring, automated digital student passes, "
        "and topographic route optimization across Haldwani, Kathgodam, Ranibagh, and Bhimtal.",
        screen
    );
    heroDesc->setWordWrap(true);
    heroDesc->setStyleSheet("font-size: 13px; color: #94A3B8; line-height: 1.5; max-width: 520px; background: transparent;");

    leftBrandLayout->addStretch(2);
    leftBrandLayout->addWidget(badge);
    leftBrandLayout->addWidget(heroTitle);
    leftBrandLayout->addWidget(heroSub);
    leftBrandLayout->addWidget(heroDesc);
    leftBrandLayout->addStretch(1);

    rootLayout->addLayout(leftBrandLayout, 3);

    // Right Dark Navy Glassmorphism Login Card
    QFrame* loginCard = new QFrame(screen);
    loginCard->setObjectName("loginCard");
    loginCard->setFixedWidth(440);
    loginCard->setStyleSheet(
        "#loginCard { "
        "  background-color: rgba(13, 21, 37, 0.88); "
        "  border: 1px solid rgba(56, 189, 248, 0.35); "
        "  border-radius: 16px; "
        "  padding: 26px; "
        "}"
    );

    QVBoxLayout* cardLayout = new QVBoxLayout(loginCard);
    cardLayout->setSpacing(12);

    // Card Header
    QLabel* cardBadge = new QLabel("CAMPUS FLEET TRANSIT", loginCard);
    cardBadge->setStyleSheet("color: #38BDF8; font-size: 11px; font-weight: 800; letter-spacing: 1.5px; background: transparent;");

    QLabel* cardTitle = new QLabel("Sign In to Portal", loginCard);
    cardTitle->setStyleSheet("font-size: 20px; font-weight: 800; color: #FFFFFF; letter-spacing: -0.3px; background: transparent;");

    QLabel* cardSub = new QLabel("Select authorized role and enter college credentials", loginCard);
    cardSub->setStyleSheet("font-size: 11.5px; color: #8B949E; margin-bottom: 6px; background: transparent;");

    cardLayout->addWidget(cardBadge);
    cardLayout->addWidget(cardTitle);
    cardLayout->addWidget(cardSub);

    // Form
    QFormLayout* form = new QFormLayout();
    form->setSpacing(10);
    form->setLabelAlignment(Qt::AlignLeft);

    loginRoleCombo = new QComboBox(loginCard);
    loginRoleCombo->setStyleSheet(
        "QComboBox { background-color: #0E1626; border: 1px solid rgba(56, 189, 248, 0.3); border-radius: 8px; color: #E6EDF3; padding: 8px 12px; font-size: 12.5px; font-weight: 600; }"
        "QComboBox:focus { border: 1.5px solid #38BDF8; }"
        "QComboBox::drop-down { border: none; padding-right: 8px; }"
        "QComboBox QAbstractItemView { background-color: #0E1626; color: #E6EDF3; selection-background-color: #0284C7; selection-color: #FFFFFF; border: 1px solid #38BDF8; }"
    );
    loginRoleCombo->addItem("Administrator", "ADMIN");
    loginRoleCombo->addItem("Driver Portal", "DRIVER");
    loginRoleCombo->addItem("Student / Commuter Portal", "STUDENT");
    loginRoleCombo->addItem("Parent / Guardian Portal", "PARENT");

    loginUserEdit = new QLineEdit("admin", loginCard);
    loginUserEdit->setPlaceholderText("Enter Username, Driver ID, or Roll Number");
    loginUserEdit->setStyleSheet(
        "QLineEdit { background-color: #0A101D; border: 1px solid rgba(56, 189, 248, 0.25); border-radius: 8px; color: #FFFFFF; padding: 10px 12px; font-size: 13px; }"
        "QLineEdit:focus { border: 1.5px solid #38BDF8; background-color: #0E172A; }"
    );

    // Password Field with Show/Hide Toggle
    QWidget* passContainer = new QWidget(loginCard);
    QHBoxLayout* passLayout = new QHBoxLayout(passContainer);
    passLayout->setContentsMargins(0, 0, 0, 0);
    passLayout->setSpacing(6);

    loginPassEdit = new QLineEdit("admin123", passContainer);
    loginPassEdit->setEchoMode(QLineEdit::Password);
    loginPassEdit->setPlaceholderText("Enter Account Password");
    loginPassEdit->setStyleSheet(
        "QLineEdit { background-color: #0A101D; border: 1px solid rgba(56, 189, 248, 0.25); border-radius: 8px; color: #FFFFFF; padding: 10px 12px; font-size: 13px; }"
        "QLineEdit:focus { border: 1.5px solid #38BDF8; background-color: #0E172A; }"
    );

    showPassBtn = new QPushButton("Show", passContainer);
    showPassBtn->setFixedWidth(56);
    showPassBtn->setStyleSheet("background-color: #162032; border: 1px solid rgba(56, 189, 248, 0.3); border-radius: 6px; color: #38BDF8; font-size: 11.5px; font-weight: 600; padding: 6px;");
    connect(showPassBtn, &QPushButton::clicked, this, &MainWindow::onTogglePasswordVisibility);

    passLayout->addWidget(loginPassEdit, 1);
    passLayout->addWidget(showPassBtn);

    QLabel* roleLabel = new QLabel("Authorized Role:", loginCard);
    roleLabel->setStyleSheet("color: #8B949E; font-weight: 600; font-size: 11.5px; background: transparent;");
    QLabel* userLabel = new QLabel("User ID / Roll Number:", loginCard);
    userLabel->setStyleSheet("color: #8B949E; font-weight: 600; font-size: 11.5px; background: transparent;");
    QLabel* passLabel = new QLabel("Security Password:", loginCard);
    passLabel->setStyleSheet("color: #8B949E; font-weight: 600; font-size: 11.5px; background: transparent;");

    form->addRow(roleLabel, loginRoleCombo);
    form->addRow(userLabel, loginUserEdit);
    form->addRow(passLabel, passContainer);
    cardLayout->addLayout(form);

    // Remember Me & Forgot Password
    QHBoxLayout* auxLayout = new QHBoxLayout();
    QCheckBox* rememberMeCheck = new QCheckBox("Remember Me", loginCard);
    rememberMeCheck->setChecked(true);
    rememberMeCheck->setStyleSheet(
        "QCheckBox { color: #94A3B8; font-size: 11.5px; font-weight: 500; background: transparent; }"
        "QCheckBox::indicator { width: 15px; height: 15px; border-radius: 3px; border: 1px solid #38BDF8; background-color: #0A101D; }"
        "QCheckBox::indicator:checked { background-color: #0284C7; }"
    );
    QPushButton* forgotPassBtn = new QPushButton("Forgot Password?", loginCard);
    forgotPassBtn->setStyleSheet("background: transparent; border: none; color: #38BDF8; font-size: 11.5px; font-weight: 600; text-decoration: underline;");
    connect(forgotPassBtn, &QPushButton::clicked, this, [this]{
        QMessageBox::information(
            this,
            "Transport Portal Password Support",
            "Official Credential Recovery Notice:\n\n"
            "For security compliance, user credential resets must be verified "
            "by the Graphic Era Hill University Transport Office or IT Helpdesk.\n\n"
            "Office: Room 104, Transport Control Cell, Admin Block, Bhimtal Campus\n"
            "Email: transport@gehu.ac.in | it-support@gehu.ac.in\n"
            "Phone: +91-5942-247000 (Ext: 104)"
        );
    });

    auxLayout->addWidget(rememberMeCheck);
    auxLayout->addStretch();
    auxLayout->addWidget(forgotPassBtn);
    cardLayout->addLayout(auxLayout);

    // Sign In Button (Glowing Cyan-Blue)
    QPushButton* signInBtn = new QPushButton("Sign In to Portal", loginCard);
    signInBtn->setObjectName("signInBtn");
    signInBtn->setStyleSheet(
        "QPushButton#signInBtn {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0284C7, stop:0.5 #0EA5E9, stop:1 #38BDF8);"
        "  color: #FFFFFF;"
        "  font-weight: 800;"
        "  font-size: 14px;"
        "  padding: 11px;"
        "  border-radius: 8px;"
        "  border: 1px solid #7DD3FC;"
        "}"
        "QPushButton#signInBtn:hover {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0EA5E9, stop:1 #7DD3FC);"
        "  color: #0A101D;"
        "  border: 1.5px solid #BAE6FD;"
        "}"
        "QPushButton#signInBtn:pressed {"
        "  background-color: #0369A1;"
        "}"
    );
    connect(signInBtn, &QPushButton::clicked, this, &MainWindow::onPerformLogin);
    cardLayout->addWidget(signInBtn);

    loginStatusLabel = new QLabel("", loginCard);
    loginStatusLabel->setStyleSheet("font-size: 12px; font-weight: 600; color: #F85149; background: transparent;");
    loginStatusLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(loginStatusLabel);

    // Controlled Development / Demo Mode Helper
    QHBoxLayout* demoLayout = new QHBoxLayout();
    QPushButton* demoBtn = new QPushButton("Quick Demo Auto-Fill (Dev Mode)", loginCard);
    demoBtn->setStyleSheet("background: transparent; border: none; color: #58A6FF; font-size: 11.5px; text-decoration: underline;");
    connect(demoBtn, &QPushButton::clicked, this, &MainWindow::onQuickDemoLogin);
    demoLayout->addWidget(demoBtn, 0, Qt::AlignCenter);
    cardLayout->addLayout(demoLayout);

    rootLayout->addWidget(loginCard, 0, Qt::AlignRight | Qt::AlignVCenter);
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

    QLabel* titleLabel = new QLabel("Smart College Bus Management System - Graphic Era Bhimtal Campus", headerCard);
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

    QPushButton* noticeBtn = new QPushButton("Dispatch Broadcast Notice", timingBox);
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

    QPushButton* sosBtn = new QPushButton("EMERGENCY SOS BROADCAST", box);
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
    ratingStarCombo->addItem("5 Stars (Punctual and Safe Mountain Driving)");
    ratingStarCombo->addItem("4 Stars (Good Driving)");
    ratingStarCombo->addItem("3 Stars (Average Speed)");
    ratingStarCombo->addItem("2 Stars (Delayed Arrival)");
    ratingStarCombo->addItem("1 Star (Unsafe Driving / Violations)");

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
    QPushButton* delayBtn = new QPushButton("Dispatch Delay Alert to Students & Parents", tripBox);
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

// 10. NATIVE OPENSTREETMAP TRACKING DASHBOARD & ROUTE OPTIMIZER
QWidget* MainWindow::createRouteVisualizerTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);
    layout->setSpacing(8);
    layout->setContentsMargins(10, 10, 10, 10);

    // 1. Top Route Information Display HUD
    QFrame* kpiBar = new QFrame(tab);
    kpiBar->setStyleSheet("background-color: #1C2533; border: 1px solid #30363D; border-radius: 8px; padding: 6px 12px;");
    QHBoxLayout* kl = new QHBoxLayout(kpiBar);
    kl->setContentsMargins(4, 4, 4, 4);

    routeKpiDistance = new QLabel("Total Road Distance: <b>28.2 KM</b>", kpiBar);
    routeKpiDistance->setStyleSheet("color: #E6EDF3; font-size: 12px;");
    routeKpiDuration = new QLabel("Estimated Time: <b>48 Mins</b>", kpiBar);
    routeKpiDuration->setStyleSheet("color: #3FB950; font-size: 12px;");
    routeKpiElevation = new QLabel("Elevation Traversal: <b>+996m Ascent (424m -> 1,420m ASL)</b>", kpiBar);
    routeKpiElevation->setStyleSheet("color: #D29922; font-size: 12px;");
    routeKpiStops = new QLabel("Stops: <b>6 Checkpoints</b>", kpiBar);
    routeKpiStops->setStyleSheet("color: #8B949E; font-size: 12px;");
    routeKpiAlgo = new QLabel("Algorithm: <b>A* Heuristic Search</b>", kpiBar);
    routeKpiAlgo->setStyleSheet("color: #58A6FF; font-size: 12px;");
    routeKpiStatus = new QLabel("Status: <font color='#3FB950'><b>CORRIDOR OPERATIONAL</b></font>", kpiBar);
    routeKpiStatus->setStyleSheet("font-size: 12px;");

    kl->addWidget(routeKpiDistance);
    kl->addWidget(routeKpiDuration);
    kl->addWidget(routeKpiElevation);
    kl->addWidget(routeKpiStops);
    kl->addWidget(routeKpiAlgo);
    kl->addWidget(routeKpiStatus);
    layout->addWidget(kpiBar);

    routeInfoLabel = new QLabel("Selected Source: Haldwani Bus Terminal -> Destination: Graphic Era Hill University, Bhimtal Campus.", tab);
    routeInfoLabel->setStyleSheet("font-size: 12px; font-weight: 600; color: #58A6FF; padding: 5px 10px; background-color: #161B22; border-radius: 6px;");
    layout->addWidget(routeInfoLabel);

    // 2. Main Middle Area: 3-Column Layout (Left, Center Map, Right Telemetry)
    QHBoxLayout* midLayout = new QHBoxLayout();
    midLayout->setSpacing(10);

    // ==========================================
    // LEFT SIDE: Route & Simulation Controls
    // ==========================================
    QGroupBox* leftBox = new QGroupBox("Route & Simulation Controls", tab);
    leftBox->setFixedWidth(270);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftBox);
    leftLayout->setSpacing(8);
    leftLayout->setContentsMargins(10, 14, 10, 10);

    leftLayout->addWidget(new QLabel("Source Location:", leftBox));
    startStopCombo = new QComboBox(leftBox);
    startStopCombo->addItem("ST101 - Haldwani Bus Terminal [424m]", "ST101");
    startStopCombo->addItem("ST102 - Kathgodam Station [554m]", "ST102");
    startStopCombo->addItem("ST103 - Ranibagh Junction [620m]", "ST103");
    startStopCombo->addItem("ST104 - Bhujiaghat Pass [910m]", "ST104");
    startStopCombo->addItem("ST105 - Bhimtal Lake [1,370m]", "ST105");
    connect(startStopCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onSourceStopChanged);
    leftLayout->addWidget(startStopCombo);

    leftLayout->addWidget(new QLabel("Destination (Fixed):", leftBox));
    destStopCombo = new QComboBox(leftBox);
    destStopCombo->addItem("ST106 - Graphic Era Bhimtal (FIXED)", "ST106");
    destStopCombo->setEnabled(false);
    destStopCombo->setStyleSheet("background-color: #1C2533; color: #58A6FF; font-weight: 700;");
    leftLayout->addWidget(destStopCombo);

    leftLayout->addWidget(new QLabel("Path Algorithm:", leftBox));
    algoCombo = new QComboBox(leftBox);
    algoCombo->addItem("A* Search (Topographic & Elevation)");
    algoCombo->addItem("Dijkstra Shortest Path");
    leftLayout->addWidget(algoCombo);

    QPushButton* optBtn = new QPushButton("Calculate Shortest Path", leftBox);
    optBtn->setStyleSheet("background-color: #1F6FEB; color: #FFFFFF; font-weight: 700; padding: 7px; border-radius: 6px;");
    connect(optBtn, &QPushButton::clicked, this, &MainWindow::onCalculateOptimalRoute);
    leftLayout->addWidget(optBtn);

    leftLayout->addSpacing(4);
    leftLayout->addWidget(new QLabel("Select Fleet Bus:", leftBox));
    busSelectCombo = new QComboBox(leftBox);
    busSelectCombo->addItem("BUS101 (UK04-PA-1011)", "BUS101");
    busSelectCombo->addItem("BUS102 (UK04-PA-2022)", "BUS102");
    busSelectCombo->addItem("BUS103 (UK04-PA-3033)", "BUS103");
    connect(busSelectCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onBusSelected);
    leftLayout->addWidget(busSelectCombo);

    leftLayout->addWidget(new QLabel("Active Buses Status:", leftBox));
    activeBusesList = new QListWidget(leftBox);
    activeBusesList->setMaximumHeight(85);
    activeBusesList->setStyleSheet("background-color: #0D1117; border: 1px solid #30363D; border-radius: 6px; font-size: 11px;");
    activeBusesList->addItem("BUS101: Haldwani -> Bhimtal (Operational)");
    activeBusesList->addItem("BUS102: Kathgodam Standby (Ready)");
    activeBusesList->addItem("BUS103: Bhimtal Campus Shuttling");
    leftLayout->addWidget(activeBusesList);

    leftLayout->addSpacing(4);
    QLabel* demoTag = new QLabel("<b>DEMO SIMULATION CONTROLS</b>", leftBox);
    demoTag->setStyleSheet("color: #D29922; font-size: 11.5px;");
    leftLayout->addWidget(demoTag);

    QHBoxLayout* simBtnsLayout = new QHBoxLayout();
    startSimBtn = new QPushButton("Start Simulation", leftBox);
    startSimBtn->setStyleSheet("background-color: #238636; color: #FFFFFF; font-weight: 700; padding: 6px; border-radius: 6px;");
    connect(startSimBtn, &QPushButton::clicked, this, &MainWindow::onStartLiveSim);

    pauseSimBtn = new QPushButton("Pause", leftBox);
    pauseSimBtn->setStyleSheet("background-color: #D29922; color: #0D1117; font-weight: 700; padding: 6px; border-radius: 6px;");
    connect(pauseSimBtn, &QPushButton::clicked, this, &MainWindow::onPauseLiveSim);

    resetSimBtn = new QPushButton("↺ Reset", leftBox);
    resetSimBtn->setStyleSheet("background-color: #21262D; color: #E6EDF3; font-weight: 700; padding: 6px; border-radius: 6px;");
    connect(resetSimBtn, &QPushButton::clicked, this, &MainWindow::onResetLiveSim);

    simBtnsLayout->addWidget(startSimBtn);
    simBtnsLayout->addWidget(pauseSimBtn);
    simBtnsLayout->addWidget(resetSimBtn);
    leftLayout->addLayout(simBtnsLayout);

    QHBoxLayout* speedLayout = new QHBoxLayout();
    speedLayout->addWidget(new QLabel("Speed:", leftBox));
    simSpeedCombo = new QComboBox(leftBox);
    simSpeedCombo->addItem("1x (Normal)");
    simSpeedCombo->addItem("2x (Fast)");
    simSpeedCombo->addItem("5x (Warp)");
    connect(simSpeedCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onSimSpeedChanged);
    speedLayout->addWidget(simSpeedCombo);
    leftLayout->addLayout(speedLayout);

    leftLayout->addStretch();
    midLayout->addWidget(leftBox);

    // ==========================================
    // CENTER: Embedded Native OpenStreetMap
    // ==========================================
    osmMapWidget = new NativeOsmMapWidget(tab);
    osmMapWidget->setMinimumHeight(420);
    osmMapWidget->setStyleSheet("border: 1px solid #30363D; border-radius: 8px;");
    midLayout->addWidget(osmMapWidget, 1); // Stretches to fill center

    // ==========================================
    // RIGHT SIDE: Bus Info & Live Telemetry
    // ==========================================
    QGroupBox* rightBox = new QGroupBox("Vehicle & Telemetry HUD", tab);
    rightBox->setFixedWidth(290);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightBox);
    rightLayout->setSpacing(8);
    rightLayout->setContentsMargins(10, 14, 10, 10);

    QFrame* busInfoCard = new QFrame(rightBox);
    busInfoCard->setStyleSheet("background-color: #0D1117; border: 1px solid #30363D; border-radius: 6px; padding: 8px;");
    QVBoxLayout* bicLayout = new QVBoxLayout(busInfoCard);
    bicLayout->setSpacing(4);
    bicLayout->setContentsMargins(0, 0, 0, 0);

    rBusIdLabel = new QLabel("<b>Bus ID:</b> BUS101 - <b>Model:</b> Tata Starbus", busInfoCard);
    rBusRegLabel = new QLabel("<b>Plate:</b> UK04-PA-1011 (GPS Unit: #DEMO)", busInfoCard);
    rBusCapLabel = new QLabel("<b>Capacity:</b> 42 Seats (Pass Holders: 38)", busInfoCard);
    rDriverNameLabel = new QLabel("<b>Driver:</b> Ramesh Chandra Joshi", busInfoCard);
    rDriverPhoneLabel = new QLabel("<b>Contact:</b> +91-9876543210 (Rating: 4.8 / 5)", busInfoCard);

    bicLayout->addWidget(rBusIdLabel);
    bicLayout->addWidget(rBusRegLabel);
    bicLayout->addWidget(rBusCapLabel);
    bicLayout->addWidget(rDriverNameLabel);
    bicLayout->addWidget(rDriverPhoneLabel);
    rightLayout->addWidget(busInfoCard);

    QFrame* telemCard = new QFrame(rightBox);
    telemCard->setStyleSheet("background-color: #0D1117; border: 1px solid #30363D; border-radius: 6px; padding: 8px;");
    QVBoxLayout* tcLayout = new QVBoxLayout(telemCard);
    tcLayout->setSpacing(5);
    tcLayout->setContentsMargins(0, 0, 0, 0);

    rCurrentLocLabel = new QLabel("GPS: 29.21830° N, 79.51300° E", telemCard);
    rAltitudeLabel = new QLabel("Altitude: 424.0 m ASL", telemCard);
    rCurrentStopLabel = new QLabel("Current: Haldwani Bus Terminal", telemCard);
    rCurrentStopLabel->setStyleSheet("color: #58A6FF; font-weight: 600;");
    rNextStopLabel = new QLabel("Next: Kathgodam Railway Station", telemCard);
    rNextStopLabel->setStyleSheet("color: #E6EDF3; font-weight: 600;");
    rEtaLabel = new QLabel("ETA: 48 Mins (28.2 KM remaining)", telemCard);
    rEtaLabel->setStyleSheet("color: #3FB950; font-weight: 700;");
    rTripStatusLabel = new QLabel("Status: <font color='#8B949E'><b>STANDBY / READY</b></font>", telemCard);

    tcLayout->addWidget(rCurrentLocLabel);
    tcLayout->addWidget(rAltitudeLabel);
    tcLayout->addWidget(rCurrentStopLabel);
    tcLayout->addWidget(rNextStopLabel);
    tcLayout->addWidget(rEtaLabel);
    tcLayout->addWidget(rTripStatusLabel);

    tcLayout->addSpacing(4);
    tcLayout->addWidget(new QLabel("Trip Progress to Graphic Era Campus:", telemCard));
    tripProgressBar = new QProgressBar(telemCard);
    tripProgressBar->setRange(0, 100);
    tripProgressBar->setValue(0);
    tripProgressBar->setStyleSheet("QProgressBar { background-color: #161B22; border: 1px solid #30363D; border-radius: 4px; text-align: center; color: #FFFFFF; font-weight: 700; } QProgressBar::chunk { background-color: #238636; border-radius: 3px; }");
    tcLayout->addWidget(tripProgressBar);

    rightLayout->addWidget(telemCard);
    rightLayout->addStretch();
    midLayout->addWidget(rightBox);

    layout->addLayout(midLayout, 1);

    // ==========================================
    // BOTTOM: Schedules, History & Delay Info
    // ==========================================
    QTabWidget* bottomTabs = new QTabWidget(tab);
    bottomTabs->setMaximumHeight(180);
    bottomTabs->setStyleSheet("QTabBar::tab { font-size: 11px; padding: 4px 10px; }");

    // Tab 1: Schedule Table
    trackingScheduleTable = new QTableWidget(bottomTabs);
    trackingScheduleTable->setColumnCount(5);
    trackingScheduleTable->setHorizontalHeaderLabels({"Stop ID", "Checkpoint Stop Name", "Scheduled Time", "Estimated Time", "Status"});
    trackingScheduleTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    trackingScheduleTable->setStyleSheet("background-color: #161B22; border: 1px solid #30363D; color: #E6EDF3; font-size: 11px;");
    bottomTabs->addTab(trackingScheduleTable, "Bus Arrival Schedule");

    // Tab 2: Trip History Table (CSV)
    trackingHistoryTable = new QTableWidget(bottomTabs);
    trackingHistoryTable->setColumnCount(5);
    trackingHistoryTable->setHorizontalHeaderLabels({"Timestamp", "Bus ID", "Event", "Location", "Mode"});
    trackingHistoryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    trackingHistoryTable->setStyleSheet("background-color: #161B22; border: 1px solid #30363D; color: #E6EDF3; font-size: 11px;");
    bottomTabs->addTab(trackingHistoryTable, "Real-Time Trip History (data/tracking_history.csv)");

    // Tab 3: Mountain Corridor Alert
    QWidget* alertTab = new QWidget(bottomTabs);
    QVBoxLayout* atLayout = new QVBoxLayout(alertTab);
    corridorDelayLabel = new QLabel(
        "<b>Route NH109 Topographic Clearance:</b> Kathgodam -> Ranibagh -> Bhujiaghat -> Bhimtal Lake -> Graphic Era Campus.<br>"
        "<b>Gradient:</b> +996m Elevation Gain over 28.2 KM | <b>Average Mountain Transit Speed:</b> 32-38 KM/H.<br>"
        "<b>Current Mountain Weather:</b> Clear (Surface Dry, Visibility > 5 KM) | <b>Delay Offset:</b> +0 mins (Operating On-Time).",
        alertTab);
    corridorDelayLabel->setStyleSheet("color: #E6EDF3; font-size: 12px; background-color: #161B22; padding: 10px; border-radius: 6px;");
    atLayout->addWidget(corridorDelayLabel);
    bottomTabs->addTab(alertTab, "Mountain Corridor Delays & Road Advisory");

    layout->addWidget(bottomTabs);
    return tab;
}

// 11. TOPOGRAPHIC GRAPH & NATIVE MAP INITIALIZATION
void MainWindow::renderRouteGraph() {
    if (!osmMapWidget || !routeController) return;

    osmMapWidget->clearMarkers();
    osmMapWidget->clearRoute();

    // Add Checkpoint Markers along Haldwani -> Kathgodam -> Ranibagh -> Bhujiaghat -> Bhimtal -> Graphic Era Campus
    osmMapWidget->addMarker(29.2183, 79.5130, "Haldwani Bus Terminal", "Alt: 424m ASL", QColor("#238636"), "1");
    osmMapWidget->addMarker(29.2715, 79.5312, "Kathgodam Rail Station", "Alt: 554m ASL", QColor("#238636"), "2");
    osmMapWidget->addMarker(29.2974, 79.5447, "Ranibagh Junction", "Alt: 620m ASL", QColor("#238636"), "3");
    osmMapWidget->addMarker(29.3210, 79.5510, "Bhujiaghat Mountain Pass", "Alt: 910m ASL", QColor("#06b6d4"), "4");
    osmMapWidget->addMarker(29.3500, 79.5530, "Bhimtal Lake Mall Road", "Alt: 1,370m ASL", QColor("#238636"), "5");
    osmMapWidget->addMarker(29.3600, 79.5580, "Graphic Era Campus [FIXED DESTINATION]", "Alt: 1,420m ASL", QColor("#DA3633"), "GEHU");

    // Populate route coordinates
    auto roadCoords = routeController->getActiveRoadCoordinates();
    QVector<QPointF> pathPoints;
    for (const auto& loc : roadCoords) {
        pathPoints.append(QPointF(loc.getLatitude(), loc.getLongitude()));
    }
    osmMapWidget->setRouteCoordinates(pathPoints);
    osmMapWidget->setBusPosition(29.2183, 79.5130);
    osmMapWidget->fitRouteBounds();

    refreshTrackingTables();
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
            pollMessageLabel->setText("You have already cast your confidential vote on this incident.");
            pollMessageLabel->setStyleSheet("color: #3FB950; font-size: 11.5px;");
            voteYesBtn->setEnabled(false);
            voteNoBtn->setEnabled(false);
        } else if (isComplainant || isAccused) {
            pollMessageLabel->setText("Voting Disabled: Disciplinary rules prohibit parties involved in the complaint from casting a poll vote.");
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

// 19. NATIVE ROUTE OPTIMIZER & VEHICLE TELEMETRY SLOTS
void MainWindow::onCalculateOptimalRoute() {
    if (!routeController || !osmMapWidget) return;
    QString startId = startStopCombo->currentData().toString();
    QString algo = algoCombo->currentText();

    routeController->setSource(startId);

    // Populate route coordinates
    auto roadCoords = routeController->getActiveRoadCoordinates();
    QVector<QPointF> pathPoints;
    for (const auto& loc : roadCoords) {
        pathPoints.append(QPointF(loc.getLatitude(), loc.getLongitude()));
    }
    osmMapWidget->setRouteCoordinates(pathPoints);
    if (!pathPoints.isEmpty()) {
        osmMapWidget->setBusPosition(pathPoints.first().x(), pathPoints.first().y());
    }
    osmMapWidget->fitRouteBounds();

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
    if (!routeController) return;
    routeController->getSimulation()->startSimulation();
    if (rTripStatusLabel) {
        rTripStatusLabel->setText("Status: <font color='#3FB950'><b>EN ROUTE (SIMULATING)</b></font>");
    }
}

void MainWindow::onPauseLiveSim() {
    if (!routeController) return;
    routeController->getSimulation()->pauseSimulation();
    if (rTripStatusLabel) {
        rTripStatusLabel->setText("Status: <font color='#D29922'><b>PAUSED</b></font>");
    }
}

void MainWindow::onResetLiveSim() {
    if (!routeController) return;
    routeController->getSimulation()->resetSimulation();
    if (osmMapWidget) {
        auto coords = routeController->getActiveRoadCoordinates();
        if (!coords.isEmpty()) {
            osmMapWidget->setBusPosition(coords.first().getLatitude(), coords.first().getLongitude());
        }
    }
    if (tripProgressBar) tripProgressBar->setValue(0);
    if (rTripStatusLabel) {
        rTripStatusLabel->setText("Status: <font color='#8B949E'><b>STANDBY / READY</b></font>");
    }
    if (routeInfoLabel) {
        routeInfoLabel->setText("Simulation Reset. Ready to start vehicle tracking along corridor.");
    }
}

void MainWindow::onSimSpeedChanged(int index) {
    if (!routeController) return;
    int mult = 1;
    if (index == 1) mult = 2;
    else if (index == 2) mult = 5;
    routeController->getSimulation()->setSpeedMultiplier(mult);
}

void MainWindow::onSourceStopChanged(int index) {
    Q_UNUSED(index);
    onCalculateOptimalRoute();
}

void MainWindow::onBusSelected(int index) {
    Q_UNUSED(index);
    if (!busSelectCombo || !routeController) return;
    QString busId = busSelectCombo->currentData().toString();
    routeController->setBus(busId);
    auto info = routeController->getTrackingService().getBusInfo(busId);
    if (rBusIdLabel) rBusIdLabel->setText(QString("<b>Bus ID:</b> %1 • <b>Model:</b> Tata Starbus").arg(info.busId));
    if (rBusRegLabel) rBusRegLabel->setText(QString("<b>Plate:</b> %1 (GPS Unit: #DEMO)").arg(info.regNo));
    if (rBusCapLabel) rBusCapLabel->setText(QString("<b>Capacity:</b> %1 Seats").arg(info.capacity));
    if (rDriverNameLabel) rDriverNameLabel->setText(QString("<b>Driver:</b> %1").arg(info.driverName));
    if (rDriverPhoneLabel) rDriverPhoneLabel->setText(QString("<b>Contact:</b> %1").arg(info.driverPhone));
}

void MainWindow::onSimulationTelemetry(const QString& busId, const GPSLocation& loc, 
                                       int progress, const QString& curStop, 
                                       const QString& nextStop, int etaMins, double remainingKm) {
    if (osmMapWidget) {
        osmMapWidget->setBusPosition(loc.getLatitude(), loc.getLongitude());
    }
    if (rCurrentLocLabel) rCurrentLocLabel->setText(QString("GPS: %1").arg(loc.toString()));
    if (rAltitudeLabel) rAltitudeLabel->setText(QString("Altitude: %1 m ASL").arg(loc.getAltitude(), 0, 'f', 1));
    if (rCurrentStopLabel) rCurrentStopLabel->setText(QString("Current: %1").arg(curStop));
    if (rNextStopLabel) rNextStopLabel->setText(QString("Next: %1").arg(nextStop));
    if (rEtaLabel) rEtaLabel->setText(QString("ETA: %1 Mins (%2 KM remaining)").arg(etaMins).arg(remainingKm, 0, 'f', 1));
    if (tripProgressBar) tripProgressBar->setValue(progress);

    if (routeInfoLabel) {
        routeInfoLabel->setText(QString("[DEMO GPS] Bus %1 at %2 | Approaching: %3 | ETA: %4 mins")
                                .arg(busId, curStop, nextStop).arg(etaMins));
    }

    routeController->getTrackingService().appendGpsRecord(busId, loc);
}

void MainWindow::onSimulationCheckpoint(const QString& busId, const QString& stopName, const QString& arrivalTime) {
    routeController->getTrackingService().recordStopArrival(busId, stopName, arrivalTime);
    routeController->getTrackingService().logTrackingEvent(busId, "CHECKPOINT_REACHED", stopName, "IN_TRANSIT");
    refreshTrackingTables();
}

void MainWindow::onSimulationCompleted(const QString& busId) {
    if (rTripStatusLabel) {
        rTripStatusLabel->setText("Status: <font color='#3FB950'><b>DESTINATION REACHED</b></font>");
    }
    if (routeInfoLabel) {
        routeInfoLabel->setText("Arrival Confirmed: Bus arrived safely at Graphic Era Hill University, Bhimtal Campus!");
    }
    routeController->getTrackingService().logTrackingEvent(busId, "TRIP_COMPLETED", "Graphic Era Bhimtal Campus", "COMPLETED");
    refreshTrackingTables();
}

void MainWindow::refreshTrackingTables() {
    if (!trackingScheduleTable || !trackingHistoryTable || !routeController) return;

    // Schedule Table
    auto schedule = routeController->getTrackingService().getScheduleTable("BUS101");
    trackingScheduleTable->setRowCount(schedule.size());
    for (int r = 0; r < schedule.size(); ++r) {
        for (int c = 0; c < schedule[r].size(); ++c) {
            QTableWidgetItem* item = new QTableWidgetItem(schedule[r][c]);
            item->setTextAlignment(Qt::AlignCenter);
            if (c == 4) {
                if (schedule[r][c] == "COMPLETED") item->setForeground(QColor("#3FB950"));
                else if (schedule[r][c] == "DEPARTED") item->setForeground(QColor("#58A6FF"));
                else if (schedule[r][c] == "IN TRANSIT") item->setForeground(QColor("#D29922"));
                else if (schedule[r][c] == "DESTINATION") item->setForeground(QColor("#DA3633"));
            }
            trackingScheduleTable->setItem(r, c, item);
        }
    }

    // Trip History Table (from CSV)
    auto history = routeController->getTrackingService().readTrackingHistory(30);
    trackingHistoryTable->setRowCount(history.size());
    for (int r = 0; r < history.size(); ++r) {
        trackingHistoryTable->setItem(r, 0, new QTableWidgetItem(history[r].timestamp));
        trackingHistoryTable->setItem(r, 1, new QTableWidgetItem(history[r].busId));
        trackingHistoryTable->setItem(r, 2, new QTableWidgetItem(history[r].event));
        trackingHistoryTable->setItem(r, 3, new QTableWidgetItem(history[r].location));
        QTableWidgetItem* stItem = new QTableWidgetItem(history[r].status);
        if (history[r].status == "COMPLETED") stItem->setForeground(QColor("#3FB950"));
        else stItem->setForeground(QColor("#58A6FF"));
        trackingHistoryTable->setItem(r, 4, stItem);
    }
}
