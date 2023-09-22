#include <algorithm>
#include <fstream>
#include <exception>
#include <queue>
#include <iostream>
#include <set>

#include "Graph.hpp"

static
void
InPrintHt(NodeNameDoubleValMap HashMap)
{
   std::cout << std::endl;
   for(const auto& it : HashMap)
   {
      std::cout << "[" << it.first << "," << it.second << "]" << std::endl;
   }
}

GraphPtr
GraphReader::ReadFile(
   const std::string FileName)
{
   if(FileName.empty()) { return nullptr;}

   std::ifstream f(FileName);

   if(!f.is_open()) { return nullptr;}

   GraphPtr ret = std::make_shared<Graph>();

   f >> ret->modV >> ret->modE;
   if(0 == ret->modV || 0 == ret->modE) {throw std::runtime_error("File hasn't verticies or edges!");}

   double requiredTime;
   f >> requiredTime;

   for(std::size_t i = 0; i < ret->modV; ++i)
   {
      std::string nodeName;
      double delay;
      f >> nodeName >> delay;
      auto node = std::make_shared<Node>(nodeName, delay);
      if(ret->modV -1 == i) { ret->dst = node;}
      ret->nodeHashMap[nodeName] = node;      
   }

   for(std::size_t i = 0; i < ret->modE; ++i)
   {
      std::string lhsNodeName;
      std::string rhsNodeName;
      f >> lhsNodeName >> rhsNodeName;
      ret->edgesMap[lhsNodeName].push_back(ret->nodeHashMap[rhsNodeName]);
   }

   ret->src = std::make_shared<Node>("SRC",0);
   ret->src->maxTime = ret->src->minTime = std::optional<double>(0);

   ret->nodeHashMap[ret->src->name] = ret->src;
   for(const auto& nodeIter : ret->nodeHashMap)
   {
      bool isSrc = true;
      for(const auto& edgeIter : ret->edgesMap)
      {
         const auto searchRes = std::find_if(edgeIter.second.begin(), edgeIter.second.end(),[&nodeIter](const auto& val) 
         { 
            if(nodeIter.first == "SRC") {return false;}
            return nodeIter.first == val->name;
         });
         if(searchRes != edgeIter.second.end())
         {
            isSrc = false;
            break;
         }
      }
      if(isSrc && nodeIter.first != "SRC") {ret->edgesMap[ret->src->name].push_back(ret->nodeHashMap[nodeIter.first]);  }
   }
   return ret;
}


NodeNameDoubleValMap
TimingGraphProcessor::CalcAAT()
{
   std::queue<std::string> queue;

   queue.push(Gr->src->name);

   while(!queue.empty())
   {
      const auto currName = queue.front();
      queue.pop();

      for(auto& neighbor : Gr->edgesMap[currName])
      {
         const auto potentialMaxTime = Gr->nodeHashMap[currName]->maxTime.value() + neighbor->delay;
         if(!neighbor->maxTime.has_value()) { neighbor->maxTime = std::optional<double>(potentialMaxTime);}
         else if(potentialMaxTime > neighbor->maxTime)
         {
            neighbor->maxTime = potentialMaxTime;
         }
         queue.push(neighbor->name);
      }
   }
   NodeNameDoubleValMap ret;
   for(const auto& it : Gr->nodeHashMap)
   {
      ret[it.first] = it.second->maxTime.value();
   }

   InPrintHt(ret);
   return ret;
}

NodeNameDoubleValMap
TimingGraphProcessor::CalcRAT()
{
   std::queue<std::string> queue;

   queue.push(Gr->src->name);

   while(!queue.empty())
   {
      const auto currName = queue.front();
      queue.pop();

      for(auto& neighbor : Gr->edgesMap[currName])
      {
         const auto potentialMinTime = Gr->nodeHashMap[currName]->minTime.value() + neighbor->delay;
         if(!neighbor->minTime.has_value()) { neighbor->minTime = std::optional<double>(potentialMinTime);}
         else if(potentialMinTime < neighbor->minTime)
         {
            neighbor->minTime = potentialMinTime;
         }
         queue.push(neighbor->name);
      }
   }
   NodeNameDoubleValMap ret;
   for(const auto& it : Gr->nodeHashMap)
   {
      ret[it.first] = it.second->minTime.value();
   }
   InPrintHt(ret);
   return ret;
}


NodeNameDoubleValMap
TimingGraphProcessor::CalcSlackes()
{
   const auto aatMap = CalcAAT();
   const auto ratMap = CalcRAT();

   NodeNameDoubleValMap ret;
   for(const auto& it : aatMap)
   {
      ret[it.first] = it.second - ratMap.at(it.first);
   }
   InPrintHt(ret);
   return ret;
}