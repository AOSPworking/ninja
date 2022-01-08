#include "origin_util.h"
#include "graphviz.h"

int OriginUtil::GetAllImpactNode(vector<Node*> inputs) {
  ReverseDijkstra();
  PrintJSON();
  return 0;
}

void OriginUtil::ReverseDijkstra() {
  queue<Node*> q;
  // init queue and map.
  for (auto node : input_target_) {
    q.push(node);
    reverse_djk_m.insert({ node, 1 });
  }

  while (!q.empty()) {
    Node* node = q.front();
    q.pop();
    auto node_it = reverse_djk_m.find(node);
    size_t dist = node_it->second;

    vector<Node*> output_nodes;
    GetOutputNodeByNode(node, &output_nodes);
    // 获取当前节点所有输出后，更新 map 中的距离
    for (auto node : output_nodes) {
      auto iter = reverse_djk_m.find(node);
      if (iter != reverse_djk_m.end() && iter->second < dist + 1)
        iter->second = dist + 1;

      else if (iter == reverse_djk_m.end()) {
        // 说明可以入 queue，因为 map 还没有记录过，说明是新节点
        q.push(node);
        reverse_djk_m.insert({ node, dist + 1 });
      }
    }
  }

  // sort
  for (auto it : reverse_djk_m)
    nodes_dist.push_back({ it.first, it.second });
  using _pns = pair<Node*, size_t>;
  sort(nodes_dist.begin(), nodes_dist.end(), [](_pns a, _pns b) {
      return a.second > b.second;
  });
}

void OriginUtil::PrintJSON() {
  printf("{\n");
  printf("  \"nodes\": [\n");
  for (auto it = nodes_dist.begin(); it < nodes_dist.end(); ++it) {
    printf("    \"%s\"", (*it).first->path().c_str());
    if (it + 1 != nodes_dist.end())
      printf(",");
    printf("\n");
  }
  printf("  ],\n");
  printf("  \"edges\": [");
  bool has_edges = false;
  for (auto it = nodes_dist.begin(); it < nodes_dist.end(); ++it) {
    Edge* edge = (*it).first->in_edge();
    if (!edge || visited_edges_.find(edge) != visited_edges_.end())
      continue;

    // 先打印 "," 意思是只有存在下一个 edge 需要打印时，才打印 ","
    if (has_edges && it + 1 != nodes_dist.end())
      printf(",");
    printf("\n");

    has_edges = true;
    PrintEdge("    ", edge);
    visited_edges_.insert(edge);
  }
  printf("  ],\n");
  printf("  \"node_num\": %ld,\n", nodes_dist.size());
  printf("  \"edge_num\": %ld\n", visited_edges_.size());
  printf("}\n");
  return;
}

void OriginUtil::PrintEdge(string indent, Edge* edge) {
  printf("%s{", indent.c_str());
  printf("\"target\": [");
  vector<Node*> outputs = edge->outputs_;

  for (auto it = outputs.begin(); it < outputs.end(); ++it) {
    printf("\"%s\"", (*it)->path().c_str());
    if (it + 1 != outputs.end())
      printf(",");
  }
  printf("], ");

  vector<Node*> direct_nodes, impact_nodes;
  for (Node* node : edge->inputs_) {
    if (reverse_djk_m.find(node) != reverse_djk_m.end())
      direct_nodes.push_back(node);
    else
      impact_nodes.push_back(node);
  }

  printf("\"source\": [");
  for (auto it = direct_nodes.begin(); it < direct_nodes.end(); ++it) {
    printf("\"%s\"", (*it)->path().c_str());
    if (it + 1 != direct_nodes.end())
      printf(",");
  }
  printf("], ");

  printf("\"impact_source\": [");
  for (auto it = impact_nodes.begin(); it < impact_nodes.end(); ++it) {
    printf("\"%s\"", (*it)->path().c_str());
    if (it + 1 != impact_nodes.end())
      printf(",");
  }
  printf("]}");
}