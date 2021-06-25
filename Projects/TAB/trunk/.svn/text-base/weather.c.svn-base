#include "weather.h"

/*http://rss.weather.com/weather/rss/local/08544?cm_ven=LWO&cm_cat=rss&par=LWO_rss*/

/*Give this a zip code as a string*/
void sayWeather(gchar* zip) {
	gchar* url = g_strconcat("http://rss.weather.com/weather/rss/local/", 
									zip, "?cm_ven=LWO&cm_cat=rss&par=LWO_rss", NULL);
	gchar* location;
	gchar* begin;
	gchar* end;
	gchar* temperature;
	gchar* saytemperature;	
	int i = 0;

	mrss_t* rssdata;
	mrss_error_t ret;
	mrss_item_t* item;
	ret = mrss_parse_url(url, &rssdata);

	item = rssdata->item;

	location = item->title;
	begin = strstr(item->description, ">");	
	if (begin == NULL)
		return;
	begin++;
	end = strstr(begin, "&");
	if (end == NULL)
		return;
	temperature = (gchar*)calloc(end - begin, 1);
	strncpy(temperature, begin, end - begin);
	saytemperature = g_strconcat("The outdoor temperature is ", temperature, " degrees Fahrenheit", NULL);	
	speak(location);
	speak(saytemperature);
	

	mrss_free(rssdata);
	free(temperature);
	free(saytemperature);
	free(url);
}

/*http://www.cplusplus.com/reference/clibrary/ctime/strftime.html*/
void sayTime() {
	time_t rawtime;
	struct tm* timeinfo;
	char buffer[100];

	time (&rawtime);
	timeinfo = localtime(&rawtime);

	strftime (buffer,100,"It is %I:%M%p, %A %B %d %Y",timeinfo);
	speak(buffer);
}
