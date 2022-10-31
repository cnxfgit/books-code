#include <iostream>
#include "foo.h"
#include <functional>

int main() {
    // 字符串字面值的变化
    char *s1 = "这是被启用的特性";
    const char * s2 = "这是现代c++推荐的字符串字面量";
    auto *s3 = "或者使用auto接收";

    // 调用c语言用 extern "C" 语法
    foo();
    [out = std::ref(std::cout << "Result from C code: " << add(1, 2))](){
        out.get() << ".\n";
    }();
    return 0;
}