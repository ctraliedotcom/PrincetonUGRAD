#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#ifndef H_CONFIG_H
#define H_CONFIG_H

struct Configuration {
	int alarm_hour; /*Hour to wake user up (0-23)*/
	int alarm_minute; /*Minute to wake user up*/
	char* NewsURL; /*URL for RSS feed for news*/
	int rssnum; /*Max number of RSS feed items to read*/
	char* zipcode; /*Zipcode for obtaining weather*/
	char* name; /*Name of user*/
};

struct Configuration* getConfig(char* file);

void freeConfig(struct Configuration* config);

int getCurrentHour();

int getCurrentMinute();

void printInfo(struct Configuration* config);

#endif
