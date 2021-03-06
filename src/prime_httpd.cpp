#include "prime_server.hpp"
#include "http_protocol.hpp"
using namespace prime_server;
#include "logging.hpp"

#include <cstring>
#include <string>

int main(int argc, char** argv) {

  if(argc < 5) {
    logging::ERROR("Usage: " + std::string(argv[0]) +
      " [tcp|ipc]://server_listen_endpoint[:tcp_port] [tcp|ipc]://downstream_proxy_endpoint[:tcp_port] [tcp|ipc]://server_result_loopback[:tcp_port] [tcp|ipc]://server_request_interrupt[:tcp_port] [enable_logging] [max_request_size_bytes] [request_timeout_seconds]");
    return EXIT_FAILURE;
  }

  //TODO: validate these
  std::string server_endpoint(argv[1]);
  std::string proxy_endpoint(argv[2]);
  std::string server_result_loopback(argv[3]);
  std::string server_request_interrupt(argv[4]);
  if(server_endpoint.find("://") != 3)
    logging::ERROR("bad server listen endpoint");
  if(proxy_endpoint.find("://") != 3)
    logging::ERROR("bad downstream proxy endpoint");
  if(server_result_loopback.find("://") != 3)
    logging::ERROR("bad server result loopback");
  if(server_request_interrupt.find("://") != 3)
    logging::ERROR("bad server request interrupt");

  //default to logging requests/responses
  bool log = argc < 6 || strcasecmp(argv[5], "false") != 0;

  //default to 10mb
  size_t max_request_size_bytes = 1024*1024*10;
  try {
    if(argc > 6)
      max_request_size_bytes = std::stoul(argv[6]);
  } catch(...){ }

  //default to no timeout
  uint32_t request_timeout_seconds = -1;
  try {
    if(argc > 7)
      request_timeout_seconds = std::stoul(argv[7]);
  } catch(...){ }

  //start it up
  zmq::context_t context;
  http_server_t server(context, server_endpoint, proxy_endpoint, server_result_loopback, server_request_interrupt, log, max_request_size_bytes, request_timeout_seconds);

  //TODO: catch SIGINT for graceful shutdown
  server.serve();

  return EXIT_SUCCESS;
}
