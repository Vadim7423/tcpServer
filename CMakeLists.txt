cmake_minimum_required(VERSION 3.16)

project(testServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(testServer main.cpp
    server.h server.cpp
    src/channel.h src/channel.cpp
    src/range.h src/range.cpp
    src/row_channels.h src/row_channels.cpp
    src/volt_meter_protocol.h src/volt_meter_protocol.cpp
    src/protocol.h
    src/timer.h src/timer.cpp )

include(GNUInstallDirs)
install(TARGETS testServer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
