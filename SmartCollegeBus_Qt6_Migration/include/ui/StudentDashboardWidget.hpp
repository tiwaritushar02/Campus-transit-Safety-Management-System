#ifndef STUDENT_DASHBOARD_WIDGET_HPP
#define STUDENT_DASHBOARD_WIDGET_HPP

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>

// Inheritance
class StudentDashboardWidget : public QWidget {
    Q_OBJECT

private:
    QLineEdit* m_rollInput;
    QLabel* m_infoLabel;
    QTableWidget* m_scheduleTable;
    QLineEdit* m_driverIdInput;
    QComboBox* m_starCombo;
    QLineEdit* m_feedbackInput;

    void setupUI();

public:
    // Constructor
    explicit StudentDashboardWidget(QWidget* parent = nullptr);
    void setStudentRoll(const QString& roll);

private slots:
    void onLookupSchedule();
    void onShowDigitalQRPass();
    void onTriggerSOS();
    void onSubmitDriverRating();
};

#endif // STUDENT_DASHBOARD_WIDGET_HPP
