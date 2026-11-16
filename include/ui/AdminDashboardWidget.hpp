#ifndef ADMIN_DASHBOARD_WIDGET_HPP
#define ADMIN_DASHBOARD_WIDGET_HPP

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include "FleetManager.hpp"
#include "AttendanceManager.hpp"
#include "ComplaintManager.hpp"
#include "AuthManager.hpp"
#include "BusAssignmentMonitorService.hpp"

// Inheritance
class AdminDashboardWidget : public QWidget {
    Q_OBJECT

private:
    // Composition
    FleetManager& m_fleetManager;
    AttendanceManager& m_attendManager;
    ComplaintManager& m_compManager;
    AuthManager& m_authManager;
    BusAssignmentMonitorService& m_monitorService;

    // Unassigned Alert Banner
    QFrame* m_unassignedBanner;
    QLabel* m_unassignedAlertText;
    QPushButton* m_unassignedActionBtn;

    // KPI Labels
    QLabel* m_kpiBuses;
    QLabel* m_kpiSeats;
    QLabel* m_kpiCorridors;
    QLabel* m_kpiSchedule;
    QLabel* m_kpiAttendance;
    QLabel* m_kpiComplaints;

    // Section A
    QComboBox* m_busCombo;
    QLineEdit* m_regInput;
    QLineEdit* m_capInput;
    QComboBox* m_routeCombo;

    // Section B
    QComboBox* m_timingStopCombo;
    QComboBox* m_timingBusCombo;
    QComboBox* m_timingScheduleCombo;
    QLineEdit* m_newTimeInput;

    // Section C
    QTableWidget* m_studentTable;

    // Section D
    QLineEdit* m_boardRollInput;
    QComboBox* m_boardBusCombo;
    QTableWidget* m_attendanceTable;

    // Section E: Scheduled Trips & Bus Assignment Engine
    QTableWidget* m_tripsTable;

    void setupUI();
    void updateKpis();
    void refreshTripsTable();

public:
    // Constructor
    AdminDashboardWidget(FleetManager& fm, AttendanceManager& am, ComplaintManager& cm, AuthManager& aum, BusAssignmentMonitorService& bams, QWidget* parent = nullptr);

    void refreshData();

private slots:
    void onSaveBusConfig();
    void onUpdateTiming();
    void onBroadcastNotice();
    void onRegisterStudentDialog();
    void onBoardStudent();
    void onVerifyQRPass();
    void onCreateTripDialog();
    void onAssignBusDialog(const QString& tripId);
    void checkUnassignedTripsBanner();
};

#endif // ADMIN_DASHBOARD_WIDGET_HPP
