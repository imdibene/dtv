#include "ps2gv/graph-generator.h"
#include <algorithm>
#include <graphviz/gvc.h>
#include <iomanip>
#include <iostream>
#include <sstream>

std::string generate_graph(const std::vector<ProcessInfo>& procs, const Config& config)
{
	std::stringstream dot_stream;
	dot_stream << "digraph ptree {\n";
	dot_stream << "node [style=filled];\n";

	for (const auto& proc : procs) {
		// skip invalid PIDs
		if (proc.pid.empty() || !std::isdigit(proc.pid[0]))
			continue;

		// get command name
		std::string comm = proc.command;
		size_t last_slash = comm.find_last_of('/');
		if (last_slash != std::string::npos)
			comm = comm.substr(last_slash + 1);

		// select colour based on unit, fallback to comm if unit is "-"
		const std::string ckey = (proc.unit != "-" && !proc.unit.empty()) ? proc.unit : comm;
		const auto& colour_it = config.command_colours.find(ckey);
		const std::string& colour = (colour_it != config.command_colours.end()) ? colour_it->second : config.command_colours.at("default");
		const auto label = (ckey != comm) ? comm + "\n" + proc.unit : ckey;

		// scale node size per configurable value
		float value = 0.0f;
		try {
			value = std::stof(config.scale_mode == ScaleMode::CPU ? proc.pcpu : proc.rss);
		} catch (...) {
		}
		std::string size_text;
		if (value >= (config.scale_mode == ScaleMode::CPU ? config.min_cpu_threshold : config.min_rss_threshold)) {
			float max_value = (config.scale_mode == ScaleMode::CPU ? config.cpu_limit : config.rss_limit);
			if (value > max_value)
				value = max_value;
			float ratio = value / max_value;
			float width = config.base_width + config.width_factor * ratio;
			float height = config.base_height + config.height_factor * ratio;
			std::ostringstream size_stream;
			size_stream << std::fixed << std::setprecision(2);
			size_stream << "width=\"" << width << "\" height=\"" << height << "\"";
			size_text = size_stream.str();
		}

		// tooltip for ps info in node
		std::string tooltip = "PID: " + proc.pid + "\\nPPID: " + proc.ppid + "\\nCPU%: " + proc.pcpu + "\\nRSS: " + proc.rss + " KB" + "\\nCommand: " + comm + "\\nZone: " + proc.zone + "\\nUnit: " + proc.unit;
		// Sanitise for correct DOT syntax
		std::replace(tooltip.begin(), tooltip.end(), '"', '\'');

		// Add edge & node
		dot_stream << "  \"" << proc.ppid << "\" -> \"" << proc.pid << "\";\n";
		dot_stream << "  \"" << proc.pid << "\" ["
			   << "label=\"" << label << "\" "
			   << "fillcolor=\"" << colour << "\" "
			   << size_text
			   << "tooltip=\"" << tooltip << "\""
			   << "];\n";
	}

	dot_stream << "}\n";
	return dot_stream.str();
}

void render_graph(const std::string& dot_graph, const std::string& output_file)
{
	GVC_t* gvc = gvContext();
	Agraph_t* g = agmemread(const_cast<char*>(dot_graph.c_str()));
	if (!g) {
		std::cerr << "Error: Failed to parse DOT graph" << std::endl;
		gvFreeContext(gvc);
		return;
	}

	if (gvLayout(gvc, g, "fdp") != 0) {
		std::cerr << "Error: Failed to layout graph" << std::endl;
		agclose(g);
		gvFreeContext(gvc);
		return;
	}

	if (gvRenderFilename(gvc, g, "svg", output_file.c_str()) != 0)
		std::cerr << "Error: Failed to render graph" << std::endl;
	else
		std::cout << "Successfully rendered graph to " << output_file << std::endl;

	gvFreeLayout(gvc, g);
	agclose(g);
	gvFreeContext(gvc);
}
