
art_make( LIBRARY_NAME  RecoObjects
          LIB_LIBRARIES AnalysisAlg
                        AnalysisBase
                        RecoBase
                        LArProperties_service
                        DetectorProperties_service
                        Geometry_service
                        Geometry
                        ${ART_FRAMEWORK_CORE}
			${ART_FRAMEWORK_PRINCIPAL}
			${ART_FRAMEWORK_SERVICES_REGISTRY}
			${ART_PERSISTENCY_COMMON}
			${ART_PERSISTENCY_PROVENANCE}
			${ART_UTILITIES}
			${MF_MESSAGELOGGER}
			${MF_UTILITIES}
	                ${FHICLCPP}
			${CETLIB}
			${ROOT_BASIC_LIB_LIST}
        )


install_headers()
install_fhicl()
install_source()

add_subdirectory( test )

