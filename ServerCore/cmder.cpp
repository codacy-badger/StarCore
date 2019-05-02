﻿#include "cmder.h"
#include <algorithm>
#include <iostream>
#include "basic_types.h"
cmder::cmder()
{
	this->insert("builtin", new data_container(new builtin));
}

void cmder::run()
{
	for (size_t i = 0; i < commands.size(); ++i)
	{
		DEB(TS_ID_16 + commands[i]);
		auto x = convert_var(commands[i]);
		if (x != nullptr)
			delete x;
	}
}
data_container* cmder::member_access(std::string name)
{
	if (name.find("->") != name.npos)
	{
		auto s = SplitString(name, "->");
		if (s.size() < 2)
		{
			ERR(TS_ID_32);
			return nullptr;
		}
		data_container* p = this->get(s[0]);
		for (size_t i = 1; i < s.size() && p != nullptr; i++)
		{
			if (p != nullptr && p->get() != nullptr)
				p = p->get()->access_member(s[i]);
			else
				ERR(TS_ID_31 + name);
		}
		if (p == nullptr || p->get() == nullptr) {
			ERR(TS_ID_31 + name);
			return nullptr;
		}
		return p->copy();
	}
	else
	{
		return (*this)["builtin"]->get()->access_member(name)->copy();
	}
}
std::pair<std::string, std::vector<std::string>> cmder::ProcessCmd(std::string str) const
{
	std::pair<std::string, std::vector<std::string>> buf;
	if (str.find("(") == std::string::npos)
	{
		//DEB(TS_ID_15 "(");
		buf.first.append(str);
		return buf;
	}
	if (std::count(str.begin(), str.end(), '(') != std::count(str.begin(), str.end(), ')'))
	{
		ERR(TS_ID_15);
		return buf;
	}
	buf.first = database::SplitString(str, "(")[0];
	str = str.substr(str.find_first_of('(') + 1);
	str = str.erase(str.find_last_of(')'));
	std::vector<std::string> args = database::SplitString(str, ","), buf_;
	for (size_t t = 0; t < args.size(); ++t) {
		if (buf_.size() == 0)
		{
			buf_.push_back(args[t]);
			continue;
		}
		auto x = buf_[(buf_.size() - 1)];
		if (std::count(x.begin(), x.end(), '(') != std::count(x.begin(), x.end(), ')'))
		{
			x.append("," + args[t]);
			buf_.erase(buf_.end() - 1);
			buf_.push_back(x);
		}
		else
		{
			buf_.push_back(args[t]);
		}
	}
	buf.second = buf_;
	return buf;
}
data_container* cmder::convert_var(std::string token)
{
	auto x = ProcessCmd(token);
	if (token.find("(") == std::basic_string<char, std::char_traits<char>, std::allocator<char>>::npos)
	{
		return member_access(x.first);
	}
	else
	{
		_SWITCH_BEGIN(x.first)
			_SWITCH_CASE("string")
		{
			std::string str;
			for (const auto& i : x.second)
				str += ((!str.empty()) ? "," : "") + i;
			return new data_container(new data_string(str));
		}
		_SWITCH_CASE("int")
		{
			std::string str;
			for (const auto& i : x.second)
				str += ((!str.empty()) ? "," : "") + i;
			return new data_container(new data_int(std::stoi(str)));
		}
		_SWITCH_CASE("void")
		{
			return new data_container(new data_void());
		}
		_SWITCH_DEFAULT{
			cmd c;
			for (size_t i = 0; i < x.second.size(); ++i)
				c.second.push_back(convert_var(x.second[i]));
			c.first = x.first;
			DEB(TS_ID_30 + c.first);
			auto t = member_access(c.first);
			if (t != nullptr)
			{
				if (t->get() != nullptr) {
					auto n = t;
					t = t->get()->execute(c.second);
					delete n;
				}
				else {
					ERR(TS_ID_31 + c.first);
					delete t;
					t = nullptr;
				}
			}
			else
			{
				ERR(TS_ID_31 + c.first);
			}
			if (t == nullptr)
			{
				ERR(TS_ID_33 + c.first);
			}
			for (size_t i = 0; i < c.second.size(); ++i)
			{
					delete c.second[i];
			}
			return t;
		}
			_SWITCH_END
	}
}