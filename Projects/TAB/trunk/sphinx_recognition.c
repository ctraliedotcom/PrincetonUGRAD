#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <string.h>

#include <s2types.h>
#include <fbs.h>
#include <CM_macros.h>
#include <ad.h>
#include <cont_ad.h>
#include <err.h>
#include <glib.h>

#define SAMPLE_RATE 16000

static char* sphinx_command = "TAB "
"-verbose 0 "
"-live FALSE "
"-langwt 6.5 "
"-fwdflatlw 8.5 "
"-rescorelw 9.5 "
"-ugwt 0.5 "
"-fillpen 1e-10 "
"-silpen 0.005 "
"-inspen 0.65 "
"-top 4 "
"-topsenfrm 3 "
"-topsenthresh "
"-70000 "
"-beam 2e-06 "
"-npbeam 2e-06 "
"-lpbeam 2e-05 "
"-lponlybeam 0.0005 "
"-nwbeam 0.0005 "
"-fwdflat TRUE "
"-fwdflatbeam 1e-08 "
"-fwdflatnwbeam 0.0003 "
"-bestpath TRUE "
"-8bsen TRUE "
"-sendumpfn /usr/share/sphinx2/model/hmm/6k/sendump "
"-ndictfn /usr/share/sphinx2/model/hmm/6k/noisedict "
"-phnfn /usr/share/sphinx2/model/hmm/6k/phone "
"-mapfn /usr/share/sphinx2/model/hmm/6k/map "
"-hmmdir /usr/share/sphinx2/model/hmm/6k "
"-hmmdirlist /usr/share/sphinx2/model/hmm/6k "
"-cbdir /usr/share/sphinx2/model/hmm/6k "
"-dictfn alarm.dict ";
//"-lmfn /usr/share/sphinx2/model/lm/turtle/turtle.lm ";
//"-fsgfn alarm.fsg "
//"-allphone TRUE";
//"-fsgusefiller TRUE "
//"-fsgfn alarm.fsg "

char **argv;
int argc;
int32 frames;
char* said;
search_hyp_t** nbest;

int sphinx_rec_init() {
	g_shell_parse_argv (sphinx_command, &argc, &argv, NULL);
	fbs_init(argc, argv);
   return 0;
}

/* Record voice input for time specified by the argument "seconds"
   return a string containing the translated text */
int sphinx_rec_recordSpeech(int seconds) {
	ad_rec_t* ad;
	int16 adbuf[1000];
	int32 num_recorded;
	int i = 0;
    int wordID;

	if (uttproc_begin_utt(NULL) < 0) {
		printf("Failed to begin utterance processing");
		return 1;
	}

	ad = ad_open_sps(SAMPLE_RATE);
	if (ad == NULL) {
		printf("Failed to open audio device");
		return 1;
	}

	if (ad_start_rec(ad) < 0) {
		printf("Failed to start recording on audio device");
		return 1;
	}
	printf("Beginning recording...\n");
	//Go for two seconds
	for (i = 0; i < seconds*SAMPLE_RATE; ) {
		num_recorded = ad_read(ad, adbuf, 1000);
		i += num_recorded;
		if (num_recorded < 0) {
			printf("There was a problem recording data into the buffer, code %i", num_recorded);
			return 1;
		}
		uttproc_rawdata (adbuf, num_recorded, 1);
	}
	printf("Ending recording and beginning processing...\n");
	ad_stop_rec(ad);
    
	uttproc_end_utt ();
	if (uttproc_result (&frames, &said, 1) < 0) {
		printf("A problem occurred determining the string utterance");
		return 1;
	}

    //search_save_lattice();
    //search_get_alt (5, 0, frames -1, -1, kb_get_word_id ("<s>"), &nbest);

	ad_close(ad);
	return 0;
}

const char* sphinx_rec_getText() {
    return said;
}

const search_hyp_t** sphinx_rec_getNBest() {
    return nbest;
}

int sphinx_rec_shutdown() {
	fbs_end();
	free(argv);
	return 0;
}
