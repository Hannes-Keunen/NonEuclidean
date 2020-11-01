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
RoomGraph GraphTests::test3()
{
    // clang-format off
    return RoomGraph
    {
        .rooms =
        {
            {.type = 0, .w = 8.0f, .h = 8.0f, .portals = { 9.0f,  7.0f}}, // #0
            {.type = 0, .w = 2.0f, .h = 2.0f, .portals = { 7.0f,  3.0f}}, // #1
            {.type = 0, .w = 2.0f, .h = 8.0f, .portals = {19.0f, 13.0f}}, // #2
            {.type = 0, .w = 7.0f, .h = 2.0f, .portals = { 8.0f, 17.0f}}, // #3
            {.type = 0, .w = 8.0f, .h = 9.0f, .portals = {16.0f,  9.0f}}, // #4
            {.type = 0, .w = 5.0f, .h = 2.0f, .portals = { 8.0f, 13.0f}}, // #5
        },
        .adjacency =
        {
            {false, true , false, false, false, true },
            {false, false, true , false, false, false},
            {false, false, false, true , false, false},
            {false, false, false, false, true , false},
            {false, false, false, false, false, true },
            {false, false, false, false, false, false},
        }
    };
    // clang-format on
}

/**
 *
 */
RoomGraph GraphTests::test4()
{
    // clang-format off
    return RoomGraph
    {
        .rooms =
        {
            {.type = 0, .w =  8.0f, .h = 8.0f, .portals = {14.0f, 10.0f}}, // #0
            {.type = 0, .w =  4.0f, .h = 8.0f, .portals = {22.0f, 17.0f}}, // #1
            {.type = 0, .w =  4.0f, .h = 8.0f, .portals = {18.0f, 23.0f}}, // #2
            {.type = 0, .w = 12.0f, .h = 6.0f, .portals = {17.0f,  1.0f}}, // #3
            {.type = 0, .w = 12.0f, .h = 6.0f, .portals = {13.0f, 29.0f}}, // #4
            {.type = 0, .w =  8.0f, .h = 8.0f, .portals = {23.0f,  1.0f}}, // #5
        },
        .adjacency =
        {
            {false, true , true , false, false, false},
            {false, false, false, true , false, false},
            {false, false, false, false, true , false},
            {false, false, false, false, false, true},
            {false, false, false, false, false, true},
            {false, false, false, false, false, false},
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
