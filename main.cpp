#include <iostream>
#include <queue>
#include <string>
#include <unordered_set>
#include <vector>
#include <chrono>

#include <curl/curl.h>
#include <rapidjson/document.h>


const std::string BASE = "http://hollywood-graph-crawler.bridgesuncc.org/neighbors/";



size_t write_callback(void* ptr, size_t size, size_t nmemb, void* userdata) {
    std::string* response = static_cast<std::string*>(userdata);
    response->append((char*)ptr, size * nmemb);
    return size * nmemb;
}


std::vector<std::string> getNeighbors(CURL* curl, const std::string& nodeName) {
    std::vector<std::string> neighbors;


    char* encoded = curl_easy_escape(curl, nodeName.c_str(), (int)nodeName.size());
    std::string url = BASE + encoded;
    curl_free(encoded);

    std::string body;

    // Setup curl
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);

    // Run the request
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "Curl failed: " << curl_easy_strerror(res) << "\n";
        return neighbors; // return empty
    }

    // JSON parsing
    rapidjson::Document doc;
    doc.Parse(body.c_str());

    if (doc.HasMember("neighbors") && doc["neighbors"].IsArray()) {
        for (auto& v : doc["neighbors"].GetArray()) {
            neighbors.push_back(v.GetString());
        }
    }

    return neighbors;
}


void bfs(const std::string& startNode, int maxDepth) {
    CURL* curl = curl_easy_init();


    std::unordered_set<std::string> visited;
    std::queue<std::string> q; // pair of node and depth

    q.push(startNode);
    visited.insert(startNode);

    auto t0 = std::chrono::steady_clock::now();

    for (int i = 0; i < maxDepth; i++) {



        // Fetch neighbors and enqueue unseen ones
        std::vector<std::string> neighbors = getNeighbors(curl, startNode);
        for (auto& nb : neighbors) {
            if (visited.find(nb) == visited.end()) {
                visited.insert(nb);
                q.push(nb);
            }
        }
    }

    auto t1 = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << "Visited " << visited.size() << " nodes in " << ms << " ms\n";

    for (int i = 0 ; i<q.size(); i++) {
        std::string s = q.front();
        q.pop();
        std::cout << s << "\n";
    }

    curl_easy_cleanup(curl);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " \"Start Node\" <depth>\n";
        return 1;
    }

    std::string startNode = argv[1];
    int depth = std::stoi(argv[2]);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    bfs(startNode, depth);
    curl_global_cleanup();

    return 0;
}
