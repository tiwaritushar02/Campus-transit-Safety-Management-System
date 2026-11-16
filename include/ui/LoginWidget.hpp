#ifndef LOGIN_WIDGET_HPP
#define LOGIN_WIDGET_HPP

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include "User.hpp"

// Inheritance
class LoginWidget : public QWidget {
    Q_OBJECT

private:
    // Encapsulation
    QComboBox* m_roleCombo;
    QLineEdit* m_usernameInput;
    QLineEdit* m_passwordInput;
    QPushButton* m_loginBtn;
    QPushButton* m_togglePasswordBtn;
    QCheckBox* m_rememberMeCheck;
    QPushButton* m_forgotPasswordBtn;
    QLabel* m_errorLabel;
    QComboBox* m_demoAccountCombo;
    bool m_passwordVisible;

    void setupUI();

public:
    // Constructor
    explicit LoginWidget(QWidget *parent = nullptr);
    void setErrorMessage(const QString& msg);
    void clearFields();

signals:
    void loginRequested(const QString& username, const QString& password, UserRole role);

protected:
    // Function Overriding
    // Polymorphism
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onLoginClicked();
    void onTogglePassword();
    void onDemoAccountSelected(int index);
    void onForgotPassword();
};

#endif // LOGIN_WIDGET_HPP
