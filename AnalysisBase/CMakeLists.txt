
art_make( LIBRARY_NAME  AnalysisBase
          LIB_LIBRARIES ${ART_FRAMEWORK_CORE}
			${ART_FRAMEWORK_PRINCIPAL}
			${ART_PERSISTENCY_PROVENANCE}
			${ART_UTILITIES}
			${MF_MESSAGELOGGER}
			${MF_UTILITIES}
			${CETLIB}
        )

install_headers()
install_fhicl()
install_source()
