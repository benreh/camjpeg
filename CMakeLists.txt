cmake_minimum_required (VERSION 2.6) 
project (camjpeg)

include(FindLibJpeg.cmake)

FIND_PACKAGE( OpenCV REQUIRED )
#~ FIND_PACKAGE( libjpeg REQUIRED )
add_executable (camjpeg main.cpp capture.cpp ipl2jpeg settings)
TARGET_LINK_LIBRARIES( camjpeg ${OpenCV_LIBS} ${LibJpeg_LIBRARY})

