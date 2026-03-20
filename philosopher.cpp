#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>


std::mutex table_mutex;    
int seats_available = 2;     
std::mutex cout_mutex;      

void philosopher(int id, std::mutex& left_fork, std::mutex& right_fork) {
    for (int meal = 0; meal < 12; ++meal) {
        
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "Философ " << id + 1 << " размышляет..." << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        {
            std::lock_guard<std::mutex> lock(table_mutex);
            while (seats_available == 0) {
                table_mutex.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                table_mutex.lock();
            }
            seats_available--;
        }


        {
            std::lock_guard<std::mutex> left_lock(left_fork);
            std::lock_guard<std::mutex> right_lock(right_fork);
            
            {
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout << "  >>> Философ " << id + 1 << " ЕСТ (мест: " 
                          << (2 - seats_available) << "/2)" << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        {
            std::lock_guard<std::mutex> lock(table_mutex);
            seats_available++;
        }
    }
}

int main() {
    const int N = 5;
    std::vector<std::mutex> forks(N);
    std::vector<std::thread> philosophers;

    std::cout << "=== Философы (максимум 2 едят одновременно) ===" << std::endl;

    for (int i = 0; i < N; ++i) {
        philosophers.emplace_back(philosopher, 
                                  i, 
                                  std::ref(forks[i]), 
                                  std::ref(forks[(i + 1) % N]));
    }

    for (auto& t : philosophers) {
        t.join();
    }

    std::cout << "Все наелись!" << std::endl;
    return 0;
}