#include "MallocHook.h"

#include "bench.h"
#include "map_defines.h"

#include <fstream>
#include <regex>

double median(std::vector<double> v) {
	if (v.empty()) {
		return 0;
	}

	std::sort(v.begin(), v.end());
	size_t const s = v.size();
	return (v[(s - 1) / 2] + v[s / 2]) / 2;
}

std::vector<double> run(std::string const& appname, std::string const& name, std::function<void(Bench&)> fn) {
	std::vector<double> times;
#ifdef ENABLE_MALLOC_HOOK
	Bench bench;
	fn(bench);
	auto const& d = bench.periodicMemoryStats()->data();

	// write JSON results
	std::ofstream fout(name + "_" + appname + ".json");
	fout << "{\"name\": \"" << MapName << "\",\n\"x\": [";
	const char* pre = "";
	for (auto const& p : d.periodic) {
		fout << pre << p.timeSec;
		pre = ", ";
	}
	fout << "],\n\"y\": [";

	pre = "";
	for (auto const& p : d.periodic) {
		fout << pre << p.memByte;
		pre = ", ";
	}
	fout << "],\n";

	fout << "\"events_x\": [";
	pre = "";
	for (auto const& e : d.event) {
		fout << pre << e.timeSec;
		pre = ", ";
	}
	fout << "],\n\"events_y\": [";
	pre = "";
	for (auto const& e : d.event) {
		fout << pre << e.memByte;
		pre = ", ";
	}
	fout << "],\n\"events_text\": [";
	pre = "";
	for (auto const& e : d.event) {
		fout << pre << '"' << e.title << '"';
		pre = ", ";
	}
	fout << "],\n\"peak\": " << d.peakMem << "\n}\n";

	std::cout << bench.str() << " " << MapName << std::endl;
#else
	for (size_t i = 0; i < 10; ++i) {
		Bench bench;
		fn(bench);
		times.push_back(bench.runtimeSeconds());
		std::cout << "\t" << bench.str() << std::endl;
	}
	std::sort(times.begin(), times.end());
#endif

	return times;
}

void list(std::vector<std::string> const& args) {
	for (auto const& nameFn : BenchRegister::nameToFn()) {
		std::cout << nameFn.first << std::endl;
	}
}

void filter(std::string const& appname, std::vector<std::string> const& args) {
	std::regex reFilter(args.at(1));
	std::smatch baseMatch;

	for (auto const& nameFn : BenchRegister::nameToFn()) {
		if (!std::regex_match(nameFn.first, baseMatch, reFilter)) {
			continue;
		}

		run(appname, nameFn.first, nameFn.second);
	}
}

void help(std::vector<std::string> const& args) {
	std::cout << "TODO this should print a help" << std::endl;
}

int main(int cargi, char** cargv) {
	std::vector<std::string> args(cargv + 1, cargv + cargi);

	std::string appname = cargv[0];
	auto const pos = appname.find_last_of('/');
	appname = appname.substr(pos + 1);

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
		filter(appname, args);
		break;

	default:
		help(args);
		break;
	}
}
