#pragma once
#include "num.h"
#include <string>
#include "types.h"
#include <vector>

namespace LIB {
    ui64 ___i = 0;
    std::string __arg(const std::string str) { return str; }
    std::string __arg(const const char* str) { return str; }
    std::string __arg(const long double& number) { return std::to_string(number); }
    std::string __arg(const double& number) { return std::to_string(number); }
    std::string __arg(const float& number) { return std::to_string(number); }
    std::string __arg(const char& s) {
        std::string rt;
        rt += s;    return rt;
    }
    std::string __arg(const bool& s) {
        if (s == true) return "true";
        else return "false";
    }
    std::string __arg(const number& nm) {
        return nm.str();
    }
    template <typename type> std::string __arg(const std::vector<type>& v) {
        TxtFormat a;
        (a = "%m") << v;
        return a.str();
    }
    template <typename type> std::string __arg(const type& arg) {
        return std::to_string(arg);
    }
    struct _____str : public std::string {

    };
    void _format(std::string& str) { ; }
    template <typename frst, typename... any> void _format(std::string& str, const frst& arg, const any... any_) {
        std::string boof;  ui64 i = ___i;
        while (str[i] != '{' && str[i + 1] != '}' && i + 1 < str.size()) i++;
        boof = __arg(arg);
        if (str[i] == '{' && str[i + 1] == '}') {
            std::string nw;
            for (ui64 o = 0; o < i; o++) nw += str[o];
            nw += boof;
            for (ui64 o = i + 2; o < str.size(); o++) nw += str[o];
            str = nw;
        }
        else str += boof;
        ___i = i + boof.size();
        _format(str, any_...);;
    }
    template <typename... any> std::string format(std::string str, const any... packet) {
        ___i = 0;
        _format(str, packet...);
        return str;
    }


















    //
    struct __labelPack {
        std::string txt;    ui64 sz = 0;
        operator std::string& () { return txt; }
    };
    constexpr __labelPack read_label(const std::string& txt, ui64 ind = 0) {
        struct _NotALabel {};
        __labelPack ret;
        if (txt[ind] != '\"' && txt[ind] != '\'') throw _NotALabel();
        for (ui64 i = ind + 1; i < txt.size(); i++) {
            if (txt[i] == '\\') {
                i++;  ret.sz++;
                if (!(i < txt.size())) break;
                switch (txt[i]) {
                case 'n':
                    ret.txt += '\n';
                    break;
                case 'r':
                    ret.txt += '\r';
                    break;
                case 'b':
                    ret.txt += '\b';
                    break;
                case 'a':
                    ret.txt += '\a';
                    break;
                case 't':
                    ret.txt += '\t';
                    break;
                case '\\':
                    ret.txt += '\\';
                    break;
                case '\"':
                    ret.txt += '\"';
                    break;
                case '\'':
                    ret.txt += '\'';
                    break;
                }
            }
            else if (txt[i] == '\"' || txt[i] == '\'') break;
            else ret.txt += txt[i];
            ret.sz++;
        }
        return ret;
    }
    std::string build_label(const std::string& txt) {
        std::string ret;
        for (ui64 i = 0; i < txt.size(); i++) {
            switch (txt[i]) {
            case '\n':
                ret += "\\n";
                break;
            case '\r':
                ret += "\\r";
                break;
            case '\b':
                ret += "\\b";
                break;
            case '\a':
                ret += "\\a";
                break;
            case '\t':
                ret += "\\t";
                break;
            case '\\':
                ret += "\\\\";
                break;
            case '\"':
                ret += "\\\"";
                break;
            case '\'':
                ret += "\\\'";
                break;
            default:
                ret += txt[i];
                break;
            }
        }
        return LIB::format("\"{}\"", ret);
    }
    bool isHere(std::string str, ui64 ind, std::string text) {
        ui64 _i = 0;
        for (ui64 i = ind; i < i + text.size(); i++, _i++) {
            if (str[i] != text[_i])return false;
        }
        return true;
    }

    
}