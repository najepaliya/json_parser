#include "json_parser.hpp"

int main ()
{
    json j;
    std::string str = "[1,2,3,4, {} ]";
    
    
    std::cout << str << "\n";
    j.parse (str);
}