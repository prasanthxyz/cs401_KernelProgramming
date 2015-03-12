#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

#define QUEUE_SIZE 10

struct cqueue {
	char data[QUEUE_SIZE];
	int front;
	int rear;
	int counter;
};

void initCQ(struct cqueue *cq)
{
	cq->front = 0;
	cq->rear = -1;
	cq->counter = 0;
}

int putCQ(char x, struct cqueue *cq)
{
	if( (cq->front == 0 && cq->rear==(QUEUE_SIZE-1)) || (cq->front > 0 && cq->rear == cq->front-1) )
		return -1;
	cq->counter++;
	if(cq->rear == (QUEUE_SIZE-1) && cq->front > 0) {
		cq->rear = 0;
		cq->data[cq->rear] = x;
		return 0;
	}
	if( (cq->front == 0 && cq->rear == -1) || (cq->rear != cq->front-1) )
		cq->data[++cq->rear] = x;
	return 0;
}

char getCQ(struct cqueue *cq)
{
	char a;
	if(cq->front == 0 && cq->rear == -1)
		return (char)(-1);

	cq->counter--;
	if(cq->front == cq->rear) {
		a = cq->data[cq->front];
		cq->rear = -1;
		cq->front = 0;
		return a;
	}
	if(cq->front == (QUEUE_SIZE-1)) {
		a = cq->data[cq->front];
		cq->front = 0;
		return a;
	}
	a = cq->data[cq->front++];
	return a;
}

#endif
