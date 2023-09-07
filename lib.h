#ifndef LIB_H
#define LIB_H

#define STR_SIZE 50

typedef struct memory_struct {
    char *memory;
    size_t size;
} memory_struct;

int http_request(char *location);
void get_str(char *result_str, char *insert, char *end);
size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userp);
int parsing_json(char *data, char *location);
void print(char *location, char *weather_description, char *temperature, char *wind_dir, char *wind_speed);

#endif /* LIB_H */
