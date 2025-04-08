#!/bin/bash

OUT="kmods.dot"

# Map of subsystems to colors
declare -A COLORS=(
[usb]="#88c0d0"
[net]="#ebcb8b"
[sound]="#a3be8c"
[rtc]="#b48ead"
[video]="#5e81ac"
[crypto]="#bf616a"
[filesystem]="#d08770"
[input]="#a3be8c"
[block]="#d08770"
[default]="#e5e9f0"
)

# Function to extract the subsystem from modinfo path
get_subsystem() {
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

# Get fill color based on subsystem
get_color() {
local sub="$1"
echo "${COLORS[$sub]:-${COLORS[default]}}"
}

# Start dot graph
{
echo "digraph kmods {"
echo "  rankdir=LR;"
echo "  node [style=filled, shape=box, fontname=monospace];"
} > "$OUT"

declare -A seen_subsystems

# Graph nodes and edges
while read -r name size usedby deps _; do
subsystem=$(get_subsystem "$name")
color=$(get_color "$subsystem")
echo "  \"$name\" [label=\"$name\\n($subsystem)\", fillcolor=\"$color\"];" >> "$OUT"
seen_subsystems["$subsystem"]=1

if [[ "$deps" != "-" ]]; then
IFS=',' read -ra dep_array <<< "$deps"
for dep in "${dep_array[@]}"; do
echo "  \"$name\" -> \"$dep\";" >> "$OUT"
done
fi
done < /proc/modules

# Add legend
echo "  subgraph cluster_legend {" >> "$OUT"
echo "    label=\"Subsystem Color Legend\";" >> "$OUT"
echo "    style=dashed;" >> "$OUT"
for sub in "${!seen_subsystems[@]}"; do
color=$(get_color "$sub")
echo "    \"legend_$sub\" [label=\"$sub\", fillcolor=\"$color\"];" >> "$OUT"
done
echo "  }" >> "$OUT"

echo "}" >> "$OUT"

echo "[+] DOT graph with color-coded legend saved to $OUT"
echo "    Render with: dot -Tsvg $OUT -o kmods.svg"

