#include <vector>
#include <chrono> // Para medir tempo
#include <random>
#include "FileSystemLinked.hpp"
#include "InodeLinked.hpp"
#include "Inode.hpp"
#include "FileSystem.hpp"
#include <fstream>

struct BenchmarkResult {
    std::vector<int> blockIndex;   // Índice do bloco acessado
    std::vector<double> inodeTimes; // Tempo de acesso (ms) para i-node
    std::vector<double> linkedTimes; // Tempo de acesso (ms) para lista encadeada
};


// Para i-node (acesso direto)
std::string AccessBlockInode(const Inode* file, const std::vector<std::string>& blockStorage, int n) {
    if (n < 0 || n >= (int)file->dataBlocks.size()) return "";
    int index = file->dataBlocks[n];
    return (index >= 0 && index < (int)blockStorage.size()) ? blockStorage[index] : "";
}

// Para lista encadeada (acesso sequencial)
std::string AccessBlockLinked(const InodeLinked* file, const std::vector<DataBlock>& blocks, int n) {
    int current = file->firstBlock;
    int count = 0;
    while (current != -1 && current < (int)blocks.size()) {
        if (count == n) return blocks[current].data;
        current = blocks[current].next;
        count++;
    }
    return "";
}


#include <chrono>

void BenchmarkRandomAccess(
    const Inode* inodeFile, const std::vector<std::string>& inodeBlocks,
    const InodeLinked* linkedFile, const std::vector<DataBlock>& linkedBlocks,
    int numAccesses, BenchmarkResult& result)
{
    std::mt19937 rng(42); // semente fixa para reprodutibilidade
    std::uniform_int_distribution<int> dist(0, inodeFile->dataBlocks.size() - 1);

    for (int i = 0; i < numAccesses; ++i) {
        int blockN = dist(rng);
        result.blockIndex.push_back(blockN);

        // i-node
        auto t1 = std::chrono::high_resolution_clock::now();
        volatile auto data1 = AccessBlockInode(inodeFile, inodeBlocks, blockN);
        auto t2 = std::chrono::high_resolution_clock::now();
        double inodeTime = std::chrono::duration<double, std::micro>(t2 - t1).count();
        result.inodeTimes.push_back(inodeTime);

        // lista encadeada
        t1 = std::chrono::high_resolution_clock::now();
        volatile auto data2 = AccessBlockLinked(linkedFile, linkedBlocks, blockN);
        t2 = std::chrono::high_resolution_clock::now();
        double linkedTime = std::chrono::duration<double, std::micro>(t2 - t1).count();
        result.linkedTimes.push_back(linkedTime);
    }
}

void ExportBenchmarkCSV(const BenchmarkResult& result, const std::string& filename) {
    std::ofstream out(filename);
    out << "BlockIndex,InodeTime_us,LinkedTime_us\n";
    for (size_t i = 0; i < result.blockIndex.size(); ++i) {
        out << result.blockIndex[i] << "," << result.inodeTimes[i] << "," << result.linkedTimes[i] << "\n";
    }
    out.close();
}

int CountBlocksInChain(int startBlockIndex, const std::vector<DataBlock>& linkedBlocks) {
    int count = 0;
    int current = startBlockIndex;
    while (current != -1) {
        count++;
        current = linkedBlocks[current].next;
    }
    return count;
}

