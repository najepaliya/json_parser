#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <stack>
#include <iostream>

// use stack of node* duirng parsing to assign children

class node
{
    public:
        int_fast8_t type = 0;
        std::string value = "";
        std::vector<node> children = {};
};

class json
{
    public:
        node root;
        void parse (std::string& buffer);
};

enum symbol : uint_fast8_t
{
    // tokens
    end, whitespace, n, f, t, digit, minus, dot, Ee, quote, character, backslash, openbrace, closebrace, colon, comma, openbracket, closebracket,
    // nonterminals
    nt_primitive, nt_containter, nt_whitespace, nt_value, nt_end, nt_digits, nt_fraction, nt_exponent, nt_characters, nt_headmember, /*nt_closebrace,*/ nt_colon, nt_memberlist, nt_quote, nt_headelement, nt_elementlist,
};

void json::parse (std::string& buffer)
{
    /*repeating match: 3*/
    uint_fast8_t rule_table [15][18] =
    {/* $ ws   n   f   t   #   -  . Ee   " ch   \   {  }  :   ,   [   ]*/
        0, 0,  0,  0,  0,  0,  0, 0, 0,  0, 0,  0,  0, 0, 0,  0,  0,  0, /*prim 0*/
        0, 0,  0,  0,  0,  0,  0, 0, 0,  0, 0,  0,  0, 0, 0,  0,  0,  0, /*cont 1*/
        1, 3,  1,  1,  1,  1,  1, 0, 0,  1, 0,  0,  1, 1, 1,  1,  1,  1, /*ws   2*/
        0, 0,  4,  5,  6,  7,  7, 0, 0,  9, 0,  0, 11, 0, 0,  0, 13,  0, /*val  3*/
        2, 0,  0,  0,  0,  0,  0, 0, 0,  0, 0,  0,  0, 0, 0,  0,  0,  0, /*end  4*/
        1, 1,  0,  0,  0,  3,  0, 1, 1,  0, 0,  0,  0, 1, 0,  1,  0,  0, /*digs 5*/
        1, 1,  0,  0,  0,  0,  0, 8, 1,  0, 0,  0,  0, 1, 0,  1,  0,  0, /*frac 6*/
        1, 1,  0,  0,  0,  0,  0, 0, 8,  0, 0,  0,  0, 1, 0,  1,  0,  0, /*exp  7*/
        0, 3,  3,  3,  3,  3,  3, 3, 3,  2, 3, 10,  3, 3, 3,  3,  3,  3, /*chs  8*/
        0, 0,  0,  0,  0,  0,  0, 0, 0, 12, 0,  0,  0, 2, 0,  0,  0,  0, /*hmem 9*/
        // 0, 0,  0,  0,  0,  0,  0, 0, 0,  0, 0,  0,  0, 2, 0,  0,  0,  0, /*cbrc 10 may not be needed*/
        0, 0,  0,  0,  0,  0,  0, 0, 0,  0, 0,  0,  0, 0, 2,  0,  0,  0, /*col  11*/
        0, 0,  0,  0,  0,  0,  0, 0, 0,  0, 0,  0,  0, 2, 0, 12,  0,  0, /*meml 12*/
        0, 0,  0,  0,  0,  0,  0, 0, 0,  2, 0,  0,  0, 0, 0,  0,  0,  0, /*quo  13*/
        0, 0, 14, 14, 14, 14, 14, 0, 0, 14, 0,  0, 14, 0, 0,  0, 14,  2, /*helm 14*/
        0, 0,  0,  0,  0,  0,  0, 0, 0,  0, 0,  0,  0, 0, 0, 14,  0,  2, /*elml 15*/
    };

    std::stack<uint_fast8_t> stack;
    stack.push (nt_end);
    stack.push (nt_whitespace);
    stack.push (nt_value);
    stack.push (nt_whitespace);

    buffer += -1;
    
    for (int index = 0; index < buffer.size(); index += 1)
    {
        uint_fast8_t token = character;
        
        switch (buffer[index])
        {
            case -1:
                token = end;
                break;
            case ' ': case '\n': case '\r': case '\t':
                token = whitespace;
                break;
            case 'n':
                token = n;
                break;
            case 'f':
                token = f;
                break;
            case 't':
                token = t;
                break;
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                token = digit;
                break;
            case '-':
                token = minus;
                break;
            case '.':
                token = dot;
                break;
            case 'E': case 'e':
                token = Ee;
                break;
            case '"':
                token = quote;
                break;
            case '\\':
                token = backslash;
                break;
            case '{':
                token = openbrace;
                break;
            case '}':
                token = closebrace;
                break;
            case ':':
                token = colon;
                break;
            case ',':
                token = comma;
                break;
            case '[':
                token = openbracket;
                break;
            case ']':
                token = closebracket;
                break;
        }

        switch (rule_table[stack.top() % 18][token % 18])
        {
            case 0: // error
                std::cout << "FAIL | INDEX: " << index << " | TOKEN: " << int(token) << " | STACK: " << int(stack.top() % 18) << "\n";
                exit (1);
                break;
            case 1: // null
                std::cout << "NULL | INDEX: " << index << " POPPED: " << int(stack.top() % 18) << "\n";
                stack.pop ();
                index -= 1;
                break;
            case 2: // match
                std::cout << "MATCH | INDEX: " << index << " POPPED: " << int(stack.top() % 18) << "\n";
                stack.pop ();
                break;
            case 4: // null value
                if (buffer[index + 1] == 'u' && buffer[index + 2] == 'l' && buffer[index + 3] == 'l')
                {
                    stack.pop();
                    index += 3;
                    break;
                }
                std::cout << "ERROR: EXPECTED NULL | RULE 4 | INDEX: " << index << "\n";
                exit (1);
            case 5: // false value
                if (buffer[index + 1] == 'a' && buffer[index + 2] == 'l' && buffer[index + 3] == 's' && buffer[index + 4] == 'e')
                {
                    stack.pop();
                    index += 4;
                    break;
                }
                std::cout << "ERROR: EXPECTED FALSE | RULE 5 | INDEX: " << index << "\n";
                exit (1);
            case 6: // true value
                if (buffer[index + 1] == 'r' && buffer[index + 2] == 'u' && buffer[index + 3] == 'e')
                {
                    stack.pop();
                    index += 3;
                    break;
                }
                std::cout << "ERROR: EXPECTED TRUE | RULE 6 | INDEX: " << index << "\n";
                exit (1);
            case 7: // number value
                stack.pop ();
                stack.push (nt_exponent);
                stack.push (nt_fraction);
                stack.push (nt_digits);
                if (token == minus)
                {
                    if (buffer[index + 1] > 47 && buffer[index + 1] < 58)
                    {
                        index += 1;
                    }
                    else
                    {
                        std::cout << "ERROR: EXPECTED -# | RULE 7 | INDEX: " << index << "\n";
                        exit (1);
                    }
                }
                break;
            case 8: // fractional or exponential number
                if (buffer[index + 1] > 47 && buffer[index + 1] < 58)
                {
                    index += 1;
                }
                else if ((buffer[index + 1] == '+' || buffer[index + 1] == '-') && (buffer[index + 2] > 47 && buffer[index + 2] < 58) && token == Ee)
                {
                    index += 2;
                }
                else
                {
                    std::cout << "ERROR: EXPECTED .# or (E/e)# or (E/e)(+/-)# | RULE 8 | INDEX: " << index << "\n";
                    exit (-1);
                }
                stack.pop ();
                stack.push (nt_digits);
                break;
            case 9: // string value
                stack.pop ();
                stack.push (nt_characters);
                break;
            case 10: // escape sequence
                if (buffer[index + 1] == '"' || buffer[index + 1] == '\\' || buffer[index + 1] == '/' || buffer[index + 1] == 'b' || buffer[index + 1] == 'f' || buffer[index + 1] == 'n' || buffer[index + 1] == 'r' || buffer[index + 1] == 't')
                {
                    index += 1;
                }
                else if (buffer[index + 1] == 'u')
                {
                    for (uint8_t jump = 2; jump < 6; jump += 1)
                    {
                        if (!((buffer[index + jump] > 47 && buffer[index + jump] < 58) || (buffer[index + jump] > 64 && buffer[index + jump] < 71) || (buffer[index + jump] > 96 && buffer[index + jump] < 103)))
                        {
                            std::cout << "EXPECTED HEX | INDEX: " << buffer[index + jump] << "\n";
                            exit (-1);
                        }
                    }
                    index += 5;
                }
                else
                {
                    std::cout << "ERROR: EXPECTED \\{\",\\,/,b,f,n,r,t,u-h-h-h-h} | RULE 10 | INDEX: " << index << "\n";
                    exit (-1);
                }
                break;
            case 11: // object value
                stack.pop ();
                // stack.push (nt_closebrace);
                stack.push (nt_headmember);
                stack.push (nt_whitespace);
                break;
            case 12: // load object member
                stack.pop ();
                stack.push (nt_memberlist);
                stack.push (nt_whitespace);
                stack.push (nt_value);
                stack.push (nt_whitespace);
                stack.push (nt_colon);
                stack.push (nt_whitespace);
                stack.push (nt_characters);
                if (token == comma)
                {
                    stack.push (nt_quote);
                    stack.push (nt_whitespace);
                }
                break;
            case 13: // array value
                stack.pop ();
                stack.push (nt_headelement);
                stack.push (nt_whitespace);
                break;
            case 14: // load array element
                stack.pop ();
                stack.push (nt_elementlist);
                stack.push (nt_whitespace);
                stack.push (nt_value);
                index -= 1;
                if (token == comma)
                {
                    stack.push (nt_whitespace);
                    index += 1;
                }
                break;
        }
    }
    std::cout << stack.size() << "\n";
    std::cout << "PASS\n";
}