#include <doctest.h>
#include <graph/Graph.h>
#include <graph/move.h>

#include <graph/breath_first_search.h>

using namespace graph;
using namespace move;


TEST_CASE("Graph") {
  auto adj_matrix = example_01::adj_matrix();
  auto initial_positions_red = example_01::initial_positions_red();
  auto initial_positions_blue = example_01::initial_positions_blue();
  auto graph =
      Graph(adj_matrix, initial_positions_red, initial_positions_blue);

  SUBCASE("Equality"){
    auto same_graph = Graph(graph);
    REQUIRE(graph == same_graph);
    auto other_graph = Graph(adj_matrix, initial_positions_blue, initial_positions_red);
    REQUIRE_FALSE(graph == other_graph);
  }

  SUBCASE("neighbors") {
    auto adj_matrix = AdjazenzMatrix({{1, 2, 3}, {0, 3}, {0, 3}, {0, 1, 2}});
    auto graph = Graph(adj_matrix);
    auto first_node = graph[0];
    auto neighbors = graph.node_neighbors(first_node);
    REQUIRE(neighbors == std::vector{Node{1}, Node{2}, Node{3}});
    auto second_node = graph[1];
    neighbors = graph.node_neighbors(second_node);
    REQUIRE(neighbors == std::vector{Node{0}, Node{3}});
  }

  SUBCASE("breath_first_search") {
    auto always_true = [](Node const &node) {
      return true;
    };
    REQUIRE(graph.breath_first_search( 0, 11, always_true));
    REQUIRE(graph.breath_first_search( 6, 9, always_true));
    auto no_jumps = [](Node const &node) {
      return node.field == Field::EMPTY;
    };
    REQUIRE(graph.breath_first_search( 2, 6, no_jumps));
    REQUIRE(graph.breath_first_search( 6, 7, no_jumps));
    REQUIRE(graph.breath_first_search( 7, 4, no_jumps));
    REQUIRE_FALSE(graph.breath_first_search( 4, 8, no_jumps));
    REQUIRE_FALSE(graph.breath_first_search( 6, 10, no_jumps));
    REQUIRE_FALSE(graph.breath_first_search( 1, 5, no_jumps));
    REQUIRE_FALSE(graph.breath_first_search( 6, 3, no_jumps));
    REQUIRE_FALSE(graph.breath_first_search( 0, 11, no_jumps));
    graph.do_move({2,5});
    REQUIRE_FALSE(graph.breath_first_search( 1, 5, no_jumps));
    REQUIRE_FALSE(graph.breath_first_search( 1, 7, no_jumps));
    REQUIRE_FALSE(graph.breath_first_search( 1, 6, no_jumps));
    REQUIRE_FALSE(graph.breath_first_search( 8, 6, no_jumps));
    REQUIRE_FALSE(graph.breath_first_search( 8, 5, no_jumps));
    REQUIRE(graph.breath_first_search( 1, 4, no_jumps));
    REQUIRE(graph.breath_first_search( 3, 4, no_jumps));
    REQUIRE(graph.breath_first_search( 5, 4, no_jumps));
    REQUIRE(graph.breath_first_search( 5, 6, no_jumps));
    REQUIRE(graph.breath_first_search( 5, 7, no_jumps));
    REQUIRE(graph.breath_first_search( 8, 7, no_jumps));
  }

  SUBCASE("Possible moves"){
    auto possible_moves = graph.possible_moves();
    REQUIRE(possible_moves == std::vector<Move>{{2,4}, {2,5}, {2,6}, {2,7}, {8,4}, {8,5}, {8,6}, {8,7}});
  }

  SUBCASE("Print") {
    graph.print();
  }

  SUBCASE("Move") {
    SUBCASE("Move is_inverse") {
      static_assert(Move{10, 10}.is_inverse({10, 10}));
      static_assert(Move{10, 20}.is_inverse({20, 10}));
      static_assert(Move{20, 10}.is_inverse({10, 20}));
      static_assert(not Move{20, 10}.is_inverse({30, 20}));
    }
    SUBCASE("Move equality") {
      static_assert(Move{10, 10} == Move{10, 10});
      static_assert(Move{1, 10} != Move{10, 10});
    }
    SUBCASE("Invalid Move: Source field empty"){
      REQUIRE(graph.invalid_move({4,2}).has_value());
    }
    SUBCASE("Invalid Move: Destination field not empty"){
      REQUIRE(graph.invalid_move({2,10}).has_value());
      REQUIRE(graph.invalid_move({2,3}).has_value());
    }
    SUBCASE("Valid Move"){
      REQUIRE_FALSE(graph.invalid_move({2,4}).has_value());
      REQUIRE_FALSE(graph.invalid_move({8,6}).has_value());
      REQUIRE_FALSE(graph.invalid_move({2,7}).has_value());
    }
    SUBCASE("Do Move") {
      auto move = Move{2, 4};
      graph.do_move(move);
      REQUIRE(graph[move.source].field == Field::EMPTY);
      REQUIRE(graph[move.destination].field == Field::RED);
      move = {8, 6};
      graph.do_move(move);
      REQUIRE(graph[move.source].field == Field::EMPTY);
      REQUIRE(graph[move.destination].field == Field::BLUE);

      SUBCASE("Do invalid move"){
        graph.print();
        auto move = Move{1,4};
        REQUIRE(graph.invalid_move(move));
        move = Move{1,5};
        REQUIRE(graph.invalid_move(move));
        move = Move{8,2};
        REQUIRE(graph.invalid_move(move));
      }
    }
  }
}