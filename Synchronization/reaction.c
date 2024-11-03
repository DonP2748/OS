#include "pintos_thread.h"

// Forward declaration. This function is implemented in reaction-runner.c,
// but you needn't care what it does. Just be sure it's called when
// appropriate within reaction_o()/reaction_h().
void make_water();



typedef struct lock lock_t;
typedef struct condition condition_t;

struct reaction {
	// FILL ME IN
	lock_t lck;
	condition_t reaction_occurred_h;
	condition_t new_h_arrival;
};


static int H = 0;
static int O = 0;


void
reaction_init(struct reaction *reaction)
{
	// FILL ME IN
	reaction->lck.init = LOCK_COND_INIT_MAGIC;
	reaction->reaction_occurred_h.init = LOCK_COND_INIT_MAGIC;
	reaction->new_h_arrival.init = LOCK_COND_INIT_MAGIC;
	lock_init(&reaction->lck);
	cond_init(&reaction->reaction_occurred_h);
	cond_init(&reaction->new_h_arrival);
}

void
reaction_h(struct reaction *reaction)
{
	// FILL ME IN
	lock_acquire(&reaction->lck);
	H++;
	cond_signal(&reaction->new_h_arrival,&reaction->lck); 
	cond_wait(&reaction->reaction_occurred_h,&reaction->lck); 
	lock_release(&reaction->lck);
}

void
reaction_o(struct reaction *reaction)
{
	// FILL ME IN
 	lock_acquire(&reaction->lck);
 	O++;	
// 	printf("CREATE O %d\n",O );
 	while(H < 2){
 		cond_wait(&reaction->new_h_arrival,&reaction->lck);
 	}
 	make_water();
	H--;
	O--;
	H--; 
	cond_signal(&reaction->reaction_occurred_h,&reaction->lck);
	cond_signal(&reaction->reaction_occurred_h,&reaction->lck);
//	printf("DELETE O\n");
 	lock_release(&reaction->lck);
}



