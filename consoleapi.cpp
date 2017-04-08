#include "consoleapi.h"
#include <algorithm>
#include <iostream>

ConsoleApi::ConsoleApi(string tok){
    auth += tok;
}

bool ConsoleApi::insertSong(string songId){
    librarySongs.push_back(songId);
    // std::cout << songId << std::endl;
    return true;
}
bool ConsoleApi::findSong(string songId){
    std::vector<string>::iterator it;
    it = find(librarySongs.begin(), librarySongs.end(), songId);
    if(it == librarySongs.end()) return false;
    else return true;
    // return it;
}
void ConsoleApi::setPlaylist(string id){
    playlistID = id;
}

void ConsoleApi::setOwner(string id){
    ownerID = id;
}

const char* ConsoleApi::getMime(){
    return mime.c_str();
}

const char* ConsoleApi::getAuth(){
    return auth.c_str();
}
const char* ConsoleApi::getUrl(){
    return url.c_str();
}
const char* ConsoleApi::getPlaylistUrl(){
    playlistUrl += ownerID + "/playlists/" + playlistID + "/tracks";
    return playlistUrl.c_str();
}
