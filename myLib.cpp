// myLib.cpp : Определяет функции для статической библиотеки.
//

#include "pch.h"
#include "framework.h"
#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <initializer_list>
#include <math.h>

// TODO: Это пример библиотечной функции.

namespace LIB {
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

    template <typename _Save_Type = char> class _str {
    private:

	    _Save_Type* start = 0;
        ui64 _Select_Start = 0;
        ui64 _Select_End = 0;
        _str* _Orig = this;
        ui64 _size = 0;



        template <typename txtType> void paste(ui64 _start, ui64 end, const txtType* text, ui64 sz) {
            if (_Orig != this) return;

            ui64 nwSize = _size + sz - (end - _start + 1);
            _Save_Type* nw = new _Save_Type[nwSize];
            ui64 ind = 0;

            while (ind < _start) {
                if (start[ind] == 0) break;
                nw[ind] = start[ind];
                ind++;
            }

            for (ui64 i = 0; i < sz; i++) {
                if (text[i] == (const txtType)0) break;
                nw[ind] = (_Save_Type)text[i];
                ind++;
            }

            for (ui64 i = end + 1; i < _size; i++) {
                if (start[i] == 0) break;
                nw[ind] = start[i];
                ind++;
            }

            delete[] start;
            start = nw;
            _size += sz - (end - _start + 1);
        }

        void _Set_Size(ui64 sz) {
            if (_Orig->start != 0) {
                delete[] _Orig->start;
            }
            _Orig->start = new _Save_Type[sz + _Select_Start];
            _Orig->_size = sz + _Select_Start;
        }

        long double readNum(ui64 ind) {

        }
        
    public:

        long double* numberStart = 0;
        ui64 numberSize;
        ui64* numberInd = 0;

        

        ui64 size() {
            return _size;
        }

        const _Save_Type* begin() {
            return start;
        }

        const _Save_Type* end() {
            return &(start[_size - 1]);
        }

        _str& operator += (const char* txt) {
            ui64 sz = 0;
            while (txt[sz] != (const char)0) sz++;
            _Save_Type* nw = new _Save_Type[_size + sz - 1];
            for (ui64 i = 0; i < _size; i++) {
                if (start[i] == (const _Save_Type)0) break;
                nw[i] = start[i];
            }
            for (ui64 i = 0; i < sz; i++) {
                if (txt[i] == (const char)0) break;
                nw[i + _size] = (_Save_Type)txt[i];
            }
            _size += sz;
            delete[] start;
            start = nw;

        }

        void paste(ui64 index, const _str txt) {
            paste<char>(index, index - 1, txt.c_str(), txt._size);
        }

        template <ui64 sz> void cpy(const _Save_Type(&from)[sz]) {
            cpy(from, sz);
        }

        _Save_Type* getStart() const {
            return start;
        }

        void cpy(const wchar_t* from) {
            ui64 sz = 0;
            while (from[sz] != (const wchar_t)0) sz++;
            if (_Orig != this) goto _Not_Orig;
            _Set_Size(sz);
            for (ui64 i = 0; i < sz; i++) {
                start[i] = (_Save_Type)from[i];
            }
            return;
        _Not_Orig:
            _Orig->paste<wchar_t>(_Select_Start, _Select_End, from, sz);
            _Orig->_size += sz - (_Select_End - _Select_Start + 1);
            return;
        }

        _Save_Type& operator [] (ui64 ind) {
            return start[ind];
        }

        _str<_Save_Type> select(ui64 start, ui64 end) {
            _str<_Save_Type> selected;
            selected._Orig = _Orig;
            selected._Select_Start = start + _Select_Start;
            selected._Select_End = end + _Select_Start;
            selected._size = end - start + 1;
            return selected;
        }

        void cpy(const char* from) {
            ui64 sz = 0;
            while (from[sz] != (const char)0) sz++;
            if (_Orig != this) goto _Not_Orig;
            _Set_Size(sz);
            for (ui64 i = 0; i < sz; i++) {
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
            for (ui64 i = 0; i < _size; i++) {
                _Cpy[i] = (char)start[i];
            }
            _Cpy[_size] = (char)NULL;
            return _Cpy;
        }

        wchar_t* wch_str() const {
            wchar_t* _Cpy = new wchar_t[_size];
            for (ui64 i = 0; i < _size; i++) {
                _Cpy[i] = (wchar_t)start[i];
            }
            _Cpy[_size] = (wchar_t)NULL;
            return _Cpy;
        }
        
        void erase(ui64 start, ui64 end) {
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
            else if(d > e->el) {
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

    template <typename T> bool eq(T arg1, std::vector<T> args2) {
        for (ui64 i = 0; i < args2.size(); i++) {
            if (arg1 != args2[i]) return 0;
        }
        return 1;
    }

    template <typename type> class _num {
    public:
        type elem;
        ui8 prs = 0;
        ui8 nums = 0;
        ui8 out = 10;
        _num(type from, ui8 _out = 10, ui8 _prs = 0, ui8 _nums = 0) {
            elem = from;
            prs = _prs;
            nums = _nums;
            out = _out;
        }
        _num() {

        }
        operator type& () { return elem; }
    };
    using num = _num<long double>;

    template <typename type> class El {
    public:
        std::string name;
        type val;
        El(std::string _name, type _val) : name(_name), val(_val){

        }
    };

    namespace data {
        std::string st_start = "{";
        std::string st_end = "}";
        std::string st_between = ", ";
    }

    template <typename type> struct mass {
        std::string start = data::st_start;
        std::string end = data::st_end;
        std::string between = data::st_between;
        std::vector<type> ms;
        mass(std::vector<type> elems = {}, std::string start_ = data::st_start, std::string end_ = data::st_end, std::string btw = data::st_between) : start(start_), end(end_), between(btw), ms(elems) {

        }
    };

    class TxtFormat {
    public:
        std::string InputTxt;
        std::string OutputTxt;
        ui8 foundFunc() {
            while (InputTxt.size() != 0) {
                if (InputTxt[0] == funcStr) {
                    if (InputTxt.size() == 1) {
                        OutputTxt += funcStr;
                        return 0;
                    }
                    if (InputTxt[1] == funcStr) {
                        OutputTxt += funcStr;
                        InputTxt.replace(0, 2, "");
                    }
                    else {
                        return 1;
                    }
                }
                else {
                    OutputTxt += InputTxt[0];
                    InputTxt.replace(0, 1, "");
                }
            }
            return 0;
        }

        void pasteMas(mass<std::string> str) {
            if (foundFunc()) {
                if (InputTxt[1] == 'm' || InputTxt[1] == 'a') {
                    InputTxt.replace(0, 2, "");
                    OutputTxt += str.start;
                    for (std::string el : str.ms) {
                        OutputTxt += "\"" + el + "\"" + str.between;
                    }
                    OutputTxt.replace(OutputTxt.size() - str.between.size(), OutputTxt.size(), "");
                    OutputTxt += str.end;
                }
            }
        }

    public:

        operator std::string() { return OutputTxt; }
        std::string str() {
            return OutputTxt;
        }

        template <typename type> static type vol(type num, type min, type max, ui8 t = 3) {
            if (t >> 1) if (num < min)num = min;
            if (t % 2) if (num > max)num = max;
            return num;
        }

        static ui16 getNum(char s, ui8 type = 10) {
            if (s >= '0' && s <= '0' + vol<ui8>(type, 0, 10) - 1) return s - 48;
            if (type > 10) if (s >= 'A' && s <= 'A' + vol<ui8>(type, 11, 25) - 10 - 1) return s - 48 - 7;
            return 100;
        }

        static bool isNum(std::string& str, ui8 type = 10) {
            ui8 ind = 0;
            if (str[0] == '-') ind++;
            if (str[ind] >= '0' && str[ind] <= '0' + type - 1) return 1;
            if (type > 10) if ((str[ind] >= 'A' && str[ind] <= 'A' + type - 10 - 1))return 1;
            return 0;
        }

        static ui16 getFullNumsInNum(long double num, ui8 outputType = 10) {
            ui16 ret = 0;
            long double boof = 1;
            ui64 cpy = abs(num);
            while (cpy / boof >= outputType) {
                boof *= outputType;
                ret++;
            }
            return ret;
        }

        static ui16 getPresission(long double& nm, ui8 outputType = 10, bool inc = false) {
            long double n = abs(nm);
            n = abs(n) - (unsigned __int64)abs(n);
            ui64 x = 1;
            unsigned __int64 test = n;
            int count = 0;
            while ((n - test <= 0.8) && (n - test >= 0.01)) {
                n *= 10;
                test = (unsigned __int64)n;
                x *= 10;
                count++;
            }
            if (n - test >= 0.8 && inc) {
                if (nm < 0) nm -= 1.0 / x;
                else nm += 1.0 / x;
            }
            /*
            while (n - test != 0) {
                count++;
                n = n / outputType;
                test = (unsigned __int64)n;
            }
            */
            return count;
            
        }

        static std::string numToTxt(long double num, ui8 outputType = 10, ui16 nums = 0, ui16 presission = 0) {
            std::string ret;
            if (nums == 0) {
                nums = getFullNumsInNum(num, outputType); 
            }
            if (presission == 0) {
                presission = getPresission(num, outputType, true);
                
            }
            if (num < 0)ret += '-';
            ui64 x = 1;
            for (ui64 i = 0; i < nums; i++) {
                x *= outputType;
            }
            ui64 n = (ui64)abs(num);
            ui8 boof = 0;
            for (ui64 i = 0; i <= nums; i++) {
                boof = n / x % outputType;
                if (boof <= 9) {
                    ret += boof + 48;
                }
                else {
                    ret += boof + 48 + 7;
                }
                x /= outputType;
            }
            if(presission != 0) ret += '.';
            x = outputType;
            for (ui64 i = 0; i < presission; i++) {
                
                boof = ((ui64)(abs(num) * x)) % outputType;
                if (boof <= 9) {
                    ret += boof + 48;
                }
                else {
                    ret += boof + 48 + 7;
                }
                x *= outputType;
            }
            return ret;
        }

        static mass<num> convert(mass<long double> ms) {
            mass<num> ret;
            ret.ms.resize(ms.ms.size());
            for (ui64 i = 0; i < ms.ms.size(); i++) {
                ret.ms[i] = ms.ms[i];
            }
            return ret;
        }

        static long double txtToNum(std::string str, ui8 type = 10) {
            ui8 z = 0;
            if (str[0] == '-') {
                z = 1;
                str.replace(0, 1, "");
            }
            ui16 x = 0;
            long double ret = 0;
            while (true) {
                if (isNum(str, type)) {
                    ret *= type;
                    ret += getNum(str[0], type);
                }
                else {
                    break;
                }

                str.replace(0, 1, "");
            }
            long double boof = 0;
            if (str[0] == '.') {
                str.replace(0, 1, "");
                while (isNum(str, type)) {
                    boof = getNum(str[0]);
                    for (ui16 i = 0; i <= x; i++)
                    {
                        boof /= type;
                    }
                    x++;
                    ret += boof;
                    str.replace(0, 1, "");
                }
            }
            if (z) ret = 0 - ret;
            return ret;
        }

        char funcStr = '%';
        TxtFormat& operator = (std::string text) {
            InputTxt = text;
            OutputTxt.clear();
            return *this;
        }

        TxtFormat& operator << (num& num) {
            if (foundFunc()) {
                if (InputTxt[1] == 'n' || InputTxt[1] == 'a') {
                    InputTxt.replace(0, 2, "");
                    OutputTxt += numToTxt(num, num.out, num.nums, num.prs);
                }
                else if (InputTxt[1] == 'c') {
                    InputTxt.replace(0, 2, "");
                    OutputTxt += (char)num.elem;
                }
            }
            foundFunc();
            return *this;
        }
        TxtFormat& operator << (long double num) {
            if (foundFunc()) {
                if (InputTxt[1] == 'n' || InputTxt[1] == 'a') {
                    InputTxt.replace(0, 2, "");
                    OutputTxt += numToTxt(num);
                }
                else if (InputTxt[1] == 'c') {
                    InputTxt.replace(0, 2, "");
                    OutputTxt += (char)num;
                }
            }
            foundFunc();
            return *this;
        }
        
        TxtFormat& operator << (std::string str) {
            if (foundFunc()) {
                if (InputTxt[1] == 's' || InputTxt[1] == 'a') {
                    InputTxt.replace(0, 2, "");
                    OutputTxt += str;
                }
            }
            foundFunc();
            return *this;
        }
        TxtFormat& operator << (El<std::string> elem) {
            if (foundFunc()) {
                if (InputTxt[1] == 'e' || InputTxt[1] == 'a') {
                    InputTxt.replace(0, 2, "");
                    OutputTxt += "\"" + elem.name + "\" = \"" + elem.val + "\"";
                }
            }
            foundFunc();
            return *this;
        }
        TxtFormat& operator << (El<num> elem) {
            if (foundFunc()) {
                if (InputTxt[1] == 'e' || InputTxt[1] == 'a') {
                    InputTxt.replace(0, 2, "");
                    OutputTxt += "\"" + elem.name + "\" = " + numToTxt(elem.val.elem, elem.val.out, elem.val.nums, elem.val.prs);
                }
            }
            foundFunc();
            return *this;
        }
        
        TxtFormat& operator << (mass<std::string> str) {
            foundFunc();
            pasteMas(str);
            foundFunc();
            return *this;
        }

        TxtFormat& operator << (std::vector<std::string> str) {
            foundFunc();
            pasteMas(mass<std::string>(str));
            foundFunc();
            return *this;
        }
        template <typename type> TxtFormat& operator << (mass<type> ms) {
            if (foundFunc()) {
                if (InputTxt[1] == 'm' || InputTxt[1] == 'a') {
                    InputTxt.replace(0, 2, "");
                    OutputTxt += ms.start;
                    for (long double el : ms.ms) {
                        OutputTxt += numToTxt(el) + ms.between;
                    }
                    OutputTxt.replace(OutputTxt.size() - ms.between.size(), OutputTxt.size(), "");
                    OutputTxt += ms.end;
                }
            }
            foundFunc();
            return *this;
        }
        template <typename type> TxtFormat& operator << (std::vector<type> ms) {
            if (foundFunc()) {
                if (InputTxt[1] == 'm' || InputTxt[1] == 'a') {
                    InputTxt.replace(0, 2, "");
                    OutputTxt += data::st_start;
                    for (long double el : ms) {
                        OutputTxt += numToTxt(el) + data::st_between;
                    }
                    if(!ms.empty())OutputTxt.replace(OutputTxt.size() - data::st_between.size(), OutputTxt.size() - 1, "");
                    OutputTxt += data::st_end;
                }
            }
            foundFunc();
            return *this;
        }
    };

    class Enum;

    struct ___IMPORT {
        Enum* from;
        signed __int64 id;
        ___IMPORT() {

        }
    };

    class Enum {
        std::vector<___IMPORT> files;
    public:
        std::string str;
        std::vector<signed __int64> args;
        
        bool status = false;

        void importFrom(Enum& from, signed __int64 id) {
            ___IMPORT package;
            package.from = &from;
            package.id = id;
            files.push_back(package);
        }

        signed __int64 parse(signed __int64 number, i64 startKey = 0) {
            i64 key = startKey;
            i64 min = 0;
            i64 max = 0;
            ui64 ind = 0;
            ui64 vInd = 0;
            i64 keyInc = 1;
            status = true;
            while (ind < str.size()) {
                if (str[ind] == '%') {
                    if (ind >= str.size()) break;
                    ind++;
                    if (str[ind] == 'm') {
                        min = args[vInd];
                        vInd++;
                        max = args[vInd];
                        vInd++;
                        if (number >= min && number <= max) return key + (number - min) * keyInc;
                        key += (max - min + 1) * keyInc;
                    }
                    else if (str[ind] == 'n') {
                        min = args[vInd];
                        max = min;
                        vInd++;
                        if (min == number) return key;
                        key += keyInc;
                    }
                    else if (str[ind] == 'k') {
                        key = args[vInd];
                        vInd++;
                    }
                    else if (str[ind] == 'i') {
                        keyInc = args[vInd];
                        vInd++;
                    }
                    else if (str[ind] == 'c') {
                        ui64 _id = args[vInd];
                        vInd++;
                        Enum& el = *files[_id].from;
                        i64 rt = el.parse(number, key);
                        if (el.status == true) return rt;
                    }
                    else {
                        ind++;
                        continue;
                    }
                }
                ind++;
            }
            status = false;
            return -1;
        }

        signed __int64 unParse(signed __int64 _key, i64 startKey = 0) {
            i64 key = startKey;
            i64 min = 0;
            i64 max = 0;
            ui64 ind = 0;
            ui64 vInd = 0;
            i64 keyInc = 1;
            status = true;
            while (ind < str.size()) {
                if (str[ind] == '%') {
                    if (ind >= str.size()) break;
                    ind++;
                    if (str[ind] == 'm') {
                        min = args[vInd];
                        vInd++;
                        max = args[vInd];
                        vInd++;
                        if (key + (max - min) * keyInc >= _key && _key >= key) return min + (_key - key) * keyInc;
                        key += keyInc * (max - min + 1);
                    }
                    else if (str[ind] == 'n') {
                        min = args[vInd];
                        max = min;
                        vInd++;
                        if (key == _key) return min;
                        key += keyInc;
                    }
                    else if (str[ind] == 'k') {
                        key = args[vInd];
                        vInd++;
                    }
                    else if (str[ind] == 'i') {
                        keyInc = args[vInd];
                        vInd++;
                    }
                    else if (str[ind] == 'c') {
                        ui64 _id = args[vInd];
                        vInd++;
                        Enum& el = *files[_id].from;
                        i64 rt = el.unParse(_key, key);
                        if (el.status == true) return rt;
                    }
                    else {
                        ind++;
                        continue;
                    }
                }
                ind++;
            }
            status = false;
            return -1;
        }

        unsigned __int64 size() {
            i64 key = 0;
            i64 min = 0;
            i64 max = 0;
            ui64 ind = 0;
            ui64 vInd = 0;
            i64 keyInc = 1;
            status = true;
            unsigned __int64 i = 0;
            while (ind < str.size()) {
                if (str[ind] == '%') {
                    if (ind >= str.size()) break;
                    ind++;
                    if (str[ind] == 'm') {
                        min = args[vInd];
                        vInd++;
                        max = args[vInd];
                        vInd++;
                        i += (max - min + 1);
                    }
                    else if (str[ind] == 'n') {
                        min = args[vInd];
                        max = min;
                        vInd++;
                        i++;
                    }
                    else if (str[ind] == 'c') {
                        ui64 _id = args[vInd];
                        vInd++;
                        Enum& el = *files[_id].from;
                        i += el.size();
                    }
                    else {
                        ind++;
                        continue;
                    }
                }
                ind++;
            }
            return i;
        }

        Enum(std::string str = "", std::vector<signed __int64> args = {}) : str(str), args(args) {
            
        }
    };

    struct Byte {
        unsigned char* byte;
        void write(unsigned __int8 id, char d) {
            if (((*byte) >> id) % 2 == 0) (*byte) += d << id;
        }
        bool read(unsigned __int8 id) {
            return ((*byte) >> id) % 2;
        }
    };

    namespace language {
        class Language {
        public:
            std::string id;
            Enum data;
            Language(std::string id = "", Enum data = Enum()) : id(id), data(data) {

            }
        };

        std::vector<Language> languages = { Language("rus", Enum("%m%n%m%m%n%m", {(char)'а', (char)'е', (char)'ё', (char)'ж', (char)'я', (char)'А', (char)'Е', (char)'Ё', (char)'Ж', (char)'Я'})), Language("eng", Enum("%m%m", {'a', 'z', 'A', 'Z'})) };

        signed __int64 getId(std::string language, char smvl) {
            for (size_t i = 0; i < languages.size(); i++) {
                if (languages[i].id == language) {
                    return languages[i].data.parse(smvl);
                }
            }
            return 0;
        }

        char getChar(std::string language, signed __int64 id) {
            for (size_t i = 0; i < languages.size(); i++) {
                if (languages[i].id == language) {
                    return languages[i].data.unParse(id);
                }
            }
            return 0;
        }

        std::string autoLanguage(std::string txt) {
            for (size_t i = 0; i < languages.size(); i++) {
                for (size_t o = 0; o < txt.size(); o++) {
                    languages[i].data.parse(txt[o]);
                    if (languages[i].data.status == true) return languages[i].id;
                }
            }
            return "";
        }

        Language& get(std::string name) {
            for (Language& el : languages) {
                if (el.id == name) return el;
            }
        }
    }
}

namespace core {
    using namespace LIB;
    using namespace std;

    string select(string& str, ui64 start, ui64 end) {
        string ret;
        for (ui64 i = start; i < end; i++) {
            ret += str[i];
        }
        return ret;
    }

    string getFile(vector<string> data) {
        string ret;
        for (string str : data) {
            ret += str;
            ret += '\n';
        }
        return ret;
    }

    bool isSimv(const char s) {
        return (s == 95) || (s >= 65 && s <= 90) || (s >= 97 && s <= 122);
    }

    bool isOperand(const char s) {
        return (s == '=' || s == '+' || s == '-' || s == '/' || s == '*' || s == '%' || s == '&' || s == '|' || s == '!' || s == ':' || s == '?' || s == '<' || s == '>' || s == '@');
    }

    bool isScope(const char s) {
        return (s == '(' || s == ')' || s == '[' || s == ']' || s == '{' || s == '}' || s == '\"' || s == '\'');
    }

    vector<ui64> foundStr(string in, string str) {
        vector<ui64> ret;
        ui64 ind = 0;
        for (ui64 i = 0; i < in.size(); i++) {
            if (in[i] == str[ind])ind++;
            else ind = 0;
            if (ind == str.size()) ret.push_back(i - ind + 1);
        }
        return ret;
    }
}

template <typename type> std::vector<type> plus(std::vector<type> arg, type num) {
    std::vector<type> ret(arg.size());
    for (LIB::ui64 i = 0; i < arg.size(); i++) {
        ret[i] = arg[i] + num;
    }
    return ret;
}

template <typename type> std::vector<type> minus(std::vector<type> arg, type num) {
    std::vector<type> ret(arg.size());
    for (LIB::ui64 i = 0; i < arg.size(); i++) {
        ret[i] = arg[i] - num;
    }
    return ret;
}

template <typename type> std::vector<type> delen(std::vector<type> arg, type num) {
    std::vector<type> ret(arg.size());
    for (LIB::ui64 i = 0; i < arg.size(); i++) {
        ret[i] = arg[i] / num;
    }
    return ret;
}

template <typename type> std::vector<type> umn(std::vector<type> arg, type num) {
    std::vector<type> ret(arg.size());
    for (LIB::ui64 i = 0; i < arg.size(); i++) {
        ret[i] = arg[i] * num;
    }
    return ret;
}

template <typename type> std::vector<type> Or(std::vector<type> arg, type num) {
    std::vector<type> ret(arg.size());
    for (LIB::ui64 i = 0; i < arg.size(); i++) {
        ret[i] = arg[i] | num;
    }
    return ret;
}

template <typename type> std::vector<type> And(std::vector<type> arg, type num) {
    std::vector<type> ret(arg.size());
    for (LIB::ui64 i = 0; i < arg.size(); i++) {
        ret[i] = arg[i] & num;
    }
    return ret;
}

template <typename type> std::vector<type> persent(std::vector<type> arg, type num) {
    std::vector<type> ret(arg.size());
    for (LIB::ui64 i = 0; i < arg.size(); i++) {
        ret[i] = arg[i] % num;
    }
    return ret;
}

template <typename type> std::vector<type> operator + (std::vector<type> arg, type num) {
    return plus<type>(arg, num);
}
template <typename type> std::vector<type> operator + (type num, std::vector<type> arg) {
    return plus<type>(arg, num);
}


template <typename type> std::vector<type> operator - (std::vector<type> arg, type num) {
    return minus<type>(arg, num);
}
template <typename type> std::vector<type> operator - (type num, std::vector<type> arg) {
    return minus<type>(arg, num);
}


template <typename type> std::vector<type> operator / (std::vector<type> arg, type num) {
    return delen<type>(arg, num);
}
template <typename type> std::vector<type> operator / (type num, std::vector<type> arg) {
    return delen<type>(arg, num);
}


template <typename type> std::vector<type> operator * (std::vector<type> arg, type num) {
    return umn<type>(arg, num);
}
template <typename type> std::vector<type> operator * (type num, std::vector<type> arg) {
    return umn<type>(arg, num);
}


template <typename type> std::vector<type> operator | (std::vector<type> arg, type num) {
    return Or<type>(arg, num);
}
template <typename type> std::vector<type> operator | (type num, std::vector<type> arg) {
    return Or<type>(arg, num);
}


template <typename type> std::vector<type> operator & (std::vector<type> arg, type num) {
    return And<type>(arg, num);
}
template <typename type> std::vector<type> operator & (type num, std::vector<type> arg) {
    return And<type>(arg, num);
}


template <typename type> std::vector<type> operator % (std::vector<type> arg, type num) {
    return persent<type>(arg, num);
}
template <typename type> std::vector<type> operator % (type num, std::vector<type> arg) {
    return persent<type>(arg, num);
}