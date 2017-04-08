#include <iostream>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;
int main(){
    json o({});
    json temp;
    temp["uri"] = "53321";
    o["track"] = json::array();
    o["track"].push_back(temp);
    o["track"].push_back(temp);
    cout << temp;
    cout << o;
    // o.at("tracks").push
}
