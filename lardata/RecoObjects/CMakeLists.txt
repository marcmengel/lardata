
art_make( 
          LIB_LIBRARIES lardata_AnalysisAlg
                        lardata_AnalysisBase
                        lardata_RecoBase
                        larcore_Geometry_Geometry_service
                        larcore_Geometry
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


