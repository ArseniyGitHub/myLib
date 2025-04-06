#pragma once
#include  <vector>
#include  <string>
#include <iostream>
#include "types.h"
#include "format.h"
#include "txtWork.h"

using __bytes = std::vector<ui8>;

template <typename nameType, typename typeType, typename blockType, typename sizeType, fn<ui64, typeType&> _Unparse_Process = nullptr>
struct __defParserVer;

struct __NullParserType {
	__NullParserType(){}
	__NullParserType(const __NullParserType& from){}
	template <typename type> __NullParserType(type& from){}
	__bytes* parse() {
		return nullptr;
	}
	void unParse(__bytes* pack) {
		return;
	}
};

template <typename _Type>
struct __ParserByteCopy {
	_Type el;
	__ParserByteCopy(const __ParserByteCopy& from) : el(from.el) {}
	__ParserByteCopy(_Type from) : el(from) {}
	__ParserByteCopy() {}
	__ParserByteCopy& unParse(__bytes* _from) {
		__bytes& from = *_from;
		for (ui64 i = 0; i < sizeof(_Type) && i < from.size(); i++) {
			((ui8*)(&el))[i] = from[i];
		}
		from.erase(from.begin(), from.begin() + sizeof(_Type));
		return *this;
	}
	__bytes* parse() {
		__bytes* ret = new __bytes(sizeof(_Type));
		for (ui64 i = 0; i < sizeof(_Type); i++) {
			(*ret)[i] = ((ui8*)(&el))[i];
		}
		return ret;
	}
};

template <typename _Type>
struct __NullParserSize {
	_Type el;
	__NullParserSize(const __NullParserSize& from) : el(from.el) {}
	__NullParserSize(_Type from) : el(from) {}
	__NullParserSize() {}
	void unParse(__bytes* _from) {
		return;
	}
	__bytes* parse() {
		return new __bytes;
	}
};

template <typename _sizeType>
struct __ParserText {
	using sizeType = __ParserByteCopy<_sizeType>;
	std::string text;
	__ParserText(const __ParserText& from) : text(from.text) {}
	__ParserText(const std::string& from) : text(from) {}
	template <ui64 sz> __ParserText(const char(&_text)[sz]) : text(_text) {}
	__ParserText() {}
	__bytes* parse() {
		__bytes* boofer;  __bytes bf2(text.size());
		sizeType sz = text.size();

		boofer = sz.parse();
		//if (boofer != nullptr && !boofer->empty()) ret.insert(ret.end(), boofer->begin(), boofer->end());
		//delete boofer;
		if (boofer == nullptr) boofer = new __bytes;

		for (ui64 i = 0; i < text.size(); i++) {
			bf2[i] = text[i];
		}
		boofer->insert(boofer->end(), bf2.begin(), bf2.end());
		return boofer;
	}

	__ParserText& unParse(__bytes* _from) {
		text.clear();
		__bytes& from = *_from;
		sizeType sz;
		sz.unParse(_from);
		text.resize(sz.el);
		for (ui64 i = 0; i < sz.el && i < from.size(); i++) {
			this->text[i] = from[i];
		}
		from.erase(from.begin(), from.begin() + sz.el);
		return *this;
	}
};


struct __ParserBlock {
	struct __NotFullPacket {};
	__bytes data;
	__bytes* parse(ui64 sz) {
		if (sz != data.size()) throw __NotFullPacket();
		__bytes* ret = new __bytes;
		ret->insert(ret->begin(), data.begin(), data.end());
		return ret;
	}
	void unParse(__bytes* _from, ui64 sz) {
		__bytes& from = *_from;
		data.clear();
		data.insert(data.begin(), from.begin(), from.begin() + sz);
		from.erase(from.begin(), from.begin() + sz);
	}
};

template <typename nameType, typename typeType, typename blockType, typename sizeType, fn<ui64, typeType&> _Unparse_Process>
struct __defParserVer {
	nameType name;   sizeType size;  typeType type;  blockType data;  void* elemBoofer = nullptr;
	__defParserVer(sizeType _size, nameType _name, typeType _type, blockType _data) : size(_size), name(_name), type(_type), data(_data) {}
	__defParserVer() {}
	__defParserVer(const __defParserVer& from) : size(from.size), name(from.name), type(from.type), data(from.data) {}
	__bytes* parse() {
		__bytes* ret = (new __bytes);
		__bytes* boofer;
		boofer = (name.parse());
		if (boofer != nullptr && !boofer->empty()) ret->insert(ret->end(), boofer->begin(), boofer->end());
		delete boofer;

		boofer = (type.parse());
		if (boofer != nullptr && !boofer->empty()) ret->insert(ret->end(), boofer->begin(), boofer->end());
		delete boofer;

		boofer = (size.parse());
		if (boofer != nullptr && !boofer->empty()) ret->insert(ret->end(), boofer->begin(), boofer->end());
		delete boofer;

		if (_Unparse_Process != nullptr) size.el = _Unparse_Process(type);
		boofer = data.parse(size.el);
		if (boofer != nullptr && !boofer->empty()) ret->insert(ret->end(), boofer->begin(), boofer->end());
		delete boofer;

		return ret;
	}
	__defParserVer& unParse(__bytes* _from) {
		__bytes& from = *_from;
		name.unParse(_from);
		type.unParse(_from);
		if (_Unparse_Process != nullptr) size.el = _Unparse_Process(type);
		else size.unParse(_from);
		data.unParse(_from, size.el);
		return *this;
	}
	template <typename type> __defParserVer& createBoofer() {
		elemBoofer = new type;
		return *this;
	}
	template <typename type> type& getBoofer() {
		return *(type*)elemBoofer;
	}
	template <typename type> __defParserVer& parseBoofer() {
		data.data = ((type*)elemBoofer)->parse();
		return *this;
	}
	template <typename type> __defParserVer& unParseBoofer() {
		elemBoofer = new type;
		((type*)elemBoofer)->unParse(data.data);
		return *this;
	}
};

using _defParserSize = __ParserByteCopy<ui64>;
using _defParserText =     __ParserText<ui64>;
template <typename szType, fn<ui64, __ParserByteCopy<szType>&> unParseS> using _EcoParserElem = __defParserVer<__NullParserType, __ParserByteCopy<szType>, __ParserBlock, __NullParserSize<ui64>, unParseS>;
template <typename typeT> using _defParserElem                               =                                    __defParserVer<__ParserText<ui64>, typeT, __ParserBlock, __ParserByteCopy<ui64>, nullptr>;
struct _NullPointer {};

using _defParserText = __ParserText<ui64>;
template <typename szType> struct Parser2 {
	using elType = _defParserElem<szType>;
	std::vector<_defParserElem<szType>> data;
	elType* boofer = nullptr;
	
	_defParserElem<szType>& operator [](ui64 ind) {
		if (ind + 1 > data.size()) data.resize(ind + 1);
		boofer = &(data[ind]);
		return data[ind];
	}
	
	_defParserElem<szType>& operator [](std::string name) {
		ui64 i = 0;  _defParserElem<szType> b;
		while (i < data.size() && data[i].name.text != name) i++;
		if (i == data.size()) data.push_back(b);
		boofer = &(data[i]);
		return data[i];
	}
	
	
	bool isHere(std::string name) {
		ui64 i = 0;
		while (i < data.size() && data[i].name.text != name) i++;
		if (i == data.size()) return false;
		return true;
	}
	
	__bytes* parse() {
		__bytes* ret = new __bytes();  __bytes* boofer = nullptr;
		for (_defParserElem<szType>& el : data) {
			boofer = el.parse();
			ret->insert(ret->end(), boofer->begin(), boofer->end());
			delete boofer;
		}
		return ret;
	}
	void unParse(__bytes* from) {
		data.clear();  _defParserElem<szType>* boofer = nullptr;
		while (from->size() != 0) {
			boofer   =   new _defParserElem<szType>;
			boofer->unParse(from);
			data.push_back(*boofer);
			delete        boofer;
		}
		return;
	}

	elType& l() {
		if (boofer) return *boofer; else throw _NullPointer();
	}

	elType& operator *() {
		return l();
	}
	
};



template <typename szType, fn<ui64, __ParserByteCopy<szType>&> unParseProc> struct Parser2_Eco {
	std::vector<_EcoParserElem<szType, unParseProc>> data;
	_EcoParserElem<szType, unParseProc>& operator [](ui64 ind) {
		if (ind + 1 > data.size()) data.resize(ind + 1);
		return data[ind];
	}
	__bytes* parse() {
		__bytes* ret = new __bytes();  __bytes* boofer = nullptr;
		for (_EcoParserElem<szType, unParseProc>& el : data) {
			boofer = el.parse();
			ret->insert(ret->end(), boofer->begin(), boofer->end());
		}
		return ret;
	}
	void unParse(__bytes* from) {
		data.clear();
		while (from->size() != 0)
			data.push_back(_EcoParserElem<szType, unParseProc>().unParse(from));
		return;
	}
};








struct __TextLabel {
	struct _Read_Err {};
	struct _TextPkg {
		std::string text;  ui64 s = 0; ui8 scType;
	};
	std::string readText(const std::string& str, ui64 ind = 0) {
		while (str[ind] == ' ' && ind < str.size()) ind++;
		if (ind == str.size()) {
			throw _Read_Err();
			return std::string();
		}
		_TextPkg ret;
		if (str[ind] == '\"' || str[ind] == '\'') {
			if (str[ind] == '\"') ret.scType = 1;
			else ret.scType = 2;
			auto boofer = LIB::read_label(str, ind);
			ret.text = boofer.txt;
			ret.s = boofer.sz;
		}
		else {
			ret.scType = 0;
			ui64 i = ind;
			while (isName(str[ind], false)) {
				ret.text.push_back(str[ind]);
				ind++;
			}
			ret.s = ind - i;
		}
		return ret;
	}
};