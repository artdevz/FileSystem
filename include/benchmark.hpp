#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <iostream>
#include <functional>
#include <memory>
#include <fstream>
#include <FileSystem.hpp>  // Incluindo a classe FileSystem (supondo que você tenha ela)
#include <random>

struct Inode;
struct InodeLinked;
struct DataBlock;

struct BenchmarkResult {
    std::vector<int> blockIndex;      // Índice do bloco acessado
    std::vector<double> inodeTimes;   // Tempo de acesso (us) para i-node
    std::vector<double> linkedTimes;  // Tempo de acesso (us) para lista encadeada
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


// Exemplo de como medir o tempo de acesso a blocos aleatórios para i-node e lista encadeada
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

int CountBlocksInChain(int startBlockIndex, const std::vector<DataBlock>& linkedBlocks) {
    int count = 0;
    int current = startBlockIndex;
    while (current != -1) {
        count++;
        current = linkedBlocks[current].next;
    }
    return count;
}

// Função de benchmark para medir tempo de execução para funções que retornam algo
template <typename F, typename... Args>
auto benchmark(F&& f, Args&&... args) -> std::enable_if_t<!std::is_void_v<std::invoke_result_t<F, Args...>>, double> {
    auto start = std::chrono::high_resolution_clock::now();
    auto result = std::forward<F>(f)(std::forward<Args>(args)...);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    return duration.count(); // Tempo em segundos
}

// Função de benchmark para medir tempo de execução para funções void
template <typename F, typename... Args>
auto benchmark(F&& f, Args&&... args) -> std::enable_if_t<std::is_void_v<std::invoke_result_t<F, Args...>>, double> {
    auto start = std::chrono::high_resolution_clock::now();
    std::forward<F>(f)(std::forward<Args>(args)...);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    return duration.count(); // Tempo em segundos
}

// Função para benchmark de leitura de arquivo
double benchmark_read(FileSystem* fs, const std::string& path) {
    return benchmark([&](std::string p) { fs->Cat(p); }, path);
}

// Função para benchmark de escrita de arquivo
double benchmark_write(FileSystem* fs, const std::string& path, const std::string& content, bool overwrite) {
    return benchmark([&](std::string p, std::string c, bool o) { fs->Echo(p, c, o); }, path, content, overwrite);
}

// Função para benchmark de remoção de arquivo
double benchmark_remove(FileSystem* fs, const std::string& path) {
    return benchmark([&](std::string p) { fs->Rm(p, false); }, path);
}

// Função para benchmark de navegação para diretório (cd)
double benchmark_cd(FileSystem* fs, const std::string& path) {
    auto start = std::chrono::high_resolution_clock::now();
    fs->Cd(path);  // Supondo que o método Cd altere o diretório atual
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    return duration.count(); // Retorna o tempo em segundos
}

// Função para benchmark de movimentação de arquivo (mv)
double benchmark_move(FileSystem* fs, const std::string& source, const std::string& destiny) {
    auto start = std::chrono::high_resolution_clock::now();
    fs->Mv(source, destiny);  // Supondo que o método Mv mova o arquivo de um diretório para outro
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    return duration.count(); // Retorna o tempo em segundos
}

