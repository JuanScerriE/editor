#pragma once

#include <cstdlib>
#include <string>

enum Status {
    Error = 0,
    Ok = 1,
};

template <typename T>
class Result {
   private:
    T m_value;
    Status m_status;

   public:
    Result(T&& value) : m_value(value), m_status(Ok) {
    }
    Result(Status status) : m_status(status) {
    }
    Result() : m_status(Error) {
    }
    Result(const Result& other) = delete;

    bool ok() const noexcept {
        return m_status == Ok;
    }

    T unwrap() const noexcept {
        if (!ok()) {
            abort();
        }

        return m_value;
    }
};
