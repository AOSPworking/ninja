#include "origin_util.h"
#include "graphviz.h"

int OriginUtil::GetAllImpactNode(vector<Node*> inputs) {
  for (auto it = inputs.begin(); it < inputs.end(); ++it) {
    GetImpactNode(*it);
  }
  return 0;
}

int OriginUtil::GetImpactNode(Node* node) {
  // if node is a leaf node, return directly.
  vector<Edge*> out_edges = node->GetOutEdges();
  if (0 == out_edges.size())
    return 2;

  // for each out_edge, get their outputs node,
  // and call GetImpactNode recursively.
  for (auto edge : out_edges) {
    // check whether edge is visited.
    if (visited_edges_.find(edge) != visited_edges_.end())
      continue;
    visited_edges_.insert(edge);

    vector<Node*> input_nodes = edge->inputs_;
    vector<Node*> output_nodes = edge->outputs_;
    printf("rule_name: %s, ", edge->rule_->name_.c_str());
    printf("explicit: %d, implicit: %d, order_only: %d ",
            edge->explicit_deps_,
            edge->implicit_deps_ - edge->explicit_deps_,
            edge->order_only_deps_ - edge->implicit_deps_);
    printf("%ld, %ld", input_nodes.size(), output_nodes.size());
    printf("%s -> %s\n", VecNodeFmt(input_nodes).c_str(),
                         VecNodeFmt(output_nodes).c_str());
    for (auto output_node : output_nodes) {
      GetImpactNode(output_node);
    }
  }
  return 0;
}
