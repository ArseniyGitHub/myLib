#pragma once
#include <string>
#include <vector>
#include "types.h"

namespace LIB {
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
        El(std::string _name, type _val) : name(_name), val(_val) {

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

    ui16 szRam;

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

        static bool isNum(char s, ui8 type = 10) {
            if (s >= '0' && s <= '0' + type - 1) return 1;
            if (type > 10) if ((s >= 'A' && s <= 'A' + type - 10 - 1))return 1;
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

        static ui16 getFullNumsInNum(std::string str, ui8 type = 10) {
            if (str[0] == '-')str.replace(0, 1, "");
            ui64 i = 0, rt = 0;
            while (str[i] == '0') i++;
            while (isNum(str[i], type)) i++, rt++;
            if (rt != i && rt == 0) rt = 1;
            else rt--;
            return rt;
        }

        static ui16 getPresission(long double& nm, ui8 outputType = 10, bool inc = false) {
            long double n = abs(nm);
            n = abs(n) - (unsigned __int64)abs(n);
            ui64 x = 1;
            unsigned __int64 test = n;
            int count = 0;
            while ((n - test <= 0.99) && (n - test >= 0.001) || (n - test != 0 && false)) {
                n *= 10;
                test = (unsigned __int64)n;
                x *= 10;
                count++;
            }
            if (n - test >= 0.5 && inc) {
                if (nm > 0) nm += 1.0 / x;
                else nm -= 1.0 / x;
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

        static ui16 getPresission(std::string str, ui8 type = 10) {
            if (str[0] == '-') str.replace(0, 1, "");
            str.replace(0, getFullNumsInNum(str, type) + 1, "");
            ui16 i = 0;
            while (isNum(str[i], type)) i++;
            return i;
        }

        static std::string numToTxt(const long double _num, const ui8 outputType = 10, ui16 nums = 0, ui16 presission = 0) {
            long double num = _num;
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
            if (presission != 0) ret += '.';
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

        static ui8 getNumType(std::string str) {
            if (str[0] == '-') str.replace(0, 1, "");
            if (str[0] == '0') {
                switch (str[1]) {
                case 'x':
                    return 16;
                    break;
                case 'b':
                    return 2;
                    break;
                }
            }
            else return 10;
        }
        struct __rtPacket {
            long double nm;
            bool status = false;
            ui64 startInd, endInd = 0;
        };
        static __rtPacket txtToNum(std::string str, ui8 type = 10, bool autoFind = true) {
            __rtPacket r;
            szRam = 0;
            ui8 z = 0;
            if (type != 0) type = getNumType(str);
            if (str[0] == '-') {
                szRam++;
                z = 1;
                str.replace(0, 1, "");
            }
            while (!isNum(str[0], type) && str.size() > 0 && autoFind) { str.replace(0, 1, ""); r.startInd++; };
            if (!isNum(str[0])) return r;
            r.status = true;  r.endInd = r.startInd;
            ui16 x = 0;
            long double ret = 0;
            while (true) {
                if (isNum(str, type)) {
                    szRam++;
                    ret *= type;
                    ret += getNum(str[0], type);
                }
                else {
                    break;
                }
                r.endInd++;
                str.replace(0, 1, "");
            }
            long double boof = 0;
            if (str[0] == '.') {
                r.endInd++;
                szRam++;
                str.replace(0, 1, "");
                while (isNum(str, type)) {
                    szRam++;
                    boof = getNum(str[0]);
                    for (ui16 i = 0; i <= x; i++)
                    {
                        boof /= type;
                    }
                    x++;
                    ret += boof;
                    str.replace(0, 1, "");
                    r.endInd++;
                }
            }
            if (z) ret = 0 - ret;
            r.nm = ret;
            return r;
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
                    if (!ms.empty())OutputTxt.replace(OutputTxt.size() - data::st_between.size(), OutputTxt.size() - 1, "");
                    OutputTxt += data::st_end;
                }
            }
            foundFunc();
            return *this;
        }
    };
    class Diap {
    public:
        i64 start, end = 0;   bool selected;
        Diap(const i64 _start, const i64 _end) : start(_start), end(_end), selected(true) {

        }
        Diap() {
            selected = false;
        }
        Diap(const i64 _start) : start(_start), end(_start) {
            selected = true;
        }
        void clear() {
            selected = false;
        }
    };

    bool __status_;  ui64 __startInd_;
    class number {
    private:
        long double num;
        unsigned __int16 _prs = 0;
        unsigned __int16 _nums = 0;
        unsigned __int8 outType = 10;
    public:

        bool status() { return __status_; }
        ui64 startInd() { return __startInd_; }
        operator long double& () { return num; }
        operator std::string() const { return str(); }
        void cpy(std::string str) {
            outType = TxtFormat::getNumType(str);
            _prs = TxtFormat::getPresission(str, outType);
            _nums = TxtFormat::getFullNumsInNum(str, outType);
            auto data = TxtFormat::txtToNum(str, outType);
            __status_ = data.status;
            num = data.nm;
            __startInd_ = data.startInd;
        }
        void cpy(long double _number, ui16 prs_ = 0, ui16 nums_ = 0, ui8 _outType = 10) {
            outType = _outType;  num = _number;    _nums = nums_;
            _prs = prs_;
            if (_nums == 0) _nums = TxtFormat::getFullNumsInNum(_number, _outType);
            if (_prs == 0) _prs = TxtFormat::getPresission(num, outType, true);
            __startInd_ = 0;
        }
        auto operator <=> (const number& arg) const = default;
        number(long double _number, ui16 prs_ = 0, ui16 nums_ = 0, ui8 _outType = 10) { cpy(_number, prs_, nums_, _outType); }
        number(const char* str) { cpy(str); }
        number(std::string str) { cpy(str); }
        number() { ; }
        void operator = (std::string str) { cpy(str); }
        void operator = (const char* str) { cpy(str); }
        void operator = (long double from) { cpy(from); }
        std::string str() const { return TxtFormat::numToTxt(num, outType, _nums, _prs); }
        ui8 getNumber(signed __int16 id) const {
            long double boof = abs(num);
            i16 rt;
            if (id >= 0) {
                for (ui64 i = 1; i < id; i++) {
                    boof /= outType;
                }
            }
            else {
                for (ui64 i = 0; i < abs(id); i++) {
                    boof *= 10;
                }
            }
            rt = (unsigned __int64)boof % outType;
            //if (num < 0) rt = 0 - rt;
            return rt;
        }
        void setNumber(ui8 nm, i16 id) {
            if (id >= 0) {
                while (id + 1 - _nums > 0) _nums++;
            }
            else {
                while (abs(id) - _prs > 0) _prs++;
            }
            std::string _cpy = TxtFormat::numToTxt(num, outType, _nums, _prs);
            ui64 i = 0;    while (_cpy[i] != '.')i++;
            if (id >= 0) {
                while (id + 1 >= 0) i--, id--;
            }
            else {
                while (id < 0) id++, i++;
            }
            _cpy[i] = TxtFormat::numToTxt(nm, outType)[0];
            cpy(_cpy);
        }
        ui8 operator [] (signed __int16 id) const { return getNumber(id); }
        ui16 size() const { return str().size(); }
        ui16 nums() const { return _nums; }
        ui16 prs() const { return _prs; }
    };

    struct Byte {
        unsigned char* byte;
        void write(unsigned __int8 id, char d) {
            if (((*byte) >> id)
                % 2 == 0) (*byte) += d << id;
        }
        bool read(unsigned __int8 id) {
            return ((*byte) >> id) % 2;
        }
    };


    //    number2

    struct textNumber {
        std::string str;  ui8 type = 0; // 0 - auto type
    };

    class __hello {

        struct _dynamic_number {
            ui64 size;
            ui8* bytes;
            _dynamic_number(const ui64 sz = 8) : size(sz) {
                bytes = new ui8[size];
            }
            void realloc(ui64 sz) {
                ui8* nw = new ui8[sz];
                ui64 mn = ::std::min(sz, size);
                for (ui64 i = 0; i < mn; i++) {
                    nw[i] = bytes[i];
                }
                delete[] bytes;
                bytes = nw;
            }
        };

        enum types {
            none = 0 << 0,
            num = 1 << 0,

            plus = 0 << 1,
            minus = 1 << 1,

            infinity = 0 << 2,
            notInfinity = 1 << 2
        };

        types conf;

        _dynamic_number a; // целая часть
        _dynamic_number b; // числитель дроби
        _dynamic_number c; // знаменатель дроби

        __hello() : a(8), b(0), c(0){}

        
    };
}