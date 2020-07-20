//
// Created by bla on 20/07/20.
//

#ifndef ADES_CODE_ENGINE_API_FRAMEWORK_EXEC_HPP
#define ADES_CODE_ENGINE_API_FRAMEWORK_EXEC_HPP

#include <fstream>
#include <sstream>
#include <ostream>

class Exec {
public:
    Exec();
    int exec();

    ~Exec();
private:
    std::string value{""};
    std::string result{""};
    int return_code{0};

public:
    Exec(const Exec &other);
    Exec(Exec &&other) noexcept(false);
    Exec &operator=(const Exec &other);
    Exec &operator=(Exec &&other) noexcept(false);

    Exec &operator=(std::string &&other) noexcept(false);
    Exec &operator=(const std::string &other);

    explicit operator std::string() const;
    explicit operator int() const;

    friend std::ostream &operator<<(std::ostream &os, const Exec &type) {
        os << type.result;
        return os;
    }
};

#endif //ADES_CODE_ENGINE_API_FRAMEWORK_EXEC_HPP
