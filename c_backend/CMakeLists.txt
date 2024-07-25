cmake_minimum_required (VERSION 3.5)
project (ShadowRemover)
file(GLOB_RECURSE SRCS "./src/*.cpp") #递归查找src目录下所有的后缀为c的文件，并保存在SRCS变量中

list(APPEND CMAKE_PREFIX_PATH C:\\Users\\HowXu\\Documents\\source\\opencv-4.10.0\\build\\install\\x64\\mingw\\lib)
# 引入 OpenCV 库
find_package(OpenCV REQUIRED)
# 添加 OpenCV 库头文件搜索路径
include_directories(${OpenCV_INCLUDE_DIRS})

file(GLOB_RECURSE SRCS_H "./src/*.hpp") #递归查找src目录下所有的后缀为hpp的文件，并保存在SRCS_H变量中(这是一种很不规范的写法，但是我这里为了省时，正常应该是使用设置头文件libraries的函数)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "./out") #设置输出路径为build路径
add_executable(output ${SRCS}) #用src下的源文件生成一个叫Hello的可执行文件

target_link_libraries(output ${OpenCV_LIBS} Ws2_32)
