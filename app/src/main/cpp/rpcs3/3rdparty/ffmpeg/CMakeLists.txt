
set(FFMPEG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-5.1.7")
set(FFMPEG_INC "${CMAKE_CURRENT_SOURCE_DIR}/include/android_aarch64")

add_subdirectory(libavdevice EXCLUDE_FROM_ALL)
add_subdirectory(libavfilter EXCLUDE_FROM_ALL)
add_subdirectory(libavformat EXCLUDE_FROM_ALL)
add_subdirectory(libavcodec EXCLUDE_FROM_ALL)
add_subdirectory(libswresample EXCLUDE_FROM_ALL)
add_subdirectory(libswscale EXCLUDE_FROM_ALL)
add_subdirectory(libavutil EXCLUDE_FROM_ALL)