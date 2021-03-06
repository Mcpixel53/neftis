/*
 *    ELF32 Linux ABI VDSO for Atomik
 *    Copyright (C) 2014  Gonzalo J. Carracedo
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

#include <atomik.h>
#include <stdlib.h>
#include <ipc/msg.h>
#include <errno.h>

int
msgreq (int size)
{
  ipc (SYS_IPC_msg_request, size, 0, 0, 0, 0);
}

int
msgmap (int id)
{
  return ipc (SYS_IPC_msg_map, id, 0, 0, 0, 0);
}

int
msgunmap (int id)
{
  return ipc (SYS_IPC_msg_unmap, id, 0, 0, 0, 0);
}

int
msgsend (int id)
{
  return ipc (SYS_IPC_msg_send, id, 0, 0, 0, 0);
}

int
msgrecv (int id)
{
  return ipc (SYS_IPC_msg_recv, id, 0, 0, 0, 0);
}

int
msgmicro_read (int id, void *data, unsigned int datasize)
{
  return ipc (SYS_IPC_msg_read_micro, id, (int) data, (int) datasize, 0, 0);
}

int
msgmicro_write (int id, const void *data, unsigned int datasize)
{
  return ipc (SYS_IPC_msg_write_micro, id, (int) data, (int) datasize, 0, 0);
}

int
msgread_by_type (unsigned int type,
                 unsigned int link,
                 void *data,
                 unsigned int size,
                 int nonblock)
{
  return ipc (SYS_IPC_msg_read_by_type, type, link, (int) data, size, nonblock);
}

int
msgread (void *data,
         unsigned int size,
         int nonblock)
{
  return ipc (SYS_IPC_msg_read, (int) data, size, nonblock, 0, 0);
}

int
msgwrite (int tid, const void *data, unsigned int size)
{
  return ipc (SYS_IPC_msg_write, tid, (int) data, size, 0, 0);
}

int
msggetinfo (int id, void *data)
{
  return ipc (SYS_IPC_msg_get_info, id, (int) data, 0, 0, 0);
}

int
msgrelease (int id)
{
  return ipc (SYS_IPC_msg_release, id, 0, 0, 0, 0);
}

int
fragmsg_read (void **data, unsigned int *size, int nonblock)
{
  struct frag_msg_header *msg = NULL, *new = NULL;
  int ret;
  unsigned int p;
  
  if ((msg = malloc (MSG_MICRO_SIZE)) == NULL)
    return -ENOMEM;

  if ((ret = msgread (msg, MSG_MICRO_SIZE, nonblock)) < 0)
    goto fail;

  /* Message is not fragmented */
  if (msg->mh_msg_size <= MSG_FRAG_PAYLOAD_SIZE)
  {
    if (msg->mh_msg_size + sizeof (struct frag_msg_header) != ret)
    {
      ret = -ENOMSG;
      goto fail;
    }
    
    *data = msg;
    *size = ret;

    return 0;
  }

  if ((new = realloc (msg, sizeof (struct frag_msg_header) + msg->mh_msg_size)) == NULL)
    goto fail;

  msg = new;

  /* Buffer to hold each incoming fragment */
  if ((new = malloc (MSG_MICRO_SIZE)) == NULL)
    goto fail;

  p = ret - sizeof (struct frag_msg_header);
  
  /* Retrieve all remaining messages */
  while (p < msg->mh_msg_size)
  {
    /* These calls should be blocking if we want a complete message */
    if ((ret = msgread_by_type (msg->mh_type, msg->mh_link, new, MSG_MICRO_SIZE, 0)) < 0)
      goto fail;

    /* Hackers? */
    if (ret - sizeof (struct frag_msg_header) + p > msg->mh_msg_size)
    {
      ret = -E2BIG;
      goto fail;
    }

    memcpy (msg->mh_data + p, new->mh_data, ret - sizeof (struct frag_msg_header));

    p += ret - sizeof (struct frag_msg_header);
  }

  *data = msg;
  *size = msg->mh_msg_size;
  
  return 0;
  
fail:
  if (msg != NULL)
    free (msg);

  if (new != NULL)
    free (new);
  
  return ret;
}

int
fragmsg_write (int tid, const void *data, unsigned int size)
{
  struct frag_msg_header *msg;
  const struct frag_msg_header *bigmsg = data;
  
  int p;
  int ret;
  int sendsize;
  
  if ((msg = malloc (MSG_MICRO_SIZE)) == NULL)
    return -ENOMEM;

  p = 0;

  memcpy (msg, data, sizeof (struct frag_msg_header));

  msg->mh_msg_size = size - sizeof (struct frag_msg_header);
  
  while (p < size - sizeof (struct frag_msg_header))
  {
    sendsize = size - sizeof (struct frag_msg_header) - p;

    if (sendsize > MSG_FRAG_PAYLOAD_SIZE)
      sendsize = MSG_FRAG_PAYLOAD_SIZE;
    
    memcpy (msg->mh_data, bigmsg->mh_data + p, sendsize);

    if ((ret = msgwrite (tid, msg, sendsize)) < 0)
      goto done;

    p += sendsize;
  }

  ret = 0;
  
done:
  free (msg);

  return ret;
}

                
