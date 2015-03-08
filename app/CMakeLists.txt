project(rf)

include_directories(${CMAKE_SOURCE_DIR}/inc)
file(GLOB sources ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${sources} main.cpp)

target_link_libraries(${PROJECT_NAME} mpi)
