#pragma once

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
  public:
  	void parse (std::string& buffer);
};

void json::parse (std::string& buffer)
{
}