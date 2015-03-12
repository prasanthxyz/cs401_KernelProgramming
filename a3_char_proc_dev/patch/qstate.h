#ifndef QSTATE_H
#define QSTATE_H

struct q_state {
	int reads;
	int writes;
	int free_space;
	int front;
	int rear;
};

void initQState(struct q_state *QS, int queue_size)
{
	QS->reads = 0;
	QS->writes = 0;
	QS->free_space = queue_size;
	QS->front = 0;
	QS->rear = -1;
}

void write_QState(struct q_state *QS, int rear)
{
	QS->writes++;
	QS->free_space--;
	QS->rear = rear;
}

void read_QState(struct q_state *QS, int front, int rear)
{
	QS->reads++;
	QS->free_space++;
	QS->front = front;
	QS->rear = rear;
}

void get_QState(struct q_state qs, char *buf)
{
	sprintf(buf, "Reads       : %d\n\
Writes      : %d\n\
Free Space  : %d\n\
Front       : %d\n\
Rear        : %d\n\
", qs.reads, qs.writes, qs.free_space, qs.front, qs.rear);
}

void print_QState(struct q_state qs)
{
	printk(KERN_INFO "Reads       : %d\n", qs.reads);
	printk(KERN_INFO "Writes      : %d\n", qs.writes);
	printk(KERN_INFO "Free Space  : %d\n", qs.free_space);
	printk(KERN_INFO "Front       : %d\n", qs.front);
	printk(KERN_INFO "Rear        : %d\n", qs.rear);
}

#endif
