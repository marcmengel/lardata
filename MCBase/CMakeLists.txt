# define ROOTLIB
##set( ROOTLIB -L$ENV{ROOTSYS}/lib -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -pthread -lm -ldl -rdynamic)

art_make( BASENAME_ONLY
	  LIBRARY_NAME     MCBase
          LIB_LIBRARIES
			SimulationBase
			${ROOT_BASIC_LIB_LIST}
                        ${ROOT_EG}
)

install_headers()
install_fhicl()
install_source()
