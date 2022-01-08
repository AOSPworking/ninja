#ifndef NINJA_ORIGIN_UTIL_H_
#define NINJA_ORIGIN_UTIL_H_

#include <set>
#include <queue>
#include <map>

#include "dyndep.h"
#include "graph.h"

struct Node;
struct Edge;
struct State;

struct OriginUtil {
  OriginUtil(vector<Node*> input_target, State* state,
             DiskInterface* disk_interface)
      : state_(state),
        disk_interface_(disk_interface),
        input_target_(input_target),
        dyndep_loader_(state, disk_interface) {}
  
  State* state_;
  DiskInterface* disk_interface_;
  DyndepLoader dyndep_loader_;
  set<Node*> visited_nodes_;
  EdgeSet visited_edges_;

  vector<Node*> input_target_;
  int GetAllImpactNode(vector<Node*> input_nodes);

  vector<pair<Node*, size_t>> nodes_dist;
  void ReverseDijkstra();
  void PrintJSON();
  void PrintNode(string indent, Node* node, size_t distance);
  void PrintEdge(string indent, Edge* edge);

private:
  map<Node*, size_t> reverse_djk_m;

  string VecNodeFmt(vector<Node*> inputs) {
    string res = "";
    if (0 == inputs.size())
      return res;
    else if (1 == inputs.size())
      return inputs[0]->path();
    else {
      res += inputs[0]->path();
      for (auto it = inputs.begin()+1; it < inputs.end(); ++it)
        res += ", " + (*it)->path();
      return res;
    }
  }

  void GetOutputNodeByNode(Node* input_node, vector<Node*>* output_nodes) {
    vector<Edge*> edges = input_node->GetOutEdges();
    for (auto edge : edges) {
      vector<Node*> edge_outputs = edge->outputs_;
      output_nodes->insert((*output_nodes).end(),
                            edge_outputs.begin(), edge_outputs.end());
    }
    return;
  }
};

#endif // NINJA_ORIGIN_UTIL_H_