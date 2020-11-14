#pragma once

typedef struct {
  unsigned long long map_offset;
  unsigned long long map_length;
  unsigned long long buf_offset;
} snapshot_memory_t;

typedef struct {
  snapshot_memory_t *memory;
  unsigned long memory_nlines;
  struct user_regs_struct registers;
  unsigned char *buffer;
  unsigned long long buffer_length;
} snapshot_t;

snapshot_t *create_snapshot(pid_t pid);
void free_snapshot(snapshot_t *snapshot);
ssize_t restore_snapshot(snapshot_t *snapshot, pid_t pid);
