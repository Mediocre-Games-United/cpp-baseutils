#pragma once

#include <vector>

namespace cbu {
    template<typename T>
    inline int vector_get_value_index(std::vector<T> &v, T value) {
        int index = -1;
        for (size_t i = 0; i < v.size(); i++) {
            T el = v.at(i);
            if (el != value) continue;

            index = i;
            break;
        }
        return index;
    }
    template<typename T>
    inline bool vector_has_value(std::vector<T> &v, T value) {
        int index = vector_get_value_index(v,value);

        return index >= 0;
    }
    template<typename T>
    inline void vector_erase_index(std::vector<T> &v, size_t index) {
        v.erase(v.begin() + index);
    }
    template<typename T>
    inline void vector_erase_value(std::vector<T> &v, T value) {
        int index = vector_get_value_index(v,value);
        if (index < 0) return;

        vector_erase_index(v,index);
    }

    template <class T>
    inline void vector_shift_back(std::vector<T>& v, std::size_t n) {
        if (n == 0) return;
        if (n >= v.size()) {
            v.clear();
            return;
        }
        std::move(v.begin() + n, v.end(), v.begin());
        v.resize(v.size() - n);
    }
}
