//
// Created by Administrator on 2022/10/29.
//
#include <memory>
#include <iostream>

struct Foo {
    Foo() { std::cout << "Foo::Foo" << std::endl; }

    ~Foo() { std::cout << "Foo::~Foo" << std::endl; }

    void foo() { std::cout << "Foo::foo" << std::endl; }
};

void f(const Foo &) {
    std::cout << "f(const Foo&)" << std::endl;
}

struct A;
struct B;

struct A {
    std::shared_ptr<B> pointer;
    ~A() {
        std::cout << "A 被销毁" << std::endl;
    }
};
struct B {
    std::shared_ptr<A> pointer;
    ~B() {
        std::cout << "B 被销毁" << std::endl;
    }
};

struct C;
struct D;

struct C {
    std::weak_ptr<D> pointer;
    ~C() {
        std::cout << "C 被销毁" << std::endl;
    }
};
struct D {
    std::weak_ptr<C> pointer;
    ~D() {
        std::cout << "D 被销毁" << std::endl;
    }
};

int main() {
    // shared_ptr 共享指针 可以多个指针共用值
    auto pointer = std::make_shared<int>(10);
    auto pointer2 = pointer; // 引用计数+1
    auto pointer3 = pointer; // 引用计数+1
    int *p = pointer.get();  // 这样不会增加引用计数
    std::cout << "pointer.use_count() = " << pointer.use_count() << std::endl;   // 3
    std::cout << "pointer2.use_count() = " << pointer2.use_count() << std::endl; // 3
    std::cout << "pointer3.use_count() = " << pointer3.use_count() << std::endl; // 3

    pointer2.reset();
    std::cout << "reset pointer2:" << std::endl;
    std::cout << "pointer.use_count() = " << pointer.use_count() << std::endl;   // 2
    std::cout << "pointer2.use_count() = "
              << pointer2.use_count() << std::endl;           // pointer2 已 reset; 0
    std::cout << "pointer3.use_count() = " << pointer3.use_count() << std::endl; // 2
    pointer3.reset();
    std::cout << "reset pointer3:" << std::endl;
    std::cout << "pointer.use_count() = " << pointer.use_count() << std::endl;   // 1
    std::cout << "pointer2.use_count() = " << pointer2.use_count() << std::endl; // 0
    std::cout << "pointer3.use_count() = "
              << pointer3.use_count() << std::endl;           // pointer3 已 reset; 0

    // std::unique 独占指针 不能有别的指针共享
    std::unique_ptr<Foo> p1(std::make_unique<Foo>());
    // p1 不空, 输出
    if (p1) p1->foo();
    {
        std::unique_ptr<Foo> p2(std::move(p1));
        // p2 不空, 输出
        f(*p2);
        // p2 不空, 输出
        if(p2) p2->foo();
        // p1 为空, 无输出
        if(p1) p1->foo();
        p1 = std::move(p2);
        // p2 为空, 无输出
        if(p2) p2->foo();
        std::cout << "p2 被销毁" << std::endl;
    }
    // p1 不空, 输出
    if (p1) p1->foo();
    // Foo 的实例会在离开作用域时被销毁

    // std::weak_ptr 弱引用，在循环依赖中摆脱循环使用
    auto a = std::make_shared<A>();
    auto b = std::make_shared<B>();
    a->pointer = b;
    b->pointer = a;

    auto c = std::make_shared<D>();
    auto d = std::make_shared<C>();
    c->pointer = d;
    d->pointer = c;

    return 0;
}