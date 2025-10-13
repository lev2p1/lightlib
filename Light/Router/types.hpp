#pragma once

#include <boost/beast/http.hpp>

namespace beast = boost::beast;
namespace http = beast::http;

constexpr auto GET = http::verb::get;
constexpr auto POST = http::verb::post;
constexpr auto PUT = http::verb::put;
constexpr auto PATCH = http::verb::patch;
constexpr auto DELETE_ = http::verb::delete_;
constexpr auto OPTIONS = http::verb::options;

