#include <cctype>
#include <fcntl.h>
#include <fstream>
#include <getopt.h>
#include <graphviz/gvc.h>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

struct Config {
	std::string input_file;
	std::string output_file = "process_tree.svg";
	bool use_stdin = false;
	bool use_stdout = false;
	bool is_macos;
};

Config parse_args(int argc, char* argv[]);
std::string generate_graph(const std::string& ps_output, bool is_macos);
std::string get_process_info(const Config& config);
std::string run_ps_command(bool is_macos);
void output_results(const Config& config, const std::string& dot_graph);
void render_graph(const std::string& dot_graph, const std::string& output_file);

Config parse_args(int argc, char* argv[])
{
	Config config;
	config.is_macos =
#ifdef __APPLE__
	    true;
#else
	    false;
#endif

	int opt;
	while ((opt = getopt(argc, argv, "i:o:")) != -1) {
		switch (opt) {
		case 'i':
			config.input_file = optarg;
			break;
		case 'o':
			config.output_file = optarg;
			break;
		default:
			std::cerr << "Usage: " << argv[0] << " [-i input_file] [-o output_file]\n";
			exit(EXIT_FAILURE);
		}
	}

	if (config.input_file.empty())
		config.use_stdin = true;
	if (config.output_file == "-")
		config.use_stdout = true;

	return config;
}

std::string get_process_info(const Config& config)
{
	if (!config.use_stdin) { // Read from file
		std::ifstream file(config.input_file);
		if (!file)
			throw std::runtime_error("Failed to open input file");
		return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	} else { // Get from ps command
		return run_ps_command(config.is_macos);
	}
}

void output_results(const Config& config, const std::string& dot_graph)
{
	if (config.use_stdout) // Output DOT to stdout
		std::cout << dot_graph;
	else // Render to PNG file
		render_graph(dot_graph, config.output_file);
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
	char buffer[4096];
	ssize_t bytes_read;
	while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer))) > 0)
		output.append(buffer, bytes_read);
	close(pipefd[0]);
	waitpid(pid, nullptr, 0);
	return output;
}

std::string generate_graph(const std::string& ps_output, bool is_macos)
{
	std::stringstream dot_stream;
	dot_stream << "digraph ptree {\n";
	dot_stream << "node [style=filled];\n";

	std::istringstream iss(ps_output);
	std::string line;

	// Skip header line
	std::getline(iss, line);

	// TODO: Colour the nodes per configurable category
	// TODO: Set the node size accordingly to its CPU usage
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
		if (std::stoi(pid) <= 10)
			continue;

		size_t last_slash = comm.find_last_of('/');
		if (last_slash != std::string::npos)
			comm = comm.substr(last_slash + 1);

		dot_stream << "  \"" << ppid << "\" -> \"" << pid << "\";\n";
		dot_stream << "  \"" << pid << "\" [label=\"" << comm << "\"];\n";
	}

	dot_stream << "}\n";
	return dot_stream.str();
}

void render_graph(const std::string& dot_graph, const std::string& output_file = "process_tree.svg")
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
	try {
		// Parse command line arguments
		Config config = parse_args(argc, argv);

		// Step 1: Get process information
		std::string ps_output = get_process_info(config);

		// Step 2: Generate DOT graph
		std::string dot_graph = generate_graph(ps_output, config.is_macos);

		// Step 3: Output results
		output_results(config, dot_graph);

	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
