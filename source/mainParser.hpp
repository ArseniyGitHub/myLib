#pragma once
#include "parserElem.hpp"
#include "types.h"
#include <concepts>

namespace PARSER_V2 {
	
	struct _defObjectInfo : public packable {
		packableObject<_defObjectInfo>::objectFunctions myOF;
		enum standardTypes {
			_null,
			_int,
			_uint,
			_float,
			_double,
			_object, 
			_stTypes_end
		};
		enum dataTypes {
			_elem,
			_vec,
			_dtTypes_end
		};
		size_t objT = standardTypes::_null;
		ui8 dataT = dataTypes::_elem;
		std::string name;
		void* buffer = nullptr;

		struct _NullType {
			template <typename T> operator T& () { return *(new T); }
			template <typename T> _NullType(const T&) {}
			template <typename T> void operator = (const T&) {}
			_NullType() {}
		};

		enum _bufferOperands {
			_delete,
			_create,
			_get,
			_set,
			_import,
			_export,
			_pack,
			_parse,
			_operands_end
		};

		template <typename T>
		requires (std::same_as<T, _defObjectInfo>)
		packableObject<T>::objectFunctions _getOF() {
			return ;
		}

		template <typename T>
		requires (!std::same_as<T, _defObjectInfo>)
		packableObject<T>::objectFunctions _getOF() {
			return packableObject<T>::_defByteFunctions;
		}

		template <typename T>
		requires (is_any_array_c<T>)
		void pack_or_parse(const _bufferOperands id, _packableArgs* args) {
			using elType = element_type<T>;
			packableVector<T> vec((T*)buffer);
			vec.setOF(_getOF<elType>());
			switch (id) {
			case _pack:
				vec.pack(args->to, args->whr, args->data);
				break;
			case _parse:
				vec.parse(args->to, args->whr, args->data);
				break;
			}
		}

		template <typename T>
		requires (!is_any_array_c<T>)
		void pack_or_parse(const _bufferOperands id, _packableArgs* args) {
			using elType = T;
			packableObject<elType> obj((T*)buffer);
			obj.setOF(_getOF<elType>());
			switch (id) {
			case _pack:
				obj.pack(args->to, args->whr, args->data);
				break;
			case _parse:
				obj.parse(args->to, args->whr, args->data);
				break;
			}
		}


		template <typename type, typename T> T* bufferDo(const _bufferOperands id, void* data = nullptr) {
			switch (id) {
			case _delete:
				delete (type*)buffer;
				break;
			case _create:
				buffer = new type;
				break;
			case _set:
				*(type*)buffer = (type)(*(T*)data);
				break;
			case _export:
				*(T*)data = *(type*)buffer;
				break;
			case _get:
				return new T(*(type*)buffer);
			case _import:
				buffer = new type(*(type*)data);
				break;
			case _pack:
				pack_or_parse<type>(id, (_packableArgs*)data);
				break;
			case _parse:
				pack_or_parse<type>(id, (_packableArgs*)data);
				break;
			}
			return (T*)buffer;
		}

		template <typename T = _NullType> T* _buffer(const _bufferOperands id, void* data = nullptr) {
			switch (dataT) {
			case dataTypes::_elem:
				switch (objT) {
				case standardTypes::_float:
					return bufferDo<float, T>(id, data);
				case standardTypes::_int:
					return bufferDo<i64, T>(id, data);
				case standardTypes::_uint:
					return bufferDo<ui64, T>(id, data);
				case standardTypes::_string:
					return bufferDo<std::string, T>(id, data);
				case standardTypes::_double:
					return bufferDo<long double, T>(id, data);
				case standardTypes::_object:
					return bufferDo<_defObjectInfo, T>(id, data);
				case standardTypes::_null:
					return bufferDo<_NullType, T>(id, data);
				default:
					return bufferDo<bytesVec, T>(id, data);
				}
			case dataTypes::_vec:
				switch (objT) {
				case standardTypes::_float:
					return bufferDo<std::vector<float>, T>(id, data);
				case standardTypes::_int:
					return bufferDo<std::vector<i64>, T>(id, data);
				case standardTypes::_uint:
					return bufferDo<std::vector<ui64>, T>(id, data);
				case standardTypes::_string:
					return bufferDo<std::vector<std::string>, T>(id, data);
				case standardTypes::_double:
					return bufferDo<std::vector<long double>, T>(id, data);
				case standardTypes::_object:
					return bufferDo<std::vector<_defObjectInfo>, T>(id, data);
				case standardTypes::_null:
					return bufferDo<std::vector<_NullType>, T>(id, data);
				default:
					return bufferDo<std::vector<bytesVec>, T>(id, data);
				}
			}
		}

		template <typename type> void createBufferAs() {
			buffer = new type;
		}

		void copyFrom(const float& num) {
			_buffer(_delete);
			createBufferAs<float>();
			dataT = dataTypes::_elem;
			objT = standardTypes::_float;
			*(float*)buffer = num;
		}
		void copyFrom(const long double& num) {
			_buffer(_delete);
			createBufferAs<long double>();
			dataT = dataTypes::_elem;
			objT = standardTypes::_double;
			*(long double*)buffer = num;
		}
		void copyFrom(const i64& num) {
			_buffer(_delete);
			createBufferAs<i64>();
			dataT = dataTypes::_elem;
			objT = standardTypes::_int;
			*(i64*)buffer = num;
		}
		void copyFrom(const ui64& num) {
			_buffer(_delete);
			createBufferAs<ui64>();
			dataT = dataTypes::_elem;
			objT = standardTypes::_uint;
			*(ui64*)buffer = num;
		}
		void copyFrom(const std::string& str) {
			_buffer(_delete);
			createBufferAs<std::string>();
			dataT = dataTypes::_elem;
			objT = standardTypes::_string;
			*(std::string*)buffer = str;
		}
		
		void copyFrom(const _defObjectInfo& obj) {
			_buffer(_delete);
			createBufferAs<_defObjectInfo>();
			dataT = dataTypes::_elem;
			objT = standardTypes::_object;
			*(_defObjectInfo*)buffer = obj;
		}
		
		void copyFrom(const std::vector<float>& num) {
			_buffer(_delete);
			createBufferAs<std::vector<float>>();
			dataT = dataTypes::_vec;
			objT = standardTypes::_float;
			*(std::vector<float>*)buffer = num;
		}
		void copyFrom(const std::vector<double>& num) {
			_buffer(_delete);
			createBufferAs<std::vector<double>>();
			dataT = dataTypes::_elem;
			objT = standardTypes::_double;
			*(std::vector<double>*)buffer = num;
		}
		void copyFrom(const std::vector<i64>& num) {
			_buffer(_delete);
			createBufferAs<std::vector<i64>>();
			dataT = dataTypes::_elem;
			objT = standardTypes::_int;
			*(std::vector<i64>*)buffer = num;
		}
		void copyFrom(const std::vector<ui64>& num) {
			_buffer(_delete);
			createBufferAs<std::vector<ui64>>();
			dataT = dataTypes::_elem;
			objT = standardTypes::_uint;
			*(std::vector<ui64>*)buffer = num;
		}
		void copyFrom(const std::vector<std::string>& str) {
			_buffer(_delete);
			createBufferAs<std::vector<std::string>>();
			dataT = dataTypes::_elem;
			objT = standardTypes::_string;
			*(std::vector<std::string>*)buffer = str;
		}
		
		void copyFrom(const std::vector<_defObjectInfo>& obj) {
			_buffer(_delete);
			createBufferAs<std::vector<_defObjectInfo>>();
			dataT = dataTypes::_elem;
			objT = standardTypes::_object;
			*(std::vector<_defObjectInfo>*)buffer = obj;
		}
		

		size_t pack (bytesVec*   to, base_iterator* whr, void* data = nullptr) {
			packableObject<std::string> _name(&name, _defStrFunctions);
			packableObject<size_t> _objT(objT);
			packableObject<ui8> _dataT(dataT);
			base_iterator cpy = *whr;
			_name.pack(to, whr, nullptr);
			_dataT.pack(to, whr, nullptr);
			_objT.pack(to, whr, nullptr);
			_buffer(_bufferOperands::_pack, new _packableArgs(to, whr, nullptr));
			return (size_t)(whr->_Ptr - cpy._Ptr);
		}

		size_t parse(bytesVec* from, base_iterator* whr, void* data = nullptr) {
			packableObject<std::string> _name(&name, _defStrFunctions);
			packableObject<size_t> _objT(&objT);
			packableObject<ui8> _dataT(&dataT);
			base_iterator cpy = *whr;
			_name.parse(from, whr, nullptr);
			_dataT.parse(from, whr, nullptr);
			_objT.parse(from, whr, nullptr);
			_buffer(_bufferOperands::_parse, new _packableArgs(from, whr, nullptr));
			return (size_t)(whr->_Ptr - cpy._Ptr);
		}

		void operator = (const _defObjectInfo& from) {
			copyFrom(from);
		}
		_defObjectInfo() {}
		_defObjectInfo(const _defObjectInfo& from) {
			copyFrom(from);
		}
	};
}