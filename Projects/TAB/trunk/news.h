#ifndef H_NEWS_H
#define H_NEWS_H

#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include <mrss.h>
#include "speak.h"

/*NOTE: This requires libmrss0-dev, which is a C library
 *used to read RSS feeds*/

void readHeadlines(char* url, int num_headlines);

#endif
