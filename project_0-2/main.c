#include "bitmap.h"
#include "hash.h"
#include "list.h"
#include "debug.h"
#include "hex_dump.h"
#include "limits.h"
#include "round.h"
#include <stdio.h>
#include <string.h>
char command[100];
struct list *lists[10];
char listname[10][100];
struct hash *hashes[10];
char hashname[10][100];
struct bitmap *bitmaps[10];
char bitmapname[10][100];
char line[3][100];
int listnum, hashnum, bitmapnum;

// list_less_func
bool list_less(const struct list_elem *a, const struct list_elem *b, void *aux){
  struct list_item *c = list_entry(a, struct list_item, elem);
  struct list_item *d = list_entry(b, struct list_item, elem);
  if (c->data < d->data) return true;
  else return false;
}

// hash_hash_func
unsigned hash_hash(const struct hash_elem *a, void *aux){
	struct hash_item *newhash = hash_entry(a, struct hash_item, elem);
	return hash_int(newhash->data);
}

// hash_less_func
bool hash_less(const struct hash_elem *a, const struct hash_elem *b, void *aux){
	struct hash_item *c = hash_entry(a, struct hash_item, elem);
	struct hash_item *d = hash_entry(b, struct hash_item, elem);
	if (c->data < d->data) return true;
	else return false;
}

// hash_action_func
void hash_destructor(struct hash_elem *a, void *aux){
	struct hash_item *desitem = hash_entry(a, struct hash_item, elem);
	free(desitem);
	return;
}

void hash_square(struct hash_elem *a, void *aux){
	struct hash_item *sitem = hash_entry(a, struct hash_item, elem);
	sitem->data *= sitem->data;
	return;
}

void hash_triple(struct hash_elem *a, void *aux){
	struct hash_item *titem = hash_entry(a, struct hash_item, elem);
	titem->data *= titem->data * titem->data;
	return;
}

int main(int argc, char *argv[]){
	int i, j, k;
	while(1){
		if(fgets(command, 100, stdin) == NULL) continue;
		for (i = 0; i < strlen(command); i++) {
			if (command[i] == '\n') {
				command[i] = '\0';
				break;
			}
		}
		if (!strcmp(command, "quit")) break; // quit
		sscanf(command, "%s", line[0]);
		if (!strcmp(line[0], "create")){ // create
			sscanf(command, "%s %s", line[0], line[1]);
			if (!strcmp(line[1], "list")){ // create list
				struct list *tmplist = (struct list*)malloc(sizeof(struct list));
				list_init(tmplist);
				lists[listnum] = tmplist;
				sscanf(command, "%s %s %s", line[0], line[1], line[2]);
				strcpy(listname[listnum], line[2]);
				listnum++;
			}
			else if (!strcmp(line[1], "bitmap")){ // create bitmap
				int bitmapsize;
				sscanf(command, "%s %s %s %d", line[0], line[1], line[2], &bitmapsize);
				struct bitmap *tmpbitmap = bitmap_create(bitmapsize);
				bitmaps[bitmapnum] = tmpbitmap;
				strcpy(bitmapname[bitmapnum], line[2]);
				bitmapnum++;
			}
			else if (!strcmp(line[1], "hashtable")){ // create hashtable
				struct hash *newhash = (struct hash*)malloc(sizeof(struct hash));
				void *aux;
				hash_init(newhash, hash_hash, hash_less, aux);
				hashes[hashnum] = newhash;
				sscanf(command, "%s %s %s", line[0], line[1], line[2]);
				strcpy(hashname[hashnum], line[2]);
				hashnum++;
			}
		}
		else if (!strcmp(line[0], "dumpdata")){ // print data
			sscanf(command, "%s %s", line[0], line[1]);
			if (!strncmp(line[1], "list", 4)){ // print data of list
				for (i = 0; i < listnum; i++){
					if (!strcmp(listname[i], line[1])) break;
				}
				struct list_elem *curr = list_begin(lists[i]);
				int s = list_size(lists[i]);
				int flag = 0;
				for (j = 0; j < s; j++){
					struct list_item *temp = list_entry(curr, struct list_item, elem);
					printf("%d ", temp->data);
					curr = list_next(curr);
					if (!flag) flag++;
				}
				if (flag) printf("\n");
			}
			else if (!strncmp(line[1], "bm", 2)){ // print data of bitmap
				for (i = 0; i < bitmapnum; i++)
					if (!strcmp(bitmapname[i], line[1])) break;
				int s = bitmap_size(bitmaps[i]), flag = 0;
				for (j = 0; j < s; j++){
					if (bitmap_test(bitmaps[i], j)) printf("1");
					else printf("0");
					if (!flag) flag++;
				}
				if (flag) printf("\n");
			}
			else if (!strncmp(line[1], "hash", 4)){ // print data of hashtable
				for (i = 0; i < hashnum; i++)
					if (!strcmp(hashname[i], line[1])) break;
				int s = hashes[i]->bucket_cnt, flag = 0;
				for (j = 0; j < s; j++){
					struct list_elem *curr = list_begin(&(hashes[i]->buckets[j]));
					while(curr != list_end(&(hashes[i]->buckets[j]))){
						struct hash_elem *tmp = list_entry(curr, struct hash_elem, list_elem);
						struct hash_item *temp = hash_entry(tmp, struct hash_item, elem);
						printf("%d ", temp->data);
						curr = list_next(curr);
						if (!flag) flag++;
					}
				}
				if (flag) printf("\n");
			}
		}
		//   list func
		else if (!strcmp(line[0], "list_push_front")){  // push_front
			int pushdata;
			sscanf(command, "%s %s %d", line[0], line[1], &pushdata);
			for (i = 0; i < listnum; i++)
				if (!strcmp(listname[i], line[1])) break;
			struct list_item *temp = (struct list_item*)malloc(sizeof(struct list_item));
			temp->data = pushdata;
			list_push_front(lists[i], &temp->elem);
		}
		else if (!strcmp(line[0], "list_push_back")){  // push_back
			int pushdata;
			sscanf(command, "%s %s %d", line[0], line[1], &pushdata);
			for (i = 0; i < listnum; i++)
				if (!strcmp(listname[i], line[1])) break;
			struct list_item *temp = (struct list_item*)malloc(sizeof(struct list_item));
			temp->data = pushdata;
			list_push_back(lists[i], &temp->elem);
		}
		else if (!strcmp(line[0], "list_pop_front")){ // pop_front 
			sscanf(command, "%s %s", line[0], line[1]);
			for (i = 0; i < listnum; i++)
				if (!strcmp(listname[i], line[1])) break;
			struct list_elem *tmp = list_pop_front(lists[i]);
			free(tmp);
		}
		else if (!strcmp(line[0], "list_pop_back")){  // pop_back
			sscanf(command, "%s %s", line[0], line[1]);
			for (i = 0; i < listnum; i++)
				if (!strcmp(listname[i], line[1])) break;
			struct list_elem *tmp = list_pop_back(lists[i]);
			free(tmp);
		}
		else if (!strcmp(line[0], "list_insert")){  // insert
			int idx, pushdata;
			sscanf(command, "%s %s %d %d", line[0], line[1], &idx, &pushdata);
			for (i = 0; i < listnum; i++)
				if (!strcmp(listname[i], line[1])) break;
			struct list_item *tmp = (struct list_item*)malloc(sizeof(struct list_item));
			tmp->data = pushdata;
			list_push_front(lists[i], &tmp->elem);
			struct list_elem *curr = list_begin(lists[i]);
			for (i = 0; i < idx; i++){
				list_swap(curr, curr->next);
				curr = curr->next;
			}
		}
		else if (!strcmp(line[0], "list_insert_ordered")){  // insert_ordered
			int pushdata;
			sscanf(command, "%s %s %d", line[0], line[1], &pushdata);
			for (i = 0; i < listnum; i++)
				if (!strcmp(listname[i], line[1])) break;
			struct list_item *tmp = (struct list_item*)malloc(sizeof(struct list_item));
			tmp->data = pushdata;
			list_push_front(lists[i], &tmp->elem);
			struct list_elem *curr = list_begin(lists[i]);
			for (j = 0; j < list_size(lists[i]) - 1; j++){
				struct list_item *a = list_entry(curr, struct list_item, elem);
				struct list_item *b = list_entry(curr->next, struct list_item, elem);
				if (a->data > b->data) list_swap(curr, curr->next);
				curr = curr->next;
			}
		}
		else if (!strcmp(line[0], "list_empty")){  // list_empty
			sscanf(command, "%s %s", line[0], line[1]);
			for (i = 0; i < listnum; i++)
				if (!strcmp(listname[i], line[1])) break;
			if (list_empty(lists[i])) printf("true\n");
			else printf("false\n");
		}
		else if (!strcmp(line[0], "list_size")){  // list_size
			sscanf(command, "%s %s", line[0], line[1]);
			for (i = 0; i < listnum; i++)
				if (!strcmp(listname[i], line[1])) break;
			printf("%zu\n", list_size(lists[i]));
		}
		else if (!strcmp(line[0], "list_max")){  // list_max
			sscanf(command, "%s %s", line[0], line[1]);
			for (i = 0; i < listnum; i++)
				if (!strcmp(listname[i], line[1])) break;
			void *aux;
			struct list_elem *tmp = list_max(lists[i], list_less, aux);
			struct list_item *ans = list_entry(tmp, struct list_item, elem);
			printf("%d\n", ans->data);
		}
		else if (!strcmp(line[0], "list_min")){  //list_min
			sscanf(command, "%s %s", line[0], line[1]);
			for (i = 0; i < listnum; i++)
				if (!strcmp(listname[i], line[1])) break;
			void *aux;
			struct list_elem *tmp = list_min(lists[i], list_less, aux);
			struct list_item *ans = list_entry(tmp, struct list_item, elem);
			printf("%d\n", ans->data);
		}
		else if (!strcmp(line[0], "list_remove")){  //list_remove
			int rmidx;
			sscanf(command, "%s %s %d", line[0], line[1], &rmidx);
			for (i = 0; i < listnum; i++)
				if (!strcmp(listname[i], line[1])) break;
			struct list_elem *curr = list_begin(lists[i]);
			for (j = 0; j < rmidx; j++)
				curr = curr->next;
			list_remove(curr);
			free(curr);
		}
		else if (!strcmp(line[0], "list_reverse")){  // list_reverse
			sscanf(command, "%s %s", line[0], line[1]);
			for (i = 0; i < listnum; i++)
				if (!strcmp(listname[i], line[1])) break;
			list_reverse(lists[i]);
		}
		else if (!strcmp(line[0], "list_shuffle")){  // list_shuffle
			sscanf(command, "%s %s", line[0], line[1]);
			for (i = 0; i < listnum; i++)
				if (!strcmp(listname[i], line[1])) break;
			list_shuffle(lists[i]);
		}
		else if (!strcmp(line[0], "list_sort")){  //list_sort
			sscanf(command, "%s %s", line[0], line[1]);
			for (i = 0; i < listnum; i++)
				if (!strcmp(listname[i], line[1])) break;
			void *aux;
			list_sort(lists[i], list_less, aux);
		}
		else if (!strcmp(line[0], "list_splice")){  //list_sort
			int insidx, fromidx, toidx;
			sscanf(command, "%s %s %d %s %d %d", line[0], line[1], &insidx, line[2], &fromidx, &toidx);
			for (i = 0; i < listnum; i++)
				if (!strcmp(listname[i], line[1])) break;
			for (j = 0; j < listnum; j++)
				if (!strcmp(listname[j], line[2])) break;
			struct list_elem *where = list_begin(lists[i]);
			struct list_elem *From = list_begin(lists[j]);
			struct list_elem *To = list_begin(lists[j]);
			for (k = 0; k < insidx; k++) where = list_next(where);
			for (k = 0; k < fromidx; k++) From = list_next(From);
			for (k = 0; k < toidx; k++) To = list_next(To);
			list_splice(where, From, To);
		}
		else if (!strcmp(line[0], "list_swap")){  // list_swap
			int aidx, bidx;
			sscanf(command, "%s %s %d %d", line[0], line[1], &aidx, &bidx);
			for (i = 0; i < listnum; i++)
				if (!strcmp(listname[i], line[1])) break;
			struct list_elem *a = list_begin(lists[i]);
			struct list_elem *b = list_begin(lists[i]);
			for (j = 0; j < aidx; j++) a = list_next(a);
			for (j = 0; j < bidx; j++) b = list_next(b);
			list_swap(a, b);
		}
		else if (!strcmp(line[0], "list_unique")){  // list_unique
			int flag = 0;
			void *aux;
			sscanf(command, "%s %s", line[0], line[1]);
			if (strlen(command) > strlen(line[0]) + strlen(line[1]) + 1){
				sscanf(command, "%s %s %s", line[0], line[1], line[2]);
				flag++;
			}
			if (!flag){
				for (i = 0; i < listnum; i++)
					if (!strcmp(listname[i], line[1])) break;
				list_unique(lists[i], NULL, list_less, aux);
			}
			else{
				for (i = 0; i < listnum; i++)
					if (!strcmp(listname[i], line[1])) break;
				for (j = 0; j < listnum; j++)
					if (!strcmp(listname[j], line[2])) break;
				list_unique(lists[i], lists[j], list_less, aux);
			}
		}
		else if (!strcmp(line[0], "list_front")){  // list_front
			sscanf(command, "%s %s", line[0], line[1]);
			for (i = 0; i < listnum; i++)
				if (!strcmp(listname[i], line[1])) break;
			struct list_elem *tmp = list_front(lists[i]);
			struct list_item *temp = list_entry(tmp, struct list_item, elem);
			printf("%d\n", temp->data);
		}
		else if (!strcmp(line[0], "list_back")){  // list_back
			sscanf(command, "%s %s", line[0], line[1]);
			for (i = 0; i < listnum; i++)
				if (!strcmp(listname[i], line[1])) break;
			struct list_elem *tmp = list_back(lists[i]);
			struct list_item *temp = list_entry(tmp, struct list_item, elem);
			printf("%d\n", temp->data);
		}
		//   bitmap func
		else if (!strcmp(line[0], "bitmap_mark")){  // bitmap_mark
			size_t markidx;
			sscanf(command, "%s %s %zu", line[0], line[1], &markidx);
			for (i = 0; i < bitmapnum; i++)
				if (!strcmp(bitmapname[i], line[1])) break;
			bitmap_mark(bitmaps[i], markidx);
		}
		else if (!strcmp(line[0], "bitmap_set")){  // bitmap_set
			size_t idx;
			sscanf(command, "%s %s %zu %s", line[0], line[1], &idx, line[2]);
			for (i = 0; i < bitmapnum; i++)
				if (!strcmp(bitmapname[i], line[1])) break;
			if (!strcmp(line[2], "true")) 
				bitmap_set(bitmaps[i], idx, true);
			else if (!strcmp(line[2], "false"))
				bitmap_set(bitmaps[i], idx, false);
		}
		else if (!strcmp(line[0], "bitmap_set_all")){  //bitamp_set_all
			sscanf(command, "%s %s %s", line[0], line[1], line[2]);
			for (i = 0; i < bitmapnum; i++)
				if (!strcmp(bitmapname[i], line[1])) break;
			if (!strcmp(line[2], "true"))
				bitmap_set_all(bitmaps[i], true);
			else if (!strcmp(line[2], "false"))
				bitmap_set_all(bitmaps[i], false);
		}
		else if (!strcmp(line[0], "bitmap_set_multiple")){  // bitmap_set_multiple
			size_t From, Cnt;
			sscanf(command, "%s %s %zu %zu %s", line[0], line[1], &From, &Cnt, line[2]);
			for (i = 0; i < bitmapnum; i++)
				if (!strcmp(bitmapname[i], line[1])) break;
			if (!strcmp(line[2], "true"))
				bitmap_set_multiple(bitmaps[i], From, Cnt, true);
			else if (!strcmp(line[2], "false"))
				bitmap_set_multiple(bitmaps[i], From, Cnt, false);
		}
		else if (!strcmp(line[0], "bitmap_expand")){  // bitmap_expand
			int expandsize;
			sscanf(command, "%s %s %d", line[0], line[1], &expandsize);
			for (i = 0; i < bitmapnum; i++)
				if (!strcmp(bitmapname[i], line[1])) break;
			struct bitmap *newbm = bitmap_expand(bitmaps[i], expandsize);
			bitmap_destroy(bitmaps[i]);
			bitmaps[i] = newbm;
		}
		else if (!strcmp(line[0], "bitmap_all")){  // bitmap_all
			size_t st, ed;
			sscanf(command, "%s %s %zu %zu", line[0], line[1], &st, &ed);
			for (i = 0; i < bitmapnum; i++)
				if (!strcmp(bitmapname[i], line[1])) break;
			if (bitmap_all(bitmaps[i], st, ed)) printf("true\n");
			else printf("false\n");
		}
		else if (!strcmp(line[0], "bitmap_any")){  // bitmap_any
			size_t st, ed;
			sscanf(command, "%s %s %zu %zu", line[0], line[1], &st, &ed);
			for (i = 0; i < bitmapnum; i++)
				if (!strcmp(bitmapname[i], line[1])) break;
			if (bitmap_any(bitmaps[i], st, ed)) printf("true\n");
			else printf("false\n");
		}
		else if (!strcmp(line[0], "bitmap_contains")){  // bitmap_contains
			size_t st, ed;
			sscanf(command, "%s %s %zu %zu %s", line[0], line[1], &st, &ed, line[2]);
			bool key = false;
			if (!strcmp(line[2], "true")) key = true;
			for (i = 0; i < bitmapnum; i++)
				if (!strcmp(bitmapname[i], line[1])) break;
			if (bitmap_contains(bitmaps[i], st, ed, key))
				printf("true\n");
			else printf("false\n");
		}
		else if (!strcmp(line[0], "bitmap_count")){  // bitmap_count
			size_t st, ed;
			sscanf(command, "%s %s %zu %zu %s", line[0], line[1], &st, &ed, line[2]);
			bool key = false;
			if (!strcmp(line[2], "true")) key = true;
			for (i = 0; i < bitmapnum; i++)
				if (!strcmp(bitmapname[i], line[1])) break;
			printf("%zu\n", bitmap_count(bitmaps[i], st, ed, key));
		}
		else if (!strcmp(line[0], "bitmap_dump")){  // bitmap_dump
			sscanf(command, "%s %s", line[0], line[1]);
			for (i = 0; i < bitmapnum; i++)
				if (!strcmp(bitmapname[i], line[1])) break;
			bitmap_dump(bitmaps[i]);
		}
		else if (!strcmp(line[0], "bitmap_flip")){  // bitmap_flip
			size_t idx;
			sscanf(command, "%s %s %zu", line[0], line[1], &idx);
			for (i = 0; i < bitmapnum; i++)
				if (!strcmp(bitmapname[i], line[1])) break;
			bitmap_flip(bitmaps[i], idx);
		}
		else if (!strcmp(line[0], "bitmap_none")){  // bitmap_none
			size_t st, ed;
			sscanf(command, "%s %s %zu %zu", line[0], line[1], &st, &ed);
			for (i = 0; i < bitmapnum; i++)
				if (!strcmp(bitmapname[i], line[1])) break;
			if (bitmap_none(bitmaps[i], st, ed)) printf("true\n");
			else printf("false\n");
		}
		else if (!strcmp(line[0], "bitmap_reset")){  // bitmap_reset
			size_t idx;
			sscanf(command, "%s %s %zu", line[0], line[1], &idx);
			for (i = 0; i < bitmapnum; i++)
				if (!strcmp(bitmapname[i], line[1])) break;
			bitmap_reset(bitmaps[i], idx);
		}
		else if (!strcmp(line[0], "bitmap_scan")){  // bitmap_scan
			size_t st, ed;
			sscanf(command, "%s %s %zu %zu %s", line[0], line[1], &st, &ed, line[2]);
			for (i = 0; i < bitmapnum; i++)
				if (!strcmp(bitmapname[i], line[1])) break;
			bool key = false;
			if (!strcmp(line[2], "true")) key = true;
			printf("%zu\n", bitmap_scan(bitmaps[i], st, ed, key));
		}
		else if (!strcmp(line[0], "bitmap_scan_and_flip")){  // bitmap_scan_and_flip
			size_t st, ed;
			sscanf(command, "%s %s %zu %zu %s", line[0], line[1], &st, &ed, line[2]);
			for (i = 0; i < bitmapnum; i++)
				if (!strcmp(bitmapname[i], line[1])) break;
			bool key = false;
			if (!strcmp(line[2], "true")) key = true;
			printf("%zu\n", bitmap_scan_and_flip(bitmaps[i], st, ed, key));
		}
		else if (!strcmp(line[0], "bitmap_size")){  // bitmap_size
			sscanf(command, "%s %s", line[0], line[1]);
			for (i = 0; i < bitmapnum; i++)
				if (!strcmp(bitmapname[i], line[1])) break;
			printf("%zu\n", bitmap_size(bitmaps[i]));
		}
		else if (!strcmp(line[0], "bitmap_test")){  // bitmap_test
			size_t idx;
			sscanf(command, "%s %s %zu", line[0], line[1], &idx);
			for (i = 0; i < bitmapnum; i++)
				if (!strcmp(bitmapname[i], line[1])) break;
			if (bitmap_test(bitmaps[i], idx)) printf("true\n");
			else printf("false\n");
		}
		//   hash func
		else if (!strcmp(line[0], "hash_insert")){  // hash_insert
			int newdata;
			sscanf(command, "%s %s %d", line[0], line[1], &newdata);
			for (i = 0; i < hashnum; i++)
				if (!strcmp(hashname[i], line[1])) break;
			struct hash_elem *tmp = (struct hash_elem*)malloc(sizeof(struct hash_elem));
			struct hash_item *temp = hash_entry(tmp, struct hash_item, elem);
			temp->data = newdata;
			hash_insert(hashes[i], tmp);
		}
		else if (!strcmp(line[0], "hash_delete")){  // hash_delete
			int delkey;
			sscanf(command, "%s %s %d", line[0], line[1], &delkey);
			for (i = 0; i < hashnum; i++)
				if (!strcmp(hashname[i], line[1])) break;
			struct hash_elem *tmp = (struct hash_elem*)malloc(sizeof(struct hash_elem));
			struct hash_item *temp = hash_entry(tmp, struct hash_item, elem);
			temp->data = delkey;
			hash_delete(hashes[i], tmp);
		}
		else if (!strcmp(line[0], "hash_empty")){  // hash_empty
			sscanf(command, "%s %s", line[0], line[1]);
			for (i = 0; i < hashnum; i++)
				if (!strcmp(hashname[i], line[1])) break;
			if (hash_empty(hashes[i])) printf("true\n");
			else printf("false\n");
		}
		else if (!strcmp(line[0], "hash_size")){  // hash_size
			sscanf(command, "%s %s", line[0], line[1]);
			for (i = 0; i < hashnum; i++)
				if (!strcmp(hashname[i], line[1])) break;
			printf("%zu\n", hash_size(hashes[i]));
		}
		else if (!strcmp(line[0], "hash_clear")){  // hash_clear
			sscanf(command, "%s %s", line[0], line[1]);
			for (i = 0; i < hashnum; i++)
				if (!strcmp(hashname[i], line[1])) break;
			hash_clear(hashes[i], hash_destructor);
		}
		else if (!strcmp(line[0], "hash_find")){  // hash_find
			int findkey;
			sscanf(command, "%s %s %d", line[0], line[1], &findkey);
			for (i = 0; i < hashnum; i++)
				if (!strcmp(hashname[i], line[1])) break;
			struct hash_elem *tmp = (struct hash_elem*)malloc(sizeof(struct hash_elem));
			struct hash_item *temp = hash_entry(tmp, struct hash_item, elem);
			temp->data = findkey;
			struct hash_elem *F = hash_find(hashes[i], tmp);
			if (F){
				struct hash_item *Fitem = hash_entry(F, struct hash_item, elem);
				printf("%d\n", Fitem->data);
			}
		}
		else if (!strcmp(line[0], "hash_replace")){  // hash_replace
			int repkey;
			sscanf(command, "%s %s %d", line[0], line[1], &repkey);
			for (i = 0; i < hashnum; i++)
				if (!strcmp(hashname[i], line[1])) break;
			struct hash_elem *tmp = (struct hash_elem*)malloc(sizeof(struct hash_elem));
			struct hash_item *temp = hash_entry(tmp, struct hash_item, elem);
			temp->data = repkey;
			struct hash_elem *o = hash_replace(hashes[i], tmp);
			if (o) free(o);
		}
		else if (!strcmp(line[0], "hash_apply")){  // hash_apply
			sscanf(command, "%s %s %s", line[0], line[1], line[2]);
			for (i = 0; i < hashnum; i++)
				if (!strcmp(hashname[i], line[1])) break;
			if (!strcmp(line[2], "square"))
				hash_apply(hashes[i], hash_square);
			else if (!strcmp(line[2], "triple"))
				hash_apply(hashes[i], hash_triple);
		}
		//////////// delete
		else if (!strcmp(line[0], "delete")){
			sscanf(command, "%s %s", line[0], line[1]);
			if (!strncmp(line[1], "list", 4)){
				for (i = 0; i < listnum; i++)
					if (!strcmp(listname[i], line[1])) break;
				int s = list_size(lists[i]);
				for (j = 0; j < s; j++){
					struct list_elem *curr = list_begin(lists[i]);
					list_remove(curr);
					free(curr);
				}
				free(lists[i]);
				listnum--;
				for (j = i; j < listnum; j++){
					lists[j] = lists[j + 1];
					strcpy(listname[j], listname[j + 1]);
				}
				listname[j][0] = '\0';
			}
			else if (!strncmp(line[1], "bm", 2)){
				for (i = 0; i < bitmapnum; i++)
					if (!strcmp(bitmapname[i], line[1])) break;
				bitmapnum--;
				bitmap_destroy(bitmaps[i]);
				for (j = i; j < bitmapnum; j++){
					bitmaps[j] = bitmaps[j + 1];
					strcpy(bitmapname[j], bitmapname[j + 1]);
				}
				bitmapname[j][0] = '\0';
			}
			else if (!strncmp(line[1], "hash", 4)){
				for (i = 0; i < hashnum; i++)
					if (!strcmp(hashname[i], line[1])) break;
				hashnum--;
				hash_destroy(hashes[i], hash_destructor);
				for (j = i; j < hashnum; j++){
					hashes[j] = hashes[j + 1];
					strcpy(hashname[j], hashname[j + 1]);
				}
				hashname[j][0] = '\0';
			}
		}
	}
	

	return 0;
}
