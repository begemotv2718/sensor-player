#include "wav_file.h"


#define RIFF ((union header_rec){ .as_chr = "RIFF" })
#define WAVE ((union header_rec){ .as_chr = "WAVE" })
#define DATA ((union header_rec){ .as_chr = "data" })
#define FMT ((union header_rec){ .as_chr = "fmt " })

int wav_eof(struct wav_file *wf){
  return f_eof(wf->fp); 
}


int wav_fill_buffer_uint16(struct wav_file *wf, uint16_t *buf, uint16_t size){
  UINT actually_read;
  uint32_t read_ahead = wf->cur_chunk.size-wf->chunk_position;
  if(read_ahead>=size*sizeof(uint16_t)){
    f_read(wf->fp,buf,size*sizeof(uint16_t),&actually_read);
    wf->chunk_position+=actually_read;
    int i;
    for(i=0;i<size;i++){
      buf[i]^=0x8000;
    }
    return(actually_read == size*sizeof(uint16_t));
  }else{
    f_read(wf->fp,buf,read_ahead,&actually_read);
    int i;
    for(i=0;i<actually_read/sizeof(uint16_t);i++){
      buf[i]=buf[i]^0x8000;
    }
    size-=actually_read/sizeof(uint16_t);
    buf+=actually_read/sizeof(uint16_t);
    memset(buf,0,sizeof(uint16_t)*size);
    return 0;
  }
    
}

int wav_open(FIL *fp, struct wav_file *wf){
  UINT actually_read;
  union header_rec wave_header; 
  struct fmt_data fmt;

  wf->fp=fp;

  f_read(fp,&(wf->cur_chunk),sizeof(wf->cur_chunk),&actually_read);
  if(actually_read<sizeof(wf->cur_chunk) || wf->cur_chunk.head.as_int != RIFF.as_int) return 0;

  f_read(fp,&wave_header,sizeof(wave_header),&actually_read);
  if(actually_read<sizeof(wave_header) || wave_header.as_int != WAVE.as_int) return 0;

  
  f_read(fp,&(wf->cur_chunk),sizeof(wf->cur_chunk),&actually_read);
  if(actually_read<sizeof(wf->cur_chunk) || wf->cur_chunk.head.as_int != FMT.as_int) return 0;

  f_read(fp,&fmt,wf->cur_chunk.size,&actually_read);
  if(actually_read<wf->cur_chunk.size) return 0;

  if(fmt.format_tag != WAVE_FORMAT_PCM) return 0; 
  if(fmt.nchannels != 1) return 0;
  if(fmt.data_block_size !=2) return 0;

  wf->chunk_position = 0;
  wf->sample_rate = fmt.sample_rate;

  f_read(fp,&(wf->cur_chunk),sizeof(wf->cur_chunk),&actually_read);
  if(actually_read<sizeof(wf->cur_chunk) || wf->cur_chunk.head.as_int != DATA.as_int) return 0;

  return 1;
}
