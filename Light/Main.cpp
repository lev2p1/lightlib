#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/config.hpp>
#include "Router/RouterRegisterer.hpp"
#include "vendor/Handlers/ENV.hpp"
#include "Database/Queue.hpp"
#include "Database/Cache.hpp"
#include "Database/Migrations/MigrationManager.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

bool ENV::initialized = false;
const std::string ENV::env_file_path = "./.env";
redisContext* Queue::context_ = nullptr;
redisContext* Cache::context_ = nullptr;

int main() {
    try {
        ENV::initialize();
        Logger::init("debug.log");
        Logger::registerSignalHandlers();

        try {
            Queue::connect(ENV::env_variables["REDIS_HOST"], stoi(ENV::env_variables["REDIS_PORT"]));

        }
        catch (const std::exception& e) {
            Logger::log("Connection to queue failed", "ERROR");
        }

        try {
            Cache::connect(ENV::env_variables["REDIS_HOST"], stoi(ENV::env_variables["REDIS_PORT"]));
        }
        catch(const std::exception& e){
            Logger::log("Connection to NOSQL database failed", "ERROR");
        }

        Logger::log("Application started", "INFO");
        try {
            Database db;
            (new MigrationManager(db))->Initialize();

            //Initializer::initMigrations();
            //try {
            //    // Создаем таблицу migrations
            //    db.execute(MigrationMigrationsCreate::up());
            //    std::cout << "Table 'migrations' created successfully." << std::endl;
            //}
            //catch (const std::exception& e) {
            //    std::cerr << "Error creating 'migrations' table: " << e.what() << std::endl;
            //    return 1;
            //}
        }
        catch (const std::exception& e) {
            Logger::log("Connection to database failed", "ERROR");

        }
        const unsigned short port = 8080;
        net::io_context ioc;
        tcp::acceptor acceptor(ioc, { tcp::v4(), port });
        Logger::log("Server is running on port " + std::to_string(port), "SUCCESS");
        RouterRegisterer::init();

        while (true) {
            tcp::socket socket(ioc);
            acceptor.accept(socket);

            try {
                net::steady_timer timer(ioc);
                timer.expires_after(std::chrono::seconds(30));

                timer.async_wait([&socket](const boost::system::error_code& ec) {
                    if (!ec) {
                        socket.close();
                    }
                    });

                beast::flat_buffer buffer;

                http::request<http::string_body> req;
                boost::system::error_code ec;
                http::read(socket, buffer, req, ec);

                if (ec == beast::http::error::end_of_stream) {
                    Logger::log("Client closed the connection prematurely.", "WARNING");
                    continue;
                }
                else if (ec) {
                    Logger::log("Error reading request: " + ec.message(), "ERROR");
                    continue;
                }

                timer.cancel();

                http::response<http::string_body> res;
                res.version(req.version());

                Router::handle_request(req, res);

                http::write(socket, res, ec);
                if (ec) {
                    Logger::log("Error sending response: " + ec.message(), "ERROR");
                    continue;
                }

                socket.shutdown(tcp::socket::shutdown_send, ec);
                if (ec && ec != boost::system::errc::not_connected) {
                    Logger::log("Error shutting down socket: " + ec.message(), "ERROR");
                }
            }
            catch (const std::exception& e) {
                Logger::log("Error handling request: " + std::string(e.what()), "ERROR");
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        Logger::log(std::string(e.what()), "ERROR");
        return 1;
    }

    Logger::log("Application finished", "INFO");
    return 0;
}
