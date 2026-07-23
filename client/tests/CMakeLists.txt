# Property-based tests for GameCamera (headless, no GPU required)
# Uses a minimal raylib struct stub instead of the full raylib library.

# Stub include directory (provides raylib.h with just struct definitions)
set(RAYLIB_STUB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/stubs")

# Source files for GameCamera (compiled directly against the stub)
set(CAMERA_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../src/game/camera.cpp")
set(CAMERA_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/../src/game")

# Property Test 1: Camera3D struct validity after init
add_executable(test_camera_property1
    game/test_camera_property1.cpp
    ${CAMERA_SRC}
)
target_include_directories(test_camera_property1 PRIVATE
    ${RAYLIB_STUB_DIR}    # stub raylib.h (must come FIRST to override real raylib)
    ${CAMERA_INCLUDE}     # camera.hpp
)
target_compile_features(test_camera_property1 PRIVATE cxx_std_17)

add_test(NAME camera_property1_struct_validity COMMAND test_camera_property1)

# Property Test 2: Orbit position invariant
add_executable(test_camera_property2
    game/test_camera_property2.cpp
    ${CAMERA_SRC}
)
target_include_directories(test_camera_property2 PRIVATE
    ${RAYLIB_STUB_DIR}    # stub raylib.h (must come FIRST to override real raylib)
    ${CAMERA_INCLUDE}     # camera.hpp
)
target_compile_features(test_camera_property2 PRIVATE cxx_std_17)

add_test(NAME camera_property2_orbit_position_invariant COMMAND test_camera_property2)
