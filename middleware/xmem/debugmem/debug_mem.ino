#include <Arduino.h>
#include <stdlib.h>
#include "memdebug.h"

void showmem()
{
  char buffer[100];
 
  sprintf(buffer,"%04u %04u %04u : used - free - large",
      getMemoryUsed(),
      getFreeMemory(),
      getLargestAvailableMemoryBlock()
    );
 
  Serial.println(buffer);
}

void setup() {
  Serial.begin(115200);

  void *a = malloc(32);
  Serial.print("malloc(32) -> a = 0x"); Serial.println((uintptr_t)a, HEX); // Phải có để tránh compiler tối ưu con trỏ *a.
  
  void *b = malloc(20); Serial.print("malloc(20) -> b = 0x"); Serial.println((uintptr_t)b, HEX);
  void *c = malloc(50); Serial.print("malloc(50) -> c = 0x"); Serial.println((uintptr_t)c, HEX);
  showmem();

  free(b);
  Serial.print("Size block in freelist: "); Serial.println(getFreeListSize());
  Serial.print("Number of block in freelist: "); Serial.println(getNumberOfBlocksInFreeList());

}

void loop() {}
