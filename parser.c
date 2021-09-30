#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

const char* rarity_names[4] = {"common", "uncommon", "rare", "mythic"};

/* structure for all cards */
struct card_array
{
    struct card* items;  /* cards */
    int size;       /* actual number of cards at the array */
    int capacity;   /* allocated space at the array */
};


/* Helper function declarations */

/* Creates deep copy of the card */
void copy_card(struct card* from, struct card* to);

/* Creates copy of the given string and returns pointer to the copy */
char* copy_str(const char* str);

/* Frees all string fields of the given card */
void clear_card(struct card* pcard);

/* Prints the given card into the screen */
void print_card(struct card* pcard);

/* Initializes array for storing 10 cards */
void initialize_card_array(struct card_array *cards);

/* Prints card array to the stdout */
void print_card_array(struct card_array *cards);

/* Add new card into array
   If array is full, increases capacity by 10. */
void add_card_to_array(struct card_array* cards, struct card* card);

/* Looking for the card by name. If card with given name found, returns its index,
   or returns -1 if card with given name is not found */
int find_card_by_name(struct card_array *cards, const char* name);

/* Deletes all cards from the array, frees all allocated memory */
void clear_card_array(struct card_array *cards);


/* Reads information about one card from the given file
   and saves this information into structure card
   Returns 1 if parsing succeed, 0 otherwise */
int parse_card(FILE* fp, struct card* pcard);

/* Reads one string field from the source file pointed by fp
   and saves string into buffer pointed by str.
   Eliminates start and end quotes.
   Returns 1 if reading succeed, 0 otherwise */
int parse_string_field(FILE* fp, char* str);

/* Allocates memory for all strings at the card to given maximum size */
void initialize_card(struct card* pcard, int str_size);

/* Function for comparing cards by name field */
int compare_cards (const struct card* card1, const struct card* card2);

int main(int argc, char* argv[])
{
    FILE* fp;
    struct card_array cards;
    struct card card_buffer;
    int card_index;

    /* validate that file-name provided */
    if (argc < 2) {
        fprintf(stderr, "Usage: parser <file name>\n");
        return 1;
    }

    /* try to open source file */
    fp = fopen(argv[1], "r");
    if (!fp) {
        fprintf(stderr, "./parser: cannot open(\"%s\"): No such file or directory\n", argv[1]);
        return 1;
    }

    /* initialize card array */
    initialize_card_array(&cards);

    /* initialize card buffer */
    initialize_card(&card_buffer, 1024);

    /* skip the first line from the source file */
    /* while (!feof(fp) && fgetc(fp) != '\n') {} */
    fscanf(fp, "%*[^\n]\n");

    /* read all cards and copy information into array */
    while (parse_card(fp, &card_buffer)) {

        /* search for card with given name at the array */
        card_index = find_card_by_name(&cards, card_buffer.name);
        if (card_index == -1) {
            /* name not found - add new card */
            add_card_to_array(&cards, &card_buffer);
        }
        else if (card_buffer.id > cards.items[card_index].id) {
            /* if name found and new card has the bigger id - rewrite the old card */
            clear_card(&(cards.items[card_index])); /* clear old card */
            copy_card(&card_buffer, &(cards.items[card_index]));
        }
    }

    /* close source file */
    fclose(fp);

    /* Sort card array by qsort */
    qsort(cards.items, cards.size, sizeof (struct card), (int(*) (const void *, const void *)) compare_cards);

    /* Print cards to the screen */
    print_card_array(&cards);

    /* Clear all allocated memory */
    clear_card(&card_buffer);
    clear_card_array(&cards);

    return 0;
}


void initialize_card_array(struct card_array *cards)
{
    if (cards == NULL) {
        return ;
    }

    cards->items = (struct card*) malloc (sizeof(struct card) * 10);
    cards->size = 0;
    cards->capacity = 10;
}

void copy_card(struct card* from, struct card* to)
{
    if (from == NULL || to == NULL) {
        return;
    }

    to->id = from->id;
    to->name = copy_str(from->name);
    to->cost = copy_str(from->cost);
    to->converted_cost = from->converted_cost;
    to->type = copy_str(from->type);
    to->text = copy_str(from->text);
    to->stats = copy_str(from->stats);
    to->rarity = from->rarity;
}

/* Creates copy of the given string and returns pointer to the copy */
char* copy_str(const char* str)
{
    int size;
    char* new_str;

    if (str == NULL) {
        return NULL;
    }

    /* allocate memory for copy string */
    size = strlen(str);

    /* copy all characters including the last \0 */
    new_str = (char*) malloc (sizeof(char) * (size + 1));
    memcpy(new_str, str, size + 1);

    /* return pointer to the string copy */
    return new_str;
}

/* Frees all string fields of the given card */
void clear_card(struct card* pcard)
{
    if (pcard == NULL) {
        return;
    }
    free(pcard->name);
    free(pcard->cost);
    free(pcard->type);
    free(pcard->text);
    free(pcard->stats);
}


/* Add new card into array
   If array is full, increases capacity by 10. */
void add_card_to_array(struct card_array* cards, struct card* pcard)
{
    if (cards == NULL || pcard == NULL) {
        return;
    }

    struct card* old_items;

    /* If array is full */
    if (cards->size == cards->capacity) {
        /* increase array capacity by 10 */
        old_items = cards->items;
        cards->capacity += 10;
        cards->items = (struct card*) malloc (sizeof(struct card) * cards->capacity);
        memcpy(cards->items, old_items, sizeof(struct card) * cards->size);
        free(old_items);
    }

    /* Copy new card at the end of the array */
    copy_card(pcard, &(cards->items[cards->size]));
    cards->size++;
}

/* Allocates memory for all strings at the card to given maximum size */
void initialize_card(struct card* pcard, int str_size)
{
    if (pcard == NULL) {
        return ;
    }

    pcard->id = 0;
    pcard->rarity = common;
    pcard->converted_cost = 0;
    pcard->name = (char*) malloc (str_size);
    pcard->cost = (char*) malloc (str_size);
    pcard->type = (char*) malloc (str_size);
    pcard->text = (char*) malloc (str_size);
    pcard->stats = (char*) malloc (str_size);
}

/* Reads information about one card from the given file
   and saves this information into structure card
   Returns 1 if parsing succeed, 0 otherwise */
int parse_card(FILE* fp, struct card* pcard)
{
    int i;
    char rarity_str[20];

    if (fp == NULL || feof(fp) || pcard == NULL) {
        return 0;
    }

    /* try to read id */
    if (fscanf(fp, "%ud", &(pcard->id)) != 1) {
        return 0; /* can't read integer id */
    }

    /* skip one ',' */
    if (fgetc(fp) != ',') {
        return 0;
    }

    /* parse name field */
    if (!parse_string_field(fp, pcard->name)) {
        return 0;
    }

    /* parse cost field */
    if (!parse_string_field(fp, pcard->cost)) {
        return 0;
    }

    /* try to read converted_cost */
    if (fscanf(fp, "%ud", &(pcard->converted_cost)) != 1) {
        return 0; /* can't read integer converted_cost */
    }

    /* skip one ',' */
    if (fgetc(fp) != ',') {
        return 0;
    }

    /* parse type field */
    if (!parse_string_field(fp, pcard->type)) {
        return 0;
    }

    /* parse text field */
    if (!parse_string_field(fp, pcard->text)) {
        return 0;
    }

    /* parse stats field */
    if (!parse_string_field(fp, pcard->stats)) {
        return 0;
    }

    /* parse rarity field */
    if (!parse_string_field(fp, rarity_str)) {
        return 0;
    }

    for (i = 0; i < 4; i++) {
        if (strcmp(rarity_names[i], rarity_str) == 0) {
            pcard->rarity = i;
            return 1;
        }
    }

    return 0; /* unknown rarity */
}

/* Reads one string field from the source file pointed by fp
   and saves string into buffer pointed by str.
   Eliminates start and end quotes.
   Returns 1 if parsing succeed, 0 otherwise */
int parse_string_field(FILE* fp, char* str)
{
    int c;
    int str_size = 0;
    int end_quote_found = 0;

    if (fp == NULL || str == NULL) {
        return 0;
    }

    str[0] = '\0';

    c = fgetc(fp);

    if (c == ',') {
        return 1; /* empty field */
    }

    /* skip leading '"' */
    if (c != '"') {
        return 0;
    }

    /* Read all characters until ending '"'  found */
    end_quote_found = 0;
    while (!end_quote_found && (c = fgetc(fp))) {
        /* analyze "\n" sequence */
        if (c == '\\') {
            c = fgetc(fp);
            if (c == 'n') {
                /* \n sequence - copy 1 character '\n' */
                c = '\n';
                str[str_size] = '\n';
                str_size++;
                str[str_size] = '\0';
            }
            else {
                /* not \n sequence - copy 2 characters */
                str[str_size] = '\\';
                str[str_size + 1] = c;
                str_size += 2;
                str[str_size] = '\0';
            }

        }
        else if (c == '"')
        {
            /* analyze "" sequence */
            c = fgetc(fp);
            if (c == '"') {
                /* "" sequence - copy only one character " */
                str[str_size] = '"';
                str_size++;
                str[str_size] = '\0';
            }
            else {
                /* ending " found - do not copy it into output string */
                end_quote_found = 1;
            }
        }
        else
        {
            /* copy character into str */
            str[str_size] = c;
            str_size++;
            str[str_size] = '\0';
        }
    }

    return 1;
}

/* Prints the given card into the screen */
void print_card(struct card* pcard)
{
    if (pcard == NULL) {
        return;
    }

    printf("%-32s%20s\n", pcard->name, pcard->cost);
    printf("%-44s%8s\n", pcard->type, rarity_names[pcard->rarity]);
    printf("----------------------------------------------------\n");
    printf("%s\n", pcard->text);
    printf("----------------------------------------------------\n");
    printf("%52s\n", pcard->stats);
}


/* Prints card array to the stdout */
void print_card_array(struct card_array* cards)
{
    int i;

    if (cards == NULL) {
        return ;
    }

    for (i = 0; i < cards->size; i++) {
        print_card(&(cards->items[i]));
        printf("\n");
    }
}

/* Looking for the card by name. If card with given name found, returns its index,
   or returns -1 if card with given name is not found */
int find_card_by_name(struct card_array *cards, const char* name)
{
    int i;

    if (cards == NULL || name == NULL) {
        return -1;
    }

    for (i = 0; i < cards->size; i++) {
        if (strcmp(cards->items[i].name, name) == 0) {
            return i;
        }
    }

    return -1; /* given name not found */
}

/* Deletes all cards from the array, frees all allocated memory */
void clear_card_array(struct card_array* cards)
{
    int i;

    if (cards == NULL) {
        return ;
    }

    for (i = 0; i < cards->size; i++) {
        clear_card(&(cards->items[i]));
    }

    free(cards->items);
    cards->items = NULL;
    cards->capacity = 0;
    cards->size = 0;
}

/* Function for comparing cards by name field */
int compare_cards (const struct card* card1, const struct card* card2)
{
    return strcmp(card1->name, card2->name);
}
