#include <stdio.h>
#include <stdlib.h>
#include "RingBuffer.h"
#include "util.h"

#define MAX_SIZE 100 

int GetAllNodeData(RingBuf *buf) { 
	Node *RefNode;
	Node *TailNode;
	int cnt = 1;

	if (IsEmpty(buf)) {
		return 0;
	}
	if (IsFull(buf) == -1) {
		return -1;
	}

	RefNode = buf->Tail;
	TailNode = RefNode;

	RefNode = RefNode->next;
	while (1) {
		printf("Node %d: %d\n", cnt, RefNode->data);
		RefNode = RefNode->next;

		if (RefNode == TailNode) {
			cnt++;
			if (RefNode->next == RefNode) {
				break;
			}

			printf("Node %d: %d\n", cnt, RefNode->data);
			break;
		}
		cnt++;
	}

	return cnt;
}

void RingBuf_init(RingBuf *buf) {
	buf->Head = NULL;
	buf->Tail = NULL;
}

int IsEmpty(RingBuf *buf) {
	if (buf->Tail == NULL) {
		return 1;
	}
	else {
		return 0;
	}
}

int IsFull(RingBuf *buf) {
	Node *RefNode = buf->Tail;
	Node *TailNode = RefNode;
	int cnt = 1;

	if (IsEmpty(buf)) {
		return 0;
	}

	RefNode = RefNode->next;
	while (1) {
		RefNode = RefNode->next;
		if (RefNode == TailNode) {
			cnt++;
			if (RefNode->next == RefNode) {
				break;
			}
			break;
		}
		cnt++;
	}

	if (cnt == MAX_SIZE) {
		return 1;
	}
	else if (cnt > MAX_SIZE) {
		return -1;
	}
	
	return 0;
}

int BufInput(RingBuf *buf, int data) {
	Node *NewNode = (Node*)malloc(sizeof(Node));
	Node *RefNode; 
	Node *tmpNode; 
	Node *TailNode; 
	int retval = 0;

	retval = IsFull(buf);

	if (retval) {
		errH("BufInput", "Buffer is full");
		return -1;
	}
	else if (retval == -1) {
		errH("BufInput","Buffer Overrun");
		return -1;
	}

	NewNode->data = data;

	if (IsEmpty(buf)) { 
		buf->Tail = NewNode;
		NewNode->next = NewNode;
	}
	else if ((buf->Tail != NULL) && (buf->Head == NULL)) {
		RefNode = buf->Tail;
		TailNode = RefNode;

		tmpNode = TailNode->next;
		TailNode->next = NewNode;
		NewNode->next = tmpNode;

		buf->Head = tmpNode;
		buf->Tail = NewNode;
	}
	else {
		RefNode = buf->Tail; 
		TailNode = RefNode; 

		tmpNode = TailNode->next;
		TailNode->next = NewNode;
		NewNode->next = tmpNode;
		buf->Tail = NewNode;
	}
	
	return 0;
}

int BufOutput(RingBuf *buf) {
	Node *RefNode = buf->Tail;
	Node *TailNode = RefNode;
	Node *DelNode;

	int cnt = 1;
	int retval = 0;

	if (IsEmpty(buf)) {
		errH("BufOutput", "Buffer is empty");
		return -1;
	}
	if (IsFull(buf) == -1) {
		errH("BufOutput", "Buffer Overrun");
		return -1;
	}

	RefNode = RefNode->next;
	while (1) {
		RefNode = RefNode->next;
		if (RefNode == TailNode) {
			cnt++;

			if (RefNode->next == RefNode) {
				retval = buf->Tail->data;
				free(buf->Tail);

				buf->Tail = NULL;
				return retval;
			}
			break;
		}
		cnt++;
	}

	if (cnt == 2) {
		retval = buf->Head->data;
	
		buf->Tail->next = buf->Tail;
		free(buf->Head);

		buf->Head = NULL;
	}
	else {
		RefNode = buf->Tail;
		retval = buf->Head->data;
		DelNode = RefNode->next;

		RefNode->next = RefNode->next->next;
		buf->Head = RefNode->next;

		free(DelNode);
	}

	return retval;
}
