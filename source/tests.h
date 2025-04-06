#pragma once
#include "types.h"
#include <iostream>

namespace _tests {
    template <typename _Save_Type = char> class _str {
    private:

        _Save_Type* start = 0;
        LIB::ui64 _Select_Start = 0;
        LIB::ui64 _Select_End = 0;
        _str* _Orig = this;
        LIB::ui64 _size = 0;



        template <typename txtType> void paste(LIB::ui64 _start, LIB::ui64 end, const txtType* text, LIB::ui64 sz) {
            if (_Orig != this) return;

            LIB::ui64 nwSize = _size + sz - (end - _start + 1);
            _Save_Type* nw = new _Save_Type[nwSize];
            LIB::ui64 ind = 0;

            while (ind < _start) {
                if (start[ind] == 0) break;
                nw[ind] = start[ind];
                ind++;
            }

            for (LIB::ui64 i = 0; i < sz; i++) {
                if (text[i] == (const txtType)0) break;
                nw[ind] = (_Save_Type)text[i];
                ind++;
            }

            for (LIB::ui64 i = end + 1; i < _size; i++) {
                if (start[i] == 0) break;
                nw[ind] = start[i];
                ind++;
            }

            delete[] start;
            start = nw;
            _size += sz - (end - _start + 1);
        }

        void _Set_Size(LIB::ui64 sz) {
            if (_Orig->start != 0) {
                delete[] _Orig->start;
            }
            _Orig->start = new _Save_Type[sz + _Select_Start];
            _Orig->_size = sz + _Select_Start;
        }

        long double readNum(LIB::ui64 ind) {

        }

    public:

        long double* numberStart = 0;
        LIB::ui64 numberSize;
        LIB::ui64* numberInd = 0;



        LIB::ui64 size() {
            return _size;
        }

        const _Save_Type* begin() {
            return start;
        }

        const _Save_Type* end() {
            return &(start[_size - 1]);
        }

        _str& operator += (const char* txt) {
            LIB::ui64 sz = 0;
            while (txt[sz] != (const char)0) sz++;
            _Save_Type* nw = new _Save_Type[_size + sz - 1];
            for (LIB::ui64 i = 0; i < _size; i++) {
                if (start[i] == (const _Save_Type)0) break;
                nw[i] = start[i];
            }
            for (LIB::ui64 i = 0; i < sz; i++) {
                if (txt[i] == (const char)0) break;
                nw[i + _size] = (_Save_Type)txt[i];
            }
            _size += sz;
            delete[] start;
            start = nw;

        }

        void paste(LIB::ui64 index, const _str txt) {
            paste<char>(index, index - 1, txt.c_str(), txt._size);
        }

        template <LIB::ui64 sz> void cpy(const _Save_Type(&from)[sz]) {
            cpy(from, sz);
        }

        _Save_Type* getStart() const {
            return start;
        }

        void cpy(const wchar_t* from) {
            LIB::ui64 sz = 0;
            while (from[sz] != (const wchar_t)0) sz++;
            if (_Orig != this) goto _Not_Orig;
            _Set_Size(sz);
            for (LIB::ui64 i = 0; i < sz; i++) {
                start[i] = (_Save_Type)from[i];
            }
            return;
        _Not_Orig:
            _Orig->paste<wchar_t>(_Select_Start, _Select_End, from, sz);
            _Orig->_size += sz - (_Select_End - _Select_Start + 1);
            return;
        }

        _Save_Type& operator [] (LIB::ui64 ind) {
            return start[ind];
        }

        _str<_Save_Type> select(LIB::ui64 start, LIB::ui64 end) {
            _str<_Save_Type> selected;
            selected._Orig = _Orig;
            selected._Select_Start = start + _Select_Start;
            selected._Select_End = end + _Select_Start;
            selected._size = end - start + 1;
            return selected;
        }

        void cpy(const char* from) {
            LIB::ui64 sz = 0;
            while (from[sz] != (const char)0) sz++;
            if (_Orig != this) goto _Not_Orig;
            _Set_Size(sz);
            for (LIB::ui64 i = 0; i < sz; i++) {
                start[i] = (_Save_Type)from[i];
            }
            return;
        _Not_Orig:
            _Orig->paste<char>(_Select_Start, _Select_End, from, sz);
            _Orig->_size += sz - (_Select_End - _Select_Start + 1);
            return;
        }

        _str(const char* from) {
            cpy(from);
        }



        _str() {

        }

        char* c_str() const {
            char* _Cpy = new char[_size + 1];
            for (LIB::ui64 i = 0; i < _size; i++) {
                _Cpy[i] = (char)start[i];
            }
            _Cpy[_size] = (char)NULL;
            return _Cpy;
        }

        wchar_t* wch_str() const {
            wchar_t* _Cpy = new wchar_t[_size];
            for (LIB::ui64 i = 0; i < _size; i++) {
                _Cpy[i] = (wchar_t)start[i];
            }
            _Cpy[_size] = (wchar_t)NULL;
            return _Cpy;
        }

        void erase(LIB::ui64 start, LIB::ui64 end) {
            _Orig->paste(_Select_Start + start, _Select_Start + end, (_Save_Type*)0, 0);
        }

        void del() {
            _Orig->paste(_Select_Start, _Select_Start + _size - 1, (_Save_Type*)0, 0);
        }


    };
    typedef _str<char>        str;
    typedef _str<wchar_t>   str16;



    template <typename type> class elem {
    public:
        type el;
        elem* right = nullptr;
        elem* left = nullptr;
        elem() {

        }
        elem(type _el) : el(_el) {

        }
    };
    template <typename type> class tree {
        typedef elem<type> el;
    private:
        el* insirt(el* a, type b) {
            if (!a) {
                return new el(b);
            }

            if (b < a->el) {
                a->left = insirt(a->left, b);
            }
            else {
                a->right = insirt(a->right, b);
            }

            return a;
        }
        bool _find(el* e, type d) const {
            if (!e) {
                return false;
            }
            if (d < e->el) {
                return _find(e->left, d);
            }
            else if (d > e->el) {
                return _find(e->right, d);
            }
            else {
                return true;
            }
        }
        void print(el* g, int lvl) {
            if (!g) return;
            print(g->left, lvl + 5);
            std::cout << std::string(lvl, ' ') << g->el << std::endl;
            print(g->right, lvl + 5);
        }
    public:
        el* root = nullptr;
        tree() {

        }
        void add(type c) {
            root = insirt(root, c);
        }
        bool contins(type el) {
            return _find(root, el);
        }
        void print() {
            print(root, 0);
        }
    };

    void printNumberF(long double num) {
        num = abs(num);
        LIB::ui64 x = 10;  LIB::ui64 boofer = 0;
        while ((boofer = (LIB::ui64)(num *= x) % 10) != 0) {
            std::cout << boofer;
        }
    }
}