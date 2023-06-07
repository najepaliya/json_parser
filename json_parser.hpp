#include <cstdint>
#include <iostream>
#include <stack>
#include <vector>

class token
{
    public:
        std::string string;
        std::vector<token> children;
        int8_t type;
        token (int8_t type, std::string&& string)
        {
            this->type = type;
            this->string = string;
        }
        token* index (const std::vector<int>& indexes)
        {
            token* temp = this;
            for (int i = 0; i < indexes.size(); i += 1)
            {
                if (temp->children.size() > indexes[i])
                {
                    temp = &(temp->children[indexes[i]]);
                    continue;
                }
                temp = nullptr;
                break;
            }
            return temp;
        }
        std::string value ()
        {
            // if (type > 0)
            // {
            //     return this->string;
            // }
            // else if (type == 0)
            // {
            //     return this->children[0].value();
            // }
            // else
            // {
            //     std::string temp;
            //     if (type == -2)
            //     {
            //         temp += '[';
            //         for (int i = 0; i < this->children.size(); i += 1)
            //         {
            //             temp += this->children[i].value() + ",";
            //         }
            //         temp.back() = ']';
            //     }
            //     else
            //     {
            //         temp += '{';
            //         for (int i = 0; i < this->children.size(); i += 1)
            //         {
            //             if (i % 2 == 0)
            //             {
            //                 temp += this->children[i].value() + ":";
            //             }
            //             else
            //             {
            //                 temp += this->children[i].value() + ",";
            //             }
            //         }
            //         temp.back() = '}';
            //     }
            //     return temp;
            // }
            return "";
        }
};

class json
{
    private:
        token root = token (0, "");
    public:
        token* index()
        {
            return &root;
        }
        void parse (std::string& buffer);
};

enum symbol : uint_fast8_t
{
    // terminals
    end, whitespace, quote, character, backslash, escape, b, u, hex, openbrace, closebrace, colon, comma, openbracket, closebracket, digit, minus, dot, Ee, n, f, t,
    // nonterminals
    nt_end, nt_primitive, nt_container, nt_whitespace, nt_value, nt_characters, nt_escape, nt_hex, nt_headmember, nt_colon, nt_memberlist, nt_string, nt_headelement, nt_elementlist, nt_digits, nt_fraction, nt_exponent,
};

const uint_fast8_t divisor = 22;

// temporary
void print_error(int index, uint_fast8_t nonterminal, uint_fast8_t terminal, char c)
{
    std::cout << "ERROR | INDEX: " << index << " | CHARACTER: " << c << " | STACK: " << (nonterminal % divisor) << "\n";
}

void json::parse (std::string& buffer)
{
    /*3 = repeating match*/
    uint_fast8_t rule_table [17][divisor] =
    {/* $  ws   "  ch   \ esc   b   u  hx   {   }   :   ,   [   ]   #   -   .  Ee   n   f   t */
        4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /*end   0*/
        8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8, /*prim  1*/
        8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8, /*cont  2*/
        1,  3,  1,  0,  0,  0,  0,  0,  0,  1,  0,  1,  1,  1,  0,  1,  1,  0,  0,  1,  1,  1, /*ws    3*/
        0,  0,  5,  0,  0,  0,  0,  0,  0,  9,  0,  0,  0, 12,  0, 14, 14,  0,  0, 16, 17, 18, /*val   4*/
        1,  3,  2,  3,  6,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3, /*chs   5*/
        0,  0,  2,  0,  2,  2,  2,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  2,  2, /*esc   6*/
        0,  0,  0,  0,  0,  0,  2,  0,  2,  0,  0,  0,  0,  0,  0,  2,  0,  0,  2,  0,  2,  0, /*hex   7*/
        0,  3, 10,  0,  0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /*hmem  8*/
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /*col   9*/
        0,  3,  0,  0,  0,  0,  0,  0,  0,  0,  2,  0, 10,  0,  0,  0,  0,  0,  0,  0,  0,  0, /*meml  10*/
        0,  0, 11,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /*str   11*/
        0,  3, 13,  0,  0,  0,  0,  0,  0, 13,  0,  0,  0, 13,  2, 13, 13,  0,  0, 13, 13, 13, /*helm  12*/
        0,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 13,  0,  2,  0,  0,  0,  0,  0,  0,  0, /*elml  13*/
        1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  1,  0,  1,  3,  0,  1,  1,  0,  0,  0, /*digs  14*/
        1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  1,  0,  1,  0,  0, 15,  1,  0,  0,  0, /*frac  15*/
        1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  1,  0,  1,  0,  0,  0, 15,  0,  0,  0, /*exp   16*/
    };

    // initialize symbol stack
    std::stack<uint_fast8_t> symbols;
    symbols.push (nt_end);
    symbols.push (nt_whitespace);
    symbols.push (nt_value);
    symbols.push (nt_whitespace);

    // initialize index stack
    std::stack<int> indexes;

    // initialize container stack
    std::stack<token*> containers;
    containers.push (&root);

    // add terminator to string
    buffer += -1;

    for (int index = 0; ; index += 1)
    {
        uint_fast8_t terminal = character;

        switch (buffer[index])
        {
            case -1:
                terminal = end;
                break;
            case ' ': case '\n': case '\r': case '\t':
                terminal = whitespace;
                break;
            case '"':
                terminal = quote;
                break;
            case '\\':
                terminal = backslash;
                break;
            case '/': case 'r':
                terminal = escape;
                break;
            case 'b':
                terminal = b;
                break;
            case 'u':
                terminal = u;
                break;
            case 'A': case 'B': case 'C': case 'D': case 'F': case 'a': case 'c': case 'd':
                terminal = hex;
                break;
            case '{':
                terminal = openbrace;
                break;
            case '}':
                terminal = closebrace;
                break;
            case ':':
                terminal = colon;
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
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                terminal = digit;
                break;
            case '-':
                terminal = minus;
                break;
            case '.':
                terminal = dot;
                break;
            case 'E': case 'e':
                terminal = Ee;
                break;
            case 'n':
                terminal = n;
                break;
            case 'f':
                terminal = f;
                break;
            case 't':
                terminal = t;
                break;
        }

        switch (rule_table[symbols.top() % divisor][terminal % divisor])
        {
            case 0: // error
                print_error (index, symbols.top(), terminal, buffer[index]);
                return;
            case 1: // null
                // std::cout << "NULL: " << symbols.top() % divisor << " | INDEX: " << index << "\n";
                symbols.pop();
                index -= 1;
                break;
            case 2: // match
                // std::cout << "MATCH: " << symbols.top() % divisor << " | INDEX: " << index << "\n";
                symbols.pop();
                break;
            case 4: // end
                std::cout << "SUCCESS\n";
                return;
            case 5: // string value
                symbols.pop();
                symbols.push (nt_primitive);
                symbols.push (nt_characters);
                indexes.push (index);
                break;
            case 6: // escape sequence
                symbols.push (nt_escape);
                break;
            case 7: // hex sequence
                symbols.pop();
                symbols.push (nt_hex);
                symbols.push (nt_hex);
                symbols.push (nt_hex);
                symbols.push (nt_hex);
                break;
            case 8: // token completion
                if (symbols.top() == nt_container)
                {
                    containers.pop();
                }
                else
                {
                    uint8_t type = 4;
                    if (buffer[indexes.top()] == '"')
                    {
                        type = 5;
                    }
                    // std::cout << buffer.substr (indexes.top(), index - indexes.top()) << "\n";
                    containers.top()->children.emplace_back (type, buffer.substr (indexes.top(), index - indexes.top()));
                    indexes.pop();
                }
                symbols.pop();
                index -= 1;
                break;
            case 9: // object value
                symbols.pop();
                containers.top()->children.emplace_back (-1, "{}");
                containers.push (&(containers.top()->children.back()));
                symbols.push (nt_container);
                symbols.push (nt_headmember);
                break;
            case 10: // load object member
                symbols.pop();
                symbols.push (nt_memberlist);
                symbols.push (nt_value);
                symbols.push (nt_whitespace);
                symbols.push (nt_colon);
                symbols.push (nt_whitespace);
                symbols.push (nt_string);
                index -= 1;
                if (terminal == comma)
                {
                    symbols.push (nt_whitespace);
                    index += 1;
                }
                break;
            case 11: // decompose string
                symbols.pop ();
                symbols.push (nt_value);
                index -= 1;
                break;
            case 12: // array value
                symbols.pop();
                containers.top()->children.emplace_back (-2, "[]");
                containers.push (&(containers.top()->children.back()));
                symbols.push (nt_container);
                symbols.push (nt_headelement);
                break;
            case 13: // load array element
                symbols.pop();
                symbols.push (nt_elementlist);
                symbols.push (nt_value);
                index -= 1;
                if (terminal == comma)
                {
                    symbols.push (nt_whitespace);
                    index += 1;
                }
                break;
            case 14: // number value
                symbols.pop ();
                symbols.push (nt_primitive);
                symbols.push (nt_exponent);
                symbols.push (nt_fraction);
                symbols.push (nt_digits);
                indexes.push (index);
                if (terminal == minus)
                {
                    if (buffer[index + 1] > 47 && buffer[index + 1] < 58)
                    {
                        index += 1;
                    }
                    else
                    {
                        print_error (index, symbols.top(), terminal, buffer[index]);
                        return;
                    }
                }
                break;
            case 15: // fractional or exponential number
                if (buffer[index + 1] > 47 && buffer[index + 1] < 58)
                {
                    index += 1;
                }
                else if (terminal = Ee && (buffer[index + 1] == '+' || buffer[index + 1] == '-') && (buffer[index + 2] > 47 && buffer[index + 2] < 58))
                {
                    index += 2;
                }
                else
                {
                    print_error (index, symbols.top(), terminal, buffer[index]);
                    return;
                }
                symbols.pop ();
                symbols.push (nt_digits);
                break;
            case 16: // null value
                if (buffer[index + 1] == 'u' && buffer[index + 2] == 'l' && buffer[index + 3] == 'l')
                {
                    containers.top()->children.emplace_back (1, "null");
                    symbols.pop();
                    index += 3;
                    break;
                }
                print_error (index, symbols.top(), terminal, buffer[index]);
            case 17: // false value
                if (buffer[index + 1] == 'a' && buffer[index + 2] == 'l' && buffer[index + 3] == 's' && buffer[index + 4] == 'e')
                {
                    containers.top()->children.emplace_back (2, "false");
                    symbols.pop();
                    index += 4;
                    break;
                }
                print_error (index, symbols.top(), terminal, buffer[index]);
            case 18: // true value
                if (buffer[index + 1] == 'r' && buffer[index + 2] == 'u' && buffer[index + 3] == 'e')
                {
                    containers.top()->children.emplace_back (3, "true");
                    symbols.pop();
                    index += 3;
                    break;
                }
                print_error (index, symbols.top(), terminal, buffer[index]);
        }
    }
}