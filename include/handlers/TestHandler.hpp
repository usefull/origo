#pragma once

#include "IHandler.hpp"

namespace epr = restinio::router::easy_parser_router;

namespace origo {

    class TestHandler : public IHandler {
    public:
        void Register(const std::unique_ptr<restinio::router::easy_parser_router_t> &&rawRouter) override {
            rawRouter->http_get(
                epr::path_to_params(
                    "/api/",
                    epr::non_negative_decimal_number_p<std::uint64_t>(),
                    "/title/",
                    epr::path_fragment_p()
                ),
                [](const auto& req, std::uint64_t id, const std::string& title) {                    
                    return req->create_response()
                        .set_body(fmt::format("id: {}, title: {}", id, title))
                        .done();
                }
            );
        }
    };
}