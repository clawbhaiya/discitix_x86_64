#include <fs/initrd.h>
#include <init/stivale2.h>
#include <libk/stdio.h>
#include <libk/string.h>
#include <mm/pmm.h>
#include <stdint.h>

uint64_t initrd_location;
initrd_header_t *header;

void init_initrd(struct stivale2_struct *bootinfo) {
  struct stivale2_struct_tag_modules *modules =
      (struct stivale2_struct_tag_modules *)stivale2_get_tag(
          bootinfo, STIVALE2_STRUCT_TAG_MODULES_ID);
  if (modules->module_count == 0) {
    printf("ERROR! No modules loaded! Cannot find initrd...\n");
    return;
  }
  initrd_location = modules->modules[0].begin;
  header = (initrd_header_t *)initrd_location;
}

int read_initrd() {
  if (header == NULL) {
    printf("No valid Initial RAM Disk was found!\n");
    return -1;
  }
  printf("Initrd location: %ul\nNumber of files: %d\n", initrd_location,
         header->num);

  initrd_file_t *file =
      (initrd_file_t *)(initrd_location + sizeof(initrd_header_t));

  for (int i = 0; i < header->num; i++) {
    if (!file) {
      printf("Invalid file pointer at index %d\n", i);
      return -1;
    }
    uint64_t offset = file->offset + initrd_location;

    printf("Filename: %s\nOffset: %ui\nLength: %ui\n\n", file->name, offset,
           file->length);

    file = (initrd_file_t *)(((uint64_t)(void *)file) + sizeof(initrd_file_t));
  }

  return 0;
}
