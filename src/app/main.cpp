#include "bench.h"

#include <fstream>
#include <regex>

void help(std::vector<std::string> const& args) {
    std::cout << "TODO this should print a help" << std::endl;
}

int main(int cargi, char** cargv) {
    auto ret = BenchRegistry::run(cargi == 2 ? cargv[1] : "");
    if (ret) {
        BenchRegistry::list();
    }
    exit(ret);
}
