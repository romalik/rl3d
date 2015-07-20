  
cmake_minimum_required(VERSION 2.8)
add_executable( rceng rceng.cpp )
FIND_PACKAGE(OpenCV REQUIRED)                                                                            
TARGET_LINK_LIBRARIES( rceng ${OpenCV_LIBS})  
