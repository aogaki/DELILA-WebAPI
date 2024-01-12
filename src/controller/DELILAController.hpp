#ifndef DELILAController_hpp
#define DELILAController_hpp 1

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

class DELILAController
{
 public:
  DELILAController(std::string URI, uint32_t port);
  ~DELILAController();

  std::string CheckStatus();

  std::string Configure();
  std::string Unconfigure();

  std::string Start(int32_t runNo);
  std::string Stop();

  std::string ConfigureAndStart(int32_t runNo);
  std::string StopAndUnconfigure();

 private:
  std::string fURI;
  uint32_t fPort;
  int32_t fSocketFD;

  bool OpenSocket();
  void CloseSocket();

  size_t SendToSiTCP(std::string command);
  std::string RecieveFromSiTCP();

  std::string AccessDAQ(std::string command);
  std::string fFailed = "Failed";
};

#endif