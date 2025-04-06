#pragma once
#include <string>
#include <vector>
#include "types.h"


namespace LIB {
    template <typename type> std::vector<ui64> found(const std::vector<type>& from, const std::vector<type>& elems) {
        ui64 o = 0;  std::vector<ui64> ret;
        for (ui64 i = 0; i < from.size(); i++) {
            if (from[i] == elems[o]) o++;
            else o = 0;
            if (o == elems.size()) {
                ret.resize(ret.size() + 1);
                ret[ret.size() - 1] = i - o + 1;
                o = 0;
            }
        }
        return ret;
    }
    std::vector<ui64> found(const std::string& from, const std::string& elems) {
        ui64 o = 0;  std::vector<ui64> ret;
        for (ui64 i = 0; i < from.size(); i++) {
            if (from[i] == elems[o]) o++;
            else o = 0;
            if (o == elems.size()) {
                ret.resize(ret.size() + 1);
                ret[ret.size() - 1] = i - o + 1;
                o = 0;
            }
        }
        return ret;
    }
    template <typename type> bool isHere(std::vector<type> vec, type el) {
        for (auto& e : vec) if (e == el) return true;
        return false;
    }
}

namespace __src {
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
}




template <typename type> std::vector<type> operator + (std::vector<type> arg, type num) {
    return __src::plus<type>(arg, num);
}
template <typename type> std::vector<type> operator + (type num, std::vector<type> arg) {
    return __src::plus<type>(arg, num);
}


template <typename type> std::vector<type> operator - (std::vector<type> arg, type num) {
    return __src::minus<type>(arg, num);
}
template <typename type> std::vector<type> operator - (type num, std::vector<type> arg) {
    return __src::minus<type>(arg, num);
}


template <typename type> std::vector<type> operator / (std::vector<type> arg, type num) {
    return __src::delen<type>(arg, num);
}
template <typename type> std::vector<type> operator / (type num, std::vector<type> arg) {
    return __src::delen<type>(arg, num);
}


template <typename type> std::vector<type> operator * (std::vector<type> arg, type num) {
    return __src::umn<type>(arg, num);
}
template <typename type> std::vector<type> operator * (type num, std::vector<type> arg) {
    return __src::umn<type>(arg, num);
}


template <typename type> std::vector<type> operator | (std::vector<type> arg, type num) {
    return __src::Or<type>(arg, num);
}
template <typename type> std::vector<type> operator | (type num, std::vector<type> arg) {
    return __src::Or<type>(arg, num);
}


template <typename type> std::vector<type> operator & (std::vector<type> arg, type num) {
    return __src::And<type>(arg, num);
}
template <typename type> std::vector<type> operator & (type num, std::vector<type> arg) {
    return __src::And<type>(arg, num);
}


template <typename type> std::vector<type> operator % (std::vector<type> arg, type num) {
    return __src::persent<type>(arg, num);
}
template <typename type> std::vector<type> operator % (type num, std::vector<type> arg) {
    return __src::persent<type>(arg, num);
}