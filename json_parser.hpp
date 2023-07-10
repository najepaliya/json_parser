#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

class token
{
  public:
    int8_t type;
    std::string string;
    std::vector<token> children;
    token();
    token (std::string s);
};

token::token()
{
}

token::token (std::string s): string (std::move (s))
{
}

class json
{
  private:
    token root;
    void clear();
  public:
    json();
    void parse (std::string& buffer);
    token* index();
};

json::json()
{
  root = token ("");
}

token* json::index()
{
  return &root;
}

void json::clear()
{
  root.children.clear();
}

enum symbols: uint_fast8_t
{
	// terminals
	end,
	// nonterminals
	nt_end
};

void json::parse (std::string& buffer)
{
	// append null terminator
	buffer.push_back ('\0');

	// remove null terminator
	buffer.pop_back();
}