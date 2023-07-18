#include <chrono>
#include <fstream>
#include "json_parser.hpp"
#include <nlohmann/json.hpp>

int main()
{
	std::cout << "Input (mb) | json_parser | nlohmann::json | improvement\n";
	// files to test
	std::vector<std::string> names = {"1.json", "5.json", "25.json"};
	// std::vector<std::string> names = {"test.json"};
	for (std::string& name : names)
	{
		std::ifstream file (name);
		std::string contents ((std::istreambuf_iterator<char> (file)), std::istreambuf_iterator<char>());
		json j;
		nlohmann::json n;
		float elapsed_j = 1;
		float elapsed_n = 1;
		for (int i = 0; i < 10; i += 1)
		{
			// time my implementation
			std::chrono::_V2::system_clock::time_point start_j = std::chrono::high_resolution_clock::now();
			j.parse (contents, false);
			std::chrono::_V2::system_clock::time_point end_j = std::chrono::high_resolution_clock::now();
			elapsed_j += std::chrono::duration_cast<std::chrono::milliseconds>(end_j - start_j).count();

			// time nlohmann::json
			std::chrono::_V2::system_clock::time_point start_n = std::chrono::high_resolution_clock::now();
			n = nlohmann::json::parse (contents);
			std::chrono::_V2::system_clock::time_point end_n = std::chrono::high_resolution_clock::now();
			elapsed_n += std::chrono::duration_cast<std::chrono::milliseconds>(end_n - start_n).count();
		}
		elapsed_j /= 10.0;
		elapsed_n /= 10.0;
		std::cout << name << " | " << elapsed_j << " | " << elapsed_n << " | " << ((elapsed_n - elapsed_j) / elapsed_n) * 100 << "%\n"; 
	}
	return 0;
}