#include "play.h"

void playFile(char* filename) {
	int argc;
	char** argv;
	gchar* command = g_strconcat("play ", filename, NULL);
	g_spawn_command_line_sync(command, NULL, NULL, NULL, NULL);
	g_free(command);
}
