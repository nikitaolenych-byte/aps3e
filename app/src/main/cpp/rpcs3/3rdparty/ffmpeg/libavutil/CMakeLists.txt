cmake_minimum_required(VERSION 3.10)
project(libavutil LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 11)

add_library(libavutil STATIC
        ${FFMPEG_DIR}/libavutil/../compat/strtod.c
        ${FFMPEG_DIR}/libavutil/aarch64/cpu.c
        ${FFMPEG_DIR}/libavutil/aarch64/float_dsp_init.c
        ${FFMPEG_DIR}/libavutil/aarch64/float_dsp_neon.S
        ${FFMPEG_DIR}/libavutil/adler32.c
        ${FFMPEG_DIR}/libavutil/aes.c
        ${FFMPEG_DIR}/libavutil/aes_ctr.c
        ${FFMPEG_DIR}/libavutil/audio_fifo.c
        ${FFMPEG_DIR}/libavutil/avsscanf.c
        ${FFMPEG_DIR}/libavutil/avstring.c
        ${FFMPEG_DIR}/libavutil/base64.c
        ${FFMPEG_DIR}/libavutil/blowfish.c
        ${FFMPEG_DIR}/libavutil/bprint.c
        ${FFMPEG_DIR}/libavutil/buffer.c
        ${FFMPEG_DIR}/libavutil/camellia.c
        ${FFMPEG_DIR}/libavutil/cast5.c
        ${FFMPEG_DIR}/libavutil/channel_layout.c
        ${FFMPEG_DIR}/libavutil/color_utils.c
        ${FFMPEG_DIR}/libavutil/cpu.c
        ${FFMPEG_DIR}/libavutil/crc.c
        ${FFMPEG_DIR}/libavutil/csp.c
        ${FFMPEG_DIR}/libavutil/des.c
        ${FFMPEG_DIR}/libavutil/detection_bbox.c
        ${FFMPEG_DIR}/libavutil/dict.c
        ${FFMPEG_DIR}/libavutil/display.c
        ${FFMPEG_DIR}/libavutil/dovi_meta.c
        ${FFMPEG_DIR}/libavutil/downmix_info.c
        ${FFMPEG_DIR}/libavutil/encryption_info.c
        ${FFMPEG_DIR}/libavutil/error.c
        ${FFMPEG_DIR}/libavutil/eval.c
        ${FFMPEG_DIR}/libavutil/fifo.c
        ${FFMPEG_DIR}/libavutil/file.c
        ${FFMPEG_DIR}/libavutil/file_open.c
        ${FFMPEG_DIR}/libavutil/film_grain_params.c
        ${FFMPEG_DIR}/libavutil/fixed_dsp.c
        ${FFMPEG_DIR}/libavutil/float_dsp.c
        ${FFMPEG_DIR}/libavutil/frame.c
        ${FFMPEG_DIR}/libavutil/hash.c
        ${FFMPEG_DIR}/libavutil/hdr_dynamic_metadata.c
        ${FFMPEG_DIR}/libavutil/hdr_dynamic_vivid_metadata.c
        ${FFMPEG_DIR}/libavutil/hmac.c
        ${FFMPEG_DIR}/libavutil/hwcontext.c
        ${FFMPEG_DIR}/libavutil/hwcontext_stub.c
        ${FFMPEG_DIR}/libavutil/imgutils.c
        ${FFMPEG_DIR}/libavutil/integer.c
        ${FFMPEG_DIR}/libavutil/intmath.c
        ${FFMPEG_DIR}/libavutil/lfg.c
        ${FFMPEG_DIR}/libavutil/lls.c
        ${FFMPEG_DIR}/libavutil/log.c
        ${FFMPEG_DIR}/libavutil/log2_tab.c
        ${FFMPEG_DIR}/libavutil/lzo.c
        ${FFMPEG_DIR}/libavutil/mastering_display_metadata.c
        ${FFMPEG_DIR}/libavutil/mathematics.c
        ${FFMPEG_DIR}/libavutil/md5.c
        ${FFMPEG_DIR}/libavutil/mem.c
        ${FFMPEG_DIR}/libavutil/murmur3.c
        ${FFMPEG_DIR}/libavutil/opt.c
        ${FFMPEG_DIR}/libavutil/parseutils.c
        ${FFMPEG_DIR}/libavutil/pixdesc.c
        ${FFMPEG_DIR}/libavutil/pixelutils.c
        ${FFMPEG_DIR}/libavutil/random_seed.c
        ${FFMPEG_DIR}/libavutil/rational.c
        ${FFMPEG_DIR}/libavutil/rc4.c
        ${FFMPEG_DIR}/libavutil/reverse.c
        ${FFMPEG_DIR}/libavutil/ripemd.c
        ${FFMPEG_DIR}/libavutil/samplefmt.c
        ${FFMPEG_DIR}/libavutil/sha.c
        ${FFMPEG_DIR}/libavutil/sha512.c
        ${FFMPEG_DIR}/libavutil/slicethread.c
        ${FFMPEG_DIR}/libavutil/spherical.c
        ${FFMPEG_DIR}/libavutil/stereo3d.c
        ${FFMPEG_DIR}/libavutil/tea.c
        ${FFMPEG_DIR}/libavutil/threadmessage.c
        ${FFMPEG_DIR}/libavutil/time.c
        ${FFMPEG_DIR}/libavutil/timecode.c
        ${FFMPEG_DIR}/libavutil/tree.c
        ${FFMPEG_DIR}/libavutil/twofish.c
        ${FFMPEG_DIR}/libavutil/tx.c
        ${FFMPEG_DIR}/libavutil/tx_double.c
        ${FFMPEG_DIR}/libavutil/tx_float.c
        ${FFMPEG_DIR}/libavutil/tx_int32.c
        ${FFMPEG_DIR}/libavutil/utils.c
        ${FFMPEG_DIR}/libavutil/uuid.c
        ${FFMPEG_DIR}/libavutil/version.c
        ${FFMPEG_DIR}/libavutil/video_enc_params.c
        ${FFMPEG_DIR}/libavutil/xga_font_data.c
        ${FFMPEG_DIR}/libavutil/xtea.c
)

target_include_directories(libavutil PRIVATE
        ${FFMPEG_DIR}
        ${FFMPEG_INC}
)

set_target_properties(libavutil PROPERTIES
        POSITION_INDEPENDENT_CODE ON
)

target_compile_options(libavutil PRIVATE
        -DHAVE_AV_CONFIG_H
)