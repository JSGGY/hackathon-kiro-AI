add_library(net STATIC
    net.cpp
    messages.cpp
)

target_include_directories(net PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(net PUBLIC
    utils
)

target_compile_options(net PRIVATE -Wall -Wextra)
