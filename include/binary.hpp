#ifndef BINARY_HPP
#define BINARY_HPP

#include "file.hpp"
#include "base_types.hpp"
#include "logger.hpp"
#include "stringmath.hpp"
#include <cstdint>
#include <cstring>
#include <format>

namespace cbu {
    inline uint8_t decode_u8(BYTEARRAY &bytes,size_t offset) {
        if (offset >= bytes.size()) return 0;

        return bytes.at(offset);
    }
    inline int8_t decode_s8(BYTEARRAY &bytes,size_t offset) {
        if (offset + 1 > bytes.size()) return 0;
        int8_t s;
        memcpy(&s,bytes.data() + offset,1);

        return s;
    }
    inline uint16_t decode_u16(BYTEARRAY &bytes,size_t offset) {
        if (offset + 2 > bytes.size()) return 0;
        uint16_t s;
        memcpy(&s,bytes.data() + offset,2);

        return s;
    }
    inline uint32_t decode_u32(BYTEARRAY &bytes,size_t offset) {
        if (offset + 4 > bytes.size()) return 0;
        uint32_t s;
        memcpy(&s,bytes.data() + offset,4);

        return s;
    }
    inline float decode_float(BYTEARRAY &bytes,size_t offset) {
        if (offset + 4 > bytes.size()) return 0;
        float s;
        memcpy(&s,bytes.data() + offset,4);

        return s;
    }
    inline string decode_string(BYTEARRAY &bytes,size_t offset,size_t *len) {
        uint16_t s = decode_u16(bytes,offset);
        offset += 2;
        if (offset + size_t(s) > bytes.size() || s == 0) {*len = 2; return string(""); }

        char *s_bytes = (char*) malloc(sizeof(char) * (s + 1));
        for (uint16_t i = 0; i < s; i ++) {
            s_bytes[i] = bytes[offset + i];
        }
        s_bytes[s] = 0;

        string str = string(s_bytes);
        free(s_bytes);
        *len = s + 2;
        return str;
    }
    inline color_t decode_color(BYTEARRAY &bytes,size_t offset) {
        return color_t(
            decode_float(bytes,offset),
            decode_float(bytes,offset + 4),
            decode_float(bytes,offset + 8),
            decode_float(bytes,offset + 12)
        );
    }
    inline BYTEARRAY decode_bytearray(BYTEARRAY &bytes,size_t offset,size_t *len) {
        uint16_t s = decode_u16(bytes,offset);
        offset += 2;
        if (offset + size_t(s) > bytes.size() || s == 0) {*len = 2; return {}; }

        *len = s + 2;
        BYTEARRAY val;
        val.insert(val.begin(),bytes.begin() + offset,bytes.begin() + offset + s);

        return val;
    }



    inline size_t encode_u8(BYTEARRAY &bytes,uint8_t value) {
        bytes.push_back(value);
        return 1;
    }
    inline size_t encode_s8(BYTEARRAY &bytes,int8_t value) {
        bytes.push_back(value);
        return 1;
    }
    inline size_t encode_u16(BYTEARRAY &bytes,uint16_t value) {
        uint8_t b[2];
        memcpy(&b,&value,2);

        bytes.push_back(b[0]);
        bytes.push_back(b[1]);

        return 2;
    }
    inline size_t encode_u32(BYTEARRAY &bytes,uint32_t value) {
        uint8_t b[4];
        memcpy(&b,&value,4);

        bytes.push_back(b[0]);
        bytes.push_back(b[1]);
        bytes.push_back(b[2]);
        bytes.push_back(b[3]);

        return 4;
    }
    inline size_t encode_float(BYTEARRAY &bytes,float value) {
        uint8_t b[4];
        memcpy(&b,&value,4);

        bytes.push_back(b[0]);
        bytes.push_back(b[1]);
        bytes.push_back(b[2]);
        bytes.push_back(b[3]);

        return 4;
    }
    inline size_t encode_string(BYTEARRAY &bytes,string value) {
        BYTEARRAY val;
        uint16_t l = value.size();
        val.resize(l + 2);
        memcpy(val.data(),&l,2);
        memcpy(val.data() + 2,value.data(),l);

        bytes.insert(bytes.end(),val.begin(),val.end());

        return l + 2;
    }
    inline size_t encode_color(BYTEARRAY &bytes,color_t value) {
        encode_float(bytes,value.r);
        encode_float(bytes,value.g);
        encode_float(bytes,value.b);
        encode_float(bytes,value.a);

        return 16;
    }
    inline size_t encode_bytearray(BYTEARRAY &bytes,BYTEARRAY value) {
        encode_u16(bytes,value.size());

        bytes.insert(bytes.end(),value.begin(),value.end());
        return value.size() + 2;
    }



    inline string bytes_to_string_logger(void *b,size_t len) {
        string txt = "0000: ";
        txt.reserve(len * 5);
        size_t row = 0;
        size_t row_index = 0;
        uint8_t byte;
        char bfr[5];
        for (size_t i = 0; i < len; i ++) {
            memcpy(&byte,((uint8_t*) b) + i,1);
            sprintf(bfr,"%02X ",byte);
            txt = txt + bfr;

            row_index += 1;
            if (row_index < 16) continue;
            row_index -= 16;
            row += 1;
            sprintf(bfr,"%04X",uint32_t(row));
            txt = std::format("{}\n{}: ",txt,bfr);
        }
        return txt;
    }
    inline string bytes_to_string(void *b,size_t len) {
        string txt;
        txt.reserve(len * 2);
        uint8_t byte;
        char bfr[5];
        for (size_t i = 0; i < len; i ++) {
            memcpy(&byte,((uint8_t*) b) + i,1);
            sprintf(bfr,"%02X",byte);
            txt = txt + bfr;
        }

        return txt;
    }
    inline BYTEARRAY string_to_bytes(string hex) {
        BYTEARRAY bytes;
        for (size_t i = 0; i < hex.size(); i += 2) {
            string byte = hex.substr(i,2);

            bytes.push_back(static_cast<uint8_t>(stoi(byte,NULL,16)));
        }

        return bytes;
    }

    class BinaryFileSection {
    public:
        BinaryFileSection(std::vector<BinaryFileSection*> sub) : sub_sections(sub) {};
        virtual ~BinaryFileSection() {
            for (BinaryFileSection *s : sub_sections) {
                delete s;
            }
        }

        std::vector<BinaryFileSection*> sub_sections;
        virtual bool sub_sections_enabled() { return true; }
        virtual bool data_is_valid(BYTEARRAY &data) { return true; };
        virtual size_t read_from_buffer(void *obj,BYTEARRAY &data,size_t offset) { return 0; };
        virtual void write_to_buffer(void *obj,BYTEARRAY &data) {};
    };
    class MainBinaryFileSection : public BinaryFileSection {
    public:
        MainBinaryFileSection(std::vector<BinaryFileSection*> sub,size_t ms = 0) : BinaryFileSection(sub), min_size(ms) {};

        bool data_is_valid(BYTEARRAY &data) override { return data.size() >= min_size; };
        size_t min_size;
    };
    class RepeatingBinarySection : public BinaryFileSection {
    public:
        RepeatingBinarySection(void *(*w)(void *obj,size_t *size,size_t *len),void (*wd)(void *obj),std::vector<BinaryFileSection*> sub) : BinaryFileSection(sub), write_callback(w), write_del(wd) {};

        void *(*write_callback)(void*,size_t*,size_t*);
        void (*write_del)(void*);
        bool sub_sections_enabled() override { return false; }
        size_t read_from_buffer(void *obj,BYTEARRAY &data,size_t offset) override {
            size_t total = 0;
            while (offset < data.size()) {
                for (BinaryFileSection *s : sub_sections) {
                    size_t size = s->read_from_buffer(obj,data,offset);

                    total += size;
                    offset += size;
                }
            }

            return total;
        }
        void write_to_buffer(void *obj,BYTEARRAY &data) override {
            size_t len = 0;
            size_t size = 0;

            void *list = write_callback(obj,&size,&len);
            void *og = list;

            cbu::log_debug(std::format("Repeating at len {}, size {}",len,size));
            for (size_t i = 0; i < len; i ++) {
                // log_debug(std::format("Repeating page {}",list));
                for (auto s : sub_sections) {
                    s->write_to_buffer(list,data);
                }
                list = (void*) ((char*) list + size);
            }
            cbu::log_debug("Repeating done");
            write_del(og);
            cbu::log_debug("Del done");
        }
    };
    class DataBinarySection : public BinaryFileSection {
    public:
        DataBinarySection(void (*c)(void*,void*),void *(*f)(void),void (*ff)(void *obj),std::vector<BinaryFileSection*> sub) : BinaryFileSection(sub), read_callback(c), factory(f), factory_free(ff) {};

        void (*read_callback)(void*,void*);
        void *(*factory)(void);
        void (*factory_free)(void*);
        bool sub_sections_enabled() override { return false; }
        size_t read_from_buffer(void *obj,BYTEARRAY &data,size_t offset) override {
            size_t size = 0;
            void *f = factory();
            for (BinaryFileSection *s : sub_sections) {
                size += s->read_from_buffer(f,data,offset + size);
            }
            read_callback(obj,f);
            factory_free(f);

            return size;
        }
        void write_to_buffer(void *obj,BYTEARRAY &data) override {
            for (auto s : sub_sections) {
                s->write_to_buffer(obj,data);
            }
        }
    };

    class StringBinarySection : public BinaryFileSection {
    public:
        StringBinarySection(void (*c)(void *object,const char *value),string (*w)(void *obj)) : BinaryFileSection({}), read_callback(c), write_callback(w) {};

        void (*read_callback)(void*,const char*);
        string (*write_callback)(void*);
        size_t read_from_buffer(void *obj,BYTEARRAY &data,size_t offset) override {
            size_t len;
            std::string value = decode_string(data,offset,&len);
            read_callback(obj,value.c_str());

            return len;
        }
        void write_to_buffer(void *obj,BYTEARRAY &data) override {
            std::string str = write_callback(obj);

            encode_string(data,str);
        }
    };
    class BytearrayBinarySection : public BinaryFileSection {
    public:
        BytearrayBinarySection(void (*c)(void *object,BYTEARRAY &value),BYTEARRAY (*w)(void *obj)) : BinaryFileSection({}), read_callback(c), write_callback(w) {};

        void (*read_callback)(void*,BYTEARRAY&);
        BYTEARRAY (*write_callback)(void*);
        size_t read_from_buffer(void *obj,BYTEARRAY &data,size_t offset) override {
            size_t len;
            BYTEARRAY value = decode_bytearray(data,offset,&len);
            read_callback(obj,value);

            return len;
        }
        void write_to_buffer(void *obj,BYTEARRAY &data) override {
            BYTEARRAY bytes = write_callback(obj);
            encode_bytearray(data,bytes);
        }
    };
    class ColorBinarySection : public BinaryFileSection {
    public:
        ColorBinarySection(void (*c)(void *object,color_t &value),color_t (*w)(void *obj)) : BinaryFileSection({}), read_callback(c), write_callback(w) {};

        void (*read_callback)(void*,color_t&);
        color_t (*write_callback)(void*);
        size_t read_from_buffer(void *obj,BYTEARRAY &data,size_t offset) override {
            color_t value = decode_color(data,offset);
            read_callback(obj,value);

            return 16;
        }
        void write_to_buffer(void *obj,BYTEARRAY &data) override {
            color_t bytes = write_callback(obj);
            encode_color(data,bytes);
        }
    };
    class U8BinarySection : public BinaryFileSection {
    public:
        U8BinarySection(void (*c)(void *object,uint8_t value),uint8_t (*w)(void *obj)) : BinaryFileSection({}), read_callback(c), write_callback(w) {};

        void (*read_callback)(void*,uint8_t);
        uint8_t (*write_callback)(void*);
        size_t read_from_buffer(void *obj,BYTEARRAY &data,size_t offset) override {
            uint8_t value = decode_u8(data,offset);
            read_callback(obj,value);

            return 1;
        }
        void write_to_buffer(void *obj,BYTEARRAY &data) override {
            uint8_t i = write_callback(obj);

            encode_u8(data,i);
        }
    };
    class U32BinarySection : public BinaryFileSection {
    public:
        U32BinarySection(void (*c)(void *object,uint32_t value),uint32_t (*w)(void *obj)) : BinaryFileSection({}), read_callback(c), write_callback(w) {};

        void (*read_callback)(void*,uint32_t);
        uint32_t (*write_callback)(void*);
        size_t read_from_buffer(void *obj,BYTEARRAY &data,size_t offset) override {
            uint32_t value = decode_u32(data,offset);
            read_callback(obj,value);

            return 4;
        }
        void write_to_buffer(void *obj,BYTEARRAY &data) override {
            uint32_t i = write_callback(obj);

            encode_u32(data,i);
        }
    };

    class BinaryFileVersion {
    public:
        virtual ~BinaryFileVersion() = default;
        size_t offset = 0;
        void buffer_to_object_recurse(BYTEARRAY &bfr,void *obj,BinaryFileSection *s) {
            if (!s) return;
            offset += s->read_from_buffer(obj,bfr,offset);

            if (!s->sub_sections_enabled()) return;
            for (BinaryFileSection *n : s->sub_sections) {
                buffer_to_object_recurse(bfr,obj,n);
            }
        }
        void object_to_buffer_recurse(BYTEARRAY &bfr,void *obj,BinaryFileSection *s) {
            if (!s) return;
            s->write_to_buffer(obj,bfr);

            if (!s->sub_sections_enabled()) return;
            for (BinaryFileSection *n : s->sub_sections) {
                object_to_buffer_recurse(bfr,obj,n);
            }
        }

        virtual BinaryFileSection *get_sections() { return NULL; };
    };
    class BinaryFileFormat {
    public:
        BinaryFileFormat(std::vector<BinaryFileVersion*> v,const char *e) : versions(v), latest_version(v.size() - 1), ext(cbu::StringName(cbu::string_uppercase(e))) {};
        ~BinaryFileFormat() {
            for (size_t i = 0; i < versions.size(); i ++) {
                delete versions[i];
            }
        }

        std::vector<BinaryFileVersion*> versions;
        uint8_t latest_version;
        // throws string on error
        inline void load_buffer_to_object(void *obj) {
            uint8_t version = decode_u8(obj_data,0);

            if (version > latest_version) version = 0;
            BinaryFileVersion *ver = versions[version];
            BinaryFileSection *sections = ver->get_sections();

            ver->offset = 1;
            if (!sections->data_is_valid(obj_data)) throw -1;
            ver->buffer_to_object_recurse(obj_data,obj,sections);

            delete sections;
        }
        inline void load_object_to_buffer(void *obj) {
            BinaryFileVersion *ver = versions[latest_version];
            BinaryFileSection *sections = ver->get_sections();

            obj_data.clear();
            encode_u8(obj_data,latest_version);

            ver->object_to_buffer_recurse(obj_data,obj,sections);

            delete sections;
        }
        inline void load_file_to_buffer(std::filesystem::path fpath) {
            try {
                obj_data = require_file_binary_path(fpath);
            } catch (...) {
                obj_data.clear();
            }
        }
        inline int save_buffer_to_file(std::filesystem::path fpath) {
            write_to_file_safe_binary(fpath,obj_data);

            return 0;
        }
        cbu::StringName ext;

        BYTEARRAY obj_data;
    };
}

#endif
