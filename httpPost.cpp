#include "httpPost.h"

struct curl_httppost *formPost = NULL;
struct curl_httppost  *lastPtr = NULL;

void createPostFrom(const char *pathToFile, const char *fileName, const char* about)
{
	if(strcmp(pathToFile, "") == 0 || strcmp(fileName, "") == 0 || strcmp(about,  "") == 0)
	{
		printf("ERROR: parameters cannot be empty\n");
		printf("\t%s\n\t%s\n\t%s\n", pathToFile, fileName, about);
		exit(-1);
	}
	
	curl_formadd(&formPost, 
				 &lastPtr,
				 CURLFORM_COPYNAME, "pictures",
				 CURLFORM_FILE, pathToFile,
				 CURLFORM_END);
				 
	curl_formadd(&formPost,
               &lastPtr,
               CURLFORM_COPYNAME, "filename",
               CURLFORM_COPYCONTENTS, fileName,
               CURLFORM_END);
               
	curl_formadd(&formPost,
               &lastPtr,
               CURLFORM_COPYNAME, "about",
               CURLFORM_COPYCONTENTS, about,
               CURLFORM_END);

	
	  curl_formadd(&formPost,
               &lastPtr,
               CURLFORM_COPYNAME, "submit",
               CURLFORM_COPYCONTENTS, "submit",
               CURLFORM_END);
}

void submitFile(const char* httpUrl)
{
	CURL *curl;
	CURLcode res;	
	struct curl_slist *headerList  = NULL;
	static const char buffer[] = "Expect";


	if(strcmp(httpUrl,  "") == 0)
	{
		printf("ERROR: Parameters cannot be empty!\n");
		exit(-1);
	}
	
	curl_global_init(CURL_GLOBAL_ALL);	
	
	curl = curl_easy_init();		
		
	headerList = curl_slist_append(headerList, buffer);
		
	if(curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, httpUrl);
			
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formPost);
			
		res = curl_easy_perform(curl);
			
		curl_easy_cleanup(curl);
		curl_formfree(formPost);
		curl_slist_free_all(headerList);
		
		}
}
