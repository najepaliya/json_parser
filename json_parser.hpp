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
	end, whitespace, n, u, l, f, a, s, e, t, r, minus, zero, onenine, dot, E, plus, quote, space, character, backslash, slash, b, hex,
	// nonterminals
	nt_end, nt_primitive, nt_whitespace, nt_value, nt_u, nt_l, nt_a, nt_s, nt_e, nt_r, nt_minus, nt_integer, nt_digits, nt_fraction, nt_digit, nt_sign, nt_exponent, nt_string, nt_characters, nt_escape, nt_hex
};

void json::parse (std::string& buffer)
{
	// erase index before parsing
	clear();

	// append null terminator
	buffer.push_back ('\0');

	uint_fast8_t rule_table [21][24] =
	{
/*  $ ws  n   u  l  f  a  s   e  t  r  -   0  19   .   E  +   "  sp  ch   \  /  b hx  */
		4, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0,  0,  0,  0,  0, 0,  0,  0,  0,  0, 0, 0, 0, // end   0
		5, 5, 5,  5, 5, 5, 5, 5,  5, 5, 5, 5,  5,  5,  5,  5, 5,  0,  5,  5,  5, 5, 5, 0, // prim  1
		1, 3, 1,  0, 0, 1, 0, 0,  0, 1, 0, 1,  1,  1,  0,  0, 0,  1,  3,  0,  0, 0, 0, 0, // ws    2
		0, 0, 6,  0, 0, 7, 0, 0,  0, 8, 0, 9,  9,  9,  0,  0, 0, 13,  0,  0,  0, 0, 0, 0, // val   3
		0, 0, 0,  2, 0, 0, 0, 0,  0, 0, 0, 0,  0,  0,  0,  0, 0,  0,  0,  0,  0, 0, 0, 0, // u     4
		0, 0, 0,  0, 2, 0, 0, 0,  0, 0, 0, 0,  0,  0,  0,  0, 0,  0,  0,  0,  0, 0, 0, 0, // l     5
		0, 0, 0,  0, 0, 0, 2, 0,  0, 0, 0, 0,  0,  0,  0,  0, 0,  0,  0,  0,  0, 0, 0, 0, // a     6
		0, 0, 0,  0, 0, 0, 0, 2,  0, 0, 0, 0,  0,  0,  0,  0, 0,  0,  0,  0,  0, 0, 0, 0, // s     7
		0, 0, 0,  0, 0, 0, 0, 0,  2, 0, 0, 0,  0,  0,  0,  0, 0,  0,  0,  0,  0, 0, 0, 0, // e     8
		0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 2, 0,  0,  0,  0,  0, 0,  0,  0,  0,  0, 0, 0, 0, // r     9
		0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 2,  1,  1,  0,  0, 0,  0,  0,  0,  0, 0, 0, 0, // min  10
		0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0,  2, 10,  0,  0, 0,  0,  0,  0,  0, 0, 0, 0, // int  11
		1, 1, 0,  0, 0, 0, 0, 0,  1, 0, 0, 0,  3,  3,  1,  1, 0,  0,  1,  0,  0, 0, 0, 0, // digs 12
		0, 1, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0,  0,  0, 11,  0, 0,  0,  0,  0,  0, 0, 0, 0, // frc  13
		0, 0, 0,  0, 0, 0, 0, 0,  2, 0, 0, 0,  2,  2,  0,  0, 0,  0,  0,  0,  0, 0, 0, 0, // dig  14
		0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 2,  1,  1,  0,  0, 2,  0,  0,  0,  0, 0, 0, 0, // sig  15
		1, 1, 0,  0, 0, 0, 0, 0, 12, 0, 0, 0,  0,  0,  0, 12, 0,  0,  0,  0,  0, 0, 0, 0, // exp  16
		0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0,  0,  0,  0,  0, 0, 14,  0,  0,  0, 0, 0, 0, // str  17
		0, 0, 3,  3, 3, 3, 3, 3,  3, 3, 3, 3,  3,  3,  3,  3, 3,  2,  3,  3, 15, 3, 3, 3, // chrs 18
		0, 0, 2, 16, 0, 2, 0, 0,  0, 2, 2, 0,  0,  0,  0,  0, 0,  2,  0,  0,  2, 2, 2, 0, // esc  19
		0, 0, 0,  0, 0, 2, 2, 0,  2, 0, 0, 0,  2,  2,  0,  2, 0,  0,  0,  0,  0, 0, 2, 2, // hex  20
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
			case '-':
				terminal = minus;
				break;
			case '0':
				terminal = zero;
				break;
			case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
				terminal = onenine;
				break;
			case '.':
				terminal = dot;
				break;
			case 'E':
				terminal = E;
				break;
			case '+':
				terminal = plus;
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
			case 'A': case 'B': case 'C': case 'D': case 'F': case 'c': case 'd':
				terminal = hex;
				break;
		}

		switch (rule_table[symbols.top() % 24][terminal % 24])
		{
			case 0: // error
				std::cout << buffer << "\n";
				std::cout << "t: '" << buffer[index] << "' i: " << index << " r: " << symbols.top() % 24 << "\n";
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
				symbols.push (nt_exponent);
				symbols.push (nt_fraction);
				symbols.push (nt_integer);
				symbols.push (nt_minus);
				index -= 1;
				break;
			case 10: // continue int
				symbols.pop();
				symbols.push (nt_digits);
				break;
			case 11: // fractional number
				symbols.pop();
				symbols.push (nt_digits);
				symbols.push (nt_digit);
				break;
			case 12: // exponential number
				symbols.pop();
				symbols.push (nt_digits);
				symbols.push (nt_digit);
				symbols.push (nt_sign);
				break;
			case 13: // string value
				symbols.pop();
				symbols.push (nt_string);
				index -= 1;
				break;
			case 14: // decompose string
				symbols.pop();
				indexes.push (index);
				symbols.push (nt_primitive);
				symbols.push (nt_characters);
				break;
			case 15: // escape sequence
				symbols.push (nt_escape);
				break;
			case 16: // unicode sequence
				symbols.pop();
				symbols.push (nt_hex);
				symbols.push (nt_hex);
				symbols.push (nt_hex);
				symbols.push (nt_hex);
				break;
		}
	}
}