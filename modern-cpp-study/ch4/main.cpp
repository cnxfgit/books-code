//
// Created by Administrator on 2022/10/29.
//
#include <iostream>
#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <array>
#include <algorithm>

void foo(int *arr, int size){

}

auto get_student(int id)
{
    // 返回类型被推断为 std::tuple<double, char, std::string>

    if (id == 0)
        return std::make_tuple(3.8, 'A', "张三");
    if (id == 1)
        return std::make_tuple(2.9, 'C', "李四");
    if (id == 2)
        return std::make_tuple(1.7, 'D', "王五");
    return std::make_tuple(0.0, 'D', "null");
    // 如果只写 0 会出现推断错误, 编译失败
}

#include <variant>
template <size_t n, typename... T>
constexpr std::variant<T...> _tuple_index(const std::tuple<T...>& tpl, size_t i) {
    if constexpr (n >= sizeof...(T))
        throw std::out_of_range("越界.");
    if (i == n)
        return std::variant<T...>{ std::in_place_index<n>, std::get<n>(tpl) };
    return _tuple_index<(n < sizeof...(T)-1 ? n+1 : 0)>(tpl, i);
}
template <typename... T>
constexpr std::variant<T...> tuple_index(const std::tuple<T...>& tpl, size_t i) {
    return _tuple_index<0>(tpl, i);
}
template <typename T0, typename ... Ts>
std::ostream & operator<< (std::ostream & s, std::variant<T0, Ts...> const & v) {
    std::visit([&](auto && x){ s << x;}, v);
    return s;
}

template <typename T>
auto tuple_len(T &tpl) {
    return std::tuple_size<T>::value;
}

int main(){
    // std::array 用来代替原生数组 std::array 对象的大小是固定的，如果容器大小是固定的，那么可以优先考虑使用 std::array 容器。
    // 另外由于 std::vector 是自动扩容的，当存入大量的数据后，并且对容器进行了删除操作， 容器并不会自动归还被删除元素相应的内存，这时候就需要手动运行 shrink_to_fit() 释放这部分内存
    std::vector<int> v;
    std::cout << "size:" << v.size() << std::endl;         // 输出 0
    std::cout << "capacity:" << v.capacity() << std::endl; // 输出 0

    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    std::cout << "size:" << v.size() << std::endl;         // 输出 3
    std::cout << "capacity:" << v.capacity() << std::endl; // 输出 4

    v.push_back(4);
    v.push_back(5);
    std::cout << "size:" << v.size() << std::endl;         // 输出 5
    std::cout << "capacity:" << v.capacity() << std::endl; // 输出 8

    // 如下可看出容器虽然清空了元素，但是被清空元素的内存并没有归还
    v.clear();
    std::cout << "size:" << v.size() << std::endl;         // 输出 0
    std::cout << "capacity:" << v.capacity() << std::endl; // 输出 8

    // 额外内存可通过 shrink_to_fit() 调用返回给系统
    v.shrink_to_fit();
    std::cout << "size:" << v.size() << std::endl;         // 输出 0
    std::cout << "capacity:" << v.capacity() << std::endl; // 输出 0

    // array 可以有边界检查，可以有常用函数用，可以使用sort等算法库
    std::array<int, 4> arr = {1, 2, 3, 4};
    arr.empty(); // 检查容器是否为空
    arr.size();  // 返回容纳的元素数

    // 迭代器支持
    for (auto &i : arr)
    {
        std::cout << i << " ";
    }
    std::cout << "\n";

    // 用 lambda 表达式排序
    std::sort(arr.begin(), arr.end(), [](int a, int b) {
        return b < a;
    });

    // 数组大小参数必须是常量表达式
    constexpr int len = 4;
    std::array<int, len> arr1 = {1, 2, 3, 4};

    // 非法,不同于 C 风格数组，std::array 不会自动退化成 T*
    // int *arr_p = arr;

    // c风格传参
    // foo(arr, arr.size()); // 非法, 无法隐式转换
    foo(&arr[0], arr.size());
    foo(arr.data(), arr.size());

    // 无序容器
    // 两组结构按同样的顺序初始化
    std::unordered_map<int, std::string> uMap = {
            {1, "1"},
            {3, "3"},
            {2, "2"}
    };
    std::map<int, std::string> map = {
            {1, "1"},
            {3, "3"},
            {2, "2"}
    };

    // 分别对两组结构进行遍历
    std::cout << "std::unordered_map" << std::endl;
    for( const auto & n : uMap)
        std::cout << "Key:[" << n.first << "] Value:[" << n.second << "]\n";

    std::cout << std::endl;
    std::cout << "std::map" << std::endl;
    for( const auto & n : map)
        std::cout << "Key:[" << n.first << "] Value:[" << n.second << "]\n";

    // 元组
    auto student = get_student(0);
    std::cout << "ID: 0, "
              << "GPA: " << std::get<0>(student) << ", "
              << "成绩: " << std::get<1>(student) << ", "
              << "姓名: " << std::get<2>(student) << '\n';

    double gpa;
    char grade;
    std::string name;

    // 元组进行拆包
    std::tie(gpa, grade, name) = get_student(1);
    std::cout << "ID: 1, "
              << "GPA: " << gpa << ", "
              << "成绩: " << grade << ", "
              << "姓名: " << name << '\n';

    // c++14 使用类型来拆包
    std::tuple<std::string, double, double, int> t("123", 4.5, 6.7, 8);
    std::cout << std::get<std::string>(t) << std::endl;
    // std::cout << std::get<double>(t) << std::endl;  非法, 引发编译期错误
    std::cout << std::get<3>(t) << std::endl;

    // 元组运行期索引
    int index = 1;
    // std::get<index>(t); 只能使用编译器常量
    int i = 1;
    std::cout << tuple_index(t, i) << std::endl;

    // 合并元组
    auto new_tuple = std::tuple_cat(get_student(1), std::move(t));

    // 迭代元组
    for(int idx = 0; idx != tuple_len(new_tuple); ++idx)
        // 运行期索引
        std::cout << tuple_index(new_tuple, idx) << std::endl;

    return 0;
}