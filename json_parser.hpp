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
	end, whitespace, n, u, l, f, a, s, e, t, r, digit, dot, E, sign, quote, space, character, backslash, slash, b,
	// nonterminals
	nt_end, nt_primitive, nt_whitespace, nt_value, nt_u, nt_l, nt_a, nt_s, nt_e, nt_r, nt_integer, nt_digit, nt_fraction, nt_sign, nt_exponent, nt_string, nt_characters, nt_escape // nt_string, nt_characters, nt_escape, nt_hex
};

void json::parse (std::string& buffer)
{
	// erase index before parsing
	clear();

	// append null terminator
	buffer.push_back ('\0');

	uint_fast8_t rule_table [18][21] =
	{
/*  $ ws  n  u  l  f  a  s   e  t  r  #   .   E +-   "  sp  ch   \  /  b  */
		4, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0,  0,  0, 0,  0,  0,  0,  0, 0, 0, // end   0
		5, 5, 5, 5, 5, 5, 5, 5,  5, 5, 5, 5,  5,  5, 5,  5,  5,  5,  0, 0, 0, // prim  1
		1, 3, 1, 0, 0, 1, 0, 0,  0, 1, 0, 1,  0,  0, 0,  1,  3,  0,  0, 0, 0, // ws    2
		0, 0, 6, 0, 0, 7, 0, 0,  0, 8, 0, 9,  0,  0, 0, 12,  0,  0,  0, 0, 0, // val   3
		0, 0, 0, 2, 0, 0, 0, 0,  0, 0, 0, 0,  0,  0, 0,  0,  0,  0,  0, 0, 0, // u     4
		0, 0, 0, 0, 2, 0, 0, 0,  0, 0, 0, 0,  0,  0, 0,  0,  0,  0,  0, 0, 0, // l     5
		0, 0, 0, 0, 0, 0, 2, 0,  0, 0, 0, 0,  0,  0, 0,  0,  0,  0,  0, 0, 0, // a     6
		0, 0, 0, 0, 0, 0, 0, 2,  0, 0, 0, 0,  0,  0, 0,  0,  0,  0,  0, 0, 0, // s     7
		0, 0, 0, 0, 0, 0, 0, 0,  2, 0, 0, 0,  0,  0, 0,  0,  0,  0,  0, 0, 0, // e     8
		0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 2, 0,  0,  0, 0,  0,  0,  0,  0, 0, 0, // r     9
		1, 1, 0, 0, 0, 0, 0, 0, 11, 0, 0, 3, 10, 11, 0,  0,  1,  0,  0, 0, 0, // int  10
		0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 2,  0,  0, 0,  0,  0,  0,  0, 0, 0, // dig  11
		1, 1, 0, 0, 0, 0, 0, 0, 11, 0, 0, 3,  0, 11, 0,  0,  1,  0,  0, 0, 0, // frac 12
		0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 1,  0,  0, 2,  0,  0,  0,  0, 0, 0, // sign 13
		1, 1, 0, 0, 0, 0, 0, 0,  0, 0, 0, 3,  0,  0, 0,  0,  1,  0,  0, 0, 0, // exp  14
		0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 13,  0,  0,  0, 0, 0, // str  15
		0, 0, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3,  3,  3, 3,  2,  3,  3, 14, 0, 0, // chrs 16
		0, 0, 2, 0, 0, 2, 0, 0,  0, 2, 2, 0,  0,  0, 0,  2,  0,  0,  2, 2, 2, // esc  17
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
			case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
				terminal = digit;
				break;
			case '.':
				terminal = dot;
				break;
			case 'E':
				terminal = E;
				break;
			case '+': case '-':
				terminal = sign;
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
			case '/':
				terminal = slash;
				break;
			case 'b':
				terminal = b;
				break;
			// case '"':
			// 	terminal = quote;
			// 	break;
			// case ' ':
			// 	terminal = space;
			// 	break;
			
			// case 'u':
			// 	terminal = u;
			// 	break;
			// case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case 'a': case 'c': case 'd': case 'e': case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
			// 	terminal = hex;
			// 	break;
		}

		switch (rule_table[symbols.top() % 21][terminal % 21])
		{
			case 0: // error
				std::cout << buffer << "\n";
				std::cout << "t: '" << buffer[index] << "' i: " << index << " r: " << symbols.top() % 18 << "\n";
				// std::cout << "==ERROR: " << index << "->'" << buffer[index] << "'==\n";
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
				indexes.push (index);
				symbols.push (nt_primitive);
				symbols.push (nt_l);
				symbols.push (nt_l);
				symbols.push (nt_u);
				break;
			case 7: // null value
				symbols.pop();
				indexes.push (index);
				symbols.push (nt_primitive);
				symbols.push (nt_e);
				symbols.push (nt_s);
				symbols.push (nt_l);
				symbols.push (nt_a);
				break;
			case 8: // null value
				symbols.pop();
				indexes.push (index);
				symbols.push (nt_primitive);
				symbols.push (nt_e);
				symbols.push (nt_u);
				symbols.push (nt_r);
				break;
			case 9: // number value
				symbols.pop();
				indexes.push (index);
				symbols.push (nt_primitive);
				symbols.push (nt_integer);
				break;
			case 10: // fractional number
				symbols.pop();
				symbols.push (nt_fraction);
				symbols.push (nt_digit);
				break;
			case 11: // exponential value
				symbols.pop();
				symbols.push (nt_exponent);
				symbols.push (nt_digit);
				symbols.push (nt_sign);
				break;
			case 12: // string value
				symbols.pop();
				symbols.push (nt_string);
				index -= 1;
				break;
			case 13: // decompose string
				symbols.pop();
				indexes.push (index);
				symbols.push (nt_primitive);
				symbols.push (nt_characters);
				break;
			case 14: // escape sequence
				symbols.push (nt_escape);
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