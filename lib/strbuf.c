#include "all.h"
#include "strbuf.h"

int starts_with(const char *str, const char *prefix)
{
	for (; ; str++, prefix++)
		if (!*prefix)
			return 1;
		else if (*str != *prefix)
			return 0;
}

/*
 * Used as the default ->buf value, so that people can always assume
 * buf is non NULL and ->buf is NUL terminated even for a freshly
 * initialized strbuf.
 */
char strbuf_slopbuf[1];

void strbuf_init(struct strbuf *sb, size_t hint)
{
	sb->alloc = sb->len = 0;
	sb->buf = strbuf_slopbuf;
	if (hint)
		strbuf_grow(sb, hint);
}


void strbuf_release(struct strbuf *sb)
{
	if (sb->alloc) {
		free(sb->buf);
		strbuf_init(sb, 0);
	}
}

char *strbuf_detach(struct strbuf *sb, size_t *sz)
{
	char *res;
	strbuf_grow(sb, 0);
	res = sb->buf;
	if (sz)
		*sz = sb->len;
	strbuf_init(sb, 0);
	return res;
}

void strbuf_attach(struct strbuf *sb, void *buf, size_t len, size_t alloc)
{
	strbuf_release(sb);
	sb->buf   = buf;
	sb->len   = len;
	sb->alloc = alloc;
	strbuf_grow(sb, 0);
	sb->buf[sb->len] = '\0';
}

void strbuf_grow(struct strbuf *sb, size_t extra)
{
	int new_buf = !sb->alloc;
	if (new_buf)
		sb->buf = NULL;
	ALLOC_GROW(sb->buf, sb->len + extra + 1, sb->alloc);
	if (new_buf)
		sb->buf[0] = '\0';
}


void strbuf_add(struct strbuf *sb, const void *data, size_t len)
{
	strbuf_grow(sb, len);
	memcpy(sb->buf + sb->len, data, len);
	strbuf_setlen(sb, sb->len + len);
}



int strbuf_getwholeline(struct strbuf *sb, FILE *fp, int term)
{
	int ch;

	if (feof(fp))
		return EOF;

	strbuf_reset(sb);
	while ((ch = fgetc(fp)) != EOF) {
		strbuf_grow(sb, 1);
		sb->buf[sb->len++] = ch;
		if (ch == term)
			break;
	}
	if (ch == EOF && sb->len == 0)
		return EOF;

	sb->buf[sb->len] = '\0';
	return 0;
}

int strbuf_getline(struct strbuf *sb, FILE *fp, int term)
{
	if (strbuf_getwholeline(sb, fp, term))
		return EOF;
	if (sb->buf[sb->len-1] == term)
		strbuf_setlen(sb, sb->len-1);
	return 0;
}

struct strbuf **strbuf_split_buf(const char *str, size_t slen,
				 int terminator, int max)
{
	struct strbuf **ret = NULL;
	size_t nr = 0, alloc = 0;
	struct strbuf *t;

	while (slen) {
		int len = slen;
		if (max <= 0 || nr + 1 < max) {
			const char *end = memchr(str, terminator, slen);
			if (end)
				len = end - str + 1;
		}
		t = malloc(sizeof(struct strbuf));
		strbuf_init(t, len);
		strbuf_add(t, str, len);
		ALLOC_GROW(ret, nr + 2, alloc);
		ret[nr++] = t;
		str += len;
		slen -= len;
	}
	ALLOC_GROW(ret, nr + 1, alloc); /* In case string was empty */
	ret[nr] = NULL;
	return ret;
}


void strbuf_splice(struct strbuf *sb, size_t pos, size_t len,
				   const void *data, size_t dlen)
{
	if (pos > sb->len){
            /*die("`pos' is too far after the end of the buffer");*/
            fprintf(stderr, "`pos' is too far after the end of the buffer");
            exit(-1);
        }
	if (pos + len > sb->len){
            /*die("`pos + len' is too far after the end of the buffer");*/
            fprintf(stderr, "`pos + len' is too far after the end of the buffer");
            exit(-1);
        }

	if (dlen >= len)
		strbuf_grow(sb, dlen - len);
	memmove(sb->buf + pos + dlen,
			sb->buf + pos + len,
			sb->len - pos - len);
	memcpy(sb->buf + pos, data, dlen);
	strbuf_setlen(sb, sb->len + dlen - len);
}

void strbuf_insert(struct strbuf *sb, size_t pos, const void *data, size_t len)
{
	strbuf_splice(sb, pos, 0, data, len);
}

void strbuf_remove(struct strbuf *sb, size_t pos, size_t len)
{
	strbuf_splice(sb, pos, len, NULL, 0);
}

void strbuf_list_free(struct strbuf **sbs)
{
	struct strbuf **s = sbs;

	while (*s) {
		strbuf_release(*s);
		free(*s++);
	}
	free(sbs);
}
