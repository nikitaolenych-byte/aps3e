cmake_minimum_required(VERSION 3.10)
project(libavdevice LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 11)

add_library(libavdevice STATIC
        ${FFMPEG_DIR}/libavdevice/alldevices.c
        ${FFMPEG_DIR}/libavdevice/android_camera.c
        ${FFMPEG_DIR}/libavdevice/avdevice.c
        ${FFMPEG_DIR}/libavdevice/fbdev_common.c
        ${FFMPEG_DIR}/libavdevice/fbdev_dec.c
        ${FFMPEG_DIR}/libavdevice/fbdev_enc.c
        ${FFMPEG_DIR}/libavdevice/lavfi.c
        ${FFMPEG_DIR}/libavdevice/timefilter.c
        ${FFMPEG_DIR}/libavdevice/utils.c
        ${FFMPEG_DIR}/libavdevice/v4l2-common.c
        ${FFMPEG_DIR}/libavdevice/v4l2.c
        ${FFMPEG_DIR}/libavdevice/v4l2enc.c
        ${FFMPEG_DIR}/libavdevice/version.c
)
target_include_directories(libavdevice PRIVATE
        ${FFMPEG_DIR}
        ${FFMPEG_INC}
)

set_target_properties(libavdevice PROPERTIES
        POSITION_INDEPENDENT_CODE ON
)

target_compile_options(libavdevice PRIVATE
        -DHAVE_AV_CONFIG_H
)