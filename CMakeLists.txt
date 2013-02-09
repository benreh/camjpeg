cmake_minimum_required (VERSION 2.6) 
project (camjpeg)

include(FindLibJpeg.cmake)

FIND_PACKAGE( OpenCV REQUIRED )
find_package(Boost 1.40.0 REQUIRED thread)

add_executable (
	camjpeg
	main.cpp
	capture.cpp
	ipl2jpeg.cpp
	settings.cpp
	shm.cpp
	configfile/configfile.cpp
	)
TARGET_LINK_LIBRARIES( camjpeg ${OpenCV_LIBS} ${LibJpeg_LIBRARY} ${Boost_THREAD_LIBRARY})

file(COPY camjpeg.conf DESTINATION .)
