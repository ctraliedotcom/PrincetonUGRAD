#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "number_game.h"
#include "weather.h"
#include "speak.h"
#include "news.h"
#include "sphinx_recognition.h"
#include "alarmconfig.h"
#include "play.h"

void news(char* rss, int num_headlines) {
    const char* text;
	speak("Would you like to hear the news?"); 
    printf("END SPEAK");
    sphinx_rec_recordSpeech(5);
    text = sphinx_rec_getText();

    //printf ("\n*%s*%i", text, strstr(text, "YES"));
    if (strstr(text, "YES") != NULL) {
        readHeadlines(rss, num_headlines);
    }
    else {
        speak("OK, no news for you.");
    }
}

int main(int argc, char** argv) {
    char say[100];
    const char* text;
    int cur_min;	
    struct Configuration* config;

	if (argc != 2) {
		fprintf(stderr, "Usage: TAB configfile\n");
		return 1;
	}
	config = getConfig(argv[1]);

	festival_initialize();

    if (sphinx_rec_init())
        return 1;

    uttproc_load_fsgfile("numbers.fsg");
    uttproc_set_fsg("numbers");
    //number_game(100);
    number_test(30);
    return;



    number_game(10);

    /* wait until the correct time to sound alarm*/
    while (1) {
        if (getCurrentHour() < config->alarm_hour) {
            sleep(60*60);
            continue;
        }

        cur_min = getCurrentMinute();
        if (config->alarm_minute - cur_min > 1) {
            printf("cur_hour=%i | cur_min=%i\n",getCurrentHour(),getCurrentMinute());
            sleep(60);
            continue;
        }
        while (getCurrentMinute() < config->alarm_minute) {
            sleep(2);
        }
        break;
    }

    while (1) {
        uttproc_load_fsgfile("alarm.fsg");
        uttproc_set_fsg("alarm");
        while (1) {
            playFile("alarm.wav");
            sphinx_rec_recordSpeech(3);
            text = sphinx_rec_getText();
            if (strstr(text, "STOP") != NULL) {
                break;
            }
        }
        sprintf(say, "Good morning %s", config->name);
        speak(say);

        sayWeather(config->zipcode);        

        speak("Let's play a game.");
        if (number_game(10) == -1) {
            continue;
        }

        //news(config->NewsURL, config->rssnum);
        readHeadlines(config->NewsURL, config->rssnum);

        speak("Let's play a game.");
        if (number_game(100) == -1) {
            continue;
        }
    }

    sphinx_rec_shutdown();
	 freeConfig(config);
    return 0; 
}
