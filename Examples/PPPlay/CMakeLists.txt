set(SRCDIR "${CMAKE_CURRENT_SOURCE_DIR}")
add_executable(PPPlay ${SRCDIR}/PPPlay.c)
SET(PPP_LIBS PlayerPROCore)

if(UNIX)
    SET(PPP_LIBS ${PPP_LIBS}
        m
        c
        bsd
        dl)
endif(UNIX)

target_link_libraries(PPPlay ${PPP_LIBS})
include_directories(${CMAKE_SOURCE_DIR}/MADDriver.source)

