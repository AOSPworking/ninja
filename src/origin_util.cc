#include "origin_util.h"
#include "graphviz.h"

int OriginUtil::GetAllImpactNode(vector<Node*> inputs) {
  for (auto it = inputs.begin(); it < inputs.end(); ++it) {
    GetImpactNode(*it);
  }
  return 0;
}

int OriginUtil::GetImpactNode(Node* node) {
  // check this node whether is visited.
  if (visited_nodes_.find(node) != visited_nodes_.end())
    return 1;
  visited_nodes_.insert(node);

  // if node is a leaf node, return directly.
  vector<Edge*> out_edges = node->GetOutEdges();
  if (0 == out_edges.size())
    return 2;

  // for each out_edge, get their outputs node,
  // and call GetImpactNode recursively.
  for (auto edge : out_edges) {
    vector<Node*> output_nodes = edge->outputs_;
    for (auto output_node : output_nodes) {
      printf("%s -> %s\n", node->path().c_str(), output_node->path().c_str());
      GetImpactNode(output_node);
    }
  }
  return 0;
}
