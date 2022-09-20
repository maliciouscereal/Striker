/**
 *--------------------------------------------------------------
 *   Shared utilities for Striker. This should be kept as small
 * as possible to avoid bloating the implant. Utilities that the
 * agent does not use should not be implemented here.
 *                                            Author: Umar Abdul
 *--------------------------------------------------------------
 */

#include "striker_utils.h"

buffer *create_buffer(size_t size){

  buffer *buff = malloc(sizeof(buffer));
  buff->size = size;
  buff->used = 0;
  buff->buffer = malloc(sizeof(char) * ((buff->size == 0) ? 1 : buff->size));
  if (!buff->buffer)
    return NULL;
  memset(buff->buffer, 0, buff->size);
  return buff;
}

void resize_buffer(buffer *buff, size_t new_size){

  if (buff->size == new_size)
    return;
  buff->size = new_size;
  if (buff->used > new_size)
    buff->used = new_size;
  buff->buffer = realloc(buff->buffer, (buff->size == 0 ? 1 : buff->size));
}

size_t append_buffer(buffer *dest, const void *src, size_t len){

  size_t free_bytes = dest->size - dest->used;
  if (free_bytes < len) // Available space not enough, increase the buffer size.
    resize_buffer(dest, dest->size + (len - free_bytes));
  memcpy(dest->buffer + dest->used, src, len);
  dest->used += len;
  return len;
}

size_t buffer_strcpy(buffer *dest, const char *src){

  size_t len = strlen(src);
  resize_buffer(dest, 0);
  return append_buffer(dest, src, len);
}

char *buffer_to_string(buffer *buff){

  char *str = malloc(sizeof(char) * (buff->used + 1));
  if (buff->used == 0)
    return NULL;
  strncpy(str, buff->buffer, buff->used);
  str[buff->used] = '\0';
  return str;
}

void free_buffer(buffer *buff){
  
  buff->size = 0;
  buff->used = 0;
  free(buff->buffer);
  free(buff);
}

long find_offset(FILE *rfo, const void *target, size_t len){

  void *buff = malloc(len);
  int c;
  size_t n = fread(buff, 1, len, rfo);
  if (!n)
    return -1;
  while (1){
    if (!memcmp(buff, target, len)){
      free(buff);
      return ftell(rfo) - len;
    }
    for (int i = 1; i < len; i++)
      *((unsigned char *)buff + (i - 1)) = *((unsigned char *)buff + i);
    c = fgetc(rfo);
    if (c == EOF)
      break;
    *((unsigned char *)buff + (len - 1)) = c;
  }
  free(buff);
  return -1;
}

ssize_t filecpy(FILE *dest, FILE *src, size_t len){

  size_t n;
  int c;
  for (n = 0; n < len; n++){
    if ((c = fgetc(src)) == EOF)
      break;
    fputc(c, dest);
  }
  return n;
}

size_t read_until_null(FILE *rfo){

  size_t n = 0;
  int c;
  while (!feof(rfo)){
    c = fgetc(rfo);
    if (c == EOF || c == 0)
      break;
    n++;
  }
  return n;
}
