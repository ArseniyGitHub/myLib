#pragma once
#include  <vector>
#include  <string>
#include <iostream>
#include "types.h"
//#include <SFML/Graphics.hpp>

/*
struct ParserElem {
	enum Types {
		coords,
		rotation,
		speed,
		direction,
		brightness,
		id,
		color,
		end
	};
	Types   type;
	void*   data;
	template <typename T> ParserElem(Types type, T* data, bool cpy = true) : type(type) {
		if (cpy) {
			this->data = new T;
			*(T*)(this->data) = *data;
		}
		else this->data = data;
	}
	ParserElem() {

	}
};

class Parser {
public:
	struct ErrorType {};
	static std::vector<ParserElem> unPack(const ui8* data, const ui64 size) {
		std::vector<ParserElem> ret;
		ui64 counter = 0;
		ui64 _sz = 0;
		ParserElem::Types T = ParserElem::Types::end;
		for (ui64 i = 0; i < size;) {


			T = *(ParserElem::Types*)(data + i);  i += sizeof(ParserElem::Types);
			if (T >= ParserElem::Types::end) throw ErrorType();

			switch (T) {
			case ParserElem::Types::coords:
				_sz = 2 * 4;
				break;
			case ParserElem::Types::rotation:
				_sz = 4;
				break;
			case ParserElem::Types::speed:
				_sz = 4;
				break;
			case ParserElem::Types::direction:
				_sz = 2 * 4;
				break;
			case ParserElem::Types::brightness:
				_sz = 4;
				break;
			case ParserElem::Types::color:
				_sz = 4;
				break;
			case ParserElem::Types::id:
				_sz = 4;
				break;
			}
			ParserElem obj;
			obj.data = new ui8[_sz];
			ui8* ptr = (ui8*)(obj.data);
			counter =  0;
			obj.type = T;
			while (counter < _sz) {
				std::cout << (int)data[i] << std::endl;
				ptr[counter++] = data[i++];
			}
			ret.push_back(obj);
		}
		return ret;
	}

	static std::vector<ui8> pack(ParserElem& frst) { 
		ui64 sz = 0;   ui8* writeData = (ui8*)frst.data;
		switch (frst.type) {
		case ParserElem::Types::coords:
			sz = 2 * 4;
			break;
		case ParserElem::Types::rotation:
			sz = 4;
			break;
		case ParserElem::Types::speed:
			sz = 4;
			break;
		case ParserElem::Types::direction:
			sz = 2 * 4;
			break;
		case ParserElem::Types::brightness:
			sz = 4;
			break;
		case ParserElem::Types::color:
			sz = 4;
			break;
		case ParserElem::Types::id:
			sz = 4;
			break;
		}
		std::vector<ui8> ret(sizeof(ParserElem::Types) + sz);
		for (ui64 i = 0; i < sizeof(ParserElem::Types); i++) {
			ret[i] = ((ui8*)(&frst.type))[i];
		}
		for (ui64 i = 0; i < sz; i++) {
			ret[i + sizeof(ParserElem::Types)] = writeData[i];
		}
		return ret;
	}
	/*
    static std::vector<ui8> pack(ParserElem frst, ParserElem... args) {
		std::vector<ui8> ret = pack(frst);
		std::vector<ui8>& inc = pack(args...);
		ret.insert(ret.end(), inc.begin(), inc.end());
		//ret.emplace_back(pack(any...));
		return ret;
	}
	*//*
	static std::vector<ui8> pack(std::vector<ParserElem>& data) {
		std::vector<ui8> ret;
		for (auto& el : data) {
			std::vector<ui8>& inc = pack(el);
			ret.insert(ret.end(), inc.begin(), inc.end());
		}
		return ret;
	}
};
*/

namespace PARSER {

	using __bytes = std::vector<ui8>;

	template <typename nameType, typename typeType, typename blockType, typename sizeType>
	struct __defParserVer;

	struct __NullParserType {
		__NullParserType() {}
		__NullParserType(const __NullParserType& from) {}
		template <typename type> __NullParserType(type& from) {}
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
		__ParserByteCopy& unParse(__bytes* from) {
			//__bytes& from = *_from;
			for (ui64 i = 0; i < sizeof(_Type) && i < from->size(); i++) {
				((ui8*)(&el))[i] = (*from)[i];
			}
			from->erase(from->begin(), from->begin() + sizeof(_Type));
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
		std::string text = "";
		__ParserText(const __ParserText& from) : text(from.text) {}
		__ParserText(const std::string& from) : text(from) {}
		//template <ui64 sz> __ParserText(const char(&_text)[sz]) : text(_text) {}
		__ParserText() {}
		__bytes* parse(bool writeSize = true) {
			__bytes* boofer; __bytes bf2(text.size());
			sizeType sz = text.size();
			if (writeSize) {
				boofer = sz.parse();
			}
			else boofer = new __bytes;

			for (ui64 i = 0; i < text.size(); i++) {
				bf2[i] = text[i];
			}
			boofer->insert(boofer->end(), bf2.begin(), bf2.end());
			return boofer;
		}

		__ParserText& unParse(__bytes* _from, bool readSize = true, size_t _sz = 0) {
			__bytes& from = *_from;
			sizeType sz;
			if (readSize) sz.unParse(_from);
			else sz.el = _sz;;
			
			text.resize(sz.el);
			for (ui64 i = 0; i < sz.el && i < from.size(); i++) {
				text[i] = from[i];
			}
			from.erase(from.begin(), from.begin() + sz.el);
			return *this;
		}
	};


	struct __ParserBlock {
		struct __NotFullPacket {};
		__ParserBlock(const __ParserBlock& from) {
			data = new __bytes(*from.data);
		}
		__ParserBlock() {
			data = new __bytes();
		}
		__bytes* data = nullptr;
		__bytes* parse(ui64 sz) {
			if (sz != data->size()) throw __NotFullPacket();
			__bytes* ret = new __bytes;
			ret->insert(ret->begin(), data->begin(), data->end());
			return ret;
		}
		void unParse(__bytes* _from, ui64 sz) {
			__bytes& from = *_from;
			data->clear();
			data->insert(data->begin(), from.begin(), from.begin() + sz);
			from.erase(from.begin(), from.begin() + sz);
		}
	};

	template <typename nameType, typename typeType, typename blockType, typename sizeType>
	struct __defParserVer {
		fn<ui64, typeType&> _Unparse_Process = nullptr;
	    nameType name;   sizeType size = 0;  typeType type = 0;  blockType data;  void* elemBoofer = nullptr;  ui64 bufferSize = 0;
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
		template <typename type> __defParserVer& deleteBooferAs() {
			if (elemBoofer != nullptr) delete ((type*)elemBoofer);
			elemBoofer = nullptr;   bufferSize = 0;
			return *this;
		}
		__defParserVer& deleteBooferAsBytes() {
			delete[](ui8*)elemBoofer;
			elemBoofer = nullptr;   bufferSize = 0;
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
	using _defParserText = __ParserText<ui64>;
	template <typename szType> using _EcoParserElem = __defParserVer<__NullParserType, __ParserByteCopy<szType>, __ParserBlock, __NullParserSize<ui64>>;
	template <typename typeT> using _defParserElem = __defParserVer<__ParserText<ui64>, typeT, __ParserBlock, __ParserByteCopy<ui64>>;
	struct _NullPointer {};

	using _defParserText = __ParserText<ui64>;
	/*
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
				boofer = new _defParserElem<szType>;
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
	*/

}