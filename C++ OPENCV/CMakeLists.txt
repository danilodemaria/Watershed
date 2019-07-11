cmake_minimum_required(VERSION 2.8)


project( DisplayImage )
find_package( OpenCV REQUIRED )
link_directories(${CMAKE_SOURCE_DIR}/lib)
add_executable( DisplayImage WatershedMain.cpp)

target_link_libraries( DisplayImage ${OpenCV_LIBS})
