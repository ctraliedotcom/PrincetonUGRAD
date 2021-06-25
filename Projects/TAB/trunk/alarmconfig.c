#include "alarmconfig.h"

/*
struct Configuration {
	int alarm_hour;
	int alarm_minute;
	char* NewsURL;
	int rssnum;
	char* zipcode;
	char* name;
};*/

void fillDefaults(struct Configuration* config) {
	config->alarm_hour = -1; /*It's bad if these are -1; check for this*/
	config->alarm_minute = -1;
	config->NewsURL = "http://rss.news.yahoo.com/rss/topstories";
	config->rssnum = 10;
	config->zipcode = "08544";
	config->name = "Princeton University Student";
}

struct Configuration* getConfig(char* file) {
	struct Configuration* config = malloc(sizeof(struct Configuration));
	FILE* fref = fopen(file, "r");
	char buf[100];
	char* equals;
	char* value;
	int i = 0;
	fillDefaults(config);	
	while (!feof(fref)) {
		if (fgets(buf, 100, fref) == 0)
			continue;
		if (equals == -1 || strlen(buf) == 0)
			continue;
		if (buf[0] == '\n')
			continue;
		equals = strstr(buf, "=");
		value = malloc(strlen(equals + 1) + 1);
		strcpy(value, equals + 1);
		for (i = 0; i < strlen(value); i++) {
			if (value[i] == '\n')
				value[i] = ' ';
		}
		if (strncmp(buf, "alarm_hour", equals - buf - 1) == 0) {
			config->alarm_hour = atoi(value);
			free(value);
		}
		else if (strncmp(buf, "alarm_minute", equals - buf) == 0) {
			config->alarm_minute = atoi(value);
			free(value);
		}
		else if (strncmp(buf, "alarm_minute", equals - buf) == 0) {
			config->alarm_minute = atoi(value);
			free(value);
		}
		else if (strncmp(buf, "NewsURL", equals - buf) == 0) {
			config->NewsURL = value;
		}
		else if (strncmp(buf, "rssnum", equals - buf) == 0) {
			config->rssnum = atoi(value);
			free(value);
		}
		else if (strncmp(buf, "zipcode", equals - buf) == 0) {
			config->zipcode = value;
		}
		else if (strncmp(buf, "name", equals - buf) == 0) {
			config->name = value;
		}
		else {
			fprintf(stderr, "ERROR: Unknown config file option \"%s\" encountered\n", buf);
		}
	}
	return config;
}

void freeConfig(struct Configuration* config) {
	free(config->NewsURL);
	free(config->zipcode);
	free(config->name);
	free(config);
}

int getInfo(char* type) {	
	time_t rawtime;
	struct tm* timeinfo;
	char buffer[100];

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer,100,type,timeinfo);
	return atoi(buffer);
}

int getCurrentHour() {
	return getInfo("%H");
}

int getCurrentMinute() {
	return getInfo("%M");
}

void printInfo(struct Configuration* config) {
	printf("alarm_hour=%i;\n", config->alarm_hour);
	printf("alarm_minute=%i;\n", config->alarm_minute);
	printf("NewsURL=%s;\n", config->NewsURL);
	printf("rssnum=%i;\n", config->rssnum);
	printf("zipcode=%s;\n", config->zipcode);
	printf("name=%s;\n", config->name);
}
