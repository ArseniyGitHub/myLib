#pragma once
#include <functional>
#include <stdexcept>
#define GETBIT (n, b) (n >> (b - 1)) % 2
#define CALL(func, ...) func(__VA_ARGS__)

namespace LIB {
    template <typename rt, typename... args> using fn = rt(*)(args...);

    /*
    template <typename nT> struct __num__ {
        nT elem;
        __num__(nT from) : elem(from) { ; }
        operator nT& () { return elem; }
        
    };
    */

    typedef unsigned __int64 ui64;
    typedef unsigned __int32 ui32;
    typedef unsigned __int16 ui16;
    typedef unsigned __int8   ui8;
    typedef signed __int64    i64;
    typedef signed __int32    i32;
    typedef signed __int16    i16;
    typedef signed __int8      i8;
    typedef           double    d;
    typedef    long   double   ld;
    typedef           float     f;

    void* getMemory(size_t blockSize) { return ::operator new(blockSize); }
    struct _dynamicMemoryObject {
        std::function<void()> create;
        std::function<void()> del;
        void* block = nullptr;
        void deleteObject() { 
            if (del != nullptr) {
                if (block != nullptr) del();
            }
            else ::operator delete(block);
            block = nullptr; 
        }

        template <typename T>
        void createObject() {
            deleteObject();
            create = [this]() { block = new T; };
            del = [this]() { delete (T*)block; };
            create();
        }

        template <typename T, typename argT = T>
        void set(const argT& from) { 
            if (block == nullptr) createObject<T>();
            *(T*)block = from;
        }

        template <typename T>
        void createObject(const T& from) {
            deleteObject();
            createObject<T>();
            set<T, T>(from);
        }
        void createObjectLast() { if (create != nullptr) create(); }

        template <typename T>
        T& get() { 
            if (block != nullptr) return *(T*)block;
            else std::runtime_error("class _dynamicMemoryObject, cant give NULL object!");
        }
    };
}
using namespace LIB;