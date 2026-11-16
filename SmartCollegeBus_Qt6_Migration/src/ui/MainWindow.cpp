#include "MainWindow.hpp"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFrame>
#include <QMessageBox>

// Constructor
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_fileManager("data"),
      m_authManager(m_fileManager),
      m_fleetManager(m_fileManager),
      m_attendManager(m_fileManager),
      m_compManager(m_fileManager),
      m_monitorService(m_fileManager),
      m_networkGraph(Graph::createHaldwaniBhimtalNetwork()),
      m_currentUser(nullptr),
      m_updatingComboFromMap(false) {

    setWindowTitle("Smart College Bus Management System - Graphic Era Hill University, Bhimtal");
    resize(1280, 850);
    setMinimumSize(1100, 720);

    applyGitHubTheme();
    setupUI();

    // Start background bus assignment monitoring (every 10 seconds)
    m_monitorService.startMonitoring(10000);
}

void MainWindow::applyGitHubTheme() {
    QString qss = R"(
        QMainWindow, QWidget#centralWidget {
            background-color: #0D1117;
            color: #E6EDF3;
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", "Noto Sans", Helvetica, Arial, sans-serif;
            font-size: 13px;
        }
        QFrame, QGroupBox {
            background-color: #161B22;
            border: 1px solid #30363D;
            border-radius: 8px;
            color: #E6EDF3;
            font-weight: 600;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            padding: 2px 8px;
            color: #58A6FF;
            font-size: 12.5px;
            font-weight: 700;
        }
        QPushButton {
            background-color: #21262D;
            border: 1px solid #30363D;
            color: #C9D1D9;
            border-radius: 6px;
            padding: 6px 12px;
            font-weight: 600;
        }
        QPushButton:hover {
            background-color: #30363D;
            border-color: #8B949E;
            color: #FFFFFF;
        }
        QPushButton:pressed {
            background-color: #161B22;
        }
        QLineEdit, QComboBox {
            background-color: #0D1117;
            border: 1px solid #30363D;
            color: #E6EDF3;
            border-radius: 6px;
            padding: 6px 10px;
            selection-background-color: #1F6FEB;
        }
        QLineEdit:focus, QComboBox:focus {
            border-color: #58A6FF;
        }
        QComboBox::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: 20px;
            border-left: 1px solid #30363D;
        }
        QTableWidget {
            background-color: #0D1117;
            border: 1px solid #30363D;
            border-radius: 6px;
            gridline-color: #21262D;
            color: #E6EDF3;
            selection-background-color: #1F6FEB;
        }
        QHeaderView::section {
            background-color: #161B22;
            color: #8B949E;
            padding: 6px;
            border: 1px solid #30363D;
            font-weight: 700;
            font-size: 11px;
            text-transform: uppercase;
        }
        QTabWidget::pane {
            border: 1px solid #30363D;
            background-color: #0D1117;
            border-radius: 8px;
        }
        QTabBar::tab {
            background-color: #161B22;
            color: #8B949E;
            padding: 8px 16px;
            border: 1px solid #30363D;
            border-bottom: none;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
            margin-right: 2px;
            font-weight: 600;
        }
        QTabBar::tab:selected {
            background-color: #0D1117;
            color: #58A6FF;
            border-bottom: 2px solid #58A6FF;
        }
        QTabBar::tab:hover {
            color: #E6EDF3;
        }
    )";
    qApp->setStyleSheet(qss);
}

void MainWindow::setupUI() {
    m_stackedWidget = new QStackedWidget(this);
    setCentralWidget(m_stackedWidget);

    m_loginWidget = new LoginWidget(this);
    connect(m_loginWidget, &LoginWidget::loginRequested, this, &MainWindow::onLoginSuccess);

    m_portalWidget = createPortalScreen();

    m_stackedWidget->addWidget(m_loginWidget);  // Index 0: Login
    m_stackedWidget->addWidget(m_portalWidget); // Index 1: Main Application
    m_stackedWidget->setCurrentIndex(0);
}

QWidget* MainWindow::createPortalScreen() {
    QWidget* screen = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(screen);
    mainLayout->setContentsMargins(14, 12, 14, 14);
    mainLayout->setSpacing(10);

    // Top Header
    QFrame* headerCard = new QFrame(screen);
    headerCard->setStyleSheet("background-color: #161B22; border: 1px solid #30363D; border-radius: 8px; padding: 10px 16px;");
    QHBoxLayout* hl = new QHBoxLayout(headerCard);
    hl->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout* titleBox = new QVBoxLayout();
    titleBox->setSpacing(2);
    QLabel* brandTitle = new QLabel("Smart College Bus Management System", headerCard);
    brandTitle->setStyleSheet("font-size: 15px; font-weight: 800; color: #E6EDF3; letter-spacing: -0.2px;");
    QLabel* subTitle = new QLabel("Graphic Era Hill University, Bhimtal Campus - C++17 OOP Architecture", headerCard);
    subTitle->setStyleSheet("font-size: 11.5px; color: #58A6FF; font-weight: 600;");
    titleBox->addWidget(brandTitle);
    titleBox->addWidget(subTitle);

    m_userNameLabel = new QLabel("Logged In: Admin", headerCard);
    m_userNameLabel->setStyleSheet("font-size: 12px; font-weight: 600; color: #8B949E; margin-right: 6px;");

    m_userRoleBadge = new QLabel("Role: SUPER_ADMIN", headerCard);
    m_userRoleBadge->setStyleSheet("background-color: rgba(35, 134, 54, 0.2); color: #3FB950; border: 1px solid #238636; border-radius: 12px; padding: 3px 10px; font-size: 11.5px; font-weight: 700;");

    m_scheduleSelector = new QComboBox(headerCard);
    m_scheduleSelector->addItem("Full-Day Schedule (Normal Hours)");
    m_scheduleSelector->addItem("Half-Day Schedule (Exam Hours)");
    connect(m_scheduleSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onScheduleChanged);

    QPushButton* logoutBtn = new QPushButton("Logout", headerCard);
    logoutBtn->setStyleSheet("background-color: #21262D; border: 1px solid #30363D; color: #F85149; font-weight: 600; padding: 5px 12px;");
    connect(logoutBtn, &QPushButton::clicked, this, &MainWindow::onLogout);

    hl->addLayout(titleBox);
    hl->addStretch();
    hl->addWidget(m_userNameLabel);
    hl->addWidget(m_userRoleBadge);
    hl->addWidget(new QLabel("Mode:", headerCard));
    hl->addWidget(m_scheduleSelector);
    hl->addWidget(logoutBtn);

    mainLayout->addWidget(headerCard);

    // Tabbed Portals
    m_tabWidget = new QTabWidget(screen);

    m_adminDash = new AdminDashboardWidget(m_fleetManager, m_attendManager, m_compManager, m_authManager, m_monitorService, screen);
    m_studentDash = new StudentDashboardWidget(screen);
    m_driverDash = new DriverDashboardWidget(screen);
    m_parentDash = new ParentDashboardWidget(screen);
    m_routeTab = createRouteMapTab();
    m_complaintDesk = new ComplaintDeskWidget(m_compManager, screen);

    m_tabWidget->addTab(m_adminDash, "Admin Control Center");
    m_tabWidget->addTab(m_studentDash, "Student Portal");
    m_tabWidget->addTab(m_driverDash, "Driver Portal");
    m_tabWidget->addTab(m_parentDash, "Parent Portal");
    m_tabWidget->addTab(m_routeTab, "Route Tracking & Fleet Monitor");
    m_tabWidget->addTab(m_complaintDesk, "Fleet Complaints & Polls");

    mainLayout->addWidget(m_tabWidget, 1);
    return screen;
}

QWidget* MainWindow::createRouteMapTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);
    layout->setSpacing(10);

    // Top Controls Bar (Pure Route Selection & Fleet Tracking)
    QFrame* controlPanel = new QFrame(tab);
    controlPanel->setStyleSheet("background-color: #161B22; border: 1px solid #30363D; border-radius: 8px; padding: 10px 14px;");
    QHBoxLayout* ctrlLayout = new QHBoxLayout(controlPanel);
    ctrlLayout->setContentsMargins(0, 0, 0, 0);
    ctrlLayout->setSpacing(8);

    m_mapSourceCombo = new QComboBox(controlPanel);
    m_mapSourceCombo->addItem("ST101 - Haldwani Bus Terminal (Tikonia)", "ST101");
    m_mapSourceCombo->addItem("ST102 - Kathgodam Railway Station", "ST102");
    m_mapSourceCombo->addItem("ST103 - Ranibagh Junction", "ST103");
    m_mapSourceCombo->addItem("ST104 - Bhujiaghat Bypass", "ST104");
    m_mapSourceCombo->addItem("ST105 - Bhimtal Lake Mall Road", "ST105");
    connect(m_mapSourceCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onSourceStopSelected);

    m_mapDestInput = new QLineEdit("Graphic Era Hill University, Bhimtal Campus (FIXED)", controlPanel);
    m_mapDestInput->setReadOnly(true);
    m_mapDestInput->setStyleSheet("background-color: #1C2533; color: #58A6FF; font-weight: 700; border: 1px solid #30363D; min-width: 290px;");

    m_simToggleBtn = new QPushButton("Start Simulation", controlPanel);
    m_simToggleBtn->setStyleSheet("background-color: #238636; color: #FFFFFF; font-weight: 700; padding: 7px 12px;");
    connect(m_simToggleBtn, &QPushButton::clicked, this, &MainWindow::onToggleSimulation);

    m_simResetBtn = new QPushButton("Reset", controlPanel);
    m_simResetBtn->setStyleSheet("background-color: #21262D; border: 1px solid #30363D; color: #E6EDF3; font-weight: 600; padding: 7px 12px;");
    connect(m_simResetBtn, &QPushButton::clicked, this, &MainWindow::onResetSimulation);

    m_fitRouteBtn = new QPushButton("Fit Route", controlPanel);
    m_fitRouteBtn->setStyleSheet("background-color: #1F6FEB; color: #FFFFFF; font-weight: 600; padding: 7px 12px;");
    connect(m_fitRouteBtn, &QPushButton::clicked, this, [this]{ m_nativeMap->fitRouteBounds(); });

    m_zoomInBtn = new QPushButton("+", controlPanel);
    m_zoomInBtn->setFixedWidth(36);
    m_zoomInBtn->setStyleSheet("background-color: #21262D; border: 1px solid #30363D; color: #E6EDF3; font-weight: 700; padding: 7px;");
    connect(m_zoomInBtn, &QPushButton::clicked, this, [this]{ m_nativeMap->zoomIn(); });

    m_zoomOutBtn = new QPushButton("-", controlPanel);
    m_zoomOutBtn->setFixedWidth(36);
    m_zoomOutBtn->setStyleSheet("background-color: #21262D; border: 1px solid #30363D; color: #E6EDF3; font-weight: 700; padding: 7px;");
    connect(m_zoomOutBtn, &QPushButton::clicked, this, [this]{ m_nativeMap->zoomOut(); });

    ctrlLayout->addWidget(new QLabel("Origin Source:", controlPanel));
    ctrlLayout->addWidget(m_mapSourceCombo);
    ctrlLayout->addWidget(new QLabel("Campus Destination:", controlPanel));
    ctrlLayout->addWidget(m_mapDestInput);
    ctrlLayout->addStretch();
    ctrlLayout->addWidget(m_simToggleBtn);
    ctrlLayout->addWidget(m_simResetBtn);
    ctrlLayout->addWidget(m_fitRouteBtn);
    ctrlLayout->addWidget(m_zoomInBtn);
    ctrlLayout->addWidget(m_zoomOutBtn);

    layout->addWidget(controlPanel);

    // KPI Strip
    QFrame* kpiBar = new QFrame(tab);
    kpiBar->setStyleSheet("background-color: #1C2533; border: 1px solid #30363D; border-radius: 8px; padding: 8px 12px;");
    QHBoxLayout* kl = new QHBoxLayout(kpiBar);
    kl->setContentsMargins(4, 4, 4, 4);

    m_mapDistLabel = new QLabel("Total Road Distance: <b>28.4 KM</b>", kpiBar);
    m_mapDistLabel->setStyleSheet("color: #E6EDF3; font-size: 12px;");
    m_mapDurationLabel = new QLabel("Est. Travel Time: <b>52 Mins</b>", kpiBar);
    m_mapDurationLabel->setStyleSheet("color: #3FB950; font-size: 12px;");
    m_mapElevationLabel = new QLabel("Elevation Traversal: <b>+996m Ascent (424m -> 1420m ASL)</b>", kpiBar);
    m_mapElevationLabel->setStyleSheet("color: #D29922; font-size: 12px;");
    m_mapStopsLabel = new QLabel("Intermediate Stops: <b>5 Checkpoints</b>", kpiBar);
    m_mapStopsLabel->setStyleSheet("color: #8B949E; font-size: 12px;");
    m_mapStatusLabel = new QLabel("Status: <font color='#3FB950'><b>ROUTE ACTIVE & READY</b></font>", kpiBar);
    m_mapStatusLabel->setStyleSheet("font-size: 12px;");

    kl->addWidget(m_mapDistLabel);
    kl->addWidget(m_mapDurationLabel);
    kl->addWidget(m_mapElevationLabel);
    kl->addWidget(m_mapStopsLabel);
    kl->addWidget(m_mapStatusLabel);
    layout->addWidget(kpiBar);

    // Native Embedded Qt Map (Runs 100% inside the Qt application window)
    m_nativeMap = new NativeOsmMapWidget(tab);
    connect(m_nativeMap, &NativeOsmMapWidget::routeCalculated, this, [this](double dist, int dur, const QString& status){
        m_mapDistLabel->setText(QString("Total Road Distance: <b>%1 KM</b>").arg(dist, 0, 'f', 1));
        m_mapDurationLabel->setText(QString("Est. Travel Time: <b>%1 Mins</b>").arg(dur));
        m_mapStatusLabel->setText("Status: <font color='#3FB950'><b>" + status + "</b></font>");
    });
    connect(m_nativeMap, &NativeOsmMapWidget::busProgress, this, [this](int pct, const QString& text){
        (void)pct;
        m_mapStatusLabel->setText(QString("Live Telemetry: <font color='#58A6FF'><b>%1</b></font>").arg(text));
    });

    // Method B: Direct Map Clicking Connection
    connect(m_nativeMap, &NativeOsmMapWidget::sourceStopClicked, this, &MainWindow::onMapSourceMarkerClicked);
    connect(m_nativeMap, &NativeOsmMapWidget::destinationClicked, this, &MainWindow::onMapDestinationClicked);
    connect(m_nativeMap, &NativeOsmMapWidget::invalidAreaClicked, this, &MainWindow::onMapInvalidAreaClicked);

    layout->addWidget(m_nativeMap, 1);

    // Trigger initial route calculation for default source (Haldwani)
    updateSelectedRoute();

    return tab;
}

void MainWindow::onSourceStopSelected(int index) {
    (void)index;
    if (m_updatingComboFromMap) return;
    updateSelectedRoute();
}

void MainWindow::onMapSourceMarkerClicked(const QString& stopId, const QString& stopName, double lat, double lon) {
    (void)lat;
    (void)lon;

    // Method B: Admin clicked directly on a registered bus stop marker on the map
    m_updatingComboFromMap = true;
    for (int i = 0; i < m_mapSourceCombo->count(); ++i) {
        if (m_mapSourceCombo->itemData(i).toString() == stopId) {
            m_mapSourceCombo->setCurrentIndex(i);
            break;
        }
    }
    m_updatingComboFromMap = false;

    updateSelectedRoute();
    m_mapStatusLabel->setText(QString("Origin selected via Map Click: <font color='#3FB950'><b>%1</b></font>").arg(stopName));
}

void MainWindow::onMapDestinationClicked() {
    m_mapStatusLabel->setText("Notice: <font color='#F85149'><b>Graphic Era Hill University is the fixed campus destination and cannot be selected as origin.</b></font>");
}

void MainWindow::onMapInvalidAreaClicked(double lat, double lon) {
    (void)lat;
    (void)lon;
    m_mapStatusLabel->setText("Notice: <font color='#D29922'><b>Please click directly on a registered bus stop marker (Haldwani, Kathgodam, Ranibagh, Bhujiaghat, Bhimtal) to select your route origin.</b></font>");
}

void MainWindow::updateSelectedRoute() {
    QString srcId = m_mapSourceCombo->currentData().toString();
    double startLat = 29.2183, startLon = 79.5130;
    QString stopName = "Haldwani Bus Terminal (Tikonia)";
    int ascentM = 996;
    int intermediateCount = 5;

    if (srcId == "ST102") {
        startLat = 29.2715; startLon = 79.5312;
        stopName = "Kathgodam Railway Station";
        ascentM = 866;
        intermediateCount = 4;
    } else if (srcId == "ST103") {
        startLat = 29.2942; startLon = 79.5445;
        stopName = "Ranibagh Junction";
        ascentM = 800;
        intermediateCount = 3;
    } else if (srcId == "ST104") {
        startLat = 29.3210; startLon = 79.5510;
        stopName = "Bhujiaghat Bypass";
        ascentM = 510;
        intermediateCount = 2;
    } else if (srcId == "ST105") {
        startLat = 29.3458; startLon = 79.5583;
        stopName = "Bhimtal Lake Mall Road";
        ascentM = 50;
        intermediateCount = 1;
    }

    // Fixed Destination: Graphic Era Hill University Bhimtal Campus
    double destLat = 29.3565, destLon = 79.5620;

    m_nativeMap->clearMarkers();

    // All registered network stops along Highway NH109
    m_nativeMap->addMarker("ST101", 29.2183, 79.5130, "Haldwani Bus Terminal", "424m ASL", QColor("#1F6FEB"), "S", (srcId == "ST101"), false);
    m_nativeMap->addMarker("ST102", 29.2715, 79.5312, "Kathgodam Station", "554m ASL", QColor("#1F6FEB"), "S", (srcId == "ST102"), false);
    m_nativeMap->addMarker("ST103", 29.2942, 79.5445, "Ranibagh Junction", "620m ASL", QColor("#1F6FEB"), "S", (srcId == "ST103"), false);
    m_nativeMap->addMarker("ST104", 29.3210, 79.5510, "Bhujiaghat Bypass", "910m ASL", QColor("#1F6FEB"), "S", (srcId == "ST104"), false);
    m_nativeMap->addMarker("ST105", 29.3458, 79.5583, "Bhimtal Lake Mall Rd", "1,370m ASL", QColor("#1F6FEB"), "S", (srcId == "ST105"), false);

    // Fixed Permanent Destination: Graphic Era Hill University
    m_nativeMap->addMarker("ST106", destLat, destLon, "Graphic Era Campus", "1,420m ASL (Fixed)", QColor("#DA3633"), "GEHU", false, true);

    // Update KPI metrics
    m_mapElevationLabel->setText(QString("Elevation Traversal: <b>+%1m Ascent</b>").arg(ascentM));
    m_mapStopsLabel->setText(QString("Intermediate Stops: <b>%1 Checkpoints</b>").arg(intermediateCount));

    // Reset simulation when route changes
    onResetSimulation();

    // Fetch and draw real road driving geometry from OSRM
    m_nativeMap->fetchOsrmRoute(startLat, startLon, destLat, destLon);
}

void MainWindow::onToggleSimulation() {
    if (m_nativeMap->isSimulating()) {
        m_nativeMap->pauseBusSimulation();
        m_simToggleBtn->setText("Resume Simulation");
        m_simToggleBtn->setStyleSheet("background-color: #238636; color: #FFFFFF; font-weight: 700; padding: 7px 12px;");
    } else {
        m_nativeMap->startBusSimulation();
        m_simToggleBtn->setText("Pause Simulation");
        m_simToggleBtn->setStyleSheet("background-color: #D29922; color: #0D1117; font-weight: 700; padding: 7px 12px;");
    }
}

void MainWindow::onResetSimulation() {
    m_nativeMap->resetBusSimulation();
    m_simToggleBtn->setText("Start Simulation");
    m_simToggleBtn->setStyleSheet("background-color: #238636; color: #FFFFFF; font-weight: 700; padding: 7px 12px;");
    m_mapStatusLabel->setText("Status: <font color='#3FB950'><b>ROUTE ACTIVE & READY</b></font>");
}

void MainWindow::configureTabsForRole(UserRole role) {
    m_tabWidget->clear();

    if (role == UserRole::ADMIN) {
        m_tabWidget->addTab(m_adminDash, "Admin Control Center");
        m_tabWidget->addTab(m_studentDash, "Student Portal");
        m_tabWidget->addTab(m_driverDash, "Driver Portal");
        m_tabWidget->addTab(m_parentDash, "Parent Portal");
        m_tabWidget->addTab(m_routeTab, "Route Tracking & Fleet Monitor");
        m_tabWidget->addTab(m_complaintDesk, "Fleet Complaints & Polls");
    } else if (role == UserRole::STUDENT) {
        m_tabWidget->addTab(m_studentDash, "Student Portal");
        m_tabWidget->addTab(m_routeTab, "Route Tracking & Fleet Monitor");
        m_tabWidget->addTab(m_complaintDesk, "Fleet Complaints & Polls");
    } else if (role == UserRole::DRIVER) {
        m_tabWidget->addTab(m_driverDash, "Driver Portal");
        m_tabWidget->addTab(m_routeTab, "Route Tracking & Fleet Monitor");
        m_tabWidget->addTab(m_complaintDesk, "Fleet Complaints & Polls");
    } else if (role == UserRole::PARENT) {
        m_tabWidget->addTab(m_parentDash, "Parent Portal");
        m_tabWidget->addTab(m_routeTab, "Route Tracking & Fleet Monitor");
        m_tabWidget->addTab(m_complaintDesk, "Fleet Complaints & Polls");
    }
}

void MainWindow::onLoginSuccess(const QString& username, const QString& password, UserRole role) {
    auto user = m_authManager.authenticate(username.toStdString(), password.toStdString(), role);
    if (!user) {
        m_loginWidget->setErrorMessage("Invalid credentials. Please verify your role and password.");
        return;
    }

    m_currentUser = user;
    m_userNameLabel->setText("User: " + QString::fromStdString(m_currentUser->getFullName()));
    m_userRoleBadge->setText("Role: " + QString::fromStdString(m_currentUser->getRoleString()));

    configureTabsForRole(m_currentUser->getRole());

    // Pass credentials to child tabs
    m_complaintDesk->setActiveUser(m_currentUser->getId(), m_currentUser->getRole());
    if (m_currentUser->getRole() == UserRole::STUDENT) {
        m_studentDash->setStudentRoll(QString::fromStdString(m_currentUser->getId()));
    }

    m_stackedWidget->setCurrentIndex(1); // Show Portal Screen
}

void MainWindow::onLogout() {
    m_authManager.logout();
    m_currentUser = nullptr;
    m_loginWidget->clearFields();
    m_stackedWidget->setCurrentIndex(0); // Return to Login Screen
}

void MainWindow::onScheduleChanged(int index) {
    (void)index;
    QMessageBox::information(this, "Schedule Synchronized", "Campus transportation network updated to: " + m_scheduleSelector->currentText());
}
