#pragma once

#include <memory>
#include <optional>
#include <vector>
#include <string>
#include <string_view>


struct Node
{
   virtual 
   std::string 
   ToString() const = 0;
   virtual ~Node() = default;

   char t;
};

using NodePtr = std::shared_ptr<Node>;


struct UnaryOp : public Node
{

   UnaryOp(char Op, NodePtr operand)
      : operand(operand)
   {
      this->t = Op;
   }
   
   std::string
   ToString() const override final
   {
      return t + operand->ToString();
   }

   NodePtr operand;
};


struct BinaryOp : public Node
{

   BinaryOp(char Op, NodePtr lhsOp, NodePtr rhsOp)
      : lhs(lhsOp)
      , rhs(rhsOp)
   {
      this->t = Op;
   }

   std::string
   ToString() const override final
   {
      
      return t + lhs->ToString() + rhs->ToString();
   }


   NodePtr lhs;
   NodePtr rhs;
};

struct Operand : public Node
{
   Operand(char Operand) { this->t = Operand; }

   std::string
   ToString() const override final
   {
      
      return std::string(&t);
   }
};


class Ast;
using AstPtr = std::shared_ptr<Ast>;
using AstArray = std::vector<AstPtr>;

class Ast
{
   public:


   bool
   IsValid() const;

   std::string
   ToString() const { return head->ToString(); }


   public:
   NodePtr head;
};


class AstReader
{
   public:

   AstArray
   ReadFile(
      const std::string& FileName);


   private:
   
   bool
   HasBrackets(std::string_view Str);

   bool
   CheckSyntax(std::string_view Str);

   AstPtr
   ParseLine(
      std::string_view Line);

   NodePtr
   CreateNode(
      std::string_view OperatorPart);

   NodePtr
   CreateNode(
      const char OperandName);
   

   private:

};