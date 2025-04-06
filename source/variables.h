#pragma once
#include "format.h"
#include "num.h"
#include "types.h"
#include "operators.h"
#include "enum.h"
#include <string>
#include <vector>


namespace LIB {
	namespace test {
		const std::string variable_readError = "not a variable!";
		const std::string arg_readError = "incorrect argument!";
		class basic_variables {
		public:
			struct variable {
				std::string name = "";
				std::string type = "";
				std::string data = "";
			};
			struct _argPack {
				std::string name = "";   std::string msg = "";   ui64 sz = 0;  bool null = true;
			};

			std::string str;

			std::string buildVariable(variable& var) {
				return LIB::format("[name=\"{}\",type=\"{}\"]={}", var.name, var.type, var.data);
			}
			_argPack readArg(std::string str) {
				_argPack rt;    ui64 i = 0;
				while (str[0] == ' ' || str[0] == '\n') { str.replace(0, 1, ""); rt.sz++; }
				if (str[0] == '[') { str.replace(0, 1, ""); rt.sz++; }
				else if (str[0] == ']') return rt;
				else {
					while (str[0] == ' ' || str[0] == '\n') { str.replace(0, 1, ""); rt.sz++; }
					if (str[0] == ',') { str.replace(0, 1, ""); rt.sz++; }
				}
				while (str[0] == ' ' || str[0] == '\n') { str.replace(0, 1, ""); rt.sz++; }

				__labelPack _name = read_label(str);
				rt.sz += _name.sz + 2;
				for (ui64 i = 0; i < _name.sz + 2; i++) {
					str.replace(0, 1, "");
				}
				rt.name = _name;

				while (str[0] == ' ' || str[0] == '\n') { str.replace(0, 1, ""); rt.sz++; }
				if (str[0] != '=') throw arg_readError;
				str.replace(0, 1, ""); rt.sz++;
				while (str[0] == ' ' || str[0] == '\n') { str.replace(0, 1, ""); rt.sz++; }

				__labelPack _msg = read_label(str);
				rt.sz += _msg.sz + 2;
				for (ui64 i = 0; i < _msg.sz + 2; i++) {
					str.replace(0, 1, "");
				}
				rt.msg = _msg;
				rt.null = false;
				return rt;
			}

			std::vector<_argPack> readArgs(std::string str) {
				std::vector<_argPack> ret;   _argPack boof;
				while (true) {
					boof = readArg(str);
					if (boof.null) break;
					for (ui64 o = 0; o < boof.sz; o++) str.replace(0, 1, "");
					ret.resize(ret.size() + 1);
					ret[ret.size() - 1] = boof;
				}
				return ret;
			}

			variable readVariable(std::string str) {
				variable ret;
				ui64 i = 0;   std::string boof;
				while (str[0] != '[') str.replace(0, 1, "");

				return ret;
			}
		};
	}

	struct basic_variable {
		std::string name;
		std::string type;
		void* data;
	};


}