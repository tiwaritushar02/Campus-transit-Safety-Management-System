#include "LoginWidget.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFrame>
#include <QPainter>
#include <QPixmap>
#include <QLinearGradient>
#include <QMessageBox>

// Constructor
LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent), m_passwordVisible(false) {
    setupUI();
}

void LoginWidget::setupUI() {
    QHBoxLayout* rootLayout = new QHBoxLayout(this);
    rootLayout->setContentsMargins(60, 40, 60, 40);
    rootLayout->setSpacing(40);

    // ========================================================
    // LEFT HERO BRANDING SECTION (Responsive Typography)
    // ========================================================
    QVBoxLayout* leftBrandLayout = new QVBoxLayout();
    leftBrandLayout->setSpacing(12);

    QLabel* badge = new QLabel("OFFICIAL TRANSPORT LOGISTICS", this);
    badge->setStyleSheet(
        "background-color: rgba(14, 165, 233, 0.15); "
        "border: 1px solid #0EA5E9; color: #38BDF8; "
        "font-size: 11px; font-weight: 700; padding: 5px 14px; "
        "border-radius: 12px; letter-spacing: 1.5px;"
    );
    badge->setFixedWidth(240);

    QLabel* heroTitle = new QLabel("Graphic Era Hill University\nBhimtal Campus", this);
    heroTitle->setStyleSheet(
        "font-size: 32px; font-weight: 800; color: #FFFFFF; "
        "line-height: 1.2; letter-spacing: -0.6px;"
    );

    QLabel* heroSub = new QLabel("Smart College Bus Fleet & Mountain Corridor Transit System", this);
    heroSub->setStyleSheet("font-size: 15px; font-weight: 600; color: #58A6FF; margin-bottom: 6px;");

    QLabel* heroDesc = new QLabel(
        "Real-time GPS bus fleet monitoring, automated digital student passes, "
        "and topographic route optimization across Haldwani, Kathgodam, Ranibagh, and Bhimtal.",
        this
    );
    heroDesc->setWordWrap(true);
    heroDesc->setStyleSheet("font-size: 13px; color: #94A3B8; line-height: 1.5; max-width: 520px;");

    leftBrandLayout->addStretch(2);
    leftBrandLayout->addWidget(badge);
    leftBrandLayout->addWidget(heroTitle);
    leftBrandLayout->addWidget(heroSub);
    leftBrandLayout->addWidget(heroDesc);
    leftBrandLayout->addStretch(1);

    rootLayout->addLayout(leftBrandLayout, 3);

    // ========================================================
    // RIGHT DARK NAVY GLASSMORPHISM LOGIN PANEL
    // ========================================================
    QFrame* card = new QFrame(this);
    card->setObjectName("loginCard");
    card->setFixedWidth(440);
    card->setStyleSheet(
        "#loginCard {"
        "  background-color: rgba(13, 21, 37, 0.85);"
        "  border: 1px solid rgba(56, 189, 248, 0.35);"
        "  border-radius: 16px;"
        "  padding: 26px;"
        "}"
    );

    QVBoxLayout* cardLayout = new QVBoxLayout(card);
    cardLayout->setSpacing(12);

    // Header inside Card
    QLabel* cardBadge = new QLabel("CAMPUS FLEET TRANSIT", card);
    cardBadge->setStyleSheet("color: #38BDF8; font-size: 11px; font-weight: 800; letter-spacing: 1.5px;");

    QLabel* cardTitle = new QLabel("Sign In to Portal", card);
    cardTitle->setStyleSheet("color: #FFFFFF; font-size: 20px; font-weight: 800; letter-spacing: -0.3px;");

    QLabel* cardSub = new QLabel("Select authorized role and enter college credentials", card);
    cardSub->setStyleSheet("color: #8B949E; font-size: 11.5px; margin-bottom: 6px;");

    cardLayout->addWidget(cardBadge);
    cardLayout->addWidget(cardTitle);
    cardLayout->addWidget(cardSub);

    // Form
    QFormLayout* form = new QFormLayout();
    form->setSpacing(10);

    // 1. Role Selection
    m_roleCombo = new QComboBox(card);
    m_roleCombo->setStyleSheet(
        "QComboBox { background-color: #0E1626; border: 1px solid rgba(56, 189, 248, 0.3); border-radius: 8px; color: #E6EDF3; padding: 8px 12px; font-size: 12.5px; font-weight: 600; }"
        "QComboBox:focus { border: 1.5px solid #38BDF8; }"
        "QComboBox::drop-down { border: none; padding-right: 8px; }"
        "QComboBox QAbstractItemView { background-color: #0E1626; color: #E6EDF3; selection-background-color: #0284C7; selection-color: #FFFFFF; border: 1px solid #38BDF8; }"
    );
    m_roleCombo->addItem("Administrator", static_cast<int>(UserRole::ADMIN));
    m_roleCombo->addItem("Student", static_cast<int>(UserRole::STUDENT));
    m_roleCombo->addItem("Driver", static_cast<int>(UserRole::DRIVER));
    m_roleCombo->addItem("Parent", static_cast<int>(UserRole::PARENT));

    // 2. User ID / Roll Number
    m_usernameInput = new QLineEdit("admin", card);
    m_usernameInput->setPlaceholderText("Enter username, Driver ID or Roll Number");
    m_usernameInput->setStyleSheet(
        "QLineEdit { background-color: #0A101D; border: 1px solid rgba(56, 189, 248, 0.25); border-radius: 8px; color: #FFFFFF; padding: 10px 12px; font-size: 13px; }"
        "QLineEdit:focus { border: 1.5px solid #38BDF8; background-color: #0E172A; }"
    );

    // 3. Password with Show/Hide Toggle
    QWidget* passContainer = new QWidget(card);
    QHBoxLayout* passLayout = new QHBoxLayout(passContainer);
    passLayout->setContentsMargins(0, 0, 0, 0);
    passLayout->setSpacing(6);

    m_passwordInput = new QLineEdit("admin123", passContainer);
    m_passwordInput->setEchoMode(QLineEdit::Password);
    m_passwordInput->setPlaceholderText("Enter secure password");
    m_passwordInput->setStyleSheet(
        "QLineEdit { background-color: #0A101D; border: 1px solid rgba(56, 189, 248, 0.25); border-radius: 8px; color: #FFFFFF; padding: 10px 12px; font-size: 13px; }"
        "QLineEdit:focus { border: 1.5px solid #38BDF8; background-color: #0E172A; }"
    );

    m_togglePasswordBtn = new QPushButton("Show", passContainer);
    m_togglePasswordBtn->setFixedWidth(56);
    m_togglePasswordBtn->setStyleSheet("background-color: #162032; border: 1px solid rgba(56, 189, 248, 0.3); border-radius: 6px; color: #38BDF8; font-size: 11.5px; font-weight: 600; padding: 6px;");
    connect(m_togglePasswordBtn, &QPushButton::clicked, this, &LoginWidget::onTogglePassword);

    passLayout->addWidget(m_passwordInput, 1);
    passLayout->addWidget(m_togglePasswordBtn);

    form->addRow("Select Role:", m_roleCombo);
    form->addRow("User ID / Roll:", m_usernameInput);
    form->addRow("Password:", passContainer);

    cardLayout->addLayout(form);

    // 4. Remember Me & Forgot Password Row
    QHBoxLayout* auxLayout = new QHBoxLayout();
    m_rememberMeCheck = new QCheckBox("Remember Me", card);
    m_rememberMeCheck->setChecked(true);
    m_rememberMeCheck->setStyleSheet(
        "QCheckBox { color: #94A3B8; font-size: 11.5px; font-weight: 500; }"
        "QCheckBox::indicator { width: 15px; height: 15px; border-radius: 3px; border: 1px solid #38BDF8; background-color: #0A101D; }"
        "QCheckBox::indicator:checked { background-color: #0284C7; }"
    );

    m_forgotPasswordBtn = new QPushButton("Forgot Password?", card);
    m_forgotPasswordBtn->setStyleSheet("background: transparent; border: none; color: #38BDF8; font-size: 11.5px; font-weight: 600; text-decoration: underline;");
    connect(m_forgotPasswordBtn, &QPushButton::clicked, this, &LoginWidget::onForgotPassword);

    auxLayout->addWidget(m_rememberMeCheck);
    auxLayout->addStretch();
    auxLayout->addWidget(m_forgotPasswordBtn);
    cardLayout->addLayout(auxLayout);

    // Error label
    m_errorLabel = new QLabel("", card);
    m_errorLabel->setAlignment(Qt::AlignCenter);
    m_errorLabel->setStyleSheet("color: #F85149; font-size: 11.5px; font-weight: 600; min-height: 18px;");
    cardLayout->addWidget(m_errorLabel);

    // 5. Glowing Cyan-Blue Sign In Button
    m_loginBtn = new QPushButton("Sign In to Portal", card);
    m_loginBtn->setObjectName("loginBtn");
    m_loginBtn->setStyleSheet(
        "QPushButton#loginBtn {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0284C7, stop:0.5 #0EA5E9, stop:1 #38BDF8);"
        "  color: #FFFFFF;"
        "  font-weight: 800;"
        "  font-size: 14px;"
        "  padding: 11px;"
        "  border-radius: 8px;"
        "  border: 1px solid #7DD3FC;"
        "}"
        "QPushButton#loginBtn:hover {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0EA5E9, stop:1 #7DD3FC);"
        "  color: #0A101D;"
        "  border: 1.5px solid #BAE6FD;"
        "}"
        "QPushButton#loginBtn:pressed {"
        "  background-color: #0369A1;"
        "}"
    );
    connect(m_loginBtn, &QPushButton::clicked, this, &LoginWidget::onLoginClicked);
    cardLayout->addWidget(m_loginBtn);

    // 6. Controlled Demo Auto-Fill (Dev Mode)
    QLabel* demoLabel = new QLabel("Quick Demo Credentials Selector:", card);
    demoLabel->setStyleSheet("font-size: 10.5px; color: #8B949E; margin-top: 4px;");

    m_demoAccountCombo = new QComboBox(card);
    m_demoAccountCombo->setStyleSheet(
        "QComboBox { background-color: #10192A; border: 1px solid #23334D; border-radius: 6px; color: #94A3B8; font-size: 11px; padding: 6px 10px; }"
        "QComboBox QAbstractItemView { background-color: #10192A; color: #E6EDF3; selection-background-color: #0284C7; border: 1px solid #38BDF8; }"
    );
    m_demoAccountCombo->addItem("-- Select Controlled Demo Profile --");
    m_demoAccountCombo->addItem("Admin (College Transport Head)");
    m_demoAccountCombo->addItem("Student: Aarav Sharma (2024-CS-001)");
    m_demoAccountCombo->addItem("Driver: Rajesh Kumar (D-01)");
    m_demoAccountCombo->addItem("Parent: Vijay Sharma (P-101)");
    connect(m_demoAccountCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &LoginWidget::onDemoAccountSelected);

    cardLayout->addWidget(demoLabel);
    cardLayout->addWidget(m_demoAccountCombo);

    rootLayout->addWidget(card, 0, Qt::AlignRight | Qt::AlignVCenter);
}

void LoginWidget::onTogglePassword() {
    m_passwordVisible = !m_passwordVisible;
    m_passwordInput->setEchoMode(m_passwordVisible ? QLineEdit::Normal : QLineEdit::Password);
    m_togglePasswordBtn->setText(m_passwordVisible ? "Hide" : "Show");
}

void LoginWidget::onForgotPassword() {
    QMessageBox::information(
        this,
        "Transport Portal Password Support",
        "Official Credential Recovery Notice:\n\n"
        "For security compliance, user credential resets must be verified "
        "by the Graphic Era Hill University Transport Office or IT Helpdesk.\n\n"
        "Office: Room 104, Transport Control Cell, Admin Block, Bhimtal Campus\n"
        "Email: transport@gehu.ac.in | it-support@gehu.ac.in\n"
        "Phone: +91-5942-247000 (Ext: 104)"
    );
}

void LoginWidget::onDemoAccountSelected(int index) {
    if (index == 1) { // Admin
        m_roleCombo->setCurrentIndex(0);
        m_usernameInput->setText("admin");
        m_passwordInput->setText("admin123");
    } else if (index == 2) { // Student
        m_roleCombo->setCurrentIndex(1);
        m_usernameInput->setText("2024-CS-001");
        m_passwordInput->setText("student123");
    } else if (index == 3) { // Driver
        m_roleCombo->setCurrentIndex(2);
        m_usernameInput->setText("D-01");
        m_passwordInput->setText("driver123");
    } else if (index == 4) { // Parent
        m_roleCombo->setCurrentIndex(3);
        m_usernameInput->setText("P-101");
        m_passwordInput->setText("parent123");
    }
    m_errorLabel->clear();
}

void LoginWidget::onLoginClicked() {
    QString user = m_usernameInput->text().trimmed();
    QString pass = m_passwordInput->text();
    UserRole role = static_cast<UserRole>(m_roleCombo->currentData().toInt());

    if (user.isEmpty() || pass.isEmpty()) {
        setErrorMessage("Please enter both username and password.");
        return;
    }

    emit loginRequested(user, pass, role);
}

void LoginWidget::setErrorMessage(const QString& msg) {
    m_errorLabel->setText(msg);
}

void LoginWidget::clearFields() {
    m_errorLabel->clear();
}

// Function Overriding
// Polymorphism
void LoginWidget::paintEvent(QPaintEvent* event) {
    (void)event;
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // 1. High-Resolution Yellow Bus Depot Background Image
    QPixmap bg("assets/campus_bus_bg.jpg");
    if (!bg.isNull()) {
        p.drawPixmap(rect(), bg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    } else {
        p.fillRect(rect(), QColor("#080D16"));
    }

    // 2. Responsive Dark Navy Glassmorphism Vignette
    // Leaves left side scenic yellow buses bright while darkening right side behind the glassmorphism card
    QLinearGradient vignette(0, 0, width(), 0);
    vignette.setColorAt(0.0, QColor(8, 13, 22, 60));
    vignette.setColorAt(0.40, QColor(8, 13, 22, 100));
    vignette.setColorAt(0.65, QColor(8, 13, 22, 175));
    vignette.setColorAt(1.0, QColor(8, 13, 22, 225));
    p.fillRect(rect(), vignette);
}
