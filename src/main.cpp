#include <csignal>
#include <atomic>
#include <chrono>
#include <thread>

#include <restinio/all.hpp>
#include <nlohmann/json.hpp>
#include <fmt/core.h>

#include "../include/config.hpp"
#include "../include/messages.hpp"

#ifdef NDEBUG
    #define MODE "Release"
#else
    #define MODE "Debug"
#endif

using namespace std;
using json = nlohmann::json;
namespace err = origo::ErrorMessages;
namespace mess = origo::InfoMessages;

atomic<bool> stop_flag{false};

int main(int argc, char* argv[])
{
    const auto config = origo::Config::from_file(argc > 1 ? argv[1] : "origo.conf");

    using namespace restinio;
    using traits_t = restinio::default_traits_t;

    const auto cores = std::thread::hardware_concurrency();
    const auto pool_size = std::max(cores, 2u);

    restinio::http_server_t<traits_t> server
    {
        restinio::own_io_context(),
        restinio::server_settings_t<traits_t>{}
            .address(config.ip)
            .port(config.port)
            .request_handler([](auto req) {
                json response;
                response["message"] = "Hello from Origo!";
                response["status"] = "ok***";
                response["path"] = req->header().path();
                
                return req->create_response()
                    .append_header("Content-Type", "application/json")
                    .set_body(response.dump(4))
                    .done();
            })
    };

    restinio::on_pool_runner_t<restinio::http_server_t<traits_t>> runner
    {
        pool_size, 
        server
    };

    signal(SIGINT, [](int) { stop_flag = true; });
    signal(SIGTERM, [](int) { stop_flag = true; });

    runner.start();

    cout << fmt::format(mess::StartPrompt, MODE, config.port) << endl;
    cout << mess::CtrlC << endl;
    while(!stop_flag)
    {
        this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    cout << mess::StopSigRecivied << endl;
    
    runner.stop();
    runner.wait();
    
    cout << mess::StoppedSuccessfully << endl;
    
    return 0;
}