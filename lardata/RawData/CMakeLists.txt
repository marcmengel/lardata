
# for classes_def.xml!!
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

art_make( 
          LIB_LIBRARIES art_Persistency_Common canvas_Persistency_Common
			${MF_MESSAGELOGGER}
			${MF_UTILITIES}
			${CETLIB}
        )


install_headers()
install_fhicl()
install_source()

add_subdirectory(utils)

# uboone_datatypes requires some proprietary software
# we cannot build it everywhere
# that also means that we must also exclucde LArRawInputDriverUBooNE from the utils library build
# this code will be moved into a uboone package
#add_subdirectory(uboone_datatypes)
