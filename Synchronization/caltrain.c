#include "pintos_thread.h"

/*

1 station_on_board k phai thread, no chi la 1 function de call thoi
2 1 thread la 1 passenger va chi co 1 passenger on board tai 1 thoi diem, tuc la tau den, broadcast tat ca thread hanh khach, se co thang len truoc, thang len truoc on board xong se notify nhung thang khach khac de no di len tung thang` 

*/


typedef struct lock lock_t;
typedef struct condition condition_t;

struct station {
	// FILL ME IN
	//int has_train;
	int has_passenger;
	lock_t lck;
	condition_t cond;
};

static int passengers = 0;
static int train_seat_full = 0;
static int train_seat_current = 0;

void
station_init(struct station *station)
{
	// FILL ME IN
	station->has_passenger = 0; //passengers have not came yet
	station->lck.init = LOCK_COND_INIT_MAGIC;
	station->cond.init = LOCK_COND_INIT_MAGIC;
	lock_init(&station->lck);
	cond_init(&station->cond);
}

void
station_load_train(struct station *station, int count)
{
	// FILL ME IN

	//printf("station->has_passenger %d\n",station->has_passenger);
	if(!count){
		train_seat_full = train_seat_current = 0;
		return;
	}
	lock_acquire(&station->lck);

	train_seat_full = count;

	if(!station->has_passenger){
		train_seat_full = train_seat_current = 0;
		lock_release(&station->lck);
		return;
	}
	cond_signal(&station->cond,&station->lck);  
	while(train_seat_full != train_seat_current){ // wait until all passengers on board and/or seat full
		cond_wait(&station->cond,&station->lck);  
	}
	lock_release(&station->lck);
}

void
station_wait_for_train(struct station *station)
{
	// FILL ME IN
	lock_acquire(&station->lck);
	station->has_passenger = 1;
	//	printf("WAKE up %d - %d\n",train_seat_full,train_seat_current);
	passengers++;
	while(train_seat_full == train_seat_current){
		cond_wait(&station->cond,&station->lck);
		//printf("WAKE up %d - %d\n",train_seat_full,train_seat_current);
	}
	lock_release(&station->lck);
}

void
station_on_board(struct station *station)
{
	// FILL ME IN

	lock_acquire(&station->lck);
	passengers--;
	// printf(">>>>>> %d\n",passengers );
	train_seat_current++;
	if(train_seat_full != train_seat_current && passengers){ //still have empty seats and still have passengers 
		cond_signal(&station->cond,&station->lck); // one passenger on board, wake up another passenger
	}
	else{ //wake up the train thread for departing
		train_seat_full = train_seat_current = 0;
		cond_broadcast(&station->cond,&station->lck); //don't know which is train thread and passenger thread so wake up all
	}
	// printf("AAAAAA %d\n", train_seat_current);
	lock_release(&station->lck);	
}
