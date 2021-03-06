/*	$Id$ */
/*
 * Copyright (c) 2019 Kristaps Dzonsons <kristaps@bsd.lv>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef EXTERN_H
#define EXTERN_H

/*
 * This is the rsync protocol version that we support.
 */
#define	RSYNC_PROTOCOL	(27)

/*
 * Maximum amount of data sent over the wire at once.
 */
#define MAX_CHUNK	(32 * 1024)

/*
 * The sender and receiver use a two-phase synchronisation process.
 * The first uses two-byte hashes; the second, 16-byte.
 * (The second must hold a full MD4 digest.)
 */
#define	CSUM_LENGTH_PHASE1 (2)
#define	CSUM_LENGTH_PHASE2 (16)

/*
 * Operating mode for a client or a server.
 * Sender means we synchronise local files with those from remote.
 * Receiver is the opposite.
 * This is relative to which host we're running on.
 */
enum	fmode {
	FARGS_LOCAL, /* FIXME: necessary? */
	FARGS_SENDER, 
	FARGS_RECEIVER
};

/*
 * File arguments given on the command line.
 * See struct opts.
 */
struct	fargs {
	char	  *host; /* hostname or NULL if FARGS_LOCAL */
	char	 **sources; /* transfer source */
	size_t	   sourcesz; /* number of sources */
	char	  *sink; /* transfer endpoint */
	enum fmode mode; /* mode of operation */
	int	   remote; /* uses rsync:// or :: for remote */
	char	  *module; /* if rsync://, the module */
};

/*
 * The subset of stat(2) information that we need.
 * (There are some parts we don't use yet.)
 */
struct	flstat {
	mode_t		 mode; /* mode */
	uid_t		 uid; /* user */
	gid_t		 gid; /* group */
	off_t		 size; /* size */
	time_t		 mtime; /* modification */
};

/*
 * A list of files with their statistics.
 */
struct	flist {
	char		*path; /* path relative to root */
	const char	*wpath; /* "working" path for receiver */
	struct flstat	 st; /* file information */
	char		*link; /* symlink target or NULL */
};

/*
 * Options passed into the command line.
 * See struct fargs.
 */
struct	opts {
	int		 sender; /* --sender */
	int		 server; /* --server */
	int		 recursive; /* -r */
	int		 verbose; /* -v */
	int		 dry_run; /* -n */
	int		 preserve_times; /* -t */
	int		 preserve_perms; /* -p */
	int		 preserve_links; /* -l */
	int		 del; /* --delete */
	const char	*rsync_path; /* --rsync-path */
};

/*
 * An individual block description for a file.
 * See struct blkset.
 */
struct	blk {
	off_t		 offs; /* offset in file */
	size_t		 idx; /* block index */
	size_t		 len; /* bytes in block */
	uint32_t	 chksum_short; /* fast checksum */
	unsigned char	 chksum_long[CSUM_LENGTH_PHASE2]; /* slow checksum */
};

/*
 * When transferring file contents, we break the file down into blocks
 * and work with those.
 */
struct	blkset {
	off_t		 size; /* file size */
	size_t		 rem; /* terminal block length if non-zero */
	size_t		 len; /* block length */
	size_t		 csum; /* checksum length */
	struct blk	*blks; /* all blocks */
	size_t		 blksz; /* number of blks */
};

/*
 * Values required during a communication session.
 */
struct	sess {
	const struct opts *opts; /* system options */
	int32_t		   seed; /* checksum seed */
	int32_t		   lver; /* local version */
	int32_t		   rver; /* remote version */
	int		   mplex_reads; /* multiplexing reads? */
	size_t		   mplex_read_remain; /* remaining bytes */
	int		   mplex_writes; /* multiplexing writes? */
};

#define LOG0(_sess, _fmt, ...) \
	rsync_log((_sess), __FILE__, __LINE__, -1, (_fmt), ##__VA_ARGS__)
#define LOG1(_sess, _fmt, ...) \
	rsync_log((_sess), __FILE__, __LINE__, 0, (_fmt), ##__VA_ARGS__)
#define LOG2(_sess, _fmt, ...) \
	rsync_log((_sess), __FILE__, __LINE__, 1, (_fmt), ##__VA_ARGS__)
#define LOG3(_sess, _fmt, ...) \
	rsync_log((_sess), __FILE__, __LINE__, 2, (_fmt), ##__VA_ARGS__)
#define LOG4(_sess, _fmt, ...) \
	rsync_log((_sess), __FILE__, __LINE__, 3, (_fmt), ##__VA_ARGS__)
#define ERRX1(_sess, _fmt, ...) \
	rsync_errx1((_sess), __FILE__, __LINE__, (_fmt), ##__VA_ARGS__)
#define WARNX(_sess, _fmt, ...) \
	rsync_warnx((_sess), __FILE__, __LINE__, (_fmt), ##__VA_ARGS__)
#define WARN(_sess, _fmt, ...) \
	rsync_warn((_sess), 0, __FILE__, __LINE__, (_fmt), ##__VA_ARGS__)
#define WARN1(_sess, _fmt, ...) \
	rsync_warn((_sess), 1, __FILE__, __LINE__, (_fmt), ##__VA_ARGS__)
#define WARN2(_sess, _fmt, ...) \
	rsync_warn((_sess), 2, __FILE__, __LINE__, (_fmt), ##__VA_ARGS__)
#define ERR(_sess, _fmt, ...) \
	rsync_err((_sess), __FILE__, __LINE__, (_fmt), ##__VA_ARGS__)
#define ERRX(_sess, _fmt, ...) \
	rsync_errx((_sess), __FILE__, __LINE__, (_fmt), ##__VA_ARGS__)

__BEGIN_DECLS

void		  rsync_log(struct sess *, 
			const char *, size_t, int, const char *, ...)
			__attribute__((format(printf, 5, 6)));
void		  rsync_warnx1(struct sess *, 
			const char *, size_t, const char *, ...)
			__attribute__((format(printf, 4, 5)));
void		  rsync_warn(struct sess *, int, 
			const char *, size_t, const char *, ...)
			__attribute__((format(printf, 5, 6)));
void		  rsync_warnx(struct sess *, const char *, 
			size_t, const char *, ...)
			__attribute__((format(printf, 4, 5)));
void		  rsync_err(struct sess *, const char *, 
			size_t, const char *, ...)
			__attribute__((format(printf, 4, 5)));
void		  rsync_errx(struct sess *, const char *, 
			size_t, const char *, ...)
			__attribute__((format(printf, 4, 5)));
void		  rsync_errx1(struct sess *, const char *, 
			size_t, const char *, ...)
			__attribute__((format(printf, 4, 5)));

int		  flist_del(struct sess *, int, const struct flist *,
			size_t, const struct flist *, size_t);
int		  flist_gen(struct sess *, size_t, char **, 
			struct flist **, size_t *);
int		  flist_gen_local(struct sess *, const char *, 
			struct flist **, size_t *);
void		  flist_free(struct flist *, size_t);
int		  flist_recv(struct sess *, int, 
			struct flist **, size_t *);
int		  flist_send(struct sess *, int, 
			const struct flist *, size_t);

char		**fargs_cmdline(struct sess *, const struct fargs *);

int		  io_read_buf(struct sess *, int, void *, size_t);
int		  io_read_byte(struct sess *, int, uint8_t *);
int		  io_read_check(struct sess *, int);
int		  io_read_int(struct sess *, int, int32_t *);
int		  io_read_size(struct sess *, int, size_t *);
int		  io_read_long(struct sess *, int, int64_t *);
int		  io_write_buf(struct sess *, int, const void *, size_t);
int		  io_write_byte(struct sess *, int, uint8_t);
int		  io_write_int(struct sess *, int, int32_t);
int		  io_write_line(struct sess *, int, const char *);
int		  io_write_long(struct sess *, int, int64_t);

int		  io_read_blocking(struct sess *, int, void *, size_t);
int		  io_read_nonblocking(struct sess *, int, void *, size_t, size_t *);

void		  rsync_child(const struct opts *, int, const struct fargs *)
			__attribute__((noreturn));
int		  rsync_receiver(struct sess *, int, int, const char *);
int		  rsync_sender(struct sess *, int, int, size_t, char **);
int		  rsync_client(const struct opts *, int, const struct fargs *);
int		  rsync_socket(const struct opts *, const struct fargs *);
int		  rsync_server(const struct opts *, size_t, char *[]);

struct blkset	 *blk_recv(struct sess *, int, const char *);
int		  blk_recv_ack(struct sess *, 
			int, const struct blkset *, int32_t);
int		  blk_match(struct sess *, int, 
			const struct blkset *, const char *);
int		  blk_send(struct sess *, int, 
			const struct blkset *, const char *);
int		  blk_send_ack(struct sess *, int, 
			const struct blkset *, size_t);
int		  blk_merge(struct sess *, int, int, 
			const struct blkset *, int, const char *, 
			const void *, size_t);
void		  blkset_free(struct blkset *);

uint32_t	  hash_fast(const void *, size_t);
void		  hash_slow(const void *, size_t, 
			unsigned char *, const struct sess *);
void		  hash_file(const void *, size_t, 
			unsigned char *, const struct sess *);

int		  mkpath(struct sess *, char *);

char		 *symlink_read(struct sess *, const char *);
char		 *symlinkat_read(struct sess *, int, const char *);

int		  sess_stats_send(struct sess *, int);
int		  sess_stats_recv(struct sess *, int);

#ifdef __linux__
#include <bsd/stdlib.h>
#include <bsd/string.h>
#include "pledge.h"
#include "recallocarray.h"
#endif

__END_DECLS

#endif /*!EXTERN_H*/
