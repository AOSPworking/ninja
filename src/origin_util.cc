#include "origin_util.h"
#include "graphviz.h"

int OriginUtil::GetAllImpactNode() {
  GetAllDirectNodes();
  ReverseDijkstra();
  PrintJSON();

  return 0;
}

/**
 * @brief 获取所有直接影响的 Nodes，也就是 target 和 source 中的
 */
void OriginUtil::GetAllDirectNodes() {
  queue<Node*> q;
  for (auto node : input_target_) {
    q.push(node);
    direct_nodes_.push_back(node);
  }

  while (!q.empty()) {
    Node* node = q.front();
    q.pop();
    
    vector<Node*> output_nodes;
    GetOutputNodeByNode(node, &output_nodes);
    for (auto node : output_nodes) {
      if (std::find(direct_nodes_.begin(), direct_nodes_.end(), node) == direct_nodes_.end()) {
        q.push(node);
        direct_nodes_.push_back(node);
      }
    }
  }
}

void OriginUtil::ReverseDijkstra() {
  queue<Node*> q;
  // init queue and map.
  for (auto node : input_target_) {
    q.push(node);
    reverse_djk_m.insert({ node, 1 });
    ready_nodes_.insert(node);
  }

  while (!q.empty()) {
    Node* node = q.front();
    q.pop();

    vector<Node*> output_nodes;
    GetOutputNodeByNode(node, &output_nodes);
    // 获取当前节点所有输出后，更新 map 中的距离
    for (auto node : output_nodes) {
      // 如果 target 节点并不是所有 source 都 ready 了，那就跳过
      if (!AllSourcesReady(node)) {
        continue;
      }

      if (ready_nodes_.find(node) == ready_nodes_.end()) {
        q.push(node);
        size_t dist = GetMaxDistanceByNode(node);
        reverse_djk_m.insert({ node, dist + 1 });
        ready_nodes_.insert(node);
      }
    }
  }

  // sort
  for (auto it : reverse_djk_m)
    nodes_dist.push_back({ it.first, it.second });
  using _pns = pair<Node*, size_t>;
  sort(nodes_dist.begin(), nodes_dist.end(), [](_pns a, _pns b) {
      return a.second < b.second;
  });
}

void OriginUtil::PrintJSON() {
  printf("{\n");
  printf("  \"nodes\": [\n");
  for (auto it = nodes_dist.begin(); it < nodes_dist.end(); ++it) {
    PrintNode("    ", (*it).first, (*it).second);
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
    if (has_edges && it != nodes_dist.end())
      printf(",");
    printf("\n");

    has_edges = true;
    PrintEdge("    ", edge);
    visited_edges_.insert(edge);
  }
  // 补一个 \n，因为输出最后一个
  printf("\n");
  printf("  ],\n");
  printf("  \"node_num\": %ld,\n", nodes_dist.size());
  printf("  \"edge_num\": %ld\n", visited_edges_.size());
  printf("}\n");
  return;
}

void OriginUtil::PrintNode(string indent, Node* node, size_t distance) {
  printf("%s{", indent.c_str());
  printf("\"name\": \"%s\", ", node->path().c_str());
  printf("\"distance\": %ld", distance);
  printf("}");
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