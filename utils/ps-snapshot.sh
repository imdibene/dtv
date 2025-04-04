#!/usr/bin/env bash

if [[ "$(uname)" == "Darwin" ]]; then # Apple MacOS
	psargs="-eo ppid,pid,pcpu,rss,comm"
       	readargs="ppid pid pcpu rss comm"
else # Linux
	psargs="-eo zone,ppid,pid,pcpu,rss,comm"
       	readargs="zone ppid pid pcpu rss comm"
fi

ps ${psargs} | while read -r ${readargs}; do
	if [[ "$(uname)" == "Darwin" ]]; then
		# For macOS (using launchctl to check services)
		service=$(launchctl list | grep -E "\.${pid}" | cut -d'.' -f2)
	else
		# For Linux (systemd service check)
		service=$(readlink /proc/$pid/cgroup 2>/dev/null | grep -oP '(?<=/system.slice/)(.*)(?=.service)' | head -n 1)
	fi
	printf "%-5s %-5s %-5s %-6s %-6s %-20s %-20s\n" "$zone" "$ppid" "$pid" "$pcpu" "$rss" "$comm" "${service:--}"
done
