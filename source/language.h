#pragma once
#include <string>
#include <vector>
#include "enum.h"

namespace LIB {
    class Language {
    public:
        std::string id;
        Enum data;
        Language(std::string id = "", Enum data = Enum()) : id(id), data(data) {

        }
    };

    std::vector<Language> languages = { Language("rus", Enum("%m%n%m%m%n%m", {(char)'à', (char)'å', (char)'¸', (char)'æ', (char)'ÿ', (char)'À', (char)'Å', (char)'¨', (char)'Æ', (char)'ß'})), Language("eng", Enum("%m%m", {'a', 'z', 'A', 'Z'})) };

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