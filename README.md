# Overview

Trippin on Tubs is an endless runner side-scrolling platformer.

The game is build upon the [Simple DirectMedia Layer](https://www.libsdl.org/) library,
that provides graphics and media facilities. 

Game art is developed in Inkscape and rasterized using Inkscape CLI invoked from
custom Python tools. Sprite sheets are assembled using the [Pillow](https://pillow.readthedocs.io/en/stable/) image processing
library also via custom Python tools.

# Dependencies

Building:
* C++17+
* CMake
* SDL2 library
* SDL2_image library
* Nlohmann JSON library

Testing:
* Catch2 library
* SDL2_tff library

Assets:
* Inkscape
* Python 3.7+
* Pillow image processing Python library

# Building

The CMake build system generator is used for this project.

SDL2 installations include a CMake config file for use with `find_package`. 
Similarly, installations of the Catch2 testing library include a CMake config file.
Ensure these config file are locatable by `find_package`.

SDL2_image, SDL2_tff, and nlohmann JSON are referenced by CMake options
that must be supplied explicitly:

* SDL2_IMAGE_INCLUDE_DIR
* SDL2_IMAGE_LIB
* SDL2_TTF_INCLUDE_DIR
* SDL2_TTF_LIB
* NLOHMANN_JSON_INCLUDE_DIR

# Timing

Two independent timers drive game progress. First, the render rate is the
frequency with which game assets are rendered. The render rate is
synchronized with the display refresh rate, typically 60 Hz. Second, the engine
tick rate is the frequency with which the physical properties of game objects
are updated, typically 100 Hz. The frame rendering thread and the physics engine 
thread communicate via a shared channel. Object position data is communicated from
the engine thread to the rendering thread. User input is communicated in the opposite
direction.

![Frame rate and engine tick rate](docs/game-rate.png)

![Thread communication](docs/thread-communication.png)

# Engine
The `trippin` physics engine handles the movement and interaction of all objects.
The footprint of an object is represented with an axis aligned bounding box (AABB)
that consists of top-left corner floating point numbers (x, y) and 
side length positive integers (w, h).  

## Engine Ticks

The engine operates on discrete steps called ticks. At each tick, the engine
performs six tasks:
1. Position updates
2. Grid snapping
3. Intersection detection 
4. Object snapping 
5. Collision detection
6. Collision reaction

### Update Position
During the update stage of an engine tick, motion-related updates are applied 
to each object. This includes acceleration, velocity, gravity, and friction. Updates
to these quantities are _discrete_, not continuous. Objects teleport from one
location to the next during each tick. No effort is made to examine the space
between consecutive positions for interactions.

![Image of position updates](docs/update.png)


## Grid Snapping

Object AABB intersections are calculated using discrete integer values.
Object AABBs are _snapped_ to the coordinate grid prior to intersection checks.
Snapping is accomplished by rounding the position floating point numbers (x, y).
This is done only for the purpose of intersection detection and object snapping - 
objects retain their _true_ floating positions as well.

![Image of object position](docs/position.png)

### Intersection Detection

After object positions have been updated, some AABBs may intersect. 
Intersections are calculated based upon grid-snapped AABBs.

![Image of position updates](docs/intersection.png)

### Object Snapping 
The engine performs an object snapping procedure to separate intersecting objects such 
that the AABBs are touching but not intersecting.

![Image of position updates](docs/snap.png)

When two intersecting objects are snapped, one of the objects acts as a platform and is 
immovable while the other moves off of the overlapping area. The object that 
moves is translated on the axis of least displacement. In the example above, `A`
is a platform and `B` is a movable object. It moves along the x axis
since the overlap is only 1 unit in that direction.

Object snapping proceeds in priority order. First, objects are snapped to actual
platforms. Then, objects are snapped to objects touching platforms. Then objects
are snapped to those objects and so on. The effect is a breadth-first application
of snapping that radiates away from platforms.

![Image of position updates](docs/reaction.png) 

#### Unresolvable Intersections
The simple object snapping procedure isn't always able to remove all intersections. 

In the example below, `A` is a platform and it has the highest priority. 
It is processed first. Snapping cascades out to `B`, `C`, `D`, and `E`.
Finally, `E` overlaps with `A`. Since `A` is an immovable platform, it
cannot snap to `E`, leaving the conflict unresolved.

![Image of position updates](docs/cycle.png)

Unresolvable intersections can also arise from snap reactions that cascade into each
other. In the example below, `A` and `D` are platforms. Consequently, `B` snaps to `A`
and `C` snaps to `D`. That leaves a new intersection between `B` and `C`. If `B` snaps to
`C` an intersection with platform `A` would result.

![Image of position updates](docs/unresolvable.png)

### Collision Detection

Once intersection detection and object snapping are done, collision detection is 
quite straightforward. A collision occurs when two grid-snapped AABBs share a portion
of their perimeter. In other words, when they are touching.

![Image of position updates](docs/collision.png)

There are several special cases in which touching objects are not considered to be
colliding.

One such case is an irrational collision. An irrational collision is a collision
that is attributed entirely to object snapping rather than object motion. In the
example below, `A` and `B` are moving south but a collision occurs on the x-axis
due to object snapping.

![Image of position updates](docs/irrational.png)

Another case is a redundant collision. A redundant collision occurs when the reaction
to an initial collision doesn't create enough distance to avoid a second collision
in the subsequence tick after grid snapping. The example below demonstrates that
problem.

![Image of position updates](docs/redundant.png)  

### Collision Reaction

Collision reactions can be divided into two categories:
1. Object-platform
2. Object-object

Object-platform collision reactions can be further subdivided:
1. Absorbant
2. Reflective

Object-object collisions reaction can be further subdivided:
1. 1D elastic
2. 2D elastic
3. 2D inelastic