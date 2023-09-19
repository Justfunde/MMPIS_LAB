#include <iostream>
#include <cstdint>
#include <string>


#include "Ast.hpp"

int32_t main(int32_t argc, char** argv)
{
   const std::string path = std::string(TEST_FILES_DIR) + '/';
   std::cout<< "BEGIN\n";
   AstReader reader;
   auto ret = reader.ReadFile(path + "1.txt");
   std::cout<< ret[0]->ToString() << std::endl;
   return 0;
}