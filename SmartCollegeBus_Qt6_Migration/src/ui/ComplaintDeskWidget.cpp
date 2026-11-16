#include "ComplaintDeskWidget.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>

// Constructor
ComplaintDeskWidget::ComplaintDeskWidget(ComplaintManager& cm, QWidget* parent)
    : QWidget(parent), m_compManager(cm), m_activeUsername("admin"), m_activeRole(UserRole::ADMIN),
      m_selectedComplaintId("CMP-102") {
    setupUI();
    refreshData();
}

void ComplaintDeskWidget::setActiveUser(const std::string& username, UserRole role) {
    m_activeUsername = username;
    m_activeRole = role;
    bool isAdmin = (role == UserRole::ADMIN);
    m_investigateBtn->setVisible(isAdmin);
    m_resolveBtn->setVisible(isAdmin);
    m_rejectBtn->setVisible(isAdmin);
    refreshData();
}

void ComplaintDeskWidget::setupUI() {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(12);

    // 1. Filter Bar
    QFrame* filterCard = new QFrame(this);
    filterCard->setStyleSheet("background-color: #161B22; border: 1px solid #30363D; border-radius: 8px; padding: 8px 12px;");
    QHBoxLayout* fl = new QHBoxLayout(filterCard);
    fl->setContentsMargins(4, 4, 4, 4);
    fl->setSpacing(10);

    m_filterCat = new QComboBox(filterCard);
    m_filterCat->addItem("Category: All");
    m_filterCat->addItem("Category: Rash Driving");
    m_filterCat->addItem("Category: Student Misbehavior");
    m_filterCat->addItem("Category: Delay Notice");
    connect(m_filterCat, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ComplaintDeskWidget::onFilterChanged);

    m_filterRepRole = new QComboBox(filterCard);
    m_filterRepRole->addItem("Reporter: All");
    m_filterRepRole->addItem("Reporter: Student");
    m_filterRepRole->addItem("Reporter: Driver");
    m_filterRepRole->addItem("Reporter: Parent");
    connect(m_filterRepRole, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ComplaintDeskWidget::onFilterChanged);

    m_filterAccRole = new QComboBox(filterCard);
    m_filterAccRole->addItem("Accused: All");
    m_filterAccRole->addItem("Accused: Driver");
    m_filterAccRole->addItem("Accused: Student");
    connect(m_filterAccRole, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ComplaintDeskWidget::onFilterChanged);

    m_filterStatus = new QComboBox(filterCard);
    m_filterStatus->addItem("Status: All");
    m_filterStatus->addItem("Status: OPEN");
    m_filterStatus->addItem("Status: UNDER INVESTIGATION");
    m_filterStatus->addItem("Status: POLL ACTIVE");
    m_filterStatus->addItem("Status: RESOLVED");
    m_filterStatus->addItem("Status: REJECTED");
    connect(m_filterStatus, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ComplaintDeskWidget::onFilterChanged);

    fl->addWidget(new QLabel("Filters:", filterCard));
    fl->addWidget(m_filterCat);
    fl->addWidget(m_filterRepRole);
    fl->addWidget(m_filterAccRole);
    fl->addWidget(m_filterStatus);
    fl->addStretch();
    layout->addWidget(filterCard);

    // 2. Master Table (All 11 Fields)
    m_table = new QTableWidget(0, 8, this);
    m_table->setHorizontalHeaderLabels({"ID", "Category", "Reported By", "Against", "Bus", "Date", "Time", "Status"});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(m_table, &QTableWidget::itemSelectionChanged, this, &ComplaintDeskWidget::onTableSelectionChanged);
    layout->addWidget(m_table, 3);

    // 3. Bottom Panels: Inspection, Student Poll, and Submission
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->setSpacing(12);

    // Panel 1: Inspection
    QGroupBox* detailBox = new QGroupBox("Selected Incident Inspection & Resolution History", this);
    QVBoxLayout* dbl = new QVBoxLayout(detailBox);
    dbl->setSpacing(8);

    m_detailHeader = new QLabel("Select a complaint from table above to inspect details.", detailBox);
    m_detailHeader->setStyleSheet("font-size: 13px; font-weight: 700; color: #58A6FF;");

    m_detailBody = new QLabel("", detailBox);
    m_detailBody->setStyleSheet("font-size: 12.5px; color: #E6EDF3; line-height: 1.4;");
    m_detailBody->setWordWrap(true);

    QHBoxLayout* adminActionLayout = new QHBoxLayout();
    m_investigateBtn = new QPushButton("Investigate", detailBox);
    m_investigateBtn->setStyleSheet("background-color: #1F6FEB; color: #FFFFFF; font-weight: 600; font-size: 12px;");
    connect(m_investigateBtn, &QPushButton::clicked, this, &ComplaintDeskWidget::onInvestigate);

    m_resolveBtn = new QPushButton("Resolve", detailBox);
    m_resolveBtn->setStyleSheet("background-color: #238636; color: #FFFFFF; font-weight: 600; font-size: 12px;");
    connect(m_resolveBtn, &QPushButton::clicked, this, &ComplaintDeskWidget::onResolve);

    m_rejectBtn = new QPushButton("Reject with Reason", detailBox);
    m_rejectBtn->setStyleSheet("background-color: #21262D; border: 1px solid #30363D; color: #F85149; font-weight: 600; font-size: 12px;");
    connect(m_rejectBtn, &QPushButton::clicked, this, &ComplaintDeskWidget::onReject);

    adminActionLayout->addWidget(m_investigateBtn);
    adminActionLayout->addWidget(m_resolveBtn);
    adminActionLayout->addWidget(m_rejectBtn);

    dbl->addWidget(m_detailHeader);
    dbl->addWidget(m_detailBody, 1);
    dbl->addLayout(adminActionLayout);

    // Panel 2: Student Poll Box
    QGroupBox* pollBox = new QGroupBox("Student Peer Disciplinary Poll (YES / NO)", this);
    QVBoxLayout* pbl = new QVBoxLayout(pollBox);
    pbl->setSpacing(8);

    m_pollQuestion = new QLabel("Question: \"Do you agree that this incident requires disciplinary action?\"", pollBox);
    m_pollQuestion->setStyleSheet("font-size: 12px; font-weight: 700; color: #E6EDF3;");
    m_pollQuestion->setWordWrap(true);

    m_pollStats = new QLabel("Total Votes: 0 | YES: 0 (0%) | NO: 0 (0%)", pollBox);
    m_pollStats->setStyleSheet("font-size: 11.5px; color: #58A6FF; font-weight: 600;");

    m_pollProgressBar = new QProgressBar(pollBox);
    m_pollProgressBar->setRange(0, 100);

    QHBoxLayout* voteBtns = new QHBoxLayout();
    m_voteYesBtn = new QPushButton("Vote YES", pollBox);
    m_voteYesBtn->setStyleSheet("background-color: #238636; color: #FFFFFF; font-weight: 700; padding: 6px;");
    connect(m_voteYesBtn, &QPushButton::clicked, this, [this]{ onVote(true); });

    m_voteNoBtn = new QPushButton("Vote NO", pollBox);
    m_voteNoBtn->setStyleSheet("background-color: #30363D; color: #E6EDF3; font-weight: 700; padding: 6px;");
    connect(m_voteNoBtn, &QPushButton::clicked, this, [this]{ onVote(false); });

    voteBtns->addWidget(m_voteYesBtn);
    voteBtns->addWidget(m_voteNoBtn);

    m_pollMessage = new QLabel("", pollBox);
    m_pollMessage->setStyleSheet("font-size: 11px; color: #3FB950; font-weight: 600;");
    m_pollMessage->setWordWrap(true);

    pbl->addWidget(m_pollQuestion);
    pbl->addWidget(m_pollStats);
    pbl->addWidget(m_pollProgressBar);
    pbl->addLayout(voteBtns);
    pbl->addWidget(m_pollMessage);
    pbl->addStretch();

    // Panel 3: Submit New Complaint Form
    QGroupBox* newBox = new QGroupBox("Submit New Safety Complaint", this);
    QFormLayout* ncl = new QFormLayout(newBox);
    ncl->setSpacing(6);

    m_newCatCombo = new QComboBox(newBox);
    m_newCatCombo->addItem("Rash Driving");
    m_newCatCombo->addItem("Student Misbehavior");
    m_newCatCombo->addItem("Delay Notice");

    m_newAccNameEdit = new QLineEdit("Rajesh Kumar", newBox);
    m_newAccRoleCombo = new QComboBox(newBox);
    m_newAccRoleCombo->addItem("Driver");
    m_newAccRoleCombo->addItem("Student");

    m_newBusEdit = new QLineEdit("B-101", newBox);
    m_newDescEdit = new QLineEdit("", newBox);
    m_newDescEdit->setPlaceholderText("Description of violation...");

    QPushButton* submitBtn = new QPushButton("Submit Complaint", newBox);
    submitBtn->setStyleSheet("background-color: #1F6FEB; color: #FFFFFF; font-weight: 700;");
    connect(submitBtn, &QPushButton::clicked, this, &ComplaintDeskWidget::onSubmitComplaint);

    ncl->addRow("Category:", m_newCatCombo);
    ncl->addRow("Accused Name:", m_newAccNameEdit);
    ncl->addRow("Accused Role:", m_newAccRoleCombo);
    ncl->addRow("Bus ID:", m_newBusEdit);
    ncl->addRow("Description:", m_newDescEdit);
    ncl->addRow("", submitBtn);

    bottomLayout->addWidget(detailBox, 4);
    bottomLayout->addWidget(pollBox, 3);
    bottomLayout->addWidget(newBox, 3);
    layout->addLayout(bottomLayout, 2);
}

void ComplaintDeskWidget::refreshData() {
    m_compManager.reload();
    const auto& all = m_compManager.getComplaints();
    m_table->setRowCount(0);

    QString catFilter = m_filterCat->currentText().section(": ", 1, 1);
    QString repFilter = m_filterRepRole->currentText().section(": ", 1, 1);
    QString accFilter = m_filterAccRole->currentText().section(": ", 1, 1);
    QString statFilter = m_filterStatus->currentText().section(": ", 1, 1);

    int row = 0;
    for (const auto& c : all) {
        if (catFilter != "All" && QString::fromStdString(c.getCategory()) != catFilter) continue;
        if (repFilter != "All" && QString::fromStdString(c.getReporterRole()) != repFilter) continue;
        if (accFilter != "All" && QString::fromStdString(c.getAccusedRole()) != accFilter) continue;
        if (statFilter != "All" && QString::fromStdString(c.getStatus()) != statFilter) continue;

        m_table->insertRow(row);
        m_table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(c.getId())));
        m_table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(c.getCategory())));
        m_table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(c.getReporterName()) + " (" + QString::fromStdString(c.getReporterRole()) + ")"));
        m_table->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(c.getAccusedName()) + " (" + QString::fromStdString(c.getAccusedRole()) + ")"));
        m_table->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(c.getBusId())));
        m_table->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(c.getDate())));
        m_table->setItem(row, 6, new QTableWidgetItem(QString::fromStdString(c.getTime())));

        QTableWidgetItem* statItem = new QTableWidgetItem(QString::fromStdString(c.getStatus()));
        if (c.getStatus() == "RESOLVED") statItem->setForeground(QColor("#3FB950"));
        else if (c.getStatus() == "POLL ACTIVE") statItem->setForeground(QColor("#BC8CFF"));
        else if (c.getStatus() == "UNDER INVESTIGATION") statItem->setForeground(QColor("#D29922"));
        else statItem->setForeground(QColor("#58A6FF"));
        statItem->setFont(QFont(statItem->font().family(), -1, QFont::Bold));
        m_table->setItem(row, 7, statItem);
        row++;
    }

    if (!m_selectedComplaintId.empty()) {
        updatePollUI(m_selectedComplaintId);
    }
}

void ComplaintDeskWidget::onFilterChanged() {
    refreshData();
}

void ComplaintDeskWidget::onTableSelectionChanged() {
    auto items = m_table->selectedItems();
    if (items.isEmpty()) return;
    int row = items.first()->row();
    m_selectedComplaintId = m_table->item(row, 0)->text().toStdString();
    updatePollUI(m_selectedComplaintId);
}

void ComplaintDeskWidget::updatePollUI(const std::string& compId) {
    const auto& all = m_compManager.getComplaints();
    const Complaint* target = nullptr;
    for (const auto& c : all) {
        if (c.getId() == compId) { target = &c; break; }
    }
    if (!target) return;

    m_detailHeader->setText(QString("Incident #%1 • %2 [%3]")
                            .arg(QString::fromStdString(target->getId()))
                            .arg(QString::fromStdString(target->getCategory()))
                            .arg(QString::fromStdString(target->getStatus())));

    QString adminNotesStr = QString::fromStdString(target->getAdminNotes());
    QString formattedHistory = adminNotesStr.isEmpty() ? "No investigation notes recorded yet." : adminNotesStr;
    formattedHistory.replace(" | ", "<br>• ");
    if (!adminNotesStr.isEmpty()) formattedHistory.prepend("• ");

    m_detailBody->setText(QString("<b>Reported By:</b> %1 (%2)  |  <b>Accused:</b> %3 (%4)<br>"
                                  "<b>Bus:</b> %5  |  <b>Timestamp:</b> %6 %7<br><br>"
                                  "<b>Incident Details:</b><br>%8<br><br>"
                                  "<b>Investigation & Resolution History:</b><br><font color='#58A6FF'>%9</font>")
                          .arg(QString::fromStdString(target->getReporterName()))
                          .arg(QString::fromStdString(target->getReporterRole()))
                          .arg(QString::fromStdString(target->getAccusedName()))
                          .arg(QString::fromStdString(target->getAccusedRole()))
                          .arg(QString::fromStdString(target->getBusId()))
                          .arg(QString::fromStdString(target->getDate()))
                          .arg(QString::fromStdString(target->getTime()))
                          .arg(QString::fromStdString(target->getDescription()))
                          .arg(formattedHistory));

    // Check poll status
    const Poll* poll = m_compManager.findPollForComplaint(compId);
    if (poll && poll->isActive()) {
        m_pollQuestion->setText(QString::fromStdString(poll->getQuestion()));
        m_pollStats->setText(QString("Total Votes: <b>%1</b> | YES: <b>%2 (%3%)</b> | NO: <b>%4 (%5%)</b>")
                             .arg(poll->getTotalVotes())
                             .arg(poll->getYesVotes())
                             .arg(poll->getYesPercentage())
                             .arg(poll->getNoVotes())
                             .arg(poll->getNoPercentage()));
        m_pollProgressBar->setValue(poll->getYesPercentage());

        if (poll->hasVoted(m_activeUsername)) {
            m_pollMessage->setText("You have already cast your confidential vote.");
            m_voteYesBtn->setEnabled(false);
            m_voteNoBtn->setEnabled(false);
        } else if (m_activeUsername == poll->getComplainantRoll() || m_activeUsername == poll->getAccusedRoll()) {
            m_pollMessage->setText("Voting Disabled: Disciplinary rules prohibit involved parties from voting.");
            m_voteYesBtn->setEnabled(false);
            m_voteNoBtn->setEnabled(false);
        } else {
            m_pollMessage->setText("• Please cast your confidential peer vote.");
            m_voteYesBtn->setEnabled(true);
            m_voteNoBtn->setEnabled(true);
        }
    } else {
        m_pollQuestion->setText("No active student peer poll for this incident.");
        m_pollStats->setText("Total Votes: N/A");
        m_pollProgressBar->setValue(0);
        m_pollMessage->clear();
        m_voteYesBtn->setEnabled(false);
        m_voteNoBtn->setEnabled(false);
    }
}

void ComplaintDeskWidget::onInvestigate() {
    if (m_selectedComplaintId.empty()) return;
    bool ok;
    QString note = QInputDialog::getText(this, "Investigate Incident", "Investigation notes / CCTV inquiry details:", QLineEdit::Normal, "CCTV review underway.", &ok);
    if (ok && !note.trimmed().isEmpty()) {
        m_compManager.investigateComplaint(m_selectedComplaintId, note.trimmed().toStdString(), m_activeUsername);
        refreshData();
    }
}

void ComplaintDeskWidget::onResolve() {
    if (m_selectedComplaintId.empty()) return;
    bool ok;
    QString note = QInputDialog::getText(this, "Resolve Incident", "Disciplinary resolution directive:", QLineEdit::Normal, "Incident resolved by Proctorial Board.", &ok);
    if (ok && !note.trimmed().isEmpty()) {
        m_compManager.resolveComplaint(m_selectedComplaintId, note.trimmed().toStdString(), m_activeUsername);
        refreshData();
    }
}

void ComplaintDeskWidget::onReject() {
    if (m_selectedComplaintId.empty()) return;
    bool ok;
    QString note = QInputDialog::getText(this, "Reject Incident", "Mandatory reason for dismissal / rejection:", QLineEdit::Normal, "", &ok);
    if (ok) {
        if (note.trimmed().isEmpty()) {
            QMessageBox::warning(this, "Reason Required", "A mandatory reason must be provided to dismiss or reject a complaint.");
            return;
        }
        m_compManager.rejectComplaint(m_selectedComplaintId, note.trimmed().toStdString(), m_activeUsername);
        refreshData();
    }
}

void ComplaintDeskWidget::onVote(bool isYes) {
    if (m_compManager.castPollVote(m_selectedComplaintId, m_activeUsername, isYes)) {
        QMessageBox::information(this, "Vote Recorded", "Your confidential vote has been recorded.");
        refreshData();
    } else {
        QMessageBox::warning(this, "Voting Denied", "Unable to cast vote: Either already voted or disciplinary rules restrict your account.");
    }
}

void ComplaintDeskWidget::onSubmitComplaint() {
    QString cat = m_newCatCombo->currentText();
    QString accName = m_newAccNameEdit->text().trimmed();
    QString accRole = m_newAccRoleCombo->currentText();
    QString bus = m_newBusEdit->text().trimmed();
    QString desc = m_newDescEdit->text().trimmed();

    if (accName.isEmpty() || desc.isEmpty()) {
        QMessageBox::warning(this, "Input Required", "Accused person name and incident description are mandatory.");
        return;
    }

    std::string repRole = "Student";
    if (m_activeRole == UserRole::ADMIN) repRole = "Admin";
    else if (m_activeRole == UserRole::DRIVER) repRole = "Driver";
    else if (m_activeRole == UserRole::PARENT) repRole = "Parent";

    std::string newId = m_compManager.submitComplaint(
        cat.toStdString(), desc.toStdString(),
        m_activeUsername, repRole,
        accName.toStdString(), accRole.toStdString(),
        bus.toStdString()
    );

    QMessageBox::information(this, "Complaint Registered", "Incident record " + QString::fromStdString(newId) + " registered.");
    m_newDescEdit->clear();
    refreshData();
}
