# to compile the plugin out of
# kopete source tree
if(CMAKE_SOURCE_DIR STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
  set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
  find_package(KDE4 REQUIRED)
  find_package(Qt4 REQUIRED)
  find_package(OpenSSL REQUIRED)
  INCLUDE_DIRECTORIES(${QT4_INCLUDE_DIR})
  INCLUDE_DIRECTORIES(${QT_QTCORE_INCLUDE_DIR})
  INCLUDE_DIRECTORIES(${QT_QTGUI_INCLUDE_DIR})
  INCLUDE_DIRECTORIES(${QT_QTNETWORK_INCLUDE_DIR})
  INCLUDE_DIRECTORIES(${KDE4_INCLUDE_DIR})
  INCLUDE_DIRECTORIES(${KDE4_INCLUDES})

  include(KDE4Defaults)
  include(MacroLibrary)

  find_path(KOPETE_INCLUDE_DIR
    NAME kopeteversion.h
    PATH_SUFFIXES kopete
    HINTS
    ${INCLUDE_INSTALL_DIR}
    ${KDE4_INCLUDE_DIR}
  )

  if(KOPETE_INCLUDE_DIR)
    MESSAGE("Found Kopete: includes - ${KOPETE_INCLUDE_DIR}")
  else(KOPETE_INCLUDE_DIR)
    MESSAGE(FATAL_ERROR "Kopete headers not found")
  endif(KOPETE_INCLUDE_DIR)

  mark_as_advanced(KOPETE_INCLUDE_DIR)
endif(CMAKE_SOURCE_DIR STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")


include_directories(${KOPETE_INCLUDE_DIR} ${KOPETE_INCLUDE_DIR}/ui ${OPENSSL_INCLUDE_DIR})


set(libeva_SRCS
    libeva/libuh/evauhinfo.cpp
    libeva/libuh/evauhmanager.cpp
    libeva/libuh/evauhpacket.cpp
    libeva/libuh/evauhsocket.cpp
    libeva/libuh/evauhtransfer.cpp
    libeva/evacrypt.cpp
    libeva/evafriend.cpp
    libeva/evagroup.cpp
    libeva/evaimreceive.cpp
    libeva/evaimsend.cpp
    libeva/evakeepalive.cpp
    libeva/evakeyrequest.cpp
    libeva/evalevel.cpp
    libeva/evalogin.cpp
    libeva/evalogout.cpp
    libeva/evanetwork.cpp
    libeva/evaonlinestatus.cpp
    libeva/evapacket.cpp
    libeva/evasignature.cpp
    libeva/evasocket.cpp
    libeva/evauserinfo.cpp
    libeva/evautil.cpp
    libeva/evaweather.cpp
    libeva/md5.c
)

kde4_add_ui_files(kopete_qq_PART_UIS
#     ui/qqcontactinfo.ui
    ui/qqeditaccount.ui
)

set(kopete_qq_PART_SRCS
    ${libeva_SRCS}
    ${kopete_qq_PART_UIS}
    qqaccount.cpp
    qqaddcontactpage.cpp
    qqchatsession.cpp
    qqcontact.cpp
    qqeditaccountwidget.cpp
    qqprotocol.cpp
)

kde4_add_plugin(kopete_qq ${kopete_qq_PART_SRCS})

target_link_libraries(kopete_qq ${KDE4_KIO_LIBS} kopete)

install(TARGETS kopete_qq DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install(FILES kopete_qq.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES ui/qqchatui.rc DESTINATION ${DATA_INSTALL_DIR}/kopete_qq)

# add_subdirectory(libeva)
add_subdirectory(icons)
