#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <functional>
#include <vector>
#include "vectormath.hpp"

template<typename ...T>
struct SocketData {
    func<void(T...)> callback;
};


template<typename ...T>
class BaseSocket {
public:
    inline ~BaseSocket() {
        for (auto s : list) {
            delete s;
        }
    }
    inline void *add_callback(func<void(T...)> callback) {
        list.push_back(new SocketData{callback});

        return (void*) list.back();
    }
    inline void remove_callback(void *obj) {
        cbu::vector_erase_value(list,(SocketData<T...>*) obj);
        delete ((SocketData<T...>*) obj);
    }
    inline void emit(T... v) {
        size_t s = list.size();
        for (size_t i = 0; i < s;i ++) {
            SocketData<T...> *is = list[i];
            is->callback(v...);
        }
    }
private:
    vector<SocketData<T...>*> list;
};
class Socket : public BaseSocket<> {

};

class OrderedSocket {
private:
    vector<func<bool()>> list;
public:
    void add_callback(func<bool()> cb) {
        list.push_back(cb);
    }
    bool emit() {
        for (int i = 0; i < list.size();i ++) {
            bool r = list[i]();

            if (r) return true;
        }
        return false;
    }
};
class TriState {
public:
    using cb = func<void(void*)>;
    using cb2 = func<void(void*,F64)>;
    TriState(
        func<void(void*)> init = NULL,
        func<void(void*,F64)> step = NULL,
        func<void(void*)> final = NULL
    ) {
        init_callback = init;
        step_callback = step;
        final_callback = final;
    }

    inline void init(void *s) {
        if (!init_callback) return;

        init_callback(s);
    }
    inline void final(void *s) {
        if (!final_callback) return;

        final_callback(s);
    }
    inline void step(void *s,F64 dt) {
        if (!step_callback) return;

        step_callback(s,dt);
    }
private:
    func<void(void *s)> init_callback = NULL;
    func<void(void *s,F64 dt)> step_callback = NULL;
    func<void(void *s)> final_callback = NULL;
};
template <typename objtype>
class TriStateTyped : public TriState {
public:

    TriStateTyped(
        func<void(objtype*)> init = NULL,
        func<void(objtype*,F64)> step = NULL,
        func<void(objtype*)> final = NULL
    ) : TriState((init ? cb([init](void *o) {
        init((objtype*) o);
    }) : cb([](void*) {})),cb2(step ? ([step](void *o,F64 dt) {
        step((objtype*) o,dt);
    }) : cb2([](void*,F64) {})),cb(final ? ([final](void *o) {
        final((objtype*) o);
    }) : cb([](void*) {}))) {

    }
};


#endif
