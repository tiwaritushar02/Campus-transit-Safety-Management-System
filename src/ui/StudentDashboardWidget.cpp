#include "StudentDashboardWidget.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>

// Constructor
StudentDashboardWidget::StudentDashboardWidget(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void StudentDashboardWidget::setupUI() {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(12);

    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->setSpacing(12);

    // Box 1: Schedule & QR Pass
    QGroupBox* box = new QGroupBox("Student Morning Departure & Digital Pass Lookup", this);
    QFormLayout* form = new QFormLayout(box);
    form->setSpacing(10);

    m_rollInput = new QLineEdit("2024-CS-001", box);
    QPushButton* searchBtn = new QPushButton("Lookup Departure Schedule", box);
    searchBtn->setStyleSheet("background-color: #1F6FEB; color: #FFFFFF; font-weight: 600;");
    connect(searchBtn, &QPushButton::clicked, this, &StudentDashboardWidget::onLookupSchedule);

    QPushButton* qrPassBtn = new QPushButton("Show Digital QR Bus Pass", box);
    qrPassBtn->setStyleSheet("background-color: #238636; color: #FFFFFF; font-weight: 600;");
    connect(qrPassBtn, &QPushButton::clicked, this, &StudentDashboardWidget::onShowDigitalQRPass);

    QPushButton* sosBtn = new QPushButton("EMERGENCY SOS BROADCAST", box);
    sosBtn->setStyleSheet("background-color: #DA3633; color: #FFFFFF; font-weight: 700; padding: 9px;");
    connect(sosBtn, &QPushButton::clicked, this, &StudentDashboardWidget::onTriggerSOS);

    form->addRow("Registered Roll Number:", m_rollInput);
    QHBoxLayout* btnRow = new QHBoxLayout();
    btnRow->addWidget(searchBtn);
    btnRow->addWidget(qrPassBtn);
    form->addRow("", btnRow);
    form->addRow("", sosBtn);

    // Box 2: Driver Rating
    QGroupBox* rateBox = new QGroupBox("Driver Rating & Journey Quality Feedback", this);
    QFormLayout* rateLayout = new QFormLayout(rateBox);
    rateLayout->setSpacing(8);

    m_driverIdInput = new QLineEdit("D-01", rateBox);
    m_starCombo = new QComboBox(rateBox);
    m_starCombo->addItem("5 Stars (Punctual and Safe Mountain Driving)");
    m_starCombo->addItem("4 Stars (Good Driving)");
    m_starCombo->addItem("3 Stars (Average Speed)");
    m_starCombo->addItem("2 Stars (Delayed Arrival)");
    m_starCombo->addItem("1 Star (Unsafe Driving / Violations)");

    m_feedbackInput = new QLineEdit("Very punctual and smooth driving across Kathgodam hill turns.", rateBox);
    QPushButton* submitRateBtn = new QPushButton("Submit Driver Rating", rateBox);
    submitRateBtn->setStyleSheet("background-color: #D29922; color: #0D1117; font-weight: 700;");
    connect(submitRateBtn, &QPushButton::clicked, this, &StudentDashboardWidget::onSubmitDriverRating);

    rateLayout->addRow("Driver Identifier:", m_driverIdInput);
    rateLayout->addRow("Rating Grade:", m_starCombo);
    rateLayout->addRow("Feedback Notes:", m_feedbackInput);
    rateLayout->addRow("", submitRateBtn);

    topLayout->addWidget(box, 3);
    topLayout->addWidget(rateBox, 2);
    layout->addLayout(topLayout);

    m_infoLabel = new QLabel("Enter Roll Number above to inspect assigned bus, boarding schedule, and QR pass status.", this);
    m_infoLabel->setStyleSheet("font-size: 13px; color: #8B949E; padding: 10px; background-color: #161B22; border: 1px solid #30363D; border-radius: 6px;");

    m_scheduleTable = new QTableWidget(0, 4, this);
    m_scheduleTable->setHorizontalHeaderLabels({"Assigned Bus", "Corridor Route", "Boarding Stop", "Morning Departure Time"});
    m_scheduleTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    layout->addWidget(m_infoLabel);
    layout->addWidget(m_scheduleTable, 1);
}

void StudentDashboardWidget::setStudentRoll(const QString& roll) {
    m_rollInput->setText(roll);
    onLookupSchedule();
}

void StudentDashboardWidget::onLookupSchedule() {
    QString roll = m_rollInput->text().trimmed();
    m_scheduleTable->setRowCount(1);
    m_scheduleTable->setItem(0, 0, new QTableWidgetItem("B-101 (UK-04-PA-1001)"));
    m_scheduleTable->setItem(0, 1, new QTableWidgetItem("R1 - Express Hill Corridor"));
    m_scheduleTable->setItem(0, 2, new QTableWidgetItem("ST101 - Haldwani Bus Terminal"));
    m_scheduleTable->setItem(0, 3, new QTableWidgetItem("07:00 AM (Full-Day) / 06:45 AM (Half-Day)"));

    m_infoLabel->setText("Student: " + roll + " | Fee Status: CLEARED (Paid) | Boarding Authorized at Haldwani Terminal");
    m_infoLabel->setStyleSheet("font-size: 13px; color: #3FB950; font-weight: 600; padding: 10px; background-color: #161B22; border: 1px solid #238636; border-radius: 6px;");
}

void StudentDashboardWidget::onShowDigitalQRPass() {
    QString roll = m_rollInput->text().trimmed();
    QMessageBox::information(this, "Digital QR Bus Pass",
        "====================================\n"
        " GRAPHIC ERA HILL UNIVERSITY\n"
        " DIGITAL BUS PASS - VERIFIED\n"
        "====================================\n"
        "Roll Number: " + roll + "\n"
        "Route Corridor: R1 Express (Haldwani -> GEHU Bhimtal)\n"
        "Boarding Stop: ST101 - Haldwani Terminal\n"
        "Assigned Bus: B-101\n"
        "Cryptographic Token: SHA256-GEHU-VALID-2026\n"
        "====================================");
}

void StudentDashboardWidget::onTriggerSOS() {
    QMessageBox::critical(this, "EMERGENCY SOS DISPATCHED",
        "EMERGENCY BROADCAST TRANSMITTED!\n\n"
        "Alert sent to:\n"
        "1. College Proctorial Board & Transport Authority\n"
        "2. Operating Driver of Bus B-101\n"
        "3. Local Kathgodam & Bhimtal Highway Police Patrol\n\n"
        "Live GPS Coordinates dispatched: [29.2850, 79.5380]");
}

void StudentDashboardWidget::onSubmitDriverRating() {
    QMessageBox::information(this, "Rating Recorded", "Thank you! Journey feedback recorded for Driver " + m_driverIdInput->text());
}
