cmake_minimum_required(VERSION 3.10)
project(chipeu)

add_library(chipeu SHARED
    chipeu.cpp
    wrapperandroid.cpp
)

set(CMAKE_CXX_STANDARD 17)