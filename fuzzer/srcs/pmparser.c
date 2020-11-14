#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <limits.h>
#include <errno.h>

#include "pmparser.h"

pmap_t create_pmap(pid_t pid)
{
  FILE *fh;
  char path[PATH_MAX];
  pmap_t map = { NULL, 0, 0 };

  sprintf(path, "/proc/%d/maps", pid);
  fh = fopen(path, "r");
  if (fh == NULL)
  {
    printf("Cannot open %s (%d)", path, errno);
    exit(1);
  }

  map.lines = malloc(LINES_LEN * sizeof(*map.lines));
  char *str = malloc(LINE_LEN * sizeof(*str));
  while (fgets(str, LINE_LEN, fh) != NULL)
  {
    line_t line = { 0, 0, 0, "", 0, 0, 0, "" };
    sscanf(str, "%llx-%llx %4s %llx %x:%x %u %128s[^\n]",
        &line.from, &line.to, line.flags, &line.offset, &line.major,
        &line.minor, &line.inode, (char*)&line.location);
    if (line.flags[0] != 'r' || line.flags[1] != 'w')
      continue;

    map.lines[map.nlines++] = line;
    map.length += line.to - line.from;
  }

  free(str);
  return map;
}

void free_pmap(pmap_t *pmap)
{
  free(pmap->lines);
}
