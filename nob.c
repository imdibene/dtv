#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#define NOB_EXPERIMENTAL_DELETE_OLD
#include "nob.h"

// TODO: libfdt headers on MacOS throw some warnings =/
#if defined(__linux__)
#define COMMON_CFLAGS    \
	"-Wall",         \
	    "-Werror",   \
	    "-Wextra",   \
	    "-pedantic", \
	    "-std=c++17"
#elif defined(__APPLE__) && defined(__MACH__)
#define COMMON_CFLAGS    \
	"-Wall",         \
	    "-Wextra",   \
	    "-pedantic", \
	    "-std=c++17"
#else
#error "Unsupported platform"
#endif
#define TARGET     \
	"-o",      \
	    "dtv", \
	    "dtv.cc"

void usage(const char* program)
{
	nob_log(INFO, "Usage: %s [<subcommand>]", program);
	nob_log(INFO, "Subcommands:");
	nob_log(INFO, "		build");
	nob_log(INFO, "			Build the application.");
	nob_log(INFO, "		help");
	nob_log(INFO, "			Print this message.");
}

bool build(Cmd* cmd)
{
	cmd_append(cmd, "bash", "format");
	if (!cmd_run_sync_and_reset(cmd))
		return false;

	cmd_append(cmd,
	    "clang++", COMMON_CFLAGS, TARGET,
	    "-I/opt/homebrew/include", "-I/opt/homebrew/include/graphviz",
	    "-L/opt/homebrew/lib",
	    "-lfdt", "-lgvc", "-lcgraph", "-lcdt");
	if (!cmd_run_sync_and_reset(cmd))
		return false;

	return true;
}

int main(int argc, char** argv)
{
	NOB_GO_REBUILD_URSELF(argc, argv);

	Cmd cmd = { 0 };

	const char* program = shift_args(&argc, &argv);

	if (argc > 0) {
		const char* subcmd = shift_args(&argc, &argv);
		if (strcmp(subcmd, "build") == 0) {
			if (!build(&cmd)) {
				return 1;
			}
		} else if (strcmp(subcmd, "help") == 0) {
			usage(program);
		} else {
			usage(program);
			nob_log(ERROR, "Unknown command `%s`", subcmd);
			return 1;
		}
	} else if (!build(&cmd)) {
		return 1;
	}
	return 0;
}
