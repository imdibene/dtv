#ifndef PS2GV_CAPTURE_H
#define PS2GV_CAPTURE_H
#include <string>
#include <vector>

enum class PSFormat {
	Linux,
	MacOS,
	Unknown
};

struct ProcessInfo {
	std::string zone; // Linux only
	std::string ppid;
	std::string pid;
	std::string rss;
	std::string pcpu;
	std::string command;
	std::string unit; // systemd unit if available
};

std::vector<ProcessInfo> capture_live();
std::vector<ProcessInfo> parse_ps_snapshot(const std::string& filename);
PSFormat detect_format(const std::string& first_line);
#endif // PS2GV_CAPTURE_H
