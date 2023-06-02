#pragma once

#include <cstdint>
#include <string>
#include <stack>
#include <vector>
#include <iostream>

enum symbol : uint_fast8_t
{
    // tokens
    whitespace, n, f, t, digit, minus, dot, Ee, quote,
    // nonterminals
    nt_whitespace, nt_value, nt_digits, nt_fraction, nt_exponent,
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

uint_fast8_t error ()
{
    std::cout << "ERROR\n";
    return -1;
}

uint_fast8_t parse_json (std::string&& buffer)
{
    // repeating match: 3
    uint8_t rule_table [5][9] =
    {
    //  ws  n  f  t  #  -  .  Ee   "
         3, 1, 1, 1, 1, 1, 1,  1,  0, // ws
         0, 4, 5, 6, 7, 7, 0,  0, 10, // val
         1, 1, 1, 1, 3, 1, 1,  1,  0, // digs
         1, 0, 0, 0, 0, 0, 8,  1,  0, // frac
         1, 0, 0, 0, 0, 0, 0,  9,  0, // exp
    };

    std::stack<uint_fast8_t> stack;
    push_multiple(stack, {nt_whitespace, nt_value, nt_whitespace});

    uint_fast8_t token;

    for (int index = 0; index < buffer.size(); index += 1)
    {
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
        }

        switch (rule_table[stack.top() % 9][token % 9])
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
    }

    std::cout << "PASS\n";
    return 0;
}