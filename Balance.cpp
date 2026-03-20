#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <random>
#include <future>
#include <chrono>

void worker(std::vector<int>& balances, std::mutex& mtx, std::promise<void>& prom) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> account_dist(0, balances.size() - 1);
    std::uniform_int_distribution<> op_dist(0, 1);

    for (int i = 0; i < 10000; ++i) {
        int acc_idx = account_dist(gen);
        int change = (op_dist(gen) == 0) ? 1 : -1;

        {
            std::lock_guard<std::mutex> lock(mtx);
            balances[acc_idx] += change;
        }
    }

    prom.set_value();
}

int main() {
    std::vector<int> balances(3, 0);

    std::mutex mtx;

    std::vector<std::thread> threads;
    std::vector<std::promise<void>> promises(10);
    std::vector<std::future<void>> futures;

    futures.reserve(10);

    std::cout << "Запуск " << 10 << " потоков по " << 10000 << " операций..." << std::endl;

    for (int i = 0; i < 10; ++i) {
        futures.push_back(promises[i].get_future());

        threads.emplace_back(worker, 
                             std::ref(balances), 
                             std::ref(mtx), 
                             std::ref(promises[i]));
    }

    for (int i = 0; i < 10; ++i) {
        futures[i].get(); 
    }

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    std::cout << "Счета:" << std::endl;
    int total_sum = 0;
    for (int i = 0; i < 3; ++i) {
        std::cout << "Счет " << (i + 1) << " - " << balances[i] << std::endl;
        total_sum += balances[i];
    }
    std::cout << "Сумма счетов" << total_sum << std::endl;

    return 0;
}