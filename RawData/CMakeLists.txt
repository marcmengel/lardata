
# for classes_def.xml!!
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( $ENV{NUTOOLS_INC} )

art_make( LIBRARY_NAME  RawData
          LIB_LIBRARIES SummaryData
	                ${ART_FRAMEWORK_CORE}
			${ART_FRAMEWORK_PRINCIPAL}
			${ART_PERSISTENCY_PROVENANCE}
			${ART_UTILITIES}
			${MF_MESSAGELOGGER}
			${MF_UTILITIES}
			${CETLIB}
			${ROOT_BASIC_LIB_LIST}
                        ${ROOT_EG}
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
