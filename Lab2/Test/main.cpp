#include <iostream>
#include <cstdint>
#include <string>
#include <fstream>

#include <gtest/gtest.h>


#include "Graph.hpp"

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

class Test_TimingGraphProcessor : public ::testing::Test
{
};

TEST_F(Test_TimingGraphProcessor, TestFile1)
{
   auto gr = GraphReader::ReadFile(std::string(g_InputFileDir) + "1.txt");
   TimingGraphProcessor proc(gr);
   const auto res = proc.CalcSlackes();
//   EXPECT_EQ(res, etalonRes);
}

int32_t main(int32_t argc, char** argv)
{
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
   return 0;
}