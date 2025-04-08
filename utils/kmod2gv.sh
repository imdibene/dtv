#!/bin/bash
#TODO: collect the modinfo and put it into a tooltip
# better colour mapping true to kernel subsystem
OUT="kmods.dot"

# Map of subsystems to colours
declare -A COLOURS=(
	[usb]="#88c0d0"
	[net]="#ebcb8b"
	[sound]="#a3be8c"
	[rtc]="#b48ead"
	[video]="#5e81ac"
	[crypto]="#bf616a"
	#[filesystem]="#d08770"
	[filesystem]="red"
	[input]="#a3be8c"
	[block]="#d08770"
	[default]="#e5e9f0"
)

# Function to extract the subsystem from modinfo path
function get_subsystem()
{
	local mod="$1"
	local path
	path=$(modinfo -n "$mod" 2>/dev/null) || return
	# Normalize and extract folder under kernel/drivers/
	if [[ "$path" =~ /kernel/drivers/([^/]+)/ ]]; then
		echo "${BASH_REMATCH[1]}"
	elif [[ "$path" =~ /kernel/(fs|crypto|net|sound|block)/ ]]; then
		echo "${BASH_REMATCH[1]}"
	else
		echo "default"
	fi
}

# Get fill colour based on subsystem
get_colour()
{
	local sub="$1"
	echo "${COLOURS[$sub]:-${COLOURS[default]}}"
}

# Start dot graph
{
	echo "digraph kmods {"
	echo "  rankdir=LR;"
	echo "  node [style=filled, shape=box, fontname=monospace];"
} > "$OUT"


# Graph nodes and edges
while read -r name size usedby deps _; do
	subsystem=$(get_subsystem "$name")
	colour=$(get_colour "$subsystem")
	echo "  \"$name\" [label=\"$name\\n($subsystem)\", fillcolor=\"$colour\"];" >> "$OUT"

	if [[ "$deps" != "-" ]]; then
		IFS=',' read -ra dep_array <<< "$deps"
		for dep in "${dep_array[@]}"; do
			echo "  \"$name\" -> \"$dep\";" >> "$OUT"
		done
	fi
done < /proc/modules

echo "}" >> "$OUT"

echo "[+] DOT graph saved to $OUT"
echo "    Render with: fdp -Tsvg $OUT -o kmods.svg"

