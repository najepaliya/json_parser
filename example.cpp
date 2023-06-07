#include "json_parser.hpp"
#include <iostream>

// Integer associated types
// Array   -2
// Object  -1
// Root     0
// Null     1
// False    2
// True     3
// Number   4
// String   5

int main()
{
    std::string s = "[{},null,false,true,0.1e+2,\"hello\"]";// test string
    json j;                                                     // create json object
    j.parse (s);                                                // parse and create index
    token* t = j.index();                                       // get pointer to root of token tree
    t = t->index({0,2});                                        // get pointer to specific token using index
    std::cout << t->value() << "\n";                            // print token value
    t->set (4,"5");                                             // set token to another value
    std::cout << t->value() << "\n";                            // print token value again
    t = j.index();                                              // get pointer to root of token tree
    std::cout << t->value() << "\n";                            // print all values from root
    return 0;
}