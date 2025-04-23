#include <stdbool.h>
#include <stdio.h>

bool is_printable_buf(const void *buf, int len) {
  char *buffer = (char *)buf;
  for(int i=0; i<len; i++) {
    if((*buffer < 32) || (*buffer > 126)) {
      return false;
    }
    buffer++;
  }
  return true;
}

int main() {
  char buffer[256];
  char *curr = buffer;
  int no_elements = 0;
  while(no_elements<255) {
    scanf("%c", curr);
    if(*curr == '0') {
      *curr = 0;
      break;
    }
    no_elements++;
    curr++;
  }
  printf("%s\n", buffer);
  printf(is_printable_buf(buffer, no_elements) ? "true\n" : "false\n");
  return 0;
}
  
