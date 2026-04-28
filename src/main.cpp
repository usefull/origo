#include <restinio/all.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

int main() {

    std::ifstream file("origo.conf");
    if (!file)
    {
        throw std::runtime_error("Не удалось открыть файл конфигурации origo.conf");
    }

    json config;
    try {
        file >> config;
    } catch (const json::parse_error& e) {
        throw std::runtime_error("Ошибка чтения файла конфигурации");
    }

    using namespace restinio;

#ifdef NDEBUG
    std::cout << "Origo is running in RELEASE mode..." << std::endl;
#else
    std::cout << "Origo is running in DEBUG mode..." << std::endl;
#endif

    std::cout << "Starting HTTP server on port " << config["port"] << "..." << std::endl;

    run(
        on_this_thread()
            .port(config["port"])
            .address(config["ip"])
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