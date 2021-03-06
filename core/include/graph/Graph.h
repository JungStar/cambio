#pragma once
#include <cassert>
#include <type_traits>
#include <queue>
#include <vector>
#include <set>
#include <iostream>
#include <string>
#include <optional>

#include <graph/Node.h>
#include <graph/move.h>

namespace graph {

using AdjazenzMatrix = std::vector<std::vector<size_t>>;

class Graph {
public:
  explicit Graph(AdjazenzMatrix adjazenzMatrix,
                 std::vector<size_t> positions_red = {},
                 std::vector<size_t> positions_blue = {}) noexcept
      : adj_matrix(std::move(adjazenzMatrix)) {
    nodes.reserve(adj_matrix.size());
    for (auto i = 0; i < adj_matrix.size(); ++i){
      if (std::find(begin(positions_red), end(positions_red), i) != end(positions_red))
        nodes.emplace_back(i, Field::RED);
      else if (std::find(begin(positions_blue), end(positions_blue), i) != end(positions_blue))
        nodes.emplace_back(i, Field::BLUE);
      else
        nodes.emplace_back(i);
    }
  }

  [[nodiscard]] auto size() const noexcept -> size_t {
    return adj_matrix.size();
  }

  [[nodiscard]] auto operator[](size_t node_index) const noexcept -> Node {
    return nodes[node_index];
  }

  [[nodiscard]] auto operator== (Graph const& other) const noexcept -> bool{
    if (not (adj_matrix == other.adj_matrix ))
      return false;
    for (auto i = 0; i < nodes.size(); i++){
      if (not (nodes[i].field == other.nodes[i].field))
        return false;
    }
    return true;
  }
  [[nodiscard]] auto operator!= (Graph const& other) const noexcept -> bool{
    return not operator==(other);
  }

  [[nodiscard]] auto node_neighbors(Node const &node) const noexcept
      -> std::vector<Node> {
    auto result = std::vector<Node>();
    assert(node.index >= 0 && node.index < adj_matrix.size() &&
           "Node index is out of range");
    result.reserve(adj_matrix[node.index].size());
    auto const &neighbor_indices = adj_matrix[node.index];
    std::transform(begin(neighbor_indices), end(neighbor_indices),
                   std::back_inserter(result),
                   [this](auto node_index) { return operator[](node_index); });
    return result;
  }

  [[nodiscard]] auto invalid_move(move::Move const& move) const noexcept -> std::optional<std::string>{
    if(nodes[move.source].field == Field::EMPTY)
        return {"ILLEGAL MOVE: The source field is empty"};
    if(nodes[move.destination].field != Field::EMPTY)
      return {"ILLEGAL MOVE: The destination field is emtpy"};
    auto no_jumps = [](Node const &node) {
      return node.field == Field::EMPTY;
    };
    if(not breath_first_search(move.source, move.destination, no_jumps))
      return {"ILLEGAL MOVE: Unreachable destination"};
    return {};
  }

  template <class Predicate>
  [[nodiscard]] auto breath_first_search(size_t start_index,
                                         size_t end_index,
                                         Predicate predicate) const noexcept
  -> bool {
    auto nodes = std::queue<Node>(); // Change to vector?
    auto visited_nodes = std::set<Node>();
    nodes.push(operator[](start_index));

    while (not nodes.empty()) {
      auto const node = nodes.front();
      if (node.index == end_index)
        return true;
      nodes.pop();
      visited_nodes.insert(node);
      auto const neighbors = node_neighbors(node);
      for (auto const &neighbor : neighbors)
        if (not visited_nodes.contains(neighbor) and predicate(neighbor))
          nodes.push(neighbor);
    }
    return false;
  }

  auto do_move(move::Move const& move) noexcept -> void{
    auto invalid = invalid_move(move);
    assert(not invalid);
    nodes[move.destination].field = nodes[move.source].field;
    nodes[move.source].field = Field::EMPTY;
  }

  [[nodiscard]] auto possible_moves() const noexcept -> std::vector<move::Move>{
    auto moves = std::vector<move::Move>{};
    for (size_t source = 0; source< size(); source++){
      for (size_t destination = 0; destination < size(); destination++){
        if (source == destination)
          continue;
        auto move = move::Move{source, destination};
        if (not invalid_move(move))
          moves.push_back(move);
      }
    }
    return moves;
  }

  /// Extremely basic, only for current test problem
  auto print() const noexcept -> void {
    assert(size() == 12);
    std::cout << "      " << nodes[3] << "           " << nodes[9] << "\n";
    std::cout << "      |           |\n";
    for(auto i : {0,1,2,4,5,7,8,10,11}) {
      std::cout << nodes[i];
      if (i != 11)
        std::cout << "--";
    }
    std::cout<< "\n            |" << std::endl;
    std::cout<< "            " << nodes[6] << std::endl;
  }

private:
  AdjazenzMatrix adj_matrix;
  std::vector<Node> nodes;
};

namespace example_01{
[[nodiscard]] auto adj_matrix() noexcept -> AdjazenzMatrix {
  return {std::vector<size_t>{1u},          std::vector<size_t>{0u, 2u},
          std::vector<size_t>{1u, 3u, 4u},  std::vector<size_t>{2u},
          std::vector<size_t>{2u, 5u},      std::vector<size_t>{4u, 6u, 7u},
          std::vector<size_t>{5u},          std::vector<size_t>{5u, 8u},
          std::vector<size_t>{7u, 9u, 10u}, std::vector<size_t>{8u},
          std::vector<size_t>{8u, 11u},     std::vector<size_t>{10u}};
}
[[nodiscard]] auto initial_positions_red() noexcept -> std::vector<size_t>{
  return {0,1,2,3};
}
[[nodiscard]] auto initial_positions_blue() noexcept -> std::vector<size_t>{
  return {8,9,10,11};
}
}
} // namespace graph