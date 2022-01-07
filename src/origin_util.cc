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
  if (0 == out_edges.size()) {
    printf("leaf node: %s\n", node->path().c_str());
    return 2;
  }

  // for each out_edge, get their outputs node,
  // and call GetImpactNode recursively.
  for (auto edge : out_edges) {
    vector<Node*> output_nodes = edge->outputs_;
    // check the edge whether is visited.
    if (visited_edges_.find(edge) != visited_edges_.end())
      continue;
    visited_edges_.insert(edge);

    for (auto node : output_nodes) {
      GetImpactNode(node);
    }
  }
  return 0;
}

int OriginUtil::GraphVizOutput() {
  GraphViz graph = GraphViz(state_, disk_interface_);
  graph.Start();
  for (auto it = visited_nodes_.begin(); it != visited_nodes_.end(); ++it) {
    graph.AddTarget(*it);
  }
  graph.Finish();
  return 0;
}