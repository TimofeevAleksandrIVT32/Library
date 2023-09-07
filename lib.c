#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include "lib.h"

// функция для выполнения HTTP запроса
int http_request(char *location) {
    CURLcode res;
    CURL *curl_handle;

    memory_struct chunk;
    chunk.memory = NULL;
    chunk.size = 0;
    
    chunk.memory = malloc(1);
    if (!chunk.memory) {
        printf("Memory allocation error\n");
        return 1;
    }
    res = curl_global_init(CURL_GLOBAL_ALL);
    if (res != CURLE_OK) {
        printf("Error when organizing the use of libcurl resources\n");
        free(chunk.memory);
        return 1;
    }
    curl_handle = curl_easy_init();
    if (!curl_handle) {
        printf("Error initializing curl session\n");
        curl_global_cleanup();
        free(chunk.memory);
        return 1;
    }

    char result_str[STR_SIZE] = "https://wttr.in/";
    char end[STR_SIZE] = "?format=j1";
    get_str(result_str, location, end);

    curl_easy_setopt(curl_handle, CURLOPT_URL, result_str);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    res = curl_easy_perform(curl_handle);
    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();

    if(res != CURLE_OK) {
        printf("An error occurred while making an HTTP request, or the city does not exist\n");
        printf("Let's try to take information from the file\n");
        int er = parsing_json(NULL, location);
        if (er) {
            free(chunk.memory);
            return 1;
        }
    }
    else {
        int er = parsing_json(chunk.memory, location);
        if (er) {
            free(chunk.memory);
            return 1;
        }
    }
    free(chunk.memory);
    return 0;
}

// функция для склеивания нескольких строк
void get_str(char *res, char *insert, char *end) {
    strcat(res,insert);
    strcat(res, end);
}

// функция для загрузки данных из сети в память
size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    memory_struct *mem = (memory_struct *)userp;
    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(!ptr) {
        printf("Memory allocation error\n");
        return 0;
    }
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

// функция парсинга json
int parsing_json(char *data, char *location) {
    json_object *json;
    if (!data) {
        char file_name[STR_SIZE] = "";
        char end[STR_SIZE] = ".json";
        get_str(file_name, location, end);
        FILE *file = fopen(file_name, "rb");
        if (!file) {
            printf("The file could not be opened or the file does not exist\n");
            return 1;
        }
        json = json_object_from_file(file_name);
        fclose(file);
    } else {
        json = json_tokener_parse(data);
    }
    if (!json) {
        printf("Error in line json or insufficient memory\n");
        return 1;
    }
        
    json_object *current_condition;
    int exists = json_object_object_get_ex(json,"current_condition",&current_condition);
    if (!exists) {
        printf("Object does not exist\n");
        json_object_put(json);
        return 1;
    }
        
    json_object *first_element = json_object_array_get_idx(current_condition,0);
    if (!first_element) {
        printf("Object does not exist\n");
        json_object_put(json);
        return 1;
    }

    json_object *temp_C;
    exists = json_object_object_get_ex(first_element,"temp_C",&temp_C);
    if (!exists) {
        printf("Object does not exist\n");
        json_object_put(json);
        return 1;
    }
    char temperature[STR_SIZE];
    strcpy(temperature, json_object_get_string(temp_C));
    get_str(temperature," °C","");

    json_object *winddir16Point;
    exists = json_object_object_get_ex(first_element,"winddir16Point",&winddir16Point);
    if (!exists) {
        printf("Object does not exist\n");
        json_object_put(json);
        return 1;
    }
    char wind_dir[STR_SIZE];
    strcpy(wind_dir, json_object_get_string(winddir16Point));

    json_object *windspeedKmph;
    exists = json_object_object_get_ex(first_element,"windspeedKmph",&windspeedKmph);
    if (!exists) {
        printf("Object does not exist\n");
        json_object_put(json);
        return 1;
    }
    char wind_speed[STR_SIZE];
    strcpy(wind_speed, json_object_get_string(windspeedKmph));
    get_str(wind_speed," km/h","");

    json_object *weather;
    exists = json_object_object_get_ex(first_element,"weatherDesc",&weather);
    if (!exists) {
        printf("Object does not exist\n");
        json_object_put(json);
        return 1;
    }

    first_element = json_object_array_get_idx(weather,0);
    if (!first_element) {
        printf("Object does not exist\n");
        json_object_put(json);
        return 1;
    }

    json_object *weather_desc;
    exists = json_object_object_get_ex(first_element,"value",&weather_desc);
    if (!exists) {
        printf("Object does not exist\n");
        json_object_put(json);
        return 1;
    }
    char weather_description[STR_SIZE];
    strcpy(weather_description, json_object_get_string(weather_desc));
    print(location, weather_description, temperature, wind_dir, wind_speed);
    json_object_put(json);
    return 0;
}

// функция вывода данных на экран
void print(char *location, char *weather_description, char *temperature, char *wind_dir, char *wind_speed) {
    printf("***************************************\n");
    printf("* Weather in %-*s     *\n", 20, location);
    printf("***************************************\n");
    printf("* Description: %-*s   *\n", 20, weather_description);
    printf("* Temperature: %-*s    *\n", 20, temperature);
    printf("* Wind direction: %-*s*\n", 20, wind_dir);
    printf("* Wind speed: %-*s    *\n", 20, wind_speed);
    printf("***************************************\n");
}
