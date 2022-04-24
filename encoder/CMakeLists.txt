add_library(encoder encoder.cpp)

# 添加 interface 之后，使用时的根目录不需要再进行 LIBS 的配置
target_include_directories(encoder
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

# install(TARGETS encoder DESTINATION lib)
# install(FILES encoder.h DESTINATION include)
