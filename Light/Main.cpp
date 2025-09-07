#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/config.hpp>
#include "Router/RouterRegisterer.hpp"
#include "vendor/Handlers/ENV.hpp"
#include "Database/Queue.hpp"
#include "Database/Cache.hpp"
#include "Database/Migrations/MigrationManager.hpp"
#include "App/Http/Services/AuthService.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;
using namespace std::chrono_literals;

bool ENV::initialized = false;
const std::string ENV::env_file_path = "./.env";
redisContext* Queue::context_ = nullptr;
redisContext* Cache::context_ = nullptr;

net::awaitable<void> handle_connection(tcp::socket socket) {
    try {
        beast::flat_buffer buffer;
        http::request<http::string_body> req;
        http::response<http::string_body> res;

        co_await http::async_read(socket, buffer, req, net::use_awaitable);

        res.version(req.version());
        res.keep_alive(req.keep_alive());

        Router::handle_request(req, res);

        co_await http::async_write(socket, res, net::use_awaitable);
        beast::error_code ec;
        socket.shutdown(tcp::socket::shutdown_send, ec);
    }
    catch (const std::exception& e) {
        Logger::log("Exception in connection: " + std::string(e.what()), "ERROR");
    }
    co_return;
}

net::awaitable<void> accept_loop(tcp::acceptor& acceptor) {
    for (;;) {
        beast::error_code ec;
        tcp::socket socket = co_await acceptor.async_accept(net::use_awaitable);
        net::co_spawn(acceptor.get_executor(), handle_connection(std::move(socket)), net::detached);
    }
}

int main() {
    try {
        ENV::initialize();
        Logger::init("debug.log");
        Logger::registerSignalHandlers();
        AuthService::secret = ENV::env_variables["AUTH_SECRET"];

        try {
            Queue::connect(ENV::env_variables["REDIS_HOST"], stoi(ENV::env_variables["REDIS_PORT"]));
        } catch (const std::exception& e) {
            Logger::log("Connection to queue failed", "ERROR");
        }

        try {
            Cache::connect(ENV::env_variables["REDIS_HOST"], stoi(ENV::env_variables["REDIS_PORT"]));
        } catch (const std::exception& e) {
            Logger::log("Connection to NOSQL database failed", "ERROR");
        }

        try {
            Database db;
            (new MigrationManager(db))->Initialize();
        } catch (const std::exception& e) {
            Logger::log("Connection to database failed", "ERROR");
        }

        const unsigned short port = stoi(ENV::env_variables["S_PORT"]);
        net::io_context io;
        tcp::acceptor acceptor(io, { tcp::v4(), port });

        Logger::log("Server is running on port " + std::to_string(port), "SUCCESS");

        RouterRegisterer::init(io);

        net::co_spawn(io, accept_loop(acceptor), net::detached);

        io.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        Logger::log(std::string(e.what()), "ERROR");
        return 1;
    }

    Logger::log("Application finished", "INFO");
    return 0;
}
