#include <iostream>
#include <memory>
#include <utility>
#include <functional>


// 值捕获 lambda 在创建时捕获值 而非 调用时
void lambda_value_capture() {
    int value = 1;
    auto copy_value = [value] {
        return value;
    };
    value = 100;
    auto stored_value = copy_value();
    std::cout << "stored_value = " << stored_value << std::endl;
    // 这时, stored_value == 1, 而 value == 100.
    // 因为 copy_value 在创建时就保存了一份 value 的拷贝
}

// 引用捕获 lambda 捕获的是引用 值会发生变化
void lambda_reference_capture() {
    int value = 1;
    auto copy_value = [&value] {
        return value;
    };
    value = 100;
    auto stored_value = copy_value();
    std::cout << "stored_value = " << stored_value << std::endl;
    // 这时, stored_value == 100, value == 100.
    // 因为 copy_value 保存的是引用
}

// c++14不再只能捕获左值,现在可以捕获右值
void lambda_expression_capture() {
    auto important = std::make_unique<int>(1);
    auto add = [v1 = 1, v2 = std::move(important)](int x, int y) -> int {
        return x + y + v1 + (*v2);
    };
    std::cout << add(3, 4) << std::endl;
}

using foo = void(int); // 定义函数类型, using 的使用见上一节中的别名语法
void functional(foo f) { // 参数列表中定义的函数类型 foo 被视为退化后的函数指针类型 foo*
    f(1); // 通过函数指针调用函数
}

int bar(int a, int b, int c) {
    std::cout << a << std::endl;
    return a + b + c;
}

std::vector<int> foo2() {
    std::vector<int> temp = {1, 2, 3, 4};
    return temp;
}


void reference(std::string &str) {
    std::cout << "左值" << std::endl;
}

void reference(std::string &&str) {
    std::cout << "右值" << std::endl;
}


class A {
public:
    int *pointer;
    A():pointer(new int(1)) {
        std::cout << "构造" << pointer << std::endl;
    }
    A(A& a):pointer(new int(*a.pointer)) {
        std::cout << "拷贝" << pointer << std::endl;
    }
    // 无意义的对象拷贝
    A(A&& a):pointer(a.pointer) {
        a.pointer = nullptr;
        std::cout << "移动" << pointer << std::endl;
    }
    ~A(){
        std::cout << "析构" << pointer << std::endl;
        delete pointer;
    }
};
// 防止编译器优化
A return_rvalue(bool test) {
    A a,b;
    if(test) return a; // 等价于 static_cast<A&&>(a);
    else return b;     // 等价于 static_cast<A&&>(b);
}

void reference(int &i) {
    std::cout << "左值" << std::endl;
}

void reference(int &&i) {
    std::cout << "右值" << std::endl;
}


template <typename T>
void pass(T&& v) {
    std::cout << "普通传参:";
    reference(v); // 始终调用 reference(int&)
}

template <typename T>
void pass1(T&& v) {
    std::cout << "              普通传参: ";
    reference(v);
    std::cout << "       std::move 传参: ";
    reference(std::move(v));
    std::cout << "    std::forward 传参: ";
    reference(std::forward<T>(v));
    std::cout << "static_cast<T&&> 传参: ";
    reference(static_cast<T&&>(v));
}

int main() {
    // lambda 表达式
    lambda_value_capture();
    lambda_reference_capture();

    // 隐式捕获
    // [] 空捕获列表
    // [name1, name2, ...] 捕获一系列变量
    // [&] 引用捕获, 让编译器自行推导引用列表
    // [=] 值捕获, 让编译器自行推导值捕获列表

    // 表达式捕获
    lambda_expression_capture();

    // 泛型lambda
    auto add = [](auto x, auto y) {
        return x + y;
    };
    std::cout << add(1, 2) << std::endl;
    std::cout << add(1.1, 2.2) << std::endl;

    // std::function
    auto f = [](int value) {
        std::cout << value << std::endl;
    };
    functional(f); // 传递闭包对象，隐式转换为 foo* 类型的函数指针值
    f(3); // lambda 表达式调用

    auto foo1 = [](int i) -> int {
        return i;
    };

    std::function<int(int)> func = foo1;

    int important = 10;
    std::function<int(int)> func2 = [&](int value) -> int {
        return 1 + value + important;
    };
    important = 20;
    std::cout << func(10) << std::endl;
    std::cout << func2(10) << std::endl;

    // std::bind std::placeholders
    auto barBind = std::bind(bar, std::placeholders::_1, 1, 2);
    barBind(3);

    // 左值 右值 纯右值 将亡值
    // 左值 顾名思义就是赋值符号左边的值。准确来说， 左值是表达式（不一定是赋值表达式）后依然存在的持久对象。
    &"abc";
    const char (&left)[5] = "char";
    int i = 123;
    &i;

    // 右值  右边的值，是指表达式结束后就不再存在的临时对象。
    // 纯右值
    true;
    1;
    false;
    nullptr;
    []() {};
    1 + 2 * 3;
    // 将亡值 foo2中的temp
    foo2();

    // 右值引用 和 左值引用
    // 想要拿到将亡值就必须使用右值引用
    std::string lv1 = "string,"; // lv1 是一个左值
    // std::string&& r1 = lv1; // 非法, 右值引用不能引用左值
    std::string &&rv1 = std::move(lv1); // 合法, std::move可以将左值转移为右值
    std::cout << rv1 << std::endl; // string,

    const std::string &lv2 = lv1 + lv1; // 合法, 常量左值引用能够延长临时变量的生命周期
    // lv2 += "Test"; // 非法, 常量引用无法被修改
    std::cout << lv2 << std::endl; // string,string,

    std::string &&rv2 = lv1 + lv2; // 合法, 右值引用延长临时对象生命周期
    rv2 += "Test"; // 合法, 非常量引用能够修改临时变量
    std::cout << rv2 << std::endl; // string,string,string,Test

    reference(rv2); // 输出左值
    reference("123"); // 输出右值

    // int &a = std::move(1);    // 不合法，非常量左引用无法引用右值
    const int &b = std::move(1); // 合法, 常量左引用允许引用右值
    int &&c = 1;

    //首先会在 return_rvalue 内部构造两个 A 对象，于是获得两个构造函数的输出；
    //函数返回后，产生一个将亡值，被 A 的移动构造（A(A&&)）引用，从而延长生命周期，并将这个右值中的指针拿到，保存到了 obj 中，而将亡值的指针被设置为 nullptr，防止了这块内存区域被销毁。
    A obj = return_rvalue(false);
    std::cout << obj.pointer << std::endl;
    std::cout << *obj.pointer << std::endl;

    // 标准库的移动语义案例
    std::string str = "Hello world.";
    std::vector<std::string> v;
    // 将使用 push_back(const T&), 即产生拷贝行为
    v.push_back(str);
    // 将输出 "str: Hello world."
    std::cout << "str: " << str << std::endl;

    // 将使用 push_back(const T&&), 不会出现拷贝行为
    // 而整个字符串会被移动到 vector 中，所以有时候 std::move 会用来减少拷贝出现的开销
    // 这步操作后, str 中的值会变为空
    v.push_back(std::move(str));
    // 将输出 "str: "
    std::cout << "str: " << str << std::endl;

    // 不完美转发
    pass(1); // 传递右值
    int left1 = 123;
    pass(left1); // 传递左值值

    // 完美转发 保持原来的左右值类型
    pass1(1);
    pass1(left1);

    return 0;
}