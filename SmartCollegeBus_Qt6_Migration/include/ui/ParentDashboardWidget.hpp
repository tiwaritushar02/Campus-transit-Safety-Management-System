#ifndef PARENT_DASHBOARD_WIDGET_HPP
#define PARENT_DASHBOARD_WIDGET_HPP

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

// Inheritance
class ParentDashboardWidget : public QWidget {
    Q_OBJECT

private:
    QLineEdit* m_phoneInput;
    QLabel* m_infoLabel;

    void setupUI();

public:
    // Constructor
    explicit ParentDashboardWidget(QWidget* parent = nullptr);

private slots:
    void onTrackChild();
};

#endif // PARENT_DASHBOARD_WIDGET_HPP
