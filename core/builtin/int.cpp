#include "int.h"
#include "_string.h"
#include "bool.h"
#include "char.h"
#include "void.h"

std::unique_ptr<root> root_int::convert_type(const std::string& t)
{
	SWITCH_BEGIN(t)
		SWITCH_CASE("string")
	{
		return _make_unique<root_string>(std::to_string(d));
	}
	SWITCH_CASE("bool")
	{
		return _make_unique<root_bool>(d);
	}
	SWITCH_CASE("char")
	{
		return _make_unique<root_char>(char(d));
	}
	SWITCH_DEFAULT
		return std::unique_ptr<root_void>();
	SWITCH_END
}