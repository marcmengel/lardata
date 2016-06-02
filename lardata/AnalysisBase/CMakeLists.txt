
art_make( 
          LIB_LIBRARIES ${ART_FRAMEWORK_CORE}
			${ART_FRAMEWORK_PRINCIPAL}
			art_Persistency_Provenance canvas_Persistency_Provenance
			art_Utilities canvas_Utilities
			${MF_MESSAGELOGGER}
			${MF_UTILITIES}
			${CETLIB}
			${ROOT_BASIC_LIB_LIST}
        )

install_headers()
install_fhicl()
install_source()
