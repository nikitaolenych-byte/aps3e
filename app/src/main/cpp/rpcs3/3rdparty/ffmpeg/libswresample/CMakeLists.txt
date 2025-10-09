cmake_minimum_required(VERSION 3.10)
project(libswresample LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 11)

add_library(libswresample STATIC
        ${FFMPEG_DIR}/libswresample/aarch64/audio_convert_init.c
        ${FFMPEG_DIR}/libswresample/aarch64/audio_convert_neon.S
        ${FFMPEG_DIR}/libswresample/aarch64/resample_init.c
        ${FFMPEG_DIR}/libswresample/aarch64/resample.S
        ${FFMPEG_DIR}/libswresample/audioconvert.c
        ${FFMPEG_DIR}/libswresample/dither.c
        ${FFMPEG_DIR}/libswresample/options.c
        ${FFMPEG_DIR}/libswresample/rematrix.c
        ${FFMPEG_DIR}/libswresample/resample.c
        ${FFMPEG_DIR}/libswresample/resample_dsp.c
        ${FFMPEG_DIR}/libswresample/swresample.c
        ${FFMPEG_DIR}/libswresample/swresample_frame.c
        ${FFMPEG_DIR}/libswresample/version.c
)

target_include_directories(libswresample PRIVATE
        ${FFMPEG_DIR}
        ${FFMPEG_INC}
)

set_target_properties(libswresample PROPERTIES
        POSITION_INDEPENDENT_CODE ON
)

target_compile_options(libswresample PRIVATE
        -DHAVE_AV_CONFIG_H
)