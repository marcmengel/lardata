
# for classes_def.xml!!
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

art_make( LIBRARY_NAME  RecoBase
          LIB_LIBRARIES RawData
                        Geometry
                        Geometry_service
                        ${ART_FRAMEWORK_CORE}
			${ART_FRAMEWORK_PRINCIPAL}
			${ART_FRAMEWORK_SERVICES_REGISTRY}
			${ART_PERSISTENCY_COMMON}
			${ART_PERSISTENCY_PROVENANCE}
			${ART_UTILITIES}
			${MF_MESSAGELOGGER}
			${MF_UTILITIES}
			${CETLIB}
			${ROOT_BASIC_LIB_LIST}
        )

# temporary workaround until art v1_12_01 is available
set_target_properties(RecoBase_dict RecoBase_map
                      PROPERTIES COMPILE_FLAGS -fno-var-tracking)

install_headers()
install_fhicl()
install_source()
