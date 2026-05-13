#pragma once

#include <string>
#include <iostream>
#include <nlohmann/json.hpp>
#include <fmt/core.h>

#include "./messages.hpp"

using namespace std;
using json = nlohmann::json;
namespace err = origo::ErrorMessages;

namespace origo
{
    struct Config {
        string ip;
        uint port;

        json to_json() const {
            return {{"ip", ip}, {"port", port}};
        }

        static Config from_json(const json& j) {

            Config config;

            try
            {
                config.ip = j.at("ip");
            }
            catch (const exception& e)
            {
                throw runtime_error(fmt::format(err::CantReadIpFromConfig, e.what()));
            }

            try
            {
                config.port = j.at("port");
            }
            catch (const exception& e)
            {
                throw runtime_error(fmt::format(err::CantReadPortFromConfig, e.what()));
            }
            return config;
        }

        static Config from_file(const char* file_path)
        {
            ifstream file(file_path);
            if (!file)
            {
                throw runtime_error(fmt::format(err::CantOpenConfigFile, file_path));
            }

            json jsonConfig;
            try
            {
                file >> jsonConfig;
            }
            catch (const json::parse_error& e)
            {
                throw runtime_error(fmt::format(err::ConfigFileReadingError, e.what()));
            }

            return Config::from_json(jsonConfig);
        }
    };
}