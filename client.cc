#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <grpcpp/grpcpp.h>

#include "protofile.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using myproto::Numbers;
using myproto::Answer;
using myproto::MyService;

class MyClient {
 public:
  MyClient(std::shared_ptr<Channel> channel)
      : stub_(MyService::NewStub(channel)) {}

    int64_t GetMaxOne(const std::vector<int32_t>& nums, uint64_t size){
    
    Numbers numbers;
    for (uint64_t i = 0; i < nums.size(); ++i){
        numbers.add_nums(nums[i]);
    }
    Answer answer;

    ClientContext context;

    Status status = stub_->GetMaxOne(&context, numbers, &answer);

    if (status.ok()) {
      return answer.answer();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return -1;
    }
  }

 private:
  std::unique_ptr<MyService::Stub> stub_;
};

int main(int argc, char** argv) {
    std::string target_str;
    std::string arg_str("--target");
    if (argc > 1) {
        std::string arg_val = argv[1];
        size_t start_pos = arg_val.find(arg_str);
        if (start_pos != std::string::npos) {
            start_pos += arg_str.size();
            if (arg_val[start_pos] == '=') {
                target_str = arg_val.substr(start_pos + 1);
            } else {
                std::cout << "The only correct argument syntax is --target=" << std::endl;
                return 0;
            }
        } else {
            std::cout << "The only acceptable argument is --target=" << std::endl;
            return 0;
        }
    } else {
        target_str = "localhost:50051";
    }
    MyClient client(grpc::CreateChannel(
      target_str, grpc::InsecureChannelCredentials()));
  
    int32_t num;
    std::vector<int32_t> nums;
    while (std::cin >> num){
        nums.push_back(num);
    }
    if (nums.size() == 0) {
        std::cout << "No input data" << std::endl;
        return 0;
    }
    uint64_t ans = client.GetMaxOne(nums, nums.size());
    std::cout << ans << std::endl;
    return 0;
}
