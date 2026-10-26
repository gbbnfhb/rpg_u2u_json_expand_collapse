#include <iostream>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::ordered_json;

std::string removeNewlines(const std::string& str) {
	std::string result;
	result.reserve(str.size());
	for (char c : str) {
		if (c != '\n' && c != '\r') {
			result.push_back(c);
		}
	}
	return result;
}

void processJson(json& j, const std::string& mode) {
	if (j.is_object()) {
		for (auto it = j.begin(); it != j.end(); ++it) {
			if (it.key() == "data") {
				if (mode == "expand") {
					if (it.value().is_string()) {
						try {
							it.value() = json::parse(it.value().get<std::string>());
						} catch (...) {}
					}
				} else if (mode == "collapse") {
					if (it.value().is_object() || it.value().is_array()) {
						std::string dumped = it.value().dump(0);
						it.value() = removeNewlines(dumped);
					}
				}
			}
			processJson(it.value(), mode);
		}
	} else if (j.is_array()) {
		for (auto& el : j) {
			processJson(el, mode);
		}
	}
}

int main(int argc, char* argv[]) {
	if (argc < 4) {
		std::cout << "Usage: " << argv[0] << " <input_file> <output_file> <expand/collapse>" << std::endl;
		return 1;
	}

	std::ifstream ifs(argv[1]);
	if (!ifs.is_open()) {
		std::cerr << "Error: Could not open input file." << std::endl;
		return 1;
	}

	json root;
	ifs >> root;
	ifs.close();

	processJson(root, argv[3]);

	std::ofstream ofs(argv[2]);
	// どんな構造であれ、インデント4をつけてダンプする
	ofs << root.dump(4) << std::endl;
	ofs.close();

	std::cout << "Done." << std::endl;
	return 0;
}