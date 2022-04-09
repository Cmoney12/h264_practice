cmake_minimum_required(VERSION 3.21)
project(h264_practice)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )


###########################################################################


#set(LIBS x264 swscale avformat avcodec avutil swresample  # x264 and ffmpeg libs
#        pthread)


find_library(SWSCALE_LIBRARIES swscale
        HINTS ${PC_SWSCALE_LIBDIR} ${PC_SWSCALE_LIBRARY_DIRS}
        )

find_path(SWSCALE_INCLUDE_DIR swscale.h
        HINTS ${PC_SWSCALE_INCLUDEDIR} ${PC_SWSCALE_INCLUDE_DIRS}
        PATH_SUFFIXES libswscale
        )

find_package_handle_standard_args(Swscale "Could not find libswscale; available at www.ffmpeg.org" SWSCALE_LIBRARIES SWSCALE_INCLUDE_DIR)

mark_as_advanced(SWSCALE_INCLUDE_DIR SWSCALE_LIBRARIES)

find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
find_library(AVCODEC_LIBRARY avcodec)
IF (AVCODEC_INCLUDE_DIR AND AVCODEC_LIBRARY)
    SET(AVCODEC_FOUND 1)
ENDIF()

IF(AVCODEC_FOUND)
    MESSAGE(STATUS "FOUND AVCODEC" ${AVCODEC_INCLUDE_DIR} ${AVCODEC_LIBRARY})
ENDIF()

find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
find_library(AVFORMAT_LIBRARY avformat)

IF (AVFORMAT_INCLUDE_DIR AND AVFORMAT_LIBRARY)
    SET(AVFORMAT_FOUND 1)
ENDIF()

IF(AVFORMAT_FOUND)
    MESSAGE(STATUS "FOUND AVFORMAT" ${AVFORMAT_INCLUDE_DIR} ${AVFORMAT_LIBRARY})
ENDIF()

find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
find_library(AVUTIL_LIBRARY avutil)

IF (AVUTIL_INCLUDE_DIR AND AVUTIL_LIBRARY)
    SET(AVUTIL_FOUND 1)
ENDIF()

IF(AVUTIL_FOUND)
    MESSAGE(STATUS "FOUND AVUTIL" ${AVUTIL_INCLUDE_DIR} ${AVUTIL_LIBRARY})
ENDIF()


find_path(AVDEVICE_INCLUDE_DIR libavdevice/avdevice.h)
find_library(AVDEVICE_LIBRARY avdevice)

find_path(X264_INCLUDE_DIRS
        NAMES x264.h
        HINTS _X264_ROOT_PATHS
        PATH_SUFFIXES include
        )
if(X264_INCLUDE_DIRS)
    set(HAVE_X264_H 1)
endif()

find_library(X264_LIBRARIES
        NAMES x264
        HINTS _X264_ROOT_PATHS
        PATH_SUFFIXES bin lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(X264
        DEFAULT_MSG
        X264_INCLUDE_DIRS X264_LIBRARIES HAVE_X264_H)

mark_as_advanced(X264_INCLUDE_DIRS X264_LIBRARIES HAVE_X264_H)

add_executable(h264_practice main.cpp Encoder.h Decoder.h)

target_link_libraries(h264_practice ${OpenCV_LIBS} ${LIBS}
        ${AVCODEC_LIBRARY} ${AVFORMAT_LIBRARY} ${AVUTIL_LIBRARY} ${AVDEVICE_LIBRARY} ${SWSCALE_LIBRARIES} ${X264_LIBRARIES})
