//
// Created by bla on 20/07/20.
//

#include "exec.hpp"
#include <cstdlib>
#include <array>
#include <iostream>

Exec::Exec() {
}

Exec::~Exec() {
}

Exec::Exec(const Exec &other) : value{other.value}, result{other.result}, return_code{other.return_code} {
}

Exec::Exec(Exec &&other) : value{std::move(other.value)}, result{std::move(other.result)}, return_code{other.return_code} {
}

Exec &Exec::operator=(const Exec &other) {

    if (this == &other)
        return *this;

    this->value = other.value;
    this->result = other.result;
    this->return_code = other.return_code;

    return *this;
}

Exec &Exec::operator=(Exec &&other) noexcept(false) {

    this->value = std::move(other.value);
    this->result = std::move(other.result);
    this->return_code = (other.return_code);

    return *this;
}

Exec::operator std::string() const {
    return result.c_str();
}

Exec::operator int() const {
    return return_code;
}

Exec &Exec::operator=(const std::string &other) {
    Exec::value = other;
    Exec::result = "";
    Exec::return_code = 0;
    return *this;
}

Exec &Exec::operator=(std::string &&other) noexcept(false) {
    Exec::value = std::move(other);
    Exec::result = "";
    Exec::return_code = 0;
    return *this;
}

int Exec::exec() {

    if (value.empty()) {
        return_code = -1;
        result = "";
    }

    std::array<char, 128> buffer{};
    std::stringbuf strBuffer;
    std::ostream os(&strBuffer);
    std::string go(value);
    //go.append(" 2>&1");
    auto X = popen(go.c_str(), "r");
    while (fgets(buffer.data(), 128, X) != NULL) {
        os << buffer.data();
    }

    auto fc = fclose(X);

    return_code = WEXITSTATUS(fc);

    result = strBuffer.str();

    return return_code;
}




