#include "play.h"
#include <unistd.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

void playFile(char* filename) {
	int argc;
	char** argv;
    /* note: user must have play command installed for this to work */
	gchar* command = g_strconcat("play ", filename, NULL);
	g_spawn_command_line_sync(command, NULL, NULL, NULL, NULL);
	g_free(command);
    sleep(1); /* allow time for sound device to close. Before adding this statement,
                 speech synthesis failed when called immediately after play */
}  
