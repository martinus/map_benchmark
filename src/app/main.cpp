#include "MallocHook.h"

#include "bench.h"

#include <fstream>
#include <regex>

void help(std::vector<std::string> const& args) {
	std::cout << "TODO this should print a help" << std::endl;
}

int main(int cargi, char** cargv) {
	if (cargi == 1) {
		BenchRegistry::list();
		exit(1);
	}

	BenchRegistry::run(cargv[1]);
}
