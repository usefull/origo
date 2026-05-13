#pragma once

namespace origo {

    class IHandler {
    public:
        virtual void Register(const std::unique_ptr<restinio::router::easy_parser_router_t> &&rawRouter) = 0;
    };
}