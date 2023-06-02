#include "json_parser.hpp"

int main ()
{
    return parse_json ("\" abcd \u00Af noway \"");
}