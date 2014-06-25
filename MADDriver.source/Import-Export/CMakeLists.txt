set(SRCDIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
include_directories(${CMAKE_SOURCE_DIR}/MADDriver.source)

add_library(669 SHARED 669.c)
target_link_libraries(669 PlayerPROCore)

add_library(AMF SHARED AMF.c)
target_link_libraries(AMF PlayerPROCore)

add_library(DMF SHARED DMF.c)
target_link_libraries(DMF PlayerPROCore)

add_library(IT SHARED IT.c)
target_link_libraries(IT PlayerPROCore)

add_library(MADfg SHARED MADfg.c)
target_link_libraries(MADfg PlayerPROCore)

add_library(MADH SHARED MADH.c)
target_link_libraries(MADH PlayerPROCore)

add_library(MADI SHARED MADI.c)
target_link_libraries(MADI PlayerPROCore)

add_library(MED SHARED MED.c)
target_link_libraries(MED PlayerPROCore)

add_library(MOD SHARED Mod.c)
target_link_libraries(MOD PlayerPROCore)

add_library(MTM SHARED MTM.c)
target_link_libraries(MTM PlayerPROCore)

add_library(OKTA SHARED Okta.c)
target_link_libraries(OKTA PlayerPROCore)

add_library(S3M SHARED S3M.c)
target_link_libraries(S3M PlayerPROCore)

add_library(ULT SHARED ULT.c)
target_link_libraries(ULT PlayerPROCore)

add_library(UMX SHARED UMX.c)
target_link_libraries(UMX PlayerPROCore)

add_library(XM SHARED XM.c)
target_link_libraries(XM PlayerPROCore)

