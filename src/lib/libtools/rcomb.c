
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "rcomb.h"

/* ---------------------------------------------------------------------- */
/* heap implementation (CLR) */


typedef struct heap_elt_t {
  int key;
  void *val;
} heap_elt_t;

typedef struct heap_t {
  int size;
  int capacity;
  heap_elt_t data[0];			/* variable size */
} heap_t;
/* data[0] unused */


heap_t *heap_new(int n);
void heap_delete(heap_t *heap);
void *heap_extract_max(heap_t *heap);
void heap_insert(heap_t *heap, int key, void *val);
int heap_size(const heap_t *heap);


#define PARENT(n) ((n)>>1)
#define LEFT(n) ((n)<<1)
#define RIGHT(n) (((n)<<1)+1)

heap_t *
heap_new(int n) {
  heap_t *heap;

  heap = (heap_t *)malloc(sizeof(heap_t) + sizeof(heap_elt_t) * (n+1));
  assert(heap);
  heap->size = 0;
  heap->capacity = n;
  return heap;
}

void
heap_delete(heap_t *heap) {
  if(heap) {
    free(heap);
  }
}

int
heap_size(const heap_t *heap) {
  return heap->size;
}

static void
heapify(heap_t *heap, int i) {
  int l, r;
  int largest;

  l = LEFT(i);
  r = RIGHT(i);
  if(l < heap->size && heap->data[l].key > heap->data[i].key) {
    largest = l;
  } else {
    largest = i;
  }
  if(r < heap->size && heap->data[i].key > heap->data[largest].key) {
    largest = r;
  }
  if(largest != i) {
    heap_elt_t tmp = heap->data[i];
    heap->data[i] = heap->data[largest];
    heap->data[largest] = tmp;
    heapify(heap, largest);
  }
}

void *
heap_extract_max(heap_t *heap) {
  heap_elt_t max;
  
  assert(heap->size > 0);
  max = heap->data[1];
  heap->data[1] = heap->data[heap->size];
  heap->size--;
  heapify(heap, 1);
  return max.val;
}

const void *
heap_max(heap_t *heap) {

  assert(heap->size > 0);
  return heap->data[1].val;
}

void
heap_insert(heap_t *heap, int key, void *val) {
  int i;

  assert(heap->size < heap->capacity);  
  heap->size++;
  i = heap->size;
  while(i > 1 && heap->data[PARENT(i)].key < key) {
    heap->data[i] = heap->data[PARENT(i)];
    i = PARENT(i);
  }
  heap->data[i].key = key;
  heap->data[i].val = val;
}
  

/* ---------------------------------------------------------------------- */

#define PM_VALID_MAGIC 0xaf95
#define PM_CHECK_VALID(x) (assert((x)->valid == PM_VALID_MAGIC))

permutation_t *
pmNew(int n) {
  permutation_t *ptr;

  assert(n >= 0);
  ptr = (permutation_t *)malloc(sizeof(permutation_t) +
				sizeof(pelt_t) * n);
  assert(ptr);
  ptr->size = 0;
  ptr->capacity = n;
  ptr->valid = PM_VALID_MAGIC;

  return ptr;
}

void
pmDelete(permutation_t *ptr) {
  if(ptr) {
    PM_CHECK_VALID(ptr);
    ptr->valid = 0;
    free(ptr);
  }
}


void
pmCopy(permutation_t *copy, const permutation_t *ptr) {
  int i;
  
  PM_CHECK_VALID(ptr);
  PM_CHECK_VALID(copy);
  assert(copy->capacity >= ptr->size);
  for(i=0; i<ptr->size; i++) {
    copy->elements[i] = ptr->elements[i];
  }
  copy->size = ptr->size;
}


void
pmCopyAll(permutation_t *copy, const permutation_t *ptr) {
  int i;
  
  PM_CHECK_VALID(ptr);
  PM_CHECK_VALID(copy);
  assert(copy->capacity >= ptr->capacity);
  for(i=0; i<ptr->capacity; i++) {
    copy->elements[i] = ptr->elements[i];
  }
  copy->size = ptr->size;
}

permutation_t *
pmDup(const permutation_t *ptr) {
  permutation_t *copy;
  int i;

  PM_CHECK_VALID(ptr);
  copy = pmNew(ptr->capacity);
  /* copy all elements, not just valid ones (overloaded semantic XXX) */
  for(i=0; i<ptr->capacity; i++) {
    copy->elements[i] = ptr->elements[i];
  }
  copy->size = ptr->size;
  return copy;
}

pelt_t
pmElt(const permutation_t *pm, int i) {
  PM_CHECK_VALID(pm);
  assert(i < pm->capacity);
  //assert(i < pm->size);
  return pm->elements[i];
}

void
pmSetElt(permutation_t *pm, int i, pelt_t val) {
  PM_CHECK_VALID(pm);
  assert(i < pm->capacity);
  if(i >= pm->size) {
    pm->size = i + 1;
  }
  pm->elements[i] = val;
}

int
pmSize(permutation_t *pm) {
  return pm->size;
}

void
pmSetSize(permutation_t *pm, int n) {
  assert(n <= pm->capacity);
  pm->size = n;
}

void
pmIdentity(permutation_t *pm) {
  int i;
  
  for(i=0; i<pm->capacity; i++) {
    pm->elements[i] = i;
  }
  pm->size = pm->capacity;
}

const pelt_t *
pmArr(const permutation_t *pm) {
  PM_CHECK_VALID(pm);
  return pm->elements;
}

int
pmLength(const permutation_t *pm) {
  PM_CHECK_VALID(pm);
  return pm->size;
}

void
pmSwap(permutation_t *pm, int i, int j) {
  pelt_t tmp;

  PM_CHECK_VALID(pm);
  //assert(i < pm->size);
  //assert(j < pm->size);
  tmp = pm->elements[i];
  pm->elements[i] = pm->elements[j];
  pm->elements[j] = tmp;
}

int
pmEqual(const permutation_t *pm1, const permutation_t *pm2) {
  int i;

  if(pm1->size != pm2->size) {
    return 0;
  }
  for(i=0; i<pm1->size; i++) {
    if(pm1->elements[i] != pm2->elements[i]) {
      return 0;
    }
  }
  return 1;
}


char *
pmPrint(const permutation_t *pm, char *buf, int bufsiz) {
  int i;
  char buf2[BUFSIZ];

  sprintf(buf, "[");  
  for(i=0; i<pm->size; i++) {
    sprintf(buf2, "%s%d", (i?" ":""), pm->elements[i]);
    if(strlen(buf) + strlen(buf2) > bufsiz) {
      break;
    }
    strcat(buf, buf2);
  }
  strcat(buf, "]");
  return buf;
}

/* ---------------------------------------------------------------------- */
/* poset functions */

static int poGet(const partial_order_t *po, int u, int v);


partial_order_t *
poNew(int n) {
  partial_order_t *po;

  po = (partial_order_t *)malloc(sizeof(partial_order_t) +
				 n * n * sizeof(char));
  assert(po);
  po->dim = n;
  memset(po->data, PO_INCOMPARABLE, n * n * sizeof(char));

  return po;
}

void
poDelete(partial_order_t *po) {
  if(po) {
    free(po);
  }
}

int
poIsMin(const partial_order_t *po, int u) {
  int i;
  for(i=0; i<po->dim; i++) {
    if(poGet(po, u, i) == PO_GT) {
      return 0;
    }
  }
  return 1;
}

void
poPrint(partial_order_t *po) {
  int i,j;

  printf("   ");
  for(i=0; i<po->dim; i++) {
    printf(" %1x", i);
  }
  printf("\n");

  for(i=0; i<po->dim; i++) {
    printf(" %2d", i);
    for(j=0; j<po->dim; j++) {
      char c = ' ';
      switch(poGet(po, i, j)) {
      case PO_EQ:
	c = '=';
	break;
      case PO_LT:
	c = '<';
	break;
      case PO_GT:
	c = '>';
	break;
      default:
	c = '?';
      }
      printf(" %c", c);
    }
    printf("\n");
  }
}

static po_relation_t
poInverse(po_relation_t rel) {
  return (rel == PO_INCOMPARABLE ? rel : -rel);
}
    

void
poSetOrder(partial_order_t *po, int u, int v, po_relation_t rel) {
  assert(u < po->dim);
  assert(v < po->dim);
  po->data[u * po->dim + v] = rel;
  po->data[v * po->dim + u] = poInverse(rel);
}

static int
poGet(const partial_order_t *po, int u, int v) {
  assert(u < po->dim);
  assert(v < po->dim);
  return po->data[u * po->dim + v];
}

void
poClosure(partial_order_t *po) {
  int i, j, k;
  int n = po->dim;

  /* Warshall's alg */
  for (k = 0; k < n; k++) {
    for (i = 0; i < n; i++) {
      for (j = 0; j < n; j++) {
	if (poIncomparable(po, i, j)) {
	  if( poGet(po, i, k) == poGet(po, k, j) ) {
	    poSetOrder(po, i, j, poGet(po, i, k));
	  }
	}
      }
    }
  }
}

int
poIncomparable(const partial_order_t *po, int u, int v) {
  char cmp;

  assert(u < po->dim);
  assert(v < po->dim);
  cmp = po->data[u * po->dim + v];

 /*  if(cmp == PO_INCOMPARABLE) { */
/*     printf("INCMP: %d %d\n", u, v); */
/*   } */

  return (cmp == PO_INCOMPARABLE);
}

int
poComparable(const partial_order_t *po, int u, int v) {
  char cmp;

  assert(u < po->dim);
  assert(v < po->dim);
  cmp = po->data[u * po->dim + v];

  return (cmp != PO_INCOMPARABLE);
}

/* ---------------------------------------------------------------------- */



/* some permutation algorithms adapted from the Perl versions by Rahul S.
 * Rajiv Wickremesinghe 5/2003
 */



#define SWAP_PTR(p1,p2)				\
{						\
  void *tmp;					\
  tmp = p1;					\
  p1 = p2;					\
  p2 = tmp;					\
}




/* 
 * single iteration of hill climbing.
 * returns a new permutation
 */

void
hill_climb_init(hc_state_t *ptr, const permutation_t *start) {
  ptr->best_seq = pmDup(start);
  ptr->next_seq = pmNew(pmLength(start));
  ptr->n = pmLength(start);
  ptr->i = 0;
  ptr->j = 1;
  ptr->improved = 1;
}



void
hill_climb_cleanup(hc_state_t *ptr) {
  pmDelete(ptr->best_seq);
  pmDelete(ptr->next_seq);
}


/* verify all the pairwise swaps that would be necessary to do a
 * general swap between u,v */
static int
check_valid_swap(const partial_order_t *po, const permutation_t *perm, int u, int v) {
  int i;

  if(poComparable(po, pmElt(perm, u), pmElt(perm, v))) {
    return 0;
  }
  for(i=u+1; i<v; i++) {
    if( poComparable(po, pmElt(perm, u), pmElt(perm, i)) ||
        poComparable(po, pmElt(perm, i), pmElt(perm, v)) ) {
      return 0;
    }
  }
  return 1;
}


/* we have the option of exhaustively looking at all the neighboring
 * permutations and picking the best, or we could just greedily pick
 * the first improving one.. */

int
hill_climb_step(hc_state_t *hc, const partial_order_t *po, 
		evaluation_func_t evf, void *context) {
  int i,j;
  int err=0;
  int n = hc->n;
  int best_score;
	int next_score;
  char buf[BUFSIZ];

  /* evaluate (re-evaluate?) current state */
  err = evf(context, hc->best_seq, &best_score);

  if(err) {
    printf("could not evaluate current best!\n");
    err = RC_ERR_NODATA;
    pmCopy(hc->next_seq, hc->best_seq);
  }

  while(!err && hc->improved) {
    printf("best: %s, score=%d\n", pmPrint(hc->best_seq, buf, BUFSIZ),
	   best_score);

    /* test seq */
    pmCopy(hc->next_seq, hc->best_seq);
    
    i = hc->i;
    j = hc->j;

    while(i < n-1) {
      /* check if this is a valid swap. if there is a partial order, we can't swap */
      /* next_seq == best_seq at this point */
      int valid_swap = 0;
      valid_swap = check_valid_swap(po, hc->next_seq, i, j);
      if(valid_swap) {
	/* generate swapped perm, evaluate */
	pmSwap(hc->next_seq, i, j);
	
	/* evaluate option */
	err = evf(context, hc->next_seq, &next_score);
	if(err) {
	  goto done;
	}

	printf("permutation %d/%d: %s, score=%d... ", i, j, pmPrint(hc->next_seq, buf, BUFSIZ),
	       next_score);

	/* keep track of best */
	if(next_score > best_score) {
	  printf("improved!\n");
	  hc->improved = 1;
	  best_score = next_score;
	  pmCopy(hc->best_seq, hc->next_seq);
	} else {
	  printf("rejected!\n");
	}
	/* swap back to generate original (cheaper than copy) */
	pmSwap(hc->next_seq, i, j);
      }

      /* update loop */
      j++;
      if(j>=n) {
	i++;
	j = i+1;
      }
    } /* while(i.. */

    /* reset loop */
    i = 0;
    j = 1;
    hc->improved = 0;

  done:
    hc->i = i;
    hc->j = j;
  }
  
  /* check if we are done */
  if(!err && hc->improved == 0) {
    err = RC_ERR_COMPLETE;
  }

  return err;
}

const permutation_t *
hill_climb_result(hc_state_t *hc) {
  return hc->best_seq;
}

const permutation_t *
hill_climb_next(hc_state_t *hc) {
  return hc->next_seq;
}

/* ********************************************************************** */


/* 
 */

void
best_first_init(bf_state_t *ptr, int n, const partial_order_t *po,
		evaluation_func_t evf, const void *context) {
  ptr->n = n;
  ptr->i = 0;
  ptr->j = 0;
  ptr->improved = 1;
  ptr->pq = heap_new(n * n);	/* XXX */
  ptr->po = po;
  ptr->best_seq = pmNew(n);
  ptr->next_seq = pmNew(n);

  ptr->evfunc = evf;
  ptr->evcontext = context;

  ptr->state = RC_BFS_INIT;
}



void
best_first_cleanup(bf_state_t *ptr) {
  if(ptr->best_seq) {
    pmDelete(ptr->best_seq);
  }
  pmDelete(ptr->next_seq);
  heap_delete(ptr->pq);
}


/* assumes that perm[0..n-1] contains all the filters, and we are
 * checking perm[0..len-1] */
static int
is_valid_partial_perm(const partial_order_t *po, const permutation_t *perm, int n) {
  int i, j;
  //char buf[BUFSIZ];

  //printf("checking %s", pmPrint(perm, buf, BUFSIZ));
  for(i=0; i<pmLength(perm); i++) {
    for(j=i+1; j<n; j++) {
      if(poGet(po, pmElt(perm, i), pmElt(perm, j)) == PO_GT) {
	//printf("\tnot valid\n");
	return 0;
      }
    }
  }
  //printf("\tvalid\n");
  return 1;
}

/* basically, run topo sort on perm[len..n-1] */
static void
make_valid_perm(const partial_order_t *po, permutation_t *perm, int n) {
  int i, j;
  int start = pmLength(perm);
  int v1, v2;

  /* yeah, it's n^2... XXX */
  for(i=start; i < n; i++) {
    v1 = pmElt(perm, i);
    for(j=i+1; j<n; j++) {
      v2 = pmElt(perm, j);
      if(poGet(po, v1, v2) == PO_GT) {
	pmSwap(perm, i, j);
	v1 = v2;
      }
    }
  }
  assert(is_valid_partial_perm(po, perm, n));
}


/* we have the option of exhaustively looking at all the neighboring
 * permutations and picking the best, or we could just greedily pick
 * the first improving one.. */

int
best_first_step(bf_state_t *bf) {
  int err=0;
  const int n = bf->n;
  //int best_score;
  int next_score;
  char buf[BUFSIZ];
  int pos;

  switch(bf->state) {
  case RC_BFS_INIT:

    while(bf->i < n) {
      if(poIsMin(bf->po, bf->i)) {	/* inefficient XXX */
	permutation_t *perm;
	/* setup the permutation so that the unused part contains the
	 * filters not used */
	perm = pmNew(n);
	pmIdentity(perm);
	pmSwap(perm, 0, bf->i);
	pmSetSize(perm, 1);

	err = bf->evfunc(bf->evcontext, perm, &next_score);
	if(err) {
	  pmCopyAll(bf->next_seq, perm); /* try this */
	  make_valid_perm(bf->po, bf->next_seq, n);
	  pmSetSize(bf->next_seq, n);
	  return RC_ERR_NODATA;
	}
	printf("heap insert: %s\n", pmPrint(perm, buf, BUFSIZ));
	heap_insert(bf->pq, next_score, perm);
      }
      bf->i++;
    }
    bf->state = RC_BFS_VISIT;
    break;


  case RC_BFS_VISIT:

    //printf("bfs_visit\n");
    if(!heap_size(bf->pq)) {
      assert(bf->best_seq);
      assert(pmLength(bf->best_seq) == bf->n);
      return RC_ERR_COMPLETE;
    }

    pmCopyAll(bf->best_seq, heap_extract_max(bf->pq));
    printf("bfs visiting: %s\n", pmPrint(bf->best_seq, buf, BUFSIZ));

    /* found full permutation */
    if(pmLength(bf->best_seq) == bf->n) {
      bf->state = RC_BFS_DONE;
      return RC_ERR_COMPLETE;
    }

    bf->state = RC_BFS_EXPAND;
    bf->j = pmSize(bf->best_seq);
    break;


  case RC_BFS_EXPAND:

    //printf("bfs_expand: %s\n", pmPrint(bf->best_seq, buf, BUFSIZ));
    /* get all the children */
    while(bf->j < n) {
      pos = pmSize(bf->best_seq);
      pmCopyAll(bf->next_seq, bf->best_seq);
      pmSwap(bf->next_seq, pos, bf->j);
      pmSetSize(bf->next_seq, pos+1);
      if(is_valid_partial_perm(bf->po, bf->next_seq, n)) {
	int score;
	err = bf->evfunc(bf->evcontext, bf->next_seq, &score);
	if(err) {
	  printf("bfs needs info for %s\n", pmPrint(bf->next_seq, buf, BUFSIZ));
	  make_valid_perm(bf->po, bf->next_seq, n);
	  pmSetSize(bf->next_seq, n);
	  return RC_ERR_NODATA;
	}
	printf("heap inserting: %s (size=%d)\n", 
	       pmPrint(bf->next_seq, buf, BUFSIZ), heap_size(bf->pq));
	//score /= pmLength(bf->next_seq); /* XXX average cost */
	heap_insert(bf->pq, score, pmDup(bf->next_seq));
      }
      bf->j++;
    }
    bf->state = RC_BFS_VISIT;
    break;

  case RC_BFS_DONE:
    /* drain the pq */
    while(heap_size(bf->pq)) {	/* inefficient XXX */
      permutation_t *perm;
      perm = (permutation_t *)heap_extract_max(bf->pq);
      pmDelete(perm);
    }
    bf->state = RC_BFS_INIT;
    break;

  }

  return RC_ERR_NONE;
}

const permutation_t *
best_first_result(bf_state_t *bf) {
  return bf->best_seq;
}

const permutation_t *
best_first_next(bf_state_t *bf) {
  return bf->next_seq;
}

/* ********************************************************************** */