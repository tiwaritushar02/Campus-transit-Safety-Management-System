#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

/**
 * ============================================================================
 * QT GUI MODULE: Smart College Bus Management System (Master Redesign)
 * FILE: MainWindow.hpp
 * 
 * DESIGN FEATURES:
 * - GitHub-Inspired Professional Dark Theme (#0D1117, #161B22, #1C2533, #30363D)
 * - Glass/Dark Login Screen with College Bus & Campus Background Overlay
 * - Advanced Complaint Management with Reporter/Accused Tracking & Student YES/NO Polls
 * - Real Haldwani to Graphic Era Hill University (Bhimtal Campus) Route Optimizer
 * - Fixed Destination (GEHU Bhimtal Campus) with A* and Dijkstra Algorithms
 * - Native Embedded OpenStreetMap Widget (NO BROWSER, NO WEBSITE, NO POPUP)
 * - Multi-Role Authorization: Admin, Student, Driver, Parent Portals
 * ============================================================================
 */

#include <QMainWindow>
#include <QStackedWidget>
#include <QTabWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QProgressBar>
#include <QListWidget>
#include <QTimer>
#include <QGroupBox>
#include "SystemManager.hpp"
#include "NativeOsmMapWidget.hpp"
#include "RouteController.hpp"

using namespace std;

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    SystemManager sysManager;
    string activeUsername;
    UserRole activeRole;

    // Stacked Navigation (Index 0 = Login Screen, Index 1 = Authenticated Portal)
    QStackedWidget* stackedWidget;
    QWidget* loginWidget;
    QWidget* portalWidget;

    // Login Form Widgets
    QComboBox* loginRoleCombo;
    QLineEdit* loginUserEdit;
    QLineEdit* loginPassEdit;
    QPushButton* showPassBtn;
    QLabel* loginStatusLabel;
    bool isPasswordVisible;

    // Authenticated Header Bar
    QLabel* loggedUserLabel;
    QLabel* loggedRoleBadge;
    QComboBox* scheduleComboBox;
    QTabWidget* tabWidget;

    // Portals
    QWidget* adminTabWidget;
    QWidget* studentTabWidget;
    QWidget* driverTabWidget;
    QWidget* parentTabWidget;
    QWidget* routeVisualizerTabWidget;
    QWidget* complaintDeskTabWidget;

    // Admin Dashboard Summary Metric KPI Labels
    QLabel* kpiTotalBusesLabel;
    QLabel* kpiTotalSeatsLabel;
    QLabel* kpiCorridorsLabel;
    QLabel* kpiScheduleLabel;
    QLabel* kpiAttendanceLabel;
    QLabel* kpiComplaintsLabel;

    // Admin Bus Fleet Configuration
    QComboBox* adminBusCombo;
    QLineEdit* adminRegInput;
    QLineEdit* adminCapInput;
    QComboBox* adminRouteCombo;

    // Admin Stop Timings & Broadcast
    QComboBox* adminTimingStopCombo;
    QComboBox* adminTimingBusCombo;
    QComboBox* adminTimingScheduleCombo;
    QLineEdit* adminNewTimeInput;

    // Admin Student Transport Directory
    QTableWidget* studentDirTable;
    QPushButton* registerStudentBtn;

    // Admin Attendance & Boarding Management
    QLineEdit* boardRollInput;
    QComboBox* boardBusCombo;
    QTableWidget* attendanceTable;

    // Student Portal
    QLineEdit* studentRollInput;
    QLabel* studentInfoLabel;
    QTableWidget* studentScheduleTable;
    QLineEdit* ratingDriverIdInput;
    QComboBox* ratingStarCombo;
    QLineEdit* ratingFeedbackInput;

    // Driver Portal
    QComboBox* driverBusCombo;
    QLineEdit* driverDelayReasonInput;
    QLineEdit* driverDelayMinsInput;
    QLineEdit* misbehaviorRollInput;
    QLineEdit* misbehaviorDescInput;

    // Parent Portal
    QLineEdit* parentPhoneInput;
    QLabel* parentInfoLabel;

    // Complaint Desk & Student Disciplinary Poll
    QComboBox* filterCategoryCombo;
    QComboBox* filterReporterRoleCombo;
    QComboBox* filterAccusedRoleCombo;
    QComboBox* filterStatusCombo;
    QTableWidget* complaintTable;

    QLabel* complaintDetailHeader;
    QLabel* complaintDetailBody;
    QGroupBox* pollGroupBox;
    QLabel* pollQuestionLabel;
    QLabel* pollStatsLabel;
    QProgressBar* pollProgressBar;
    QPushButton* voteYesBtn;
    QPushButton* voteNoBtn;
    QLabel* pollMessageLabel;
    string selectedComplaintId;

    // Admin Complaint Action Buttons
    QPushButton* adminInvestigateBtn;
    QPushButton* adminResolveBtn;
    QPushButton* adminRejectBtn;

    // Submit Complaint Form
    QComboBox* newCompCategoryCombo;
    QLineEdit* newCompAccusedNameEdit;
    QComboBox* newCompAccusedRoleCombo;
    QLineEdit* newCompBusEdit;
    QLineEdit* newCompDescEdit;

    // ========================================================================
    // NATIVE TRACKING DASHBOARD & ROUTE OPTIMIZER
    // ========================================================================
    RouteController* routeController;

    // Left Panel
    QComboBox* startStopCombo;
    QComboBox* destStopCombo; // FIXED: Graphic Era Hill University, Bhimtal Campus
    QComboBox* algoCombo;     // Dijkstra vs A*
    QComboBox* busSelectCombo;
    QListWidget* activeBusesList;
    QPushButton* startSimBtn;
    QPushButton* pauseSimBtn;
    QPushButton* resetSimBtn;
    QComboBox* simSpeedCombo;

    // Center Map
    NativeOsmMapWidget* osmMapWidget;

    // Top / Status HUD
    QLabel* routeKpiDistance;
    QLabel* routeKpiDuration;
    QLabel* routeKpiElevation;
    QLabel* routeKpiStops;
    QLabel* routeKpiAlgo;
    QLabel* routeKpiStatus;
    QLabel* routeInfoLabel;

    // Right Panel (Telemetry & Bus Info)
    QLabel* rBusIdLabel;
    QLabel* rBusRegLabel;
    QLabel* rBusCapLabel;
    QLabel* rDriverNameLabel;
    QLabel* rDriverPhoneLabel;
    QLabel* rCurrentLocLabel;
    QLabel* rAltitudeLabel;
    QLabel* rCurrentStopLabel;
    QLabel* rNextStopLabel;
    QLabel* rEtaLabel;
    QLabel* rTripStatusLabel;
    QProgressBar* tripProgressBar;

    // Bottom Panel (Schedules, History, Delay info)
    QTableWidget* trackingScheduleTable;
    QTableWidget* trackingHistoryTable;
    QLabel* corridorDelayLabel;

    // Initialization Helpers
    void setupUI();
    void applyGitHubTheme();
    QWidget* createLoginScreen();
    QWidget* createPortalScreen();

    QWidget* createAdminTab();
    QWidget* createStudentTab();
    QWidget* createDriverTab();
    QWidget* createParentTab();
    QWidget* createRouteVisualizerTab();
    QWidget* createComplaintDeskTab();

    void setupTabsForRole(UserRole role, const string& username);
    void refreshAllDataViews();
    void refreshFleetMetrics();
    void refreshStudentDirectory();
    void refreshAttendanceTable();
    void refreshComplaintTable();
    void displayComplaintDetails(const string& complaintId);
    void refreshTrackingTables();
    void renderRouteGraph();

private slots:
    // Authentication Slots
    void onPerformLogin();
    void onPerformLogout();
    void onTogglePasswordVisibility();
    void onQuickDemoLogin();

    // Portal Global Slots
    void onScheduleChanged(int index);

    // Admin Slots
    void onAdminUpdateBus();
    void onAdminUpdateTiming();
    void onAdminBroadcastNotice();
    void onAdminRegisterStudentDialog();
    void onBoardStudent();
    void onVerifyQRPass();

    // Student Slots
    void onSearchStudentSchedule();
    void onShowDigitalQRPass();
    void onSubmitDriverRating();
    void onTriggerStudentSOS();

    // Driver Slots
    void onDriverStartTrip();
    void onDriverEndTrip();
    void onDriverReportDelay();
    void onDriverReportMisbehavior();

    // Parent Slots
    void onParentTrackChild();

    // Complaint & Student Poll Slots
    void onComplaintTableSelectionChanged();
    void onApplyComplaintFilters();
    void onCastStudentVote(bool voteYes);
    void onAdminInvestigateComplaint();
    void onAdminResolveComplaint();
    void onAdminRejectComplaint();
    void onSubmitNewComplaint();

    // Route Optimizer & Tracking Slots (Native embedded map)
    void onCalculateOptimalRoute();
    void onStartLiveSim();
    void onPauseLiveSim();
    void onResetLiveSim();
    void onSimSpeedChanged(int index);
    void onSourceStopChanged(int index);
    void onBusSelected(int index);
    void onSimulationTelemetry(const QString& busId, const GPSLocation& loc, 
                               int progress, const QString& curStop, 
                               const QString& nextStop, int etaMins, double remainingKm);
    void onSimulationCheckpoint(const QString& busId, const QString& stopName, const QString& arrivalTime);
    void onSimulationCompleted(const QString& busId);

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;
};

#endif // MAIN_WINDOW_HPP
