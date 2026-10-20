/**
 * ============================================================================
 * MODULE: Authentication & Session Management
 * FILE: AuthManager.cpp
 * ============================================================================
 */

#include "AuthManager.hpp"
#include <iomanip>

using namespace std;

void AuthManager::registerAccount(const UserAccount& account) {
    accounts.push_back(account);
}

UserSession AuthManager::login(const string& username, const string& password) {
    for (auto& acc : accounts) {
        if (acc.getUsername() == username && acc.validatePassword(password)) {
            currentSession.activeAccount = &acc;
            currentSession.isAuthenticated = true;
            cout << "🔑 [AUTH SUCCESS] Welcome, " << acc.getUsername() 
                      << "! Logged in as Role: " << acc.getRoleString() << "\n";
            return currentSession;
        }
    }

    currentSession.activeAccount = nullptr;
    currentSession.isAuthenticated = false;
    cout << "❌ [AUTH FAILED] Invalid Username or Password.\n";
    return currentSession;
}

void AuthManager::logout() {
    if (currentSession.isAuthenticated && currentSession.activeAccount) {
        cout << "🔒 [LOGOUT] User " << currentSession.activeAccount->getUsername() << " logged out.\n";
    }
    currentSession.activeAccount = nullptr;
    currentSession.isAuthenticated = false;
}

void AuthManager::displayRegisteredAccounts() const {
    cout << "\n===========================================================\n";
    cout << "             REGISTERED USER ACCOUNTS LOG                  \n";
    cout << "===========================================================\n";
    cout << left << setw(20) << "USERNAME"
              << setw(15) << "ROLE"
              << "PERSON DETAILS\n";
    cout << "-----------------------------------------------------------\n";

    for (const auto& acc : accounts) {
        cout << left << setw(20) << acc.getUsername()
                  << setw(15) << acc.getRoleString()
                  << (acc.getPersonDetails() ? acc.getPersonDetails()->getName() : "N/A") << "\n";
    }
    cout << "===========================================================\n\n";
}
