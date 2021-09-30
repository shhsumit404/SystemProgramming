#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "card.h"

/*
enum rarity
{
    common,
    uncommon,
    rare,
    mythic
};

struct card
{
    unsigned int id;
    char* name;
    char* cost;
    unsigned int converted_cost;
    char* type;
    char* text;
    char* stats;
    enum rarity rarity;
};
*/

typedef struct offset {
  uint32_t name_length;
  char *name;
  off_t offset_position;
} offset_t;

// Convert rarity to string. Using for printing
const char* get_rarity_str(enum rarity r);
// Compare function using in bsearch(). Compare key name with offset name
int cmp_offset_name(const void *a, const void *b);

int main(int argc, char **argv) {
  if (argc != 1) {
    printf("Use: %s\n", argv[0]);
    return 1;
  }
  // Read data from index bin
  FILE *index_fd = fopen("correct-index.bin", "rb");
  uint32_t buff_size;
  off_t posision_offset;
  offset_t *card_pos = malloc(0);  // data location with name
  int i = 1, j = 1;
  int card_pos_length;
  char *user_input, c;
  do {
    card_pos = realloc(card_pos, sizeof(offset_t) * i);

    card_pos[i - 1].name = malloc(1);
    card_pos[i - 1].name[0] = '\0';
    // Read length of name
    if (fread(&buff_size, sizeof(buff_size), 1, index_fd) != 1) {
      break;
    }
    card_pos[i - 1].name_length = buff_size;
    char c = ' ';
    int j = 1;
    // Read name base on name length
    for (int k = 0; k < buff_size; k++) {
      fread(&c, sizeof(c), 1, index_fd);
      card_pos[i - 1].name = realloc(card_pos[i - 1].name, ++j);
      strncat(card_pos[i - 1].name, &c, sizeof(char));
    }
    // Read the location in binary file with off_t type
    if (fread(&posision_offset, sizeof(posision_offset), 1, index_fd) != 1) {
      break;
    }
    card_pos[i - 1].offset_position = posision_offset;
    i++;
  } while (index_fd);
  // Close the file
  fclose(index_fd);

  card_pos_length = i - 1;
  j = 1;
  // Loop until quit command
  while (true) {
    user_input = malloc(1);
    user_input[0] = '\0';
    printf(">> ");
    while ((c = fgetc(stdin)) != '\n') {
      if (!isatty(fileno(stdin))) {
        printf("%c", c);
      }
      user_input = realloc(user_input, ++j);
      strncat(user_input, &c, sizeof(char));
    }
    if (strcmp(user_input, "q") == 0 || strcmp(user_input, "Q") == 0) {
      // Quit
      free(user_input);
      break;
    }
    // using bsearch to search by name in the card location array
    offset_t *offset_found = bsearch(user_input, card_pos, card_pos_length, sizeof(offset_t), cmp_offset_name);

    if (offset_found == NULL) {
      // not found
      printf("./search: \'%s\' not found!\n", user_input);
    } else {
      // We found. Try to read data from cards.bin base on offset location.
      FILE *card_bin_fd = fopen("correct-cards.bin", "rb");
      // Move to location
      fseek(card_bin_fd, offset_found->offset_position, SEEK_SET);
      card_t card_info;
      // Read data
      card_info.name = malloc(strlen(offset_found->name) + 1);
      strcpy(card_info.name, offset_found->name);
      uint32_t tmp = 0;
      fread(&tmp, sizeof(uint32_t), 1, card_bin_fd);
      card_info.id = tmp;
      fread(&tmp, sizeof(uint32_t), 1, card_bin_fd);
      card_info.cost = malloc(1);
      card_info.cost[0] = '\0';
      char character;
      for (int i = 0; i < tmp; i++) {
        fread(&character, 1, 1, card_bin_fd);
        card_info.cost = realloc(card_info.cost, i + 2);
        strncat(card_info.cost, &character, sizeof(char));
      }
      fread(&card_info.converted_cost, sizeof(uint32_t), 1, card_bin_fd);
      fread(&tmp, sizeof(uint32_t), 1, card_bin_fd);
      card_info.type = malloc(1);
      card_info.type[0] = '\0';
      for (int i = 0; i < tmp; i++) {
        fread(&character, 1, 1, card_bin_fd);
        card_info.type = realloc(card_info.type, i + 2);
        strncat(card_info.type, &character, sizeof(char));
      }
      fread(&tmp, sizeof(uint32_t), 1, card_bin_fd);
      card_info.text = malloc(1);
      card_info.text[0] = '\0';
      for (int i = 0; i < tmp; i++) {
        fread(&character, 1, 1, card_bin_fd);
        card_info.text = realloc(card_info.text, i + 2);
        strncat(card_info.text, &character, sizeof(char));
      }
      fread(&tmp, sizeof(uint32_t), 1, card_bin_fd);
      card_info.stats = malloc(1);
      card_info.stats[0] = '\0';
      for (int i = 0; i < tmp; i++) {
        fread(&character, 1, 1, card_bin_fd);
        card_info.stats = realloc(card_info.stats, i + 2);
        strncat(card_info.stats, &character, sizeof(char));
      }

      fread(&card_info.rarity, sizeof(enum rarity), 1, card_bin_fd);
      fclose(card_bin_fd);
      // Printout
      printf("%-30s%12s\n%-34s%8s\n%s\n%s\n%s\n%42s\n\n", card_info.name,
             card_info.cost, card_info.type, get_rarity_str(card_info.rarity),
             "------------------------------------------",
             card_info.text,
             "------------------------------------------",
             card_info.stats);

      free(card_info.name);
      free(card_info.stats);
      free(card_info.text);
      free(card_info.type);
      free(card_info.cost);
    }
    free(user_input);
  }
  for (int i = 0; i < card_pos_length; i++) {
    free(card_pos[i].name);
  }
  free(card_pos);
}

const char* get_rarity_str(enum rarity r) {
  switch (r) {
    case common:
      return "common";
    case uncommon:
      return "uncommon";
    case rare:
      return "rare";
    case mythic:
      return "mythic";
    default:
      return "wrong";
  }
}

int cmp_offset_name(const void *a, const void *b) {
  return (strcmp((char*) a, (*(offset_t*) b).name));
}

