#ifndef WAV_FILE_H
#define WAV_FILE_H
#include "fatfs/ff.h"
#include "stm32f10x.h"
#include "xprintf.h"
#include <string.h>
union header_rec {
  uint32_t as_int;
  char as_chr[4];
};

struct wav_chunk {
  union header_rec head;
  uint32_t size;
}; 

struct fmt_data {
  uint16_t format_tag;
  uint16_t nchannels;
  uint32_t sample_rate;
  uint32_t avg_bytes_sec;
  uint16_t data_block_size;
  uint16_t bits_sample;
  uint16_t extension_size;
  uint16_t valid_bits_sample;
  uint32_t channel_mask;
  uint8_t sub_format[16];
};

#define WAVE_FORMAT_PCM 0x0001
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#define WAVE_FORMAT_ALAW 0x0006
#define WAVE_FORMAT_MULAW 0x0007
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE

struct wav_file {
  FIL *fp;
  uint16_t sample_rate;
  uint32_t chunk_position;
  struct wav_chunk cur_chunk;
};

int wav_eof(struct wav_file *wf);

int wav_fill_buffer_uint16(struct wav_file *wf, uint16_t *buf, uint16_t size);

int wav_open(FIL *fp, struct wav_file *wf);
#endif
