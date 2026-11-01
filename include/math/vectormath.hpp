#pragma once

#include "base_types.hpp"
#include <cassert>
#include <functional>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <map>

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
    inline void vector_shift_back(std::vector<T> &v, std::size_t n) {
        if (n == 0) return;
        if (n >= v.size()) {
            v.clear();
            return;
        }
        std::move(v.begin() + n, v.end(), v.begin());
        v.resize(v.size() - n);
    }

    template<typename T>
    inline void vector_erase_index_fast(vector<T> &objects,const size_t i) {
        objects[i] = std::move(objects.back());
        objects.pop_back();
    }

    template<typename obj>
    // Fast pointer unique unordered list,
    class FPUList {
    public:
        inline void insert(obj *n) {
            indexes[n] = objects.size();
            objects.push_back(n);
        }
        inline void remove(obj *r) {
            if (!indexes.contains(r)) return;

            const size_t i = indexes[r];
            indexes[objects.back()] = i;
            indexes.erase(r);

            vector_erase_index_fast(objects,i);

        }
        inline bool has(obj *r) {
            return indexes.contains(r);
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

        vector<obj*> objects{};
        umap<obj*,size_t> indexes{};
    };
    template<typename obj,typename other>
    class FPUListLinked {
    public:
        inline void insert(obj *n,other *o) {
            list.insert(n);
            olist.emplace_back(o);
        }
        inline other *remove(obj *n) {
            if (!list.has(n)) return NULL;
            size_t i = list.indexes[n];

            auto o = olist[i];
            vector_erase_index_fast(olist,i);
            list.remove(n);

            return o;
        }

        inline auto begin() { return olist.begin(); }
        inline auto begin() const { return olist.begin(); }
        inline auto end() { return olist.end(); }
        inline auto end() const { return olist.end(); }
        size_t size() const { return olist.size(); }

        vector<other*> olist{};
        FPUList<obj> list{};
    };
    template<typename obj,typename depth>
    // Fast ordered depth list
    class FODList {
    public:
        inline void insert(obj *n,depth d) {
            objects[d].insert(n);
            indexes[n] = d;
        }
        inline void remove(obj *r) {
            assert(indexes.contains(r));

            depth d = indexes[r];
            auto &ref = objects[d];
            ref.remove(r);
            if (ref.size() > 0) return;
            objects.erase(d);
        }
        inline bool has(obj *r) {
            return indexes.contains(r);
        }
        inline void foreach(std::function<void(obj*)> cb) {
            for (auto &[dp,objects] : objects) {
                for (auto &s : objects) {
                    cb(s);
                }
            }
        }

        std::map<depth,FPUList<obj>> objects{};
        umap<obj*,depth> indexes{};
    };
    template<typename obj,typename other,typename depth>
    class FODListLinked {
    public:
        inline void insert(obj *n,other *o,depth d) {
            list.insert(n,d);
            olist[n] = o;
        }
        inline other *remove(obj *n) {
            if (!list.has(n)) return NULL;

            other *o = olist[n];
            olist.erase(n);
            list.remove(n);
            return o;
        }
        inline void foreach(std::function<void(other*)> cb) {
            list.foreach([&cb,this](obj *o) { cb(olist[o]); });
        }
        inline void foreach_obj(std::function<void(obj*)> cb) {
            list.foreach(cb);
        }



        umap<obj*,other*> olist{};
        FODList<obj,depth> list{};
    };


    inline void draw_line(S64 x0, S64 y0,
                  S64 x1, S64 y1,
                  std::function<void(S64 x,S64 y)> plot)
    {
        using i128 = __int128_t;

        const i128 dx = (x0 < x1)
        ? static_cast<i128>(x1) - x0
        : static_cast<i128>(x0) - x1;
        const i128 dy = (y0 < y1)
        ? static_cast<i128>(y1) - y0
        : static_cast<i128>(y0) - y1;

        const S64 sx = (x0 < x1) ? 1 : -1;
        const S64 sy = (y0 < y1) ? 1 : -1;
        i128 err = dx - dy;

        while (true) {
            plot(x0, y0);

            if (x0 == x1 && y0 == y1)
                break;

            const i128 e2 = 2 * err;

            if (e2 > -dy) {
                err -= dy;
                x0 += sx;
            }
            if (e2 < dx) {
                err += dx;
                y0 += sy;
            }
        }
    }

}
