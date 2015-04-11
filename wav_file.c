#include "ff.h"
union header_rec {
  uint32_t as_int,
  char[4] as_chr
};

struct wav_chunk {
  union header_rec head,
  uint32_t size,
  uint32_t cur_position,
  struct wav_chunk *parent
} 

struct wav_file {
  FIL *fp;
  uint16_t sample_rate;
  struct wav_chunk *cur_chunk;
};

int wav_eof(struct wav_file *wf){
  struct wav_chunk *cur_chunk = wf->cur_chunk;
  while(cur_chunk->head.as_int != RIFF_HEAD){
    if(cur_chunk->cur_position<cur_chunk->size){
      return 0;
    }
    cur_chunk=cur_chunk->parent;
  }
  return 1;
}
    

int wav_fill_buffer_uint16(struct wav_file *wf, uint16_t *buf, uint16_t size){
  uint32_t actually_read;
  uint32_t read_ahead = wf->cur_chunk->size-wf->cur_chunk->cur_position;
  if(read_ahead>=size){
    f_read(wf->fp,buf,size,&actually_read);
    wf->cur_chunk->cur_position+=actually_read;
    return(actually_read == size);
  }else{
    f_read(wf->fp,buf,read_ahead,&actually_read);
    if(actually_read==read_ahead){
      buf+=
      size-=actually_read;
      wf->cur_chunk->parent->cur_position+=wf->cur_chunk->size+sizeof(header_rec);

      f_read(fp,wf->cur_chunk,sizeof(header_rec)+sizeof(uint32_t),&actually_read);
      if(wf->cur_chunk->

    
}

int wav_open(FIL *fp, struct wav_file *wf){
}
