project(eva)
cmake_minimum_required(VERSION 2.6.0)

find_package(Qt4 REQUIRED)
find_package(KDE4 REQUIRED)
include_directories(${QT_INCLUDES})

set(eva_SRCS
    libuh/evauhinfo.cpp
    libuh/evauhmanager.cpp
    libuh/evauhpacket.cpp
    libuh/evauhsocket.cpp
    libuh/evauhtransfer.cpp
    evacrypt.cpp
    evaimreceive.cpp
    evaimsend.cpp
    evakeepalive.cpp
    evalevel.cpp
    evalogin.cpp
    evalogout.cpp
    evanetwork.cpp
    evaonlinestatus.cpp
    evapacket.cpp
    evasignature.cpp
    evasocket.cpp
    evauserinfo.cpp
    evautil.cpp
    evaweather.cpp
    md5.c
    main.cpp
)

# qt4_automoc(${eva_SRCS})

kde4_add_executable(eva ${eva_SRCS})

target_link_libraries(eva ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTNETWORK_LIBRARY})

