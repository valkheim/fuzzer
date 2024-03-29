#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <err.h>

#define PATH_LEN (128)
#define LINE_LEN (512)
#define LINES_LEN (0x2A)

typedef struct {
  unsigned long long from;
  unsigned long long to;
  unsigned long long offset;
  char flags[5];
  int minor;
  int major;
  int inode;
  char location[PATH_LEN];
} line_t;

unsigned int g_line_idx = 0;
line_t g_lines[LINES_LEN];

static int usage(void)
{
  puts("Usage: proc_maps_parser <pid>");
  return 2;
}

#if 1
static void print_line(line_t *line)
{
  printf("%llx-%llx %4s %llx %x:%x %d %s\n",
      line->from, line->to, line->flags, line->offset, line->major,
      line->minor, line->inode, line->location);
}
#else
static void print_line(line_t *line)
{
    printf("offset: %llx\tlength: %llx\n",
      line->from,
      line->to - line->from
    );
}
#endif


static void read_line(char *input)
{
  line_t line = { 0, 0, 0, "", 0, 0, 0, "" };
  sscanf(input, "%llx-%llx %4s %llx %x:%x %d %128s[^\n]",
      &line.from, &line.to, line.flags, &line.offset,
      (unsigned int *)&line.major, (unsigned int *)&line.minor, &line.inode,
      (char*)&line.location);

  g_lines[g_line_idx++] = line;
}

static void parse(pid_t pid)
{
  FILE *fh;
  char path[PATH_LEN];
  sprintf(path, "/proc/%d/maps", pid);
  fh = fopen(path, "r");
  if (fh == NULL)
  {
    errx(1, "Cannot open %s (%d)\n", path, errno);
  }

  char *line = malloc(LINE_LEN * sizeof(*line));
  while (fgets(line, LINE_LEN, fh) != NULL)
  {
    read_line(line);
  }

  for (unsigned int i = 0 ; i < g_line_idx ; ++i)
    print_line(&g_lines[i]);

  free(line);
}

int main(int ac, char **av)
{
  if (ac != 2)
    return usage();

  parse(atoi(av[1]));
}
