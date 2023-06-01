#pragma once

#include <cstdint>
#include <string>
#include <stack>
#include <vector>
#include <iostream>

enum symbol : uint_fast8_t
{
    // tokens
    whitespace, n,
    // nonterminals
    nt_whitespace, nt_value
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
    // repeating match: 4
    uint8_t rule_table [2][2] =
    {
    //  ws  n
         4, 1, // ws
         0, 3, // val
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

        switch (rule_table[stack.top() % 2][token % 2])
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
            case 3: // null value
                if (buffer[index + 1] == 'u' && buffer[index + 2] == 'l' && buffer[index + 3] == 'l')
                {
                    stack.pop();
                    index += 3;
                }
                else
                {
                    return error ();
                }
                break;
        }
    }

    std::cout << "PASS\n";
    return 0;
}