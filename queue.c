#include "stm32f10x.h"
#include "queue.h"
#include <string.h>

int Enqueue(struct Queue *q, uint8_t data){
  if(QueueFull(q)){
    return 0;
  }
  q->q[q->pWR]=data;
  /*Note interrupting this here with another Enqueue will be disastrous!!!! Use cli*/
  q->pWR=(q->pWR>=(QUEUE_SIZE-1))?0:q->pWR+1;
  return 1;
}

int Deque(struct Queue *q, uint8_t *data){
  if(QueueEmpty(q)){
    return 0;
  }
  *data=q->q[q->pRD];
  q->pRD =  (q->pRD>= (QUEUE_SIZE -1))? 0: q->pRD+1;
  return 1;
}

void InitQueue(struct Queue *q){
  memset(q,0,sizeof(struct Queue));
}
