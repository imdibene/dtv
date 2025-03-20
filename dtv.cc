#include <graphviz/gvc.h>
#include <iostream>
#include <libfdt.h>
#include <map>
#include <string>
#include <vector>

struct Device_Tree_Node_t {
	std::string name;
	std::map<std::string, std::string> properties;
	std::vector<Device_Tree_Node_t*> children;
};

Device_Tree_Node_t* parse_tree(void* fdt, int node_offset)
{
	Device_Tree_Node_t* node = new Device_Tree_Node_t();
	node->name = fdt_get_name(fdt, node_offset, NULL);

	// Parse properties
	int property_offset;
	fdt_for_each_property_offset(property_offset, fdt, node_offset)
	{
		const char* name;
		const char* value;
		int len;
		value = (const char*)fdt_getprop_by_offset(fdt, property_offset, &name, &len);
		node->properties[name] = std::string(value, len);
	}

	// Parse children
	int child_offset;
	fdt_for_each_subnode(child_offset, fdt, node_offset)
	{
		// I hope that for a finite number of subnodes, the recursive call
		// will not be too much of an issue. If it gets tanked for big dtbs
		// maybe migrate this whole thing to OCaml or some flavour of lisp.
		node->children.push_back(parse_tree(fdt, child_offset));
	}

	return node;
}

// escape special chars for correct html/svg/xml rendering
std::string escape_special_chars(const std::string& s)
{
	std::string escaped;
	for (char c : s)
		switch (c) {
		case '<':
			escaped += "&lt;";
			break;
		case '>':
			escaped += "&gt;";
			break;
		case '&':
			escaped += "&amp;";
			break;
		default:
			escaped += c;
			break;
		}
	return escaped;
}

// keep only printable chars (ASCII 0x20-0x7E)
std::string sanitise_string(const std::string& s)
{
	std::string sanitised;
	for (char c : s)
		if (c >= 32 && c <= 126)
			sanitised += c;
	return escape_special_chars(sanitised);
}

void create_graph(Agraph_t* graph, Device_Tree_Node_t* node)
{
	// Create the node using only the name as the label
	Agnode_t* parent_node = agnode(graph, const_cast<char*>(sanitise_string(node->name).c_str()), 1);
	agsafeset(parent_node, const_cast<char*>("label"), const_cast<char*>(sanitise_string(node->name).c_str()), const_cast<char*>(""));
	// Add properties as a tooltip
	if (!node->properties.empty()) {
		std::string tooltip;
		for (const auto& [k, v] : node->properties)
			tooltip += sanitise_string(k) + "=" + sanitise_string(v) + "\\n";
		agsafeset(parent_node, const_cast<char*>("tooltip"), const_cast<char*>(tooltip.c_str()), const_cast<char*>(""));
	}

	for (auto child : node->children) {
		Agnode_t* child_node = agnode(graph, (char*)child->name.c_str(), 1);
		agedge(graph, parent_node, child_node, nullptr, 1);
		// Same concern, maybe not an issue
		create_graph(graph, child);
	}
}

int main(int argc, char** argv)
{
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <dtb_file> <render_engine>\n";
		std::cerr << "Render engine options: dot, fdp\n";
		return 1;
	}

	const char* dtb_path = argv[1];
	std::string in(dtb_path);
	std::string out = in.substr(0, in.find_last_of('.')) + ".svg";
	const char* render_engine = argv[2];
	// validate render engine
	if (std::string(render_engine) != "dot" && std::string(render_engine) != "fdp") {
		std::cerr << "Invalid render engine. Choose either 'dot' or 'fdp'.\n";
		return 1;
	}

	// Load DTB file
	FILE* f = fopen(dtb_path, "rb");
	if (!f) {
		std::cerr << "Failed to open DTB file: " << dtb_path << "\n";
		return 1;
	}
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fseek(f, 0, SEEK_SET);
	void* fdt = malloc(size);
	fread(fdt, 1, size, f);
	fclose(f);

	// Parse DTB file
	if (fdt_check_header(fdt)) {
		std::cerr << "Invalid DTB file: " << dtb_path << "\n";
		free(fdt);
		return 1;
	}
	Device_Tree_Node_t* root = parse_tree(fdt, 0);

	// Create the graph
	GVC_t* gvc = gvContext();
	Agraph_t* graph = agopen((char*)"Device-Tree", Agdirected, nullptr);
	// Add nodes and edges to the graph
	create_graph(graph, root);
	// Render
	gvLayout(gvc, graph, render_engine);
	gvRenderFilename(gvc, graph, "svg", out.c_str());
	gvFreeLayout(gvc, graph);

	// Clean up
	agclose(graph);
	gvFreeContext(gvc);
	free(fdt);
	return 0;
}
