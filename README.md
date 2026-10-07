# LLVM Pass：Recursive Call Detector

一个基于 LLVM 新版 Pass Manager 的模块级 Pass，用于检测 **直接递归** 与 **间接递归**，并输出递归函数及调用路径。

---

## 1. 功能说明

该 Pass 会：

- 构建模块调用图（CallGraph）
- 基于 SCC（强连通分量）识别递归环
- 输出递归函数名到控制台
- 同时写入 `recursive_functions.txt`
- 尝试记录递归调用路径（如 `A -> B -> A`）

---

## 2. 项目结构

```text
.
├── RecursiveCallDetector.cpp   # Pass 实现
├── CMakeLists.txt              # 构建配置（LLVM 19）
├── test.c                      # 示例测试文件
└── README.md
```

---

## 3. 环境要求

- LLVM / Clang / opt：`19.x`
- CMake：`>= 3.20`
- C++17 编译器

> 当前 `CMakeLists.txt` 默认按如下路径查找 LLVM：
>
> `/usr/lib/llvm-19/lib/cmake/llvm`
>
> 若本机路径不同，请先修改 `CMakeLists.txt` 中 `find_package(LLVM ...)` 的 `PATHS`。

---

## 4. 快速开始

以下命令均在**项目根目录**执行。

### 4.1 配置 LLVM 环境

```bash
export LLVM_DIR=/usr/lib/llvm-19
export PATH=$LLVM_DIR/bin:$PATH
```

### 4.2 构建 Pass 插件

```bash
mkdir -p build
cd build
cmake ..
make -j
cd ..
```

构建成功后会生成：

- `build/libRecursiveCallDetector.so`

### 4.3 生成测试 IR（bitcode）

```bash
clang-19 -emit-llvm -c ./test.c -o ./test.bc
```

### 4.4 运行 Pass

```bash
opt-19 -load-pass-plugin=./build/libRecursiveCallDetector.so \
  -passes=recursive-call-detector \
  ./test.bc -o /dev/null
```

---

## 5. 输出说明

运行后可看到两类输出：

1. 控制台输出（`opt` 执行时打印）
2. 文件输出：`recursive_functions.txt`

输出内容示例：

- `Function 'direct_recursive' is recursive`
- `Function 'indirect_recursive_a' is recursive`
- `Function 'indirect_recursive_b' is recursive`
- 以及对应的递归调用路径

---

## 6. 常见问题

### Q1：提示找不到 LLVM 19

- 检查 `LLVM_DIR` 与 `PATH` 是否正确
- 检查 `CMakeLists.txt` 的 LLVM 路径是否和本机一致

### Q2：`opt-19` 无法加载插件

- 确认插件文件存在：`./build/libRecursiveCallDetector.so`
- 确认 `opt-19` 与构建时 LLVM 主版本一致（都为 19）
