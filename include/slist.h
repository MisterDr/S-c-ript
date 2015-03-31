struct list_int {
   int val;
   struct list_int * next;
};

typedef struct list_int intitem;

/***********************************
*   Add item to the list           *
************************************/
#define add_intitem(value)\
	curr = (item *)malloc(sizeof(item));\
	curr->val = value;\
	curr->next = tail;\
	tail = curr;\
		if(!head) head = curr;