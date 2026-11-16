#include "ParentDashboardWidget.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

// Constructor
ParentDashboardWidget::ParentDashboardWidget(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void ParentDashboardWidget::setupUI() {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(14);

    QGroupBox* box = new QGroupBox("Parent Live Telemetry & Fee Clearance Status", this);
    QHBoxLayout* form = new QHBoxLayout(box);
    form->setSpacing(10);

    m_phoneInput = new QLineEdit("+91-9811001122", box);
    QPushButton* trackBtn = new QPushButton("Track Ward Transit & ETA", box);
    trackBtn->setStyleSheet("background-color: #1F6FEB; color: #FFFFFF; font-weight: 700; padding: 8px 16px;");
    connect(trackBtn, &QPushButton::clicked, this, &ParentDashboardWidget::onTrackChild);

    form->addWidget(new QLabel("Parent Mobile / Ward Roll Number:", box));
    form->addWidget(m_phoneInput, 1);
    form->addWidget(trackBtn);

    m_infoLabel = new QLabel(
        "Enter parent contact phone above to inspect live vehicle telemetry, boarding confirmations, and semester transportation fee balances.",
        this
    );
    m_infoLabel->setStyleSheet("font-size: 13px; line-height: 1.5; color: #E6EDF3; padding: 18px; background-color: #161B22; border: 1px solid #30363D; border-radius: 8px;");

    layout->addWidget(box);
    layout->addWidget(m_infoLabel);
    layout->addStretch();
}

void ParentDashboardWidget::onTrackChild() {
    m_infoLabel->setText(
        "<h3>Ward Live Transit Status: EN ROUTE</h3>"
        "<b>Student:</b> Aarav Sharma (2024-CS-001)<br>"
        "<b>Transit Bus:</b> B-101 (UK-04-PA-1001) | Driver: Rajesh Kumar (Tel: +91-9876500001)<br>"
        "<b>Boarded At:</b> 07:15 AM at Haldwani Bus Terminal (Tikonia)<br>"
        "<b>Current GPS Location:</b> Ranibagh Junction [29.2942, 79.5445] - Ascending towards Bhimtal<br>"
        "<b>Estimated Arrival at GEHU Bhimtal:</b> 08:22 AM (On Schedule)<br>"
        "<b>Semester Fee Status:</b> <font color='#3FB950'><b>CLEARED (INR 0 Balance)</b></font>"
    );
}
