#include <iostream>
#include <curl/curl.h>


std::vector<std::string> getNeighbors(CURL* curl, std::string& name) {


    std::string response;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, name.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    }

}

int main(int argc, char *argv[]) {

    CURL *curl = curl_easy_init();

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " must input 2 parameters" << std::endl;
    }



    return 0;

}