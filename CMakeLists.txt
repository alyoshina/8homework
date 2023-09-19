cmake_minimum_required(VERSION 3.16.3)

project(08.homework)

add_executable(counter 
    main.cpp
    src/multi_thread_word_counter.cpp
    )

target_include_directories(counter PRIVATE
    include
    src
)

find_package(Threads REQUIRED)

target_link_libraries(
    counter PRIVATE
    Threads::Threads
)
    
set_target_properties(counter PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

