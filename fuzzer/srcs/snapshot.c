#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/user.h>
#include <fcntl.h>

#define _GNU_SOURCE
#include <sys/uio.h>

#include "snapshot.h"
#include "pmparser.h"
#include "ptrace.h"
#include "utils.h"

static void get_snapshot_memory(snapshot_t *snapshot, pmap_t *pmap)
{
  snapshot->memory = malloc(sizeof(*snapshot->memory) * pmap->nlines);
  snapshot->memory_nlines = pmap->nlines;
  snapshot->buffer_length = pmap->length;

  unsigned long long snapshot_buffer_offset = 0;
  LOG("[+] create snapshot: %llx bytes\n", snapshot->buffer_length);
  for (unsigned int i = 0 ; i < pmap->nlines ; ++i)
  {
    if (i > 0)
      snapshot_buffer_offset += pmap->lines[i - 1].to - pmap->lines[i - 1].from - 1;

    snapshot->memory[i].map_offset = pmap->lines[i].from;
    snapshot->memory[i].map_length = pmap->lines[i].to - pmap->lines[i].from;
    snapshot->memory[i].buf_offset = snapshot_buffer_offset;
    LOG("map offset: %llx\n", snapshot->memory[i].map_offset);
    LOG("map length: %llx\n", snapshot->memory[i].map_length);
    LOG("buf offset: %llx\n", snapshot->memory[i].buf_offset);
  }
}

static void get_snapshot_buffer(snapshot_t *snapshot, pid_t pid)
{
  char path[PATH_MAX] = { 0 };

  sprintf(path, "/proc/%d/mem", pid);
  snapshot->buffer = malloc(snapshot->buffer_length * sizeof(*snapshot->buffer));
  int fd = open(path, O_RDONLY);
  for (unsigned int i = 0 ; i < snapshot->memory_nlines ; ++i)
  {
    lseek(fd, snapshot->memory[i].map_offset, SEEK_SET);
    read(fd, (unsigned char *)(snapshot->buffer + snapshot->memory[i].buf_offset), snapshot->memory[i].map_length);
  }

  close(fd);
}

snapshot_t *create_snapshot(pid_t pid)
{
  pmap_t pmap = create_pmap(pid);
  snapshot_t *snapshot = malloc(sizeof(*snapshot));

  get_snapshot_memory(snapshot, &pmap);
  get_snapshot_buffer(snapshot, pid);
  snapshot->registers = get_regs(pid);

  free_pmap(&pmap);
  return snapshot;
}

ssize_t restore_snapshot(snapshot_t *snapshot, pid_t pid)
{
  ssize_t written = 0;
  unsigned long nlines = snapshot->memory_nlines;
  struct iovec *local = malloc(nlines * sizeof(*local));
  struct iovec *remote = malloc(nlines * sizeof(*remote));

  for (unsigned int i = 0 ; i < nlines ; ++i)
  {
    local[i].iov_base = (unsigned char *)(snapshot->buffer + snapshot->memory[i].buf_offset);
    local[i].iov_len = snapshot->memory[i].map_length;

    remote[i].iov_base = (void *)(snapshot->memory[i].map_offset);
    remote[i].iov_len = snapshot->memory[i].map_length;
  }

  written = process_vm_writev(pid, local, nlines, remote, nlines, 0);
  LOG("[+] restore snapshot: %lx bytes restored\n", written);

  set_regs(pid, snapshot->registers);

  free(local);
  free(remote);
  return written;
}

void free_snapshot(snapshot_t *snapshot)
{
  free(snapshot->memory);
  free(snapshot->buffer);
}
