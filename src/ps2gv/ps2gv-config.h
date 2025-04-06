#ifndef PS2GV_CONFIG_H
#define PS2GV_CONFIG_H

#include <map>
#include <string>

struct Config {
	// colouring
	std::map<std::string, std::string> comm_colours = {
		// default
		{ "default", "silver" },
		// web & communication
		{ "NetworkManager", "palevioletred3" },
		{ "curl", "palevioletred3" },
		{ "httpd", "palevioletred3" },
		{ "mosquitto", "palevioletred3" },
		{ "nginx", "palevioletred3" },
		{ "openvpn", "palevioletred3" },
		{ "wget", "palevioletred3" },
		// databases
		{ "mysql", "orchid" },
		{ "sqlite3", "orchid" },
		// languages
		{ "erl", "palegreen3" },
		{ "java", "palegreen3" },
		{ "miniperl", "palegreen3" },
		{ "ocaml", "palegreen3" },
		{ "perl", "palegreen3" },
		{ "php", "palegreen3" },
		{ "python", "palegreen3" },
		{ "python2.5", "palegreen3" },
		{ "python2.6", "palegreen3" },
		{ "python3", "palegreen3" },
		{ "ruby", "palegreen3" },
		{ "sbcl", "palegreen3" },
		// user
		{ "emacs", "olivedrab3" },
		{ "screen", "olivedrab3" },
		{ "ssh", "olivedrab3" },
		{ "sshd", "olivedrab3" },
		{ "tmux", "olivedrab3" },
		{ "vi", "olivedrab3" },
		{ "vim", "olivedrab3" },
		// compiler
		{ "clang", "peachpuff3" },
		{ "clang++", "peachpuff3" },
		{ "g++", "peachpuff3" },
		{ "gcc", "peachpuff3" },
		{ "gdb", "peachpuff3" },
		{ "gdbserver", "peachpuff3" },
		{ "ld", "peachpuff3" },
		{ "lldb", "peachpuff3" },
		// other
		{ "imapd", "skyblue3" },
		{ "master", "skyblue3" },
		{ "nrpe", "skyblue3" },
		{ "pickup", "skyblue3" },
		{ "postgres", "skyblue3" },
		{ "qmgr", "skyblue3" },
		{ "rrdtool", "skyblue3" },
		{ "sendmail", "skyblue3" },
		{ "smtp", "skyblue3" },
		{ "zabbix", "skyblue3" },
		{ "zabbix_agentd", "skyblue3" },
		// highlight
		{ "bcc", "lightsalmon" },
		{ "bpf", "lightsalmon" },
		{ "htop", "lightsalmon" },
		{ "iperf", "lightsalmon" },
		{ "perf", "lightsalmon" },
		{ "sysbench", "lightsalmon" },
		{ "top", "lightsalmon" },
		// shell scripting
		{ "awk", "lightblue3" },
		{ "basename", "lightblue3" },
		{ "bc", "lightblue3" },
		{ "cat", "lightblue3" },
		{ "cut", "lightblue3" },
		{ "date", "lightblue3" },
		{ "dc", "lightblue3" },
		{ "dirname", "lightblue3" },
		{ "gawk", "lightblue3" },
		{ "ggrep", "lightblue3" },
		{ "grep", "lightblue3" },
		{ "head", "lightblue3" },
		{ "hostname", "lightblue3" },
		{ "sed", "lightblue3" },
		{ "sleep", "lightblue3" },
		{ "tail", "lightblue3" },
		{ "uname", "lightblue3" },
		{ "wc", "lightblue3" },
		// custom
		{ "heartbeat", "royalblue" },
		// system
		{ "-bash", "paleturquoise3" },
		{ "-zsh", "paleturquoise3" },
		{ "<defunct>", "paleturquoise3" },
		{ "bash", "paleturquoise3" },
		{ "cron", "paleturquoise3" },
		{ "devfsadmd", "paleturquoise3" },
		{ "dlmgmtd", "paleturquoise3" },
		{ "fmd", "paleturquoise3" },
		{ "fsflush", "paleturquoise3" },
		{ "in.ndpd", "paleturquoise3" },
		{ "inetd", "paleturquoise3" },
		{ "init", "paleturquoise3" },
		{ "kcfd", "paleturquoise3" },
		{ "ldap_cachemgr", "paleturquoise3" },
		{ "lockd", "paleturquoise3" },
		{ "login", "paleturquoise3" },
		{ "nscd", "paleturquoise3" },
		{ "ntpd", "paleturquoise3" },
		{ "pageout", "paleturquoise3" },
		{ "picld", "paleturquoise3" },
		{ "poold", "paleturquoise3" },
		{ "ps", "paleturquoise3" },
		{ "rcapd", "paleturquoise3" },
		{ "rotatelogs", "paleturquoise3" },
		{ "rpcbind", "paleturquoise3" },
		{ "sac", "paleturquoise3" },
		{ "sh", "paleturquoise3" },
		{ "snmpd", "paleturquoise3" },
		{ "sort", "paleturquoise3" },
		{ "sshd", "paleturquoise3" },
		{ "statd", "paleturquoise3" },
		{ "syseventd", "paleturquoise3" },
		{ "syslogd", "paleturquoise3" },
		{ "systemd", "paleturquoise3" },
		{ "ttymod", "paleturquoise3" },
		{ "ttymon", "paleturquoise3" },
		{ "utmpd", "paleturquoise3" },
		{ "zlogin", "paleturquoise3" },
		{ "zoneadmd", "paleturquoise3" },
		{ "zsched", "paleturquoise3" },
		{ "zsh", "paleturquoise3" }
	};
	// scaling
	float cpu_limit = 10.0f;
	float min_cpu_threshold = 0.1f; // cpu < 0.1 gets no scaling
	float base_width = 1.0f;
	float width_factor = 1.8f;
	float base_height = 0.7f;
	float height_factor = 2.3f;
	int base_font_size = 10;
	// zones
	bool hide_zones = false;
	// TODO: animation
	// int frame_delay_ms = 500;
	// bool create_animation = false;
	// load from file if exists
	void load(const std::string& filename);
};
#endif // PS2GV_CONFIG_H
