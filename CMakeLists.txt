  
cmake_minimum_required(VERSION 2.8)
FIND_PACKAGE(OpenCV REQUIRED)                                                                            
add_executable( rceng rceng.cpp )
TARGET_LINK_LIBRARIES( rceng ${OpenCV_LIBS})  
add_executable( rcv rcv.cpp )
TARGET_LINK_LIBRARIES( rcv ${OpenCV_LIBS})  
add_executable( editor editor.cpp )
TARGET_LINK_LIBRARIES( editor ${OpenCV_LIBS})  

