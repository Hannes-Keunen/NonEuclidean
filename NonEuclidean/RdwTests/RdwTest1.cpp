#include "Corridor.h"
#include "RdwTests.h"
#include "Room.h"
#include "RoomGraph.h"

/**
 * two adjacent rooms with partial overlap, connected by a straight corridor.
 */
RoomGraph GraphTests::test1()
{
    // clang-format off
    return RoomGraph
    {
        .rooms =
        {
            {.type = 0, .w = 6.0f, .h = 6.0f, .portals = {1.0f}},
            {.type = 1, .w = 9.0f, .h = 2.0f, .portals = {19.0f, 12.0f}},
            {.type = 0, .w = 6.0f, .h = 6.0f, .portals = {5.0f}}
        },
        .adjacency =
        {
            {false, true, false},
            {true, false, true},
            {false, true, false}
        }
    };
    // clang-format on
}

/**
 * three rooms arranged in a square; impossible in physical space because
 * rooms 1 and 2 don't align
 */
RoomGraph GraphTests::test2()
{
    // clang-format off
    return RoomGraph
    {
        .rooms =
        {
            {.type = 0, .w = 10.0f, .h = 10.0f, .portals = {25.0f, 15.0f}}, // #0
            {.type = 0, .w = 10.0f, .h = 10.0f, .portals = { 5.0f, 15.0f}}, // #1
            {.type = 0, .w = 10.0f, .h = 10.0f, .portals = {35.0f, 25.0f}}  // #2
        },
        .adjacency =
        {
            {false, true, true},
            {true, false, true},
            {true, true, false}
        }
    };
    // clang-format on
}

/**
 * Two overlapping rooms with multi-segment corridors in between.
 * The overlap is unnoticed when going from first to second, but not from
 * second to first as there is only one corridor segment.
 * Overlap is detected because both rooms can be visible at the same time.
 */
RoomGraph2 GraphTests::test3()
{
    // clang-format off
    return RoomGraph2
    {
        .rooms =
        {
            {.type = 1, .w = 8.0f, .h = 8.0f, .adjacency = {0.0f,  9.0f,  0.0f,  0.0f,  0.0f, 7.0f}}, // #0
            {.type = 1, .w = 2.0f, .h = 2.0f, .adjacency = {7.0f,  0.0f,  3.0f,  0.0f,  0.0f, 0.0f}}, // #1
            {.type = 1, .w = 2.0f, .h = 8.0f, .adjacency = {0.0f, 19.0f,  0.0f, 13.0f,  0.0f, 0.0f}}, // #2
            {.type = 1, .w = 7.0f, .h = 2.0f, .adjacency = {0.0f,  0.0f,  8.0f,  0.0f, 17.0f, 0.0f}}, // #3
            {.type = 1, .w = 8.0f, .h = 9.0f, .adjacency = {0.0f,  0.0f,  0.0f, 16.0f,  0.0f, 9.0f}}, // #4
            {.type = 1, .w = 5.0f, .h = 2.0f, .adjacency = {8.0f,  0.0f,  0.0f,  0.0f, 13.0f, 0.0f}}, // #5
        }
    };
    // clang-format on
}

/**
 *
 */
RoomGraph2 GraphTests::test4()
{
    // clang-format off
    return RoomGraph2
    {
        .rooms =
        {
            {.type = 1, .w =  8.0f, .h = 8.0f, .adjacency = { 0.0f, 14.0f, 10.0f,  0.0f,  0.0f,  0.0f}}, // #0
            {.type = 1, .w =  4.0f, .h = 8.0f, .adjacency = {22.0f,  0.0f,  0.0f, 17.0f,  0.0f,  0.0f}}, // #1
            {.type = 1, .w =  4.0f, .h = 8.0f, .adjacency = {18.0f,  0.0f,  0.0f,  0.0f, 23.0f,  0.0f}}, // #2
            {.type = 1, .w = 12.0f, .h = 6.0f, .adjacency = { 0.0f, 17.0f,  0.0f,  0.0f,  0.0f,  1.0f}}, // #3
            {.type = 1, .w = 12.0f, .h = 6.0f, .adjacency = { 0.0f,  0.0f, 13.0f,  0.0f,  0.0f, 29.0f}}, // #4
            {.type = 1, .w =  8.0f, .h = 8.0f, .adjacency = { 0.0f,  0.0f,  0.0f, 23.0f,  1.0f,  0.0f}}, // #5
        }
    };
    // clang-format on
}

/**
 *
 */
RoomGraph GraphTests::test5()
{
    // clang-format off
    return RoomGraph
    {
        .rooms =
        {
            // central rooms
            {.type = 0, .w =  4.0f, .h = 8.0f, .portals = { 6.0f, 10.0f, 18.0f, 22.0f}}, // #0 central corridor
            {.type = 0, .w =  4.0f, .h = 4.0f, .portals = {14.0f}}, // #1: bottom right
            {.type = 0, .w =  4.0f, .h = 4.0f, .portals = {14.0f, 10.0f}}, // #2: top right
            {.type = 0, .w =  4.0f, .h = 4.0f, .portals = { 6.0f, 10.0f}}, // #3: top left
            {.type = 0, .w =  4.0f, .h = 4.0f, .portals = { 6.0f}}, // #4: bottom left

            // top rooms
            {.type = 0, .w =  8.0f, .h = 2.0f, .portals = { 6.0f, 12.0f}}, // #5: right
            {.type = 0, .w =  8.0f, .h = 2.0f, .portals = { 2.0f, 16.0f}}, // #6: left
            {.type = 0, .w = 12.0f, .h = 2.0f, .portals = {10.0f,  2.0f}}, // #7: top corridor
        },
        .adjacency =
        {
            //#0    #1     #4     #3     #4         #5     #6     #7
            {false, true , true , true , true ,     false, false, false}, // #0
            {false, false, false, false, false,     false, false, false}, // #1
            {false, false, false, false, false,     true , false, false}, // #2
            {false, false, false, false, false,     false, true , false}, // #3
            {false, false, false, false, false,     false, false, false}, // #4

            {false, false, false, false, false,     false, false, true }, // #5
            {false, false, false, false, false,     false, false, true }, // #6
            {false, false, false, false, false,     false, false, false}, // #7
        }
    };
    // clang-format on
}

/**
 * Large environment consisting of two smaller parts which don't overlap with
 * themselves, but do overlap with each other.
 */
RoomGraph2 GraphTests::test6()
{
    // clang-format off
    return RoomGraph2
    {
        .rooms =
        {
            {.type = 1, .w = 4, .h = 4, .adjacency = { 0.0f,  6.0f,  0.0f,  0.0f,  0.0f,    0.0f,  0.0f, 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,   0.0f, 0.0f,  0.0f}},
            {.type = 1, .w = 8, .h = 8, .adjacency = {26.0f,  0.0f, 30.0f,  0.0f, 22.0f,    0.0f,  0.0f, 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,   0.0f, 0.0f,  0.0f}},
            {.type = 1, .w = 8, .h = 8, .adjacency = { 0.0f, 14.0f,  0.0f, 22.0f,  0.0f,    0.0f,  0.0f, 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,   0.0f, 0.0f,  0.0f}},
            {.type = 1, .w = 8, .h = 8, .adjacency = { 0.0f,  0.0f,  6.0f,  0.0f, 14.0f,    0.0f,  0.0f, 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,   0.0f, 0.0f,  0.0f}},
            {.type = 1, .w = 8, .h = 8, .adjacency = { 0.0f,  6.0f,  0.0f, 30.0f,  0.0f,   14.0f,  0.0f, 0.0f,  0.0f,  0.0f,  0.0f, 26.0f,  0.0f,   0.0f, 0.0f,  0.0f}},

            {.type = 1, .w = 3, .h = 2, .adjacency = { 0.0f,  0.0f,  0.0f,  0.0f,  9.0f,    0.0f,  2.0f, 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,   0.0f, 0.0f,  0.0f}},
            {.type = 1, .w = 2, .h = 9, .adjacency = { 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,   12.0f,  0.0f, 1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,   0.0f, 0.0f,  0.0f}},
            {.type = 1, .w = 8, .h = 8, .adjacency = { 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,    0.0f, 18.0f, 0.0f, 30.0f,  0.0f,  0.0f,  0.0f, 26.0f,   0.0f, 0.0f,  0.0f}},
            {.type = 1, .w = 4, .h = 2, .adjacency = { 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,    0.0f,  0.0f, 5.0f,  0.0f, 11.0f,  0.0f,  0.0f,  0.0f,   0.0f, 0.0f,  0.0f}},
            {.type = 1, .w = 8, .h = 8, .adjacency = { 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,    0.0f,  0.0f, 0.0f, 10.0f,  0.0f, 22.0f,  0.0f,  0.0f,   0.0f, 0.0f,  0.0f}},
            {.type = 1, .w = 2, .h = 9, .adjacency = { 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,    0.0f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f, 12.0f,  0.0f,   0.0f, 0.0f,  6.0f}},
            {.type = 1, .w = 7, .h = 2, .adjacency = { 0.0f,  0.0f,  0.0f,  0.0f,  8.0f,    0.0f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,   0.0f, 0.0f,  0.0f}},
            {.type = 1, .w = 3, .h = 2, .adjacency = { 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,    0.0f,  0.0f, 4.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,   9.0f, 0.0f,  0.0f}},

            {.type = 1, .w = 2, .h = 3, .adjacency = { 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,    0.0f,  0.0f, 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  3.0f,   0.0f, 6.0f,  0.0f}},
            {.type = 1, .w = 8, .h = 8, .adjacency = { 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,    0.0f,  0.0f, 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,   4.0f, 0.0f, 28.0f}},
            {.type = 1, .w = 5, .h = 2, .adjacency = { 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,    0.0f,  0.0f, 0.0f,  0.0f,  0.0f, 13.0f,  0.0f,  0.0f,   0.0f, 6.0f,  0.0f}},
        }
    };
    // clang-format on
}
