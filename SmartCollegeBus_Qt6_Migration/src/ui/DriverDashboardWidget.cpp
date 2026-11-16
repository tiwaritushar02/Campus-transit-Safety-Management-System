#include "DriverDashboardWidget.hpp"
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>

// Constructor
DriverDashboardWidget::DriverDashboardWidget(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void DriverDashboardWidget::setupUI() {
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setSpacing(14);

    // Trip Telemetry
    QGroupBox* tripBox = new QGroupBox("Driver Trip Telemetry & Delay Dispatch", this);
    QFormLayout* tripForm = new QFormLayout(tripBox);
    tripForm->setSpacing(10);

    m_busCombo = new QComboBox(tripBox);
    m_busCombo->addItem("B-101 (UK-04-PA-1001)");
    m_busCombo->addItem("B-102 (UK-04-TA-2002)");
    m_busCombo->addItem("B-103 (UK-04-PA-3003)");

    QPushButton* startBtn = new QPushButton("Start Mountain Trip", tripBox);
    startBtn->setStyleSheet("background-color: #238636; color: #FFFFFF; font-weight: 700; padding: 8px;");
    connect(startBtn, &QPushButton::clicked, this, &DriverDashboardWidget::onStartTrip);

    QPushButton* endBtn = new QPushButton("Conclude Trip (Campus Terminal)", tripBox);
    endBtn->setStyleSheet("background-color: #21262D; border: 1px solid #30363D; color: #8B949E; font-weight: 600; padding: 8px;");
    connect(endBtn, &QPushButton::clicked, this, &DriverDashboardWidget::onEndTrip);

    m_delayReasonInput = new QLineEdit("Kathgodam Highway Landslide / Road Diversion", tripBox);
    m_delayMinsInput = new QLineEdit("15", tripBox);
    QPushButton* delayBtn = new QPushButton("Dispatch Delay Alert to Students & Parents", tripBox);
    delayBtn->setStyleSheet("background-color: #D29922; color: #0D1117; font-weight: 700; padding: 8px;");
    connect(delayBtn, &QPushButton::clicked, this, &DriverDashboardWidget::onReportDelay);

    tripForm->addRow("Operating Bus:", m_busCombo);
    QHBoxLayout* tripBtns = new QHBoxLayout();
    tripBtns->addWidget(startBtn);
    tripBtns->addWidget(endBtn);
    tripForm->addRow("", tripBtns);
    tripForm->addRow("Delay Reason:", m_delayReasonInput);
    tripForm->addRow("Minutes Delayed:", m_delayMinsInput);
    tripForm->addRow("", delayBtn);

    // Misbehavior Incident Box
    QGroupBox* misbehaviorBox = new QGroupBox("Report Student Misbehavior & Safety Incidents", this);
    QFormLayout* misForm = new QFormLayout(misbehaviorBox);
    misForm->setSpacing(10);

    m_misbehaviorRollInput = new QLineEdit("2024-ME-012", misbehaviorBox);
    m_misbehaviorDescInput = new QLineEdit("Loud altercation, refusal to follow safety rules during ascent.", misbehaviorBox);
    QPushButton* reportMisBtn = new QPushButton("Submit Disciplinary Incident", misbehaviorBox);
    reportMisBtn->setStyleSheet("background-color: #DA3633; color: #FFFFFF; font-weight: 700; padding: 8px;");
    connect(reportMisBtn, &QPushButton::clicked, this, &DriverDashboardWidget::onReportMisbehavior);

    misForm->addRow("Accused Student Roll:", m_misbehaviorRollInput);
    misForm->addRow("Incident Description:", m_misbehaviorDescInput);
    misForm->addRow("", reportMisBtn);

    layout->addWidget(tripBox);
    layout->addWidget(misbehaviorBox);
}

void DriverDashboardWidget::onStartTrip() {
    QMessageBox::information(this, "Trip Initiated", "Trip commenced on Bus " + m_busCombo->currentText() + ".\nGPS telemetry active.");
}

void DriverDashboardWidget::onEndTrip() {
    QMessageBox::information(this, "Trip Concluded", "Bus reached Graphic Era Hill University Bhimtal Campus.\nJourney log archived.");
}

void DriverDashboardWidget::onReportDelay() {
    QMessageBox::warning(this, "Delay Broadcast", "Delay notification of " + m_delayMinsInput->text() + " minutes sent to all boarding students and parents.");
}

void DriverDashboardWidget::onReportMisbehavior() {
    QMessageBox::information(this, "Incident Logged", "Disciplinary incident against " + m_misbehaviorRollInput->text() + " submitted to Proctorial Board.");
}
