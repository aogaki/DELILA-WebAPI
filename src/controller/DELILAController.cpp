#include "controller/DELILAController.hpp"

#include "xml2json/xml2json.hpp"

namespace DELILA
{
DELILAController::DELILAController(std::string URI, uint32_t port)
    : fURI(URI), fPort(port)
{
}

DELILAController::~DELILAController() { CloseSocket(); }

bool DELILAController::OpenSocket()
{
  fSocketFD = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(fPort);
  addr.sin_addr.s_addr = inet_addr(fURI.data());

  bool success = false;
  if (connect(fSocketFD, (struct sockaddr *)&addr,
              sizeof(struct sockaddr_in)) == 0) {
    success = true;
  }

  return success;
}

void DELILAController::CloseSocket() { close(fSocketFD); }

size_t DELILAController::SendToSiTCP(std::string command)
{
  const uint32_t com_size = command.size();
  std::string buf;
  constexpr uint32_t header_size = 4;
  buf.resize(header_size);
  memcpy((void *)buf.data(), &com_size, header_size);

  buf += command;
  return send(fSocketFD, buf.data(), buf.size(), 0);
}

std::string DELILAController::RecieveFromSiTCP()
{
  constexpr uint32_t header_size = 4;
  std::string header;
  header.resize(header_size);
  recv(fSocketFD, (void *)header.data(), header_size, 0);

  auto rcv_size = *((uint32_t *)header.data());

  std::string rcv_str;
  rcv_str.resize(rcv_size);
  recv(fSocketFD, (void *)rcv_str.data(), rcv_size, 0);

  auto json = xml2json(rcv_str.c_str());

  return json;
}

std::string DELILAController::AccessDAQ(std::string command)
{
  if (OpenSocket()) {
    SendToSiTCP(command);
    auto response = RecieveFromSiTCP();
    CloseSocket();
    return response;
  } else {
    std::cerr << "Failed to open socket" << std::endl;
    return Failed;
  }
}

std::string DELILAController::CheckStatus()
{
  std::string command = "get:Log";
  auto response = AccessDAQ(command);
  if (response == Failed) std::cerr << "CheckStatus failed" << std::endl;

  return response;
}

std::string DELILAController::Configure()
{
  std::string command =
      "put:Params:<?xml version='1.0' encoding='UTF-8' "
      "?><request><params>config.xml</params></request>";
  auto response = AccessDAQ(command);
  if (response == Failed) std::cerr << "Configure failed" << std::endl;

  return response;
}

std::string DELILAController::Unconfigure()
{
  std::string command = "put:ResetParams";
  auto response = AccessDAQ(command);
  if (response == Failed) std::cerr << "Unconfigure failed" << std::endl;

  return response;
}

std::string DELILAController::Start(int32_t runNo)
{
  std::string part1 =
      "put:Begin:<?xml version='1.0' encoding='UTF-8' ?><request><runNo>";
  auto run = std::to_string(runNo);
  std::string part2 = "</runNo></request>";
  std::string command = part1 + run + part2;
  auto response = AccessDAQ(command);
  if (response == Failed) std::cerr << "Start failed" << std::endl;

  return response;
}

std::string DELILAController::Stop()
{
  std::string command = "put:End";
  auto response = AccessDAQ(command);
  if (response == Failed) std::cerr << "Stop failed" << std::endl;

  return response;
}

std::string DELILAController::ConfigureAndStart(int32_t runNo)
{
  std::string command =
      "put:Params:<?xml version='1.0' encoding='UTF-8' "
      "?><request><params>config.xml</params></request>";
  auto response = AccessDAQ(command);
  if (response == Failed) {
    std::cerr << "Configure failed" << std::endl;
    return response;
  } else {
    sleep(1);
    std::string part1 =
        "put:Begin:<?xml version='1.0' encoding='UTF-8' ?><request><runNo>";
    auto run = std::to_string(runNo);
    std::string part2 = "</runNo></request>";
    command = part1 + run + part2;
    response = AccessDAQ(command);
    if (response == Failed) std::cerr << "Start failed" << std::endl;
  }

  return response;
}

std::string DELILAController::StopAndUnconfigure()
{
  std::string command = "put:End";
  auto response = AccessDAQ(command);
  if (response == Failed) {
    std::cerr << "Stop failed" << std::endl;
    return response;
  } else {
    sleep(1);
    command = "put:ResetParams";
    response = AccessDAQ(command);
    if (response == Failed) std::cerr << "Unconfigure failed" << std::endl;
  }

  return response;
}
}  // namespace DELILA
