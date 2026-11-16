#ifndef DRIVER_DASHBOARD_WIDGET_HPP
#define DRIVER_DASHBOARD_WIDGET_HPP

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>

// Inheritance
class DriverDashboardWidget : public QWidget {
    Q_OBJECT

private:
    QComboBox* m_busCombo;
    QLineEdit* m_delayReasonInput;
    QLineEdit* m_delayMinsInput;
    QLineEdit* m_misbehaviorRollInput;
    QLineEdit* m_misbehaviorDescInput;

    void setupUI();

public:
    // Constructor
    explicit DriverDashboardWidget(QWidget* parent = nullptr);

private slots:
    void onStartTrip();
    void onEndTrip();
    void onReportDelay();
    void onReportMisbehavior();
};

#endif // DRIVER_DASHBOARD_WIDGET_HPP
