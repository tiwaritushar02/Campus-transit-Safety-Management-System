/**
 * ============================================================================
 * MODULE: Authentication System
 * FILE: UserAccount.cpp
 * ============================================================================
 */

#include "UserAccount.hpp"

using namespace std;

UserAccount::UserAccount(const string& user, const string& pass, UserRole r, shared_ptr<Person> person)
    : username(user), password(pass), role(r), personDetails(person) {}
