
#include <ruby.h>
#include <ruby/encoding.h>

#include <libzstd/common/zstd.h>
#include "varint.h"


static VALUE compress(VALUE self, VALUE input)
{
  Check_Type(input, T_STRING);
  const char* input_data = RSTRING_PTR(input);
  size_t input_size = RSTRING_LEN(input);

  size_t varint_size = varint_encoded_size(input_size);
  size_t max_compressed_size = ZSTD_compressBound(input_size);

  VALUE output = rb_str_new(NULL, varint_size + max_compressed_size);
  char* output_data = RSTRING_PTR(output);

  varint_encode(input_size, output_data);

  size_t compressed_size = ZSTD_compress((void*)(output_data + varint_size), max_compressed_size,
                                         (const void*)input_data, input_size, 1);

  if (ZSTD_isError(compressed_size)) {
    size_t zero_varint_size = varint_encode(0, output_data);
    memcpy(output_data + varint_size, input_data, input_size);
    rb_str_resize(output, zero_varint_size + input_size);
  } else {
    rb_str_resize(output, varint_size + compressed_size);
  }

  return output;
}

static VALUE decompress(VALUE self, VALUE input)
{
  Check_Type(input, T_STRING);
  const char* input_data = RSTRING_PTR(input);
  size_t input_size = RSTRING_LEN(input);

  uint64_t uncompressed_size;
  size_t varint_size = varint_decode(&uncompressed_size, input_data);

  if (uncompressed_size == 0) {
    return rb_str_substr(input, varint_size, input_size - varint_size);
  }

  VALUE output = rb_str_new(NULL, uncompressed_size);
  char* output_data = RSTRING_PTR(output);

  size_t decompress_size = ZSTD_decompress((void*)output_data, uncompressed_size,
                                           (const void*)(input_data + varint_size), input_size - varint_size);

  if (ZSTD_isError(decompress_size)) {
      output = Qnil;
  }

  return output;
}

void Init_zstd()
{
  VALUE mZstd = rb_const_get(rb_cObject, rb_intern("Zstd"));

  rb_define_module_function(mZstd, "compress", compress, 1);
  rb_define_module_function(mZstd, "decompress", decompress, 1);
}
