static: error(This library cannot be built for static linkage)

TEMPLATE = lib
TARGET   = fftreal

# FFTReal
HEADERS  += Array.h \
            Array.hpp \
            DynArray.h \
            DynArray.hpp \
            FFTRealFixLen.h \
            FFTRealFixLen.hpp \
            FFTRealFixLenParam.h \
            FFTRealPassDirect.h \
            FFTRealPassDirect.hpp \
            FFTRealPassInverse.h \
            FFTRealPassInverse.hpp \
            FFTRealSelect.h \
            FFTRealSelect.hpp \
            FFTRealUseTrigo.h \
            FFTRealUseTrigo.hpp \
            OscSinCos.h \
            OscSinCos.hpp \
            def.h
	    
# Wrapper used to export the required instantiation of the FFTRealFixLen template
HEADERS  += fftreal_wrapper.h
SOURCES  += fftreal_wrapper.cpp

DEFINES  += FFTREAL_LIBRARY

DESTDIR = ../..$${AudioSubdirs_build_dir}/lib


EXAMPLE_FILES = bwins/fftreal.def eabi/fftreal.def readme.txt license.txt

target.path = $$PWD/../3rdparty/fftreal
INSTALLS += target

CONFIG += install_ok  # Do not cargo-cult this!
