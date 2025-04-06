#pragma once
#include <string>
#include "types.h"
#include <vector>

namespace LIB {
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
}