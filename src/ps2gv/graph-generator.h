#ifndef PS2GV_GENERATOR_H
#define PS2GV_GENERATOR_H
#include "ps2gv/config-settings.h"
#include "ps2gv/process-capture.h"

std::string generate_graph(const std::vector<ProcessInfo>& procs, const Config& cfg);
void render_graph(const std::string& dot_graph, const std::string& output_path);
#endif // PS2GV_GENERATOR_H
