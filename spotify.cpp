#include <iostream>
#include <curl/curl.h>
#include <iomanip>
#include <string>
#include <vector>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

std::vector<string> mainLibrary;
std::vector<string> duplicates;
CURL* rCurl;
CURL* dCurl;
bool error = false;
struct Headers{
	string auth = "Authorization: Bearer ";
	string mime = "Accept: application/json";
    string url = "https://api.spotify.com/v1/me/tracks?";
};

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

json createDelObj(unsigned int start, unsigned int amount, std::vector<string>* cont){
	if(amount > 100 || amount <=0){
		cerr << "Too big request for json list" << endl;
		return NULL;
	}else{
		json ret({});
		for(unsigned int i = 0; i<amount;i++){
			json buffer;
			buffer["uri"] = "spotify:track:"+cont->at(start+i);
			ret["track"].push_back(buffer);
		}
		return ret;
	}
}

bool findDuplicate(string id){
	std::vector<string>::iterator it;
    it = find(mainLibrary.begin(), mainLibrary.end(), id);
	if(it == mainLibrary.end()) return false;
    else return true;
}

bool responseManip(string* response, std::vector<string>* cont, bool compare){
	json obj;
	if(response->length() > 2){
		obj = json::parse(*response);
		response->clear();
		auto error = obj.find("error");
		if(error != obj.end()){
			std::cerr << "Error response" << '\n';
			std::cerr << obj << '\n';
			return false;
		}else{
			//Reading response
			if(!obj.at("items").empty()) {
				if(!compare){
					for(unsigned int i=0; i<obj.at("items").size(); i++) {
						cont->push_back(obj.at("items").at(i).at("track").at("id"));
					}
				}else{
					for(unsigned int i=0; i<obj.at("items").size(); i++) {
						if(findDuplicate(obj.at("items").at(i).at("track").at("id")))
						cont->push_back(obj.at("items").at(i).at("track").at("id"));
					}
				}
				if(!obj.at("next").is_null()) {
					string next = obj.at("next");
					curl_easy_setopt(rCurl, CURLOPT_URL,next.c_str());
					return true;
				}else{
					std::cerr << "next = null" << '\n';
					return false;
				}
			}else{
				std::cerr << "Empty object" << '\n';
				return false;
			}
		}
	}else{
		std::cerr << "Too short response" << '\n';
		return false;
	}
}

int main(int argc, char* argv[]){
	Headers lHeaders;
	Headers pHeaders;
	if(argc > 3){
		string auth = argv[1];
		string login = argv[2];
		string playlistID = argv[3];
		lHeaders.auth += auth;
		pHeaders.auth += auth;
		pHeaders.url = "https://api.spotify.com/v1/users/" + login
							+ "/playlists/" + playlistID + "/tracks";
	}else{
		std::cerr << "You need to pass parameters \n Required: \n oauth \n playlist owner login \n playlist ID" << '\n';
		return 10;
	}
	struct curl_slist *params = NULL;
	string readBuffer;
	string* rBPointer = &readBuffer;
	CURLcode res;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	rCurl = curl_easy_init();
	if(rCurl) {
		params = curl_slist_append(params, lHeaders.mime.c_str());
		params = curl_slist_append(params, lHeaders.auth.c_str());
		curl_easy_setopt(rCurl, CURLOPT_HTTPHEADER, params);
		curl_easy_setopt(rCurl, CURLOPT_URL,lHeaders.url.c_str());
		curl_easy_setopt(rCurl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(rCurl, CURLOPT_WRITEDATA, &readBuffer);
		curl_easy_setopt(rCurl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(rCurl, CURLOPT_SSL_VERIFYHOST, 0L);
		//Populate mainLibrary vector
		std::vector<string>* libraryPtr;
		libraryPtr = &mainLibrary;
		do{
			res = curl_easy_perform(rCurl);
			if(res != CURLE_OK) {
				std::cerr << "curl_easy_perform() failed: %s\n" << curl_easy_strerror(res);
				break;
			}
		}while(responseManip(rBPointer, libraryPtr, false));
		// for(auto lel : mainLibrary){
		// 	cout << lel <<endl;
		// }
		curl_easy_setopt(rCurl, CURLOPT_URL,pHeaders.url.c_str());
		libraryPtr = &duplicates;
		do{
			res = curl_easy_perform(rCurl);
			if(res != CURLE_OK) {
				std::cerr << "curl_easy_perform() failed: %s\n" << curl_easy_strerror(res);
				break;
			}
		}while(responseManip(rBPointer, libraryPtr, true));
		for(auto lel : duplicates){
			cout << lel <<endl;
		}

		//Remove duplicates
		CURLcode delReturn;
		dCurl = curl_easy_init();
		curl_easy_setopt(dCurl, CURLOPT_CUSTOMREQUEST, "DELETE");
		curl_easy_setopt(dCurl, CURLOPT_URL, pHeaders.url.c_str());
		struct curl_slist *dParams = NULL;
		dParams = curl_slist_append(params, lHeaders.mime.c_str());
		dParams = curl_slist_append(params, lHeaders.auth.c_str());
		curl_easy_setopt(dCurl, CURLOPT_HTTPHEADER, dParams);

		//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "name=daniel&project=curl");
		unsigned int amount = libraryPtr->size();
		unsigned int tens = amount%100;
		cout << "TENS" << tens << "AMOUNT" << amount << endl;
		string temp;
		for(unsigned int i = 0; i<amount; i+=100){
			if(i+100 > amount){
				temp = createDelObj(i, tens,libraryPtr).dump();
			}
			else{
				temp = createDelObj(i, i+100,libraryPtr).dump();
			}
			curl_easy_setopt(dCurl, CURLOPT_POSTFIELDS, temp.c_str());
			delReturn = curl_easy_perform(rCurl);
			if(delReturn != CURLE_OK) {
				std::cerr << "curl_easy_perform() failed: %s\n" << curl_easy_strerror(res);
				break;
			}
		}
		curl_slist_free_all(params);
		curl_slist_free_all(dParams);
		curl_easy_cleanup(rCurl);
		curl_easy_cleanup(dCurl);
	}
	curl_global_cleanup();
	return 0;
}
