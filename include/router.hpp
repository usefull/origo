#pragma once

#include <memory>

#include "./handlers/TestHandler.hpp"
#include "./handlers/StubHandler.hpp"

namespace epr = restinio::router::easy_parser_router;

namespace origo {    

    class Router {
    public:
        Router() {
            m_pRouter = std::make_unique<restinio::router::easy_parser_router_t>();
            RegisterHandlers();
        }

        constexpr std::unique_ptr<restinio::router::easy_parser_router_t> &&get() noexcept { return std::move(m_pRouter); }

    private:
        std::unique_ptr<restinio::router::easy_parser_router_t> m_pRouter;
        std::vector<std::unique_ptr<IHandler>> m_pHandlers;

        std::unique_ptr<origo::TestHandler> _pTestHandler;

        void RegisterHandlers() {
            
            Register<origo::TestHandler>();
            Register<origo::StubHandler>();
            // _pRouter->http_get(
            //     epr::path_to_params(
            //         "/api/",
            //         epr::non_negative_decimal_number_p<std::uint64_t>(),
            //         "/title/",
            //         epr::path_fragment_p()
            //     ),
            //     [](const auto& req, std::uint64_t id, const std::string& title) {                    
            //         return req->create_response(restinio::http_status_line_t{
            //             restinio::http_status_code_t{299}//, 
            //             //"I'm a Teapot" // Custom reason phrase
            //         })
            //             .set_body(fmt::format("ID: {}, Title: {}", id, title))
            //             .done();
            //     }
            // );
        }

        template <typename THandler>
        void Register() {
            auto h = std::make_unique<THandler>();
            h->Register(std::move(m_pRouter));
            m_pHandlers.push_back(std::move(h));
        }
    };
}