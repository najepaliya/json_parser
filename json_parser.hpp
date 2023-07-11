#pragma once

#include <cstdint>
#include <iostream>
#include <stack>
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

enum symbol: uint_fast8_t
{
	// terminals
	end, whitespace, quote, space, character, backslash, escape,
	// nonterminals
	nt_end, nt_primitive, nt_whitespace, nt_value, nt_string, nt_characters, nt_escape
};

void json::parse (std::string& buffer)
{
	// erase index before parsing
	clear();

	// append null terminator
	buffer.push_back ('\0');

	uint_fast8_t rule_table [7][7] =
	{
/*  $ ws  " sp ch  \ esc  */
		4, 0, 0, 0, 0, 0,  0, // end  0
		7, 7, 7, 7, 7, 7,  0, // prim 1 FIX LATER
		1, 3, 1, 3, 0, 0,  0, // ws   2
		0, 5, 5, 0, 0, 0,  0, // val  3
		0, 0, 6, 0, 0, 0,  0, // str  4
		0, 0, 2, 3, 3, 8,  3, // chrs 5
		0, 0, 2, 0, 0, 2,  2, // esc  6
	};
	
	// initialize symbol stack
	std::stack<uint_fast8_t> symbols;
	symbols.push (nt_end);
	symbols.push (nt_whitespace);
	symbols.push (nt_value);
	symbols.push (nt_whitespace);

	// initialize container stack
	std::stack<token*> containers;
	containers.push (&root);

	// initialize index stack
	std::stack<int> indexes;
	
	for (int index = 0; ; index += 1)
	{
		uint_fast8_t terminal = character;

		switch (buffer[index])
		{
			case '\0':
				terminal = end;
				break;
			case '\n': case '\r': case '\t':
				terminal = whitespace;
				break;
			case '"':
				terminal = quote;
				break;
			case ' ':
				terminal = space;
				break;
			case '\\':
				terminal = backslash;
				break;
			case '/': case 'b': case 'f': case 'n': case 'r': case 't':
				terminal = escape;
				break;
		}

		switch (rule_table[symbols.top() % 7][terminal % 7])
		{
			case 0: // error
				std::cout << "==ERROR: " << index << "->'" << buffer[index] << "'==\n";
				clear();
				buffer.pop_back();
				return;
			case 1: // null
				std::cout << "==NULL==\n";
				symbols.pop();
				index -= 1;
				break;
			case 2: // match
				symbols.pop();
				break;
			case 4: // end
				std::cout << "==VALID==\n";
				buffer.pop_back();
				return;
			case 5: // string value
				symbols.pop();
				symbols.push (nt_string);
				index -= 1;
				break;
			case 6: // decompose string
				symbols.pop();
				indexes.push (index);
				symbols.push (nt_primitive);
				symbols.push (nt_characters);
				break;
			case 7: // store primitive
				containers.top()->children.emplace_back (buffer.substr (indexes.top(), index - indexes.top()));
				indexes.pop();
				symbols.pop();
				index -= 1;
				break;
			case 8:
				symbols.push (nt_escape);
				break;
		}
	}
}