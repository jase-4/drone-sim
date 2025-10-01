cmake_minimum_required(VERSION 3.15)

# Set up vcpkg toolchain if it's defined
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
else()
    message(FATAL_ERROR "VCPKG_ROOT environment variable not set.")
endif()

project(drone_sim)


add_definitions(-D_WIN32_WINNT=0x0A00)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the external branch_rend library
add_subdirectory(external/branch_rend)


add_definitions(-D_WIN32_WINNT=0x0A00)



# Include the main application source
add_executable(drone_sim main.cpp)


file(GLOB_RECURSE PHYSICS_SOURCES "physics/*.cpp")
file(GLOB_RECURSE PHYSICS_HEADERS "physics/*.hpp")

file(GLOB_RECURSE DRONE_SOURCES "drone/*.cpp")
file(GLOB_RECURSE DRONE_HEADERS "drone/*.hpp" "drone/*.h")


# include_directories(
#     ${CMAKE_SOURCE_DIR}/physics
#      ${CMAKE_SOURCE_DIR}/drone
#     ${CMAKE_SOURCE_DIR}/drone/sensor
# )
target_include_directories(drone_sim PRIVATE
    ${CMAKE_SOURCE_DIR}/physics
    ${CMAKE_SOURCE_DIR}/drone
    ${CMAKE_SOURCE_DIR}/drone/sensor
)


target_sources(drone_sim PRIVATE
    ${PHYSICS_SOURCES} ${PHYSICS_HEADERS}
    ${DRONE_SOURCES} ${DRONE_HEADERS}
)


# set(SHADERS_SRC_DIR ${CMAKE_SOURCE_DIR}/shaders)
# set(SHADERS_DST_DIR ${CMAKE_BINARY_DIR}/shaders)

# add_custom_command(
#     TARGET copy_shaders
#     COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADERS_DST_DIR} # Ensure the destination exists
#     COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHADERS_SRC_DIR} ${SHADERS_DST_DIR} # Copy the directory
#     COMMENT "Copying shaders files from ${SHADERS_SRC_DIR} to ${SHADERS_DST_DIR}"
# )


find_package(glad CONFIG REQUIRED)
target_link_libraries(drone_sim PRIVATE glad::glad)

# Link the branch_rend library to the main application

find_package(Bullet CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread)

include_directories(${Boost_INCLUDE_DIRS})


target_link_libraries(drone_sim PRIVATE
    BulletDynamics
    BulletCollision
    LinearMath
)

target_link_libraries(drone_sim PRIVATE branch_rend ${BULLET_LIBRARIES} ${Boost_LIBRARIES} )

# Set additional compiler flags if needed (optional)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
