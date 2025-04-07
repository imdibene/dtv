#ifndef PS2GV_PARSER_H
#define PS2GV_PARSER_H
#include <string>
#include <vector>

struct Options {
	std::vector<std::string> input_files;
	std::string output_file = "ptree.svg";
	std::string config_file = "ps2gv.conf";
	bool use_ps_command = true;
};

Options parse_args(int argc, char* argv[]);
#endif // PS2GV_PARSER_H
