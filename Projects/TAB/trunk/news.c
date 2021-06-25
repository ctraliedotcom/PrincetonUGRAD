#include "news.h"

/*gchar* getYahoo() {
 	gchar* toReturn;
 	gchar* command = "curl http://www.yahoo.com";
 	g_spawn_command_line_sync(command, &toReturn, NULL, NULL, NULL);
 	return toReturn;
}*/

/*Look at the header mrss.h for more info on these structs*/
void readHeadlines(char* url, int num_headlines) {
    int i = 0;
	mrss_t* rssdata;
	mrss_error_t ret; /*Use this later for error detection*/
	mrss_item_t* item;	

	ret = mrss_parse_url(url, &rssdata);

	item = rssdata->item;
	while (item && (i < num_headlines))
	{
		speak(item->title);	
		item = item->next;
        i++;
	}
	mrss_free (rssdata);
}
