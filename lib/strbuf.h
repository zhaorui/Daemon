#ifndef STRBUF_H
#define STRBUF_H

/*
 * Define below is from the cache.h
 * --------------------------------
 * */

#define REALLOC_ARRAY(x, alloc) (x) = realloc((x), (alloc) * sizeof(*(x)))
#define alloc_nr(x) (((x)+16)*3/2)

/*
 * Realloc the buffer pointed at by variable 'x' so that it can hold
 * at least 'nr' entries; the number of entries currently allocated
 * is 'alloc', using the standard growing factor alloc_nr() macro.
 *
 * DO NOT USE any expression with side-effect for 'x', 'nr', or 'alloc'.
 */
#define ALLOC_GROW(x, nr, alloc) \
	do { \
		if ((nr) > alloc) { \
			if (alloc_nr(alloc) < (nr)) \
				alloc = (nr); \
			else \
				alloc = alloc_nr(alloc); \
			REALLOC_ARRAY(x, alloc); \
		} \
	} while (0)


/**
 * Data Structures
 * ---------------
 */

/**
 * This is the string buffer structure. The `len` member can be used to
 * determine the current length of the string, and `buf` member provides
 * access to the string itself.
 */
struct strbuf {
	size_t alloc;
	size_t len;
	char *buf;
};

extern char strbuf_slopbuf[];
#define STRBUF_INIT  { 0, 0, strbuf_slopbuf }


/**
 * Life Cycle Functions
 * --------------------
 */

/**
 * Initialize the structure. The second parameter can be zero or a bigger
 * number to allocate memory, in case you want to prevent further reallocs.
 */
extern void strbuf_init(struct strbuf *, size_t);

/**
 * Release a string buffer and the memory it used. You should not use the
 * string buffer after using this function, unless you initialize it again.
 */
extern void strbuf_release(struct strbuf *);

/**
 * Detach the string from the strbuf and returns it; you now own the
 * storage the string occupies and it is your responsibility from then on
 * to release it with `free(3)` when you are done with it.
 */
extern char *strbuf_detach(struct strbuf *, size_t *);

/**
 * Attach a string to a buffer. You should specify the string to attach,
 * the current length of the string and the amount of allocated memory.
 * The amount must be larger than the string length, because the string you
 * pass is supposed to be a NUL-terminated string.  This string _must_ be
 * malloc()ed, and after attaching, the pointer cannot be relied upon
 * anymore, and neither be free()d directly.
 */
extern void strbuf_attach(struct strbuf *, void *, size_t, size_t);

/**
 * Swap the contents of two string buffers.
 */
static inline void strbuf_swap(struct strbuf *a, struct strbuf *b)
{
	struct strbuf tmp = *a;
	*a = *b;
	*b = tmp;
}

/**
 * Functions related to the size of the buffer
 * -------------------------------------------
 */

/**
 * Determine the amount of allocated but unused memory.
 */
static inline size_t strbuf_avail(const struct strbuf *sb)
{
	return sb->alloc ? sb->alloc - sb->len - 1 : 0;
}

/**
 * Ensure that at least this amount of unused memory is available after
 * `len`. This is used when you know a typical size for what you will add
 * and want to avoid repetitive automatic resizing of the underlying buffer.
 * This is never a needed operation, but can be critical for performance in
 * some cases.
 */
extern void strbuf_grow(struct strbuf *, size_t);

/**
 * Set the length of the buffer to a given value. This function does *not*
 * allocate new memory, so you should not perform a `strbuf_setlen()` to a
 * length that is larger than `len + strbuf_avail()`. `strbuf_setlen()` is
 * just meant as a 'please fix invariants from this strbuf I just messed
 * with'.
 */
static inline void strbuf_setlen(struct strbuf *sb, size_t len)
{
	if (len > (sb->alloc ? sb->alloc - 1 : 0))
        {
            /*die("BUG: strbuf_setlen() beyond buffer");*/
            fprintf(stderr, "BUG: strbuf_setlen beyond buffer");
            exit(-1);
        }
	sb->len = len;
	sb->buf[len] = '\0';
}

/**
 * Empty the buffer by setting the size of it to zero.
 */
#define strbuf_reset(sb)  strbuf_setlen(sb, 0)

/**
 * Adding data to the buffer
 * -------------------------
 *
 * NOTE: All of the functions in this section will grow the buffer as
 * necessary.  If they fail for some reason other than memory shortage and the
 * buffer hadn't been allocated before (i.e. the `struct strbuf` was set to
 * `STRBUF_INIT`), then they will free() it.
 */

/**
 * Add a single character to the buffer.
 */
static inline void strbuf_addch(struct strbuf *sb, int c)
{
	strbuf_grow(sb, 1);
	sb->buf[sb->len++] = c;
	sb->buf[sb->len] = '\0';
}

/**
 * Add data of given length to the buffer.
 */
extern void strbuf_add(struct strbuf *, const void *, size_t);


/**
 * Like `strbuf_getline`, but keeps the trailing terminator (if
 * any) in the buffer.
 */
extern int strbuf_getwholeline(struct strbuf *, FILE *, int);

#endif /*STRBUF_H*/
