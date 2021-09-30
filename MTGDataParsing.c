#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
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

// offset store data
typedef struct offset {
  uint32_t name_length;  // length of name
  char *name;  // store name
  off_t offset_position;  // Store position in file
} offset_t;

// Convert enum rarity for string. For printing
const char* get_rarity_str(enum rarity r);
// Convert from string to rarity type. For reading
int get_rarity_type(char *r);
// Get the total line in file
int count_line_in_file(char fileName[]);
// Compare function. Using in qsort. Compare by name with 2 cards
int compare_card_by_name(const void *a, const void *b);
// Compare funtion. Using in qsort. Compare by name with 2 offset_t
int compare_offset_by_name(const void *a, const void *b);


int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Use: %s <CSV file>\n", argv[0]);
    return 1;
  }
  if ((open(argv[1], O_RDONLY)) != -1) {
    // Try to get the line of cards file.
    int line_cnt = count_line_in_file(argv[1]);
    line_cnt--;
    card_t *data_cards = NULL;
    FILE *input_file = fopen(argv[1], "r");
    int cards_ind = 0;
    // Fields contain 8 field. Card structure
    char **fields = calloc(8, sizeof(char*));
    // Ignore the first line with header
    while (getc(input_file) != '\n' && getc(input_file) != EOF) {
    }
    // Try to parse data - read from the second line
    for (int line = 0; line <= line_cnt; line++) {
      char *line_str = malloc(1);
      line_str[0] = '\0';
      int line_length = 1;
      char c = ' ';
      // Flag store when we paring the ' and "
      bool have_quote = false, have_double_quote = false;

      // Read until meet endline. That's one line data
      while (c != '\n' && c != EOF) {
        c = getc(input_file);
        line_str = realloc(line_str, ++line_length);
        strncat(line_str, &c, 1);
      }

      char c2;
      int j = 0;

      // initialize the data. Just allocate 1 byte. Will be allocate more if need
      for (int k = 0; k < 8; k++) {
        fields[k] = malloc(1);
        fields[k][0] = '\0';
      }

      int field_num = 0;  // Contain current field num. 0 mean name, 1 mean cost, etc
      int length = 0;
      bool have_slash = false;
      bool ndouble = false;
      while ((c2 = line_str[j]) != '\0' && c2 != '\n') {
        if (!have_quote || c2 != '"' || length != 1) {
          if (have_double_quote == true && c2 == '"') {
            fields[field_num] = realloc(fields[field_num], ++length * sizeof(char));
            strncat(fields[field_num], &c2, 1);
            ndouble = true;
          }
          have_double_quote = false;
          if (c2 == '"') {
            have_slash = false;
            have_quote = !have_quote;
            if (!ndouble) {
              have_double_quote = true;
              ndouble = false;
            }
          } else {
            ndouble = false;
            if (have_quote == false && c2 == ',') {
              length = 1;
              field_num++;
            } else {
              if (c2 == '\\') {
                have_slash = true;
              } else if (have_slash && c2 == 'n') {
                char ok = '\n';
                length = length + 1;
                fields[field_num] = realloc(fields[field_num], length * sizeof(char*));
                strncat(fields[field_num], &ok, 1);
                have_slash = false;
              } else {
                fields[field_num] = realloc(fields[field_num], ++length * sizeof(char*));
                strncat(fields[field_num], &c2, 1);
                have_slash = false;
              }
            }
          }
        } else {
          have_quote = !have_quote;
        }
        ++j;
      }
      // We need check this is a new card or it already exist. No duplicate card
      bool need_add_new_card = true;
      for (int i = 0; i < cards_ind; i++) {
        if (strcmp(fields[1], data_cards[i].name) == 0) {
          if (atoi(fields[0]) > data_cards[i].id) {
            free(data_cards[i].name);
            free(data_cards[i].cost);
            free(data_cards[i].type);
            free(data_cards[i].text);
            free(data_cards[i].stats);
            data_cards[i].id = atoi(fields[0]);
            data_cards[i].name = calloc(strlen(fields[1]) + 1, 1);
            strcpy(data_cards[i].name, fields[1]);
            data_cards[i].cost = calloc(strlen(fields[2]) + 1, 1);
            strcpy(data_cards[i].cost, fields[2]);
            data_cards[i].converted_cost = atoi(fields[3]);
            data_cards[i].type = calloc(strlen(fields[4]) + 1, 1);
            strcpy(data_cards[i].type, fields[4]);
            data_cards[i].text = calloc(strlen(fields[5]) + 1, 1);
            strcpy(data_cards[i].text, fields[5]);
            data_cards[i].stats = calloc(strlen(fields[6]) + 1, 1);
            strcpy(data_cards[i].stats, fields[6]);
            data_cards[i].rarity = get_rarity_type(fields[7]);
          }
          need_add_new_card = false;
          break;
        }
        need_add_new_card = true;
      }

      // If this is new card data. Add this to array
      if (need_add_new_card == true) {
        data_cards = realloc(data_cards, (cards_ind + 1) * sizeof(card_t)); 
        data_cards[cards_ind].id = atoi(fields[0]);
        data_cards[cards_ind].name = calloc(strlen(fields[1]) + 1, 1);
        strcpy(data_cards[cards_ind].name, fields[1]);
        data_cards[cards_ind].cost = calloc(strlen(fields[2]) + 1, 1);
        strcpy(data_cards[cards_ind].cost, fields[2]);
        data_cards[cards_ind].converted_cost = atoi(fields[3]);
        data_cards[cards_ind].type = calloc(strlen(fields[4]) + 1, 1);
        strcpy(data_cards[cards_ind].type, fields[4]);
        data_cards[cards_ind].text = calloc(strlen(fields[5]) + 1, 1);
        strcpy(data_cards[cards_ind].text, fields[5]);
        data_cards[cards_ind].stats = calloc(strlen(fields[6]) + 1, 1);
        strcpy(data_cards[cards_ind].stats, fields[6]);
        data_cards[cards_ind].rarity = get_rarity_type(fields[7]);
        cards_ind++;
      }
      // Free resource avoid memory leak
      free(fields[0]);
      free(fields[1]);
      free(fields[2]);
      free(fields[3]);
      free(fields[4]);
      free(fields[5]);
      free(fields[6]);
      free(fields[7]);
      free(line_str);
    }
    // Close file
    fclose(input_file);

    // Write to cards.bin file
    FILE *cards_fd = fopen("correct-cards.bin", "wb");
    uint32_t len = 0;
    offset_t card_position[cards_ind - 1];

    // Sort data before write using qsort()
    qsort(data_cards, cards_ind - 1, sizeof(card_t), compare_card_by_name);

    for (int i = 0; i < cards_ind - 1; i++) {
      card_position[i].offset_position = ftell(cards_fd);
      card_position[i].name = calloc(strlen(data_cards[i].name) + 1, 1);
      strcpy(card_position[i].name, data_cards[i].name);
      fwrite(&data_cards[i].id, sizeof(data_cards[i].id), 1, cards_fd);
      len = (uint32_t) strlen(data_cards[i].cost);
      fwrite(&len, sizeof(uint32_t), 1, cards_fd);
      int j = 0;
      while (data_cards[i].cost[j] != '\0') {
        fwrite(&data_cards[i].cost[j], sizeof(char), 1, cards_fd);
        j++;
      }
      fwrite(&data_cards[i].converted_cost,
             sizeof(data_cards[i].converted_cost), 1, cards_fd);
      len = strlen(data_cards[i].type);
      fwrite(&len, sizeof(uint32_t), 1, cards_fd);
      j = 0;
      while (data_cards[i].type[j] != '\0') {
        fwrite(&data_cards[i].type[j], sizeof(char), 1, cards_fd);
        j++;
      }
      len = strlen(data_cards[i].text);
      fwrite(&len, sizeof(uint32_t), 1, cards_fd);
      j = 0;
      while (data_cards[i].text[j] != '\0') {
        fwrite(&data_cards[i].text[j], sizeof(char), 1, cards_fd);
        j++;
      }
      len = strlen(data_cards[i].stats);
      fwrite(&len, sizeof(uint32_t), 1, cards_fd);
      j = 0;
      while (data_cards[i].stats[j] != '\0') {
        fwrite(&data_cards[i].stats[j], sizeof(char), 1, cards_fd);
        j++;
      }
      fwrite(&data_cards[i].rarity, sizeof(enum rarity), 1, cards_fd);
    }
    fclose(cards_fd);

    // Sort index before write using qsort()
    qsort(card_position, cards_ind - 1, sizeof(offset_t), compare_offset_by_name);

    // Write to index file
    FILE *index_file = fopen("correct-index.bin", "wb");
    for (int i = 0; i < cards_ind - 1; i++) {
      card_position[i].name_length = strlen(data_cards[i].name);
      fwrite(&card_position[i].name_length,
             sizeof(card_position[i].name_length), 1, index_file);
      int j = 0;
      while (data_cards[i].name[j] != '\0') {
        fwrite(&data_cards[i].name[j], sizeof(char), 1, index_file);
        j++;
      }
      if ((i) < cards_ind) {
        fwrite(&card_position[i].offset_position, sizeof(off_t), 1, index_file);
      } else {
        off_t a = 0;
        fwrite(&a, sizeof(a), 1, index_file);
      }
    }
    fclose(index_file);

    // For testing 
    // Printout the data
    /*for (int i = 0; i < cards_ind - 1; i++) {
      printf("%-30s%12s\n%-34s%8s\n%s\n%s\n%s\n%42s\n\n", data_cards[i].name,
             data_cards[i].cost, data_cards[i].type,
             get_rarity_str(data_cards[i].rarity),
             "-------------------------------------------------",
             data_cards[i].text,
             "-------------------------------------------------",
             data_cards[i].stats);
    }*/
    for (int i = 0; i < cards_ind; i++) {
      free(data_cards[i].name);
      data_cards[i].name = NULL;
      free(data_cards[i].cost);
      free(data_cards[i].type);
      free(data_cards[i].text);
      free(data_cards[i].stats);
      if (i < cards_ind - 1)
        free(card_position[i].name);
    }
    free(data_cards);
    free(fields);
    } 
      else {
   	fprintf(stderr, "./parser: cannot open(\"%s\"): No such file or directory\n", argv[1]);
 	   }
  return 0;
}

// Convert enum rarity for string. For printing
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

// Convert from string to rarity type. For reading
int get_rarity_type(char *r) {

  if ((strcmp(r, "common")) == 0) {
    return common;
  } else if ((strcmp(r, "uncommon")) == 0) {
    return uncommon;
  } else if (strcmp(r, "rare") == 0) {
    return rare;
  } else if ((strcmp(r, "mythic")) == 0) {
    return mythic;
  } else {
    return 4;
  }
}

// Get the total line in file
int count_line_in_file(char fileName[]) {
  FILE *filePtr = fopen(fileName, "r");
  int lines = 0;
  int character;
  while (!feof(filePtr)) {
    character = getc(filePtr);
    if (character == '\n') {
      ++lines;
    }
  }
  fclose(filePtr);
  return lines;
}

// Compare function. Using in qsort. Compare by name with 2 cards
int compare_card_by_name(const void *a, const void *b) {
  return (strcmp((*(card_t*) a).name, (*(card_t*) b).name));
}

// Compare funtion. Using in qsort. Compare by name with 2 offset_t
int compare_offset_by_name(const void *a, const void *b) {
  return (strcmp((*(offset_t*) a).name, (*(offset_t*) b).name));
}
