#pragma once
#include <pthread.h>
typedef struct _Node {
	struct _Node *next;
	int data;
}Node;

typedef struct _RingBuf {
	Node *Head; //Read(OUTPUT)
	Node *Tail; //Write(INPUT)
}RingBuf;

typedef struct _Argument_Set{
	RingBuf *buf;
	int num_argm;
	int *argm;
}Ringbuf_set;

void RingBuf_init(RingBuf *buf);
int IsEmpty(RingBuf *buf);
int IsFull(RingBuf *buf);
int BufInput(RingBuf *buf, int data);
int BufOutput(RingBuf *buf);
int GetAllNodeData(RingBuf *buf);
