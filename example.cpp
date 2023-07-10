#include <fstream>
#include "json_parser.hpp"

int main()
{
	std::ifstream file ("test.json");
	std::string contents ((std::istreambuf_iterator<char> (file)), std::istreambuf_iterator<char>());
	json j;
	j.parse (contents);
  return 0;
}
