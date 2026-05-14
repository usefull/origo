#pragma once

namespace origo {

    class IHandler {
    public:
        virtual void Register(restinio::router::easy_parser_router_t* rawRouter) = 0;
    };
}