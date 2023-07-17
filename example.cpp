#include <chrono>
#include <fstream>
#include "json_parser.hpp"

int main()
{
	std::ifstream file ("test.json");
	std::string contents ((std::istreambuf_iterator<char> (file)), std::istreambuf_iterator<char>());
	json j;
	std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();
	j.parse (contents);
	std::chrono::_V2::system_clock::time_point stop = std::chrono::high_resolution_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << "\n";
	return 0;
}