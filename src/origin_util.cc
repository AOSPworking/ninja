#include "origin_util.h"
#include "graphviz.h"

int OriginUtil::GetAllImpactNode(vector<Node*> inputs) {
  ReverseDijkstra(inputs);
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

void OriginUtil::ReverseDijkstra(vector<Node*> input_nodes) {
  queue<Node*> q;
  // init queue and map.
  for (auto node : input_nodes) {
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
      return a.second < b.second;
  });

  for (auto elem : nodes_dist) {
    printf("%s : %d\n", elem.first->path().c_str(), elem.second);
  }
}