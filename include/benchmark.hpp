#include <vector>
#include <string>

// Forward declarations (caso não tenha os includes completos aqui)
struct Inode;
struct InodeLinked;
struct DataBlock;

struct BenchmarkResult {
    std::vector<int> blockIndex;      // Índice do bloco acessado
    std::vector<double> inodeTimes;   // Tempo de acesso (us) para i-node
    std::vector<double> linkedTimes;  // Tempo de acesso (us) para lista encadeada
};

// Para i-node (acesso direto)
std::string AccessBlockInode(const Inode* file, const std::vector<std::string>& blockStorage, int n);

// Para lista encadeada (acesso sequencial)
std::string AccessBlockLinked(const InodeLinked* file, const std::vector<DataBlock>& blocks, int n);

void BenchmarkRandomAccess(
    const Inode* inodeFile, const std::vector<std::string>& inodeBlocks,
    const InodeLinked* linkedFile, const std::vector<DataBlock>& linkedBlocks,
    int numAccesses, BenchmarkResult& result);

void ExportBenchmarkCSV(const BenchmarkResult& result, const std::string& filename);

int CountBlocksInChain(int startBlockIndex, const std::vector<DataBlock>& linkedBlocks);
