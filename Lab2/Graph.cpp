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

   f >> ret->requiredTime;

   for(std::size_t i = 0; i < ret->modV; ++i)
   {
      std::string nodeName;
      double delay;
      f >> nodeName >> delay;
      auto node = std::make_shared<Node>(nodeName, delay);

      if(ret->modV - 1 == i) { ret->dst = node;}
      if(0 == i) { ret->src = node;}

      ret->nodeHashMap[nodeName] = node;
   }

   ret->dst->minTime = std::optional<double>(ret->requiredTime);

   for(std::size_t i = 0; i < ret->modE; ++i)
   {
      std::string lhsNodeName;
      std::string rhsNodeName;
      double edgeWeight;
      f >> lhsNodeName >> rhsNodeName >> edgeWeight;
      ret->edgesMap[lhsNodeName].push_back(std::make_pair(ret->nodeHashMap[rhsNodeName], edgeWeight));
   }

   ret->src->maxTime = ret->src->minTime = std::optional<double>(0);
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
         const auto potentialMaxTime = Gr->nodeHashMap[currName]->maxTime.value() + neighbor.first->delay + neighbor.second;
         if(!neighbor.first->maxTime.has_value())
         {
            neighbor.first->maxTime = std::optional<double>(potentialMaxTime);
         }
         else if(potentialMaxTime > neighbor.first->maxTime)
         {
            neighbor.first->maxTime = potentialMaxTime;
         }
         queue.push(neighbor.first->name);
      }
   }
   NodeNameDoubleValMap ret;
   for(const auto& it : Gr->nodeHashMap)
   {
      auto res = it.second->maxTime.value();
      ret[it.first] = res;
   }

   InPrintHt(ret);
   return ret;
}

NodeNameDoubleValMap
TimingGraphProcessor::CalcRAT()
{
   std::queue<std::string> queue;

   queue.push(Gr->dst->name);

   while(!queue.empty())
   {
      const auto currName = queue.front();
      queue.pop();
      //find neighbors
      std::list<std::pair<std::string, double>> connectedEdgesNames;
      for(const auto& neighborIter : Gr->edgesMap)
      {
         auto resIter = std::find_if(neighborIter.second.begin(), neighborIter.second.end(),[&currName](const auto& val) { return val.first->name == currName;});
         if(resIter != neighborIter.second.end())
         {
            connectedEdgesNames.push_back(std::make_pair(neighborIter.first, resIter->second));
         }
      }

     for(const auto& neighborIter : connectedEdgesNames)
      {
         const auto potentialMinTime = Gr->nodeHashMap[currName]->minTime.value() - Gr->nodeHashMap[neighborIter.first]->delay - neighborIter.second;
         if(!Gr->nodeHashMap[neighborIter.first]->minTime.has_value())
         {
            Gr->nodeHashMap[neighborIter.first]->minTime = std::optional<double>(potentialMinTime);
         }
         else if(potentialMinTime < Gr->nodeHashMap[neighborIter.first]->minTime)
         {
            Gr->nodeHashMap[neighborIter.first]->minTime = potentialMinTime;
         }
         queue.push(Gr->nodeHashMap[neighborIter.first]->name);
      }

   }
   NodeNameDoubleValMap ret;
   for(const auto& it : Gr->nodeHashMap)
   {
      ret[it.first] = it.second->minTime.value() + it.second->delay;
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
      ret[it.first] = - it.second + ratMap.at(it.first);
   }
   InPrintHt(ret);
   return ret;
}