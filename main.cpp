#include <mpi.h>

#include <chrono>
#include <iostream>
#include <random>
#include <vector>

class ArraySummer {
private:
    std::vector<int> numbers;
    int arrayLength;
    int randomSeed;

public:
    explicit ArraySummer(int length, int seed)
        : arrayLength(length), randomSeed(seed) {
        numbers.resize(arrayLength);
    }

    void generateNumbers() {
        std::mt19937 generator(randomSeed);
        std::uniform_int_distribution<> range(0, 99);
        for (int& number : numbers) {
            number = range(generator);
        }
    }

    int getSingleThreadSum(double& timeTaken) {
        auto begin = std::chrono::high_resolution_clock::now();
        int result = 0;
        for (int number : numbers) {
            result += number;
        }
        timeTaken = std::chrono::duration<double>(
                        std::chrono::high_resolution_clock::now() - begin)
                        .count();
        return result;
    }

    const std::vector<int>& getNumbers() const {
        return numbers;
    }

    int getLength() const {
        return arrayLength;
    }
};

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int processId, totalProcesses;
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    MPI_Comm_size(MPI_COMM_WORLD, &totalProcesses);

    // Настройки и создание объекта
    const int arrayLength = 1'000'000;
    ArraySummer summer(arrayLength, 123);

    if (processId == 0) {
        summer.generateNumbers();
    }

    const int chunkSize = arrayLength / totalProcesses;
    std::vector<int> localNumbers(chunkSize);

    double singleThreadTime = 0.0;
    int singleThreadResult  = 0;
    if (processId == 0) {
        singleThreadResult = summer.getSingleThreadSum(singleThreadTime);
    }

    auto multiThreadStart = std::chrono::high_resolution_clock::now();

    MPI_Scatter(
        summer.getNumbers().data(), chunkSize, MPI_INT, localNumbers.data(),
        chunkSize, MPI_INT, 0, MPI_COMM_WORLD);

    int chunkSum = 0;
    for (int number : localNumbers) chunkSum += number;

    int multiThreadResult = 0;
    MPI_Reduce(
        &chunkSum, &multiThreadResult, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    double multiThreadTime =
        std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now() - multiThreadStart)
            .count();

    if (processId == 0) {
        std::cout << "Array length: " << arrayLength << "\n";
        std::cout << "Number of processes: " << totalProcesses << "\n";
        std::cout << "Single-thread sum: " << singleThreadResult
                  << " (Time: " << singleThreadTime << " sec)\n";
        std::cout << "Multi-thread sum:  " << multiThreadResult
                  << " (Time: " << multiThreadTime << " sec)\n";
        std::cout << "Acceleration: " << singleThreadTime / multiThreadTime
                  << "\n";
    }

    MPI_Finalize();
    return 0;
}
