#include <stdio.h>
#include <iostream>
#include <curl/curl.h>
#include <iomanip>
#include <vector>
#include "json.hpp"
#include "consoleapi.h"

using namespace std;
using json = nlohmann::json;

size_t stringify(void *content, size_t size, size_t mem, void *userdata);

int main(int argc, char* argv[])
{
	if(argc < 2) {
		cout << "BRAK PARAMETROW" << endl;
		cout << "Podaji auth token" << endl;
	}else{
		ConsoleApi songs(argv[1]);
		songs.setOwner(argv[2]);
		songs.setPlaylist(argv[3]);
		json o;
		struct curl_slist *params = NULL;
		string response;
		CURL *curl;
		CURLcode res;
		curl_global_init(CURL_GLOBAL_DEFAULT);
		curl = curl_easy_init();
		if(curl) {
			params = curl_slist_append(params, songs.getMime());
			params = curl_slist_append(params, songs.getAuth());
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, params);
			curl_easy_setopt(curl, CURLOPT_URL,songs.getUrl());
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
						response.clear();
						if(!o.at("items").empty()) {
							for(unsigned int i=0; i<o.at("items").size(); i++) {
								songs.insertSong(o.at("items").at(i).at("track").at("id"));
							}
							string next;
							if(!o.at("next").is_null()) {
								next = o.at("next");
								curl_easy_setopt(curl, CURLOPT_URL,next.c_str());
							}
						}
					}
				}
			} while(!o.at("next").is_null());

			//Look for duplicates in playlist by comparison with librarySongs
			curl_easy_setopt(curl, CURLOPT_URL,songs.getPlaylistUrl());
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
					response.clear();
                    auto err = o.find("error");
                    if(err == o.end()){
						if(!o.at("items").empty()) {
							for(unsigned int i=0; i<o.at("items").size(); i++) {
								//if(o.at("items").at(i).at("is_local") == true) continue;

								if(songs.findSong(o.at("items").at(i).at("track").at("id"))){
									songs.playlistDuplicates.push_back(o.at("items").at(i).at("track").at("id"));
									cout << "Duplikat" << endl;
								}
							}
							string next;
							if(!o.at("next").is_null()) {
								next = o.at("next");
								curl_easy_setopt(curl, CURLOPT_URL,next.c_str());
							}
						}
					}
				}
			} while(!o.at("next").is_null());
			/* always cleanup */
			curl_easy_cleanup(curl);
		}

		curl_global_cleanup();
	}
	return 0;
}

size_t stringify(void *content, size_t size, size_t mem, void *userdata){
	size_t wholesize = size* mem;
	((string*)userdata)->append((char*)content, wholesize);
	//cout << wielkosc;
	return wholesize;
}

/*
{ "tracks":[
	{ "uri": "spotify:track:4iV5W9uYEdYUVa79Axb7Rh" },
	{ "uri": "spotify:track:1301WleyT98MSxVHPZCA6M" }]
}


*/
