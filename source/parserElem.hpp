#pragma once
#include <string>
#include <vector>
#include <iterator>
#include <type_traits>
#include "types.h"

namespace PARSER_V2 {

	template <typename type, size_t sz> struct _Array : public std::array<type, sz> {
		constexpr _Array(const type* begin, size_t read = sz) {
			
			for (size_t i = 0; i < read; i++) {
				this->_Elems[i] = begin[i];
			}
			for (size_t i = read; i < sz; i++) {
				this->_Elems[i] = 0;
			}
		}
	};
	template <typename type> struct _Vector : public std::vector<type> {
		constexpr _Vector(size_t sz, const type* begin, size_t read = sz) {
			this->resize(sz);
			type* d = this->data();
			for (size_t i = 0; i < read; i++) {
				(*this)[i] = begin[i];
			}
			for (size_t i = read; i < sz; i++) {
				d[i] = 0;
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
	concept is_size_dynamic = requires(T container, typename T::size_type s) {
		{ container.resize(s) } -> std::convertible_to<void>;
		requires ((is_any_array_c<T> || requires{ typename T::_dynamic_size; }) && !requires{ typename T::_not_dynamic_size; });
	};

	template <typename T>
	concept is_packed_size_static = requires { std::is_same_v<decltype(T::_packed_size), const size_t>; };
	template <typename T>
	requires (is_any_array_c<T>)
	using element_type = typename T::value_type;

	using byte_type = ui8;
	using bytesVec = _Vector<byte_type>;
	template <size_t sz> using bytesArr = _Array<byte_type, sz>;
	using base_iterator = std::vector<byte_type>::iterator;

	template <typename T, size_t s, typename arr = std::array<T, s>>
	class arrayWithDynamicSize : public arr { 
	public: 
		using _dynamic_size = void; 
		void resize(size_t s) {}
	};

	template <typename T, typename vec = std::vector<T>>
	class vectorWithStaticSize : public vec { 
	public: 
		vectorWithStaticSize(size_t s) { this->resize(s); }
		vectorWithStaticSize() = default;
		using _not_dynamic_size = void;
	};

	struct _Packet : public bytesVec { using _PacketType = void; };

	struct packable {
		virtual constexpr size_t pack(bytesVec*, size_t, void*, size_t)    = 0;
		virtual constexpr size_t parse(bytesVec*, size_t, void*, size_t)   = 0;
		virtual constexpr void  set(packable* from)  = 0;
		virtual constexpr void* get()                = 0;
		virtual constexpr void myFunc(size_t, void*) = 0;
		virtual ~packable()                          = default;
		virtual packable* cpy()                      = 0;
	};
	using _szType = ui32;

	struct _PackableObject_ErrSizeArg {};

	template <typename type>
	class _packableObject : public packable {
	private:
		type* object = nullptr;
	public:
		struct _fnArgs;
		using packFn  = size_t(*)(bytesVec*, size_t, type*, void*, size_t);
		using parseFn = size_t(*)(bytesVec*, size_t, type*, void*, size_t);
		struct  typeOF;

		virtual constexpr void* get()            { return (void*)object; }
		virtual constexpr void set(packable* from) { *object = *(type*)from; }
		constexpr type& getObject()            { return *object; }
		constexpr type*& getObjectPointer()     { return object; }


		//    byte copy OF
		static constexpr size_t _defByteCopyPack (bytesVec* whr, size_t id, type* obj, void* args, size_t szLimit = 0) {
			if (whr == nullptr || obj == nullptr) return 0;
			auto iter = whr->begin() + id;
			byte_type* obj_ptr = (byte_type*)(void*)obj;
			if (szLimit == 0) {
				whr->insert(iter, obj_ptr, obj_ptr + sizeof(type));
				return sizeof(type);
			}
			else {
				bytesVec buffer(szLimit, obj_ptr, sizeof(type));
				whr->insert(iter, buffer.begin(), buffer.end());
				return szLimit;
			}
		}
		static constexpr size_t _defByteCopyParse(bytesVec* whr, size_t id, type* obj, void* args, size_t szLimit = 0) {
			if (whr == nullptr || obj == nullptr) return 0;;
			const byte_type* from = &(*whr)[id];
			if (szLimit == 0) {
				*obj = *std::bit_cast<const type*>(from);
				return sizeof(type);
			}
			else {
				bytesArr<sizeof(type)>* ptr = (new bytesArr<sizeof(type)>(from, szLimit));
				*obj = *(type*)(void*)(ptr->_Elems);
				return szLimit;
			}
		}

		//    array/vector/etc OF
		
		static constexpr size_t _defVecPack (bytesVec* whr, size_t id, type* obj, void* _args, size_t szLimit = 0) {
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
						if (args.OF.packObj == nullptr) p = &_packableObject<arrElT>::_defByteCopyPack;
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
				_packableObject<_szType> eSize((_szType)0);
				if constexpr (is_packed_size_static<arrElT>) {
					eSize.getObject() = arrElT::_packed_size;
					id += eSize.pack(whr, id);
				}
				else if (p == &_packableObject<arrElT>::_defByteCopyPack) {
					eSize.getObject() = sizeof(arrElT);
					id += eSize.pack(whr, id);
				}
				if (szLimit != 0) eSize.getObject() = szLimit;
				if constexpr (is_size_dynamic<type>) {
					_packableObject<_szType> sz(obj->size());
					id += sz.pack(whr, id);
				}
				/*
				if (szLimit == 0);
				else if (szLimit < (id - beginID)) throw _PackableObject_ErrSizeArg();
				else if ((szLimit - (id - beginID)) < (eSize.getObject() * obj->size())) throw _PackableObject_ErrSizeArg();
				*/
				for (arrElT& e : *obj) {
					if (!is_packed_size_static<arrElT> && p != &_packableObject<arrElT>::_defByteCopyPack) { eSize.getObject() = p(whr, id, &e, givePtr, szLimit); id += eSize.pack(whr, id) + eSize.getObject(); }
					else id += p(whr, id, &e, givePtr, szLimit);
				}
				//if (szLimit < (id - beginID) && szLimit != 0) throw _PackableObject_ErrSizeArg();
				return id - beginID;
			}
			else return 0;
		}

		//  packetOF
		
		static constexpr size_t _defVecParse(bytesVec* whr, size_t id, type* obj, void* _args, size_t szLimit = 0) {
			if constexpr (is_any_array_c<type>) {
				using arrElT = typename type::value_type;
				size_t beginID = id;
				typename _packableObject<arrElT>::_fnArgs& args = *(typename _packableObject<arrElT>::_fnArgs*)_args;
				void* givePtr;
				typename _packableObject<arrElT>::parseFn p;
				if (args.useWhen == 0) {
					p = args.OF.parseObj;
					givePtr = args.data;
				}
				else if (args.useWhen == -1) {
					if constexpr (!is_any_array_c<arrElT>) {
						if (args.OF.parseObj == nullptr) p = &_packableObject<arrElT>::_defByteCopyParse;
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
					p = _packableObject<arrElT>::typeOF::getOF().parseObj;
					givePtr = _args;
				}
				_packableObject<_szType> eSize((_szType)0);
				if constexpr (is_packed_size_static<arrElT>) {
					id += eSize.parse(whr, id);
				}
				else if (p == &_packableObject<arrElT>::_defByteCopyParse) {
					id += eSize.parse(whr, id);
				}

				if (szLimit != 0) eSize.getObject() = szLimit;
				
				if constexpr (is_size_dynamic<type>) {
					_packableObject<_szType> sz;
					id += sz.parse(whr, id);
					obj->resize(sz.getObject());
				}
				/*
				if (szLimit == 0);
				else if (szLimit < (id - beginID)) throw _PackableObject_ErrSizeArg();
				else if ((szLimit - (id - beginID)) < (eSize.getObject() * obj->size())) throw _PackableObject_ErrSizeArg();
				*/
				for (typename type::value_type& e : *obj) {
					if (!is_packed_size_static<typename type::value_type> && p != &_packableObject<typename type::value_type>::_defByteCopyParse) { id += eSize.parse(whr, id); }
					id += p(whr, id, &e, givePtr, eSize.getObject());
				}
				//if (szLimit < (id - beginID) && szLimit != 0) throw _PackableObject_ErrSizeArg();
				return id - beginID;
			}
			else return 0;
		}

		static constexpr size_t _defPacketPack(bytesVec* v, size_t whr, type* obj, void* _args, size_t szLimit = 0) {
			if constexpr (std::is_same_v<type, _Packet>) {
				size_t beginID = whr;
				size_t fullSize = obj->size();
				if (szLimit == 0) szLimit = fullSize;
				_fnArgs* arg = (_fnArgs*)_args;
				bool sz = false;
				if (arg != nullptr) if (arg->data != nullptr) sz = *(bool*)arg->data;
				if (sz) {
					_packableObject<_szType> s(szLimit);
					whr += s.pack(v, whr);
				}
				if (szLimit > fullSize) {
					v->insert(v->begin() + whr, obj->begin(), obj->begin() + fullSize);
					for (size_t i = 0; i < szLimit - fullSize; i++) 
						v->insert(v->begin() + whr + fullSize, (byte_type)0);
				}
				else v->insert(v->begin() + whr, obj->begin(), obj->begin() + szLimit);
				whr += szLimit;
				return whr - beginID;
			}
			else return 0;
		}

		static constexpr size_t _defPacketParse(bytesVec* v, size_t whr, type* obj, void* _args, size_t szLimit = 0) {
			if constexpr (std::is_same_v<type, _Packet>) {
				size_t beginID = whr;
				size_t fullSize = szLimit;
				_fnArgs* arg = (_fnArgs*)_args;
				bool sz = false;
				if (arg != nullptr) if (arg->data != nullptr) sz = *(bool*)arg->data;
				if (sz) {
					_packableObject<_szType> s;
					whr += s.parse(v, whr);
					if (szLimit == 0) { szLimit = s.getObject(); fullSize = szLimit; }
					else fullSize = s.getObject();
				}
				obj->assign(v->begin() + whr, v->begin() + whr + szLimit);
				whr += fullSize;
				return whr - beginID;
			}
			else return 0;
		}

		struct _fnArgs {
			i32 useWhen = -1;
			typeOF OF;
			void* data = nullptr;
			constexpr _fnArgs(void* _data = nullptr, i32 i = -1, typeOF _OF = typeOF()) : useWhen(i), data(_data) {
				OF = _OF;
			}
		};

		enum class myFuncCommands {
			_setOF,
			_end
		};

		virtual void myFunc(size_t c, void* data) {
			switch (c) {
			case (size_t)myFuncCommands::_setOF:
				OF = *(typeOF*)data;
				break;
			}
		}


		inline static typeOF constexpr _defByteCopyOF{ &_defByteCopyPack, &_defByteCopyParse };
		inline static typeOF constexpr _defVecOF{ &_defVecPack, &_defVecParse };
		inline static typeOF constexpr _defPacketOF{ &_defPacketPack, &_defPacketParse };
		
		
		struct typeOF {
			packFn packObj; parseFn parseObj;
			constexpr typeOF() : packObj(nullptr), parseObj(nullptr) {}
			constexpr typeOF(packFn _pack, parseFn _parse) : packObj(_pack), parseObj(_parse) {}
			constexpr typeOF(const typeOF&) = default;
			constexpr typeOF& operator=(const typeOF&) = default;
			
			static constexpr const typeOF& getOF() {
				if constexpr (std::is_same_v<type, _Packet>)
					return _defPacketOF;
				else if constexpr (is_any_array_c<type>)
					return _defVecOF;
				else return _defByteCopyOF;
			}
			
		};
		
		typeOF OF;
		bool connect = false;

		constexpr _packableObject(type* connect, typeOF _OF = typeOF::getOF()) : object(connect), OF(_OF), connect(true) {}
		template <typename T>
		constexpr _packableObject(const T& from, typeOF _OF = typeOF::getOF()) : object(new type(from)), OF(_OF) {}
		constexpr _packableObject(const type& from, typeOF _OF = typeOF::getOF()) : object(new type(from)), OF(_OF) {}
		constexpr _packableObject(typeOF _OF = typeOF::getOF()) : object(new type), OF(_OF) {}
		template <typename T, typename... args>
		constexpr _packableObject(T arg1, args... _any_args) : object(new type(arg1, _any_args...)), OF(typeOF::getOF()) {}
		constexpr _packableObject(bytesVec* to, size_t id, void* args = new _fnArgs()) { parse(to, id, args); }
		virtual ~_packableObject() {
			if (!connect)
				delete object;
		}
		virtual packable* cpy() { return new _packableObject<type>(*this->object, this->OF); }

		virtual constexpr size_t pack(bytesVec* to, size_t whr, void* d = new _fnArgs(), size_t szLimit = 0) {
			return  OF.packObj(to, whr, object,   d, szLimit);
		}
		virtual constexpr size_t parse(bytesVec* from, size_t whr, void* d = new _fnArgs(), size_t szLimit = 0) {
			return OF.parseObj(from, whr, object, d, szLimit);
		}
	};
}