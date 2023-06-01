#pragma once

#include <cstdint>
#include <string>
#include <stack>
#include <vector>
#include <iostream>

enum symbol : uint_fast8_t
{
    // tokens
    whitespace, n, f, t,
    // nonterminals
    nt_whitespace, nt_value,
};

void push_multiple (std::stack<uint_fast8_t>& stack, std::vector<uint_fast8_t>&& symbols)
{
    for (uint_fast8_t index = 0; index < symbols.size(); index += 1)
    {
        stack.push(symbols[index]);
    }
}

uint_fast8_t error ()
{
    std::cout << "ERROR\n";
    return -1;
}

uint_fast8_t parse_json (std::string buffer)
{
    // repeating match: 3
    uint8_t rule_table [2][4] =
    {
    //  ws  n  f  t
         3, 1, 1, 1, // ws
         0, 4, 5, 6, // val
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
        }

        switch (rule_table[stack.top() % 4][token % 4])
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
                if (buffer[index + 1] == 'u' && buffer[index + 2] == 'l' && buffer[index + 3] == 'l')
                {
                    stack.pop();
                    index += 3;
                    break;
                }
                return error ();
            case 5: // false value
                if (buffer[index + 1] == 'a' && buffer[index + 2] == 'l' && buffer[index + 3] == 's' && buffer[index + 4] == 'e')
                {
                    stack.pop();
                    index += 4;
                    break;
                }
                return error ();
            case 6: // true value
                 if (buffer[index + 1] == 'r' && buffer[index + 2] == 'u' && buffer[index + 3] == 'e')
                {
                    stack.pop();
                    index += 3;
                    break;
                }
                return error ();
        }
    }

    std::cout << "PASS\n";
    return 0;
}