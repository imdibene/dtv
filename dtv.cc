#include <fstream>
#include <graphviz/gvc.h>
#include <iostream>
#include <libfdt.h>
#include <map>
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
		node->properties[name] = std::string((char*)value, len);
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

void create_graph(Agraph_t* graph, Device_Tree_Node_t* node)
{
	Agnode_t* parent_node = agnode(graph, (char*)node->name.c_str(), 1);
	for (auto& [k, v] : node->properties)
		agsafeset(parent_node, (char*)k.c_str(), (char*)v.c_str(), "");

	for (auto child : node->children) {
		Agnode_t* child_node = agnode(graph, (char*)child->name.c_str(), 1);
		agedge(graph, parent_node, child_node, nullptr, 1);
		// Same concern, maybe not an issue
		create_graph(graph, child);
	}
}

int main(int argc, char** argv)
{
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <dtb file>\n";
		return 1;
	}

	const char* dtb_path = argv[1];

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
	gvLayout(gvc, graph, "dot");
	gvRenderFilename(gvc, graph, "svg", "tree.svg");
	gvFreeLayout(gvc, graph);

	// Clean up
	agclose(graph);
	gvFreeContext(gvc);
	free(fdt);
	return 0;
}
