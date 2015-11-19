#include "http.h"


int request(char *buf)
{
	char *request = buf;
	char *Header_Field;
	char *method;
	int ret;
	struct http_request request_info;
	/* process the first line of http request */
	method = strtok_r(NULL, "\r\n", &request);
	ret = set_method(method, &request_info);
	if (ret)
	{
		perror("set method error");
		return(1);
	}
	/* process the following header fields */
	while(1){
		Header_Field = strtok_r(NULL, "\r\n", &request);
		if (Header_Field == NULL)
		{
			printf("end of http request\n");
			break;
		}
		ret = process_header(Header_Field, &request_info);
	}
	/* generate http response */
	/* http_response() */
	return 0;
}

/* set http method to http_request, return 1 if error */
int set_method(char *method, struct http_request *request_info)
{
	char *method_type;
	char *method_val;
	char *http_version;
	char *rest;
	int ret;

	method_type = strtok_r(method, " ", &rest);
	method_val = strtok_r(NULL, " ", &rest);
	(void*)strtok_r(rest, "/", &http_version);
	if (method_type&&method_val&&http_version)
	{
		if (strcmp(method_type, "GET") == 0)
			request_info->method_type = GET;
		if (strcmp(method_type, "HEAD") == 0)
			request_info->method_type = HEAD;
		if (strcmp(method_type, "POST") == 0)
			request_info->method_type = POST;
		request_info->request_URL = set_request(request_info->request_URL, method_val);
		request_info->http_version = set_request(request_info->http_version, http_version);
		return 0;
	}
	perror("set method error ");
	return 1;
}

/* process http request header field 
 * split header and value
 * return 1 if error 
 */
int process_header(char *Header_Field, struct http_request *request_info)
{
	char *header;
	char *header_value;
	header = strtok_r(Header_Field, ":", &header_value);
	if (header == NULL&&header_value == NULL)
	{
		perror("strtok_r header");
		return 1;
	}
	switch (to_num(header))
	{
	case 0:		/*Date*/
		request_info->HTTP_date = set_date(header_value);
		break;
	case 1:		/*If-Modified-Since*/
		request_info->if_modified_since = set_date(header_value);
		break;
	case 2:		/*Last-Modified*/
		request_info->last_modified = set_date(header_value);
		break;
	case 3:		/*Location*/
		request_info->absoluteURI = set_request(request_info->absoluteURI, header_value);
		break;
	case 4:		/*Server*/
		request_info->server = set_request(request_info->server, header_value);
		break;
	case 5:		/*User-Agent*/
		request_info->user_agent = set_request(request_info->user_agent, header_value);
		break;
	default:
		break;
	}
	return 0;
}

/* set string request value, return null if error */
char *set_request(char *request_type, char *request_val)
{
	request_type = (char *)malloc((strlen(request_val) + 1)*sizeof(char));
	if (request_type == NULL){
		perror("set_request malloc failed");
		return NULL;
	}
	strncpy(request_type, request_val, strlen(request_val) + 1);
	request_type[strlen(request_val)] = '\0';
	return request_type;
}

/* set tm from rfc850 format date, return 1 if error */
int set_rfc850(struct tm *http_date, char *request_val)
{
	time_t t_now = time(NULL);
	struct tm *gmt_now = gmtime(&t_now);
	char *rest;
	char *week;
	char *hour;
	char *minute;
	char *second;
	char *year;
	char *month;
	char *day;
	int n_week;
	int n_month;
	week = strtok_r(request_val, ",", &rest);
	day = strtok_r(NULL, "-", &rest);
	/* delete " " in day */
	strtok_r(NULL, " ", &day);
	month = strtok_r(NULL, "-", &rest);
	year = strtok_r(NULL, " ", &rest);
	hour = strtok_r(NULL, ":", &rest);
	minute = strtok_r(NULL, ":", &rest);
	second = strtok_r(NULL, " ", &rest);
	if (week&&day&&month&&year&&hour&&minute&&second)
	{
		if ((n_week = get_datenum(weekday, week)) == -1){
			perror("get date week");
			return 1;
		}
		if ((n_month = get_datenum(months, month)) == -1){
			perror("get date num month");
			return 1;
		}
		if (check_num(day) && check_num(year) && check_num(hour) && check_num(minute) && check_num(second))
		{
			http_date->tm_hour = atoi(hour);
			http_date->tm_min = atoi(minute);
			http_date->tm_sec = atoi(second);
			if (atoi(year) + 100 <= gmt_now->tm_year)
				http_date->tm_year = atoi(year) + 100;
			else
				http_date->tm_year = atoi(year);
			http_date->tm_mon = n_month;
			http_date->tm_mday = atoi(day);
			http_date->tm_wday = n_week;
			http_date->tm_isdst = 0;
			if (check_tm(http_date)){
				perror("check tm");
				return 1;
			}
			return 0;
		}
	}
	return 1;
}

/* set tm from rfc1123 format date, return 1 if error */
int set_rfc1123(struct tm *http_date, char *request_val)
{
	char *rest;
	char *week;
	char *hour;
	char *minute;
	char *second;
	char *year;
	char *month;
	char *day;
	int n_week;
	int n_month;
	//Date:Mon, 02 Jun 1982 23:59:59 GMT
	week = strtok_r(request_val, ",", &rest);
	day = strtok_r(NULL, " ", &rest);
	month = strtok_r(NULL, " ", &rest);
	year = strtok_r(NULL, " ", &rest);
	hour = strtok_r(NULL, ":", &rest);
	minute = strtok_r(NULL, ":", &rest);
	second = strtok_r(NULL, " ", &rest);
	if (week&&day&&month&&year&&hour&&minute&&second)
	{
		if ((n_week = get_datenum(wkday, week)) == -1)
			return 1;
		if ((n_month = get_datenum(months, month)) == -1)
			return 1;
		if (check_num(day) && check_num(year) && check_num(hour) && check_num(minute) && check_num(second))
		{
			http_date->tm_hour = atoi(hour);
			http_date->tm_min = atoi(minute);
			http_date->tm_sec = atoi(second);
			http_date->tm_year = atoi(year) - 1900;
			http_date->tm_mon = n_month;
			http_date->tm_mday = atoi(day);
			http_date->tm_wday = n_week;
			http_date->tm_isdst = 0;
			if (check_tm(http_date)){
				perror("check tm");
				return 1;
			}
			return 0;
		}
	}
	perror("strtok_r");
	return 1;
}

/* set tm from asctime format date, return 1 if error */
int set_asctime(struct tm *http_date, char *request_val)
{
	char *rest;
	char *week;
	char *hour;
	char *minute;
	char *second;
	char *year;
	char *month;
	char *day;
	int n_week;
	int n_month;
	//Date:Mon Jun  2 23:12:26 2010
	week = strtok_r(request_val, " ", &rest);
	month = strtok_r(NULL, " ", &rest);
	day = strtok_r(NULL, " ", &rest);
	hour = strtok_r(NULL, ":", &rest);
	minute = strtok_r(NULL, ":", &rest);
	second = strtok_r(NULL, " ", &rest);
	year = strtok_r(NULL, " ", &rest);

	if (week&&day&&month&&year&&hour&&minute&&second)
	{
		if ((n_week = get_datenum(wkday, week)) == -1)
			return 1;
		if ((n_month = get_datenum(months, month)) == -1)
			return 1;
		if (check_num(day) && check_num(year) && check_num(hour) && check_num(minute) && check_num(second))
		{
			http_date->tm_hour = atoi(hour);
			http_date->tm_min = atoi(minute);
			http_date->tm_sec = atoi(second);
			http_date->tm_year = atoi(year) - 1900;
			http_date->tm_mon = n_month;
			http_date->tm_mday = atoi(day);
			http_date->tm_wday = n_week;
			http_date->tm_isdst = 0;
			if (check_tm(http_date)){
				perror("check tm");
				return 1;
			}
			return 0;
		}
	}
	perror("strtok_r");
	return 1;
}

/*
rfc1123-date   = wkday "," SP date1 SP time SP "GMT"
Mon, 02 Jun 1982 23:59:59 GMT

rfc850-date    = weekday "," SP date2 SP time SP "GMT"
Monday, 02-Jun-82 23:59:59 GMT

asctime-date   = wkday SP date3 SP time SP 4DIGIT
Mon Jun  2 23:59:59 1982
*/
struct tm set_date(char *request_val)
{
	struct tm http_date;
	if (strstr(request_val, "GMT")){
		if (strstr(request_val, "-")){
			//rfc850 
			//Date:Monday, 02-Jun-02 23:54:07 GMT
			if (set_rfc850(&http_date, request_val))
				perror("set rfc850 error");
		}
		else{
			//rfc1123
			//Date:Mon, 02 Jun 1982 23:59:59 GMT
			if (set_rfc1123(&http_date, request_val))
				perror("set rfc1123 error");
		}
	}
	else{
		//asctime
		//Date:Mon Jun  2 23:12:26 2010
		if (set_asctime(&http_date, request_val))
			perror("set rfc1123 error");
	}
	return http_date;
}

/* transform header to category number, return -1 if error */
int to_num(char *header)
{
	char *hf_list[] = {
		"Date",
		"If-Modified-Since",
		"Last-Modified",
		"Location",
		"Server",
		"User-Agent"
	};
	int i = 0;
	for (i = 0; i < HEADER_FIELD; i++)
	{
		if (strcmp(header, hf_list[i]) == 0){
			return i;
		}
	}
	return -1;
}

/* if check_val is a number string, return 1 , else 0*/
int check_num(char *check_val)
{
	int i = 0;
	int ret = 1;
	for (i = 0; i < strlen(check_val); i++)
		isdigit(check_val[i]) > 0 ? (ret = ret * 1) : (ret = ret * 0);
	return ret;
}
/* get the index num of month and week, return -1 if error */
int get_datenum(char *date_list[], char *sub)
{
	int i = 0;
	for (i = 0; date_list[i] != NULL; i++)
		if (strcmp(date_list[i], sub) == 0)
			return i;
	return -1;
}

/* check tm value, return 1 if there is an error */
int check_tm(struct tm *http_data)
{
	if (http_data->tm_hour>23 && http_data->tm_hour < 0)
		return 1;
	if (http_data->tm_mday>31 && http_data->tm_mday < 1)
		return 1;
	if (http_data->tm_min>59 && http_data->tm_min < 0)
		return 1;
	if (http_data->tm_sec>59 && http_data->tm_sec < 0)
		return 1;
	if (http_data->tm_year < 0)
		return 1;
	return 0;
}