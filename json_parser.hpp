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
	end, whitespace, n, u, l, f, a, s, e, t, r, // quote, space, character, backslash, escape, u, hex,
	// nonterminals
	nt_end, nt_primitive, nt_whitespace, nt_value, nt_u, nt_l, nt_a, nt_s, nt_e, nt_r // nt_string, nt_characters, nt_escape, nt_hex
};

void json::parse (std::string& buffer)
{
	// erase index before parsing
	clear();

	// append null terminator
	buffer.push_back ('\0');

	uint_fast8_t rule_table [10][11] =
	{
/*  $ ws  n  u  l  f  a  s  e  t  r  */
		4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // end  0
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, // prim 1
		1, 3, 1, 0, 0, 1, 0, 0, 0, 1, 0, // ws   2
		0, 0, 6, 0, 0, 7, 0, 0, 0, 8, 0, // val  3
		0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, // u    4
		0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, // l    5
		0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, // a    6
		0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, // s    7
		0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, // e    8
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, // r    9
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
		uint_fast8_t terminal; //= character;

		switch (buffer[index])
		{
			case '\0':
				terminal = end;
				break;
			case ' ': case '\n': case '\r': case '\t':
				terminal = whitespace;
				break;
			case 'n':
				terminal = n;
				break;
			case 'u':
				terminal = u;
				break;
			case 'l':
				terminal = l;
				break;
			case 'f':
				terminal = f;
				break;
			case 'a':
				terminal = a;
				break;
			case 's':
				terminal = s;
				break;
			case 'e':
				terminal = e;
				break;
			case 't':
				terminal = t;
				break;
			case 'r':
				terminal = r;
				break;
			// case '"':
			// 	terminal = quote;
			// 	break;
			// case ' ':
			// 	terminal = space;
			// 	break;
			// case '\\':
			// 	terminal = backslash;
			// 	break;
			// case '/': case 'b': case 'f': case 'n': case 'r': case 't':
			// 	terminal = escape;
			// 	break;
			// case 'u':
			// 	terminal = u;
			// 	break;
			// case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case 'a': case 'c': case 'd': case 'e': case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
			// 	terminal = hex;
			// 	break;
		}

		switch (rule_table[symbols.top() % 11][terminal % 11])
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
			case 5: // store primitive
				containers.top()->children.emplace_back (buffer.substr (indexes.top(), index - indexes.top()));
				indexes.pop();
				symbols.pop();
				index -= 1;
				break;
			case 6: // null value
				symbols.pop();
				// symbols.push (nt_primitive);
				symbols.push (nt_l);
				symbols.push (nt_l);
				symbols.push (nt_u);
				break;
			case 7: // null value
				symbols.pop();
				// symbols.push (nt_primitive);
				symbols.push (nt_e);
				symbols.push (nt_s);
				symbols.push (nt_l);
				symbols.push (nt_a);
				break;
			case 8: // null value
				symbols.pop();
				// symbols.push (nt_primitive);
				symbols.push (nt_e);
				symbols.push (nt_u);
				symbols.push (nt_r);
				break;
			// case 5: // string value
			// 	symbols.pop();
			// 	symbols.push (nt_string);
			// 	index -= 1;
			// 	break;
			// case 6: // decompose string
			// 	symbols.pop();
			// 	indexes.push (index);
			// 	symbols.push (nt_primitive);
			// 	symbols.push (nt_characters);
			// 	break;
			
			// case 8:
			// 	symbols.push (nt_escape);
			// 	break;
			// case 9:
			// 	symbols.push (nt_hex);
			// 	symbols.push (nt_hex);
			// 	symbols.push (nt_hex);
			// 	symbols.push (nt_hex);
			// 	break;
		}
	}
}