#pragma once

#include <windows.h>

namespace win32 {

struct Handle {
    Handle() = default;
    explicit Handle(HANDLE handle)
        : m_handle{handle} {}
    auto get() const -> HANDLE { return m_handle; }

private:
    HANDLE m_handle{};
};

} // namespace win32
