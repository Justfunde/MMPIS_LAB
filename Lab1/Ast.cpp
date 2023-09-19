#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <utility>
#include <vector>
#include <exception>
#include <regex>
#include <unordered_map>
#include <stack>

#include "Ast.hpp"

struct BracketNode
{
   static std::size_t token;
   NodePtr node;
   std::size_t begInd;
   std::size_t endInd;
};

std::size_t BracketNode::token = 2;

AstArray
AstReader::ReadFile(
   const std::string& FilePath)
{
   std::ifstream file(FilePath);

   if(!file.is_open())
   {
      throw std::invalid_argument("File does not exist!");
   }

   std::string tmpStr;

   file >> tmpStr;
   const std::size_t strCnt = std::stoul(tmpStr);
   if(0 == strCnt) return AstArray();

   AstArray ret(strCnt, nullptr);
   std::getline(file, tmpStr);
   for(auto& it : ret)
   {
      std::getline(file, tmpStr);
      it = ParseLine(tmpStr);
   }
   return ret;
}

   
bool
AstReader::CheckSyntax(std::string_view Str)
{
   if(Str.empty()) { return false;}

   //static const std::regex regexp("R(((\w[\+\*])+\w))");
   //return std::regex_match(Str.data(), regexp);
   return true;
}

bool
AstReader::HasBrackets(std::string_view Str)
{
   if(std::string::npos != Str.find("(")) { return false;}
   return true;
}


AstPtr
AstReader::ParseLine(
   std::string_view Line)
{
   if(!CheckSyntax(Line)) { return nullptr; }

   AstPtr tree = std::make_shared<Ast>();
   std::cout<< std::endl << Line << std::endl;

   //prepare str
   std::string prepared_line(Line.begin(), Line.end());
   prepared_line.erase(std::remove(prepared_line.begin(), prepared_line.end(), ' '), prepared_line.end());

   std::cout<< std::endl << prepared_line << std::endl;

   tree->head = CreateNode(prepared_line);
   return tree;
}


NodePtr
AstReader::CreateNode(
   const char OperandName)
{
   return std::make_shared<Operand>(OperandName);
}

NodePtr
AstReader::CreateNode(
   std::string_view OperatorPart)
{
   if(OperatorPart.empty()) { return nullptr; }

   std::cout << OperatorPart << std::endl;

   NodePtr topNode = nullptr;

   

   if(OperatorPart.size() == 1)
   {
      topNode = CreateNode(OperatorPart[0]);
   }
   else if(OperatorPart.size() == 2)
   {
      topNode = std::make_shared<UnaryOp>(OperatorPart[0], CreateNode(OperatorPart[1]));
   }
   else if(OperatorPart.size() == 3)
   {
      NodePtr lhs = CreateNode(OperatorPart[0]);
      NodePtr rhs = CreateNode(OperatorPart[3]);
      topNode = std::make_shared<BinaryOp>(OperatorPart[1], lhs, rhs);
   }
   else if('~' == OperatorPart[0] && '(' == OperatorPart[1])
   {
      const std::size_t closeBracketPos = OperatorPart.find_last_of(')');
      if(closeBracketPos == std::string::npos) { return nullptr;}
      if(closeBracketPos != OperatorPart.size() - 1) { return nullptr;}

      topNode = std::make_shared<UnaryOp>(OperatorPart[0], CreateNode(OperatorPart.substr(2, OperatorPart.size() - 3)));
   }
   else
   {
      std::string resStr(OperatorPart.begin(), OperatorPart.end());
      std::vector<BracketNode> bracketNodes;
      std::stack<std::pair<char, std::size_t>> bracketsStack;
      std::size_t i = 0;
      while(i < resStr.size())
      {
         if(resStr[i] == '(') { bracketsStack.push(std::make_pair('(', i));}
         if(resStr[i] == ')')
         {
            if(bracketsStack.size() != 1) { bracketsStack.pop();}
            else
            {
               BracketNode node;
               node.begInd = bracketsStack.top().second + 1;
               node.endInd = i;
               node.node = CreateNode(resStr.substr(node.begInd, node.endInd - node.begInd));
               std::cout << resStr.substr(node.begInd, node.endInd - node.begInd) << std::endl;
               resStr.replace(node.begInd - 1, node.endInd + 1, std::to_string(BracketNode::token++));
               i = 0;
               std::cout << resStr << std::endl;
               bracketsStack.pop();
            }
           
         }
          ++i;
      }

      



      //std::vector<std::pair<std::size_t, char>>;//operands and
      

   }
   return topNode;
}