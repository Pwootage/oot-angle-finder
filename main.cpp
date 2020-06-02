#include <iostream>
#include <sstream>
#include <array>
#include <iomanip>

#include "angles.h"
#include "graph.h"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

using namespace std;

int ESS_COUNT = 8;
bool SWORD_ENABLED = true;
bool BIGGORON_ENABLED = false;
bool NO_CARRY_ENABLED = true;
bool SHIELD_CORNER_ENABLED = false;

struct GenerationResults {
  uint32_t longestPath;
  angle longestAngle;
  std::array<angle, 0x10000> backPath;
};

std::vector<Node> generateGraph();

GenerationResults generateFastestPaths(std::vector<Node> &graph, angle src);

Node generateNode(angle a);

#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif

#ifdef EMSCRIPTEN
angle *result;
std::vector<Node> g_graph;
std::string pathStorage;

int main() {
  result = new angle[0x10000];
  printf("Allocated result array\n");
}

extern "C" {
EMSCRIPTEN_KEEPALIVE
uint16_t *genBackpaths(angle a) {
  if (g_graph.size() == 0) {
      printf("Generating graph...\n");
      g_graph = std::move(generateGraph());
  }
  auto angles = generateFastestPaths(g_graph, a);
  memcpy(result, &angles.backPath[0], sizeof(angle) * 0x10000);
  return result;
}

EMSCRIPTEN_KEEPALIVE
char *genPathForCurrentResult(angle dest) {
  std::vector<angle> stack;
  {
    angle a = dest;
    while (true) {
      angle prev = result[a];
      if (prev == a) {
        stack.push_back(a);
        printf("Done %04x to %04x!\n", prev, dest);
        break;
      }
      stack.push_back(a);
      a = prev;
    }
    std::reverse(stack.begin(), stack.end());
  }
  {
    std::ostringstream res;
    res << hex << setw(2) << stack[0] << " start angle" << endl;
//    printf("%04x start angle x1\n", stack[0]);
    angle previousAngle = stack[0];
    for (int i = 1; i < stack.size(); i++) {
      angle currentAngle = stack[i];
      Node n = generateNode(previousAngle);
      MovementType::Type t;
      int count = 1;
      for (auto &neighbor : n.neighbors) {
        if (neighbor.value == currentAngle) {
          t = neighbor.movementType.type;
          count = neighbor.movementType.count;
        }
      }
      res << nameForType(t) << " x" << dec << count << " to " << hex << currentAngle << endl;
//      printf("%s x%d to %04x\n", nameForType(t), count, currentAngle);
      previousAngle = currentAngle;
    }
    pathStorage = res.str();
  }

  return pathStorage.data();
}
};
//int main() {
//  angle longestStart;
//  angle longestEnd;
//  int longest = 0;
//
//  auto graph = generateGraph();
//  for (uint32_t i = 0; i <= 0x0u; i++) {
//    angle a = i;
//    printf("Getting fastest paths for %04x\n", a);
//    auto angles = generateFastestPaths(graph, a);
//    if (angles.longestPath > longest) {
//      longest = angles.longestPath;
//      longestStart = i;
//      longestEnd = angles.longestAngle;
//    }
//  }
//
//  printf("Longest angle: %04x -> %04x: %d steps\n", longestStart, longestEnd, longest);
//
//  return 0;
//}
#else
int main() {
  angle longestStart;
  angle longestEnd;
  int longest = 0;

  auto graph = generateGraph();
  for (uint32_t i = 0; i <= 0x10000u; i++) {
    angle a = i;
    printf("Getting fastest paths for %04x\n", a);
    auto angles = generateFastestPaths(graph, a);
    if (angles.longestPath > longest) {
      longest = angles.longestPath;
      longestStart = i;
      longestEnd = angles.longestAngle;
    }
  }

  printf("Longest angle: %04x -> %04x: %d steps\n", longestStart, longestEnd, longest);

  return 0;
}
#endif

std::vector<Node> generateGraph() {
  std::vector<Node> nodes;
  for (uint32_t i = 0; i <= 0xFFFFu; i++) {
    angle a = i;
    Node n = generateNode(a);

    nodes.push_back(n);
  }
  printf("Finished generating graph\n");
  return nodes;
}

Node generateNode(angle a) {
  // TODO: exclude types we don't care about, probably just as #defines tbh
  Node n;
  n.value = a;

  // it's so fast this isn't relevant
//    if (i % 0x1000 == 0) {
//      printf("Angle %x\n", i);
//    }
  // Stuff you can always do
  auto _ess_up = ess_up_adjust(a);
  if (_ess_up.has_value()) {
    n.neighbors.push_back(Neighbor{
        .movementType = {.type = MovementType::Type::ess_up},
        .value = _ess_up.value()
    });
  }

  for (int count = 1; count <= ESS_COUNT; count++) {
    n.neighbors.push_back(Neighbor{
        .movementType = {.type = MovementType::Type::ess_left, .count = count},
        .value = ess_left(a, count)
    });
    n.neighbors.push_back(Neighbor{
        .movementType = {.type = MovementType::Type::ess_right, .count = count},
        .value = ess_right(a, count)
    });
  }
  auto _turn_left = turn_left(a);
  if (_turn_left.has_value()) {
    n.neighbors.push_back(Neighbor{
        .movementType = {.type = MovementType::Type::turn_left},
        .value = _turn_left.value()
    });
  }
  auto _turn_right = turn_right(a);
  if (_turn_right.has_value()) {
    n.neighbors.push_back(Neighbor{
        .movementType = {.type = MovementType::Type::turn_right},
        .value = _turn_right.value()
    });
  }
  auto _turn_180 = turn_180(a);
  if (_turn_180.has_value()) {
    n.neighbors.push_back(Neighbor{
        .movementType = {.type = MovementType::Type::turn_180},
        .value = _turn_180.value()
    });
  }

  // Stuff you can do if you're not carrying anything
  if (NO_CARRY_ENABLED) {
    n.neighbors.push_back(Neighbor{
        .movementType = {.type = MovementType::Type::sidehop_roll_left},
        .value = sidehop_roll_left(a)
    });
    n.neighbors.push_back(Neighbor{
        .movementType = {.type = MovementType::Type::sidehop_roll_right},
        .value = sidehop_roll_right(a)
    });
    n.neighbors.push_back(Neighbor{
        .movementType = {.type = MovementType::Type::ess_down_sideroll},
        .value = ess_down_sideroll(a)
    });
    n.neighbors.push_back(Neighbor{
        .movementType = {.type = MovementType::Type::backflip_roll},
        .value = backflip_roll(a)
    });
  }

  if (SWORD_ENABLED) {
    n.neighbors.push_back(Neighbor{
        .movementType = {.type = MovementType::Type::kokiri_spin},
        .value = kokiri_spin(a)
    });
  }

  if (BIGGORON_ENABLED) {
    n.neighbors.push_back(Neighbor{
        .movementType = {.type = MovementType::Type::biggoron_spin},
        .value = biggoron_spin(a)
    });
    n.neighbors.push_back(Neighbor{
        .movementType = {.type = MovementType::Type::biggoron_spin_shield},
        .value = biggoron_spin_shield(a)
    });
  }

  if (SHIELD_CORNER_ENABLED) {
    auto _shield_topright = shield_topright(a);
    if (_shield_topright.has_value()) {
      n.neighbors.push_back(Neighbor{
          .movementType = {.type = MovementType::Type::shield_topright},
          .value = _shield_topright.value()
      });
    }
    auto _shield_topleft = shield_topleft(a);
    if (_shield_topleft.has_value()) {
      n.neighbors.push_back(Neighbor{
          .movementType = {.type = MovementType::Type::shield_topleft},
          .value = _shield_topleft.value()
      });
    }
    auto _shield_bottomright = shield_bottomright(a);
    if (_shield_bottomright.has_value()) {
      n.neighbors.push_back(Neighbor{
          .movementType = {.type = MovementType::Type::shield_bottomright},
          .value = _shield_bottomright.value()
      });
    }
    auto _shield_bottomleft = shield_bottomleft(a);
    if (_shield_bottomleft.has_value()) {
      n.neighbors.push_back(Neighbor{
          .movementType = {.type = MovementType::Type::shield_bottomleft},
          .value = _shield_bottomleft.value()
      });
    }
  }
  return n;
}


GenerationResults generateFastestPaths(std::vector<Node> &graph, angle src) {
  std::array<angle, 0x10000> backPath{0};
  std::array<bool, 0x10000> found{false};
  int remaining = backPath.size() - 1; // we found us after all

  std::vector<Node *> heap;

  heap.push_back(&graph[src]);
  std::push_heap(heap.begin(), heap.end(), sort_nodes);
  graph[src].distance = 0;

  backPath[src] = src;
  found[src] = true;
  int visited = 0;
  uint32_t lastDistance = 0;
  Node *last = nullptr;
  while (!heap.empty() && remaining > 0) {
    // Get the top of the heap
    std::pop_heap(heap.begin(), heap.end(), sort_nodes);
    auto current = heap.back();
    last = current;
    heap.pop_back();
    // Check the current distance
    int distance = current->distance;
    if (distance != lastDistance) {
      lastDistance = distance;
      printf("Distance: %d, visited: %d, found: %d, remaining: %d, heap size %ld\n", distance, visited,
             0x10000 - remaining, remaining, heap.size());
    }
    visited++;

    // For each neighbor
    for (auto &neighbor : current->neighbors) {
      if (found[neighbor.value] == 1) continue; // We don't care anymore, we already found a path tho this
      // Yay, someone new
      Node *node = &graph[neighbor.value];
      node->distance = distance + 1;
      heap.push_back(node);
      std::push_heap(heap.begin(), heap.end(), sort_nodes);

      found[node->value] = true;
      backPath[node->value] = current->value;
      remaining--;
    }
  }

  return {
      .longestPath = lastDistance,
      .longestAngle = last->value,
      .backPath = backPath,
  };
}
