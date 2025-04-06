#pragma once
#include "types.h"
#include "format.h"
#include "num.h"
#include <string>
#include <vector>

namespace LIB {
	bool isName(char s, bool isFirst = true) {
		return (s >= 'a' && s <= 'z') || (s >= 'A' && s <= 'Z') || (s >= 'à' && s <= 'ÿ') || (s >= 'À' && s <= 'ß') || (s == '¸' || s == '¨') || (s == '_') || (!isFirst && (s >= '0' && s <= '9'));
	}
	bool isOperand(char s) {
		return isHere<char>({ ';', ',', '.', '@', '#', '$', '\\', '=', '>', '<', '!', '-', '+', '*', '/', '&', '|', '%', '^', ':' }, s);
	}
	struct _code_element {
		std::string type;
		std::string rtType;
		void* data = 0;
		ui64 szEl = 0;
		ui64 szTxt = 0;
	};
	struct scopes {
		std::vector<_code_element> data;
		char start;
		char end;
		bool end_or_start;
	};
	struct _label {
		std::string text;
		std::string last;
		std::string next;
	};
	struct var_name {
		std::string name;
	};
	struct _operator {
		std::string name;
	};
	struct _operand {
		std::string name;
	};
	struct _lekserData {
		std::vector<char> skip = { ' ', '\n' };
	}_lekser_data;
	_code_element getNext(std::string&, _lekserData&);
	void getScopeBody(char start, char end, std::string& str, std::vector<_code_element>& writeIn, _lekserData& dt) {
		_code_element boof;
		ui64 scLvl = 1;
		while (scLvl != 0) {
			boof = getNext(str, dt);
			if (boof.type == "scope" && scLvl != 0) {
				scopes& el = (*(scopes*)boof.data);
				if (el.end_or_start) {
					if (el.end == end && el.start == start) scLvl--;
				}
				else {
					if (el.end == end && el.start == start) scLvl++;
				}
			}
			if (scLvl != 0) {
				writeIn.resize(writeIn.size() + 1);
				writeIn[writeIn.size() - 1] = boof;
			}
		}

		return;
	}
	std::string getName(std::string str) {
		std::string ret;
		if (isName(str[0], true)) {
			ret += str[0];
			str.replace(0, 1, "");
			while (isName(str[0], false)) { ret += str[0]; str.replace(0, 1, ""); }
		}
		return ret;
	}
	ui64 skipNull(std::string& str, std::vector<char> symbols = {' ', '\n'}) {
		ui64 rt = 0;
		while (isHere<char>(symbols, str[0]) && str.size() > 0) { str.replace(0, 1, ""); rt++; }
		return rt;
	}
	_code_element getNext(std::string& str, _lekserData& data) {
		_code_element ret;
		ret.szTxt += skipNull(str, data.skip);
		if (str.size() == 0) {
			ret.type = "code_end";
			return ret;
		}
		bool scEnd = false;
		switch (str[0]) {
		case '[':
			ret.type = "scope";
			ret.data = new scopes;
			((scopes*)ret.data)->start = '[';
			((scopes*)ret.data)->end = ']';
			str.replace(0, 1, ""); ret.szEl++;
			((scopes*)ret.data)->end_or_start = scEnd;
			break;
		case '(':
			ret.type = "scope";
			ret.data = new scopes;
			((scopes*)ret.data)->start = '(';
			((scopes*)ret.data)->end = ')';
			str.replace(0, 1, ""); ret.szEl++;
			((scopes*)ret.data)->end_or_start = scEnd;
			break;
		case '{':
			ret.type = "scope";
			ret.data = new scopes;
			((scopes*)ret.data)->start = '{';
			((scopes*)ret.data)->end = '}';
			str.replace(0, 1, ""); ret.szEl++;
			((scopes*)ret.data)->end_or_start = scEnd;
			break;
		case ']':
			ret.type = "scope";
			ret.data = new scopes;
			((scopes*)ret.data)->start = '[';
			((scopes*)ret.data)->end = ']';
			str.replace(0, 1, ""); ret.szEl++;
			scEnd = true;
			((scopes*)ret.data)->end_or_start = scEnd;
			break;
		case ')':
			ret.type = "scope";
			ret.data = new scopes;
			((scopes*)ret.data)->start = '(';
			((scopes*)ret.data)->end = ')';
			str.replace(0, 1, ""); ret.szEl++;
			scEnd = true;
			((scopes*)ret.data)->end_or_start = scEnd;
			break;
		case '}':
			ret.type = "scope";
			ret.data = new scopes;
			((scopes*)ret.data)->start = '{';
			((scopes*)ret.data)->end = '}';
			str.replace(0, 1, ""); ret.szEl++;
			scEnd = true;
			((scopes*)ret.data)->end_or_start = scEnd;
			break;
		}
		if (ret.type == "scope") {
			if(!scEnd) getScopeBody(((scopes*)ret.data)->start, ((scopes*)ret.data)->end, str, ((scopes*)ret.data)->data, data);
			return ret;
		}
		
		auto st = TxtFormat::txtToNum(str, 10, false);
		if (st.status) {
			ret.type = "num";
			ret.rtType = "number";
			ret.data = new number;
			str = str.substr(st.endInd - st.startInd, str.size() - (st.endInd - st.startInd));
			(*(number*)ret.data) = st.nm;
			ret.szEl += st.endInd - st.startInd;
			return ret;
		}

		std::string boof = getName(str);  __labelPack boof2;   std::string boof3;  bool label = true;
		str = str.substr(boof.size(), str.size() - boof.size());
		try {
			boof2 = read_label(str);
		}
		catch (const char* str) {
			if (str == "not a label!") {
				label = false;
				goto ex;

			}
			else throw str;
		}
	ex:
		if (label) {
			ret.szEl += boof2.sz + 2;
			str = str.substr(boof2.sz + 2, str.size() - boof2.sz - 2);
			/*
			for (ui64 i = 0; i < boof2.sz + 2; i++) {
				str.replace(0, 1, "");
			}
			*/
			boof3 = getName(str);
			str = str.substr(boof3.size(), str.size() - boof3.size());
			ret.type = "label";
			ret.rtType = "str";
			ret.data = new _label;
			(*(_label*)ret.data).text = boof2;
			(*(_label*)ret.data).last =  boof;
			(*(_label*)ret.data).next = boof3;
			return ret;
		}
		else if (boof.size() != 0) {
			ret.type = "name";
			ret.data = new var_name;
			(*(var_name*)ret.data).name = boof;
			return ret;
		}

		if (isOperand(str[0])) {
			boof += str[0];  ret.szEl++;  str.replace(0, 1, "");
			while (isOperand(str[0])) {
				boof += str[0];
				str.replace(0, 1, "");
				ret.szEl++;
			}
			ret.type = "operand";
			ret.data = new _operand;
			(*(_operator*)ret.data).name = boof;
			return ret;
		}

		return ret;
	}
	void scanCode(std::string str, std::vector<_code_element>& writeIn) {
		_code_element boofer;
		while (true) {
			boofer = getNext(str, _lekser_data);
			if (boofer.type == "code_end") break;
			writeIn.resize(writeIn.size() + 1);
			writeIn[writeIn.size() - 1] = boofer;
		}
	}

	// these are all in this file!
}