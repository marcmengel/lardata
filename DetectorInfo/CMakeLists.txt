
art_make( NO_PLUGINS
          LIBRARY_NAME  DetectorInfo
          LIB_LIBRARIES
			Geometry
                        ${MF_MESSAGELOGGER}
                        ${MF_UTILITIES}
                        ${CETLIB}
			${FHICLCPP}
                        ${ROOT_BASIC_LIB_LIST}
			${Boost_SYSTEM_LIBRARY}
        )



install_headers()
install_fhicl()
install_source()

#add_subdirectory(test)

