#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

/**
 * ============================================================================
 * QT GUI MODULE: Extended Desktop Application Interface (Role-Based Visibility)
 * FILE: MainWindow.hpp
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
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QTimer>
#include "SystemManager.hpp"
#include "RouteOptimizer.hpp"

using namespace std;

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    SystemManager sysManager;

    // Stacked View Navigation
    QStackedWidget* stackedWidget = nullptr;
    QWidget* loginWidget = nullptr;
    QWidget* portalWidget = nullptr;

    // Login Form Widgets
    QComboBox* loginRoleCombo = nullptr;
    QLineEdit* loginUserEdit = nullptr;
    QLineEdit* loginPassEdit = nullptr;
    QLabel* loginStatusLabel = nullptr;

    // Logged In Banner
    QLabel* loggedUserLabel = nullptr;
    QLabel* loggedRoleBadge = nullptr;
    QComboBox* scheduleComboBox = nullptr;
    QTabWidget* tabWidget = nullptr;

    // Tab Widgets for Role Filtering
    QWidget* studentTabWidget = nullptr;
    QWidget* driverTabWidget = nullptr;
    QWidget* adminTabWidget = nullptr;
    QWidget* parentTabWidget = nullptr;
    QWidget* routeVisualizerTabWidget = nullptr;
    QWidget* fleetAndComplaintsTabWidget = nullptr;

    // Tab 1: Student Portal
    QLineEdit* studentRollInput = nullptr;
    QLabel* studentInfoLabel = nullptr;
    QTableWidget* scheduleTable = nullptr;
    QLineEdit* ratingDriverIdInput = nullptr;
    QComboBox* ratingStarCombo = nullptr;
    QLineEdit* ratingFeedbackInput = nullptr;

    // Tab 2: Driver Portal
    QComboBox* driverBusCombo = nullptr;
    QLineEdit* driverDelayReasonInput = nullptr;
    QLineEdit* driverDelayMinsInput = nullptr;
    QLineEdit* misbehaviorRollInput = nullptr;
    QLineEdit* misbehaviorDescInput = nullptr;

    // Tab 3: Admin Control Panel
    QComboBox* adminBusCombo = nullptr;
    QLineEdit* adminRegInput = nullptr;
    QLineEdit* adminCapInput = nullptr;
    QComboBox* adminRouteCombo = nullptr;
    QComboBox* adminTimingStopCombo = nullptr;
    QComboBox* adminTimingBusCombo = nullptr;
    QComboBox* adminTimingScheduleCombo = nullptr;
    QLineEdit* adminNewTimeInput = nullptr;

    // Admin Attendance Module
    QComboBox* adminBoardRollCombo = nullptr;
    QComboBox* adminBoardBusCombo = nullptr;
    QTableWidget* adminAttendanceTable = nullptr;
    QLabel* adminAttendanceStatusLabel = nullptr;
    QLabel* statFleetCountLabel = nullptr;
    QLabel* statActiveRoutesLabel = nullptr;
    QLabel* statAttendanceCountLabel = nullptr;

    // Admin Student Registration & Directory
    QLineEdit* adminNewStudentNameInput = nullptr;
    QLineEdit* adminNewStudentRollInput = nullptr;
    QLineEdit* adminNewStudentPhoneInput = nullptr;
    QComboBox* adminNewStudentStopCombo = nullptr;
    QComboBox* adminNewStudentBusCombo = nullptr;
    QComboBox* adminNewStudentFeeCombo = nullptr;
    QComboBox* adminStudentLookupCombo = nullptr;
    QLabel* adminStudentLookupInfoLabel = nullptr;

    // Complaints Module
    QTableWidget* complaintTable = nullptr;
    QLabel* studentDelayBannerLabel = nullptr;

    // Tab 4: Parent Portal
    QLineEdit* parentPhoneInput = nullptr;
    QLabel* parentInfoLabel = nullptr;

    // Tab 5: Route Optimizer & Visualizer
    QGraphicsView* graphView = nullptr;
    QGraphicsScene* graphScene = nullptr;
    QComboBox* startStopCombo = nullptr;
    QComboBox* destStopCombo = nullptr;
    QComboBox* algorithmCombo = nullptr;
    QLabel* routeInfoLabel = nullptr;
    QTimer* simTimer = nullptr;
    int simStep = 0;
    QTimer* visTimer = nullptr;
    int visStep = 0;
    AlgorithmMetrics currentMetrics;
    vector<QGraphicsItem*> searchVisItems;
    vector<string> currentSimStops;
    QGraphicsEllipseItem* busMarker = nullptr;
    QGraphicsPathItem* optimalRoutePathItem = nullptr;
    QGraphicsPathItem* optimalRouteGlowItem = nullptr;
    QGraphicsEllipseItem* sourceHighlightMarker = nullptr;
    vector<pair<int, int>> activeSimWaypoints;

    // Fleet Table
    QTableWidget* fleetTable = nullptr;
    QLineEdit* boardRollInput = nullptr;
    QComboBox* boardBusCombo = nullptr;

    // GUI Initialization Helpers
    void setupUI();
    QWidget* createLoginScreen();
    QWidget* createPortalScreen();

    QWidget* createStudentTab();
    QWidget* createDriverTab();
    QWidget* createAdminTab();
    QWidget* createParentTab();
    QWidget* createRouteVisualizerTab();
    QWidget* createFleetAndComplaintsTab();
    
    void setupTabsForRole(UserRole role, const string& username);
    void renderRouteGraph();
    void refreshFleetTable();
    void refreshAttendanceTable();
    void refreshComplaintTable();
    void checkAndShowDelayNotice();
    void fitCorridorView();

private slots:
    // Authentication Slots
    void onPerformLogin();
    void onPerformLogout();

    // Portal Slots
    void onScheduleChanged(int index);
    void onSearchStudentSchedule();
    void onFindConnectingBuses();
    void onShowDigitalQRPass();
    void onSubmitDriverRating();
    void onStudentReportDriver();
    void onTriggerStudentSOS();

    void onDriverStartTrip();
    void onDriverEndTrip();
    void onDriverReportDelay();
    void onDriverReportMisbehavior();

    void onAdminUpdateBus();
    void onAdminUpdateTiming();
    void onAdminBroadcastNotice();
    void onAdminMarkAttendance();
    void onAdminScanQRAttendance();
    void onAdminRegisterStudent();
    void onAdminLookupStudentChanged(int idx);
    void onAdminResolveComplaint();

    void onParentTrackChild();

    void onOptimizeRoute();
    void onVisualizeAlgorithmSearch();
    void onVisStep();
    void onStartLiveSim();
    void onSimStep();
    void onBoardStudent();
    void onZoomIn();
    void onZoomOut();
    void onFitCorridor();
    void onOpenWebMap();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;
};

#endif // MAIN_WINDOW_HPP
