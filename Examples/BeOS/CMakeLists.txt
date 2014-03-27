if(BEOS)
    set(SRCDIR "${CMAKE_CURRENT_SOURCE_DIR}")
    add_executable(PlayerPRO.app 
        ${SRCDIR}/mainMADSimple.cpp
        ${SRCDIR}/mainMADSimple.h)
        target_link_libraries(PlayerPRO.app PlayerPROCore)
endif(BEOS)

