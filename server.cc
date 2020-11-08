#include <iostream>
#include <memory>
#include <string>
#include <algorithm>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include "protofile.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using myproto::Numbers;
using myproto::Answer;
using myproto::MyService;

class MyServer final : public MyService::Service{
    Status GetMaxOne(ServerContext* context, const Numbers* numbers,
                  Answer* answer) override {
        size_t max_ = 0;
        size_t cnt = 0;
        size_t last_zero = 0;
        bool contain_zero = false;
        for (auto it = numbers->nums().begin(); it < numbers->nums().end();++it){
            if (*it == 1){
                cnt++;
            } else {
                contain_zero = true;
                max_ = std::max(max_, cnt + last_zero);
                last_zero = cnt;
                cnt = 0;
            }
        }
        if (!contain_zero){
            answer->set_answer(cnt - 1);
        } else {
            answer->set_answer(std::max(max_, cnt + last_zero));
        }
        return Status::OK;
    }

};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
    MyServer service;

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  server->Wait();
}


int main(int argc, char** argv) {
  RunServer();

  return 0;
}

