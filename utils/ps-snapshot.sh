#!/usr/bin/env bash

if [[ "$(uname)" == "Darwin" ]]; then # Apple MacOS
	ps -eo ppid,pid,rss,pcpu,comm
else # Linux
	ps -eo zone,ppid,pid,rss,pcpu,comm | while read -r zone ppid pid rss pcpu comm; do
	#service=$(cat /proc/$pid/cgroup 2>/dev/null | grep -oP '(?<=/system.slice/)[^/]*' | head -n 1)
	#service=$(cat /proc/$pid/cgroup 2>/dev/null | grep -oP '[^/]+\.service|[^/]+\.socket|[^/]+\.scope|[^/]+\.timer|[^/]+\.mount' | tail -n 1)
	service=$(cat /proc/$pid/cgroup 2>/dev/null | grep -oP '[^/]+\.service|[^/]+\.timer' | tail -n 1)
		printf "%-5s %-5s %-5s %-6s %-6s %-20s %-20s\n" "$zone" "$ppid" "$pid" "$rss" "$pcpu" "$comm" "${service:--}"
	done
fi
