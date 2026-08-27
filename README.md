# 🎯 range-day

**range-day** is a lightweight, cache-friendly 2D top-down shooter template.

It is perfect for anyone who wants to start learning low-level game development, or anyone who needs a base template to build a 2D top-down shooter.

---

<p align="center">
  <img src="https://imgtree.co/direct/7-F4ocMN.png" width="48%" />
  <img src="https://imgtree.co/direct/OhS8A4p4.png" width="48%" />
</p>

---

### ✨ Features

* **Tech Stack:** Built with C17 and Raylib.
* **Cache-Friendly Architecture:** Custom macro-based ECS using parallel component arrays with $O(1)$ component access.
* **Decoupled Design:** Rendering and input systems are completely separated from core gameplay logic.
* **Zero Assets:** 100% procedural vector graphics and generated audio—no external asset files required.
* **Ready-to-Use Components:** Built-in transform, collider, render, player, and weapon components.
* **Cross-Platform & Toolchain Ready:** Automatic compiler detection for MinGW and MSVC (Windows) and macOS. Works out
  of the box in CLion and Visual Studio.
* **Single Binary Output:** Compiles directly into a single executable for easy distribution.
* **Free & Open:** Completely free to use and modify for any project.

---

### 📌 Developer Notes

* **System Decoupling:** Avoid making direct calls from one system to another. Systems should operate independently.
* **Cached Player Lookups:** Systems like `player_system` directly target a cached player `EntityId` instead of iterating over every entity. This is an intentional simplification suited for single-player/small-scale projects.
* **Isolating Raylib:** Keep gameplay systems and components free of direct Raylib dependencies. See `src/systems/raylib/input_system.c` -> `src/core/input_bridge.c` -> `src/systems/input_system.c` to learn how external dependencies are bridged.
* **Vector Math:** Use `Vec2` from `src/core/math.h` for all vector operations outside of `src/systems/raylib/`.
* **Central Configuration:** Use `src/core/config.h` for global constants and game parameters.
* **Fixed Timestep:** This template uses a fixed timestep accumulator loop in `src/game/main.c` to provide smooth, frame-independent physics and logic updates.
* **Debug Rendering:** Set `RENDER_COLLIDERS` to `1` in your configuration to draw collider bounds.
* **Scene Management:** Use `src/game/scene.c` to manage entity creation and lifespan for a given scene.
* **Prefab Builders:** Use helper functions inside `src/game/prefabs/` to compose entities (see `player_prefab.c` for reference).
* **Zero Asset Pipeline**: Audio and textures are fully generated via code at startup—no external files to load or pack.

---

### ⚙️ ECS 

* **Entities** are just integer IDs (`uint32_t`). They represent a "game object" in your scene, but contain no logic or data themselves.
* **Components** are pure data structures attached to an entity (e.g., `Transform` for position, `Collider` for collision bounds).
* **Systems** are stateless functions that contain game logic. They iterate over entities, check if they have a specific combination of components, and perform actions.
* **Example:** The movement system looks for every entity that has both a `Transform` and a `Velocity` component, and updates the position based on velocity.

### 🔬 ECS In-Depth

This ECS uses macro expansion to generate flat, statically allocated component stores where an Entity ID acts directly as an array index for $O(1)$ lookups.

#### 1. What the Preprocessor Generates

```c
// components/player.h

#pragma once

#include "../ecs/ecs.h"

typedef struct {
    float walk_speed;
    float run_speed;
    int locomotion_state;
} Player;

COMPONENT_DEFINE(Player, player);

```

```c
// components/player.c
#include "player.h"

COMPONENT_IMPLEMENT(Player, player);
```

When you call `COMPONENT_DEFINE` in your header and `COMPONENT_IMPLEMENT` in your source file, the preprocessor expands those macros into pure C structs and global storage allocations:

```c
// --- What COMPONENT_DEFINE(Player, player) generates in player.h ---
typedef struct {
    Player data[256];
    ComponentMask mask; // Contains bool has[256]
} playerStore;

extern playerStore player; // Global store declaration

// --- What COMPONENT_IMPLEMENT(Player, player) generates in player.c ---
playerStore player = {0}; // Allocates contiguous array memory initialized to zero

```

---

#### 2. Full Code Example: Lifecycle & Usage

```c
// 1. Get a free Entity ID (e.g., entity ID = 0)
Entity p_entity = create_entity();

// 2. Setup initial state
Player new_player = {
    .walk_speed = 5.0f,
    .run_speed = 10.0f,
    .locomotion_state = PLAYER_LOCOMOTION_IDLE
};

// 3. Attach component to entity
ADD_COMPONENT(player, p_entity, new_player);

// 4. Query and mutate component state in a system loop
if (HAS_COMPONENT(player, p_entity)) {
    Player *p = GET_COMPONENT(player, p_entity);
    p->locomotion_state = PLAYER_LOCOMOTION_WALKING; // Direct memory update
}

// 5. Detach component
REMOVE_COMPONENT(player, p_entity);
```

---

#### 3. Step-by-Step Under-the-Hood Flow

* **Entity Creation (`create_entity`):** Pops an integer index (e.g., `0`) off `free_list`. The entity is not an object—it is purely the integer `0`.
* **Adding Component (`ADD_COMPONENT`):**
  * Macro expands to: `do { player.data[0] = new_player; player.mask.has[0] = true; } while(0)`
  * Copies `new_player` directly into index `0` of the global array `player.data` and marks index `0` as active in `player.mask.has`.


* **Fetching Data (`GET_COMPONENT`):**
  * Macro expands to: `(&player.data[0])`
  * Yields a direct pointer to index `0` in memory. Modifications occur directly in place without copying data back and forth.


* **Component Mask Check (`HAS_COMPONENT`):**
  * Macro expands to: `(player.mask.has[0])`
  * Performs an $O(1)$ lookup to verify if systems should process entity `0`.


* **Removal (`REMOVE_COMPONENT`):**
  * Macro expands to: `(player.mask.has[0] = false)`
  * Flips the active flag to `false`. Old data remains in memory at index `0`, but systems ignore it because the mask check fails.

---

### 📊 Memory Layout Diagrams

```text
Entity ID ───────────────> [ 0 ]    [ 1 ]    [ 2 ]    [ 3 ]     ...    [ 255 ]
                                                        │
player.mask.has   ───────> [ F ]    [ F ]    [ F ]    [ T ]     ...    [  F  ]
player.data       ───────> [ - ]    [ - ]    [ - ]  [{Player}]  ...    [  -  ]
                                                        │
transform.mask.has ──────> [ F ]    [ F ]    [ F ]    [ T ]     ...    [  F  ]
transform.data     ──────> [ - ]    [ - ]    [ - ]  [{Pos:x,y}] ...    [  -  ]
                                                        │
inventory.mask.has ──────> [ F ]    [ F ]    [ F ]    [ T ]     ...    [  F  ]
inventory.data     ──────> [ - ]    [ - ]    [ - ]  [{Items}]   ...    [  -  ]
```

```text
                  ┌─────────────────────────────────────────┐
                  │               playerStore               │
                  └────────────────────┬────────────────────┘
                                       │
            ┌──────────────────────────┴──────────────────────────┐
            ▼                                                     ▼
┌───────────────────────┐                              ┌───────────────────────┐
│     data[256]         │                              │     mask.has[256]     │
├───────────────────────┤                              ├───────────────────────┤
│ [0] = {Player Struct} │ ◄── Index 0 (Entity 0) ───►  │ [0] = false           │
│ [1] = {Player Struct} │ ◄── Index 1 (Entity 1) ───►  │ [1] = false           │
│ [2] = {Player Struct} │ ◄── Index 2 (Entity 2) ───►  │ [2] = false           │
│ [3] = {Player Struct} │ ◄── Index 3 (Entity 3) ───►  │ [3] = true            │
│ ...                   │                              │ ...                   │
│ [255]= {Player Struct}│ ◄── Index 255 (Entity 255)─► │ [255]= false          │
└───────────────────────┘                              └───────────────────────┘
```
