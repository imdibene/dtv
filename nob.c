#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#define NOB_EXPERIMENTAL_DELETE_OLD
#include "external/nob.h"
#include <time.h>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///
///	General settings
///
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
///	Compile options and config
////////////////////////////////////////////////////////////////////////////////
#define CC         \
	"clang++", \
	    "-std=c++17"

// TODO: libfdt headers on MacOS throw some warnings =/
#if defined(__linux__)
#define COMMON_CFLAGS  \
	"-Wall",       \
	    "-Werror", \
	    "-Wextra", \
	    "-pedantic"
#elif defined(__APPLE__) && defined(__MACH__)
#define COMMON_CFLAGS  \
	"-Wall",       \
	    "-Wextra", \
	    "-pedantic"
#else
#error "Unsupported platform"
#endif
#define PRJ_INCLUDE_PATHS              \
	"-Isrc",                       \
	    "-Iexternal",              \
	    "-I/opt/homebrew/include", \
	    "-I/opt/homebrew/include/graphviz"
#define EXTERNAL_LIBS_PATHS \
	"-L/opt/homebrew/lib"
#define EXTERNAL_LIBS   \
	"-lfdt",        \
	    "-lgvc",    \
	    "-lcgraph", \
	    "-lcdt"

#define SOURCE_CODE_FORMATTABLE_CODE                           \
	"find", ".",                                           \
	    "-type", "d", "-name", "external", "-prune", "-o", \
	    "-type", "d", "-name", ".git", "-prune", "-o",     \
	    "-type", "f", "(",                                 \
	    "-name", "*.c", "-o",                              \
	    "-name", "*.cc", "-o",                             \
	    "-name", "*.h",                                    \
	    ")", "-print"
#define SOURCE_CODE_FORMAT_CODE_STYLE                                                      \
	"clang-format",                                                                    \
	    "--style={BasedOnStyle: WebKit, UseTab: Always, TabWidth: 8, IndentWidth: 8}", \
	    "-i"

////////////////////////////////////////////////////////////////////////////////
///	Targets
////////////////////////////////////////////////////////////////////////////////
#define TARGET_DTV_APP           \
	CC,                      \
	    COMMON_CFLAGS,       \
	    PRJ_INCLUDE_PATHS,   \
	    EXTERNAL_LIBS_PATHS, \
	    EXTERNAL_LIBS,       \
	    "-o",                \
	    "build/dtv",         \
	    "src/dtv.cc"
#define TARGET_PS2GV_APP         \
	CC,                      \
	    COMMON_CFLAGS,       \
	    PRJ_INCLUDE_PATHS,   \
	    EXTERNAL_LIBS_PATHS, \
	    EXTERNAL_LIBS,       \
	    "-o",                \
	    "build/ps2gv",       \
	    "src/ps2gv.cc",      \
	    "src/ps2gv-config.cc"

////////////////////////////////////////////////////////////////////////////////
///	Information/Logger symbols
////////////////////////////////////////////////////////////////////////////////
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RED "\033[31m"
#define RESET "\033[0m"
// All symbols have got an ::space:: after the glyph, for better use with the log
#define SYMBOL_ERROR "🛑 "
#define SYMBOL_INFO "ℹ️ "
#define SYMBOL_BUILD "🔨 "
#define SYMBOL_TEST "🧪 "
#define SYMBOL_REWORK "🛠️ "
#define SYMBOL_FINISH "🏁 "
#define SYMBOL_TIME "⏳ "
#define SYMBOL_DOCS "📄 "

////////////////////////////////////////////////////////////////////////////////
///	Timing Macros
////////////////////////////////////////////////////////////////////////////////
typedef struct {
	struct timespec start;
	struct timespec end;
} Timer;
#define TIMER_START(t) clock_gettime(CLOCK_MONOTONIC, &(t).start)
#define TIMER_STOP(t) clock_gettime(CLOCK_MONOTONIC, &(t).end)
#define TIMER_MS(t) \
	(((t).end.tv_sec - (t).start.tv_sec) * 1e3 + ((t).end.tv_nsec - (t).start.tv_nsec) / 1e6)
#define TIMER_PRINT(t, name) \
	nob_log(INFO, YELLOW SYMBOL_TIME "%-20s: %7.3f sec" RESET, name, TIMER_MS(t) / 1e3)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///
///	Commands
///
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
///	Help
////////////////////////////////////////////////////////////////////////////////
void usage(const char* program)
{
	nob_log(INFO, "Usage: %s [<subcommand>]", program);
	nob_log(INFO, "Subcommands:");
	nob_log(INFO, "		build");
	nob_log(INFO, "			Build the applications.");
	nob_log(INFO, "		format");
	nob_log(INFO, "			Reformat codebase according to codestyle.");
	nob_log(INFO, "		help");
	nob_log(INFO, "			Print this message.");
}

////////////////////////////////////////////////////////////////////////////////
///	Build
////////////////////////////////////////////////////////////////////////////////
bool build(Cmd* cmd)
{
	Timer t;
	TIMER_START(t);
	nob_log(INFO, YELLOW SYMBOL_INFO "Checking project dependencies..." RESET);
	cmd_append(cmd, "bash", "utils/dependencies");
	if (!cmd_run_sync_and_reset(cmd))
		return false;
	TIMER_STOP(t);
	TIMER_PRINT(t, "Dependencies check.");

	TIMER_START(t);
	nob_log(INFO, YELLOW SYMBOL_BUILD "Building..." RESET);
	if (!mkdir_if_not_exists("build"))
		return false;
	cmd_append(cmd, TARGET_DTV_APP);
	if (!cmd_run_sync_and_reset(cmd))
		return false;
	cmd_append(cmd, TARGET_PS2GV_APP);
	if (!cmd_run_sync_and_reset(cmd))
		return false;
	TIMER_STOP(t);
	TIMER_PRINT(t, "Binary build.");
	return true;
}

////////////////////////////////////////////////////////////////////////////////
///	Format code style
////////////////////////////////////////////////////////////////////////////////
bool format(Cmd* cmd)
{
	Timer t;
	TIMER_START(t);
	nob_log(INFO, YELLOW SYMBOL_INFO "Reformatting codebase..." RESET);

	// find foo | xargs bar, i.e.
	// find stdout -> stdin xargs
	int pipe_fd[2];
	if (pipe(pipe_fd) == -1) {
		nob_log(ERROR, RED SYMBOL_ERROR "Could not create file descriptors for the pipes." RESET);
		exit(EXIT_FAILURE);
	}

	pid_t pid1 = fork();
	if (pid1 == -1) {
		nob_log(ERROR, RED SYMBOL_ERROR "Could not fork find." RESET);
		goto defer;
	}
	if (pid1 == 0) { // first child, i.e. will run find
		// Mario Bros. the fds, i.e. set the plumbing and stuff
		close(pipe_fd[0]);
		if (dup2(pipe_fd[1], STDOUT_FILENO) == -1) {
			nob_log(ERROR, RED SYMBOL_ERROR "dup2 failed at %s:%d" RESET, __FILE__, __LINE__);
			goto defer;
		}
		close(pipe_fd[1]);

		execlp("find", SOURCE_CODE_FORMATTABLE_CODE, (char*)NULL);
		nob_log(ERROR, RED SYMBOL_ERROR "find exec failed." RESET);
		exit(EXIT_FAILURE);
	}

	pid_t pid2 = fork();
	if (pid2 == -1) {
		nob_log(ERROR, RED SYMBOL_ERROR "Could not fork clang-format." RESET);
		goto defer;
	}
	if (pid2 == 0) { // second child, i.e. will run clang-format
		// Mario Bros. the fds, i.e. set the plumbing and stuff
		close(pipe_fd[1]);
		if (dup2(pipe_fd[0], STDIN_FILENO) == -1) {
			;
			nob_log(ERROR, RED SYMBOL_ERROR "dup2 failed at %s:%d" RESET, __FILE__, __LINE__);
			goto defer;
		}
		close(pipe_fd[0]);

		execlp("xargs", "xargs", SOURCE_CODE_FORMAT_CODE_STYLE, (char*)NULL);
		nob_log(ERROR, RED SYMBOL_ERROR "clang-format exec failed." RESET);
		exit(EXIT_FAILURE);
	}

	// Parent, close pipes
	close(pipe_fd[0]);
	close(pipe_fd[1]);

	// wait for the children to finish
	if (pid1 != -1)
		waitpid(pid1, NULL, 0);

	if (pid2 != -1)
		waitpid(pid2, NULL, 0);

	TIMER_STOP(t);
	TIMER_PRINT(t, "Codebase reformat.");
	return true;
defer:
	// Parent, close pipes
	close(pipe_fd[0]);
	close(pipe_fd[1]);

	// wait for the children to finish
	if (pid1 != -1)
		waitpid(pid1, NULL, 0);

	if (pid2 != -1)
		waitpid(pid2, NULL, 0);

	TIMER_STOP(t);
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///
///	main
///
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
	NOB_GO_REBUILD_URSELF(argc, argv);
	Timer total;
	TIMER_START(total);
	Cmd cmd = { 0 };

	const char* program = shift_args(&argc, &argv);

	if (argc > 0) {
		const char* subcmd = shift_args(&argc, &argv);
		if (strcmp(subcmd, "build") == 0) {
			if (!build(&cmd))
				return 1;
		} else if (strcmp(subcmd, "format") == 0) {
			if (!format(&cmd))
				return 1;
		} else if (strcmp(subcmd, "help") == 0) {
			usage(program);
		} else {
			usage(program);
			nob_log(ERROR, RED SYMBOL_ERROR "Unknown command `%s`" RESET, subcmd);
			return 1;
		}
	} else {
		if (!format(&cmd))
			return 1;
		if (!build(&cmd))
			return 1;
	}
	nob_log(INFO, GREEN SYMBOL_FINISH "Finish all requested tasks!" RESET);
	TIMER_STOP(total);
	TIMER_PRINT(total, "TOTAL RUNTIME");
	return 0;
}
