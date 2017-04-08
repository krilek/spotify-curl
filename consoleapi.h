#include <string>
#include <vector>
using std::string;
class ConsoleApi{
private:
    string ownerID, playlistID;
    string offset;
    std::vector<string> librarySongs;
    string auth = "Authorization: Bearer ";
    string mime = "Accept: application/json";
    string url = "https://api.spotify.com/v1/me/tracks?";
    string playlistUrl = "https://api.spotify.com/v1/users/";
public:
    std::vector<string> playlistDuplicates; 
    ConsoleApi(string tok);
    bool insertSong(string songId);
    void setPlaylist(string id);
    void setOwner(string id);
    const char* getMime();
    const char* getAuth();
    bool findSong(string songId);
    const char* getUrl();
    const char* getPlaylistUrl();
};
