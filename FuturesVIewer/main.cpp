#include <iostream>
#include <string>
#define CURL_STATICLIB
#include "curl/curl.h"
#include <nlohmann/json.hpp>
#include <chrono>

using json = nlohmann::json;

#ifdef _DEBUG
#    pragma comment (lib,"libcurl_a_debug.lib")
#else
#    pragma comment (lib,"libcurl_a.lib")
#endif // _DEBUG

#pragma comment (lib,"Normaliz.lib")
#pragma comment (lib,"Ws2_32.lib")
#pragma comment (lib,"Wldap32.lib")
#pragma comment (lib,"Crypt32.lib")

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

const std::string symbol = "BTCUSDT"; // Adjust the symbol as needed

int main() {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        // Specify the URL for the aggTrades endpoint
        std::string url = "https://fapi.binance.com/fapi/v1/aggTrades?symbol=" + symbol; 
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); 

        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        else {
            // Parse the JSON response
            auto jsonData = json::parse(readBuffer);

            // Iterate through the JSON array and print each trade
            for (const auto& trade : jsonData) {
                auto start = std::chrono::high_resolution_clock::now();
                std::cout << "{\n"
                    << "  \"a\": " << trade["a"] << ",\n"  
                    << "  \"p\": " << trade["p"] << ",\n" 
                    << "  \"q\": " << trade["q"] << ",\n" 
                    << "  \"f\": " << trade["f"] << ",\n"   
                    << "  \"l\": " << trade["l"] << ",\n"   
                    << "  \"T\": " << trade["T"] << ",\n"   
                    << "  \"m\": " << std::boolalpha << trade["m"] << "\n"
                    << "}\n";

                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::micro> duration = end - start;
                std::cout << "Parsing time for trade ID " << trade["a"] << ": " << duration.count() << " microseconds\n";
            }
        }

        // Cleanup
        curl_easy_cleanup(curl);
    }
    return 0;
}

