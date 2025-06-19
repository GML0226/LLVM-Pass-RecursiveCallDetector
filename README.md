#LLVM指定路径（按照实际llvm19的安装路径修改）
  export LLVM_DIR=/usr/lib/llvm-19
  export PATH=$LLVM_DIR/bin:$PATH

#构建PASS
  mkdir build
  cd build
  cmake ..
  make

#生成IR(测试文件放在test文件夹下，在当前项目根目录下执行)
  clang-19 -emit-llvm -c ./test/test.c -o ./test/test.bc

#运行PASS(在当前项目根目录下执行)
  opt-19 -load-pass-plugin=./build/libRecursiveCallDetector.so -passes=recursive-call-detector ./test/test.bc -o /dev/null
