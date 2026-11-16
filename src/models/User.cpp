#include "User.hpp"

// Constructor
User::User(const std::string& id, const std::string& name, const std::string& phone,
           const std::string& email, const std::string& password, UserRole role)
    : m_userId(id), m_fullName(name), m_phone(phone),
      m_email(email), m_password(password), m_role(role) {}
