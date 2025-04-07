#include "ps2gv/process-capture.h"
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

std::vector<ProcessInfo> capture_live()
{
	int pipefd[2];
	if (pipe(pipefd) == -1)
		throw std::runtime_error("pipe failed");

	pid_t pid = fork();
	if (pid == -1)
		throw std::runtime_error("fork failed");

	const bool is_macos =
#ifdef __APPLE__
	    true;
#else
	    false;
#endif

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

	// Parse output
	std::istringstream iss(output);
	std::string line;
	std::vector<ProcessInfo> procs;

	// skip header
	std::getline(iss, line);

	while (std::getline(iss, line)) {
		std::istringstream linestream(line);
		ProcessInfo info;
		if (is_macos) { // MacOS: PPID PID RSS PCPU COMM
			info.zone = "-"; // There are no zones in MacOS.
			linestream >> info.ppid >> info.pid >> info.rss >> info.pcpu;
			std::getline(linestream, info.command);
			info.unit = "-"; // MacOS ps's shows the full command name, so no need to translate to the service name.
		} else { // Linux: ZONE PPID PID RSS PCPU COMM
			linestream >> info.zone >> info.ppid >> info.pid >> info.rss >> info.pcpu >> info.command;
			// Get systemd unit from /proc fs
			std::string cgroup_path = "/proc/" + info.pid + "/cgroup";
			std::ifstream cgroup_file(cgroup_path);
			std::string cgroup_content((std::istreambuf_iterator<char>(cgroup_file)), std::istreambuf_iterator<char>());
			// Match the last .service/.scope/.timer/etc which is the most specific unit for that PID
			// std::regex unit_re(R"(([^/\n]+\.(service|scope|timer|socket|mount)))"); // TODO: This was too verbose, add an option to enable how much translation
			std::regex unit_re(R"(([^/\n]+\.(service|timer)))");
			std::smatch match;
			std::string last_unit;
			auto begin = std::sregex_iterator(cgroup_content.begin(), cgroup_content.end(), unit_re);
			auto end = std::sregex_iterator();
			for (auto it = begin; it != end; ++it)
				last_unit = (*it)[1].str();
			info.unit = last_unit.empty() ? "-" : last_unit;
		}
		procs.push_back(info);
	}
	return procs;
}

PSFormat detect_format(const std::string& first_line)
{
	if (first_line.find("ZONE") != std::string::npos)
		return PSFormat::Linux;
	if (first_line.find("PPID") != std::string::npos)
		return PSFormat::MacOS;
	return PSFormat::Unknown;
}

std::vector<ProcessInfo> parse_ps_snapshot(const std::string& filename)
{
	std::ifstream file(filename);
	if (!file)
		throw std::runtime_error("Cannot open ps snapshot file: " + filename);

	std::vector<ProcessInfo> procs;
	std::string line;

	// Read header to detect format
	if (!std::getline(file, line))
		return procs;
	PSFormat format = detect_format(line);

	while (std::getline(file, line)) {
		std::istringstream linestream(line);
		ProcessInfo info;

		if (format == PSFormat::MacOS) {
			info.zone = "-"; // There are no zones in MacOS.
			linestream >> info.ppid >> info.pid >> info.rss >> info.pcpu >> info.command;
			info.unit = "-"; // MacOS ps's shows the full command name, so no need to translate to the service name.
		} else if (format == PSFormat::Linux) {
			linestream >> info.zone >> info.ppid >> info.pid >> info.rss >> info.pcpu >> info.command >> info.unit;
		} else {
			continue; // Unknown format
		}
		procs.push_back(info);
	}
	return procs;
}
