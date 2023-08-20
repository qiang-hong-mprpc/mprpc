# aux_source_directory(. SRC_LIST)
set(SRC_LIST mprpcApplication.cc 
             rpcProvider.cc 
             mprpcConfig.cc
             rpcheader.pb.cc 
             mprpcChannel.cc 
             mprpcController.cc 
             logger.cc
             zookeeperUtil.cc )
add_library(mprpc ${SRC_LIST})
target_link_libraries(mprpc muduo_net muduo_base pthread zookeeper_mt)