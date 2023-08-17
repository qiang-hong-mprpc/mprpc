set(SRC_LIST 
    main.cc
    ../user.pb.cc)

add_executable(provider ${SRC_LIST})

target_link_libraries(provider mprpc protobuf)