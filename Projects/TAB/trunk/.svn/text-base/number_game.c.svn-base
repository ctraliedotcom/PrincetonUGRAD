#include "number_game.h"
#include "speak.h"
#include "weather.h"
#include "sphinx_recognition.h"
#include <stdlib.h> /* required for randomize() and random() */
#include <time.h>
#include <fbs.h>
#include <string.h>
#include <mrss.h> /* for printf */

static int getNumber(const char* str_num) {
    if(strcmp(str_num, "ONE") == 0)
        return 1;

    if(strcmp(str_num, "TWO") == 0)
        return 2;

    if(strcmp(str_num, "THREE") == 0)
        return 3;

    if(strcmp(str_num, "FOUR") == 0)
        return 4;

    if(strcmp(str_num, "FIVE") == 0)
        return 5;

    if(strcmp(str_num, "SIX") == 0)
        return 6;

    if(strcmp(str_num, "SEVEN") == 0)
        return 7;

    if(strcmp(str_num, "EIGHT") == 0)
        return 8;

    if(strcmp(str_num, "NINE") == 0)
        return 9;

    if(strcmp(str_num, "TEN") == 0)
        return 10;

    if(strcmp(str_num, "ELEVEN") == 0)
        return 11;

    if(strcmp(str_num, "TWELVE") == 0)
        return 12;

    if(strcmp(str_num, "THIRTEEN") == 0)
        return 13;

    if(strcmp(str_num, "FOURTEEN") == 0)
        return 14;

    if(strcmp(str_num, "FIFTEEN") == 0)
        return 15;

    if(strcmp(str_num, "SIXTEEN") == 0)
        return 16;

    if(strcmp(str_num, "SEVENTEEN") == 0)
        return 17;

    if(strcmp(str_num, "EIGHTEEN") == 0)
        return 18;

    if(strcmp(str_num, "NINETEEN") == 0)
        return 19;

    if(strcmp(str_num, "TWENTY") == 0)
        return 20;

    if(strcmp(str_num, "THIRTY") == 0)
        return 30;

    if(strcmp(str_num, "FORTY") == 0)
        return 40;

    if(strcmp(str_num, "FIFTY") == 0)
        return 50;

    if(strcmp(str_num, "SIXTY") == 0)
        return 60;

    if(strcmp(str_num, "SEVENTY") == 0)
        return 70;

    if(strcmp(str_num, "EIGHTY") == 0)
        return 80;

    if(strcmp(str_num, "NINETY") == 0)
        return 90;

    if(strcmp(str_num, "HUNDRED") == 0)
        return 100;

    /* if ZERO, AND, or "" */
    return 0;
}

int number_test(int rounds) {
    const char* text;
    search_hyp_t** nbest;
    search_hyp_t* next;
    const char* str_num;
    long answer, prev_num, next_num, response;
    char say[100];
    const int LOW = 0;
    const int HIGH = 999;
    int i, j;
    int no_response = 0;

    uttproc_load_fsgfile("numbers.fsg");
    uttproc_set_fsg("numbers");
    
    time_t seconds;
    time(&seconds);
    srand((unsigned int) seconds);

    for (i=0; i<rounds; i++) {
        answer = rand() % (HIGH - LOW + 1) + LOW;;

        sprintf(say, "Read me %li", answer);
	    speak(say); 
 
        sphinx_rec_recordSpeech(5);
        text = sphinx_rec_getText();

        if (no_response > 3) {
            return -1;
        }
        if (strlen(text) == 0) {
            no_response++;        
            continue;
        }
        no_response = 0;

        str_num = strtok((char*)text, " ");
        prev_num = getNumber(str_num);
        response = prev_num;

        while (str_num = strtok(NULL, " ")) {
            next_num = getNumber(str_num);
            if (next_num == 100) {
                if (prev_num == 0) {
                    printf("***hmm, something funny happened. zero-hundred?***\n");
                }
                /* -prev_num because we added on before when we shouldn't have */
                response += (prev_num * 100) - prev_num;
            }
            else {
                response += next_num;
            }
            prev_num = next_num;
        }
        printf ("\n*[%li]*\n", response);
        fprintf(stderr,"%li %li\n", answer, response);

        if (response == answer) {
            speak("CORRECT!");
        }
        else {
            sprintf(say, "You said %li", response);
            speak(say);
        }
        sleep(1.5);
    }
    speak("Ok. We're done.");
    return 1;
}

int number_game(int rounds) {
    const char* text;
    search_hyp_t** nbest;
    search_hyp_t* next;
    const char* str_num;
    long num1, num2, answer, prev_num, next_num, response;
    char say[100];
    const int LOW = 2;
    const int HIGH = 13;
    int i, j;
    int no_response = 0;

    uttproc_load_fsgfile("numbers.fsg");
    uttproc_set_fsg("numbers");
    
    time_t seconds;
    time(&seconds);
    srand((unsigned int) seconds);

    for (i=0; i<rounds; i++) {
        num1 = rand() % (HIGH - LOW + 1) + LOW;;
        num2 = rand() % (HIGH - LOW + 1) + LOW;;
        answer = num1 * num2;

        sprintf(say, "What is %li times %li ?", num1, num2);
	    speak(say); 
 
        sphinx_rec_recordSpeech(5);
        text = sphinx_rec_getText();

        if (no_response > 3) {
            return -1;
        }
        if (strlen(text) == 0) {
            no_response++;        
            continue;
        }
        no_response = 0;

        str_num = strtok((char*)text, " ");
        prev_num = getNumber(str_num);
        response = prev_num;

        while (str_num = strtok(NULL, " ")) {
            next_num = getNumber(str_num);
            if (next_num == 100) {
                if (prev_num == 0) {
                    printf("***hmm, something funny happened. zero-hundred?***\n");
                }
                /* -prev_num because we added on before when we shouldn't have */
                response += (prev_num * 100) - prev_num;
            }
            else {
                response += next_num;
            }
            prev_num = next_num;
        }
        printf ("\n*[%li]*\n", response);
        
        if (response == answer) {
            speak("CORRECT!");
        }
        else {
            sprintf(say, "Wrong. It's %li", answer);
            speak(say);
        }
        sleep(1.5);
    }
    speak("Ok. Enough number crunching for you.");
    return 1;
}

