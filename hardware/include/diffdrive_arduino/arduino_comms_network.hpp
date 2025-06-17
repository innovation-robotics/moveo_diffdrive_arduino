#ifndef DIFFDRIVE_ARDUINO_ARDUINO_COMMS_HPP
#define DIFFDRIVE_ARDUINO_ARDUINO_COMMS_HPP

#include <sstream>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <mutex>
#include <vector>

bool ConnectToServer(int &skt, int portno, std::string ip_addr)
{
    struct sockaddr_in serv_addr;
    struct hostent *server;

    skt = socket(AF_INET, SOCK_STREAM, 0);
    if (skt < 0)
    {
        printf("ERROR opening socket");
        return false;
    }
    server = gethostbyname(ip_addr.c_str());
    if (server == NULL)
    {
        fprintf(stderr,"ERROR, no such host\n");
        return false;
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(skt,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        printf("ERROR connecting");
        return false;
    }
    return true;
}

bool ReadNetworkMessage(const int &socket, std::string &msg)
{
    try
    {
        char buffer[1024];
        bzero(buffer, 1024);

        int n = read(socket, buffer, 1024);
        if(n<=0)
        {
          printf("ERROR reading to socket");
          return false;
        }
        msg = buffer;
    }
    catch(...)
    {
      return false;
    }

    return true;
}

bool SendNetworkMessage(const int &socket, const std::string &msg)
{
  try
  {
    int n = send(socket, msg.c_str(), msg.length(), MSG_NOSIGNAL);
    if (n < 0)
    {
      printf("ERROR writing to socket");
      return false;
    }
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
    return false;
  }

  return true;
}

class ArduinoComms
{

public:

  ArduinoComms() = default;

  void connect(std::string ip_addr, int32_t network_port_number)
  {
    m_ip_addr = ip_addr;
    m_network_port_number = network_port_number;

    if(ConnectToServer(m_socket, m_network_port_number, m_ip_addr))
    {
      printf("succeeded++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    }
    else
    {
      printf("failed++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    }
  }

  void disconnect()
  {
    close(m_socket);
  }

  bool connected() const
  {
    if(m_socket<0)
    {
      return false;
    }
    return true;
  }

  std::string send_msg_get_response(const std::string &msg_to_send, bool print_output = false)
  {
    std::string response;
    std::string msg = "send_get_response|" + msg_to_send;

    bool res = SendNetworkMessage(m_socket, msg);
    res = ReadNetworkMessage(m_socket, response);
    if (print_output)
    {
      std::cout << "Sent: " << msg << " Recv: " << response << std::endl;
    }

    return response;
  }

  void send_msg(const std::string &msg_to_send, bool print_output = false)
  {
    std::string msg = "send|" + msg_to_send;
    SendNetworkMessage(m_socket, msg);
  }

  void send_empty_msg()
  {
    send_msg("\r");
  }

  void read_encoder_values(int16_t &val_1, int16_t &val_2)
  {
    std::string response = send_msg_get_response("e\r");

    std::string delimiter = " ";
    size_t del_pos = response.find(delimiter);
    std::string token_1 = response.substr(0, del_pos);
    std::string token_2 = response.substr(del_pos + delimiter.length());

    val_1 = std::atoi(token_1.c_str());
    val_2 = std::atoi(token_2.c_str());
  }
  
  void set_motor_values(int val_1, int val_2)
  {
    std::stringstream ss;
    ss << "m " << val_1 << " " << val_2 << "\r";
    send_msg(ss.str());
  }

  void reset_encoders()
  {
    send_msg("r\r");
  }

  void set_pid_values(int k_p, int k_d, int k_i, int k_o)
  {
    std::stringstream ss;
    ss << "u " << k_p << ":" << k_d << ":" << k_i << ":" << k_o << "\r";
    send_msg(ss.str());
  }

private:
    int m_socket=-1;
    std::string m_ip_addr;
    int32_t m_network_port_number;
};

#endif // DIFFDRIVE_ARDUINO_ARDUINO_COMMS_HPP