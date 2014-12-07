#ifndef __QUEUE_H
#define __QUEUE_H
#define QUEUE_SIZE 200

struct Queue {
  int pRD, pWR;
  uint8_t q[QUEUE_SIZE];
};

inline int QueueFull(struct Queue *q){
  return  (q->pWR+1)%QUEUE_SIZE == q->pRD;
}
inline int QueueEmpty(struct Queue *q){
  return  q->pWR == q->pRD;
}

int Enqueue(struct Queue *q, uint8_t data);
int Dequeue(struct Queue *q, uint8_t *data);
void InitQueue(struct Queue *q);
#endif
