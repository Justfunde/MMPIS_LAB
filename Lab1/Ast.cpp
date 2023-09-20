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
#include <sstream>

#include "Ast.hpp"


struct BracketNode
{
   static std::size_t token;
   std::size_t currTok;
   NodePtr node;
   std::size_t begInd;
   std::size_t endInd;
};

std::size_t BracketNode::token = 10;

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
   std::cout << tree->ToString() << std::endl; 
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
      NodePtr rhs = CreateNode(OperatorPart[2]);
      topNode = std::make_shared<BinaryOp>(OperatorPart[1], lhs, rhs);
   }
   // else if('~' == OperatorPart[0] && '(' == OperatorPart[1])
   // {
   //    const std::size_t closeBracketPos = OperatorPart.find_last_of(')');
   //    if(closeBracketPos == std::string::npos) { return nullptr;}
   //    if(closeBracketPos != OperatorPart.size() - 1) { return nullptr;}

   //    topNode = std::make_shared<UnaryOp>(OperatorPart[0], CreateNode(OperatorPart.substr(2, OperatorPart.size() - 3)));
   // }
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
               node.currTok = BracketNode::token;
               bracketNodes.push_back(node);
               resStr.replace(node.begInd - 1,  (node.endInd - node.begInd) + 2, std::to_string(BracketNode::token++));
               i = 0;
               std::cout << resStr << std::endl;
               bracketsStack.pop();
            }
           
         }
          ++i;
      }
      auto getOperatorCnt = 
         [](std::string_view Str, char Op) -> std::size_t
         {
            std::size_t cnt = 0;
            for(const auto& it : Str)
            {
               if(it == Op) { cnt++ ;}
            }
            return cnt;
         };

      //Токенизация инверсных выражений(с ~)

      const std::size_t notCnt = getOperatorCnt(resStr, '~');

      for(std::size_t i = 0; i < notCnt; ++i)
      {
         std::string_view resStrView = resStr;
         std::size_t notInd = resStr.find('~');
         std::size_t notEndInd = notInd + 1;
         while(resStr[notEndInd] != '+' && resStr[notEndInd] != '*' && notEndInd != resStr.size()) { notEndInd++;}

         std::string_view operationStr = resStrView.substr(notInd, notEndInd - notInd);
         BracketNode node;
         if(operationStr.size() == 2)
         {
            node.currTok = BracketNode::token;
            node.node = CreateNode(operationStr);
         }
         else
         {
            node.currTok = BracketNode::token;
            std::string_view tokenStr = operationStr.substr(1);
            std::size_t tok = std::stoull(std::string(tokenStr.begin(), tokenStr.end()));
            NodePtr implNode;
            for(std::vector<BracketNode>::iterator it = bracketNodes.begin(); it != bracketNodes.end(); ++it)
            {
               if(it->currTok == tok)
               {
                  implNode = it->node;
                  bracketNodes.erase(it);
                  break;
               }
            }
            node.node = std::make_shared<UnaryOp>('~', implNode);
         }
         bracketNodes.push_back(node);
         resStr.replace(notInd, notEndInd - notInd, std::to_string(BracketNode::token++));
      }

      
      const std::size_t mulCnt = getOperatorCnt(resStr, '*');

      std::cout << resStr << std::endl;   
      for(std::size_t i = 0; i < mulCnt; ++i)
      {
         std::string_view resStrView = resStr;
         std::size_t mulIndex = resStrView.find_first_of('*');
         std::size_t lhsBeginIndex = mulIndex - 1;
         std::size_t rhsEndIndex = mulIndex + 1;
         while(resStrView[lhsBeginIndex] != '+' && resStrView[lhsBeginIndex] != '*' && lhsBeginIndex != 0) 
         { --lhsBeginIndex;}
         if(lhsBeginIndex != 0 ) {lhsBeginIndex++;}
         while(resStrView[rhsEndIndex] != '+' && resStrView[rhsEndIndex] != '*' && rhsEndIndex != resStrView.size()) 
         {++rhsEndIndex;}
         if(rhsEndIndex != resStrView.size()) {rhsEndIndex--;}
         std::string_view operationSubstr = resStrView.substr(lhsBeginIndex, rhsEndIndex - lhsBeginIndex);
         //Парсинг левой и правых частей
         //Получение lhs и rhs операндов
         std::string_view lhsOperand = resStrView.substr(lhsBeginIndex, mulIndex - lhsBeginIndex );
         std::string_view rhsOperand = resStrView.substr(mulIndex + 1, rhsEndIndex - mulIndex);
         //если левая часть не токен, создаем ноду
         NodePtr lhsNode;
         std::cout<< lhsOperand << std::endl;
         if(lhsOperand.size() > 1)
         {
            std::size_t tok = std::stoull(std::string(lhsOperand.begin(), lhsOperand.end()));
            for(std::vector<BracketNode>::iterator it = bracketNodes.begin(); it != bracketNodes.end(); ++it)
            {
               if(it->currTok == tok)
               {
                  lhsNode = it->node;
                  bracketNodes.erase(it);
                  break;
               }
            }
         }
         else
         {
            lhsNode = std::make_shared<Operand>(lhsOperand[0]);
         }

         NodePtr rhsNode;
         std::cout<< rhsOperand << std::endl;
         if(rhsOperand.size() > 1)
         {
            std::size_t tok = std::stoull(std::string(rhsOperand.begin(), rhsOperand.end()));
            for(std::vector<BracketNode>::iterator it = bracketNodes.begin(); it != bracketNodes.end(); ++it)
            {
               if(it->currTok == tok)
               {
                  rhsNode = it->node;
                  bracketNodes.erase(it);
                  break;
               }
            }
         }
         else
         {
            rhsNode = std::make_shared<Operand>(rhsOperand[0]);
         }

         NodePtr resultNode = std::make_shared<BinaryOp>('*', lhsNode, rhsNode);
         BracketNode newTokNode;
         newTokNode.node = resultNode;
         newTokNode.currTok = BracketNode::token;
         bracketNodes.push_back(newTokNode);
         resStr.replace(lhsBeginIndex, rhsEndIndex - lhsBeginIndex + 1, std::to_string(BracketNode::token++));
         std::cout<< resStr << std::endl;
         std::flush(std::cout);
         std::cout<< std::endl<< resultNode->ToString() << std::endl;
      }

      const std::size_t sumCnt = getOperatorCnt(resStr, '+');

      for(std::size_t i = 0; i < sumCnt; ++i)
      {
         std::string_view resStrView = resStr;
         std::size_t mulIndex = resStrView.find_first_of('+');
         std::size_t lhsBeginIndex = mulIndex - 1;
         std::size_t rhsEndIndex = mulIndex + 1;
         while(resStrView[lhsBeginIndex] != '+' && resStrView[lhsBeginIndex] != '*' && lhsBeginIndex != 0) 
         { --lhsBeginIndex;}
         while(resStrView[rhsEndIndex] != '+' && resStrView[rhsEndIndex] != '*' && rhsEndIndex != resStrView.size()) 
         {++rhsEndIndex;}
         std::string_view operationSubstr = resStrView.substr(lhsBeginIndex, rhsEndIndex - lhsBeginIndex);
         //Парсинг левой и правых частей
         //Получение lhs и rhs операндов
         std::string_view lhsOperand = resStrView.substr(lhsBeginIndex, mulIndex - lhsBeginIndex);
         std::string_view rhsOperand = resStrView.substr(mulIndex + 1, rhsEndIndex - mulIndex - 1);
         //если левая часть не токен, создаем ноду
         NodePtr lhsNode;
         std::cout<< lhsOperand << std::endl;
         if(lhsOperand.size() > 1)
         {
            std::size_t tok = std::stoull(std::string(lhsOperand.begin(), lhsOperand.end()));
            for(std::vector<BracketNode>::iterator it = bracketNodes.begin(); it != bracketNodes.end(); ++it)
            {
               if(it->currTok == tok)
               {
                  lhsNode = it->node;
                  bracketNodes.erase(it);
                  break;
               }
            }
         }
         else
         {
            lhsNode = std::make_shared<Operand>(lhsOperand[0]);
         }

         NodePtr rhsNode;
         std::cout<< rhsOperand << std::endl;
         if(rhsOperand.size() > 1)
         {
            std::size_t tok = std::stoull(std::string(rhsOperand.begin(), rhsOperand.end()));
            for(std::vector<BracketNode>::iterator it = bracketNodes.begin(); it != bracketNodes.end(); ++it)
            {
               if(it->currTok == tok)
               {
                  rhsNode = it->node;
                  bracketNodes.erase(it);
                  break;
               }
            }
         }
         else
         {
            rhsNode = std::make_shared<Operand>(rhsOperand[0]);
         }

         NodePtr resultNode = std::make_shared<BinaryOp>('+', lhsNode, rhsNode);
         BracketNode newTokNode;
         newTokNode.node = resultNode;
         newTokNode.currTok = BracketNode::token;
         bracketNodes.push_back(newTokNode);
         resStr.replace(lhsBeginIndex, rhsEndIndex - lhsBeginIndex + 1, std::to_string(BracketNode::token++));
         std::cout<< resStr << std::endl;
      }
      topNode = bracketNodes[0].node;
   }
   return topNode;
}



void
AstWriter::WriteFile(
   const std::string FName,
   const AstArray& Arr)
{
   if(FName.empty() || Arr.empty()) { throw std::invalid_argument("Invalid args");}

   std::ofstream f(FName);
   if(f.is_open()) {throw std::runtime_error("File was not opened");}

   for(const auto& it : Arr)
   {
      f << it->ToString();
   }
}

std::string
AstWriter::WriteBuf(const AstArray& Arr)
{
   if(Arr.empty()) {return ""; }

   std::stringstream ss;

   for(std::size_t i = 0; i < Arr.size(); ++i)
   {
      ss << Arr[i]->ToString();
      if(Arr.size() - 1 != i) {ss << std::endl;}
   }
   return ss.str();
}