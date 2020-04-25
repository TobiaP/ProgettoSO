#include "util.h"

void nome_pipe(int pid, char* path_str)
{
  sprintf(path_str, "%s%d", path_pipe, pid);
}
