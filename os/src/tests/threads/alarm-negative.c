/* Tests timer_sleep(-100).  Only requirement is that it not crash. */



#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "devices/timer.h"

#define TICK  100000000

struct lock lock1 = {0};

struct lock lock2 = {0};




void task1(void* arg)
{
  int tick = TICK;
  struct thread* cur = thread_current();
  // DEBUG("%s with priority %i start\n", cur->name, cur->priority);
  while(1){
    // DEBUG("%s is running!\n",cur->name);
    lock_acquire(&lock1);
    lock_acquire(&lock2);
    while(tick--);
    tick = TICK;
    lock_release(&lock2);
    lock_release(&lock1);    
  }
  // DEBUG("%s End!\n", cur->name);
}

void task2(void* arg)
{
  int tick = TICK;
  struct thread* cur = thread_current();
  // DEBUG("%s with priority %i start\n", cur->name, cur->priority);
  // DEBUG("%s yield!\n",cur->name);
  //timer_msleep(100);
  while(1){
    // DEBUG("%s is running!\n",cur->name);
    lock_acquire(&lock2);
    timer_msleep(100);
    while(tick--);
    tick = TICK;  
    lock_release(&lock2);
  }

  // DEBUG("%s End!\n", cur->name);
}

void task3(void* arg)
{
  int tick = TICK;
  struct thread* cur = thread_current();
  // DEBUG("%s with priority %i start\n", cur->name, cur->priority);
  // DEBUG("%s yield!\n",cur->name);

  timer_msleep(100);
  while(1){
    // DEBUG("%s is running!\n",cur->name);
    lock_acquire(&lock1);
    while(tick--);
    tick = TICK;
    lock_release(&lock1);    
  }

  // DEBUG("%s End!\n", cur->name);
}

void
test_alarm_negative (void) 
{
  //init 
  lock_init(&lock1);
  lock_init(&lock2);

  timer_sleep (-100);
  //thread_create (const char *name, int priority, task *, void *);
  thread_create("thread1", 8, task1, NULL);
  thread_create("thread2", 18, task2, NULL);
  thread_create("thread3", 28, task3, NULL);
  //thread_create("thread1", 8, task, NULL);
  // DEBUG("main hang there\n");
  timer_msleep(10000);

  pass ();
}





/*












// ERROR: DEADLOCK H M L CASE 2 LOCK


#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "devices/timer.h"

#define TICK  100000000

struct lock lock1 = {0};

struct lock lock2 = {0};




void task1(void* arg)
{
  int tick = TICK;
  struct thread* cur = thread_current();
  DEBUG("%s with priority %i start\n", cur->name, cur->priority);
  while(1){
    DEBUG("%s is running!\n",cur->name);
    lock_acquire(&lock1);
    lock_acquire(&lock2);
    while(tick--);
    tick = TICK;
    lock_release(&lock2);
    lock_release(&lock1);    
  }
  DEBUG("%s End!\n", cur->name);
}

void task2(void* arg)
{
  int tick = TICK;
  struct thread* cur = thread_current();
  DEBUG("%s with priority %i start\n", cur->name, cur->priority);
  DEBUG("%s yield!\n",cur->name);
  //timer_msleep(100);
  while(1){
    DEBUG("%s is running!\n",cur->name);
    lock_acquire(&lock2);
    timer_msleep(100);
    while(tick--);
    tick = TICK;  
    lock_release(&lock2);
  }

  DEBUG("%s End!\n", cur->name);
}

void task3(void* arg)
{
  int tick = TICK;
  struct thread* cur = thread_current();
  DEBUG("%s with priority %i start\n", cur->name, cur->priority);
  DEBUG("%s yield!\n",cur->name);

  timer_msleep(100);
  while(1){
    DEBUG("%s is running!\n",cur->name);
    lock_acquire(&lock1);
    while(tick--);
    tick = TICK;
    lock_release(&lock1);    
  }

  DEBUG("%s End!\n", cur->name);
}

void
test_alarm_negative (void) 
{
  //init 
  lock_init(&lock1);
  lock_init(&lock2);

  timer_sleep (-100);
  //thread_create (const char *name, int priority, task *, void *);
  thread_create("thread1", 8, task1, NULL);
  thread_create("thread2", 18, task2, NULL);
  thread_create("thread3", 28, task3, NULL);
  //thread_create("thread1", 8, task, NULL);
  DEBUG("main hang there\n");
  timer_msleep(10000);

  pass ();
}



// ERROR: DEADLOCK H M L CASE 1 LOCK

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "devices/timer.h"

#define TICK  100000000

struct lock lock1 = {0};

struct lock lock2 = {0};




void task1(void* arg)
{
  int tick = TICK;
  struct thread* cur = thread_current();
  DEBUG("%s with priority %i start\n", cur->name, cur->priority);

  while(1){
    tick = TICK;
    DEBUG("%s is running with priority %i!\n",cur->name,cur->priority);
    //never get lock success if do this outside the critical section
    //while(tick--);
    lock_acquire(&lock1);
    while(tick--);
    lock_release(&lock1);    
  }
  DEBUG("%s End!\n", cur->name);
}

void task2(void* arg)
{
  int tick = TICK;
  struct thread* cur = thread_current();
  DEBUG("%s with priority %i start\n", cur->name, cur->priority);
  DEBUG("%s yield!\n",cur->name);
  timer_msleep(100);
  while(1){
    tick = TICK;  
    DEBUG("%s is running with priority %i!\n",cur->name,cur->priority);
    while(tick--);
  }

  DEBUG("%s End!\n", cur->name);
}

void task3(void* arg)
{
  int tick = TICK;
  struct thread* cur = thread_current();
  DEBUG("%s with priority %i start\n", cur->name, cur->priority);
  DEBUG("%s yield!\n",cur->name);
  timer_msleep(100);
  while(1){
    tick = TICK;
    DEBUG("%s is running with priority %i!\n",cur->name,cur->priority);
    lock_acquire(&lock1);
    while(tick--);
    lock_release(&lock1);    
  }

  DEBUG("%s End!\n", cur->name);
}

void
test_alarm_negative (void) 
{
  //init 
  lock_init(&lock1);
  lock_init(&lock2);

  timer_sleep (-100);
  thread_create("thread1", 8, task1, NULL);
  thread_create("thread2", 18, task2, NULL);
  thread_create("thread3", 28, task3, NULL);
  DEBUG("main end there\n");
//  timer_msleep(10000);

  pass ();
}







//PRIORITY PREEMPTION

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "devices/timer.h"

#define TICK  10000000000

struct lock _lock = {0};
int tick = TICK;

void task(void* arg)
{
  struct thread* cur = thread_current();
  DEBUG("%s with priority %i\n", cur->name, cur->priority);
  DEBUG("%s Take the lock!\n",cur->name);
  lock_acquire(&_lock);
  DEBUG("%s Hold the lock\n", cur->name);

  while(tick--);
  tick = TICK;
  lock_release(&_lock);
  DEBUG("%s release the lock!\n",cur->name);
}


void
test_alarm_negative (void) 
{
  //init 
  lock_init(&_lock);

  timer_sleep (-100);
  //thread_create (const char *name, int priority, task *, void *);
  thread_create("thread1", 8, task, NULL);
  thread_create("thread2", 18, task, NULL);
  thread_create("thread3", 28, task, NULL);
  //thread_create("thread1", 8, task, NULL);
  DEBUG("main hang there\n");
  timer_msleep(10000);

  pass ();
}


//ORIGIN

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "devices/timer.h"

void
test_alarm_negative (void) 
{
  timer_sleep (-100);
  pass ();
}



*/