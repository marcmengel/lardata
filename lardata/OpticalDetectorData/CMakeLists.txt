include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

art_make(
         LIBRARY_NAME OpticalDetectorData
         LIB_LIBRARIES
        )

install_headers()
install_fhicl()
install_source()
