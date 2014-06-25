set(SRCDIR "${CMAKE_CURRENT_SOURCE_DIR}")
add_executable(PPPlay ${SRCDIR}/PPPlay.c)
SET(PPP_LIBS PlayerPROCore)

target_link_libraries(PPPlay ${PPP_LIBS})
include_directories(${CMAKE_SOURCE_DIR}/MADDriver.source)

