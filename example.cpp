#include <fstream>
#include "json_parser.hpp"

int main()
{
	std::ifstream file ("test.json");
	std::string contents ((std::istreambuf_iterator<char> (file)), std::istreambuf_iterator<char>());
	json j;
	j.parse (contents);
	token* root = j.index();
	for (int i=0; i < root->children.size(); i += 1)
	{
		std::cout << root->children[i].string << "\n";
	}
  return 0;
}
