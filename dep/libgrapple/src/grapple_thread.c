/*
  Grapple - A fully featured network layer with a simple interface
  Copyright (C) 2006 Michael Simms
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
  Michael Simms
  michael@linuxgamepublishing.com
*/

#include "grapple_configure_substitute.h"

#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif

#include "grapple_thread.h"
#include "tools.h"


//This function creates a new thread
grapple_thread *grapple_thread_create(void *(*function)(void*),
				      void *context)
{
  grapple_thread *thread;
  int createval=-1;

  thread=(grapple_thread *)calloc(1,sizeof(grapple_thread));

  //Create the thread
  while(createval!=0)
    {
      createval=pthread_create(&thread->thread,NULL,
			       function,context);

      if (createval!=0)
	{
	  if (grapple_errno()!=EAGAIN)
	    {
	      free(thread);
	      //Problem creating the thread that isnt a case of 'it will work
	      //later, dont create it
	      return NULL;
	    }
	}
    }

  pthread_detach(thread->thread);

  return thread;
}

int grapple_thread_destroy(grapple_thread *thread)
{
  if (thread)
    free(thread);

  return 0;
}

//Create a new lock
grapple_thread_mutex *grapple_thread_mutex_init()
{
  grapple_thread_mutex *mutex;
  pthread_mutexattr_t attr;

  //Initialise the memory
  mutex=(grapple_thread_mutex *)calloc(1,sizeof(grapple_thread_mutex));
  
  //Create the lock. This is not recursive, as the locks actually dont last
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

  pthread_mutex_init(&mutex->mutex,&attr);
  pthread_mutex_init(&mutex->countmutex,&attr);

  //The counter for the locks
  mutex->lockcount=0;
  mutex->exlockcount=0;

  return mutex;
}

//Destroy a lock
int grapple_thread_mutex_destroy(grapple_thread_mutex *mutex)
{
  if (!mutex)
    return 0;

  //Delete the mutex
  pthread_mutex_destroy(&mutex->mutex);
  pthread_mutex_destroy(&mutex->countmutex);

  //Free the memory
  free(mutex);

  return 0;
}

//Lock a mutex. This can be in one of two ways
// 1) GRAPPLE_LOCKTYPE_EXCLUSIVE
//      This will lock and stop others locking. If shared mutexes are accessing
//      the same mutex, it will wait for those accesses to finish
// 2) GRAPPLE_LOCKTYPE_SHARED
//      This will lock a mutex if it ISNT exclusively locked. If it is it will
//      wait for the lock to expire. If other shared locks are there, it will
//      lock in a manner that allows them all to continue

int grapple_thread_mutex_lock(grapple_thread_mutex *mutex,
			      grapple_mutex_locktype locktype)
{
  int count;

  switch (locktype)
    {
    case GRAPPLE_LOCKTYPE_EXCLUSIVE:
      count=0;

      //Lock the thread
      pthread_mutex_lock(&mutex->mutex);
      //If any locks are already using this, shared
      while (mutex->lockcount > 0)
	{
	  //Wait for them to finish
	  count++;
	  if (count > 10)
	    {
	      //Unlock and lock in case there is a deadlock waiting to be
	      //handled
	      pthread_mutex_unlock(&mutex->mutex);
	      microsleep(1);
	      pthread_mutex_lock(&mutex->mutex);
	      count=0;
	    }
	  else
	    microsleep(100);

	}

      //Note we are using this exclusively
      pthread_mutex_lock(&mutex->countmutex);
      mutex->exlockcount++;
      pthread_mutex_unlock(&mutex->countmutex);
      break;

    case GRAPPLE_LOCKTYPE_SHARED:

      //Lock this, if there is an exclusive lock this will wait here
      pthread_mutex_lock(&mutex->mutex);
      //Incriment the counter
      pthread_mutex_lock(&mutex->countmutex);
      mutex->lockcount++;
      pthread_mutex_unlock(&mutex->countmutex);
      //Unlock the mutex, we are good to do this, it isnt exclusive
      pthread_mutex_unlock(&mutex->mutex);
      break;
    }

  return 0;
}

//TRY and lock a mutex, as above.
//In either case, if the lock succeded, it returns 0, otherwise an error code
//as per pthread_mutex_trylock
int grapple_thread_mutex_trylock(grapple_thread_mutex *mutex,
				 grapple_mutex_locktype locktype)
{
  int returnval=0;
  int count;

  switch (locktype)
    {
    case GRAPPLE_LOCKTYPE_EXCLUSIVE:
      returnval=pthread_mutex_trylock(&mutex->mutex);
      if (returnval==0)
	{
	  count=0;
	  while (returnval==0 && mutex->lockcount > 0)
	    {
	      //Wait for them to finish
	      count++;
	      if (count > 10)
		{
		  //Unlock and lock in case there is a deadlock waiting to be
		  //handled
		  pthread_mutex_unlock(&mutex->mutex);
		  microsleep(1);
		  returnval=pthread_mutex_trylock(&mutex->mutex);
		  count=0;
		}
	      else
		microsleep(100);

	    }
	  pthread_mutex_lock(&mutex->countmutex);
	  mutex->exlockcount++;
	  pthread_mutex_unlock(&mutex->countmutex);
	}
      break;
    case GRAPPLE_LOCKTYPE_SHARED:
      returnval=pthread_mutex_trylock(&mutex->mutex);
      if (returnval==0)
	{
	  pthread_mutex_lock(&mutex->countmutex);
	  mutex->lockcount++;
	  pthread_mutex_unlock(&mutex->countmutex);
	  pthread_mutex_unlock(&mutex->mutex);
	}
      break;
    }

  return returnval;
}

//This function unlocks a lock
int grapple_thread_mutex_unlock(grapple_thread_mutex *mutex)
{
  //Check the type of lock
  if (mutex->exlockcount>0)
    {
      //Its exclusive, unlock it, allow others in
      pthread_mutex_lock(&mutex->countmutex);
      mutex->exlockcount--;
      pthread_mutex_unlock(&mutex->countmutex);
      pthread_mutex_unlock(&mutex->mutex);
    }
  else if (mutex->lockcount>0)
    {
      //It is shared, just decriment the lock type
      pthread_mutex_lock(&mutex->countmutex);
      mutex->lockcount--;
      pthread_mutex_unlock(&mutex->countmutex);
    }
  //Otherwise, this is an error
  else
    {
      return -1;
    }
  return 0;
}

int debug_grapple_thread_mutex_lock(grapple_thread_mutex *mutex,
				    grapple_mutex_locktype locktype,
				    const char *file,int line)
{
  switch (locktype)
    {
    case GRAPPLE_LOCKTYPE_EXCLUSIVE:
      printf("EXlock on %p %s:%d\n",mutex,file,line);
      break;
    case GRAPPLE_LOCKTYPE_SHARED:
      printf("SHlock on %p %s:%d\n",mutex,file,line);
      break;
    }

  return grapple_thread_mutex_lock(mutex,locktype);
}

int debug_grapple_thread_mutex_unlock(grapple_thread_mutex *mutex,
				      const char *file,int line)
{
  printf("UNlock on %p %s:%d\n",mutex,file,line);
  return grapple_thread_mutex_unlock(mutex);
}
