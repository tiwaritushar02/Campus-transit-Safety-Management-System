#ifndef COMPLAINT_DESK_WIDGET_HPP
#define COMPLAINT_DESK_WIDGET_HPP

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include "ComplaintManager.hpp"
#include "User.hpp"

// Inheritance
class ComplaintDeskWidget : public QWidget {
    Q_OBJECT

private:
    // Composition
    ComplaintManager& m_compManager;
    std::string m_activeUsername;
    UserRole m_activeRole;

    QComboBox* m_filterCat;
    QComboBox* m_filterRepRole;
    QComboBox* m_filterAccRole;
    QComboBox* m_filterStatus;

    QTableWidget* m_table;

    // Inspection Box
    QLabel* m_detailHeader;
    QLabel* m_detailBody;
    QPushButton* m_investigateBtn;
    QPushButton* m_resolveBtn;
    QPushButton* m_rejectBtn;
    std::string m_selectedComplaintId;

    // Student Poll Box
    QLabel* m_pollQuestion;
    QLabel* m_pollStats;
    QProgressBar* m_pollProgressBar;
    QPushButton* m_voteYesBtn;
    QPushButton* m_voteNoBtn;
    QLabel* m_pollMessage;

    // New Complaint Form
    QComboBox* m_newCatCombo;
    QLineEdit* m_newAccNameEdit;
    QComboBox* m_newAccRoleCombo;
    QLineEdit* m_newBusEdit;
    QLineEdit* m_newDescEdit;

    void setupUI();
    void updatePollUI(const std::string& compId);

public:
    // Constructor
    explicit ComplaintDeskWidget(ComplaintManager& cm, QWidget* parent = nullptr);

    void setActiveUser(const std::string& username, UserRole role);
    void refreshData();

private slots:
    void onFilterChanged();
    void onTableSelectionChanged();
    void onInvestigate();
    void onResolve();
    void onReject();
    void onVote(bool isYes);
    void onSubmitComplaint();
};

#endif // COMPLAINT_DESK_WIDGET_HPP
