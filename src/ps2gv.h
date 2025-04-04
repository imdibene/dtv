#ifndef PS2GV_H
#define PS2GV_H
#include "ps2gv-config.h"
#include <algorithm>
#include <cctype>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <getopt.h>
#include <graphviz/gvc.h>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

namespace fs = std::filesystem;

struct Options {
	std::vector<std::string> input_files;
	std::string config_file = "ps2gv.conf"; // default: from command invocation path
	bool use_ps_command = true;
};

Options parse_args(int argc, char* argv[]);
std::string generate_graph(const std::string& ps_output, bool is_macos, const Config& config);
std::string get_process_info(const std::string& input_file);
std::string run_ps_command(bool is_macos);
void render_graph(const std::string& dot_graph, const std::string& output_file = "ptree.svg");

#endif // PS2GV_H
