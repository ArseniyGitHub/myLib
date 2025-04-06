namespace numbers {
	using  byte =   signed __int8;
	using uByte = unsigned __int8;
	struct _noLimit {

		//   storage

		size_t size;  //  bytes
		void*  data;  //  data

		_noLimit(size_t sz = 4) : size(sz), data(new byte[sz]) {}
		void clear() {
			for (size_t i = 0; i < size; i++)
				delete ((byte*)data + i);
		}
		template <typename type> void cpy(const type& obj) {  //  
			clear();
			data = new byte[sizeof(type)];
			size = sizeof(type);
			for (size_t i = 0; i < size; i++)
				*((byte*)data + i) = ((byte*)&obj)[i];
		}
		void cpy(const void* bytes, size_t sz) {
			clear();
			size = sz;
			for (size_t i = 0; i < size; i++)
				*((byte*)data + i) = ((byte*)bytes)[i];
		}
		void cpy(const _noLimit& from) {
			clear();
			cpy(from.data, from.size);
		}
		template <typename type> _noLimit(const type& obj) {  //  
			cpy<type>(obj);
		}
		_noLimit& operator = (const _noLimit& from) {
			cpy(from);
			return *this;
		}
		template <typename type> _noLimit& operator = (const type& obj) {
			cpy<type>(obj);
			return *this;
		}
		template <typename type> operator type& () { return *((type*)data); }

		_noLimit& add(_noLimit& next) {

		}
	};
}