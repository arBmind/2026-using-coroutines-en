#pragma once

#include <bit>
#include <windows.h>

template<class Parameter>
struct UniqueCallbackAdapter {
    template<class Functor>
    explicit UniqueCallbackAdapter(Functor &&) {}
    auto c_callback() const -> PAPCFUNC { return nullptr; }
    auto c_parameter() const -> Parameter { return {}; }
    void release() {}
};
