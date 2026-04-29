#include <restinio/all.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fmt/core.h>

#include "../include/config.hpp"
#include "../include/messages.hpp"

using namespace std;
using json = nlohmann::json;
namespace err = origo::ErrorMessages;
namespace mess = origo::InfoMessages;

int main(int argc, char* argv[]) {

    const auto configFileName = (argc > 1) ? argv[1] : "origo.conf";

    ifstream file(configFileName);
    if (!file)
    {
        throw runtime_error(fmt::format(err::CantOpenConfigFile, configFileName));
    }

    json jsonConfig;
    try {
        file >> jsonConfig;
    } catch (const json::parse_error& e) {
        throw runtime_error(fmt::format(err::ConfigFileReadingError, e.what()));
    }

    origo::Config config;
    config = origo::Config::from_json(jsonConfig);

    using namespace restinio;

#ifdef NDEBUG
    #define MODE "Release"
#else
    #define MODE "Debug"
#endif

    cout << fmt::format(mess::StartPrompt, MODE, config.port) << endl;

    run(
        on_this_thread()
            .port(config.port)
            .address(config.ip)
            .request_handler([](auto req) {
                json response;
                response["message"] = "Hello from Origo!";
                response["status"] = "ok+";
                response["path"] = req->header().path();
                
                return req->create_response()
                    .append_header("Content-Type", "application/json")
                    .set_body(response.dump(4))
                    .done();
            })
    );
    
    return 0;
}