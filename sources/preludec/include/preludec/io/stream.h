#pragma once

#include <preludec/defs.h>
#include <preludec/mem/alloc.h>
#include <preludec/mem/span.h>


typedef enum {
  IO_SEEK_SET = SEEK_SET,
  IO_SEEK_CUR = SEEK_CUR,
  IO_SEEK_END = SEEK_END,
} iowhence;

typedef enum {
  IO_STATUS_READY,
  IO_STATUS_ERROR,
  IO_STATUS_EOF,
  IO_STATUS_NOTREADY,
} iostatus;

typedef struct stream stream;
struct stream {
  usize (*size )(void *udata,                                iostatus *status);
  usize (*seek )(void *udata, isize offset, iowhence whence, iostatus *status);
  usize (*read )(void *udata, span       buf,                iostatus *status);
  usize (*write)(void *udata, const_span buf,                iostatus *status);
  bool  (*flush)(void *udata,                                iostatus *status);
  bool  (*close)(void *udata,                                iostatus *status);

  void *udata;
};


stream make_filestream (FILE *file);
stream make_romemstream(allocator a, const_span buffer);
stream make_rwmemstream(allocator a, span       buffer);

usize stream_size (stream *s, iostatus *status);
usize stream_seek (stream *s, isize offset, iowhence whence, iostatus *status);
usize stream_read (stream *s, span       buf,                iostatus *status);
usize stream_write(stream *s, const_span buf,                iostatus *status);
bool  stream_flush(stream *s,                                iostatus *status);
bool  stream_close(stream *s,                                iostatus *status);
