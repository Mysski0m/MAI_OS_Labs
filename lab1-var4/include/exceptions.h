#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

class OsException : public std::runtime_error {
public:
    explicit OsException(const std::string& message) : std::runtime_error(message) {}
};

#endif