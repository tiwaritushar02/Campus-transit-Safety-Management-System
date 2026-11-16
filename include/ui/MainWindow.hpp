#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QMainWindow>
#include <QStackedWidget>
#include <QTabWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>

#include "FileManager.hpp"
#include "AuthManager.hpp"
#include "FleetManager.hpp"
#include "AttendanceManager.hpp"
#include "ComplaintManager.hpp"
#include "BusAssignmentMonitorService.hpp"
#include "Graph.hpp"

#include "LoginWidget.hpp"
#include "AdminDashboardWidget.hpp"
#include "StudentDashboardWidget.hpp"
#include "DriverDashboardWidget.hpp"
#include "ParentDashboardWidget.hpp"
#include "ComplaintDeskWidget.hpp"
#include "NativeOsmMapWidget.hpp"

// Inheritance
class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    // Composition
    FileManager m_fileManager;
    AuthManager m_authManager;
    FleetManager m_fleetManager;
    AttendanceManager m_attendManager;
    ComplaintManager m_compManager;
    BusAssignmentMonitorService m_monitorService;
    Graph m_networkGraph;

    std::shared_ptr<User> m_currentUser;

    QStackedWidget* m_stackedWidget;
    LoginWidget* m_loginWidget;
    QWidget* m_portalWidget;

    // Header elements
    QLabel* m_userRoleBadge;
    QLabel* m_userNameLabel;
    QComboBox* m_scheduleSelector;

    // Tabs
    QTabWidget* m_tabWidget;
    AdminDashboardWidget* m_adminDash;
    StudentDashboardWidget* m_studentDash;
    DriverDashboardWidget* m_driverDash;
    ParentDashboardWidget* m_parentDash;
    ComplaintDeskWidget* m_complaintDesk;
    QWidget* m_routeTab;

    // Route Map Controls
    QComboBox* m_mapSourceCombo;
    QLineEdit* m_mapDestInput;
    QPushButton* m_simToggleBtn;
    QPushButton* m_simResetBtn;
    QPushButton* m_fitRouteBtn;
    QPushButton* m_zoomInBtn;
    QPushButton* m_zoomOutBtn;

    QLabel* m_mapDistLabel;
    QLabel* m_mapDurationLabel;
    QLabel* m_mapElevationLabel;
    QLabel* m_mapStopsLabel;
    QLabel* m_mapStatusLabel;
    NativeOsmMapWidget* m_nativeMap;

    bool m_updatingComboFromMap;

    void setupUI();
    void applyGitHubTheme();
    QWidget* createPortalScreen();
    QWidget* createRouteMapTab();
    void configureTabsForRole(UserRole role);
    void updateSelectedRoute();

public:
    // Constructor
    explicit MainWindow(QWidget *parent = nullptr);
    // Destructor
    ~MainWindow() override = default;

private slots:
    void onLoginSuccess(const QString& username, const QString& password, UserRole role);
    void onLogout();
    void onScheduleChanged(int index);
    void onSourceStopSelected(int index);
    void onMapSourceMarkerClicked(const QString& stopId, const QString& stopName, double lat, double lon);
    void onMapDestinationClicked();
    void onMapInvalidAreaClicked(double lat, double lon);
    void onToggleSimulation();
    void onResetSimulation();
};

#endif // MAIN_WINDOW_HPP
