# ─── 1. 编译器与选项 ─────────────────────────────
CXX       := g++
CXXFLAGS  := -std=c++17 -Wall -Wextra -g -DUNIT_TEST
LDFLAGS   := -pthread

# ─── 2. 包含目录 ───────────────────────────────
#   - ../src    : 库代码头文件
#   - .         : test 目录头文件（如 fifo_test.h）
#   - mock_stm32: mock 头
INC_FLAGS := -I. -I./lib/headers/ -Imock_stm32/ -I../inc/ 

# ─── 3. 源文件列表 ─────────────────────────────
# （a）父目录 src 下的库代码
SRC_CODE_DIR := ../src
SRC_FILES    := $(shell find $(SRC_CODE_DIR) -type f \( -name "*.c" -o -name "*.cpp" \))
# 把 "../src/xxx.c" 形式，转换成 "src/xxx.c" 便于后面映射
SRC_REL      := $(patsubst ../%,%,$(SRC_FILES))

# （b）当前 test 目录下的测试用例
TEST_FILES   := $(wildcard *.cpp)

# （c）当前 test/mock_stm32 目录下的 mock 实现
MOCK_FILES   := $(wildcard mock_stm32/*.c) $(wildcard mock_stm32/*.cpp)

# 合并所有相对路径
ALL_REL      := $(SRC_REL) $(TEST_FILES) $(MOCK_FILES)

# ─── 4. 生成对象文件路径 ───────────────────────
BUILD_DIR := build
# .cpp/.c -> .o，并加上 build/ 前缀
OBJ_FILES := $(patsubst %,$(BUILD_DIR)/%,$(ALL_REL:.cpp=.o))
OBJ_FILES := $(OBJ_FILES:.c=.o)

# ─── 5. GTest 静态库 ──────────────────────────
LIB_DIR := ./lib
LIBS    := \
    $(LIB_DIR)/libgtest.a \
    $(LIB_DIR)/libgmock.a \
    $(LIB_DIR)/libgtest_main.a \
    $(LIB_DIR)/libgmock_main.a

# 最终可执行文件
TARGET := fifoTest

# ─── 默认目标 ─────────────────────────────────
all: $(TARGET)

# ─── 链接 ─────────────────────────────────────
$(TARGET): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(OBJ_FILES) $(LIBS) $(LDFLAGS) -o $@

# ─── 编译规则 ─────────────────────────────────
# 1) 库代码：../src/xxx.c 或 ../src/xxx.cpp
$(BUILD_DIR)/src/%.o: ../src/%.c
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -c $< -o $@

$(BUILD_DIR)/src/%.o: ../src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -c $< -o $@

# 2) 测试用例：test/xxx.cpp
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -c $< -o $@

# 3) Mock 实现：test/mock_stm32/xxx.c/.cpp
$(BUILD_DIR)/mock_stm32/%.o: mock_stm32/%.c
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -c $< -o $@

$(BUILD_DIR)/mock_stm32/%.o: mock_stm32/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -c $< -o $@

# ─── 清理 ─────────────────────────────────────
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean
