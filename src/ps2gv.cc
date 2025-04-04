// TODO: Take multiple snapshots to construct an animated version
#include "ps2gv.h"

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

std::string get_process_info(const std::string& input_file)
{
	// Read from file
	std::ifstream file(input_file);
	if (!file)
		throw std::runtime_error("Failed to open input file");
	return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

std::string run_ps_command(bool is_macos)
{
	int pipefd[2];
	if (pipe(pipefd) == -1)
		throw std::runtime_error("pipe failed");

	pid_t pid = fork();
	if (pid == -1)
		throw std::runtime_error("fork failed");

	if (pid == 0) { // Child process
		// Mario Bros. the plumbing
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);

		const char* ps_args[] = {
			"ps",
			"-eo",
			is_macos ? "ppid,pid,rss,pcpu,comm" : "zone,ppid,pid,rss,pcpu,comm",
			nullptr
		};

		execvp("ps", const_cast<char**>(ps_args));
		std::cerr << "execvp failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Parent process
	close(pipefd[1]);
	std::string output;
	char buffer[1024 * 16]; // one page-macos ; 4 pages-linux
	ssize_t bytes_read;
	while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer))) > 0)
		output.append(buffer, bytes_read);
	close(pipefd[0]);
	waitpid(pid, nullptr, 0);
	return output;
}

std::string generate_graph(const std::string& ps_output, bool is_macos, const Config& config)
{
	std::stringstream dot_stream;
	dot_stream << "digraph ptree {\n";
	dot_stream << "node [style=filled];\n";

	std::istringstream iss(ps_output);
	std::string line;

	// Skip header line
	std::getline(iss, line);

	while (std::getline(iss, line)) {
		std::istringstream line_stream(line);
		std::string zone, ppid, pid, rss, pcpu, comm;

		if (is_macos) {
			line_stream >> ppid >> pid >> rss >> pcpu >> comm;
			zone = "-";
		} else {
			line_stream >> zone >> ppid >> pid >> rss >> pcpu >> comm;
		}

		if (pid == "PID" || pid.empty() || !std::isdigit(pid[0]))
			continue;
		//	if (std::stoi(pid) <= 10)
		//		continue;

		// get command name
		size_t last_slash = comm.find_last_of('/');
		if (last_slash != std::string::npos)
			comm = comm.substr(last_slash + 1);
		// map command to colour
		const auto& colour_it = config.comm_colours.find(comm);
		const std::string& colour = (colour_it != config.comm_colours.end()) ? colour_it->second : config.comm_colours.at("default");
		// scale node size per cpu usage
		float cpu = 0.0f;
		try {
			cpu = std::stof(pcpu);
		} catch (...) {
		}
		std::string size_text;
		if (cpu >= config.min_cpu_threshold) {
			if (cpu > config.cpu_limit)
				cpu = config.cpu_limit;
			float ratio = cpu / config.cpu_limit;
			float width = config.base_width + config.width_factor * ratio;
			float height = config.base_height + config.height_factor * ratio;
			std::ostringstream size_stream;
			size_stream << std::fixed << std::setprecision(2);
			size_stream << "width=\"" << width << "\" height=\"" << height << "\"";
			size_text = size_stream.str();
		}
		// tooltip for ps info in node
		std::string tooltip = "PID: " + pid + "\\nPPID: " + ppid + "\\nCPU%: " + pcpu + "\\nRSS: " + rss + " KB" + "\\nCommand: " + comm;
		// Sanitise for correct DOT syntax
		std::replace(tooltip.begin(), tooltip.end(), '"', '\'');
		dot_stream << "  \"" << ppid << "\" -> \"" << pid << "\";\n";
		dot_stream << "  \"" << pid << "\" ["
			   << "label=\"" << comm << "\" "
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

int main(int argc, char* argv[])
{
	Options options = parse_args(argc, argv);
	Config config;
	bool is_macos =
#ifdef __APPLE__
	    true;
#else
	    false;
#endif

	// load config if exists
	if (fs::exists(options.config_file))
		config.load(options.config_file);

	if (options.use_ps_command) { // handle ps command case
		// step 1: get process info
		std::string ps_output = run_ps_command(is_macos);

		// step 2: generate DOT graph
		std::string dot_graph = generate_graph(ps_output, is_macos, config);

		// step 3: output results
		render_graph(dot_graph, "ptree.svg");
	} else { // handle input files case
		for (const auto& input_file : options.input_files) {
			fs::path input_path(input_file);
			std::string output_file = input_path.stem().string() + ".svg";

			// step 1: get process info
			std::string file_content;
			try {
				file_content = get_process_info(input_file);
			} catch (const std::exception& e) {
				std::cerr << "Error: " << e.what() << std::endl;
				return EXIT_FAILURE;
			}

			// step 2: generate DOT graph
			std::string dot_graph = generate_graph(file_content, is_macos, config);

			// step 3: output results
			render_graph(dot_graph, output_file);
		}
	}
	return EXIT_SUCCESS;
}
