#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>

constexpr int N = 6;
constexpr int MAX_EATING = N / 2;

std::mutex table_mtx;
std::condition_variable table_cv;
int seats = MAX_EATING;
std::queue<int> wait_q;

std::mutex cout_mtx;
std::vector<std::mutex> forks(N);

void philosopher(int id) {
    std::mutex& left  = forks[id];
    std::mutex& right = forks[(id + 1) % N];

    for (int meal = 0; meal < 12; ++meal) {
        { 
            std::lock_guard<std::mutex> l(cout_mtx); 
            std::cout << "Философ " << id+1 << " думает" << std::endl; 
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Очередь
        {
            std::unique_lock<std::mutex> lock(table_mtx);
            if (seats == 0) {
                wait_q.push(id);
                table_cv.wait(lock, [&] { return seats > 0 && wait_q.front() == id; });
                wait_q.pop();
            }
            seats--;
        }

        // Поедание
        {
            std::scoped_lock flk(left, right);
            std::lock_guard<std::mutex> c_lock(cout_mtx);
            std::cout << "       Философ " << id+1 << " ест, едят: " << (MAX_EATING - seats) << "/" << MAX_EATING << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        // Освобождение
        {
            std::lock_guard<std::mutex> lock(table_mtx);
            seats++;
            table_cv.notify_all(); 
        }
    }
}

int main() {
    std::vector<std::thread> th; 
    th.reserve(N);
    std::cout << "Философы: " << N << ", едят: " << MAX_EATING << " - очередь" << std::endl;
    for (int i = 0; i < N; ++i) th.emplace_back(philosopher, i);
    for (auto& t : th) t.join();
    std::cout << "Все наелись" << std::endl;
    return 0;
}