#include <fstream>
#include "json_parser.hpp"

int main()
{
	json j;
	std::ifstream file ("test.json");
	std::string buffer ((std::istreambuf_iterator<char> (file)), std::istreambuf_iterator<char>());
  return 0;
}
