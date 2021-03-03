

# add_library(RpcServer STATIC
#         Exception.h
#         RpcError.h
#         common.h
#         Client/BaseClient.cpp Client/BaseClient.h
#         Server/BaseServer.h Server/BaseServer.cpp
#         Server/RpcServer.h Server/RpcServer.cpp
#         Server/Service.h  Server/Service.cpp
#         Server/Procedure.h Server/Procedure.cpp
#         )

# target_link_libraries(RpcServer NetServer_s)
# install(TARGETS RpcServer DESTINATION lib)

# set(HEADERS
#         Exception.h
#         RpcError.h
#         common.h
#         client/BaseClient.h
#         Server/BaseServer.h
#         Server/RpcServer.h
#         Server/RpcService.h
#         Server/Procedure.h
#         )
# install(FILES ${HEADERS} DESTINATION include)



list(APPEND RPCSERVER_SRCS
    ${PROJECT_SOURCE_DIR}/RpcServer/Client/BaseClient.cpp
    ${PROJECT_SOURCE_DIR}/RpcServer/Server/BaseServer.cpp
    ${PROJECT_SOURCE_DIR}/RpcServer/Server/RpcServer.cpp
    ${PROJECT_SOURCE_DIR}/RpcServer/Server/Service.cpp
    ${PROJECT_SOURCE_DIR}/RpcServer/Server/Procedure.cpp
    ${PROJECT_SOURCE_DIR}/RpcServer/Stub/StubGen.cpp
    ${PROJECT_SOURCE_DIR}/RpcServer/Stub/ClientStubGen.cpp
    ${PROJECT_SOURCE_DIR}/RpcServer/Stub/ServerStubGen.cpp
    )

add_library(RpcServer STATIC ${RPCSERVER_SRCS})
target_include_directories(RpcServer PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(RpcServer NetServer_s)
install(TARGETS RpcServer DESTINATION ${CMAKE_INSTALL_LIBDIR})


    install(FILES 

        ${PROJECT_SOURCE_DIR}/RpcServer/Exception.h
        ${PROJECT_SOURCE_DIR}/RpcServer/RpcError.h
        ${PROJECT_SOURCE_DIR}/RpcServer/common.h
        ${PROJECT_SOURCE_DIR}/RpcServer/client/BaseClient.h
        ${PROJECT_SOURCE_DIR}/RpcServer/Server/BaseServer.h
        ${PROJECT_SOURCE_DIR}/RpcServer/Server/RpcServer.h
        ${PROJECT_SOURCE_DIR}/RpcServer/Server/RpcService.h
        ${PROJECT_SOURCE_DIR}/RpcServer/Server/Procedure.h
        ${PROJECT_SOURCE_DIR}/RpcServer/Stub/StubGen.h
        ${PROJECT_SOURCE_DIR}/RpcServer/Stub/ClientStubGen.h
        ${PROJECT_SOURCE_DIR}/RpcServer/Stub/ServerStubGen.cpp

        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})


include_directories(${PROJECT_SOURCE_DIR})


function(add_RpcServer_executable EXECUTABLE_NAME EXECUTABLE_SOURCES)
    add_executable(${EXECUTABLE_NAME} ${EXECUTABLE_SOURCES})
    target_link_libraries(${EXECUTABLE_NAME} RpcServer)
    target_include_directories(${EXECUTABLE_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
    install(TARGETS ${EXECUTABLE_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
endfunction(add_RpcServer_executable)


add_RpcServer_executable(RpcEchoClient test/EchoClient.cpp)
add_RpcServer_executable(RpcEchoServer test/EchoServer.cpp)

add_RpcServer_executable(RpcGenClient testStub/GenClient.cpp)