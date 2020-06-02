#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include "camera_favored.h"

using angle = uint16_t;

inline constexpr std::optional<angle> ess_up(angle a) {
    // camera bullshit good lord

    if ((a >= 0x385f && a < 0x4000) ||
        (a >= 0x794f && a < 0x8000)) {
        return {};
    }

    if (a >= 0xff5f && a < 0xff8f) {
        return 0xff91;
    }

    if (a >= 0xbe4f && a < 0xbe7f) {
        return 0xbe81;
    }

    if (a >= 0xbe7f && a < 0xbebf) {
        return 0xbec1;
    }

    if (a >= 0xbebf && a < 0xc001) {
        return {};
    }

    if (a >= 0xff8f) {
        return {};
    }

    for (int i = 0; i < FAVORED_COUNT; i++) {
        angle camera_angle = FAVORED_ANGLES[i];
        if ((camera_angle & 0xFFF0u) >= (a & 0xFFF0u)) {
            // more camera bullshit go to hell
            if (a >= 0xF55F && a < 0xF8BF && (a & 0xFu) == 0xF) {
                i++;
            }
            if (a >= 0xF8BF) {
                i++;
            }
            if (a >= 0xB43F && a < 0xB85F && (a & 0xFu) == 0xF) {
                i++;
            }
            if ((a & 0xFu) == 0xF) {
                return FAVORED_ANGLES[i + 1];
            }
            return FAVORED_ANGLES[i];
        }
    }
    return {};
}

inline constexpr angle ess_left(angle a, int n) {
    return a + 0x0708 * n;
}

inline constexpr angle ess_right(angle a, int n) {
    return a - 0x0708 * n;
}

inline constexpr std::optional<angle> turn_left(angle a) {
    auto ess = ess_up(a);
    if (!ess.has_value()) return ess;
    return ess.value() + 0x4000;
}

inline constexpr std::optional<angle> turn_right(angle a) {
    auto ess = ess_up(a);
    if (!ess.has_value()) return {};
    return ess.value() - 0x4000;
}

inline constexpr angle sidehop_roll_left(angle a) {
    return a + 0x1CD8;
}

inline constexpr angle sidehop_roll_right(angle a) {
    return a - 0x1CD8;
}

inline constexpr angle kokiri_spin(angle a) {
    return a - 0x0CCD;
}

inline constexpr angle biggoron_spin(angle a) {
    return a + 0x1219;
}

inline constexpr angle biggoron_spin_shield(angle a) {
    return a + 0x04F5;
}

inline constexpr angle back_roll(angle a) {
    bool left = true;
    auto camera_angle = ess_up(a);
    if (!camera_angle.has_value()) {
        left = false;
    } else if (camera_angle.value() >= a) {
        left = false;
    }
    if (left) {
        return a + 0x3A98;
    } else {
        return a - 0x3A98;
    }
}

inline constexpr angle backflip_roll(angle a) {
    return a - 0x3A98;
}

inline constexpr std::optional<angle> shield_topright(angle a) {
    auto cam = ess_up(a);
    if (!cam.has_value()) return {};
    return cam.value() - 0x2000;
}

inline constexpr std::optional<angle> shield_topleft(angle a) {
    auto cam = ess_up(a);
    if (!cam.has_value()) return {};
    return cam.value() + 0x2000;
}

inline constexpr std::optional<angle> shield_bottomleft(angle a) {
    auto cam = ess_up(a);
    if (!cam.has_value()) return {};
    return cam.value() + 0x6000;
}

inline constexpr std::optional<angle> shield_bottomright(angle a) {
    auto cam = ess_up(a);
    if (!cam.has_value()) return {};
    return cam.value() - 0x6000;
}