cmake_minimum_required(VERSION 4.0)
project(HuffmanDecoder)

set(CMAKE_CXX_STANDARD 20)

add_executable(HuffmanDecoder main.cpp
        TreeNode.h
        utils.cpp
        utils.h
        HuffmanTree.cpp
        HuffmanTree.h)
