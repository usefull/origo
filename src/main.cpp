#include <csignal>
#include <atomic>
#include <chrono>
#include <thread>

#include <restinio/all.hpp>
#include <restinio/router/easy_parser_router.hpp>
#include <nlohmann/json.hpp>
#include <fmt/core.h>

#include "../include/config.hpp"
#include "../include/messages.hpp"
#include "../include/router.hpp"

#ifdef NDEBUG
    #define MODE "Release"
#else
    #define MODE "Debug"
#endif

using namespace std;
using json = nlohmann::json;
namespace err = origo::ErrorMessages;
namespace mess = origo::InfoMessages;
//namespace epr = restinio::router::easy_parser_router;

atomic<bool> stop_flag{false};

struct my_traits : public restinio::default_traits_t {
    using request_handler_t = restinio::router::easy_parser_router_t;
};

int main(int argc, char* argv[])
{
    try
    {    
        const auto config = origo::Config::from_file(argc > 1 ? argv[1] : "origo.conf");

        using namespace restinio;
        //using traits_t = restinio::default_traits_t;

        const auto cores = std::thread::hardware_concurrency();
        const auto pool_size = std::max(cores, 2u);

        //auto router = std::make_unique<restinio::router::easy_parser_router_t>();

        origo::Router router;

        // restinio::http_server_t<traits_t> server
        // {
        //     restinio::own_io_context(),
        //     restinio::server_settings_t<traits_t>{}
        //         .address(config.ip)
        //         .port(config.port)
        //         .request_handler([](auto req) {
        //             json response;
        //             response["message"] = "Hello from Origo!";
        //             response["status"] = "ok***";
        //             response["path"] = req->header().path();
                    
        //             return req->create_response()
        //                 .append_header("Content-Type", "application/json")
        //                 .set_body(response.dump(4))
        //                 .done();
        //         })
        // };

        // router->http_get(
        //     epr::path_to_params(
        //         "/api/id/",
        //         epr::non_negative_decimal_number_p<std::uint64_t>(),
        //         "/title/",
        //         epr::path_fragment_p()
        //     ),
        //     [](const auto& req, std::uint64_t id, const std::string& title) {
        //         return req->create_response()
        //             .set_body(fmt::format("Book ID: {}, Title: {}", id, title))
        //             .done();
        //     }
        // );

        restinio::http_server_t<my_traits> server
        {
            restinio::own_io_context(),
            restinio::server_settings_t<my_traits>{}
                .address(config.ip)
                .port(config.port)
                .request_handler(router.get())
        };

        restinio::on_pool_runner_t<restinio::http_server_t<my_traits>> runner
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
    catch(const std::exception& e)
    {
        std::cerr << mess::FatalError << e.what() << endl;
        return EXIT_FAILURE;
    }
}