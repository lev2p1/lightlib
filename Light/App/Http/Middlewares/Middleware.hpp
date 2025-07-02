#include <boost/beast/http.hpp>

namespace beast = boost::beast;
namespace http = beast::http;

class Middlewhare {
public:
	using Request = http::request<http::string_body>;

	virtual bool handle(Request& req) = 0;
};