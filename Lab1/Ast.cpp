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
#include <list>

#include "Ast.hpp"




std::size_t AstReader::OperandNode::token = 10;

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
   static const std::regex regexp("^[\\w+\\*~01() ]*$");
   const bool matches = std::regex_match(std::string(Str.begin(), Str.end()), regexp);
   return matches;
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
   ////std::cout<< std::endl << Line << std::endl;

   //prepare str
   std::string prepared_line(Line.begin(), Line.end());
   prepared_line.erase(std::remove(prepared_line.begin(), prepared_line.end(), ' '), prepared_line.end());

   ////std::cout<< std::endl << prepared_line << std::endl;

   tree->SetHead(CreateNode(prepared_line));
   //std::cout << tree->ToString() << std::endl; 
   return tree;
}

std::string
AstReader::ProcessBrackets(
   std::string Str,
   std::list<OperandNode>& OperandNodes)
{
   std::stack<std::pair<char, std::size_t>> bracketsStack;
   std::size_t i = 0;

   while(i < Str.size())
      {
         if(Str[i] == '(') { bracketsStack.push(std::make_pair('(', i));}
         if(Str[i] == ')')
         {
            if(bracketsStack.size() != 1) { bracketsStack.pop();}
            else
            {
               OperandNode node;
               node.begInd = bracketsStack.top().second + 1;
               node.endInd = i;
               node.node = CreateNode(Str.substr(node.begInd, node.endInd - node.begInd));
               //std::cout << resStr.substr(node.begInd, node.endInd - node.begInd) << std::endl;
               node.currTok = OperandNode::token;
               OperandNodes.push_back(node);
               Str.replace(node.begInd - 1,  (node.endInd - node.begInd) + 2, std::to_string(OperandNode::token++));
               i = 0;
               //std::cout << resStr << std::endl;
               bracketsStack.pop();
            }
           
         }
          ++i;
      }
   return Str;
}

std::size_t
AstReader::GetOperatorCnt(
   std::string_view Str,
   char Op) const
{
   std::size_t cnt = 0;
   for(const auto& it : Str)
   {
      if(it == Op) { cnt++ ;}
   }
   return cnt;
}


std::string
AstReader::ProcessOperator(
   std::string Str,
   char Operator,
   std::list<OperandNode>& OperandNodes)
{
   const std::size_t opCnt = GetOperatorCnt(Str, Operator);
   if(opCnt == 0) { return std::move(Str);}

   switch (Operator)
   {
   case '~' : return ProcessInv(Str, opCnt, OperandNodes);
   case '+':
   case '*':
   {
      for(std::size_t i = 0; i < opCnt; ++i)
      {
         std::string_view resStrView = Str;
         std::size_t opIndex = resStrView.find_first_of(Operator);
         std::size_t lhsBeginIndex = opIndex - 1;
         std::size_t rhsEndIndex = opIndex + 1;

         while(resStrView[lhsBeginIndex] != '+' && resStrView[lhsBeginIndex] != '*' && lhsBeginIndex != 0) 
         { --lhsBeginIndex;}

         if(lhsBeginIndex != 0 ) {lhsBeginIndex++;}
         while(resStrView[rhsEndIndex] != '+' && resStrView[rhsEndIndex] != '*' && rhsEndIndex != resStrView.size()) 
         {++rhsEndIndex;}
         if(rhsEndIndex != resStrView.size()) {rhsEndIndex--;}

         std::string_view operationSubstr = resStrView.substr(lhsBeginIndex, rhsEndIndex - lhsBeginIndex);
         //Парсинг левой и правых частей
         //Получение lhs и rhs операндов
         std::string_view lhsOperand = resStrView.substr(lhsBeginIndex, opIndex - lhsBeginIndex );
         std::string_view rhsOperand = resStrView.substr(opIndex + 1, rhsEndIndex - opIndex);
         //если левая часть не токен, создаем ноду
         NodePtr lhsNode;
         //std::cout<< lhsOperand << std::endl;
         if(lhsOperand.size() > 1)
         {
            std::size_t tok = std::stoull(std::string(lhsOperand.begin(), lhsOperand.end()));
            for(std::list<OperandNode>::iterator it = OperandNodes.begin(); it != OperandNodes.end(); ++it)
            {
               if(it->currTok == tok)
               {
                  lhsNode = it->node;
                  OperandNodes.erase(it);
                  break;
               }
            }
         }
         else
         {
            lhsNode = std::make_shared<Operand>(lhsOperand[0]);
         }

         NodePtr rhsNode;
         //std::cout<< rhsOperand << std::endl;
         if(rhsOperand.size() > 1)
         {
            std::size_t tok = std::stoull(std::string(rhsOperand.begin(), rhsOperand.end()));
            for(std::list<OperandNode>::iterator it = OperandNodes.begin(); it != OperandNodes.end(); ++it)
            {
               if(it->currTok == tok)
               {
                  rhsNode = it->node;
                  OperandNodes.erase(it);
                  break;
               }
            }
         }
         else
         {
            rhsNode = std::make_shared<Operand>(rhsOperand[0]);
         }

         NodePtr resultNode = std::make_shared<BinaryOp>(Operator, lhsNode, rhsNode);
         OperandNode newTokNode;
         newTokNode.node = resultNode;
         newTokNode.currTok = OperandNode::token;
         OperandNodes.push_back(newTokNode);
         Str.replace(lhsBeginIndex, rhsEndIndex - lhsBeginIndex + 1, std::to_string(OperandNode::token++));
         //std::cout<< resStr << std::endl;
         //std::flush(//std::cout);
         //std::cout<< std::endl<< resultNode->ToString() << std::endl;
      }
      return Str;
   }
   default: throw std::runtime_error("Invalid operand");
   }

}


std::string
AstReader::ProcessInv(
   std::string Str,
   std::size_t OpCnt,
   std::list<OperandNode>& OperandNodes)
{
   for(std::size_t i = 0; i < OpCnt; ++i)
   {
      std::string_view resStrView = Str;
      std::size_t notInd = Str.find('~');
      std::size_t notEndInd = notInd + 1;
      while(Str[notEndInd] != '+' && Str[notEndInd] != '*' && notEndInd != Str.size()) { notEndInd++;}

      std::string_view operationStr = resStrView.substr(notInd, notEndInd - notInd);
      OperandNode node;
      if(operationStr.size() == 2)
      {
         node.currTok = OperandNode::token;
         node.node = CreateNode(operationStr);
      }
      else
      {
         node.currTok = OperandNode::token;
         std::string_view tokenStr = operationStr.substr(1);
         std::size_t tok = std::stoull(std::string(tokenStr.begin(), tokenStr.end()));
         NodePtr implNode;
         for(std::list<OperandNode>::iterator it = OperandNodes.begin(); it != OperandNodes.end(); ++it)
         {
            if(it->currTok == tok)
            {
               implNode = it->node;
               OperandNodes.erase(it);
               break;
            }
         }
         node.node = std::make_shared<UnaryOp>('~', implNode);
      }
      OperandNodes.push_back(node);
      Str.replace(notInd, notEndInd - notInd, std::to_string(OperandNode::token++));
   }
   return Str;
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

   //std::cout << OperatorPart << std::endl;

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
   else
   {
      
      std::list<OperandNode> operandNodes;
      
      std::string resStr = ProcessBrackets(std::string(OperatorPart.begin(), OperatorPart.end()), operandNodes);

      //Токенизация инверсных выражений(с ~)

      resStr = ProcessOperator(resStr, '~', operandNodes);
      resStr = ProcessOperator(resStr, '*', operandNodes);
      resStr = ProcessOperator(resStr, '+', operandNodes);
      topNode = operandNodes.begin()->node;
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
      if(nullptr != Arr[i])
      {
         ss << Arr[i]->ToString();
      }
      else {ss << "ERROR";}
      if(Arr.size() - 1 != i) {ss << std::endl;}
   }
   return ss.str();
}