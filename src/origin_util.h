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
  OriginUtil(vector<Node*> input_target)
      : input_target_(input_target) {}
  
  set<Node*> ready_nodes_;
  EdgeSet visited_edges_;

  vector<Node*> input_target_;
  int GetAllImpactNode();

  vector<Node*> direct_nodes_;
  void GetAllDirectNodes();

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

  /**
   * @brief 判断某节点的所有 “属于直接影响节点的输入节点” 是否 ready。
   * 也就是 source 是否都 ready 了。
   * @param node 某节点
   * @return true ready
   * @return false not ready
   */
  bool AllSourcesReady(Node* node) {
    for (auto input : node->in_edge()->inputs_) {
      // printf("node to test ready: %s, its input: %s\n", node->path().c_str(), input->path().c_str());
      // input 不在 direct_nodes 中
      if (std::find(direct_nodes_.begin(), direct_nodes_.end(), input) == direct_nodes_.end()) {
        continue;
      } else {
        // 存在当前这个 input 不 ready
        if (ready_nodes_.find(input) == ready_nodes_.end())
          return false;
      }
    }
    return true;
  }

  /**
   * @brief Get the Max Distance By Node object。
   * 但是需要注意，只看当前 node 的所有 source input，impact 不看。
   * @param node 某节点
   * @return size_t 这个节点的 dist
   */
  size_t GetMaxDistanceByNode(Node* node) {
    size_t max_dist = 0;
    for (auto input : node->in_edge()->inputs_) {
      if (std::find(direct_nodes_.begin(), direct_nodes_.end(), input) != direct_nodes_.end()) {
        auto it = reverse_djk_m.find(input);
        if (it != reverse_djk_m.end() && max_dist < it->second) {
          max_dist = it->second;
        }
      }
    }
    return max_dist;
  }
};

#endif // NINJA_ORIGIN_UTIL_H_