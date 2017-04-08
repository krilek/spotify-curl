#include <stdio.h>
#include <iostream>
#include <curl/curl.h>
#include <iomanip>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;
static size_t stringify(void *content, size_t size, size_t mem, void *userdata){
	size_t wholesize = size* mem;
	((string*)userdata)->append((char*)content, wholesize);
	//cout << wielkosc;
	return wholesize;
}
int main(int argc, char* argv[])
{
	if(argc < 2) {
		cout << "BRAK PARAMETROW" << endl;
		cout << "Podaj offset i auth token" << endl;
	}else{
		string mime = "Accept: application/json";
		string oauth = argv[2];
		string offset = argv[1];
		string url = "https://api.spotify.com/v1/me/tracks?offset=" + offset;
		json o;
		struct curl_slist *params = NULL;
		string response;
		CURL *curl;
		CURLcode res;
		curl_global_init(CURL_GLOBAL_DEFAULT);
		curl = curl_easy_init();
		if(curl) {
			params = curl_slist_append(params, mime.c_str());
			params = curl_slist_append(params, ("Authorization: Bearer "+oauth).c_str());
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, params);
			curl_easy_setopt(curl, CURLOPT_URL,url.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, stringify);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

			do {
				/* Perform the request, res will get the return code */
				res = curl_easy_perform(curl);
				/* Check for errors */
				if(res != CURLE_OK) {
					fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
					break;
				}
				else{
					o= json::parse(response);
                    //cout << o;
                    auto err = o.find("error");
                    if(err == o.end()){
                        cout << "Kupa";
                    }
                    // cout << *err;
					response.clear();
					if(!o.at("items").empty()) {
						for(unsigned int i=0; i<1/*o.at("items").size()*/; i++) {
							cout << o.at("items").at(i).at("track").at("name") << endl;
						}
						string test;
						if(!o.at("next").is_null()) {
							test = o.at("next");
							curl_easy_setopt(curl, CURLOPT_URL,test.c_str());
						}
					}
				}
			} while(!o.at("next").is_null());
            // do {
            // 	/* Perform the request, res will get the return code */
            // 	res = curl_easy_perform(curl);
            // 	/* Check for errors */
            // 		o= json::parse(response);
            //         //cout << o;
            //
            //             cout << *err;
            // 		response.clear();
            // } while(true);
			/* always cleanup */
			curl_easy_cleanup(curl);
		}

		curl_global_cleanup();
	}
	return 0;
}


//Potrzebuje
//-x
//-h
