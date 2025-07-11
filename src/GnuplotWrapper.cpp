#include "GnuplotWrapper.hpp"

#include <fstream>
#include <cstdlib>
#include <filesystem>

void GnuplotWrapper::Plot(
    const std::vector<int>& x,               // Índices de blocos
    const std::vector<double>& inodeTimes,   // Tempos de acesso para Inode Indexado
    const std::string& label1,               // Rótulo para Inode Indexado
    const std::vector<double>& linkedTimes,  // Tempos de acesso para Lista Encadeada
    const std::string& label2,               // Rótulo para Lista Encadeada
    const std::string& xlabel,               // Rótulo do eixo X
    const std::string& ylabel,               // Rótulo do eixo Y
    const std::string& title,                // Título do gráfico
    const std::string& fileName,             // Nome do arquivo para salvar o gráfico
    const int scale                          // Escala do eixo Y
) {
    std::filesystem::create_directories("data");
    std::filesystem::create_directories("plots");

    std::string dataFile = "data/" + fileName + "_data.txt";
    std::string scriptFile = "data/" + fileName + "_plot.gp";
    std::string plotFile = "plots/" + fileName;

    // Salva os dados no arquivo
    std::ofstream out(dataFile);
    for (size_t i = 0; i < x.size(); ++i) {
        out << x[i] << " " << inodeTimes[i] << " " << linkedTimes[i] << "\n";
    }
    out.close();

    // Cria script gnuplot
    std::ofstream gp(scriptFile);
    gp << "set terminal pngcairo size 1920,1080 enhanced font 'Arial,18'\n";
    gp << "set output '" << plotFile << "'\n";
    gp << "set title '" << title << "'\n";
    gp << "set xlabel \"" << xlabel << "\"\n";
    gp << "set ylabel \"" << ylabel << "\"\n";
    gp << "set yrange [0:" << std::to_string(scale) << "]\n"; // Ajuste a escala do eixo Y conforme necessário

    // Plota os dados de Inode Indexado e Lista Encadeada
    gp << "plot \\\n";
    gp << "    \"" << dataFile << "\" using 1:2 with points lt rgb 'blue' lw 2 title \"" << label1 << "\", \\\n";
    gp << "    \"" << dataFile << "\" using 1:3 with points lt rgb 'red' lw 2 title \"" << label2 << "\"\n";
    gp.close();

    RunGnuPlot(scriptFile);
    OpenGnuPlot(plotFile);
}

void GnuplotWrapper::RunGnuPlot(const std::string& scriptFile) {
    std::string cmd = "gnuplot " + scriptFile;
    int ret = system(cmd.c_str());
    if (ret != 0) {
        fprintf(stderr, "Erro ao executar gnuplot\n");
    }
}

void GnuplotWrapper::OpenGnuPlot(const std::string& plotFile) {
#ifdef __linux__
    std::string cmd = "xdg-open \"" + plotFile + "\" &";
#elif _WIN32
    std::string cmd = "start " + plotFile;
#else
    std::string cmd = "open " + plotFile; // macOS
#endif
    system(cmd.c_str());
}