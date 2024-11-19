// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "ahttp.hpp"

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#if AHTTP_SSL
#   include <boost/beast/ssl.hpp>
#endif

#include <string> // must be here (workardound for furi bug)
#include <furi/furi.hpp>

#include <astl/throw_stdex.hpp>
#include <astl/move.hpp>

#include <limits>
#include <iostream>

namespace net = boost::asio;
namespace ssl = net::ssl;
namespace beast = boost::beast;
namespace http = boost::beast::http;

namespace ahttp {

bool supports_https() noexcept {
#if AHTTP_SSL
    return true;
#else
    return false;
#endif
}

bool supports_url(std::string_view url) noexcept {
    auto scheme = furi::uri_split::get_scheme_from_uri(url);
    if (scheme == "http") return true;
    if (scheme == "https") return supports_https();
    return false;
}

namespace {

class http_stream {
public:
    virtual ~http_stream() = default;

    virtual void handshake(std::string_view host) = 0;
    virtual void write(const http::request<http::empty_body>& req) = 0;
    virtual void read_header(beast::flat_buffer& buf, http::response_parser<http::buffer_body>& parser) = 0;
    virtual void read(beast::flat_buffer& buf, http::response_parser<http::buffer_body>& parser) = 0;
};

template <typename Stream>
class http_stream_t : public http_stream {
protected:
    Stream m_stream;
public:
    template <typename... Args>
    explicit http_stream_t(Args&&... args) : m_stream(std::forward<Args>(args)...) {}

    virtual void write(const http::request<http::empty_body>& req) final override {
        http::write(m_stream, req);
    }
    virtual void read_header(beast::flat_buffer& buf, http::response_parser<http::buffer_body>& parser) final override {
        http::read_header(m_stream, buf, parser);
    }
    virtual void read(beast::flat_buffer& buf, http::response_parser<http::buffer_body>& parser) final override {
        beast::error_code ec;
        http::read(m_stream, buf, parser, ec);
        if (ec && ec != http::error::need_buffer) {
            throw boost::system::system_error{ ec };
        }
    }
};

class http_stream_tcp final : public http_stream_t<beast::tcp_stream> {
public:
    using http_stream_t<beast::tcp_stream>::http_stream_t;
    virtual void handshake(std::string_view) override {} // no-op for non-ssl
};

#if AHTTP_SSL
using ssl_stream = beast::ssl_stream<beast::tcp_stream>;
class http_stream_ssl final : public http_stream_t<ssl_stream> {
public:
    http_stream_ssl(beast::tcp_stream&& stream, ssl::context& ctx)
        : http_stream_t<ssl_stream>(astl::move(stream), ctx)
    {}

    ~http_stream_ssl() {
        // too many tings can go wrong here, so instead of trying to handle them all, just ignore them
        [[maybe_unused]] beast::error_code ec;
        m_stream.shutdown(ec);
    }

    virtual void handshake(std::string_view host) override {
        std::string hostStr(host); // open ssl needs that 0-terminated and our input may not be
        if (!SSL_set_tlsext_host_name(m_stream.native_handle(), hostStr.c_str())) {
            throw boost::system::system_error(
                ::ERR_get_error(), boost::asio::error::get_ssl_category());
        }

        m_stream.handshake(ssl::stream_base::client);
    }
};
#endif

} // namespace

sync_generator get_sync(std::string_view url) {
    net::io_context ctx;
#if AHTTP_SSL
    ssl::context ssl_ctx{ssl::context::tls_client};
#endif
    beast::flat_buffer buf;
    std::string redirect_url;

    // loop while redirecting
    while (true) {
        buf.clear();

        auto splitUrl = furi::uri_split::from_uri(url);

        auto stream = /*iile*/[&]() -> std::unique_ptr<http_stream> {
            beast::tcp_stream init_stream(ctx);

            net::ip::tcp::resolver resolver(ctx);
            auto resolved = resolver.resolve(splitUrl.authority, splitUrl.scheme);
            init_stream.connect(resolved);

            if (splitUrl.scheme == "http") {
                return std::make_unique<http_stream_tcp>(astl::move(init_stream));
            }
#if AHTTP_SSL
            else if (splitUrl.scheme == "https") {
                return std::make_unique<http_stream_ssl>(astl::move(init_stream), ssl_ctx);
            }
#endif
            else {
                ac::throw_ex{} << "unsupported scheme: " << splitUrl.scheme;
            }
        }();

        stream->handshake(splitUrl.authority);

        http::request<http::empty_body> req;
        req.method(http::verb::get);
        req.target(splitUrl.req_path);
        req.keep_alive(true);
        req.set(http::field::host, splitUrl.authority);
        req.set(http::field::user_agent, "ac-file-download/1.0.0");
        req.set(http::field::accept, "*/*");
        // range support will be left for the future
        //req.set(http::field::range, "1-5");
        //std::cout << req << std::endl;

        stream->write(req);

        http::response_parser<http::buffer_body> parser;
        parser.body_limit(std::numeric_limits<std::uint64_t>::max());

        stream->read_header(buf, parser);

        auto& header = parser.get().base();
        //std::cout << header << std::endl;

        // instead of juggling redirects, just look for location header
        auto f = header.find(http::field::location);
        if (f != header.end()) {
            // redirect
            // update url and loop again
            auto loc = f->value();
            if (loc.starts_with('/')) {
                // relative path
                std::string host{splitUrl.scheme};
                host += "://";
                host += splitUrl.authority;
                redirect_url = host;
                redirect_url += loc;
            }
            else {
                // absolute path
                redirect_url = std::string(loc);
            }

            url = redirect_url;
            //std::cout << "redirecting to: " << url << std::endl;
            continue;
        }

        // no redirect, must be ok
        if (parser.get().result() != http::status::ok) {
            ac::throw_ex{} << "http response status: " << parser.get().result_int();
        }

        // get the resource and return

        {
            auto ret = parser.content_length();
            if (ret) co_yield *ret;
            else co_yield std::nullopt;
        };

        auto& body = parser.get().body();
        while (!parser.is_done()) {
            auto& chunk_buf = co_await sync_generator::chunk_buf_t{};
            body.data = chunk_buf.data();
            body.size = chunk_buf.size();
            stream->read(buf, parser);
            chunk_buf = chunk_buf.subspan(0, chunk_buf.size() - body.size);
        }

        co_return;
    }
}

} // namespace ahttp
