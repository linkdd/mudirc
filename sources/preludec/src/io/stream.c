#include <preludec/io/stream.h>


static inline void set_io_status(iostatus *status, iostatus val) {
  if (status != NULL) *status = val;
}


// MARK: - filestream

static usize filestream_size(void *udata, iostatus *status) {
  FILE *f = (FILE *)udata;
  assert(f != NULL);

  long curr = ftell(f);
  if (curr == -1L) {
    set_io_status(status, IO_STATUS_ERROR);
    return 0;
  }

  if (fseek(f, 0, SEEK_END) != 0) {
    set_io_status(status, IO_STATUS_ERROR);
    return 0;
  }

  long size = ftell(f);
  if (size == -1L) {
    set_io_status(status, IO_STATUS_ERROR);
    return 0;
  }

  if (fseek(f, curr, SEEK_SET) != 0) {
    set_io_status(status, IO_STATUS_ERROR);
    return 0;
  }

  set_io_status(status, IO_STATUS_READY);
  return (usize)size;
}


static usize filestream_seek(void *udata, isize offset, iowhence whence, iostatus *status) {
  FILE *f = (FILE *)udata;
  assert(f != NULL);

  if (fseek(f, (long)offset, (int)whence) != 0) {
    set_io_status(status, IO_STATUS_ERROR);
    return 0;
  }

  long pos = ftell(f);
  if (pos == -1L) {
    set_io_status(status, IO_STATUS_ERROR);
    return 0;
  }

  set_io_status(status, IO_STATUS_READY);
  return (usize)pos;
}


static usize filestream_read(void *udata, span buf, iostatus *status) {
  FILE *f = (FILE *)udata;
  assert(f != NULL);
  assert(buf.data != NULL && buf.size > 0);

  usize nread = fread(buf.data, 1, buf.size, f);
  if (nread < buf.size) {
    if (feof(f)) {
      set_io_status(status, IO_STATUS_EOF);
    }
    else if (ferror(f)) {
      set_io_status(status, IO_STATUS_ERROR);
    }
    else {
      set_io_status(status, IO_STATUS_NOTREADY);
    }
  }
  else {
    set_io_status(status, IO_STATUS_READY);
  }

  return (usize)nread;
}


static usize filestream_write(void *udata, const_span buf, iostatus *status) {
  FILE *f = (FILE *)udata;
  assert(f != NULL);
  assert(buf.data != NULL && buf.size > 0);

  usize nwritten = fwrite(buf.data, 1, buf.size, f);
  if (nwritten < buf.size) {
    if (ferror(f)) {
      set_io_status(status, IO_STATUS_ERROR);
    }
    else {
      set_io_status(status, IO_STATUS_NOTREADY);
    }
  }
  else {
    set_io_status(status, IO_STATUS_READY);
  }

  return (usize)nwritten;
}


static bool filestream_flush(void *udata, iostatus *status) {
  FILE *f = (FILE *)udata;
  assert(f != NULL);

  if (fflush(f) != 0) {
    set_io_status(status, IO_STATUS_ERROR);
    return false;
  }

  set_io_status(status, IO_STATUS_READY);
  return true;
}


static bool filestream_close(void *udata, iostatus *status) {
  FILE *f = (FILE *)udata;
  assert(f != NULL);

  if (fclose(f) != 0) {
    set_io_status(status, IO_STATUS_ERROR);
    return false;
  }

  set_io_status(status, IO_STATUS_READY);
  return true;
}


stream make_filestream(FILE *file) {
  assert(file != NULL);

  return (stream){
    .size  = filestream_size,
    .seek  = filestream_seek,
    .read  = filestream_read,
    .write = filestream_write,
    .flush = filestream_flush,
    .close = filestream_close,
    .udata = (void *)file,
  };
}


// MARK: - romemstream

typedef struct romemstream_ctx romemstream_ctx;
struct romemstream_ctx {
  allocator  a;
  const_span buffer;
  usize      cursor;
};


static usize romemstream_size(void *udata, iostatus *status) {
  romemstream_ctx *ctx = (romemstream_ctx *)udata;
  assert(ctx != NULL);

  set_io_status(status, IO_STATUS_READY);
  return ctx->buffer.size;
}


static usize romemstream_seek(void *udata, isize offset, iowhence whence, iostatus *status) {
  romemstream_ctx *ctx = (romemstream_ctx *)udata;
  assert(ctx != NULL);

  switch (whence) {
    case IO_SEEK_SET:
      if (offset < 0) {
        ctx->cursor = 0;
      }
      else if (offset > 0 && (usize)offset > ctx->buffer.size) {
        ctx->cursor = ctx->buffer.size;
      }
      else {
        ctx->cursor = (usize)offset;
      }
      break;

    case IO_SEEK_CUR:
      if (offset < 0 && (usize)(-offset) > ctx->cursor) {
        ctx->cursor = 0;
      }
      else if (offset > 0 && (usize)offset > (ctx->buffer.size - ctx->cursor)) {
        ctx->cursor = ctx->buffer.size;
      }
      else {
        ctx->cursor += offset;
      }
      break;

    case IO_SEEK_END:
      if (offset < 0 && (usize)(-offset) > ctx->buffer.size) {
        ctx->cursor = 0;
      }
      else if (offset > 0) {
        ctx->cursor = ctx->buffer.size;
      }
      else {
        ctx->cursor -= offset;
      }
      break;

    default:
      assert(false && "Unreachable");
      set_io_status(status, IO_STATUS_ERROR);
      return ctx->cursor;
  }

  set_io_status(status, IO_STATUS_READY);
  return ctx->cursor;
}


static usize romemstream_read(void *udata, span buf, iostatus *status) {
  romemstream_ctx *ctx = (romemstream_ctx *)udata;
  assert(ctx != NULL);
  assert(buf.data != NULL && buf.size > 0);

  usize remaining = ctx->buffer.size - ctx->cursor;
  usize to_read   = buf.size < remaining ? buf.size : remaining;

  memcpy(buf.data, (const u8 *)ctx->buffer.data + ctx->cursor, to_read);
  ctx->cursor += to_read;

  if (to_read < buf.size) {
    set_io_status(status, IO_STATUS_EOF);
  }
  else {
    set_io_status(status, IO_STATUS_READY);
  }

  return to_read;
}


static usize romemstream_write(void *udata, const_span buf, iostatus *status) {
  (void)udata;
  (void)buf;

  set_io_status(status, IO_STATUS_ERROR);
  return 0;
}


static bool romemstream_flush(void *udata, iostatus *status) {
  (void)udata;

  set_io_status(status, IO_STATUS_READY);
  return true;
}


static bool romemstream_close(void *udata, iostatus *status) {
  romemstream_ctx *ctx = (romemstream_ctx *)udata;
  assert(ctx != NULL);

  deallocate(ctx->a, ctx, sizeof(romemstream_ctx));

  set_io_status(status, IO_STATUS_READY);
  return true;
}


stream make_romemstream(allocator a, const_span buffer) {
  assert(buffer.data != NULL && buffer.size > 0);

  romemstream_ctx *ctx = allocate(a, sizeof(romemstream_ctx));

  ctx->a      = a;
  ctx->buffer = buffer;
  ctx->cursor = 0;

  return (stream){
    .size  = romemstream_size,
    .seek  = romemstream_seek,
    .read  = romemstream_read,
    .write = romemstream_write,
    .flush = romemstream_flush,
    .close = romemstream_close,
    .udata = (void *)ctx,
  };
}


// MARK: - rwmemstream

typedef struct rwmemstream_ctx rwmemstream_ctx;
struct rwmemstream_ctx {
  allocator a;
  span      buffer;
  usize     cursor;
};


static usize rwmemstream_size(void *udata, iostatus *status) {
  rwmemstream_ctx *ctx = (rwmemstream_ctx *)udata;
  assert(ctx != NULL);

  set_io_status(status, IO_STATUS_READY);
  return ctx->buffer.size;
}


static usize rwmemstream_seek(void *udata, isize offset, iowhence whence, iostatus *status) {
  rwmemstream_ctx *ctx = (rwmemstream_ctx *)udata;
  assert(ctx != NULL);

  switch (whence) {
    case IO_SEEK_SET:
      if (offset < 0) {
        ctx->cursor = 0;
      }
      else if (offset > 0 && (usize)offset > ctx->buffer.size) {
        ctx->cursor = ctx->buffer.size;
      }
      else {
        ctx->cursor = (usize)offset;
      }
      break;

    case IO_SEEK_CUR:
      if (offset < 0 && (usize)(-offset) > ctx->cursor) {
        ctx->cursor = 0;
      }
      else if (offset > 0 && (usize)offset > (ctx->buffer.size - ctx->cursor)) {
        ctx->cursor = ctx->buffer.size;
      }
      else {
        ctx->cursor += offset;
      }
      break;

    case IO_SEEK_END:
      if (offset < 0 && (usize)(-offset) > ctx->buffer.size) {
        ctx->cursor = 0;
      }
      else if (offset > 0) {
        ctx->cursor = ctx->buffer.size;
      }
      else {
        ctx->cursor -= offset;
      }
      break;

    default:
      assert(false && "Unreachable");
      set_io_status(status, IO_STATUS_ERROR);
      return ctx->cursor;
  }

  set_io_status(status, IO_STATUS_READY);
  return ctx->cursor;
}


static usize rwmemstream_read(void *udata, span buf, iostatus *status) {
  rwmemstream_ctx *ctx = (rwmemstream_ctx *)udata;
  assert(ctx != NULL);
  assert(buf.data != NULL && buf.size > 0);

  usize remaining = ctx->buffer.size - ctx->cursor;
  usize to_read   = buf.size < remaining ? buf.size : remaining;

  memcpy(buf.data, (const u8 *)ctx->buffer.data + ctx->cursor, to_read);
  ctx->cursor += to_read;

  if (to_read < buf.size) {
    set_io_status(status, IO_STATUS_EOF);
  }
  else {
    set_io_status(status, IO_STATUS_READY);
  }

  return to_read;
}


static usize rwmemstream_write(void *udata, const_span buf, iostatus *status) {
  rwmemstream_ctx *ctx = (rwmemstream_ctx *)udata;
  assert(ctx != NULL);
  assert(buf.data != NULL && buf.size > 0);

  usize remaining = ctx->buffer.size - ctx->cursor;
  usize to_write  = buf.size < remaining ? buf.size : remaining;

  memcpy((u8 *)ctx->buffer.data + ctx->cursor, buf.data, to_write);
  ctx->cursor += to_write;

  if (to_write < buf.size) {
    set_io_status(status, IO_STATUS_EOF);
  }
  else {
    set_io_status(status, IO_STATUS_READY);
  }

  return to_write;
}


static bool rwmemstream_flush(void *udata, iostatus *status) {
  (void)udata;

  set_io_status(status, IO_STATUS_READY);
  return true;
}


static bool rwmemstream_close(void *udata, iostatus *status) {
  rwmemstream_ctx *ctx = (rwmemstream_ctx *)udata;
  assert(ctx != NULL);

  deallocate(ctx->a, ctx, sizeof(rwmemstream_ctx));

  set_io_status(status, IO_STATUS_READY);
  return true;
}


stream make_rwmemstream(allocator a, span buffer) {
  assert(buffer.data != NULL && buffer.size > 0);

  rwmemstream_ctx *ctx = allocate(a, sizeof(rwmemstream_ctx));

  ctx->a      = a;
  ctx->buffer = buffer;
  ctx->cursor = 0;

  return (stream){
    .size  = rwmemstream_size,
    .seek  = rwmemstream_seek,
    .read  = rwmemstream_read,
    .write = rwmemstream_write,
    .flush = rwmemstream_flush,
    .close = rwmemstream_close,
    .udata = (void *)ctx,
  };
}


// MARK: - API

usize stream_size(stream *s, iostatus *status) {
  assert(s != NULL);
  assert(s->size != NULL);

  return s->size(s->udata, status);
}


usize stream_seek(stream *s, isize offset, iowhence whence, iostatus *status) {
  assert(s != NULL);
  assert(s->seek != NULL);

  return s->seek(s->udata, offset, whence, status);
}


usize stream_read(stream *s, span buf, iostatus *status) {
  assert(s != NULL);
  assert(s->read != NULL);

  return s->read(s->udata, buf, status);
}


usize stream_write(stream *s, const_span buf, iostatus *status) {
  assert(s != NULL);
  assert(s->write != NULL);

  return s->write(s->udata, buf, status);
}


bool stream_flush(stream *s, iostatus *status) {
  assert(s != NULL);
  assert(s->flush != NULL);

  return s->flush(s->udata, status);
}


bool stream_close(stream *s, iostatus *status) {
  assert(s != NULL);
  assert(s->close != NULL);

  bool res = s->close(s->udata, status);
  if (res) {
    memset(s, 0, sizeof(stream));
  }

  return res;
}
