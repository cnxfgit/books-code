#include <iostream>
#include <vector>
#include <algorithm>
#include <tuple>

void foo(char *s) {
    std::cout << "type char\n";
}

void foo(int i) {
    std::cout << "type int\n";
}

#define LEN 0

int const_len() {
    int i = 1;
    return i;
}

constexpr int constexpr_len() {
    return 2;
}

constexpr int constexpr_fib(int n) {
    if (n < 2) return n;
    return constexpr_fib(n - 2) + constexpr_fib(n - 1);
}

class Foo {
public:
    int value_a;
    int value_b;

    Foo(int a, int b) : value_a(a), value_b(b) {}
};

class MagicFoo {
public:
    std::vector<int> vec;

    MagicFoo(std::initializer_list<int> list) {
        for (std::initializer_list<int>::iterator it = list.begin();
             it != list.end(); ++it)
            vec.push_back(*it);
    }

    void foo(std::initializer_list<int> list) {
        for (std::initializer_list<int>::iterator it = list.begin();
             it != list.end(); ++it)
            vec.push_back(*it);
    }

};

std::tuple<int, float, char> tuple() {
    return std::make_tuple(1, 1.1, 'a');
}

// c++ 17
template<typename T>
auto print_type_info(const T &t) {
    if constexpr (std::is_integral<T>::value) {
        return t + 1;
    } else {
        return t + 0.001;
    }
}

template
class std::vector<bool>;          // 强行实例化
extern template
class std::vector<double>; // 不在该当前编译文件中实例化模板

// 变长参数模板
template<typename ...Args>
void vararg(Args ...args) {
    int len = sizeof...(args);
    printf("%d\n", len);
}

// 变参模板递归
template<typename T0>
void printf1(T0 value) {
    std::cout << value << std::endl;
}

template<typename T, typename... Ts>
void printf1(T value, Ts... args) {
    std::cout << value << std::endl;
    printf1(args...);
}

// 变参模板展开 c++17
template<typename T0, typename... T>
void printf2(T0 t0, T... t) {
    std::cout << t0 << " ";
    if constexpr (sizeof...(t) > 0) printf2(t...);
}

// 初始化列表展开
template<typename T, typename... Ts>
auto printf3(T value, Ts... args) {
    std::cout << value << std::endl;
    (void) std::initializer_list<T>{([&args] {
        std::cout << args << " ";
    }(), value)...};
    std::cout << std::endl;
}

// 折叠表达式
template<typename  ...T>
auto sum(T ... t) {
    return (t + ...);
}

// 非类型模板参数推导
template<typename T, int BufSize>
class buffer_t {
public:
    T &alloc() {};

    void free(T &item) {};
private:
    T data[BufSize];
};

template <auto value>
void bar() {
    std::cout << value << std::endl;
}

class Base {
public:
    int value1;
    int value2{};
    Base() {
        value1 = 1;
    }
    explicit Base(int value) : Base() { // 委托 Base() 构造函数
        value2 = value;
    }
};

class Subclass : public Base {
public:
    using Base::Base; // 继承构造
};

struct Base1 {
    virtual void foo(int);
};
struct SubClass: Base1 {
    virtual void foo(int) override; // 合法
    // virtual void foo(float) override; 非法, 父类没有此虚函数
};

struct Base2 {
    virtual void foo() final;
};
struct SubClass2 final: Base2 {
}; // 合法

//struct SubClass3 : SubClass2 {};  非法, SubClass1 已 final

struct SubClass4: Base2 {
    // void foo();  非法, foo 已 final
};

// 显式禁用默认函数
class Magic {
public:
    Magic() = default; // 显式声明使用编译器生成的构造
    Magic& operator=(const Magic&) = delete; // 显式声明拒绝编译器生成构造
    Magic(int magic_number);
};

// 强类型枚举
enum class new_enum : unsigned int {
    value1,
    value2,
    value3 = 100,
    value4 = 100
};

int main() {
    // foo(NULL) 不能通过编译 有可能隐式转换成 foo(0)  有些编译环境中NULL 为 ((void*)0)
    foo(nullptr);
    foo(0);
    std::cout << "NULL == 0: " << (NULL == 0) << "\n";

    // 常量表达式
    int arr1[10];   // 合法
    int arr2[LEN];  // 合法

    int len1 = 10;
    int arr3[len1]; // 老版本编译器非法
    const int len2 = len1 + 1;
    int arr4[len2]; // 老版本编译器非法

    constexpr int len3 = 3 * 4;
    int arr5[len3]; // 合法

    int arr6[const_len() + 5];  // 老版本编译器非法
    int arr7[constexpr_len() + 5]; // 合法

    std::cout << constexpr_fib(35) << "\n";

    // 变量声明明强化 可以在if 或者 switch 中声明
    std::vector<int> vec = {1, 2, 3, 4};
    // c++ 17 之前
    const std::vector<int>::iterator iter1 = std::find(vec.begin(), vec.end(), 2);
    if (iter1 != vec.end()) {
        *iter1 = 3;
    }
    const std::vector<int>::iterator iter2 = std::find(vec.begin(), vec.end(), 3);
    if (iter2 != vec.end()) {
        *iter2 = 4;
    }
    // 将输出 1, 4, 3, 4
    for (std::vector<int>::iterator element = vec.begin(); element != vec.end(); ++element)
        std::cout << *element << " ";
    // c++17之后
    if (const std::vector<int>::iterator iter3 = std::find(vec.begin(), vec.end(), 3);
            iter3 != vec.end()) {
        *iter3 = 4;
    }
    // 将输出 1, 4, 4, 4
    for (std::vector<int>::iterator element = vec.begin(); element != vec.end(); ++element)
        std::cout << *element << " ";
    std::cout << "\n";

    // 初始化列表 c++ 11之前
    Foo f(1, 2);
    // c++11 之后
    MagicFoo magicFoo = {1, 2, 3, 4, 5};
    magicFoo.foo({1, 2, 3});

    // 结构化绑定 c++17 之前
    std::tuple tuple1 = tuple();
    int tupleInt = std::get<int>(tuple1);
    float tupleFloat = std::get<float>(tuple1);
    char tupleChar = std::get<char>(tuple1);
    std::cout << tupleInt << tupleFloat << tupleChar << "\n";
    // c++ 17之后
    std::tuple tuple2 = tuple();
    auto[tupleInt1, tupleFloat1, tupleChar1] = tuple2;
    std::cout << tupleInt1 << tupleFloat1 << tupleChar1 << "\n";

    // auto 类型推导
    auto mf = Foo(1, 2);   // 被推导为Foo 类型
    auto arr9 = new int(123);  // 被推导为 int *类型
    auto vb = vec.begin();     // 被推导为 std::vector<int>::iterator

    // decltype 关键字是为了解决 auto 关键字只能对变量进行类型推导的缺陷而出现的
    auto a1 = 12; // 推导为 int
    auto a2 = 22; // 推导为 int
    decltype(a1 + a2) a3 = 66; // 推导为 int

    // constexpr 引入if
    std::cout << print_type_info(5) << std::endl;
    std::cout << print_type_info(3.14) << std::endl;

    // 区间for迭代
    if (auto itr = std::find(vec.begin(), vec.end(), 3); itr != vec.end()) *itr = 4;
    for (auto element : vec)
        std::cout << element << " "; // read only
    for (auto &element : vec) {
        element += 1;                      // writeable
    }
    std::cout << "\n";
    for (auto element : vec)
        std::cout << element << " "; // read only
    std::cout << "\n";

    // 尖括号识别
    // 连续的尖括号 在c++11之前不能被识别
    std::vector<std::vector<int> > matrix;
    // c++11 之后可以
    std::vector<std::vector<int>> matrix1;

    // 变长参数模板
    vararg<int, char, float>(1, 'a', 1.4);

    printf1(1, 2, "123", 1.1);
    printf2<int, int>(0, 1, 2);
    std::cout << "\n";
    printf3('a', 'b', 'c', 'd');
    std::cout << sum(6, 6, 6) << "\n";
    buffer_t<int, 100> buf{}; // 100 作为模板参数
    bar<'a'>();

    // c++11 委托构造 可以在构造函数中调用别的构造函数
    Base b(2);
    std::cout << "v1:" << b.value1 << "  v2:" << b.value2 << "\n";

    // 继承构造
    Subclass s(3);
    std::cout << "v1:" << s.value1 << "  v2:" << s.value2 << "\n";

    return 0;
}

// c++ 11 之前
template<typename R, typename T, typename U>
R add1(T x, U y) {
    return x + y;
}

// c++ 11
template<typename T, typename U>
auto add2(T x, U y) -> decltype(x + y) {
    return x + y;
}

// c++ 14
template<typename T, typename U>
auto add3(T x, U y) {
    return x + y;
}


// 模板别名
template<typename T, typename U>
class MagicType {
public:
    T dark;
    U magic;
};

// 不合法
//template<typename T>
//typedef MagicType<std::vector<T>, std::string> FakeDarkMagic;

// 使用using可以重命名模板
typedef int (*process)(void *);

using NewProcess = int (*)(void *);
template<typename T>
using TrueDarkMagic = MagicType<std::vector<T>, std::string>;

