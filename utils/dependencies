#!/usr/bin/env bash

declare -A TOOLS=(
	["clang"]="On macOS: brew install llvm\nOn Debian: sudo apt install clang"
	["clang++"]="On macOS: brew install llvm\nOn Debian: sudo apt install clang"
	["dot"]="On macOS: brew install graphviz\nOn Debian: sudo apt install graphviz"
	["dtc"]="On macOS: brew install dtc\nOn Debian: sudo apt install device-tree-compiler"
	["pkg-config"]="On macOS: brew install pkg-config\nOn Debian: sudo apt install pkg-config"
)

function command_exists()
{
	command -v "$1" >/dev/null 2>&1
}

function print_help()
{
	echo "ERROR: Missing dependency --> $1"
	echo "---------------------------------------------"
	echo "Installation instructions for missing dependencies:"
	echo "---------------------------------------------"
	for tool in "${!TOOLS[@]}"; do
		echo "$tool:"
		echo -e "${TOOLS[$tool]}"
		echo ""
	done
	echo "---------------------------------------------"
	echo "Please install the missing dependencies and rerun this script."
	echo "---------------------------------------------"
}

for tool in "${!TOOLS[@]}"; do
	if ! command_exists "$tool"; then
		print_help "$tool"
		exit 1
	fi
done

# Verify Graphviz libraries
if ! pkg-config --exists libgvc libcgraph libcdt; then
	echo "Graphviz libraries (libgvc, libcgraph, libcdt) are missing"
	exit 1
fi

# All dependencies are installed
echo "All dependencies are installed. You're good to go!"
exit 0
