#pragma once

#include "base_types.hpp"
#include <cassert>
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

    template<typename obj>
    class FastListPointer {
    public:
        inline void insert(obj *n) {
            indexes[n] = objects.size();
            objects.push_back(n);
        }
        inline void remove(obj *r) {
            assert(indexes.contains(r));

            const size_t i = indexes[r];
            indexes[objects.back()] = i;
            objects[i] = std::move(objects.back());
            indexes.erase(r);
            objects.pop_back();
        }
        inline void reserve(size_t c) {
            objects.reserve(c);
        }

        using iterator = typename vector<obj*>::iterator;
        using const_iterator = typename vector<obj*>::const_iterator;
        inline iterator begin() {
            return objects.begin();
        }

        inline iterator end() {
            return objects.end();
        }

        inline const_iterator begin() const {
            return objects.begin();
        }

        inline const_iterator end() const {
            return objects.end();
        }

        size_t size() const {
            return objects.size();
        }
    private:
        vector<obj*> objects{};
        umap<obj*,size_t> indexes{};
    };
}
