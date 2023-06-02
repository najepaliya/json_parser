#pragma once

#include <cstdint>
#include <string>
#include <stack>
#include <vector>
#include <iostream>

enum symbol : uint_fast8_t
{
    // tokens
    whitespace, n, f, t, digit, minus, dot, Ee, quote, character, backslash,
    // nonterminals
    nt_whitespace, nt_value, nt_digits, nt_fraction, nt_exponent, nt_characters
};

void push_multiple (std::stack<uint_fast8_t>& stack, std::vector<uint_fast8_t>&& symbols)
{
    for (uint_fast8_t index = 0; index < symbols.size(); index += 1)
    {
        stack.push(symbols[index]);
    }
}

bool match_pattern (std::string&& buffer, std::string&& pattern)
{
    bool result = true;
    for (uint_fast8_t index = 0; index < pattern.size(); index += 1)
    {
        if (buffer[index] != pattern[index])
        {
            result = false;
            break;
        }
    }
    return result;
}

bool is_digit (char& character)
{
    bool result = false;
    if (character > 47 && character < 58)
    {
        result = true;
    }
    return result;
}

bool is_escape (char& character)
{
    bool result = false;
    if (character == '"' || character == '\\' || character == '/' || character == 'b' || character == 'f' || character == 'n' || character == 'r' || character == 't')
    {
        result = true;
    }
    return result;
}

bool is_hex_pattern (std::string&& buffer)
{
    bool result = true;
    for (uint_fast8_t index = 0; index < 4; index += 1)
    {
        if (is_digit (buffer[index]) || (buffer[index] > 64 && buffer[index] < 71) || (buffer[index] > 96 && buffer[index] < 103))
        {
            result = false;
            break;
        }
    }
    return result;
}

uint_fast8_t error ()
{
    std::cout << "ERROR\n";
    return -1;
}

uint_fast8_t parse_json (std::string&& buffer)
{
    // repeating match: 3
    uint8_t rule_table [6][11] =
    {
    /*  ws  n  f  t  #  -  .  Ee   " ch   \ */
         3, 1, 1, 1, 1, 1, 1,  1,  1, 1,  1, // ws
         0, 4, 5, 6, 7, 7, 0,  0, 10, 0,  0, // val
         1, 1, 1, 1, 3, 1, 1,  1,  1, 1,  1, // digs
         1, 0, 0, 0, 0, 0, 8,  1,  0, 0,  0, // frac
         1, 0, 0, 0, 0, 0, 0,  9,  0, 0,  0, // exp
         3, 3, 3, 3, 3, 3, 3,  3,  2, 3, 11, // chs
    };

    std::stack<uint_fast8_t> stack;
    push_multiple(stack, {nt_whitespace, nt_value, nt_whitespace});

    for (int index = 0; index < buffer.size(); index += 1)
    {
        uint_fast8_t token = character;
        switch (buffer[index])
        {
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
        }

        switch (rule_table[stack.top() % 11][token % 11])
        {
            case 0: // error
                return error ();
            case 1: // null
                stack.pop();
                index -= 1;
                break;
            case 2: // match
                stack.pop();
                break;
            case 4: // null value
                if (match_pattern (buffer.substr(index + 1, 3), "ull"))
                {
                    stack.pop();
                    index += 3;
                    break;
                }
                return error ();
            case 5: // false value
                if (match_pattern (buffer.substr(index + 1, 4), "alse"))
                {
                    stack.pop();
                    index += 4;
                    break;
                }
                return error ();
            case 6: // true value
                if (match_pattern (buffer.substr(index + 1, 3), "rue"))
                {
                    stack.pop();
                    index += 3;
                    break;
                }
                return error ();
            case 7: // number value
                stack.pop();
                push_multiple (stack, {nt_exponent, nt_fraction, nt_digits});
                break;
            case 8: // fractional number
                if (is_digit (buffer[index + 1]))
                {
                    stack.pop();
                    push_multiple (stack, {nt_digits});
                    index += 1;
                    break;
                }
                return error ();
            case 9: // exponential number
            {
                uint_fast8_t jump = 1;
                if (buffer[index + 1] == '+' || buffer[index + 1] == '-')
                {
                    jump = 2;
                }
                if (is_digit (buffer[index + jump]))
                {
                    stack.pop();
                    push_multiple (stack, {nt_digits});
                    index += jump;
                    break;
                }
                return error ();
            }
            case 10: // string value
                stack.pop();
                push_multiple (stack, {nt_characters});
                break;
            case 11: // escape sequence
                if (is_escape (buffer[index + 1]))
                {
                    index += 1;
                    break;
                }
                else if (buffer[index + 1] == 'u')
                {
                    if (is_hex_pattern (buffer.substr(index + 2, 4)))
                    {
                        index += 5;
                        break;
                    }
                }
                return error ();
        }
    }

    std::cout << "PASS\n";
    return 0;
}