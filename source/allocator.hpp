#pragma once
#include "types.h"

namespace LIB {
	class _virtualMemory {
	private:
		void* ptr = nullptr; size_t blockSize = 0;
	public:
		_virtualMemory(size_t mySize) : blockSize(mySize), ptr(getMemory(mySize)) {}
		operator void* () { return ptr; }
		~_virtualMemory() { ::operator delete(ptr, blockSize); }
	};
}