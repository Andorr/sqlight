cmake_minimum_required(VERSION 3.20)

# Set project name
project(SQLight VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#
#    Create target for source files (SQLIGHT)
#
file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_library(sqlight ${SRC_FILES})
target_include_directories(sqlight PUBLIC include)

#
#   Create target executable with library SQLIGHT
#
add_executable(main main.cpp)
target_link_libraries(main PUBLIC sqlight)


#
# Create a library target for the Catch header-only test framework.
#
add_library(Catch tests/catch_amalgamted.cpp)
target_include_directories(Catch
	INTERFACE
		test/
)

#
#   Create a target for executing the Catch unit tests
#
file(GLOB_RECURSE TEST_FILES tests/*.cpp)
add_executable(tests ${TEST_FILES})
target_include_directories(tests PUBLIC test)
target_link_libraries(tests Catch sqlight)

# find_package(Catch2 REQUIRED)
# add_executable(tests tests/main_test.cpp)
# target_link_libraries(tests PRIVATE Catch2::Catch2)

# include(CTest)
# include(Catch)
# catch_discover_tests(foo)