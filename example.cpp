#include "json_parser.hpp"
#include <string>
#include <fstream>
#include <streambuf>

int main ()
{
    std::ifstream t("example.json");
    std::string str((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
    
    json j;
    j.parse (str);
}