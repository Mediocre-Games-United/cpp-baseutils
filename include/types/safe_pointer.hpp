#pragma once

#include <cstddef>
#include <format>
#include "logger.hpp"
#include <stdexcept>




template<class obj>
class SafePointer {
public:
    SafePointer() = default;

    // Implicit conversion allows: SafePointer<T> p = raw_pointer;
    SafePointer(obj* o) {
        reset(o);
    }

    // Every copy gets its own callback, capturing the new object's this.
    SafePointer(const SafePointer& other) {
        reset(other.get());
    }

    // Moving also reconnects: the existing callback captures the old this.
    SafePointer(SafePointer&& other) {
        reset(other.get());
        other.reset(nullptr);
    }

    ~SafePointer() {
        disconnect();
    }

    SafePointer& operator=(const SafePointer& other) {
        if (this != &other)
            reset(other.get());

        return *this;
    }

    SafePointer& operator=(SafePointer&& other) {
        if (this != &other) {
            reset(other.get());
            other.reset(nullptr);
        }

        return *this;
    }

    // Supports: p = raw_pointer;
    SafePointer& operator=(obj* o) {
        reset(o);
        return *this;
    }

    bool is_valid() const {
        return valid && p != nullptr;
    }

    operator bool() const {
        return is_valid();
    }

    bool operator==(const SafePointer& other) const {
        return get() == other.get();
    }

    bool operator!=(const SafePointer& other) const {
        return !(*this == other);
    }

    // Preserves your existing pointer-returning operator* behavior.
    obj* operator*() const {
        if (!is_valid()) cbu::log_error(true, std::format(
                "Invalid SafePointer access {}", static_cast<void*>(p)));

        return p;
    }

    obj* get() const {
        return is_valid() ? p : nullptr;
    }

    obj* p = nullptr;
private:
    bool valid = false;
    void* c_obj = nullptr;

    void reset(obj* o) {
        if (o == p && is_valid())
            return;

        disconnect();

        if (!o)
            return;

        p = o;
        c_obj = p->destroyed.add_callback([this]() {
            // cbu::log_verbose(std::format(
            //     "Safepointer {} destroyed", static_cast<void*>(p)));

            // The observed object is being destroyed, so don't call
            // remove_callback on its event from inside this callback.
            p = nullptr;
            valid = false;
            c_obj = nullptr;
        });

        valid = true;

        // cbu::log_verbose(std::format(
        //     "Safepointer {} connected", static_cast<void*>(p)));
    }

    void disconnect() {
        if (!valid || !p) {
            valid = false;
            p = nullptr;
            c_obj = nullptr;
            return;
        }

        obj* old_p = p;
        void* old_callback = c_obj;

        // Clear our state before unregistering.
        valid = false;
        p = nullptr;
        c_obj = nullptr;

        // cbu::log_verbose(std::format(
        //     "Safepointer {} disconnected", static_cast<void*>(old_p)));

        old_p->destroyed.remove_callback(old_callback);
    }
};
