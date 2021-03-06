/*
 *    <one line to give the program's name and a brief idea of what it does.>
 *    Copyright (C) <year>  <name of author>
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>
 */
 
#include <types.h>

#include <task/task.h>
#include <task/waitqueue.h>

#include <mm/salloc.h>

#include <misc/list.h>

#include <kctx.h>

struct wait_queue *
wait_queue_new (void)
{
  CONSTRUCTOR_BODY_OF_STRUCT (wait_queue);
}

struct waiting_task_info *
waiting_task_info_new (void)
{
  CONSTRUCTOR_BODY_OF_STRUCT (waiting_task_info);
}

int
wait_queue_destroy (struct wait_queue *wq)
{
  if (wq->wq_queue != NULL)
    return KERNEL_ERROR_VALUE;
    
  sfree (wq);
  
  return KERNEL_SUCCESS_VALUE;
}

int
wait_queue_put_task (struct wait_queue *wq, struct task *task)
{
  struct waiting_task_info *info;

  ASSERT (task);
  
  spin_lock (&wq->wq_lock);
  
  if (FAILED_PTR (info = waiting_task_info_new ()))
  {
    spin_unlock (&wq->wq_lock);
    
    return KERNEL_ERROR_VALUE;
  }
  
  info->wt_task = task;
  
  list_insert_tail ((void **) &wq->wq_queue, (void *) info);
  
  spin_unlock (&wq->wq_lock);
  
  return KERNEL_SUCCESS_VALUE;
}

struct waiting_task_info *
wait_queue_lookup_task (struct wait_queue *wq, struct task *task)
{
  struct list_head *this;
  
  ASSERT ((void **) wq->wq_queue != NULL);
  
  PTR_RETURN_ON_PTR_FAILURE (this = LIST_HEAD (*((void **) &wq->wq_queue)));
  
  while (this)
  {
    if (((struct waiting_task_info *) this)->wt_task == task)
      return (struct waiting_task_info *) this;
      
    this = this->next;
  }
  
  return KERNEL_INVALID_POINTER;
}

/* TODO: use backpointers */
/* wait_queue_lookup_task would be far faster if we used backpointers */
void
wait_queue_remove_task (struct wait_queue *wq, struct task *task)
{
  struct waiting_task_info *info;
  
  spin_lock (&wq->wq_lock);
  
  if (FAILED_PTR (info = wait_queue_lookup_task (wq, task)))
    FAIL ("process not in waitqueue, bug\n");
    
  list_remove_element ((void **) &wq->wq_queue, (void *) info);
  
  sfree (info);
  
  spin_unlock (&wq->wq_lock);
}


struct task *
wait_queue_pop_task (struct wait_queue *wq)
{
  struct waiting_task_info *task;

  spin_lock (&wq->wq_lock);
  
  if ((task = (struct waiting_task_info *) LIST_HEAD (wq->wq_queue)) != NULL)
    list_remove_element ((void **) &wq->wq_queue, task);

  spin_unlock (&wq->wq_lock);
  
  return task ? task->wt_task : NULL;
}

void
signal (struct wait_queue *wq, int reason)
{
  struct task *task;

  if ((task = wait_queue_pop_task (wq)) != NULL)
    wake_up (task, TASK_STATE_RUNNING, reason);
}

void
__signal_all (struct wait_queue *wq, int reason)
{
  struct task *task;
  int count = 0;
  
  while ((task = wait_queue_pop_task (wq)) != NULL)
  {
    wake_up (task, WAKEUP_AGAIN | WAKEUP_DELAYED | TASK_STATE_RUNNING, reason);
    ++count;
  }

  if (count)
    schedule ();
}

void
signal_all (struct wait_queue *wq, int reason)
{
  DECLARE_CRITICAL_SECTION (section);
  
  TASK_ATOMIC_ENTER (section);
  
  __signal_all (wq, reason);
  
  TASK_ATOMIC_LEAVE (section);
}

void
__sleep (struct wait_queue *wq)
{
  wait_queue_put_task (wq, get_current_task ());

  wake_up (get_current_task (), TASK_STATE_SOFT_WAIT, 0);

  schedule (); /* Won't resched until resume () */
}

int
sleep (struct wait_queue *wq)
{
  DECLARE_CRITICAL_SECTION (section);
  
  TASK_ATOMIC_ENTER (section);
  
  __sleep (wq);
  
  TASK_ATOMIC_LEAVE (section);

  return (get_current_task ())->ts_wakeup_reason;
}

void
wait_queue_remove_all (struct wait_queue *wq)
{
  struct list_head *this, *next;

  
  if ((this = LIST_HEAD (*((void **) &wq->wq_queue))) == NULL)
    return;
  
  while (this)
  {
    next = this->next;
    
    sfree (this);
      
    this = next;
  }
  
  wq->wq_queue = NULL;
}


DEBUG_FUNC (wait_queue_new);
DEBUG_FUNC (waiting_task_info_new);
DEBUG_FUNC (wait_queue_destroy);
DEBUG_FUNC (wait_queue_put_task);
DEBUG_FUNC (wait_queue_lookup_task);
DEBUG_FUNC (wait_queue_remove_task);
DEBUG_FUNC (wait_queue_pop_task);
DEBUG_FUNC (signal);
DEBUG_FUNC (signal_all);
DEBUG_FUNC (__signal_all);
DEBUG_FUNC (sleep);
DEBUG_FUNC (__sleep);
DEBUG_FUNC (wait_queue_remove_all);
