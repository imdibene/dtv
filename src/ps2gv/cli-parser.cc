#include "ps2gv/cli-parser.h"
#include <iostream>
#include <unistd.h>

Options parse_args(int argc, char* argv[])
{
	Options options;
	int opt;

	// parse commandline options
	while ((opt = getopt(argc, argv, "c:")) != -1) {
		switch (opt) {
		case 'c':
			options.config_file = optarg;
			break;
		case '?':
			std::cerr << "Usage: " << argv[0] << " [-c config_file] [input_files...]\n";
			exit(EXIT_FAILURE);
		}
	}
	// Assume all other args are input files. No [Bobby Tables](https://xkcd.com/327/) here, so no sanitising.
	if (optind < argc) {
		options.use_ps_command = false;
		while (optind < argc)
			options.input_files.push_back(argv[optind++]);
	}
	return options;
}
