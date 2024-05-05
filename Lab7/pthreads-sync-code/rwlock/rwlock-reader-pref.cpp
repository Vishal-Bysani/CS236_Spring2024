#include "rwlock.h"

void InitalizeReadWriteLock(struct read_write_lock * rw)
{
  //	Write the code for initializing your read-write lock.
  rw->read_count = 0;
  rw->m = PTHREAD_MUTEX_INITIALIZER;
  rw->reader_can_enter = PTHREAD_COND_INITIALIZER;
  rw->writer_can_enter = PTHREAD_COND_INITIALIZER;
  rw->writer_present = false;
  rw->writer_waiting = 0;
}

void ReaderLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the reader.
  pthread_mutex_lock(&rw->m);
  while(rw->writer_present){
    pthread_cond_wait(&rw->reader_can_enter, &rw->m);
  }
  rw->read_count++;
  pthread_mutex_unlock(&rw->m);
}

void ReaderUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the reader.
  pthread_mutex_lock(&rw->m);
  rw->read_count -- ;
  if(rw->read_count == 0){
    pthread_cond_broadcast(&rw->writer_can_enter);
  }
  pthread_mutex_unlock(&rw->m);
}

void WriterLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the writer.
  pthread_mutex_lock(&rw->m);
  while(rw->read_count >0 || rw->writer_present){
    pthread_cond_wait(&rw->writer_can_enter, &rw->m);
  }
  rw->writer_present = true;
  pthread_mutex_unlock(&rw->m);
}

void WriterUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the writer.
  pthread_mutex_lock(&rw->m);
  rw->writer_present = false;
  pthread_cond_broadcast(&rw->reader_can_enter);
  pthread_cond_broadcast(&rw->writer_can_enter);
  pthread_mutex_unlock(&rw->m);
}
