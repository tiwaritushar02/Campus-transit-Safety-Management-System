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

using namespace std;

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    SystemManager sysManager;

    // Stacked View Navigation
    QStackedWidget* stackedWidget;
    QWidget* loginWidget;
    QWidget* portalWidget;

    // Login Form Widgets
    QComboBox* loginRoleCombo;
    QLineEdit* loginUserEdit;
    QLineEdit* loginPassEdit;
    QLabel* loginStatusLabel;

    // Logged In Banner
    QLabel* loggedUserLabel;
    QLabel* loggedRoleBadge;
    QComboBox* scheduleComboBox;
    QTabWidget* tabWidget;

    // Tab Widgets for Role Filtering
    QWidget* studentTabWidget;
    QWidget* driverTabWidget;
    QWidget* adminTabWidget;
    QWidget* parentTabWidget;
    QWidget* routeVisualizerTabWidget;
    QWidget* fleetAndComplaintsTabWidget;

    // Tab 1: Student Portal
    QLineEdit* studentRollInput;
    QLabel* studentInfoLabel;
    QTableWidget* scheduleTable;
    QLineEdit* ratingDriverIdInput;
    QComboBox* ratingStarCombo;
    QLineEdit* ratingFeedbackInput;

    // Tab 2: Driver Portal
    QComboBox* driverBusCombo;
    QLineEdit* driverDelayReasonInput;
    QLineEdit* driverDelayMinsInput;
    QLineEdit* misbehaviorRollInput;
    QLineEdit* misbehaviorDescInput;

    // Tab 3: Admin Control Panel
    QComboBox* adminBusCombo;
    QLineEdit* adminRegInput;
    QLineEdit* adminCapInput;
    QComboBox* adminRouteCombo;
    QComboBox* adminTimingStopCombo;
    QComboBox* adminTimingBusCombo;
    QComboBox* adminTimingScheduleCombo;
    QLineEdit* adminNewTimeInput;

    // Tab 4: Parent Portal
    QLineEdit* parentPhoneInput;
    QLabel* parentInfoLabel;

    // Tab 5: Route Optimizer & Visualizer
    QGraphicsView* graphView;
    QGraphicsScene* graphScene;
    QComboBox* startStopCombo;
    QComboBox* destStopCombo;
    QLabel* routeInfoLabel;
    QTimer* simTimer;
    int simStep;
    vector<string> currentSimStops;
    QGraphicsEllipseItem* busMarker;

    // Fleet Table
    QTableWidget* fleetTable;
    QLineEdit* boardRollInput;
    QComboBox* boardBusCombo;

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
    void onTriggerStudentSOS();

    void onDriverStartTrip();
    void onDriverEndTrip();
    void onDriverReportDelay();
    void onDriverReportMisbehavior();

    void onAdminUpdateBus();
    void onAdminUpdateTiming();
    void onAdminBroadcastNotice();

    void onParentTrackChild();

    void onOptimizeRoute();
    void onStartLiveSim();
    void onSimStep();
    void onBoardStudent();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;
};

#endif // MAIN_WINDOW_HPP
