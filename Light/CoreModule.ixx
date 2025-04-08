// CoreModule.cppm
export module CoreModule;

// Подключение внешних библиотек
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
#include <libpq-fe.h>

#include <iostream>
#include <string>
#include <memory>
#include <map>

// Подключение внутренних заголовков
#include "vendor/Handlers/ENV.hpp"
#include "vendor/Facades/Hash.hpp"