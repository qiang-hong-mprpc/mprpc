set(SRC_LIST MprpcApplication.cc 
             RpcConfig.cc
             RpcProvider.cc
             client_request.pb.cc
             MprpcChannel.cc
             MprpcController.cc
             Logger.cc
             ZKClient.cc)

add_library(mprpc ${SRC_LIST})
target_link_libraries(mprpc muduo_net muduo_base pthread zookeeper_mt)