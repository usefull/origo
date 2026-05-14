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

        void RegisterHandlers() {
            
            Register<origo::TestHandler>();
            Register<origo::StubHandler>();
        }

        template <typename THandler>
        void Register() {
            auto h = std::make_unique<THandler>();
            h->Register(m_pRouter.get());
            m_pHandlers.push_back(std::move(h));
        }
    };
}