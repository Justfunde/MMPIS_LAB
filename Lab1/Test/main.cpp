#include <iostream>
#include <cstdint>
#include <string>
#include <fstream>

#include <gtest/gtest.h>


#include "Ast.hpp"

constexpr std::string_view g_InputFileDir  = TEST_FILES_DIR "/Input/";
constexpr std::string_view g_OutputFileDir = TEST_FILES_DIR "/Output/";

static
std::string
InGetFileBuf(const std::string& FName)
{
   if(FName.empty()) { return ""; }

   std::ifstream f(FName);
   if(!f.is_open()) { return "";}

   return std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
}

class Test_Ast : public ::testing::Test
{
   protected:
   AstReader reader;
   AstWriter writer;
};

TEST_F(Test_Ast, TestFile1)
{
   const auto astArr = reader.ReadFile(std::string(g_InputFileDir) + "1.txt");
   const auto res = writer.WriteBuf(astArr);
   std::cout << res << std::endl;
   const auto etalonRes = InGetFileBuf(std::string(g_OutputFileDir) + "1.txt");
   EXPECT_EQ(res, etalonRes);
}

TEST_F(Test_Ast, TestFile2)
{
   const auto astArr = reader.ReadFile(std::string(g_InputFileDir) + "2.txt");
   const auto res = writer.WriteBuf(astArr);
   std::cout << res;
   const auto etalonRes = InGetFileBuf(std::string(g_OutputFileDir) + "2.txt");
   EXPECT_EQ(res, etalonRes);
}

TEST_F(Test_Ast, TestFile3)
{
   const auto astArr = reader.ReadFile(std::string(g_InputFileDir) + "3.txt");
   const auto res = writer.WriteBuf(astArr);
   std::cout << res;
   const auto etalonRes = InGetFileBuf(std::string(g_OutputFileDir) + "3.txt");
   EXPECT_EQ(res, etalonRes);
}

/*TEST_F(Test_Ast, TestFile4)
{
   const auto astArr = reader.ReadFile(std::string(g_InputFileDir) + "1.txt");
   const auto res = writer.WriteBuf(astArr);
   std::cout << res;
   const auto etalonRes = InGetFileBuf(std::string(g_OutputFileDir) + "1.txt");
   EXPECT_EQ(res, etalonRes);
}*/

int32_t main(int32_t argc, char** argv)
{
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}