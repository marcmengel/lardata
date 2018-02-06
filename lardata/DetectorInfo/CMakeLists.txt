
art_make( NO_PLUGINS
          
          LIB_LIBRARIES
			larcorealg_Geometry
			larcorealg_CoreUtils
                        ${MF_MESSAGELOGGER}
                        ${MF_UTILITIES}
                        cetlib cetlib_except
			${FHICLCPP}
                        ${ROOT_BASIC_LIB_LIST}
			${Boost_SYSTEM_LIBRARY}
        )



install_headers()
install_fhicl()
install_source()

#add_subdirectory(test)

