project(lab6
        LANGUAGES CXX)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

add_lab(6
        LIB_SOURCES game.cpp npc.cpp observer.cpp point.cpp visitor.cpp
        TEST_SOURCES game_test.cpp
)
