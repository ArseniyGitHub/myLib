#pragma once
#include "netparser.h"
#include <list>
#include <memory>

using namespace PARSER;

namespace ParserTypes {
	template <typename txtSize = ui64> using txt =  __ParserText<txtSize>;
	template <typename type>           using num = __ParserByteCopy<type>;
}



template <typename sizeType = ui32>
class Parser {
public:
	struct _defEl : public __defParserVer<ParserTypes::txt<sizeType>, __ParserByteCopy<sizeType>, __ParserBlock, __ParserByteCopy<sizeType>> {
		Parser** _forParserRead = (Parser**) & (this->elemBoofer);
		void deleteBoofer() {
			switch (this->type.el) {
			case standardTypes::_elems:
				this->deleteBooferAs<Parser>();
				break;
			default:
				this->deleteBooferAsBytes();
				break;
			}
		}
		~_defEl() {
			deleteBoofer();
		}
	};
	__ParserByteCopy<sizeType> elemsCount = 0;
	std::vector<_defEl*> elems;
	__bytes* getCpy(__bytes* from) {
		return new __bytes(*from);
	}
public:
	enum standardTypes {
		_null,
		_elems,
	    _int,
		_uint,
		_float,
		_string,
		end
	};

	struct ErrType {};
	Parser(const Parser& from) {
		elems.clear();   elems.resize(from.elems.size());
		for (size_t i = 0; i < elems.size(); i++) 
			elems[i] = new _defEl(*from.elems[i]);
	}
	Parser() {}
	void clear() {
		for (_defEl* el : elems)
			delete el;
		elems.clear();
	}
	~Parser() {
		clear();
	}
	
	/// <summary>
	/// не гарантируем безопасность ваших личных данных!!!
	/// передавайте копию байтов!!!
	/// </summary>
	/// <param name="from"></param>
	void parse(__bytes* from) {
		clear();
		elemsCount.unParse(from);
		elems.resize(elemsCount.el);
		for (ui32 i = 0; i < elemsCount.el; i++) {
			elems[i] = new _defEl;
			elems[i]->unParse(from);
			switch (elems[i]->type.el) {
			case standardTypes::_int:
				elems[i]->elemBoofer = new i64;
				(*(i64*)elems[i]->elemBoofer) = __ParserByteCopy<i64>().unParse(getCpy(elems[i]->data.data)).el;
				elems[i]->bufferSize = 8;
				break;
			case standardTypes::_uint:
				elems[i]->elemBoofer = new ui64;
				(*(ui64*)elems[i]->elemBoofer) = __ParserByteCopy<ui64>().unParse(getCpy(elems[i]->data.data)).el;
				elems[i]->bufferSize = 8;
				break;
			case standardTypes::_float:
				elems[i]->bufferSize = elems[i]->size.el;
				if (elems[i]->size.el == 4) {
					elems[i]->elemBoofer = new float;
					(*(float*)elems[i]->elemBoofer) = __ParserByteCopy<float>().unParse(getCpy(elems[i]->data.data)).el;
				}
				else if (elems[i]->size.el == 8) {
					elems[i]->elemBoofer = new double;
					(*(double*)elems[i]->elemBoofer) = __ParserByteCopy<double>().unParse(getCpy(elems[i]->data.data)).el;
				}
				break;
			case standardTypes::_string:
				elems[i]->elemBoofer = new std::string((const char*)elems[i]->data.data->data(), elems[i]->data.data->size());
				//(*(std::string*)elems[i]->elemBoofer) = (const char*)elems[i]->data.data->data();
				elems[i]->bufferSize = sizeof(std::string);
				break;
			case standardTypes::_elems:
				elems[i]->elemBoofer = new Parser;
				elems[i]->bufferSize = sizeof(Parser);
				__bytes* cpy = new __bytes(*elems[i]->data.data);
				((Parser*)elems[i]->elemBoofer)->parse(cpy);
				break;
			}
		}
	}

	__bytes* pack() {
		__bytes* ret = new __bytes;
		__bytes* buffer;
		elemsCount.el = elems.size();
		buffer = elemsCount.parse();
		ret->insert(ret->end(), buffer->begin(), buffer->end());
		delete buffer;
		for (ui32 i = 0; i < elemsCount.el; i++) {

			if (elems[i]->type.el < standardTypes::end) {
				switch (elems[i]->type.el) {
				case standardTypes::_float:
					if (elems[i]->bufferSize == 4) {
						elems[i]->data.data = (__ParserByteCopy<float>(*(float*)elems[i]->elemBoofer).parse());
					}
					else if (elems[i]->bufferSize == 8) {
						elems[i]->data.data = (__ParserByteCopy<double>(*(double*)elems[i]->elemBoofer).parse());
					}
					break;
				case standardTypes::_int:
					elems[i]->data.data = (__ParserByteCopy<i64>(*(i64*)elems[i]->elemBoofer).parse());
					break;
				case standardTypes::_uint:
					elems[i]->data.data = (__ParserByteCopy<ui64>(*(ui64*)elems[i]->elemBoofer).parse());
					break;
				case standardTypes::_string:
					elems[i]->data.data = ParserTypes::txt<sizeType>(*(std::string*)elems[i]->elemBoofer).parse(false);
					break;
				case standardTypes::_elems:
					elems[i]->data.data = (*(Parser<sizeType>*)elems[i]->elemBoofer).pack();
					break;
				default:
					elems[i]->data.data = new __bytes(elems[i]->bufferSize);
					for (size_t o = 0; o < elems[i]->bufferSize; o++) {
						(*elems[i]->data.data)[o] = ((ui8*)elems[i]->elemBoofer)[o];
						//delete ((ui8*)elems[i]->elemBoofer + o);
					}
					//elems[i]->data.data = __bytes(elems[i]->bufferSize, (ui8*)elems[i]->elemBoofer);
					break;
				}
			}

			elems[i]->size.el = elems[i]->data.data->size();
			buffer = elems[i]->parse();
			ret->insert(ret->end(), buffer->begin(), buffer->end());
			delete buffer;
		}
		return ret;
	}

	//    for users!

	struct _Element {
		_defEl* element = nullptr;

		_Element(_defEl& from) {
			if (element != nullptr) delete element;
			element = new _defEl(from);
		}
		_Element(_defEl* from) : element(from) {}
		_Element() : element(new _defEl) {}


		//_Element* back = nullptr;

		//   element type
		const ui64 getType() {
			return element->type.el - standardTypes::end;
		}
		const ui64 getSysType() {
			return element->type.el;
		}
		void setType(ui64 type) {
			element->type.el = type + standardTypes::end;
		}
		bool isSysType() {
			return element->type.el < standardTypes::end;
		}

		//   element size
		const ui64 getBlockSize() {
			return element->data.data->size();
		}
		
		//   element name
		void setName(std::string name) {
			element->name.text = name;
		}
		const std::string& getName() {
			return element->name.text;
		}

		//   element data
		const __bytes* get() {
			return element->data.data;
		}
		/// <summary>
		/// используйте с умом! не присваивайте ему nullptr или др. нечитабельные адреса
		/// </summary>
		/// <returns></returns>
		__bytes*& getForEdit() {
			return element->data.data;
		}

		_Element& operator [] (std::string name) {
			if (element->type.el != standardTypes::_elems) {
				element->deleteBoofer();
				element->type.el = standardTypes::_elems;
			}
			if (element->elemBoofer == nullptr) {
				element->elemBoofer = new Parser;
				element->bufferSize = sizeof(Parser);
			}
			Parser* rt = (Parser*)element->elemBoofer;
			for (_defEl* el : rt->elems) {
				if (el->name.text == name) {
					_Element* e = new _Element;
					e->element = el;
					return *e;
				}
			}
			_Element* nw = new _Element;
			nw->setName(name);
			rt->elems.push_back(nw->element);
			return *nw;
		}

		_Element& operator [] (unsigned __int64 i) {
			if (element->type.el != standardTypes::_elems) {
				element->deleteBoofer();
				element->type.el = standardTypes::_elems;
			}
			if (element->elemBoofer == nullptr) {
				element->elemBoofer = new Parser;
				element->bufferSize = sizeof(Parser);
			}
			Parser* rt = (Parser*)element->elemBoofer;
			
			if (!(rt->elems.size() > i)) {
				size_t oldSize = rt->elems.size();
				rt->elems.resize(i + 1);
				for (size_t i = oldSize; i < rt->elems.size(); i++) {
					rt->elems[i] = new _defEl;
				}
			}
			_Element* r = new _Element(rt->elems[i]);
			return *r;
		}
		void operator = (Parser& from) {  //  copy
			element->deleteBoofer();
			element->type.el = standardTypes::_elems;
			element->bufferSize = sizeof(Parser);
			element->elemBoofer = new Parser(from);
		}
		void operator = (Parser* from) {  //  using! 
			element->deleteBoofer();
			element->type.el = standardTypes::_elems;
			element->bufferSize = sizeof(Parser);
			element->elemBoofer = from;
		}

		void operator = (ui64 num) {
			element->deleteBoofer();
			element->type.el = standardTypes::_uint;
			element->bufferSize = sizeof(ui64);
			element->elemBoofer = new ui64;
			*(ui64*)element->elemBoofer = num;
		}
		void operator = (i64 num) {
			element->deleteBoofer();
			element->type.el = standardTypes::_int;
			element->bufferSize = sizeof(i64);
			element->elemBoofer = new i64;
			*(i64*)element->elemBoofer = num;
		}
		void operator = (std::string& el) {
			element->deleteBoofer();
			element->type.el = standardTypes::_string;
			element->bufferSize = sizeof(std::string);
			element->elemBoofer = new std::string;
			*(std::string*)element->elemBoofer = el;
		}
		void operator = (const char* el) {
			element->deleteBoofer();
			element->type.el = standardTypes::_string;
			element->bufferSize = sizeof(std::string);
			element->elemBoofer = new std::string;
			*(std::string*)element->elemBoofer = el;
			element->size.el = ((std::string*)element->elemBoofer)->size();
		}
		void operator = (float num) {
			element->deleteBoofer();
			element->type.el = standardTypes::_float;
			element->bufferSize = sizeof(float);
			element->elemBoofer = new float;
			*(float*)element->elemBoofer = num;
		}
		void operator = (double num) {
			element->deleteBoofer();
			element->type.el = standardTypes::_float;
			element->bufferSize = sizeof(double);
			element->elemBoofer = new double;
			*(double*)element->elemBoofer = num;
		}
		_Element& write(__bytes& from) {
			if (element->data.data == nullptr) element->data.data = new __bytes(from.size());
			(*element->data.data) = from;
			return *this;
		}
		template <typename type> type& getAs() {
			if (isSysType()) return *(type*)element->elemBoofer;
			else return *(type*)(void*)element->data.data->data();
		}
	};

	_Element& operator [] (std::string name) {
		for (_defEl* el : elems) {
			if (el->name.text == name) {
				_Element* e = new _Element;
				e->element = el;
				return *e;
			}
		}
		_Element* nw = new _Element;
		nw->setName(name);
		elems.push_back(nw->element);
		return *nw;
	}

	_Element& operator [] (unsigned __int64 i) {
		if (!(elems.size() > i)) {
			size_t oldSize = elems.size();
			elems.resize(i + 1);
			for (size_t i = oldSize; i < elems.size(); i++) {
				elems[i] = new _defEl;
			}
		}
		return (elems)[i];
	}
};