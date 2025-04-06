#pragma once
#include <string>
#include <vector>
#include <iterator>
#include <type_traits>
#include "types.h"

namespace PARSER_V2 {

	template <typename type, size_t sz> struct _Array : public std::array<type, sz> {
		_Array(const type* begin, size_t read = sz) {
			for (size_t i = 0; i < read; i++) {
				this->_Elems[i] = begin[i];
			}
			for (size_t i = read; i < sz; i++) {
				this->_Elems[i] = 0;
			}
		}
	};
	template <typename type> struct _Vector : public std::vector<type> {
		_Vector(size_t sz, const type* begin, size_t read = sz) {
			this->resize(sz);
			for (size_t i = 0; i < read; i++) {
				(*this)[i] = begin[i];
			}
			for (size_t i = read; i < sz; i++) {
				this->_Elems[i] = 0;
			}
		}
		_Vector() {}
	};

	template<typename T>
	concept is_vec_c = requires {
		typename T::value_type;
		typename T::allocator_type;
			requires std::same_as<T, std::vector<typename T::value_type, typename T::allocator_type>>;
	};
	template <typename T>
	concept is_any_array_c = requires(T container) {
		typename T::value_type;
		typename T::iterator;
		typename T::size_type;
		{ container.begin() } -> std::same_as<typename T::iterator>;
		{ container.end() } -> std::same_as<typename T::iterator>;
		{ container.size() } -> std::convertible_to<typename T::size_type>;
	};
	template <typename T>
	requires (is_any_array_c<T>)
	using element_type = typename T::value_type;

	using byte_type = ui8;
	using bytesVec = _Vector<byte_type>;
	template <size_t sz> using bytesArr = _Array<byte_type, sz>;
	using base_iterator = std::vector<byte_type>::iterator;

	template <typename type> using base_objPackT  = fn<void, bytesVec*, base_iterator*, type*, size_t, void*>;
	template <typename type> using base_objParseT = fn<void, bytesVec*, base_iterator*, type*, size_t, void*>;

	__interface packable{
    public:
		virtual size_t pack(bytesVec*, base_iterator*, size_t, void*) =  0;
		virtual size_t parse(bytesVec*, base_iterator*, size_t, void*) = 0;
	};
	
	struct __ParserErr_NotFullPacket {};

	using _szType = size_t;
	
	template <typename type>
	struct packableObject : public packable {
	private:
		type* object = nullptr;  bool connect = false;
		void* defPackArg = nullptr;
		void* defParseArg = nullptr;

	public:
		type& getObj() { return *object; }
		type*& getObjPointer() { return object; }
		void*& getDefPackArg() { return defPackArg; }
		void*& getDefParseArg() { return defParseArg; }
		void setPtrAs(bool status) { connect = status; }

		struct objectFunctions {
			base_objPackT<type>   _objPack;
			base_objParseT<type> _objParse;
			size_t mySize = 0;
			objectFunctions() {}
			objectFunctions(base_objPackT<type> _pack, base_objParseT<type> _parse, size_t myS = 0) : _objPack(_pack), _objParse(_parse), mySize(myS) {}
			objectFunctions(const objectFunctions& from) : _objPack(from._objPack), _objParse(from._objParse) {}
		};

		static void _defBytePack(bytesVec* to, base_iterator* whr, type* obj, size_t write = 0, void* data = nullptr) {
			if (write == 0) write = sizeof(type);
			bytesArr<sizeof(type)>* buffer = (bytesArr<sizeof(type)>*)(void*)obj;
			if(write <= sizeof(type)) to->insert(*whr, buffer->begin(), buffer->begin() + write);
			else {
				to->insert(*whr, buffer->begin(), buffer->end());
				to->insert(*whr + sizeof(type), write - sizeof(type), 0);
			}
			whr->_Ptr += write;
		}
		static void _defByteParse(bytesVec* from, base_iterator* whr, type* obj, size_t read = 0, void* data = nullptr) {
			if (read == 0) read = sizeof(type);
			*obj = *(type*)(void*)new bytesArr<sizeof(type)>(whr->_Ptr, read);
			whr->_Ptr += sizeof(type);
		}

		inline static const objectFunctions _defByteFunctions = objectFunctions(_defBytePack, _defByteParse, sizeof(type));

		objectFunctions OF;

		size_t pack(bytesVec* to, base_iterator* whr, size_t write = 0, void* data = nullptr) {
			if (data == nullptr) data = defPackArg;
			base_iterator b = *whr;
			OF._objPack(to, whr, object, write, data);
			//OF.mySize = *whr - b;
			return *whr - b;
		}
		size_t parse(bytesVec* from, base_iterator* whr, size_t read = 0, void* data = nullptr) {
			if (data == nullptr) data = defParseArg;
			base_iterator b = *whr;
			OF._objParse(from, whr, object, read, data);
			//OF.mySize = *whr - b;
			return *whr - b;
		}

		packableObject(type* connect = new type, const objectFunctions& _OF = _defByteFunctions) : object(connect), OF(_OF), connect(true) {}
		packableObject(const type& from, const objectFunctions& _OF = _defByteFunctions) : object(new type(from)), OF(_OF) {}
		~packableObject() {
			if (!connect)
				delete object;
		}

		void setOF(packableObject<type>::objectFunctions from) {
			OF = from;
		}
		packableObject<type>::objectFunctions getOF() {
			return OF;
		}
	};
	
	template <class arrayType, typename szType = _szType>
	requires (is_any_array_c<arrayType>)
	struct packableVector : public packableObject<arrayType> {
		using type = element_type<arrayType>;
	public:
		packableObject<type>::objectFunctions objOF = packableObject<type>::_defByteFunctions;

		static void _defVecPack(bytesVec* to, base_iterator* whr, arrayType* obj, size_t write = 0, void* data = nullptr) {
			if (write < sizeof(szType) && write != 0) return;
			size_t& mySize = write;
			base_objPackT<type> buffer = (base_objPackT<type>)data;
			packableObject<szType> size;  size.getObj() = obj->size();
			packableObject<szType>   sz(mySize);
			packableObject<bool> staticS(mySize != 0);
			size.pack(to, whr, 0, nullptr);
			staticS.pack(to, whr, 0, nullptr);
			if (mySize != 0) {
				sz.pack(to, whr, 0, nullptr);
				for (type& e : *obj)
					buffer(to, whr, &e, 0, nullptr);
			}
			else {
				base_iterator cpy;
				for (type& e : *obj) {
					cpy = *whr;
					buffer(to, whr, &e, 0, nullptr);
					sz.getObj() = whr->_Ptr - cpy._Ptr;
					sz.pack(to, whr);
				}
			}
			
		}
		static void _defVecParse(bytesVec* from, base_iterator* whr, arrayType* obj, size_t read = 0, void* data = nullptr) {
			if (read < sizeof(szType) && read != 0) return;
			size_t& mySize = read;
			obj->clear();
			base_objParseT<type> buffer = (base_objParseT<type>)data;
			packableObject<szType> size;
			packableObject<szType>   sz;
			packableObject<bool> staticS;
			size.parse(from, whr, 0, nullptr);
			staticS.parse(from, whr, 0, nullptr);
			obj->resize(size.getObj());
			if (staticS.getObj()) {
				sz.parse(from, whr, 0, nullptr);
				for(type& e : *obj)
					buffer(from, whr, &e, sz.getObj(), nullptr);
			}
			else {
				for (type& e : *obj) {
					sz.parse(from, whr, 0, nullptr);
					buffer(from, whr, &e, sz.getObj(), nullptr);
				}
			}
		}
		inline static const packableObject<arrayType>::objectFunctions _defVecOF = packableObject<arrayType>::objectFunctions(_defVecPack, _defVecParse, 0);

		packableVector(arrayType* connect = new arrayType, const packableObject<type>::objectFunctions& _OF = packableObject<type>::_defByteFunctions) {
			this->getObjPointer() = connect;
			this->OF = _defVecOF;
			setOF(_OF);
		}
		packableVector(const arrayType& from, const packableObject<type>::objectFunctions& _OF = packableObject<type>::_defByteFunctions) {
			this->getObjPointer() = new arrayType(from);
			this->OF = _defVecOF;
			setOF(_OF);
		}

		size_t pack(bytesVec* to, base_iterator* whr, size_t write = 0, void* data = nullptr) {
			if (data == nullptr) data = this->getDefPackArg();
			base_iterator b = *whr;
			this->OF._objPack(to, whr, packableObject<arrayType>::getObjPointer(), objOF.mySize, objOF._objPack);
			return *whr - b;
		}
		size_t parse(bytesVec* from, base_iterator* whr, size_t read = 0, void* data = nullptr) {
			if (data == nullptr) data = this->getDefParseArg();
			base_iterator b = *whr;
			this->OF._objParse(from, whr, packableObject<arrayType>::getObjPointer(), objOF.mySize, objOF._objParse);
			return *whr - b;
		}

		void setOF(packableObject<type>::objectFunctions from) {
			this->objOF = from;
			this->getDefPackArg() = this->objOF._objPack;
			this->getDefParseArg() = this->objOF._objParse;
		}
		packableObject<type>::objectFunctions getOF() {
			return objOF;
		}
	};

	struct _packableArgs { bytesVec* to; base_iterator* whr; void* data = nullptr; _packableArgs(bytesVec* _to, base_iterator* _whr, void* _data = nullptr) : to(_to), whr(_whr), data(_data) {} 
	_packableArgs() {} };
	template <typename type> struct _packableArgsP : public _packableArgs { type::objectFunctions _defOF; };

	template <typename T, typename szT = size_t>
	using packableObjectV_auto = std::conditional_t<is_any_array_c<T>, packableVector<T>, packableObject<T>>;
}