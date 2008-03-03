#include <stdlib.h>
#include <string.h>
#include "alltom.h"

typedef struct Pair {
	char *key, *value;
	struct Pair *next;
} Pair;

struct Dict {
	Pair *head;
};

static char *parse_pair(char *qs, Dict *d);
static char *eat_key(char *str);
static char *eat_value(char *str);
static char *copy_str(char *start, char *end);
static void add_pair(Dict *d, char *key, char *value);
static void make_upper(char *str);
static char *strcp(char *str);

/* builds a dictionary from a query string */
Dict *
get_query_dict(char *qs) {
	Dict *d;

	d = (Dict *) malloc(sizeof(Dict));
	if(d == NULL)
		return NULL;
	d->head = NULL;

	if(qs == NULL) return d;

	qs = parse_pair(qs, d);
	while(qs != NULL && *qs != '\0')
		qs = parse_pair(qs + 1, d);

	return d;
}

char *
get_value(Dict *dictionary, char *key) {
	Pair *p = dictionary->head;
	char *mykey = strcp(key);
	if(mykey == NULL) return NULL;

	make_upper(mykey);

	for(; p != NULL; p = p->next) {
		if(strcmp(p->key, mykey) == 0)
			return p->value;
	}
	return NULL;
}

/* pulls off the first key/value pair from a query string */
/* adds the pair to the given dictionary */
/* returns character just after last character of value */
static char *
parse_pair(char *qs, Dict *d) {
	char *key, *next, *var_start;
	key = next = var_start = NULL;

	if(qs == NULL || *qs == '\0') return NULL;
	if(*qs == '&') return qs;
	if(*qs == '=') return eat_value(qs + 1);

	next = eat_key(qs);
	key = copy_str(qs, next);
	make_upper(key);

	var_start = next + 1; /* skip the = */
	if(*next == '\0' || *next == '&' || /* no variable */
	   *var_start == '\0' || *var_start == '&' /* empty variable */
	   ) {
		add_pair(d, key, "");
		return next;
	}

	next = eat_value(var_start);
	add_pair(d, key, copy_str(var_start, next));
	return next;
}

static void
add_pair(Dict *d, char *key, char *value) {
	Pair *p = (Pair *) malloc(sizeof(Pair));
	if(p == NULL) return; /* what can you do? */

	p->key = key;
	p->value = value;
	p->next = d->head;
	d->head = p;
}

static char *
eat_key(char *str) {
	char *end = str;
	while(*end != '=' && *end != '&' && *end != '\0')
		end++;
	return end;
}

static char *
eat_value(char *str) {
	char *end = str;
	while(*end != '&' && *end != '\0')
		end++;
	return end;
}

static char *
copy_str(char *start, char *end) {
	char *new = (char *) malloc(end - start + 1);
	char *cur = new;
	if(new == NULL) return NULL;
	while(start != end)
		*cur++ = *start++;
	*cur = '\0';
	return new;
}

static void
make_upper(char *str) {
	/* oh god such a hack */
	do {
		if(*str >= 'a' && *str <= 'z')
			*str += 'A' - 'a';
		str++;
	} while(*str != '\0');
}

static char *
strcp(char *str) {
	char *new = (char *) malloc(strlen(str));
	char *front = new;
	if(new == NULL) return NULL;
	while((*new++ = *str++) != '\0');
	return front;
}
