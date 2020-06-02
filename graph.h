#pragma once

#include "angles.h"
#include <vector>

struct MovementType {
  enum class Type {
    ess_up,
    ess_left,
    ess_right,
    turn_left,
    turn_right,
    turn_180,

    sidehop_roll_left,
    sidehop_roll_right,
    ess_down_sideroll,
    backflip_roll,

    kokiri_spin,

    biggoron_spin,
    biggoron_spin_shield,

    shield_topright,
    shield_topleft,
    shield_bottomleft,
    shield_bottomright
  } type;
  int count = 1;
};

inline constexpr bool isAdjustment(MovementType::Type type) {
  switch (type) {
    case MovementType::Type::ess_up:
    case MovementType::Type::turn_left:
    case MovementType::Type::turn_right:
    case MovementType::Type::turn_180:
    case MovementType::Type::shield_topright:
    case MovementType::Type::shield_topleft:
    case MovementType::Type::shield_bottomleft:
    case MovementType::Type::shield_bottomright:
      return true;

    case MovementType::Type::ess_left:
    case MovementType::Type::ess_right:
    case MovementType::Type::sidehop_roll_left:
    case MovementType::Type::sidehop_roll_right:
    case MovementType::Type::kokiri_spin:
    case MovementType::Type::biggoron_spin:
    case MovementType::Type::biggoron_spin_shield:
    case MovementType::Type::ess_down_sideroll:
    case MovementType::Type::backflip_roll:
      return false;
  }
}

inline constexpr const char* nameForType(MovementType::Type type) {
  switch (type) {
    case MovementType::Type::ess_up:
      return "ess up";
    case MovementType::Type::ess_left:
      return "ess left";
    case MovementType::Type::ess_right:
      return "ess right";
    case MovementType::Type::turn_left:
      return "turn left";
    case MovementType::Type::turn_right:
      return "turn right";
    case MovementType::Type::sidehop_roll_left:
      return "sidehop roll left";
    case MovementType::Type::sidehop_roll_right:
      return "sidehop roll right";
    case MovementType::Type::kokiri_spin:
      return "kokori/master spin";
    case MovementType::Type::biggoron_spin:
      return "biggoron spin";
    case MovementType::Type::biggoron_spin_shield:
      return "biggoron spin shield cancel";
    case MovementType::Type::ess_down_sideroll:
      return "ess down sideroll";
    case MovementType::Type::backflip_roll:
      return "backflip roll";
    case MovementType::Type::shield_topright:
      return "shield top right";
    case MovementType::Type::shield_topleft:
      return "shield top left";
    case MovementType::Type::shield_bottomleft:
      return "shield bottom left";
    case MovementType::Type::shield_bottomright:
      return "shield bottom right";
    case MovementType::Type::turn_180:
      return "turn 180";
  }
}

struct Neighbor {
  MovementType movementType;
  angle value = 0;
};

struct Node {
  angle value = 0;
  std::vector<Neighbor> neighbors;
  uint32_t distance = UINT32_MAX;
};

inline constexpr bool sort_nodes(const Node *a, const Node *b) {
  return a->distance > b->distance; // Look, I want it to sort the other way
}