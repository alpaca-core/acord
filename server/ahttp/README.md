# ahttp

Library to facilitate HTTP requests.

## Note

The CMake configuration of the library will check for OpenSSL and will use it if found. If not found, the library will be built without SSL support and not requests through HTTPS will be available.
