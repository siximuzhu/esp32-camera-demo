#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
//#include "protocol_examples_common.h"
#include "esp_tls.h"
#include "esp_crt_bundle.h"

#include "esp_http_client.h"
#include "cJSON.h"
//#include "base64.h"


#define MAX_HTTP_RECV_BUFFER 2048
#define MAX_HTTP_OUTPUT_BUFFER 2048
static const char *TAG = "HTTP_CLIENT";


int response_count = 0;
char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};

esp_err_t _http_event_handle(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
            ESP_LOGI(TAG,"%.*s", evt->data_len, (char*)evt->data);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
    //        if (!esp_http_client_is_chunked_response(evt->client)) {
     //           ESP_LOGI(TAG,"%.*s", evt->data_len, (char*)evt->data);
				memcpy(local_response_buffer + response_count,evt->data,evt->data_len);
				response_count += evt->data_len;
   //         }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}


extern char * base64_encode(const void *src, size_t len, size_t *out_len);


int get_token(char *access_token)
{
	int len;
	const cJSON *token = NULL;

	const char *token_url = "https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials&client_id=f0ZCCvt5ST5UMUP6XUQMST0z&client_secret=WOF6r0AAUu6wlR2U24w2mXCxeq9N7tIr";
	//const char *token_url = "https://www.baidu.com";
	esp_http_client_config_t config = {
		.url = token_url,
		.event_handler = _http_event_handle,
//		.user_data = local_response_buffer,
		.method = HTTP_METHOD_GET,
		};

	esp_http_client_handle_t client = esp_http_client_init(&config);
//	esp_http_client_set_url(client,token_url);
//	esp_http_client_set_method(client,HTTP_METHOD_GET);
	esp_err_t err = esp_http_client_perform(client);

	if(err == ESP_OK)
	{
		len = esp_http_client_get_content_length(client);
		ESP_LOGI(TAG,"HTTP GET STATUS = %d,len = %d",esp_http_client_get_status_code(client),len);
	//	ESP_LOGI(TAG,"%s",local_response_buffer);
	}
	else
	{
		ESP_LOGI(TAG,"HTTP GET FAILED:%s",esp_err_to_name(err));
	}

	cJSON *token_json = cJSON_Parse(local_response_buffer);
	if(NULL == token_json)
	{
		ESP_LOGI(TAG,"JSON error");
	}
	else
	{
		token = cJSON_GetObjectItemCaseSensitive(token_json, "access_token");

	//	ESP_LOGI(TAG,"%s",token->valuestring);

		strcpy(access_token,token->valuestring);
	}
	
	memset(local_response_buffer,0x0,MAX_HTTP_OUTPUT_BUFFER);
	response_count = 0;

	esp_http_client_cleanup(client);

	return 0;


	
}

int upload_image(const unsigned char *image_src,unsigned int image_len)
{
	char access_token[100] = {0};
	char* image_base64;
	cJSON *image_json;
	cJSON *image_base64_json;
	cJSON *access_token_json;
	char *string;
	int len;

	get_token(access_token);

	ESP_LOGI(TAG,"--%s",access_token);

//	char *p = "luozhu";
	size_t out_len;

	image_base64 = base64_encode(image_src,image_len,&out_len);

	if(image_base64 != NULL)
	{
		ESP_LOGI(TAG,"image_base64:%s",image_base64);
		//free(image_base64);
	}
	else
	{
		ESP_LOGI(TAG,"base64 error");
	}

	image_json = cJSON_CreateObject();
	access_token_json = cJSON_CreateString(access_token);
	image_base64_json = cJSON_CreateString(image_base64);
	cJSON_AddItemToObject(image_json,"access_token",access_token_json);
	cJSON_AddItemToObject(image_json,"image_base64",image_base64_json);
	
	string = cJSON_Print(image_json);

	ESP_LOGI(TAG,"string:%s",string);




		const char *iamge_post_url = "http://yida.qjun.com.cn:8078/getAudioUrl";
	esp_http_client_config_t config = {
		.url = iamge_post_url,
		.event_handler = _http_event_handle,
//		.user_data = local_response_buffer,
		.method = HTTP_METHOD_POST,
		};

	esp_http_client_handle_t client = esp_http_client_init(&config);
//	esp_http_client_set_url(client,token_url);
//	esp_http_client_set_method(client,HTTP_METHOD_GET);
	esp_http_client_set_header(client, "Content-Type", "application/json");
	esp_http_client_set_post_field(client, string, strlen(string));
	
	esp_err_t err = esp_http_client_perform(client);

	if(err == ESP_OK)
	{
		len = esp_http_client_get_content_length(client);
		ESP_LOGI(TAG,"HTTP GET STATUS = %d,len = %d",esp_http_client_get_status_code(client),len);
		ESP_LOGI(TAG,"%s",local_response_buffer);
		memset(local_response_buffer,0x0,MAX_HTTP_OUTPUT_BUFFER);
		response_count = 0;

	}
	else
	{
		ESP_LOGI(TAG,"HTTP GET FAILED:%s",esp_err_to_name(err));
	}

	esp_http_client_cleanup(client);

	return 0;
}

