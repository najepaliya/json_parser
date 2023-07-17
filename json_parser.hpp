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
		void parse (std::string& buffer, bool members);
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
	end, whitespace, quote, space, character, backslash, u, hex, openbrace, colon, closebrace, comma, openbracket, closebracket, minus, zero, onenine, dot, E, e, plus, n, l, f, a, s, t, r, slash, b,
	// nonterminals
	nt_end, nt_whitespace, nt_value, nt_string, nt_characters, nt_escape, nt_hex, nt_primitive, nt_headmember, nt_colon, nt_container, nt_memberlist, nt_headelement, nt_elementlist, nt_minus, nt_integer, nt_digits, nt_fraction, nt_digit, nt_exponent, nt_sign, nt_u, nt_l, nt_a, nt_s, nt_e, nt_r
};

void json::parse (std::string& buffer, bool members)
{
	// erase index before parsing
	clear();

	// append null terminator
	buffer.push_back ('\0');

	uint_fast8_t rule_table [27][nt_end] =
	{
	/* $  ws   "  sp  ch   \   u  hx   {   :   }   ,   [   ]   -   0  19   .   E   e   +   n   l   f   a   s   t   r   /   b  */
		 4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // end   0
		 1,  3,  1,  3,  0,  0,  0,  0,  1,  1,  0,  0,  1,  0,  1,  1,  1,  0,  0,  0,  0,  1,  0,  1,  0,  0,  1,  0,  0,  0, // ws    1
		 0,  0,  5,  0,  0,  0,  0,  0, 10,  0,  0,  0, 14,  0, 17, 17, 17,  0,  0,  0,  0, 21,  0, 22,  0,  0, 23,  0,  0,  0, // val   2
		 0,  0,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // str   3
		 0,  0,  2,  3,  3,  7,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3, // chrs  4 
		 0,  0,  2,  0,  0,  2,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  0,  2,  0,  0,  2,  2,  2,  2, // esc   5
		 0,  0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  2,  2,  0,  2,  2,  0,  0,  0,  2,  2,  0,  0,  0,  0,  2, // hex   6
		 9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9, // prim  7
		 0,  3, 11,  3,  0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // hmem  8
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // col   9
		12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, // cont 10
		 0,  3,  0,  3,  0,  0,  0,  0,  0,  0,  2, 13,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // meml 11
		 0,  3, 15,  3,  0,  0,  0,  0, 15,  0,  0,  0, 15,  2, 15, 15, 15,  0,  0,  0,  0, 15,  0, 15,  0,  0, 15,  0,  0,  0, // helm 12
		 0,  3,  0,  3,  0,  0,  0,  0,  0,  0,  0, 16,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // elml 13
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // min  14
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2, 18,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // int  15
		 1,  1,  0,  1,  0,  0,  0,  0,  0,  0,  1,  1,  0,  0,  0,  3,  3,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // digs 16
		 1,  1,  0,  1,  0,  0,  0,  0,  0,  0,  1,  1,  0,  1,  0,  0,  0, 19,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // frac 17
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // dig  18
		 1,  1,  0,  1,  0,  0,  0,  0,  0,  0,  1,  1,  0,  1,  0,  0,  0,  0, 20, 20,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // exp  19
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  1,  1,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0, // sig  20
		 0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // u    21
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0,  0, // l    22
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0, // a    23
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0, // s    24
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // e    25
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  0,  0, // r    26
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
			case 'u':
				terminal = u;
				break;
			case 'A': case 'B': case 'C': case 'D': case 'F': case 'c': case 'd':
				terminal = hex;
				break;
			case '{':
				terminal = openbrace;
				break;
			case ':':
				terminal = colon;
				break;
			case '}':
				terminal = closebrace;
				break;
			case ',':
				terminal = comma;
				break;
			case '[':
				terminal = openbracket;
				break;
			case ']':
				terminal = closebracket;
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
			case 'e':
				terminal = e;
				break;
			case 'E':
				terminal = E;
				break;
			case '+':
				terminal = plus;
				break;
			case 'n':
				terminal = n;
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
			case 't':
				terminal = t;
				break;
			case 'r':
				terminal = r;
				break;
			case '/':
				terminal = slash;
				break;
			case 'b':
				terminal = b;
				break;
		}

		switch (rule_table[symbols.top() % nt_end][terminal % nt_end])
		{
			case 0: // error
				std::cout << "t: '" << buffer[index] << "' i: " << index << " r: " << symbols.top() % nt_end << "\n";
				clear();
				buffer.pop_back();
				return;
			case 1: // null
				symbols.pop();
				index -= 1;
				break;
			case 2: // match
				symbols.pop();
				break;
			case 4: // end
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
			case 7: // escape sequence
				symbols.push (nt_escape);
				break;
			case 8: // unicode sequence
				symbols.pop();
				symbols.push (nt_hex);
				symbols.push (nt_hex);
				symbols.push (nt_hex);
				symbols.push (nt_hex);
				break;
			case 9: // store primitive
				containers.top()->children.emplace_back (buffer.substr (indexes.top(), index - indexes.top()));
				symbols.pop();
				indexes.pop();
				index -= 1;
				break;
			case 10: // object value and container
				symbols.pop();
				symbols.push (nt_container);
				symbols.push (nt_headmember);
				containers.top()->children.emplace_back ("{}");
				containers.push (&(containers.top()->children.back()));
				break;
			case 11: // load member and container
				symbols.pop();
				symbols.push (nt_memberlist);
				if (members)
				{
					symbols.push (nt_container);
					containers.top()->children.emplace_back ("");
        	containers.push (&(containers.top()->children.back()));
				}
				symbols.push (nt_value);
				symbols.push (nt_whitespace);
				symbols.push (nt_colon);
				symbols.push (nt_whitespace);
				symbols.push (nt_string);
				index -= 1;
				break;
			case 12: // unload container
				symbols.pop();
				containers.pop();
				index -= 1;
				break;
			case 13: // load additional member and container
				symbols.pop();
				symbols.push (nt_memberlist);
				if (members)
				{
					symbols.push (nt_container);
					containers.top()->children.emplace_back ("");
        	containers.push (&(containers.top()->children.back()));
				}
				symbols.push (nt_value);
				symbols.push (nt_whitespace);
				symbols.push (nt_colon);
				symbols.push (nt_whitespace);
				symbols.push (nt_string);
				symbols.push (nt_whitespace);
				break;
			case 14: // array value and container
				symbols.pop();
				symbols.push (nt_container);
				symbols.push (nt_headelement);
				containers.top()->children.emplace_back ("[]");
				containers.push (&(containers.top()->children.back()));
				break;
			case 15: // parse value and load element list
				symbols.pop();
				symbols.push (nt_elementlist);
				symbols.push (nt_value);
				index -= 1;
				break;
			case 16: // load additional element
				symbols.pop();
				symbols.push (nt_elementlist);
				symbols.push (nt_value);
				symbols.push (nt_whitespace);
				break;
			case 17: // number value
				symbols.pop();
				indexes.push (index);
				symbols.push (nt_primitive);
				symbols.push (nt_exponent);
				symbols.push (nt_fraction);
				symbols.push (nt_integer);
				symbols.push (nt_minus);
				index -= 1;
				break;
			case 18: // continue integer
				symbols.pop();
				symbols.push (nt_digits);
				break;
			case 19: // fractional number
				symbols.pop();
				symbols.push (nt_digits);
				symbols.push (nt_digit);
				break;
			case 20: // exponential number
				symbols.pop();
				symbols.push (nt_digits);
				symbols.push (nt_digit);
				symbols.push (nt_sign);
				break;
			case 21: // null value
				symbols.pop();
				indexes.push (index);
				symbols.push (nt_primitive);
				symbols.push (nt_l);
				symbols.push (nt_l);
				symbols.push (nt_u);
				break;
			case 22: // false value
				symbols.pop();
				indexes.push (index);
				symbols.push (nt_primitive);
				symbols.push (nt_e);
				symbols.push (nt_s);
				symbols.push (nt_l);
				symbols.push (nt_a);
				break;
			case 23: // true value
				symbols.pop();
				indexes.push (index);
				symbols.push (nt_primitive);
				symbols.push (nt_e);
				symbols.push (nt_u);
				symbols.push (nt_r);
				break;
		}
	}
}
