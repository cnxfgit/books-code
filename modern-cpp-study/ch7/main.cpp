#include <thread>
#include <iostream>
#include <mutex>
#include <future>
#include <queue>
#include <chrono>
#include <condition_variable>


int v= 0;

void critical_section(int change_v) {
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);

    // 执行竞争操作
    for (int i = 0; i < 5000; ++i) {
        v++;
    }

    // 离开此作用域后 mtx 会被释放
}

int v1= 0;

void critical_section1(int change_v) {
    static std::mutex mtx;
    std::unique_lock<std::mutex> lock(mtx);
    // 执行竞争操作
    v1 = change_v;
    std::cout << v1 << std::endl;
    // 将锁进行释放
    lock.unlock();

    // 在此期间，任何人都可以抢夺 v 的持有权

    // 开始另一组竞争操作，再次加锁
    lock.lock();
    v1 += 1;
    std::cout << v1 << std::endl;
}

std::atomic<int> count = {0};

struct A {
    float x;
    int y;
    long long z;
};

void memoryOrder(){
    std::atomic<int> counter = {0};
    std::vector<std::thread> vt;
    for (int i = 0; i < 100; ++i) {
        vt.emplace_back([&](){
            counter.fetch_add(1, std::memory_order_relaxed);
        });
    }

    for (auto& t : vt) {
        t.join();
    }
    std::cout << "current counter:" << counter << std::endl;
}

void freeCustomer(){
    std::atomic<int*> ptr(nullptr);
    int v3;
    std::thread producer([&]() {
        int* p = new int(42);
        v3 = 1024;
        ptr.store(p, std::memory_order_release);
    });
    std::thread consumer([&]() {
        int* p;
        while(!(p = ptr.load(std::memory_order_consume)));

        std::cout << "p: " << *p << std::endl;
        std::cout << "v3: " << v3 << std::endl;
    });
    producer.join();
    consumer.join();
}

void freeGet(){
    std::vector<int> v4;
    std::atomic<int> flag = {0};
    std::thread release([&]() {
        v4.push_back(42);
        flag.store(1, std::memory_order_release);
    });
    std::thread acqrel([&]() {
        int expected = 1; // must before compare_exchange_strong
        while(!flag.compare_exchange_strong(expected, 2, std::memory_order_acq_rel))
            expected = 1; // must after compare_exchange_strong
        // flag has changed to 2
    });
    std::thread acquire([&]() {
        while(flag.load(std::memory_order_acquire) < 2);

        std::cout << v4.at(0) << std::endl; // must be 42
    });
    release.join();
    acqrel.join();
    acquire.join();
}

void consistentOrder(){
    std::atomic<int> counter = {0};
    std::vector<std::thread> vt;
    for (int i = 0; i < 100; ++i) {
        vt.emplace_back([&](){
            counter.fetch_add(1, std::memory_order_seq_cst);
        });
    }

    for (auto& t : vt) {
        t.join();
    }
    std::cout << "current counter:" << counter << std::endl;
}

int main(){
    // lambda 函数线程
    std::thread t([](){
        std::cout << "hello world." << std::endl;
    });
    t.join();

    // RAII 自动放锁 无论 critical_section() 正常返回、还是在中途抛出异常，都会引发堆栈回退，也就自动调用了 unlock()。
    std::thread t1(critical_section, 2), t2(critical_section, 3);
    t1.join();
    t2.join();

    std::cout << v << std::endl;

    // unique_lock
    std::thread t3(critical_section1, 4), t4(critical_section1, 5);
    t3.join();
    t4.join();

    // std::future
    std::packaged_task<int()> task([](){return 7;});
    // 获得 task 的期物
    std::future<int> result = task.get_future(); // 在一个线程中执行 task
    std::thread(std::move(task)).detach();
    std::cout << "waiting...";
    result.wait(); // 在此设置屏障，阻塞到期物的完成
    // 输出执行结果
    std::cout << "done!" << std:: endl << "future result is "
              << result.get() << std::endl;

    // std::condition_variable
    std::queue<int> produced_nums;
    std::mutex mtx;
    std::condition_variable cv;
    bool notified = false;  // 通知信号

    // 生产者
    auto producer = [&]() {
        for (int i = 0;i < 10 ; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(900));
            std::unique_lock<std::mutex> lock(mtx);
            std::cout << "producing " << i << std::endl;
            produced_nums.push(i);
            notified = true;
            cv.notify_all(); // 此处也可以使用 notify_one
        }
    };
    // 消费者
    auto consumer = [&]() {
        int done = 0;
        while (true) {
            std::unique_lock<std::mutex> lock(mtx);
            while (!notified) {  // 避免虚假唤醒
                cv.wait(lock);
            }
            // 短暂取消锁，使得生产者有机会在消费者消费空前继续生产
            lock.unlock();
            // 消费者慢于生产者
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            lock.lock();
            while (!produced_nums.empty()) {
                std::cout << "consuming " << produced_nums.front() << std::endl;
                produced_nums.pop();
            }
            notified = false;
            done++;
            if (done == 5) break;
        }
    };

    // 分别在不同的线程中运行
    std::thread p(producer);
    std::thread cs[2];
    for (auto & c : cs) {
        c = std::thread(consumer);
    }
    p.join();
    for (auto & c : cs) {
        c.join();
    }


    // 原子操作
    int a = 0;
    int flag = 0;

    std::thread t5([&]() {
        while (flag != 1);

        int b = a;
        std::cout << "b = " << b << std::endl;
    });

    std::thread t6([&]() {
        a = 5;
        flag = 1;
    });

    t5.join();
    t6.join();


    std::thread t7([](){
        count.fetch_add(1);
    });
    std::thread t8([](){
        count++;        // 等价于 fetch_add
        count += 1;     // 等价于 fetch_add
    });
    t7.join();
    t8.join();
    std::cout << count << std::endl;

    // 内存顺序
    // 宽松模型
    memoryOrder();
    // 释放消费模型
    freeCustomer();
    // 释放获取模型
    freeGet();
    // 顺序一致性
    consistentOrder();

    return 0;
}