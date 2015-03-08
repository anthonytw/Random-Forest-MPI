project(rf-ut)

include_directories(${CMAKE_SOURCE_DIR}/inc)
file(GLOB sources ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB tests *.cpp)
add_executable(${PROJECT_NAME} ${sources} ${tests})

target_link_libraries(${PROJECT_NAME} mpi cppunit)
