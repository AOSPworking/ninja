#ifndef NINJA_ORIGIN_UTIL_H_
#define NINJA_ORIGIN_UTIL_H_

#include <set>

#include "dyndep.h"
#include "graph.h"

struct Node;
struct Edge;
struct State;

struct OriginUtil {
  OriginUtil(State* state, DiskInterface* disk_interface)
      : state_(state), disk_interface_(disk_interface),
        dyndep_loader_(state, disk_interface) {}
  
  int GetAllImpactNode(vector<Node*> input_nodes);
  int GetImpactNode(Node* node);
  int GraphVizOutput();

  vector<Node*> res;

  State* state_;
  DiskInterface* disk_interface_;
  DyndepLoader dyndep_loader_;
  set<Node*> visited_nodes_;
  EdgeSet visited_edges_;
};

#endif // NINJA_ORIGIN_UTIL_H_