cmake_minimum_required(VERSION 3.0.0)
project(ldpc VERSION 2.1)

# 指定 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) 

# 增加库
# add_subdirectory(encoder)
# 选择是否添加 译码器库
option(USE_ENCODER "Use tutorial provided math implementation" ON)

# 配置文件
# 必须放在 option 之后
configure_file(configure.h.in configure.h)

if (USE_ENCODER)
    add_subdirectory(encoder)
    list(APPEND EXTRA_LIBS encoder)
    # 指定 INTERFACE 之后不需要：
    #   list(APPEND EXTRA_INCLUDES "${PROJECT_SOURCE_DIR}/encoder")
endif()

add_subdirectory(decoder)
list(APPEND EXTRA_LIBS decoder)

add_executable(ldpc main.cpp)

## 指定链接库
target_link_libraries(ldpc PUBLIC ${EXTRA_LIBS})

# 指定 ${PROJECT_BINARY_DIR} 即为 build 目录
# 指定 INTERFACE 之后不需要再添加 includes，即：${EXTRA_INCLUDES}
target_include_directories(ldpc PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )

# gfsd

# install(TARGETS ldpc DESTINATION bin)
# install(FILES "${PROJECT_BINARY_DIR}/configure.h"
#   DESTINATION include
#   )
