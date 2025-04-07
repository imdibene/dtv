// TODO: Take multiple snapshots to construct an animated version
#include "ps2gv/cli-parser.h"
#include "ps2gv/config-settings.h"
#include "ps2gv/graph-generator.h"
#include "ps2gv/process-capture.h"
#include <filesystem>
#include <iostream>

int main(int argc, char* argv[])
{
	try {
		auto options = parse_args(argc, argv);
		Config config;
		if (!options.config_file.empty())
			config.load(options.config_file);

		if (options.use_ps_command) { // handle ps command case
			// step 1: get process info
			auto ps_info = capture_live();

			// step 2: generate DOT graph
			auto dot_graph = generate_graph(ps_info, config);

			// step 3: output results
			render_graph(dot_graph, options.output_file);
		} else { // handle input files case
			for (const auto& input_file : options.input_files) {
				// step 1: get process info
				auto ps_info = parse_ps_snapshot(input_file);

				// step 2: generate DOT graph
				auto dot_graph = generate_graph(ps_info, config);

				// step 3: output results
				std::filesystem::path input_path(input_file);
				auto output_file = input_path.stem().string() + ".svg";
				render_graph(dot_graph, output_file);
			}
		}

	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
