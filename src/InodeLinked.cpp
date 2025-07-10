#include "InodeLinked.hpp"
#include <iostream>

InodeLinked::InodeLinked(std::string name, Type type)
    : name(name), type(type), size(0), firstBlock(-1), parent(nullptr) {}

std::string InodeLinked::Read(const std::vector<DataBlock>& blocks) const {
    std::string result;
    int current = firstBlock;

    while (current != -1 && current < static_cast<int>(blocks.size())) {
        result += blocks[current].data;
        current = blocks[current].next;
    }

    return result;
}

void InodeLinked::Write(const std::string& data, std::vector<DataBlock>& blocks, bool overwrite) {
    const int blockSize = 8;

    if (overwrite && firstBlock != -1) {
        int current = firstBlock;
        while (current != -1) {
            if (current < 0 || current >= static_cast<int>(blocks.size())) {
                std::cerr << "[Erro] Bloco inválido ao limpar: " << current << "\n";
                break;
            }
            int next = blocks[current].next;
            blocks[current].data.clear();  // limpa corretamente
            blocks[current].next = -1;
            current = next;
        }
        firstBlock = -1;
        size = 0;
    }

    int prevBlock = -1;
    int i = 0;

    while (i < static_cast<int>(data.size())) {
        std::string chunk = data.substr(i, blockSize);
        i += blockSize;

        int newBlock = -1;

        for (int j = 0; j < static_cast<int>(blocks.size()); ++j) {
            if (blocks[j].data.empty()) {
                newBlock = j;
                break;
            }
        }

        if (newBlock == -1) {
            std::cerr << "[Erro] Limite de blocos atingido\n";
            return;
        }

        blocks[newBlock].data = chunk;
        blocks[newBlock].next = -1;

        if (firstBlock == -1) {
            firstBlock = newBlock;
        } else if (prevBlock >= 0 && prevBlock < static_cast<int>(blocks.size())) {
            blocks[prevBlock].next = newBlock;
        }

        prevBlock = newBlock;
        size += chunk.size();
    }
}

bool InodeLinked::IsDirectory() const {
    return type == Type::DIRECTORY;
}
