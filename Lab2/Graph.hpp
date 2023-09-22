#pragma once
#include <list>
#include <string>
#include <memory>
#include <unordered_map>
#include <optional>
#include <numeric>

struct Node
{
   std::string name;
   double delay = 0;
   std::optional<double> maxTime;
   std::optional<double> minTime;
   Node(const std::string& Name, double Delay) : name(Name), delay(Delay) { }
};

using NodePtr = std::shared_ptr<Node>;

class Graph
{

   public:
   NodePtr src; //a(0)
   NodePtr dst; //f

   std::unordered_map<std::string, NodePtr> nodeHashMap; //Hashtable of nodes and their names
   std::unordered_map<std::string, std::list<NodePtr>> edgesMap; //HashTable of nodename and connected nodes
   std::size_t modV; //Vertex cnt
   std::size_t modE; //Edges cnt

   friend class GraphReader;
};

using GraphPtr = std::shared_ptr<Graph>;

class GraphReader
{
   public:
   static
   GraphPtr
   ReadFile(
      const std::string FileName);
};


using NodeNameDoubleValMap = std::unordered_map<std::string, double>;

class TimingGraphProcessor
{
   public:

   TimingGraphProcessor(GraphPtr Graph) : Gr(Graph) { };

   NodeNameDoubleValMap
   CalcAAT();

   NodeNameDoubleValMap
   CalcRAT();

   NodeNameDoubleValMap
   CalcSlackes();

   private:

   GraphPtr Gr;
};