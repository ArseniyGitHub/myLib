#pragma once
#include "parserElem.hpp"
#include "types.h"
#include <concepts>
#include <variant>


namespace PARSER_V2 {

	template <typename T>
	struct _hasDataPtr {
		virtual T* cpyPtr() = 0;
	};

	struct _defObjectInfo : public packable, public _hasDataPtr<packable> {
	public:
		enum class dataTypes {
			_null,
			_int,
			_uint,
			_float,
			_package,
			_packet,
			_end
		};
		enum class objTypes {
			_elem,
			_vec,
			_end
		};
		friend struct _defParserObject;

	public:
		std::string name;
		_szType dt = (_szType)dataTypes::_null;
		ui8 ot  = (ui8)objTypes::_elem;
		_szType elSz = 0;   ui8 is_named = 0;
	public:

		packable* data = nullptr;
		

		void setType(size_t t) { dt = ((_szType)dataTypes::_end + t); }
		size_t getType() { return (size_t)dt - (size_t)dataTypes::_end; }
		_szType& getSystemType() { return dt; }
		bool getOT() { return ot; }

		template <typename T>
		T* getAs() { return ((_packableObject<T>*)data)->getObjectPointer(); }

		void setName(const std::string& _name) { name = _name; if (name.empty()) is_named = 0; else is_named = 1; }
		const std::string& getName() { return name; }
		std::string& getNameRef()    { return name; }
		bool isNamed() { return is_named; }
	};

	struct _defParserObject : public _defObjectInfo {
	public:

		static constexpr size_t _defPack (bytesVec* v, size_t whr, _defParserObject* obj, void* args, size_t szLimit = 0) {
			size_t beginID = whr;
			_packableObject<ui8> _ot = (obj->ot) + ((obj->is_named) << 1);
			whr += _ot.pack(v, whr);

			_packableObject<_szType> _dt(&obj->dt);
			whr += _dt.pack(v, whr);

			_packableObject<_szType> _elSz(&obj->elSz);
			whr += _elSz.pack(v, whr);

			if (obj->is_named) {
				_packableObject<std::string> name(&obj->name);
				whr += name.pack(v, whr);
			}

			_packableObject<int>::_fnArgs arg = nullptr;

			switch (obj->ot) {
			case (ui8)objTypes::_elem:
				switch (obj->dt) {
				case (_szType)dataTypes::_package:
					((packable*)obj->data)->myFunc(0, (void*)&myOF);
					break;
				}
				break;
			case (ui8)objTypes::_vec:
				switch (obj->dt) {
				case (_szType)dataTypes::_package:
					arg.OF.packObj  =  (fn<size_t, bytesVec*, size_t, int*, void*, size_t>)(void*)myOF.packObj;
					arg.OF.parseObj = (fn<size_t, bytesVec*, size_t, int*, void*, size_t>)(void*)myOF.parseObj;
					break;
				}
				break;
			}
			
			whr += ((packable*)obj->data)->pack(v, whr, (void*)&arg, obj->elSz);
			if (szLimit < (whr - beginID) && szLimit != 0) throw _PackableObject_ErrSizeArg();
			return whr - beginID;
		}
		static constexpr size_t _defParse(bytesVec* v, size_t whr, _defParserObject* obj, void* args, size_t szLimit = 0) {
			size_t beginID = whr;
			_packableObject<ui8> _ot;
			whr += _ot.parse(v, whr);
			obj->ot = _ot.getObject() % 2;
			obj->is_named = (_ot.getObject() >> 1) % 2;

			_packableObject<_szType> _dt(&obj->dt);
			whr += _dt.parse(v, whr);

			_packableObject<_szType> _elSz(&obj->elSz);
			whr += _elSz.parse(v, whr);

			if (obj->is_named) {
				_packableObject<std::string> name(&obj->name);
				whr += name.parse(v, whr);
			}

			_packableObject<int>::_fnArgs arg;
			if (args != nullptr) arg = *(_packableObject<int>::_fnArgs*)args;

			switch (obj->ot) {
			case (ui8)objTypes::_elem:
				switch (obj->dt) {
				case (_szType)dataTypes::_int:
				case (_szType)dataTypes::_uint:
					switch (obj->elSz) {
					case 0: obj->data = new _packableObject<bytesVec>(); break;
					case 1: obj->data = new _packableObject<i8>();  break;
					case 2: obj->data = new _packableObject<i16>(); break;
					case 4: obj->data = new _packableObject<i32>(); break;
					case 8: obj->data = new _packableObject<i64>(); break;
					default: obj->data = new _packableObject<_Packet>(); break;
					}
					break;
				case (_szType)dataTypes::_float:
					switch (obj->elSz) {
					case 0: obj->data = new _packableObject<bytesVec>(); break;
					case 4: obj->data = new  _packableObject<float>();   break;
					case 8: obj->data = new _packableObject<double>();   break;
					default: obj->data = new _packableObject<_Packet>(); ((_packableObject<_Packet>*)obj->data)->getObject().resize(obj->elSz); break;
					}
					break;
				case (_szType)dataTypes::_package: obj->data = new _packableObject<_defParserObject>; ((_packableObject<_defParserObject>*)obj->data)->OF = myOF; break;
				default: obj->data = new _packableObject<_Packet>(); break;
				}
				break;
			case (_szType)objTypes::_vec:
				switch (obj->dt) {
				case (_szType)dataTypes::_int:
				case (_szType)dataTypes::_uint:
					switch (obj->elSz) {
					case 0: obj->data = new _packableObject<std::vector<bytesVec>>(); break;
					case 1: obj->data = new _packableObject<std::vector<i8>>();       break;
					case 2: obj->data = new _packableObject<std::vector<i16>>();      break;
					case 4: obj->data = new _packableObject<std::vector<i32>>();      break;
					case 8: obj->data = new _packableObject<std::vector<i64>>();      break;
					default: obj->data = new _packableObject<_Packet>(); break;
					}
					break;
				case (_szType)dataTypes::_float:
					switch (obj->elSz) {
					case 0: obj->data = new _packableObject<std::vector<bytesVec>>(); break;
					case sizeof(float): obj->data = new _packableObject<std::vector<float>>();    break;
					case sizeof(double): obj->data = new _packableObject<std::vector<double>>();   break;
					default: obj->data = new _packableObject<_Packet>(); break;
					}
					break;
				case (_szType)dataTypes::_package: obj->data = new _packableObject<std::vector<_defParserObject>>; arg.OF.packObj = (fn<size_t, bytesVec*, size_t, int*, void*, size_t>)(void*)myOF.packObj; arg.OF.parseObj = (fn<size_t, bytesVec*, size_t, int*, void*, size_t>)(void*)myOF.parseObj; break;
				case (_szType)dataTypes::_packet: obj->data = new _packableObject<_Packet>(); break;
				default: obj->data = new _packableObject<_Packet>(); break;
				}
				break;
			}

			whr += ((packable*)obj->data)->parse(v, whr, (void*)&arg, obj->elSz);
			if (szLimit < (whr - beginID) && szLimit != 0) throw _PackableObject_ErrSizeArg();
			return whr - beginID;
		}

		inline static constexpr _packableObject<_defParserObject>::typeOF myOF{ &_defPack, &_defParse };

		virtual size_t pack (bytesVec* to,   size_t id, void* data = new _packableObject<int>::_fnArgs(), size_t szLimit = 0) {
			return this->myOF.packObj(to, id, this, data, szLimit);
		}
		virtual size_t parse(bytesVec* from, size_t id, void* data = new _packableObject<int>::_fnArgs(), size_t szLimit = 0) {
			return this->myOF.parseObj(from, id, this, data, szLimit);
		}
		template <typename T>
		requires (!PARSER_V2::is_any_array_c<T>)
		constexpr void set(const T& from) {
			this->ot = (ui8)objTypes::_elem;
			if constexpr (std::is_same_v<T, _defParserObject>) {
				this->ot = from.ot;
				this->dt = from.dt;
				this->elSz = from.elSz;
				this->data = from.cpy();
			}
			else if constexpr (std::is_floating_point_v<T>) {
				if constexpr (sizeof(T) == sizeof(float)) {
					this->elSz = sizeof(float);
					this->data = new _packableObject<float>(from);
					this->dt = (_szType)dataTypes::_float;
				}
				else if constexpr (sizeof(T) == sizeof(double)) {
					this->elSz = sizeof(double);
					this->data = new _packableObject<double>(from);
					this->dt = (_szType)dataTypes::_float;
				}
				else if constexpr (sizeof(T) == sizeof(long double)) {
					this->elSz = sizeof(long double);
					this->data = new _packableObject<long double>(from);
					this->dt = (_szType)dataTypes::_float;
				}
			}
			else if constexpr (std::is_integral_v<T>) {
				if constexpr (std::is_signed_v<T>) {
					this->elSz = sizeof(T);
					this->dt = (_szType)dataTypes::_int;
					this->data = new _packableObject<T>(from);
				}
				else {
					this->elSz = sizeof(T);
					this->dt = (_szType)dataTypes::_uint;
					this->data = new _packableObject<T>(from);
				}
			}
			else if constexpr (std::is_base_of_v<packable, T>) {
				if constexpr (is_packed_size_static<T>) {
					this->elSz = T::_packed_size;
				}
				else this->elSz = 0;
				this->dt = dataTypes::_end;
				this->data = new T(from);
			}
		}
		template <typename T>
		requires (PARSER_V2::is_any_array_c<T>)
		constexpr void set(const T& from) {
			using _elT = typename T::value_type;
			this->ot = (ui8)objTypes::_vec;
			if constexpr (std::is_same_v<_elT, _defParserObject>) {
				this->data = new _packableObject<std::vector<_defParserObject>>(from.begin(), from.end());
				//((_packableObject<std::vector<_defParserObject>>*)this->data)->getObject().assign(from.begin(), from.end());
				this->dt = (_szType)dataTypes::_package;
				this->elSz = 0;
			}
			else if constexpr (std::is_floating_point_v<_elT>) {
				if constexpr (sizeof(_elT) == sizeof(float)) {
					this->elSz = sizeof(float);
					this->data = new _packableObject<std::vector<float>>(from.begin(), from.end());
					this->dt = (_szType)dataTypes::_float;
				}
				else if constexpr (sizeof(_elT) == sizeof(double)) {
					this->elSz = sizeof(double);
					this->data = new _packableObject<std::vector<double>>(from.begin(), from.end());
					this->dt = (_szType)dataTypes::_float;
				}
				else if constexpr (sizeof(_elT) == sizeof(long double)) {
					this->elSz = sizeof(long double);
					this->data = new _packableObject<std::vector<long double>>(from.begin(), from.end());
					this->dt = (_szType)dataTypes::_float;
				}
			}
			else if constexpr (std::is_integral_v<_elT>) {
				if constexpr (std::is_signed_v<_elT>) {
					this->elSz = sizeof(_elT);
					this->dt = (_szType)dataTypes::_int;
					this->data = new _packableObject<std::vector<_elT>>(from.begin(), from.end());
				}
				else {
					this->elSz = sizeof(_elT);
					this->dt = (_szType)dataTypes::_uint;
					this->data = new _packableObject<std::vector<_elT>>(from.begin(), from.end());
				}
			}
			else if constexpr (is_any_array_c<_elT>) {
				this->elSz = 0;
				this->dt = (_szType)dataTypes::_package;
				this->data = new _packableObject<std::vector<_defParserObject>>(from.begin(), from.end());
			}
			else if constexpr (std::is_base_of_v<packable, _elT>) {
				if constexpr (is_packed_size_static<_elT>) {
					this->elSz = _elT::_packed_size;
				}
				else this->elSz = 0;
				this->dt = dataTypes::_end;
				this->data = new T(from);
			}
		}
		constexpr void  set(packable* from) { delete data; data = from; }
		constexpr void* get() { return data; }

		template <typename T>
		requires(!is_any_array_c<T>)
		constexpr void exportData(T& to) {
			if (this->ot == (ui8)objTypes::_elem) {
				if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
					switch (this->dt) {
					case (_szType)dataTypes::_int:
						switch (this->elSz) {
						case 1: to =  ((_packableObject<i8>*)this->data)->getObject(); break;
						case 2: to = ((_packableObject<i16>*)this->data)->getObject(); break;
						case 4: to = ((_packableObject<i32>*)this->data)->getObject(); break;
						case 8: to = ((_packableObject<i64>*)this->data)->getObject(); break;
						default: break; } break;
					case (_szType)dataTypes::_uint:
						switch (this->elSz) {
						case 1: to =  ((_packableObject<ui8>*)this->data)->getObject(); break;
						case 2: to = ((_packableObject<ui16>*)this->data)->getObject(); break;
						case 4: to = ((_packableObject<ui32>*)this->data)->getObject(); break;
						case 8: to = ((_packableObject<ui64>*)this->data)->getObject(); break;
						default: break; } break;
					case (_szType)dataTypes::_float:
						switch (this->elSz) {
						case sizeof(float): to  =  ((_packableObject<float>*)this->data)->getObject(); break;
						case sizeof(double): to = ((_packableObject<double>*)this->data)->getObject(); break;
						default: break; } break;
					default: break; }
				}
				else if constexpr (std::convertible_to<_defParserObject, T>) {
					to = *this;
				}
			}
		}

		template <typename vT>
		requires(is_any_array_c<vT>)
		constexpr void exportData(vT& to) {
			using T = typename vT::value_type;
			if (this->ot == (ui8)objTypes::_vec) {
				if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
					switch (this->dt) {
					case (_szType)dataTypes::_int:
						switch (this->elSz) {
						case 1: { auto& p = ((_packableObject<std::vector<i8>>*)this->data)->getObject(); to.assign(p.begin(), p.end());  } break;
						case 2: { auto& p = ((_packableObject<std::vector<i16>>*)this->data)->getObject(); to.assign(p.begin(), p.end()); } break;
						case 4: { auto& p = ((_packableObject<std::vector<i32>>*)this->data)->getObject(); to.assign(p.begin(), p.end()); } break;
						case 8: { auto& p = ((_packableObject<std::vector<i64>>*)this->data)->getObject(); to.assign(p.begin(), p.end()); } break;
						default: break; } break;
					case (_szType)dataTypes::_uint:
						switch (this->elSz) {
						case 1: { auto& p = ((_packableObject<std::vector<ui8>>*)this->data)->getObject(); to.assign(p.begin(), p.end());  } break;
						case 2: { auto& p = ((_packableObject<std::vector<ui16>>*)this->data)->getObject(); to.assign(p.begin(), p.end()); } break;
						case 4: { auto& p = ((_packableObject<std::vector<ui32>>*)this->data)->getObject(); to.assign(p.begin(), p.end()); } break;
						case 8: { auto& p = ((_packableObject<std::vector<ui64>>*)this->data)->getObject(); to.assign(p.begin(), p.end()); } break;
						default: break; } break;
					case (_szType)dataTypes::_float:
						switch (this->elSz) {
						case sizeof(float): { auto& p = ((_packableObject<std::vector<float>>*)this->data)->getObject(); to.assign(p.begin(), p.end());   } break;
						case sizeof(double): { auto& p = ((_packableObject<std::vector<double>>*)this->data)->getObject(); to.assign(p.begin(), p.end()); } break;
						default: break; } break;
					default: break; }
				}
				else if constexpr (std::is_convertible_v<_defParserObject, T>) {
					if (this->dt == (_szType)dataTypes::_package) {
						auto& p = ((_packableObject<std::vector<_defParserObject>>*)this->data)->getObject();
						to.assign(p.begin(), p.end());
					}
				}
				else {
					if (this->dt == (_szType)dataTypes::_package) {
						auto& p = ((_packableObject<std::vector<_defParserObject>>*)this->data)->getObject();
						if constexpr (is_size_dynamic<T>) { to.resize(p.size()); }
						
						size_t s = std::min((size_t)to.size(), p.size());
						for (size_t i = 0; i < s; i++) {
							p[i].exportData(to[i]);
						}
					}
				}
			}
		}

		template <typename T>
		_defParserObject(const T& from) { set<T>(from); }
		
		_defParserObject() {}
		void deleteData() {
			delete (packable*)(this->data);
			this->dt = (_szType)dataTypes::_null;
			this->ot = (ui8)objTypes::_elem;
			this->data = nullptr;
			this->is_named = 0;
			this->elSz = 0;
		}
		virtual packable* cpy() { return new _defParserObject(*this); }
		virtual packable* cpyPtr() { return this->data->cpy(); }

		void myFunc(size_t c, void* data) {}
	};

	class Parser : public _defParserObject {
	public:
		Parser& operator [] (const std::string& name) {
			if (!(this->ot == (ui8)objTypes::_vec && this->dt == (_szType)dataTypes::_package)) {
				if (this->data != nullptr) delete this->data;
				this->data = new _packableObject<std::vector<Parser>>();
				this->ot = (ui8)objTypes::_vec;
				this->dt = (_szType)dataTypes::_package;
			}
			auto* p = getAs<std::vector<Parser>>();
			for (size_t i = 0; i < p->size(); i++) if ((*p)[i].name == name) return (*p)[i];

			p->resize(p->size() + 1);
			(*p)[p->size() - 1].setName(name);
			return (*p)[p->size() - 1];

		}
		Parser& operator [] (size_t i) {
			if (!(this->ot == (ui8)objTypes::_vec && this->dt == (_szType)dataTypes::_package)) {
				if (this->data != nullptr) delete this->data;
				this->data = new _packableObject<std::vector<Parser>>();
				this->ot = (ui8)objTypes::_vec;
				this->dt = (_szType)dataTypes::_package;
			}
			auto* p = getAs<std::vector<Parser>>();
			if (p->size() <= i) p->resize(i + 1);
			return (*p)[i];
		}
		std::vector<Parser>* getPackage(){ 
			if (!(this->ot == (ui8)objTypes::_vec && this->dt == (_szType)dataTypes::_package) || data == nullptr) return nullptr;
			else return (std::vector<Parser>*)data->get();
		}
		bool isPackage() { return (this->ot == (ui8)objTypes::_vec && this->dt == (_szType)dataTypes::_package); }
		template <typename T>
		_defParserObject& operator = (const T& from) { set<T>(from); return *this; }
	};
}