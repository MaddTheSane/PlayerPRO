set(SRCDIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
add_definitions(-DBUILDINGPPRO)

//set(PlayerPRO_VERSION_MAJOR 2)
//set(PlayerPRO_VERSION_MINOR 0)
//set(PlayerPRO_VERSION_MICRO 4)
set(PlayerPRO_VERSION_INTERFACE 0)
set(PlayerPRO_VERSION_AGE 0)
set(PlayerPRO_VERSION "${PlayerPRO_VERSION_MAJOR}.${PlayerPRO_VERSION_MINOR}.${PlayerPRO_VERSION_MICRO}")

# Calculate a libtool-like version number
math(EXPR LT_CURRENT "${PlayerPRO_VERSION_MICRO} - ${PlayerPRO_VERSION_INTERFACE}")
math(EXPR LT_AGE "${PlayerPRO_VERSION_AGE} - ${PlayerPRO_VERSION_INTERFACE}")
math(EXPR LT_MAJOR "${LT_CURRENT}- ${LT_AGE}")
set(LT_REVISION "${PlayerPRO_VERSION_INTERFACE}")
set(LT_RELEASE "${PlayerPRO_VERSION_MAJOR}.${PlayerPRO_VERSION_MINOR}")
set(LT_VERSION "${LT_MAJOR}.${LT_AGE}.${LT_REVISION}")

message(STATUS "${LT_VERSION} :: ${LT_AGE} :: ${LT_REVISION} :: ${LT_CURRENT} :: ${LT_RELEASE}")


set(PPC_SRCs
    ${SRCDIR}/DelayOutPut.c
    ${SRCDIR}/Effects.c
    ${SRCDIR}/MainDriver.c
    ${SRCDIR}/FileUtils.c
    ${SRCDIR}/Interrupt.c
    ${SRCDIR}/MADDriver.c
    ${SRCDIR}/MyDebugStr.c
    ${SRCDIR}/OutPut8.c
    ${SRCDIR}/realft.c
    ${SRCDIR}/TickRemover.c
    ${SRCDIR}/stub-VSTPlugIn.c
    ${SRCDIR}/MIDI-Hardware-Stub.c
)

set(PPC_Headers
    ${SRCDIR}/MADFileUtils.h
    ${SRCDIR}/MADDriver.h
    ${SRCDIR}/MAD.h
    ${SRCDIR}/PlayerPROCore.h
    ${SRCDIR}/MADDefs.h
    ${SRCDIR}/RDriver.h
    ${SRCDIR}/VSTFunctions.h
)

if(UNIX)
    set(PPC_SRCs
        ${PPC_SRCs}
        ${SRCDIR}/Lin-PlugImport.c
    )

    add_definitions(-D__UNIX__)
endif(UNIX)

if(WIN32)
    set(PPC_SRCs
        ${PPC_SRCs}
        ${SRCDIR}/Win-PlugImport.c
        ${SRCDIR}/Win-WaveDriver.c
    )
endif(WIN32)

find_package(PulseAudio)

add_library(PlayerPROCore SHARED ${PPC_SRCs} ${PPC_Headers})

  if(UNIX)
    set_target_properties(PlayerPROCore PROPERTIES
      VERSION ${LT_VERSION}
      SOVERSION ${LT_REVISION}
      OUTPUT_NAME "PlayerPROCore-${LT_RELEASE}")
  else()
    set_target_properties(PlayerPROCore PROPERTIES
      VERSION ${PlayerPRO_VERSION}
      SOVERSION ${LT_REVISION}
      OUTPUT_NAME "PlayerPROCore-${PlayerPRO_VERSION_MAJOR}")
  endif()
 set(_INSTALL_LIBS "PlayerPROCore" ${_INSTALL_LIBS})
target_link_libraries(PlayerPROCore dl m)


add_subdirectory(Import-Export)


