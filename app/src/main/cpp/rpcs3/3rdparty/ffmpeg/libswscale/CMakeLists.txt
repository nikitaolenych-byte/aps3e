cmake_minimum_required(VERSION 3.10)
project(libswscale LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 11)

add_library(libswscale STATIC
        ${FFMPEG_DIR}/libswscale/aarch64/hscale.S
        ${FFMPEG_DIR}/libswscale/aarch64/output.S
        ${FFMPEG_DIR}/libswscale/aarch64/rgb2rgb.c
        ${FFMPEG_DIR}/libswscale/aarch64/rgb2rgb_neon.S
        ${FFMPEG_DIR}/libswscale/aarch64/swscale.c
        ${FFMPEG_DIR}/libswscale/aarch64/swscale_unscaled.c
        ${FFMPEG_DIR}/libswscale/aarch64/yuv2rgb_neon.S
        ${FFMPEG_DIR}/libswscale/alphablend.c
        ${FFMPEG_DIR}/libswscale/gamma.c
        ${FFMPEG_DIR}/libswscale/hscale.c
        ${FFMPEG_DIR}/libswscale/hscale_fast_bilinear.c
        ${FFMPEG_DIR}/libswscale/input.c
        ${FFMPEG_DIR}/libswscale/options.c
        ${FFMPEG_DIR}/libswscale/output.c
        ${FFMPEG_DIR}/libswscale/rgb2rgb.c
        ${FFMPEG_DIR}/libswscale/slice.c
        ${FFMPEG_DIR}/libswscale/swscale.c
        ${FFMPEG_DIR}/libswscale/swscale_unscaled.c
        ${FFMPEG_DIR}/libswscale/utils.c
        ${FFMPEG_DIR}/libswscale/version.c
        ${FFMPEG_DIR}/libswscale/vscale.c
        ${FFMPEG_DIR}/libswscale/yuv2rgb.c
)

target_include_directories(libswscale PRIVATE
        ${FFMPEG_DIR}
        ${FFMPEG_INC}
)

set_target_properties(libswscale PROPERTIES
        POSITION_INDEPENDENT_CODE ON
)

target_compile_options(libswscale PRIVATE
        -DHAVE_AV_CONFIG_H
)