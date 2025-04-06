#include "ps2gv-config.h"
#include <fstream>
#include <iostream>

void Config::load(const std::string& filename)
{
	std::ifstream file(filename);
	if (!file)
		return;

	std::string line;
	while (std::getline(file, line)) {
		// skip empty lines and comments
		if (line.empty() || line[0] == '#')
			continue;

		size_t eq_pos = line.find('=');
		if (eq_pos == std::string::npos)
			continue; // skip malformed lines

		std::string key = line.substr(0, eq_pos);
		std::string value = line.substr(eq_pos + 1);

		// trim whitespace
		auto trim = [](std::string& s) {
			auto start = s.find_first_not_of(" \t");
			if (start == std::string::npos) {
				s.clear();
				return;
			}
			auto end = s.find_last_not_of(" \t");
			s = s.substr(start, end - start + 1);
		};
		trim(key);
		trim(value);

		try {
			if (key == "hide_zones")
				hide_zones = (value == "true");
			else if (key == "min_cpu_threshold")
				min_cpu_threshold = std::stof(value);
			else if (key == "cpu_limit")
				cpu_limit = std::stof(value);
			else if (key == "base_width")
				base_width = std::stof(value);
			else if (key == "width_factor")
				width_factor = std::stof(value);
			else if (key == "base_height")
				base_height = std::stof(value);
			else if (key == "height_factor")
				height_factor = std::stof(value);
			else if (key.rfind("colour.", 0) == 0 && key.length() > 7) {
				std::string comm = key.substr(7);
				comm_colours[comm] = value;
			}
		} catch (const std::exception& e) {
			std::cerr << "Warning: Invalid config value for '" << key << "' - " << e.what() << std::endl;
		}
	}
}
