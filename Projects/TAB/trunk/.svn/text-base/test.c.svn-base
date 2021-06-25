/*Using stuff from
https://cmusphinx.svn.sourceforge.net/svnroot/cmusphinx/trunk/sphinx2/src/examples/*/
#include "weather.h"
#include "speak.h"
#include "news.h"
#include "sphinx_recognition.h"
#include <stdlib.h> /* required for randomize() and random() */
#include <time.h>
#include <fbs.h>

static char* sphinx_command = "test "
"-verbose 0 "
"-live TRUE "
"-langwt 6.5 "
"-fwdflatlw 8.5 -rescorelw 9.5 -ugwt 0.5 -fillpen 1e-10 -silpen 0.005 "
"-inspen 0.65 -top 1 -topsenfrm 3 -topsenthresh -70000 -beam 2e-06 "
"-npbeam 2e-06 -lpbeam 2e-05 -lponlybeam 0.0005 -nwbeam 0.0005 -fwdflat FALSE "
"-fwdflatbeam 1e-08 -fwdflatnwbeam 0.0003 -bestpath TRUE "
"-8bsen TRUE "
"-sendumpfn /usr/share/sphinx2/model/hmm/6k/sendump "
"-ndictfn /usr/share/sphinx2/model/hmm/6k/noisedict "
"-phnfn /usr/share/sphinx2/model/hmm/6k/phone "
"-mapfn /usr/share/sphinx2/model/hmm/6k/map "
"-hmmdir /usr/share/sphinx2/model/hmm/6k "
"-hmmdirlist /usr/share/sphinx2/model/hmm/6k "
"-cbdir /usr/share/sphinx2/model/hmm/6k "
"-fsgfn alarm.fsg "
"-dictfn alarm.dict "
"-fsgusefiller TRUE ";

/*static char* sphinx_command = "test "
"-fwdflat no "
"-bestpath yes "
"-hmm /usr/share/pocketsphinx/model/hmm/wsj1 "
"-fsg desktop-control.fsg "
"-dict desktop-control.dict ";*/

void weather() {
    const char* text;
	speak("Goodmorning, would you like to hear the weather?");
 
    sphinx_rec_recordSpeech(3);
    text = sphinx_rec_getText();

    //printf ("\n*%s*%i", text, strstr(text, "YES"));
    if (strstr(text, "YES") != NULL) {
        sayWeather("08540");
    }
    else {
        speak("OK, no weather for you.");
    }
}

void news() {
    const char* text;
	speak("Goodmorning, would you like to hear today's celebrity gossip?"); 
    printf("END SPEAK");
    sphinx_rec_recordSpeech(3);
    text = sphinx_rec_getText();

    //printf ("\n*%s*%i", text, strstr(text, "YES"));
    if (strstr(text, "YES") != NULL) {
        readHeadlines("http://img.perezhilton.com/?feed=rss2");
    }
    else {
        speak("OK, no gossip for you.");
    }
}

int getNumber(const char* num) {
    if(strcmp(num, "ZERO") == 0)
        return 0;

    if(strcmp(num, "ONE") == 0)
        return 1;

    if(strcmp(num, "TWO") == 0)
        return 2;

    if(strcmp(num, "THREE") == 0)
        return 3;

    if(strcmp(num, "FOUR") == 0)
        return 4;

    if(strcmp(num, "FIVE") == 0)
        return 5;

    if(strcmp(num, "SIX") == 0)
        return 6;

    if(strcmp(num, "SEVEN") == 0)
        return 7;

    if(strcmp(num, "EIGHT") == 0)
        return 8;

    if(strcmp(num, "NINE") == 0)
        return 9;
}

int game() {
    const char* text;
    search_hyp_t** nbest;
    const char* num;
    long num1, num2, answer, response;
    char say[100];
    const int LOW = 2;
    const int HIGH = 13;
    int i, j;

    uttproc_load_fsgfile("numbers.fsg");
    uttproc_set_fsg("numbers");
    
    time_t seconds;
    time(&seconds);
    srand((unsigned int) seconds);

    for (i=0; i<5; i++) {
        num1 = rand() % (HIGH - LOW + 1) + LOW;;
        num2 = rand() % (HIGH - LOW + 1) + LOW;;
        answer = num1 * num2;

        sprintf(say, "What is %i times %i ?", num1, num2);

	    speak(say); 
        printf("END SPEAK");
        sphinx_rec_recordSpeech(7);
        text = sphinx_rec_getText();
        nbest = sphinx_rec_getNBest();
/*
        while (nbest[j] != NULL) {
            printf ("\nPOSSIBLE: *%s*", nbest[j]->word);
            j++;
        }
*/
        if (strlen(text) == 0)
            continue;

        num = strtok((char*)text, " ");
        response = getNumber(num);

        while (num = strtok(NULL, " ")) {
            response = response*10 + getNumber(num);
        }
        printf ("\n*%i*", response);
        
        if (response == answer) {
            speak("CORRECT!");
        }
        else {
            sprintf(say, "Wrong, ass hole. It's %i", answer);
            speak(say);
        }
    }
    speak("Ok. Enough number crunching for you.");
}

int main(int argc, char** argv) {

    if (sphinx_rec_init())
        return 1;

    game();
    //news();

	//sayTime();
    //readHeadlines("http://rss.slashdot.org/Slashdot/slashdotYourRightsOnline");
//http://img.perezhilton.com/?feed=rss2
    sphinx_rec_shutdown();
	return 0;
}

