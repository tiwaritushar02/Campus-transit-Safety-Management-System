#ifndef AUTH_MANAGER_HPP
#define AUTH_MANAGER_HPP

#include "FileManager.hpp"
#include "User.hpp"
#include <memory>
#include <vector>

// Encapsulation
class AuthManager {
private:
    // Composition
    FileManager& m_fileManager;
    std::vector<std::shared_ptr<User>> m_users;
    std::shared_ptr<User> m_currentUser;

public:
    // Constructor
    explicit AuthManager(FileManager& fm);

    void reloadUsers();
    std::shared_ptr<User> authenticate(const std::string& username, const std::string& password, UserRole role);
    void logout();

    std::shared_ptr<User> getCurrentUser() const { return m_currentUser; }
    bool isAuthenticated() const { return m_currentUser != nullptr; }
    const std::vector<std::shared_ptr<User>>& getAllUsers() const { return m_users; }
};

#endif // AUTH_MANAGER_HPP
