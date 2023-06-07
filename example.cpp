#include "json_parser.hpp"
#include <iostream>

int main()
{
    std::string s = "[0,1,2,3]";     // test string
    json j;                          // create json object
    j.parse (s);                     // parse and create index
    token* t = j.index();            // get pointer to root of token tree
    t = t->index({0,2});             // get pointer to specific token using index
    std::cout << t->value() << "\n"; // print token value
    t->set (4,"5");                  // set token to another value
    std::cout << t->value() << "\n"; // print token value again
    t = j.index();
    std::cout << t->value() << "\n"; // print all values from root
}