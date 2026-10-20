#ifndef AUTH_MANAGER_HPP
#define AUTH_MANAGER_HPP

/**
 * ============================================================================
 * MODULE: Authentication & Session Management
 * FILE: AuthManager.hpp
 * 
 * OOP CONCEPTS DEMONSTRATED:
 * 1. CLASS & OBJECT BASED AUTHENTICATION: Manages user accounts and active session objects.
 * 2. SINGLETON / SERVICE MANAGER PATTERN: Encapsulates account creation, authentication,
 *    and logout session workflows.
 * ============================================================================
 */

#include "UserAccount.hpp"
#include <vector>
#include <memory>
#include <iostream>

using namespace std;

struct UserSession {
    UserAccount* activeAccount;
    bool isAuthenticated;

    UserSession() : activeAccount(nullptr), isAuthenticated(false) {}
};

class AuthManager {
private:
    vector<UserAccount> accounts;
    UserSession currentSession;

public:
    AuthManager() = default;

    void registerAccount(const UserAccount& account);
    
    // Authenticates credentials against registered accounts
    UserSession login(const string& username, const string& password);
    
    void logout();

    bool isLoggedIn() const { return currentSession.isAuthenticated; }
    UserSession getCurrentSession() const { return currentSession; }
    UserAccount* getActiveAccount() const { return currentSession.activeAccount; }

    void displayRegisteredAccounts() const;
};

#endif // AUTH_MANAGER_HPP
