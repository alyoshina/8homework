cmake_minimum_required(VERSION 3.16.3)

project(topk_words)

add_executable(counter 
    topk_words.cpp
)

target_include_directories(counter PRIVATE
    text
)
    
set_target_properties(counter PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

