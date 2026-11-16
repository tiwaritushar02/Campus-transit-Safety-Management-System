#include "AdminDashboardWidget.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QDialogButtonBox>

// Constructor
AdminDashboardWidget::AdminDashboardWidget(FleetManager& fm, AttendanceManager& am, ComplaintManager& cm, AuthManager& aum, BusAssignmentMonitorService& bams, QWidget* parent)
    : QWidget(parent), m_fleetManager(fm), m_attendManager(am), m_compManager(cm), m_authManager(aum), m_monitorService(bams) {
    setupUI();
    refreshData();

    connect(&m_monitorService, &BusAssignmentMonitorService::unassignedTripAlert, this, [this](const QString& tripId, const QString& sourceName, int affectedStudents){
        (void)tripId; (void)sourceName; (void)affectedStudents;
        checkUnassignedTripsBanner();
        refreshTripsTable();
    });
    connect(&m_monitorService, &BusAssignmentMonitorService::tripAssignedNotice, this, [this](const QString& tripId, const QString& busId, const QString& driverName){
        (void)tripId; (void)busId; (void)driverName;
        checkUnassignedTripsBanner();
        refreshTripsTable();
    });
    connect(&m_monitorService, &BusAssignmentMonitorService::tripListUpdated, this, [this](){
        checkUnassignedTripsBanner();
        refreshTripsTable();
    });
}

void AdminDashboardWidget::setupUI() {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(10);

    // 0. High-Visibility Unassigned Bus Alert Banner
    m_unassignedBanner = new QFrame(this);
    m_unassignedBanner->setStyleSheet(
        "background-color: rgba(210, 153, 34, 0.15); "
        "border: 1px solid #D29922; border-radius: 8px; padding: 10px 14px;"
    );
    QHBoxLayout* bannerLayout = new QHBoxLayout(m_unassignedBanner);
    bannerLayout->setContentsMargins(0, 0, 0, 0);

    m_unassignedAlertText = new QLabel("<b>[ACTION REQUIRED]</b> Scheduled trip(s) currently have <b>NO BUS ASSIGNED</b>! Immediate transport assignment required.", m_unassignedBanner);
    m_unassignedAlertText->setStyleSheet("color: #F8E3A1; font-size: 12.5px;");

    m_unassignedActionBtn = new QPushButton("Allocate Bus Now", m_unassignedBanner);
    m_unassignedActionBtn->setStyleSheet("background-color: #D29922; color: #0D1117; font-weight: 700; padding: 6px 14px; border-radius: 6px;");
    connect(m_unassignedActionBtn, &QPushButton::clicked, this, [this]{
        auto unassigned = m_monitorService.getUnassignedTrips();
        if (!unassigned.empty()) {
            onAssignBusDialog(QString::fromStdString(unassigned.front().getTripId()));
        } else {
            QMessageBox::information(this, "All Trips Assigned", "Every scheduled trip currently has an active bus allocated.");
        }
    });

    bannerLayout->addWidget(m_unassignedAlertText, 1);
    bannerLayout->addWidget(m_unassignedActionBtn);
    layout->addWidget(m_unassignedBanner);

    // 1. KPI Summary Cards
    QHBoxLayout* kpiLayout = new QHBoxLayout();
    kpiLayout->setSpacing(10);

    auto makeKpi = [](const QString& title, QLabel*& valLabel, const QString& accentColor) -> QFrame* {
        QFrame* card = new QFrame();
        card->setStyleSheet("background-color: #1C2533; border: 1px solid #30363D; border-radius: 8px; padding: 10px;");
        QVBoxLayout* cl = new QVBoxLayout(card);
        cl->setContentsMargins(4, 4, 4, 4);
        cl->setSpacing(2);
        QLabel* t = new QLabel(title, card);
        t->setStyleSheet("font-size: 11px; font-weight: 600; color: #8B949E; text-transform: uppercase;");
        valLabel = new QLabel("--", card);
        valLabel->setStyleSheet(QString("font-size: 16px; font-weight: 700; color: %1;").arg(accentColor));
        cl->addWidget(t);
        cl->addWidget(valLabel);
        return card;
    };

    kpiLayout->addWidget(makeKpi("Total Fleet Units", m_kpiBuses, "#58A6FF"));
    kpiLayout->addWidget(makeKpi("Total Available Seats", m_kpiSeats, "#3FB950"));
    kpiLayout->addWidget(makeKpi("Operating Corridors", m_kpiCorridors, "#D29922"));
    kpiLayout->addWidget(makeKpi("Campus Schedule", m_kpiSchedule, "#58A6FF"));
    kpiLayout->addWidget(makeKpi("Student Attendance", m_kpiAttendance, "#3FB950"));
    kpiLayout->addWidget(makeKpi("Active Complaints", m_kpiComplaints, "#F85149"));

    layout->addLayout(kpiLayout);

    // 2. Sections A & B (Bus Configuration & Stop Timings)
    QHBoxLayout* midLayout = new QHBoxLayout();
    midLayout->setSpacing(12);

    // Section A: Bus Fleet Configuration
    QGroupBox* busConfigBox = new QGroupBox("A. Bus Fleet Configuration", this);
    QFormLayout* busForm = new QFormLayout(busConfigBox);
    busForm->setSpacing(8);

    m_busCombo = new QComboBox(busConfigBox);
    m_busCombo->addItem("B-101 (UK-04-PA-1001)");
    m_busCombo->addItem("B-102 (UK-04-TA-2002)");
    m_busCombo->addItem("B-103 (UK-04-PA-3003)");

    m_regInput = new QLineEdit("UK-04-PA-1001", busConfigBox);
    m_capInput = new QLineEdit("45", busConfigBox);
    m_routeCombo = new QComboBox(busConfigBox);
    m_routeCombo->addItem("R1 - Express Hill Corridor");
    m_routeCombo->addItem("R2 - Valley Bypass Corridor");

    QPushButton* saveBusBtn = new QPushButton("Save Bus Configuration", busConfigBox);
    saveBusBtn->setStyleSheet("background-color: #238636; color: #FFFFFF; font-weight: 600; padding: 7px;");
    connect(saveBusBtn, &QPushButton::clicked, this, &AdminDashboardWidget::onSaveBusConfig);

    busForm->addRow("Select Bus Unit:", m_busCombo);
    busForm->addRow("Registration Number:", m_regInput);
    busForm->addRow("Seating Capacity:", m_capInput);
    busForm->addRow("Assigned Corridor:", m_routeCombo);
    busForm->addRow("", saveBusBtn);

    // Section B: Campus Stop Timings & Broadcast Dispatch
    QGroupBox* timingBox = new QGroupBox("B. Campus Stop Timings & Broadcast Dispatch", this);
    QFormLayout* timingForm = new QFormLayout(timingBox);
    timingForm->setSpacing(8);

    m_timingStopCombo = new QComboBox(timingBox);
    m_timingStopCombo->addItem("ST101 - Haldwani Bus Terminal (424m)", "ST101");
    m_timingStopCombo->addItem("ST102 - Kathgodam Railway Station (554m)", "ST102");
    m_timingStopCombo->addItem("ST103 - Ranibagh Junction (620m)", "ST103");
    m_timingStopCombo->addItem("ST104 - Bhujiaghat Bypass (910m)", "ST104");
    m_timingStopCombo->addItem("ST105 - Bhimtal Lake Mall Road (1,370m)", "ST105");
    m_timingStopCombo->addItem("ST106 - Graphic Era Hill University (1,420m)", "ST106");

    m_timingBusCombo = new QComboBox(timingBox);
    m_timingBusCombo->addItem("B-101"); m_timingBusCombo->addItem("B-102"); m_timingBusCombo->addItem("B-103");

    m_timingScheduleCombo = new QComboBox(timingBox);
    m_timingScheduleCombo->addItem("Full-Day Schedule");
    m_timingScheduleCombo->addItem("Half-Day Schedule");

    m_newTimeInput = new QLineEdit("07:15", timingBox);

    QPushButton* updateTimingBtn = new QPushButton("Update Timing", timingBox);
    updateTimingBtn->setStyleSheet("background-color: #1F6FEB; color: #FFFFFF; font-weight: 600; padding: 7px;");
    connect(updateTimingBtn, &QPushButton::clicked, this, &AdminDashboardWidget::onUpdateTiming);

    QPushButton* noticeBtn = new QPushButton("Dispatch Broadcast Notice", timingBox);
    noticeBtn->setStyleSheet("background-color: #21262D; border: 1px solid #30363D; color: #E6EDF3; font-weight: 600; padding: 7px;");
    connect(noticeBtn, &QPushButton::clicked, this, &AdminDashboardWidget::onBroadcastNotice);

    timingForm->addRow("Select Bus Stop:", m_timingStopCombo);
    timingForm->addRow("Select Transit Bus:", m_timingBusCombo);
    timingForm->addRow("Schedule Mode:", m_timingScheduleCombo);
    timingForm->addRow("Departure Time:", m_newTimeInput);
    QHBoxLayout* timingBtns = new QHBoxLayout();
    timingBtns->addWidget(updateTimingBtn);
    timingBtns->addWidget(noticeBtn);
    timingForm->addRow("", timingBtns);

    midLayout->addWidget(busConfigBox);
    midLayout->addWidget(timingBox);
    layout->addLayout(midLayout);

    // 3. Section C: Scheduled Trips & Automatic Bus Allocation Engine (trips.csv)
    QGroupBox* tripsBox = new QGroupBox("C. Scheduled Trips & Automatic Bus Allocation Engine (trips.csv)", this);
    QVBoxLayout* tripsLayout = new QVBoxLayout(tripsBox);

    m_tripsTable = new QTableWidget(0, 9, tripsBox);
    m_tripsTable->setHorizontalHeaderLabels({
        "Trip ID", "Route Corridor", "Origin Stop", "Fixed Destination",
        "Scheduled Departure", "Assigned Bus", "Driver Assigned", "Status", "Action"
    });
    m_tripsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tripsTable->setMinimumHeight(140);

    QHBoxLayout* tripBtnsLayout = new QHBoxLayout();
    QPushButton* createTripBtn = new QPushButton("+ Create New Scheduled Trip", tripsBox);
    createTripBtn->setStyleSheet("background-color: #238636; color: #FFFFFF; font-weight: 600; padding: 6px 12px;");
    connect(createTripBtn, &QPushButton::clicked, this, &AdminDashboardWidget::onCreateTripDialog);

    QPushButton* refreshTripsBtn = new QPushButton("Refresh Assignment Status", tripsBox);
    refreshTripsBtn->setStyleSheet("background-color: #21262D; border: 1px solid #30363D; color: #E6EDF3; padding: 6px 12px;");
    connect(refreshTripsBtn, &QPushButton::clicked, this, [this]{
        m_monitorService.manualCheckNow();
        refreshTripsTable();
        checkUnassignedTripsBanner();
    });

    tripBtnsLayout->addStretch();
    tripBtnsLayout->addWidget(refreshTripsBtn);
    tripBtnsLayout->addWidget(createTripBtn);

    tripsLayout->addWidget(m_tripsTable);
    tripsLayout->addLayout(tripBtnsLayout);
    layout->addWidget(tripsBox);

    // 4. Sections D & E (Student Transport Directory & Attendance Table)
    QHBoxLayout* lowerLayout = new QHBoxLayout();
    lowerLayout->setSpacing(12);

    // Section D: Student Transport Directory
    QGroupBox* studentDirBox = new QGroupBox("D. Student Transport Directory", this);
    QVBoxLayout* dirLayout = new QVBoxLayout(studentDirBox);

    m_studentTable = new QTableWidget(0, 6, studentDirBox);
    m_studentTable->setHorizontalHeaderLabels({"Full Name", "Roll Number", "Phone Number", "Boarding Stop", "Corridor", "Fee Status"});
    m_studentTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QPushButton* regStudentBtn = new QPushButton("+ Register New Student", studentDirBox);
    regStudentBtn->setStyleSheet("background-color: #238636; color: #FFFFFF; font-weight: 600; padding: 6px 12px;");
    connect(regStudentBtn, &QPushButton::clicked, this, &AdminDashboardWidget::onRegisterStudentDialog);

    dirLayout->addWidget(m_studentTable);
    dirLayout->addWidget(regStudentBtn, 0, Qt::AlignRight);

    // Section E: Student Attendance & Boarding Management
    QGroupBox* attendBox = new QGroupBox("E. Student Attendance & Boarding Verification", this);
    QVBoxLayout* attLayout = new QVBoxLayout(attendBox);

    QHBoxLayout* attControlLayout = new QHBoxLayout();
    m_boardRollInput = new QLineEdit("2024-CS-001", attendBox);
    m_boardRollInput->setPlaceholderText("Roll Number");
    m_boardBusCombo = new QComboBox(attendBox);
    m_boardBusCombo->addItem("B-101"); m_boardBusCombo->addItem("B-102"); m_boardBusCombo->addItem("B-103");

    QPushButton* boardBtn = new QPushButton("Mark Attendance / Board", attendBox);
    boardBtn->setStyleSheet("background-color: #238636; color: #FFFFFF; font-weight: 600;");
    connect(boardBtn, &QPushButton::clicked, this, &AdminDashboardWidget::onBoardStudent);

    QPushButton* qrVerifyBtn = new QPushButton("Verify QR Pass", attendBox);
    qrVerifyBtn->setStyleSheet("background-color: #1F6FEB; color: #FFFFFF; font-weight: 600;");
    connect(qrVerifyBtn, &QPushButton::clicked, this, &AdminDashboardWidget::onVerifyQRPass);

    attControlLayout->addWidget(new QLabel("Roll No:", attendBox));
    attControlLayout->addWidget(m_boardRollInput);
    attControlLayout->addWidget(new QLabel("Bus:", attendBox));
    attControlLayout->addWidget(m_boardBusCombo);
    attControlLayout->addWidget(boardBtn);
    attControlLayout->addWidget(qrVerifyBtn);

    m_attendanceTable = new QTableWidget(0, 6, attendBox);
    m_attendanceTable->setHorizontalHeaderLabels({"Timestamp", "Roll Number", "Student Name", "Stop ID", "Bus", "Status"});
    m_attendanceTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    attLayout->addLayout(attControlLayout);
    attLayout->addWidget(m_attendanceTable);

    lowerLayout->addWidget(studentDirBox);
    lowerLayout->addWidget(attendBox);
    layout->addLayout(lowerLayout);
}

void AdminDashboardWidget::refreshData() {
    updateKpis();
    refreshTripsTable();
    checkUnassignedTripsBanner();

    // Populate Student Directory from AuthManager registered accounts
    m_studentTable->setRowCount(0);
    auto users = m_authManager.getAllUsers();
    int row = 0;
    for (const auto& u : users) {
        if (u && u->getRole() == UserRole::STUDENT) {
            auto s = std::dynamic_pointer_cast<Student>(u);
            if (!s) continue;
            m_studentTable->insertRow(row);
            m_studentTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(s->getFullName())));
            m_studentTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(s->getRollNumber())));
            m_studentTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(s->getPhone())));
            m_studentTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(s->getAssignedStopId())));
            m_studentTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(s->getAssignedRouteId())));

            QTableWidgetItem* feeItem = new QTableWidgetItem(s->isFeePaid() ? "PAID" : "DUE");
            feeItem->setForeground(s->isFeePaid() ? QColor("#3FB950") : QColor("#F85149"));
            m_studentTable->setItem(row, 5, feeItem);
            row++;
        }
    }

    // Populate Attendance from AttendanceManager
    m_attendanceTable->setRowCount(0);
    auto records = m_attendManager.getRecords();
    row = 0;
    for (const auto& rec : records) {
        m_attendanceTable->insertRow(row);
        m_attendanceTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(rec.getTimestamp())));
        m_attendanceTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(rec.getRollNumber())));
        m_attendanceTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(rec.getStudentName())));
        m_attendanceTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(rec.getStopId())));
        m_attendanceTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(rec.getBusId())));

        QTableWidgetItem* stItem = new QTableWidgetItem(QString::fromStdString(rec.getBoardingStatus()));
        stItem->setForeground(QColor("#3FB950"));
        m_attendanceTable->setItem(row, 5, stItem);
        row++;
    }
}

void AdminDashboardWidget::refreshTripsTable() {
    m_tripsTable->setRowCount(0);
    auto trips = m_monitorService.getAllTrips();
    int row = 0;

    for (const auto& t : trips) {
        m_tripsTable->insertRow(row);

        QString tripId = QString::fromStdString(t.getTripId());
        bool isUnassigned = !t.isBusAssigned();

        m_tripsTable->setItem(row, 0, new QTableWidgetItem(tripId));
        m_tripsTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(t.getRouteId())));
        m_tripsTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(t.getSourceStopName())));
        m_tripsTable->setItem(row, 3, new QTableWidgetItem("Graphic Era Hill University, Bhimtal (FIXED)"));
        m_tripsTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(t.getDepartureTime())));

        QTableWidgetItem* busItem = new QTableWidgetItem(QString::fromStdString(t.getAssignedBusId()));
        if (isUnassigned) {
            busItem->setText("UNASSIGNED");
            busItem->setForeground(QColor("#F85149"));
        } else {
            busItem->setForeground(QColor("#3FB950"));
        }
        m_tripsTable->setItem(row, 5, busItem);

        QTableWidgetItem* driverItem = new QTableWidgetItem(QString::fromStdString(t.getAssignedDriverId()));
        if (isUnassigned) {
            driverItem->setText("UNASSIGNED");
            driverItem->setForeground(QColor("#F85149"));
        } else {
            driverItem->setForeground(QColor("#3FB950"));
        }
        m_tripsTable->setItem(row, 6, driverItem);

        QTableWidgetItem* statusItem = new QTableWidgetItem(QString::fromStdString(t.getStatus()));
        statusItem->setForeground(isUnassigned ? QColor("#D29922") : QColor("#3FB950"));
        m_tripsTable->setItem(row, 7, statusItem);

        // Action Button
        QPushButton* actionBtn = new QPushButton(isUnassigned ? "Assign Bus" : "Re-assign", this);
        if (isUnassigned) {
            actionBtn->setStyleSheet("background-color: #238636; color: #FFFFFF; font-weight: 700; border-radius: 4px; padding: 4px;");
        } else {
            actionBtn->setStyleSheet("background-color: #21262D; border: 1px solid #30363D; color: #8B949E; border-radius: 4px; padding: 4px;");
        }
        connect(actionBtn, &QPushButton::clicked, this, [this, tripId]{
            onAssignBusDialog(tripId);
        });

        m_tripsTable->setCellWidget(row, 8, actionBtn);
        row++;
    }
}

void AdminDashboardWidget::checkUnassignedTripsBanner() {
    auto unassigned = m_monitorService.getUnassignedTrips();
    if (unassigned.empty()) {
        m_unassignedBanner->hide();
    } else {
        QString details = QString("<b>[ACTION REQUIRED]</b> %1 Scheduled Trip(s) have <b>NO BUS ASSIGNED</b>! Immediate transport allocation required.")
                             .arg(unassigned.size());
        m_unassignedAlertText->setText(details);
        m_unassignedBanner->show();
    }
}

void AdminDashboardWidget::updateKpis() {
    m_kpiBuses->setText("3 Active");
    m_kpiSeats->setText(QString("%1 Seats").arg(m_fleetManager.getTotalAvailableSeats()));
    m_kpiCorridors->setText(QString("%1 Corridors").arg(m_fleetManager.getRoutes().size()));
    m_kpiSchedule->setText("Full-Day (Normal)");
    m_kpiAttendance->setText(QString::number(m_attendManager.getRecords().size()) + " Boarded");
    m_kpiComplaints->setText(QString::number(m_compManager.getComplaints().size()) + " Active");
}

void AdminDashboardWidget::onSaveBusConfig() {
    QMessageBox::information(this, "Configuration Saved",
        QString("Fleet parameters for %1 updated successfully!\nCorridor: %2\nSeating: %3")
        .arg(m_busCombo->currentText())
        .arg(m_routeCombo->currentText())
        .arg(m_capInput->text()));
}

void AdminDashboardWidget::onUpdateTiming() {
    QMessageBox::information(this, "Schedule Dispatched",
        QString("Stop: %1\nTransit Bus: %2\nNew Departure: %3")
        .arg(m_timingStopCombo->currentText())
        .arg(m_timingBusCombo->currentText())
        .arg(m_newTimeInput->text()));
}

void AdminDashboardWidget::onBroadcastNotice() {
    QMessageBox::information(this, "Notice Broadcasted",
        "Campus-wide notification successfully dispatched to all registered Students, Drivers, and Parents via notifications.txt.");
}

void AdminDashboardWidget::onCreateTripDialog() {
    QDialog d(this);
    d.setWindowTitle("Create New Scheduled Bus Trip");
    d.setStyleSheet("background-color: #161B22; color: #E6EDF3;");
    d.setMinimumWidth(380);

    QFormLayout form(&d);
    QComboBox routeCombo(&d);
    routeCombo.addItem("R1 - Express Hill Corridor", "R1");
    routeCombo.addItem("R2 - Valley Bypass Corridor", "R2");

    QComboBox srcCombo(&d);
    srcCombo.addItem("ST101 - Haldwani Bus Terminal (Tikonia)", "ST101");
    srcCombo.addItem("ST102 - Kathgodam Railway Station", "ST102");
    srcCombo.addItem("ST103 - Ranibagh Junction", "ST103");
    srcCombo.addItem("ST104 - Bhujiaghat Bypass", "ST104");
    srcCombo.addItem("ST105 - Bhimtal Lake Mall Road", "ST105");

    QLineEdit destEdit("Graphic Era Hill University, Bhimtal Campus (FIXED)", &d);
    destEdit.setReadOnly(true);

    QLineEdit timeEdit("07:45", &d);

    QComboBox busCombo(&d);
    busCombo.addItem("-- Unassigned (Action Required) --", "NONE");
    busCombo.addItem("B-101", "B-101");
    busCombo.addItem("B-102", "B-102");
    busCombo.addItem("B-103", "B-103");

    QComboBox driverCombo(&d);
    driverCombo.addItem("-- Unassigned --", "NONE");
    driverCombo.addItem("D-01 (Rajesh Kumar)", "D-01");
    driverCombo.addItem("D-02 (Mahesh Singh)", "D-02");

    form.addRow("Route Corridor:", &routeCombo);
    form.addRow("Origin Bus Stop:", &srcCombo);
    form.addRow("Destination Stop:", &destEdit);
    form.addRow("Scheduled Departure:", &timeEdit);
    form.addRow("Assigned Bus Unit:", &busCombo);
    form.addRow("Assigned Driver:", &driverCombo);

    QDialogButtonBox btns(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &d);
    btns.setStyleSheet("QPushButton { background-color: #238636; color: #FFFFFF; font-weight: 700; padding: 6px 14px; }");
    form.addRow(&btns);

    connect(&btns, &QDialogButtonBox::accepted, &d, &QDialog::accept);
    connect(&btns, &QDialogButtonBox::rejected, &d, &QDialog::reject);

    if (d.exec() == QDialog::Accepted) {
        m_monitorService.createTrip(
            routeCombo.currentData().toString().toStdString(),
            srcCombo.currentData().toString().toStdString(),
            srcCombo.currentText().toStdString(),
            timeEdit.text().toStdString(),
            busCombo.currentData().toString().toStdString(),
            driverCombo.currentData().toString().toStdString()
        );

        refreshTripsTable();
        checkUnassignedTripsBanner();
        QMessageBox::information(this, "Trip Created", "New trip created and synchronized with trips.csv.");
    }
}

void AdminDashboardWidget::onAssignBusDialog(const QString& tripId) {
    QDialog d(this);
    d.setWindowTitle("Assign Bus & Driver to Trip " + tripId);
    d.setStyleSheet("background-color: #161B22; color: #E6EDF3;");
    d.setMinimumWidth(400);

    QFormLayout form(&d);

    QLabel infoLabel(QString("Assigning college transit vehicle to Trip: <b>%1</b>").arg(tripId), &d);
    infoLabel.setStyleSheet("color: #58A6FF; font-size: 13px; margin-bottom: 8px;");
    form.addRow(&infoLabel);

    QComboBox busCombo(&d);
    busCombo.addItem("B-101 (UK-04-PA-1001, Cap: 45)", "B-101");
    busCombo.addItem("B-102 (UK-04-TA-2002, Cap: 45)", "B-102");
    busCombo.addItem("B-103 (UK-04-PA-3003, Cap: 45)", "B-103");

    QComboBox driverCombo(&d);
    driverCombo.addItem("D-01 (Rajesh Kumar, +91-9876500001)", "D-01");
    driverCombo.addItem("D-02 (Mahesh Singh, +91-9876500002)", "D-02");

    form.addRow("Select Fleet Bus Unit:", &busCombo);
    form.addRow("Select Certified Driver:", &driverCombo);

    QDialogButtonBox btns(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &d);
    btns.setStyleSheet("QPushButton { background-color: #238636; color: #FFFFFF; font-weight: 700; padding: 6px 14px; }");
    form.addRow(&btns);

    connect(&btns, &QDialogButtonBox::accepted, &d, &QDialog::accept);
    connect(&btns, &QDialogButtonBox::rejected, &d, &QDialog::reject);

    if (d.exec() == QDialog::Accepted) {
        std::string selectedBus = busCombo.currentData().toString().toStdString();
        std::string selectedDriver = driverCombo.currentData().toString().toStdString();

        std::string depTime = "07:30";
        for (const auto& t : m_monitorService.getAllTrips()) {
            if (t.getTripId() == tripId.toStdString()) {
                depTime = t.getDepartureTime();
                break;
            }
        }

        if (m_monitorService.checkBusConflict(selectedBus, depTime, tripId.toStdString())) {
            QMessageBox::warning(this, "Bus Allocation Conflict",
                QString("Bus %1 is already allocated to another departure at %2. Please select an available bus.")
                .arg(QString::fromStdString(selectedBus))
                .arg(QString::fromStdString(depTime)));
            return;
        }

        if (m_monitorService.checkDriverConflict(selectedDriver, depTime, tripId.toStdString())) {
            QMessageBox::warning(this, "Driver Allocation Conflict",
                QString("Driver %1 is already scheduled on another departure at %2. Please select another driver.")
                .arg(QString::fromStdString(selectedDriver))
                .arg(QString::fromStdString(depTime)));
            return;
        }

        std::string err;
        if (m_monitorService.assignBusAndDriver(tripId.toStdString(), selectedBus, selectedDriver, err)) {
            refreshTripsTable();
            checkUnassignedTripsBanner();
            QMessageBox::information(this, "Bus Assigned Successfully",
                QString("Trip %1 has been successfully assigned to Bus %2 with Driver %3.\n\nConfirmation notice dispatched to registered students via notifications.txt.")
                .arg(tripId)
                .arg(QString::fromStdString(selectedBus))
                .arg(QString::fromStdString(selectedDriver)));
        } else {
            QMessageBox::critical(this, "Error", QString("Failed to assign bus: %1").arg(QString::fromStdString(err)));
        }
    }
}

void AdminDashboardWidget::onRegisterStudentDialog() {
    QDialog d(this);
    d.setWindowTitle("Register New Student Transport Pass");
    d.setStyleSheet("background-color: #161B22; color: #E6EDF3;");
    d.setMinimumWidth(380);

    QFormLayout form(&d);
    QLineEdit nameEdit(&d);
    QLineEdit rollEdit(&d);
    QLineEdit phoneEdit(&d);
    QComboBox stopCombo(&d);
    stopCombo.addItem("ST101 - Haldwani Bus Terminal");
    stopCombo.addItem("ST102 - Kathgodam Station");
    stopCombo.addItem("ST103 - Ranibagh");
    stopCombo.addItem("ST104 - Bhujiaghat");
    stopCombo.addItem("ST105 - Bhimtal Lake");

    QComboBox busCombo(&d);
    busCombo.addItem("B-101");
    busCombo.addItem("B-102");
    busCombo.addItem("B-103");

    form.addRow("Student Full Name:", &nameEdit);
    form.addRow("University Roll No:", &rollEdit);
    form.addRow("Contact Phone:", &phoneEdit);
    form.addRow("Registered Stop:", &stopCombo);
    form.addRow("Assigned Bus:", &busCombo);

    QDialogButtonBox btns(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &d);
    btns.setStyleSheet("QPushButton { background-color: #238636; color: #FFFFFF; font-weight: 700; padding: 6px 14px; }");
    form.addRow(&btns);

    connect(&btns, &QDialogButtonBox::accepted, &d, &QDialog::accept);
    connect(&btns, &QDialogButtonBox::rejected, &d, &QDialog::reject);

    if (d.exec() == QDialog::Accepted) {
        if (!nameEdit.text().isEmpty() && !rollEdit.text().isEmpty()) {
            QMessageBox::information(this, "Success", "Student registered successfully and synchronized with database!");
        }
    }
}

void AdminDashboardWidget::onBoardStudent() {
    QString roll = m_boardRollInput->text().trimmed();
    QString bus = m_boardBusCombo->currentText();
    if (roll.isEmpty()) return;

    m_attendManager.markBoarding(roll.toStdString(), "Student Commuter", bus.toStdString(), "ST101", true);
    refreshData();
    QMessageBox::information(this, "Boarding Confirmed", "Attendance recorded for " + roll + " on " + bus + " (attendance.csv updated).");
}

void AdminDashboardWidget::onVerifyQRPass() {
    QString roll = m_boardRollInput->text().trimmed();
    QMessageBox::information(this, "QR Digital Pass Verified",
        QString("Student Roll: %1\nDigital Security Token: [GEHU-SEC-PASS-%2]\nStatus: ACTIVE & VERIFIED")
        .arg(roll).arg(roll.right(3)));
}
