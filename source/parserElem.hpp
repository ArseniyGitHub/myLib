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

	/*
	struct _PtrDataRecurce {
		std::vector<void*> args;  size_t id = 0;
		void* get() {
			if(id < args.size()); 
			else {
				args.resize(id + 1);
				args[id] = nullptr;
			}
			return args[id++];
		}
		void add(void* arg) { args.push_back(arg); }
		void reset() { id = 0; }
		_PtrDataRecurce() {}
		_PtrDataRecurce(const std::vector<void*>& from, size_t i = 0) : args(from), id(i) {}
	};
	*/

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
	concept is_size_dynamic = requires(T container, typename T::size_type s) {
		{ container.resize(s) } -> std::convertible_to<void>;
		requires is_any_array_c<T>;
	};
	template <typename T>
	requires (is_any_array_c<T>)
	using element_type = typename T::value_type;

	using byte_type = ui8;
	using bytesVec = _Vector<byte_type>;
	template <size_t sz> using bytesArr = _Array<byte_type, sz>;
	using base_iterator = std::vector<byte_type>::iterator;

	/*
	template <typename type> using base_objPackT  = fn<void, bytesVec*, base_iterator*, type*, size_t, _PtrDataRecurce*>;
	template <typename type> using base_objParseT = fn<void, bytesVec*, base_iterator*, type*, size_t, _PtrDataRecurce*>;
	__interface packable{
    public:
		virtual size_t pack(bytesVec*, base_iterator*, size_t) =  0;
		virtual size_t parse(bytesVec*, base_iterator*, size_t) = 0;
	};
	*/
	__interface packable {
		virtual size_t pack(bytesVec*, size_t)  = 0;
		virtual size_t parse(bytesVec*, size_t) = 0;
	};
	/*
	struct __ParserErr_NotFullPacket {};
	*/
	using _szType = size_t;

	
	/*
	template <typename type>
	struct packableObject : public packable {
	private:
		type* object = nullptr;  bool connect = false;

	public:
		type& getObj() { return *object; }
		type*& getObjPointer() { return object; }
		void setPtrAs(bool status) { connect = status; }

		class objectFunctions {
		public:
			base_objPackT<type>   _objPack;
			base_objParseT<type> _objParse;
			_PtrDataRecurce* data;
			size_t mySize = 0;
			objectFunctions() {}
			objectFunctions(base_objPackT<type> _pack, base_objParseT<type> _parse, _PtrDataRecurce* _data = new _PtrDataRecurce, size_t myS = 0) : _objPack(_pack), _objParse(_parse), mySize(myS), data(_data) {}
			objectFunctions(const objectFunctions& from) : _objPack(from._objPack), _objParse(from._objParse), mySize(from.mySize), data(new _PtrDataRecurce(*from.data)) {}
		};

		static void _defBytePack(bytesVec* to, base_iterator* whr, type* obj, size_t write = 0, _PtrDataRecurce* data = nullptr) {
			data->get();
			if (write == 0) write = sizeof(type);
			bytesArr<sizeof(type)>* buffer = (bytesArr<sizeof(type)>*)(void*)obj;
			size_t id = whr->_Ptr - to->begin()._Ptr;
			if(write <= sizeof(type))
				to->insert(*whr, buffer->begin(), buffer->begin() + write);
			else {
				to->insert(*whr, buffer->begin(), buffer->end());
				*whr = to->begin() + id + buffer->size();
				to->insert(*whr + sizeof(type), write - sizeof(type), 0);
			}
			*whr = to->begin() + id + write;
		}
		static void _defByteParse(bytesVec* from, base_iterator* whr, type* obj, size_t read = 0, _PtrDataRecurce* data = nullptr) {
			data->get();
			if (read == 0) read = sizeof(type);
			*obj = *(type*)(void*)new bytesArr<sizeof(type)>(whr->_Ptr, read);
			whr->_Ptr += sizeof(type);
		}

		inline static const objectFunctions _defByteFunctions = objectFunctions(_defBytePack, _defByteParse, new _PtrDataRecurce(std::vector<void*>({nullptr})), sizeof(type));

		objectFunctions OF;

		size_t pack(bytesVec* to, base_iterator* whr, size_t write = 0) {
			size_t id = whr->_Ptr - to->begin()._Ptr;
			OF._objPack(to, whr, object, write, OF.data);
			return *whr - (to->begin() + id);
		}
		size_t parse(bytesVec* from, base_iterator* whr, size_t read = 0) {
			base_iterator b = *whr;
			OF._objParse(from, whr, object, read, OF.data);
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
		

		static void _defVecPack(bytesVec* to, base_iterator* whr, arrayType* obj, size_t write = 0, _PtrDataRecurce* data = nullptr) {
			if (write < sizeof(szType) && write != 0) return;
			size_t& mySize = write;
			typename packableObject<type>::objectFunctions* buffer = (typename packableObject<type>::objectFunctions*)data->get();
			packableObject<szType> size;  size.getObj() = obj->size();
			packableObject<szType>   sz(mySize);
			packableObject<bool> staticS(mySize != 0);
			size.pack(to, whr);
			staticS.pack(to, whr);
			if (mySize != 0) {
				sz.pack(to, whr);
				for (type& e : *obj)
					buffer->_objPack(to, whr, &e, 0, nullptr);
			}
			else {
				base_iterator cpy;
				byte_type* id;
				for (type& e : *obj) {
					cpy = *whr;  id = to->begin()._Ptr;
					buffer->_objPack(to, whr, &e, 0, data);
					*whr = to->begin() + (size_t)(whr->_Ptr - id);
					sz.getObj() = whr->_Ptr - cpy._Ptr;
					sz.pack(to, whr);
				}
			}
		}
		static void _defVecParse(bytesVec* from, base_iterator* whr, arrayType* obj, size_t read = 0, _PtrDataRecurce* data = nullptr) {
			if (read < sizeof(szType) && read != 0) return;
			size_t& mySize = read;
			obj->clear();
			typename packableObject<type>::objectFunctions* buffer = (typename packableObject<type>::objectFunctions*)data->get();
			packableObject<szType> size;
			packableObject<szType>   sz;
			packableObject<bool> staticS;
			size.parse(from, whr);
			staticS.parse(from, whr);
			obj->resize(size.getObj());
			if (staticS.getObj()) {
				sz.parse(from, whr);
				for(type& e : *obj)
					buffer->_objParse(from, whr, &e, sz.getObj(), data);
			}
			else {
				for (type& e : *obj) {
					sz.parse(from, whr);
					buffer->_objParse(from, whr, &e, sz.getObj(), data);
				}
			}
		}
		inline static const packableObject<arrayType>::objectFunctions _defVecOF = typename packableObject<arrayType>::objectFunctions(_defVecPack, _defVecParse, new _PtrDataRecurce(), 0);

		packableVector(arrayType* connect = new arrayType, const packableObject<type>::objectFunctions& _OF = packableObject<type>::_defByteFunctions) {
			this->getObjPointer() = connect;
			this->OF = _defVecOF;
			this->OF.data->add(&objOF);
			this->objOF.mySize = sizeof(type);
			setOF(_OF);
		}
		packableVector(const arrayType& from, const packableObject<type>::objectFunctions& _OF = packableObject<type>::_defByteFunctions) {
			this->getObjPointer() = new arrayType(from);
			this->OF = _defVecOF;
			this->OF.data->add(&objOF);
			this->objOF.mySize = sizeof(type);
			setOF(_OF);
		}

		void setOF(packableObject<type>::objectFunctions from) {
			this->objOF = from;
			//this->getDefPackArg() = this->objOF._objPack;
			//this->getDefParseArg() = this->objOF._objParse;
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
	*/
	template <typename type>
	class _packableObject : public packable {
	private:
		type* object = nullptr;
	public:
		struct _fnArgs;
		using packFn  = size_t(*)(bytesVec*, size_t, type*, void*);
		using parseFn = size_t(*)(bytesVec*, size_t, type*, void*);
		struct  typeOF;

		type& getObject() { return *object; }
		type*& getObjectPointer() { return object; }


		//    byte copy OF
		static constexpr size_t _defByteCopyPack (bytesVec* whr, size_t id, type* obj, void* args) {
			if (whr == nullptr || obj == nullptr) return 0;
			auto iter = whr->begin() + id;
			byte_type* obj_ptr = std::bit_cast<byte_type*>(obj);
			whr->insert(iter, obj_ptr, obj_ptr + sizeof(type));
			return sizeof(type);
		}
		static constexpr size_t _defByteCopyParse(bytesVec* whr, size_t id, type* obj, void* args) {
			if (whr == nullptr || whr->size() < id + sizeof(type)) return 0;;
			const byte_type* from = &(*whr)[id];
			*obj = *std::bit_cast<const type*>(from);
			return sizeof(type);
		}


		//    array/vector/etc OF
		
		static constexpr size_t _defVecPack (bytesVec* whr, size_t id, type* obj, void* _args) {
			if constexpr (is_any_array_c<type>) {
				using arrElT = typename type::value_type;
				size_t beginID = id;
				typename _packableObject<arrElT>::_fnArgs& args = *(typename _packableObject<arrElT>::_fnArgs*)_args;
				void* givePtr;
				typename _packableObject<arrElT>::packFn p;
				if (args.useWhen == 0) {
					p = args.OF.packObj;
					givePtr = args.data;
				}
				else if (args.useWhen == -1) {
					if constexpr (!is_any_array_c<arrElT>) {
						if (args.OF.packObj == nullptr) p = &_packableObject<typename type::value_type>::_defByteCopyPack;
						else p = args.OF.packObj;
						givePtr = args.data;
					}
					else {
						p = &_packableObject<arrElT>::_defVecPack;
						givePtr = _args;
					}
				}
				else {
					args.useWhen--;
					p = _packableObject<arrElT>::typeOF::getOF().packObj;
					givePtr = _args;
				}
				_packableObject<size_t> eSize;
				if constexpr (is_size_dynamic<type>) {
					_packableObject<size_t> sz(obj->size());
					id += sz.pack(whr, id);
				}
				for (arrElT& e : *obj) {
					eSize.getObject() = p(whr, id, &e, givePtr);
					if (p == &_packableObject<arrElT>::_defByteCopyPack || p == &_packableObject<arrElT>::_defVecPack) id += eSize.getObject();
					else id += eSize.pack(whr, id) + eSize.getObject();
				}
				return id - beginID;
			}
			else return 0;
		}
		
		static constexpr size_t _defVecParse(bytesVec* whr, size_t id, type* obj, void* _args) {
			if constexpr (is_any_array_c<type>) {
				using arrElT = typename type::value_type;
				size_t beginID = id;
				typename _packableObject<typename type::value_type>::_fnArgs& args = *(typename _packableObject<typename type::value_type>::_fnArgs*)_args;
				void* givePtr;
				typename _packableObject<typename type::value_type>::parseFn p;
				if (args.useWhen == 0) {
					p = args.OF.parseObj;
					givePtr = args.data;
				}
				else if (args.useWhen == -1) {
					if constexpr (!is_any_array_c<arrElT>) {
						if (args.OF.parseObj == nullptr) p = &_packableObject<typename type::value_type>::_defByteCopyParse;
						else p = args.OF.parseObj;
						givePtr = args.data;
					}
					else {
						p = &_packableObject<arrElT>::_defVecParse;
						givePtr = _args;
					}
				}
				else {
					args.useWhen--;
					p = _packableObject<typename type::value_type>::typeOF::getOF().parseObj;
					givePtr = _args;
				}
				_packableObject<size_t> eSize;
				if constexpr (is_size_dynamic<type>) {
					_packableObject<size_t> sz;
					id += sz.parse(whr, id);
					obj->resize(sz.getObject());
				}
				for (typename type::value_type& e : *obj) {
					eSize.getObject() = p(whr, id, &e, givePtr);
					if (p == &_packableObject<typename type::value_type>::_defByteCopyParse || p == &_packableObject<typename type::value_type>::_defVecParse) id += eSize.getObject();
					else id += eSize.pack(whr, id) + eSize.getObject();
				}
				return id - beginID;
			}
			else return 0;
		}

		


		inline static typeOF constexpr _defByteCopyOF{ &_defByteCopyPack, &_defByteCopyParse };
		inline static typeOF constexpr _defVecOF{ &_defVecPack, &_defVecParse };
		
		
		struct typeOF {
			packFn packObj; parseFn parseObj;
			constexpr typeOF() : packObj(nullptr), parseObj(nullptr) {}
			constexpr typeOF(packFn _pack, parseFn _parse) : packObj(_pack), parseObj(_parse) {}
			constexpr typeOF(const typeOF&) = default;
			constexpr typeOF& operator=(const typeOF&) = default;
			
			static constexpr const typeOF& getOF() {
				if constexpr (is_any_array_c<type>)
					return _defVecOF;
				else return _defByteCopyOF;
			}
			
		};
		struct _fnArgs {
			size_t useWhen = 0;
			typeOF OF;
			void* data = nullptr;
			constexpr _fnArgs(size_t i = -1, typeOF _OF = typeOF(), void* _data = nullptr) : useWhen(i), data(_data) {
				OF = _OF;
			}
		};

		typeOF OF;
		_fnArgs args;
		bool connect = false;


		_packableObject(type* connect, typeOF _OF = typeOF::getOF(), _fnArgs _args = _fnArgs()) : object(connect), OF(_OF), args(_args), connect(true) {}
		_packableObject(const type& from, typeOF _OF = typeOF::getOF(), _fnArgs _args = _fnArgs()) : object(new type(from)), OF(_OF), args(_args) {}
		_packableObject(typeOF _OF = typeOF::getOF(), _fnArgs _args = _fnArgs()) : object(new type), OF(_OF), args(_args) {}
		~_packableObject() {
			if (!connect)
				delete object;
		}

		size_t pack(bytesVec* to, size_t whr) {
			return  OF.packObj(to, whr, object,   (void*)&args);
		}
		size_t parse(bytesVec* from, size_t whr) {
			return OF.parseObj(from, whr, object, (void*)&args);
		}
	};
}