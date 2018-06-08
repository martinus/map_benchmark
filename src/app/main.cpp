#include "MallocHook.h"

#include "bench.h"

#include <regex>

double median(std::vector<double> v) {
	if (v.empty()) {
		return 0;
	}

	std::sort(v.begin(), v.end());
	size_t const s = v.size();
	return (v[(s - 1) / 2] + v[s / 2]) / 2;
}

std::vector<double> run(std::string const& name, std::function<void(Bench&)> fn) {
	std::cout << name << std::endl;
	std::vector<double> times;
#ifdef ENABLE_MALLOC_HOOK
	size_t const runs = 1;
#else
	size_t const runs = 10;
#endif

	for (size_t i = 0; i < runs; ++i) {
		Bench bench;
		fn(bench);
		times.push_back(bench.runtimeSeconds());
		std::cout << "\t" << bench.str() << std::endl;
	}

	std::sort(times.begin(), times.end());
	std::cout << '\t' << median(times) << " median" << std::endl;

	return times;
}

void list(std::vector<std::string> const& args) {
	for (auto const& nameFn : BenchRegister::nameToFn()) {
		std::cout << nameFn.first << std::endl;
	}
}

void filter(std::vector<std::string> const& args) {
	std::regex reFilter(args.at(1));
	std::smatch baseMatch;

	for (auto const& nameFn : BenchRegister::nameToFn()) {
		if (!std::regex_match(nameFn.first, baseMatch, reFilter)) {
			continue;
		}

		run(nameFn.first, nameFn.second);
	}
}

void help(std::vector<std::string> const& args) {
	std::cout << "TODO this should print a help" << std::endl;
}

int main(int cargi, char** cargv) {
	std::vector<std::string> args(cargv + 1, cargv + cargi);

	// default to running all benchmarks
	if (args.empty()) {
		args.push_back("filter");
		args.push_back(".*");
	}

	switch (args[0][0]) {
	case 'l':
		list(args);
		break;

	case 'f':
		filter(args);
		break;

	default:
		help(args);
		break;
	}
}
