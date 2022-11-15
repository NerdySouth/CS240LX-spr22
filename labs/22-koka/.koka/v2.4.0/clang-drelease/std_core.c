// Koka generated module: "std/core", koka version: 2.4.0, platform: 64-bit
#include "std_core.h"
/*---------------------------------------------------------------------------
  Copyright 2020-2021, Microsoft Research, Daan Leijen.

  This is free software; you can redistribute it and/or modify it under the
  terms of the Apache License, Version 2.0. A copy of the License can be
  found in the LICENSE file at the root of this distribution.
---------------------------------------------------------------------------*/

kk_std_core__list kk_vector_to_list(kk_vector_t v, kk_std_core__list tail, kk_context_t* ctx) {
  // todo: avoid boxed_dup if v is unique
  kk_ssize_t n;
  kk_box_t* p = kk_vector_buf_borrow(v, &n);
  if (n <= 0) {
    kk_vector_drop(v,ctx);
    return tail;
  }
  kk_std_core__list nil  = kk_std_core__new_Nil(ctx);
  struct kk_std_core_Cons* cons = NULL;
  kk_std_core__list list = kk_std_core__new_Nil(ctx);
  for( kk_ssize_t i = 0; i < n; i++ ) {
    kk_std_core__list hd = kk_std_core__new_Cons(kk_reuse_null,kk_box_dup(p[i]), nil, ctx);
    if (cons==NULL) {
      list = hd;
    }
    else {
      cons->tail = hd;
    }
    cons = kk_std_core__as_Cons(hd);
  }
  if (cons == NULL) { list = tail; } 
               else { cons->tail = tail; }
  kk_vector_drop(v,ctx);
  return list;
}

kk_vector_t kk_list_to_vector(kk_std_core__list xs, kk_context_t* ctx) {
  // todo: avoid boxed_dup if xs is unique
  // find the length
  kk_ssize_t len = 0;
  kk_std_core__list ys = xs;
  while (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* cons = kk_std_core__as_Cons(ys);
    len++;
    ys = cons->tail;
  }
  // alloc the vector and copy
  kk_box_t* p;
  kk_vector_t v = kk_vector_alloc_uninit(len, &p, ctx);  
  ys = xs;
  for( kk_ssize_t i = 0; i < len; i++) {
    struct kk_std_core_Cons* cons = kk_std_core__as_Cons(ys);
    ys = cons->tail;
    p[i] = kk_box_dup(cons->head);
  }
  kk_std_core__list_drop(xs,ctx);  // todo: drop while visiting?
  return v;
}

kk_vector_t kk_vector_init( kk_ssize_t n, kk_function_t init, kk_context_t* ctx) {
  kk_box_t* p;
  kk_vector_t v = kk_vector_alloc_uninit(n, &p, ctx);  
  for(kk_ssize_t i = 0; i < n; i++) {
    kk_function_dup(init);
    p[i] = kk_function_call(kk_box_t,(kk_function_t,kk_ssize_t,kk_context_t*),init,(init,i,ctx));
  }
  kk_function_drop(init,ctx);
  return v;
}

kk_box_t kk_main_console( kk_function_t action, kk_context_t* ctx ) {
  return kk_function_call(kk_box_t,(kk_function_t,kk_unit_t,kk_context_t*),action,(action,kk_Unit,ctx));
}


kk_std_core__list kk_string_to_list(kk_string_t s, kk_context_t* ctx) {
  kk_ssize_t len;
  const uint8_t* p = kk_string_buf_borrow(s,&len);
  const uint8_t* const end = p + len;
  kk_std_core__list nil  = kk_std_core__new_Nil(ctx);
  kk_std_core__list list = nil;
  struct kk_std_core_Cons* tl = NULL;
  kk_ssize_t count;
  while( p < end ) {
    kk_char_t c = kk_utf8_read(p,&count);
    p += count;
    kk_std_core__list cons = kk_std_core__new_Cons(kk_reuse_null,kk_char_box(c,ctx), nil, ctx);
    if (tl!=NULL) {
      tl->tail = cons;
    }
    else {
      list = cons;
    }
    tl = kk_std_core__as_Cons(cons);
  }
  kk_string_drop(s,ctx);
  return list;
}

kk_string_t kk_string_from_list(kk_std_core__list cs, kk_context_t* ctx) {
  // TODO: optimize for short strings to write directly into a local buffer?
  // find total UTF8 length
  kk_ssize_t len = 0;
  kk_std_core__list xs = cs;
  while (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* cons = kk_std_core__as_Cons(xs);
    len += kk_utf8_len(kk_char_unbox(cons->head,ctx));
    xs = cons->tail;
  }
  // allocate and copy the characters
  uint8_t* p;
  kk_string_t s = kk_unsafe_string_alloc_buf(len,&p,ctx);  // must be initialized
  xs = cs;
  while (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* cons = kk_std_core__as_Cons(xs);
    kk_ssize_t count;
    kk_utf8_write( kk_char_unbox(cons->head,ctx), p, &count );
    p += count;
    xs = cons->tail;
  }
  kk_assert_internal(*p == 0 && (p - kk_string_buf_borrow(s,NULL)) == len);
  kk_std_core__list_drop(cs,ctx);  // todo: drop while visiting?
  return s;
}

static inline void kk_sslice_start_end_borrowx( kk_std_core__sslice sslice, const uint8_t** start, const uint8_t** end, const uint8_t** sstart, const uint8_t** send) {
  kk_ssize_t slen;
  const uint8_t* s = kk_string_buf_borrow(sslice.str,&slen);
  *start = s + sslice.start;
  *end = s + sslice.start + sslice.len;
  if (sstart != NULL) *sstart = s;
  if (send != NULL) *send = s + slen;
  kk_assert_internal(*start >= s && *start <= *end);
  kk_assert_internal(*end >= *start && *end <= s + slen);
}

static inline void kk_sslice_start_end_borrow( kk_std_core__sslice sslice, const uint8_t** start, const uint8_t** end) {
  kk_sslice_start_end_borrowx(sslice,start,end,NULL,NULL);
}

kk_integer_t kk_slice_count( kk_std_core__sslice sslice, kk_context_t* ctx ) {
  // TODO: optimize this by extending kk_string_count
  const uint8_t* start;
  const uint8_t* end;
  kk_sslice_start_end_borrow(sslice, &start, &end);
  kk_ssize_t count = 0;
  while( start < end && *start != 0 ) {
    const uint8_t* next = kk_utf8_next(start);
    count++;
    start = next;
  }
  kk_std_core__sslice_drop(sslice,ctx);
  return kk_integer_from_ssize_t(count,ctx);
}

kk_string_t kk_slice_to_string( kk_std_core__sslice  sslice, kk_context_t* ctx ) {
  const uint8_t* start;
  const uint8_t* end;
  kk_sslice_start_end_borrow(sslice, &start, &end);
  // is it the full string?
  if (sslice.start == 0 && sslice.len == kk_string_len_borrow(sslice.str)) {
    // TODO: drop sslice and dup sslice.str?
    return sslice.str;
  }
  else {
    // if not, we copy len bytes
    kk_string_t s = kk_string_alloc_dupn_valid_utf8(sslice.len, start, ctx);
    kk_std_core__sslice_drop(sslice,ctx);
    return s;
  }
}

kk_std_core__sslice kk_slice_first( kk_string_t str, kk_context_t* ctx ) {
  kk_ssize_t slen;
  const uint8_t* s = kk_string_buf_borrow(str,&slen);
  const uint8_t* next = (slen > 0 ? kk_utf8_next(s) : s);
  return kk_std_core__new_Sslice(str, 0, (next - s), ctx);
}

kk_std_core__sslice kk_slice_last( kk_string_t str, kk_context_t* ctx ) {
  kk_ssize_t slen;
  const uint8_t* s = kk_string_buf_borrow(str,&slen);
  const uint8_t* end = s + slen;
  const uint8_t* prev = (s==end ? s : kk_utf8_prev(end));
  return kk_std_core__new_Sslice(str, (prev - s), (end - prev), ctx);
}

kk_std_core__sslice kk_slice_between( struct kk_std_core_Sslice slice1, struct kk_std_core_Sslice slice2, kk_context_t* ctx ) {
  const uint8_t* s1 = kk_string_buf_borrow( slice1.str, NULL );
  const uint8_t* s2 = kk_string_buf_borrow( slice2.str, NULL );
  if (s1 != s2) {
    kk_info_message("between: not equal slices: %p vs. %p\n", s1, s2);
    return kk_std_core__new_Sslice(kk_string_empty(), 0, -1, ctx); // invalid slice
  }
  kk_ssize_t start = (slice1.start <= slice2.start ? slice1.start : slice2.start);
  kk_ssize_t len   = (slice1.start <= slice2.start ? slice2.start - slice1.start : slice1.start - slice2.start);
  return kk_std_core__new_Sslice(slice1.str, start, len, ctx);
}

kk_std_core_types__maybe kk_slice_next( struct kk_std_core_Sslice slice, kk_context_t* ctx ) {
  if (slice.len <= 0) {
    kk_std_core__sslice_drop(slice,ctx);
    return kk_std_core_types__new_Nothing(ctx);
  }
  const uint8_t* start;
  const uint8_t* end;
  kk_sslice_start_end_borrow(slice, &start, &end);
  kk_ssize_t clen;
  const kk_char_t c = kk_utf8_read(start,&clen);
  kk_assert_internal(clen > 0 && clen <= slice.len);
  if (clen > slice.len) clen = slice.len;
  // TODO: specialize type to avoid boxing
  kk_std_core__sslice snext = kk_std_core__new_Sslice(slice.str, slice.start + clen, slice.len - clen, ctx);
  kk_std_core_types__tuple2_ res = kk_std_core_types__new_dash__lp__comma__rp_( kk_char_box(c,ctx), kk_std_core__sslice_box(snext,ctx), ctx);
  return kk_std_core_types__new_Just( kk_std_core_types__tuple2__box(res,ctx), ctx );
}

/* Borrow count */
struct kk_std_core_Sslice kk_slice_extend_borrow( struct kk_std_core_Sslice slice, kk_integer_t count, kk_context_t* ctx ) {
  kk_ssize_t cnt = kk_integer_clamp_borrow(count,ctx);
  if (cnt==0 || (slice.len <= 0 && cnt<0)) return slice;
  const uint8_t* s0;
  const uint8_t* s1;
  kk_sslice_start_end_borrow(slice,&s0,&s1);
  const uint8_t* t  = s1;
  if (cnt >= 0) {
    do {
      t = kk_utf8_next(t);
      cnt--;
    } while (cnt > 0 && *t != 0);
  }
  else {  // cnt < 0
    const uint8_t* sstart = s0 - slice.start;
    do {
      t = kk_utf8_prev(t);
      cnt++;
    } while (cnt < 0 && t > sstart);
  }
  if (t == s1) return slice;  // length is unchanged
  return kk_std_core__new_Sslice(slice.str, slice.start, (t < s0 ? 0 : (t - s0)), ctx);
}

/* Borrow count */
struct kk_std_core_Sslice kk_slice_advance_borrow( struct kk_std_core_Sslice slice, kk_integer_t count, kk_context_t* ctx ) {
  const kk_ssize_t cnt0 = kk_integer_clamp_borrow(count,ctx);
  kk_ssize_t cnt = cnt0;
  if (cnt==0 || (slice.start == 0 && cnt<0)) return slice;
  const uint8_t* sstart;
  const uint8_t* s0;
  const uint8_t* s1;
  const uint8_t* send;
  kk_sslice_start_end_borrowx(slice,&s0,&s1,&sstart,&send);
  // advance the start
  const uint8_t* t0  = s0;
  if (cnt >= 0) {
    do {
      t0 = kk_utf8_next(t0);
      cnt--;
    } while (cnt > 0 && t0 < send);
  }
  else {  // cnt < 0
    do {
      t0 = kk_utf8_prev(t0);
      cnt++;
    } while (cnt < 0 && t0 > sstart);
  }
  if (t0 == s0) return slice;  // start is unchanged
  // "t0" points to the new start, now advance the end by the same amount of codepoints
  const uint8_t* t1 = s1;
  cnt = cnt0;
  if (cnt >= 0) {
    do {
      t1 = kk_utf8_next(t1);
      cnt--;
    } while (cnt > 0 && t1 < send);
  }
  else {  // cnt < 0
    do {
      t1 = kk_utf8_prev(t1);
      cnt++;
    } while (cnt < 0 && t1 > sstart);
  }
  // t1 points to the new end
  kk_assert_internal(t1 >= t0);
  return kk_std_core__new_Sslice(slice.str, (t0 - sstart), (t1 - t0), ctx);
}

/* Borrow iupto */
struct kk_std_core_Sslice kk_slice_common_prefix_borrow( kk_string_t str1, kk_string_t str2, kk_integer_t iupto, kk_context_t* ctx ) {
  const uint8_t* s1 = kk_string_buf_borrow(str1,NULL);
  const uint8_t* s2 = kk_string_buf_borrow(str2,NULL);
  kk_ssize_t upto = kk_integer_clamp_ssize_t_borrow(iupto,ctx);
  kk_ssize_t count;
  for(count = 0; count < upto && *s1 != 0 && *s2 != 0; count++, s1++, s2++ ) {
    if (*s1 != *s2) break;
  }
  kk_string_drop(str2,ctx);
  return kk_std_core__new_Sslice(str1, 0, count, ctx);
}


kk_std_core__error kk_error_ok( kk_box_t result, kk_context_t* ctx ) {
  return kk_std_core__new_Ok( result, ctx );
}

kk_std_core__error kk_error_from_errno( int err, kk_context_t* ctx ) {  
  kk_string_t msg;
  #if defined(__GLIBC__) && !defined(WIN32) && !defined(__APPLE__) && !defined(__FreeBSD__)
    // GNU version of strerror_r
    char buf[256];
    char* serr = strerror_r(err, buf, 255); buf[255] = 0;
    msg = kk_string_alloc_from_qutf8( serr, ctx );
  #elif (/* _POSIX_C_SOURCE >= 200112L ||*/ !defined(WIN32) && (_XOPEN_SOURCE >= 600 || defined(__APPLE__) || defined(__FreeBSD__) || defined(__MUSL__)))
    // XSI version of strerror_r
    char buf[256];
    strerror_r(err, buf, 255); buf[255] = 0;
    msg = kk_string_alloc_from_qutf8( buf, ctx );
  #elif defined(_MSC_VER) || (__STDC_VERSION__ >= 201112L || __cplusplus >= 201103L)
    // MSVC, or C/C++ 11
    char buf[256];
    strerror_s(buf, 255, err); buf[255] = 0;
    msg = kk_string_alloc_from_qutf8( buf, ctx );
  #else
    // Old style
    msg = kk_string_alloc_from_qutf8( strerror(err), ctx );
  #endif
  return kk_std_core__new_Error( kk_std_core__new_Exception( msg, kk_std_core__new_ExnSystem(kk_reuse_null, kk_integer_from_int(err,ctx), ctx), ctx), ctx );  
}


kk_unit_t kk_assert_fail( kk_string_t msg, kk_context_t* ctx ) {
  kk_fatal_error(EINVAL, "assertion failed: %s\n", kk_string_cbuf_borrow(msg,NULL));
  kk_string_drop(msg,ctx);
  return kk_Unit;
}
kk_define_string_literal(, kk_std_core__tag_ExnError, 17, "std/core/ExnError")
kk_define_string_literal(, kk_std_core__tag_ExnAssert, 18, "std/core/ExnAssert")
kk_define_string_literal(, kk_std_core__tag_ExnTodo, 16, "std/core/ExnTodo")
kk_define_string_literal(, kk_std_core__tag_ExnRange, 17, "std/core/ExnRange")
kk_define_string_literal(, kk_std_core__tag_ExnPattern, 19, "std/core/ExnPattern")
kk_define_string_literal(, kk_std_core__tag_ExnSystem, 18, "std/core/ExnSystem")
kk_define_string_literal(, kk_std_core__tag_ExnInternal, 20, "std/core/ExnInternal")

kk_std_core__exception kk_std_core__copy(kk_std_core__exception _this, kk_std_core_types__optional message0, kk_std_core_types__optional info0, kk_context_t* _ctx) { /* (exception, message : optional<string>, info : optional<exception-info>) -> exception */ 
  kk_string_t _x20466;
  if (kk_std_core_types__is_Optional(message0)) {
    kk_box_t _box_x18530 = message0._cons.Optional.value;
    kk_string_t _message_1688 = kk_string_unbox(_box_x18530);
    _x20466 = _message_1688; /*string*/
  }
  else {
    kk_string_t _x = _this.message;
    kk_string_dup(_x);
    _x20466 = _x; /*string*/
  }
  kk_std_core__exception_info _x20468;
  if (kk_std_core_types__is_Optional(info0)) {
    kk_box_t _box_x18531 = info0._cons.Optional.value;
    kk_std_core__exception_info _info_1694 = kk_std_core__exception_info_unbox(_box_x18531, NULL);
    kk_std_core__exception_drop(_this, _ctx);
    _x20468 = _info_1694; /*exception-info*/
  }
  else {
    kk_std_core__exception_info _x0 = _this.info;
    kk_std_core__exception_info_dup(_x0);
    kk_std_core__exception_drop(_this, _ctx);
    _x20468 = _x0; /*exception-info*/
  }
  return kk_std_core__new_Exception(_x20466, _x20468, _ctx);
}

kk_std_core__sslice kk_std_core__copy_2(kk_std_core__sslice _this, kk_std_core_types__optional str0, kk_std_core_types__optional start0, kk_std_core_types__optional len0, kk_context_t* _ctx) { /* (sslice, str : optional<string>, start : optional<ssize_t>, len : optional<ssize_t>) -> sslice */ 
  kk_string_t _x20473;
  if (kk_std_core_types__is_Optional(str0)) {
    kk_box_t _box_x18533 = str0._cons.Optional.value;
    kk_string_t _str_1826 = kk_string_unbox(_box_x18533);
    _x20473 = _str_1826; /*string*/
  }
  else {
    kk_string_t _x = _this.str;
    kk_string_dup(_x);
    _x20473 = _x; /*string*/
  }
  kk_ssize_t _x20475;
  if (kk_std_core_types__is_Optional(start0)) {
    kk_box_t _box_x18534 = start0._cons.Optional.value;
    kk_ssize_t _start_1832 = kk_ssize_unbox(_box_x18534, NULL);
    kk_std_core_types__optional_drop(start0, _ctx);
    _x20475 = _start_1832; /*ssize_t*/
  }
  else {
    kk_ssize_t _x0 = _this.start;
    _x20475 = _x0; /*ssize_t*/
  }
  kk_ssize_t _x20477;
  if (kk_std_core_types__is_Optional(len0)) {
    kk_box_t _box_x18535 = len0._cons.Optional.value;
    kk_ssize_t _len_1838 = kk_ssize_unbox(_box_x18535, NULL);
    kk_std_core_types__optional_drop(len0, _ctx);
    kk_std_core__sslice_drop(_this, _ctx);
    _x20477 = _len_1838; /*ssize_t*/
  }
  else {
    kk_ssize_t _x1 = _this.len;
    kk_std_core__sslice_drop(_this, _ctx);
    _x20477 = _x1; /*ssize_t*/
  }
  return kk_std_core__new_Sslice(_x20473, _x20475, _x20477, _ctx);
}

kk_std_core__stream kk_std_core__copy_3(kk_std_core__stream _this, kk_std_core_types__optional head0, kk_std_core_types__optional tail0, kk_context_t* _ctx) { /* forall<a> (stream<a>, head : optional<a>, tail : optional<stream<a>>) -> stream<a> */ 
  kk_box_t _x20481;
  if (kk_std_core_types__is_Optional(head0)) {
    kk_box_t _head_1884 = head0._cons.Optional.value;
    _x20481 = _head_1884; /*1901*/
  }
  else {
    struct kk_std_core_Next* _con20482 = kk_std_core__as_Next(_this);
    kk_box_t _x = _con20482->head;
    kk_box_dup(_x);
    _x20481 = _x; /*1901*/
  }
  kk_std_core__stream _x20483;
  if (kk_std_core_types__is_Optional(tail0)) {
    kk_box_t _box_x18536 = tail0._cons.Optional.value;
    kk_std_core__stream _tail_1891 = kk_std_core__stream_unbox(_box_x18536, NULL);
    kk_std_core__stream_drop(_this, _ctx);
    _x20483 = _tail_1891; /*stream<1901>*/
  }
  else {
    struct kk_std_core_Next* _con20485 = kk_std_core__as_Next(_this);
    kk_box_t _pat01 = _con20485->head;
    kk_std_core__stream _x0 = _con20485->tail;
    if (kk_likely(kk_std_core__stream_is_unique(_this))) {
      kk_box_drop(_pat01, _ctx);
      kk_std_core__stream_free(_this, _ctx);
    }
    else {
      kk_std_core__stream_dup(_x0);
      kk_std_core__stream_decref(_this, _ctx);
    }
    _x20483 = _x0; /*stream<1901>*/
  }
  return kk_std_core__new_Next(kk_reuse_null, _x20481, _x20483, _ctx);
}
 
// runtime tag for the `:exn` effect

kk_std_core_hnd__htag kk_std_core__tag_exn;
 
// handler for the `:exn` effect

kk_box_t kk_std_core__handle_exn(int32_t cfc, kk_std_core__hnd_exn hnd, kk_function_t ret, kk_function_t action, kk_context_t* _ctx) { /* forall<a,e,b> (cfc : int32, hnd : .hnd-exn<e,b>, ret : (res : a) -> e b, action : () -> <exn|e> a) -> e b */ 
  kk_std_core_hnd__htag _x20489 = kk_std_core_hnd__htag_dup(kk_std_core__tag_exn); /*std/core/hnd/htag<.hnd-exn>*/
  return kk_std_core_hnd__hhandle(_x20489, cfc, kk_std_core__hnd_exn_box(hnd, _ctx), ret, action, _ctx);
}
 
// Unsafe: transform any type to a `null` type; used internally by the compiler.

kk_std_core__null kk_std_core__null_any(kk_box_t x, kk_context_t* _ctx) { /* forall<a> (x : a) -> null<a> */ 
  return ((x).box == kk_box_null.box ? kk_datatype_from_ptr(NULL) : kk_datatype_unbox(x));
}

kk_ref_t kk_std_core_redirect;
 
// Print a string to the console, including a final newline character.

kk_unit_t kk_std_core_xprintsln(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> console () */ 
  kk_println(s,kk_context()); return kk_Unit;
}

kk_integer_t kk_std_core_string_compare(kk_string_t x, kk_string_t y, kk_context_t* _ctx) { /* (x : string, y : string) -> int */ 
  return kk_string_cmp_int(x,y,kk_context());
}
 
// Convert an integer to an `:int32`. The number is _clamped_ to the maximal or minimum `:int32`
// value if it is outside the range of an `:int32`.

int32_t kk_std_core_int32(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> int32 */ 
  return kk_integer_clamp32(i,kk_context());
}
 
// Convert an integer to an `:ssize_t`. The number is _clamped_ to the maximal or minimum `:ssize_t`
// value if it is outside the range of an `:ssize_t`.

kk_ssize_t kk_std_core_ssize__t(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> ssize_t */ 
  return kk_integer_clamp_ssize_t(i,kk_context());
}
 
// Convert a character to a string

kk_string_t kk_std_core_string(kk_char_t c, kk_context_t* _ctx) { /* (c : char) -> string */ 
  return kk_string_from_char(c,kk_context());
}
 
// Convert a vector of characters to a string.

kk_string_t kk_std_core_string_1(kk_vector_t _arg1, kk_context_t* _ctx) { /* (vector<char>) -> string */ 
  return kk_string_from_chars(_arg1,kk_context());
}
 
// Convert a list of characters to a string

kk_string_t kk_std_core_string_2(kk_std_core__list cs, kk_context_t* _ctx) { /* (cs : list<char>) -> total string */ 
  return kk_string_from_list(cs,kk_context());
}
 
// O(n). Copy the `slice` argument into a fresh string.
// Takes O(1) time if the slice covers the entire string.

kk_string_t kk_std_core_string_3(kk_std_core__sslice slice0, kk_context_t* _ctx) { /* (slice : sslice) -> string */ 
  return kk_slice_to_string(slice0,kk_context());
}
 
// Convert a vector to a list with an optional tail.

kk_std_core__list kk_std_core_vlist(kk_vector_t v, kk_std_core_types__optional tail0, kk_context_t* _ctx) { /* forall<a> (v : vector<a>, tail : optional<list<a>>) -> list<a> */ 
  kk_std_core__list _x20492;
  if (kk_std_core_types__is_Optional(tail0)) {
    kk_box_t _box_x18550 = tail0._cons.Optional.value;
    kk_std_core__list _tail_2132 = kk_std_core__list_unbox(_box_x18550, NULL);
    _x20492 = _tail_2132; /*list<2142>*/
  }
  else {
    _x20492 = kk_std_core__new_Nil(_ctx); /*list<2142>*/
  }
  return kk_vector_to_list(v,_x20492,kk_context());
}

kk_string_t kk_std_core_int_show_hex(kk_integer_t i, bool use_capitals, kk_context_t* _ctx) { /* (i : int, use-capitals : bool) -> string */ 
  return kk_integer_to_hex_string(i,use_capitals,kk_context());
}

kk_string_t kk_std_core_repeatz(kk_string_t s, kk_ssize_t n, kk_context_t* _ctx) { /* (s : string, n : ssize_t) -> string */ 
  return kk_string_repeat(s,n,kk_context());
}

kk_string_t kk_std_core_show_expx(double d, int32_t prec, kk_context_t* _ctx) { /* (d : float64, prec : int32) -> string */ 
  return kk_double_show_exp(d,prec,kk_context());
}

kk_string_t kk_std_core_show_fixedx(double d, int32_t prec, kk_context_t* _ctx) { /* (d : float64, prec : int32) -> string */ 
  return kk_double_show_fixed(d,prec,kk_context());
}
 
// Print a string to the console

kk_unit_t kk_std_core_xprints(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> console () */ 
  kk_print(s,kk_context()); return kk_Unit;
}
 
// Raise an integer `i` to the power of `exp`.

kk_integer_t kk_std_core_pow(kk_integer_t i, kk_integer_t exp, kk_context_t* _ctx) { /* (i : int, exp : int) -> int */ 
  return kk_integer_pow(i,exp,kk_context());
}
 
// O(`count`). Advance the start position of a string slice by `count` characters
// up to the end of the string.
// A negative `count` advances the start position backwards upto the first position
// in a string.
// Maintains the character count of the original slice upto the end of the string.
// For example:
//
// * `"abc".first.advance(1).string == "b"`,
// * `"abc".first.advance(3).string == ""`,
// * `"abc".last.advance(-1).string == "b"`.
//

kk_std_core__sslice kk_std_core_advance(kk_std_core__sslice slice0, kk_integer_t count, kk_context_t* _ctx) { /* (slice : sslice, count : int) -> sslice */ 
  return kk_slice_advance_borrow(slice0,count,kk_context());
}

kk_unit_t kk_std_core_unsafe_assert_fail(kk_string_t msg, kk_context_t* _ctx) { /* (msg : string) -> () */ 
  kk_assert_fail(msg,kk_context()); return kk_Unit;
}
 
// O(`count`). Extend a string slice by `count` characters up to the end of the string.
// A negative `count` shrinks the slice up to the empty slice.
// For example:
//
// * `"abc".first.extend(1).string == "ab"`
// * `"abc".last.extend(-1).string == ""`
//

kk_std_core__sslice kk_std_core_extend(kk_std_core__sslice slice0, kk_integer_t count, kk_context_t* _ctx) { /* (slice : sslice, count : int) -> sslice */ 
  return kk_slice_extend_borrow(slice0,count,kk_context());
}

kk_std_core__sslice kk_std_core_first1(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> sslice */ 
  return kk_slice_first(s,kk_context());
}
 
// Convert a string to upper-case

kk_string_t kk_std_core_to_upper(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> string */ 
  return kk_string_to_upper(s,kk_context());
}

kk_integer_t kk_std_core_cdiv_exp10(kk_integer_t i, kk_integer_t n, kk_context_t* _ctx) { /* (i : int, n : int) -> int */ 
  return kk_integer_cdiv_pow10(i,n,kk_context());
}

kk_integer_t kk_std_core_mul_exp10(kk_integer_t i, kk_integer_t n, kk_context_t* _ctx) { /* (i : int, n : int) -> int */ 
  return kk_integer_mul_pow10(i,n,kk_context());
}
 
// Return the common prefix of two strings (upto `upto` characters (default is minimum length of the two strings))

kk_std_core__sslice kk_std_core_common_prefix(kk_string_t s, kk_string_t t, kk_std_core_types__optional upto, kk_context_t* _ctx) { /* (s : string, t : string, upto : optional<int>) -> sslice */ 
  kk_integer_t _brw_20348;
  if (kk_std_core_types__is_Optional(upto)) {
    kk_box_t _box_x18551 = upto._cons.Optional.value;
    kk_integer_t _upto_2231 = kk_integer_unbox(_box_x18551);
    _brw_20348 = kk_integer_dup(_upto_2231); /*int*/
  }
  else {
    _brw_20348 = kk_integer_from_small(-1); /*int*/
  }
  kk_std_core__sslice _brw_20349 = kk_slice_common_prefix_borrow(s,t,_brw_20348,kk_context()); /*sslice*/;
  kk_integer_drop(_brw_20348, _ctx);
  return _brw_20349;
}
 
// lifted local: concat, concat-pre

kk_std_core__list kk_std_core__ctail_lift17272_concat(kk_std_core__list ys, kk_std_core__list zss, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* forall<a> (ys : list<a>, zss : list<list<a>>, ctail<list<a>>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con20495 = kk_std_core__as_Cons(ys);
    kk_box_t y = _con20495->head;
    kk_std_core__list yy = _con20495->tail;
    kk_reuse_t _ru_20351 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      _ru_20351 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(y);
      kk_std_core__list_dup(yy);
      kk_std_core__list_decref(ys, _ctx);
    }
    kk_std_core__list _ctail_17512 = kk_std_core__list_hole(); /*list<2279>*/;
    kk_std_core__list _ctail_17513 = kk_std_core__new_Cons(_ru_20351, y, _ctail_17512, _ctx); /*list<2279>*/;
    kk_box_t* _b_18563_18557 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17513)->tail)); /*cfield<list<2279>>*/;
    { // tailcall
      kk_std_core_types__ctail _x20496 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17513, _ctx)),_b_18563_18557); /*ctail<0>*/
      ys = yy;
      _acc = _x20496;
      goto kk__tailcall;
    }
  }
  if (kk_std_core__is_Cons(zss)) {
    struct kk_std_core_Cons* _con20497 = kk_std_core__as_Cons(zss);
    kk_box_t _box_x18558 = _con20497->head;
    kk_std_core__list zzs = _con20497->tail;
    kk_std_core__list zs = kk_std_core__list_unbox(_box_x18558, NULL);
    if (kk_likely(kk_std_core__list_is_unique(zss))) {
      kk_std_core__list_free(zss, _ctx);
    }
    else {
      kk_std_core__list_dup(zs);
      kk_std_core__list_dup(zzs);
      kk_std_core__list_decref(zss, _ctx);
    }
    { // tailcall
      ys = zs;
      zss = zzs;
      goto kk__tailcall;
    }
  }
  {
    kk_box_t _x20499 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20499, _ctx);
  }
}
 
// lifted local: concat, concat-pre

kk_std_core__list kk_std_core__lift17272_concat(kk_std_core__list ys0, kk_std_core__list zss0, kk_context_t* _ctx) { /* forall<a> (ys : list<a>, zss : list<list<a>>) -> list<a> */ 
  kk_std_core_types__ctail _x20500 = kk_ctail_nil(); /*ctail<0>*/
  return kk_std_core__ctail_lift17272_concat(ys0, zss0, _x20500, _ctx);
}
extern kk_box_t kk_std_core_const_fun20501_1(kk_function_t _fself, kk_box_t ___wildcard__110__6, kk_context_t* _ctx) {
  struct kk_std_core_const_fun20501__t_1* _self = kk_function_as(struct kk_std_core_const_fun20501__t_1*, _fself);
  kk_box_t default0 = _self->default0; /* 2307 */
  kk_drop_match(_self, {kk_box_dup(default0);}, {}, _ctx)
  kk_box_drop(___wildcard__110__6, _ctx);
  return default0;
}
 
// If the slice is not empty,
// return the first character, and a new slice that is advanced by 1.

kk_std_core_types__maybe kk_std_core_next(kk_std_core__sslice slice0, kk_context_t* _ctx) { /* (slice : sslice) -> maybe<(char, sslice)> */ 
  return kk_slice_next(slice0,kk_context());
}
 
// Return the number of decimal digits of `i`. Return `0` when `i==0`.

kk_integer_t kk_std_core_count_digits(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> int */ 
  return kk_integer_count_digits(i,kk_context());
}
 
// Transform an `:error` type to an `:either` value.

kk_std_core_types__either kk_std_core_either(kk_std_core__error t, kk_context_t* _ctx) { /* forall<a> (t : error<a>) -> either<exception,a> */ 
  if (kk_std_core__is_Error(t)) {
    kk_std_core__exception exn0 = t._cons.Error.exception;
    kk_std_core__exception_dup(exn0);
    kk_std_core__error_drop(t, _ctx);
    return kk_std_core_types__new_Left(kk_std_core__exception_box(exn0, _ctx), _ctx);
  }
  {
    kk_box_t x = t._cons.Ok.result;
    return kk_std_core_types__new_Right(x, _ctx);
  }
}

bool kk_std_core_xends_with(kk_string_t s, kk_string_t post, kk_context_t* _ctx) { /* (s : string, post : string) -> bool */ 
  return kk_string_ends_with(s,post,kk_context());
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift17984_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_std_core__list zz, kk_std_core__list ys1_17441, kk_context_t* _ctx) { /* forall<a,b,e> (ctail<list<b>>, f : (a) -> e list<b>, zz : list<a>, ys1.17441 : list<b>) -> e list<b> */ 
  return kk_std_core__ctail_lift17273_flatmap(f, ys1_17441, zz, _acc, _ctx);
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift17985_op(kk_function_t _accm, kk_function_t f0, kk_std_core__list zz0, kk_std_core__list ys1_174410, kk_context_t* _ctx) { /* forall<a,b,e> ((list<b>) -> list<b>, f : (a) -> e list<b>, zz : list<a>, ys1.17441 : list<b>) -> e list<b> */ 
  return kk_std_core__ctailm_lift17273_flatmap(f0, ys1_174410, zz0, _accm, _ctx);
}
 
// lifted local: flatmap, flatmap-pre


// lift anonymous function
struct kk_std_core__ctail_lift17273_flatmap_fun20507__t {
  struct kk_function_s _base;
  kk_function_t f1;
  kk_std_core__list zz1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_lift17273_flatmap_fun20507(kk_function_t _fself, kk_box_t _b_18578, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_lift17273_flatmap_fun20507(kk_function_t f1, kk_std_core__list zz1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_lift17273_flatmap_fun20507__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_lift17273_flatmap_fun20507__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_lift17273_flatmap_fun20507, kk_context());
  _self->f1 = f1;
  _self->zz1 = zz1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_lift17273_flatmap_fun20507(kk_function_t _fself, kk_box_t _b_18578, kk_context_t* _ctx) {
  struct kk_std_core__ctail_lift17273_flatmap_fun20507__t* _self = kk_function_as(struct kk_std_core__ctail_lift17273_flatmap_fun20507__t*, _fself);
  kk_function_t f1 = _self->f1; /* (2456) -> 2458 list<2457> */
  kk_std_core__list zz1 = _self->zz1; /* list<2456> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<2457>> */
  kk_drop_match(_self, {kk_function_dup(f1);kk_std_core__list_dup(zz1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_std_core__list ys1_18590_174411 = kk_std_core__list_unbox(_b_18578, _ctx); /*list<2457>*/;
  kk_std_core__list _x20508 = kk_std_core__mlift17984_op(_acc0, f1, zz1, ys1_18590_174411, _ctx); /*list<2457>*/
  return kk_std_core__list_box(_x20508, _ctx);
}

kk_std_core__list kk_std_core__ctail_lift17273_flatmap(kk_function_t f1, kk_std_core__list ys, kk_std_core__list zs, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,b,e> (f : (a) -> e list<b>, ys : list<b>, zs : list<a>, ctail<list<b>>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con20502 = kk_std_core__as_Cons(ys);
    kk_box_t y = _con20502->head;
    kk_std_core__list yy = _con20502->tail;
    kk_reuse_t _ru_20353 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      _ru_20353 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(y);
      kk_std_core__list_dup(yy);
      kk_std_core__list_decref(ys, _ctx);
    }
    kk_std_core__list _ctail_17514 = kk_std_core__list_hole(); /*list<2457>*/;
    kk_std_core__list _ctail_17515 = kk_std_core__new_Cons(_ru_20353, y, _ctail_17514, _ctx); /*list<2457>*/;
    kk_box_t* _b_18583_18576 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17515)->tail)); /*cfield<list<2457>>*/;
    { // tailcall
      kk_std_core_types__ctail _x20503 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_17515, _ctx)),_b_18583_18576); /*ctail<0>*/
      ys = yy;
      _acc0 = _x20503;
      goto kk__tailcall;
    }
  }
  if (kk_std_core__is_Cons(zs)) {
    struct kk_std_core_Cons* _con20504 = kk_std_core__as_Cons(zs);
    kk_box_t z = _con20504->head;
    kk_std_core__list zz1 = _con20504->tail;
    if (kk_likely(kk_std_core__list_is_unique(zs))) {
      kk_std_core__list_free(zs, _ctx);
    }
    else {
      kk_box_dup(z);
      kk_std_core__list_dup(zz1);
      kk_std_core__list_decref(zs, _ctx);
    }
    kk_std_core__list x_18066;
    kk_function_t _x20505 = kk_function_dup(f1); /*(2456) -> 2458 list<2457>*/
    x_18066 = kk_function_call(kk_std_core__list, (kk_function_t, kk_box_t, kk_context_t*), _x20505, (_x20505, z, _ctx)); /*list<2457>*/
    if (kk_yielding(kk_context())) {
      kk_std_core__list_drop(x_18066, _ctx);
      kk_box_t _x20506 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_lift17273_flatmap_fun20507(f1, zz1, _acc0, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20506, _ctx);
    }
    { // tailcall
      ys = x_18066;
      zs = zz1;
      goto kk__tailcall;
    }
  }
  {
    kk_function_drop(f1, _ctx);
    kk_box_t _x20509 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20509, _ctx);
  }
}
 
// lifted local: flatmap, flatmap-pre


// lift anonymous function
struct kk_std_core__ctailm_lift17273_flatmap_fun20512__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t y0;
};
static kk_std_core__list kk_std_core__ctailm_lift17273_flatmap_fun20512(kk_function_t _fself, kk_std_core__list _ctail_17517, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_lift17273_flatmap_fun20512(kk_function_t _accm0, kk_box_t y0, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17273_flatmap_fun20512__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_lift17273_flatmap_fun20512__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_lift17273_flatmap_fun20512, kk_context());
  _self->_accm0 = _accm0;
  _self->y0 = y0;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_lift17273_flatmap_fun20512(kk_function_t _fself, kk_std_core__list _ctail_17517, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17273_flatmap_fun20512__t* _self = kk_function_as(struct kk_std_core__ctailm_lift17273_flatmap_fun20512__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<2457>) -> list<2457> */
  kk_box_t y0 = _self->y0; /* 2457 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(y0);}, {}, _ctx)
  kk_std_core__list _x20513 = kk_std_core__new_Cons(kk_reuse_null, y0, _ctail_17517, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x20513, _ctx));
}


// lift anonymous function
struct kk_std_core__ctailm_lift17273_flatmap_fun20517__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t f2;
  kk_std_core__list zz2;
};
static kk_box_t kk_std_core__ctailm_lift17273_flatmap_fun20517(kk_function_t _fself, kk_box_t _b_18592, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_lift17273_flatmap_fun20517(kk_function_t _accm0, kk_function_t f2, kk_std_core__list zz2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17273_flatmap_fun20517__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_lift17273_flatmap_fun20517__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_lift17273_flatmap_fun20517, kk_context());
  _self->_accm0 = _accm0;
  _self->f2 = f2;
  _self->zz2 = zz2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_lift17273_flatmap_fun20517(kk_function_t _fself, kk_box_t _b_18592, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17273_flatmap_fun20517__t* _self = kk_function_as(struct kk_std_core__ctailm_lift17273_flatmap_fun20517__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<2457>) -> list<2457> */
  kk_function_t f2 = _self->f2; /* (2456) -> 2458 list<2457> */
  kk_std_core__list zz2 = _self->zz2; /* list<2456> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(f2);kk_std_core__list_dup(zz2);}, {}, _ctx)
  kk_std_core__list ys1_18594_174413 = kk_std_core__list_unbox(_b_18592, _ctx); /*list<2457>*/;
  kk_std_core__list _x20518 = kk_std_core__mlift17985_op(_accm0, f2, zz2, ys1_18594_174413, _ctx); /*list<2457>*/
  return kk_std_core__list_box(_x20518, _ctx);
}

kk_std_core__list kk_std_core__ctailm_lift17273_flatmap(kk_function_t f2, kk_std_core__list ys0, kk_std_core__list zs0, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,b,e> (f : (a) -> e list<b>, ys : list<b>, zs : list<a>, (list<b>) -> list<b>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys0)) {
    struct kk_std_core_Cons* _con20510 = kk_std_core__as_Cons(ys0);
    kk_box_t y0 = _con20510->head;
    kk_std_core__list yy0 = _con20510->tail;
    if (kk_likely(kk_std_core__list_is_unique(ys0))) {
      kk_std_core__list_free(ys0, _ctx);
    }
    else {
      kk_box_dup(y0);
      kk_std_core__list_dup(yy0);
      kk_std_core__list_decref(ys0, _ctx);
    }
    { // tailcall
      kk_function_t _x20511 = kk_std_core__new_ctailm_lift17273_flatmap_fun20512(_accm0, y0, _ctx); /*(list<2457>) -> list<2457>*/
      ys0 = yy0;
      _accm0 = _x20511;
      goto kk__tailcall;
    }
  }
  if (kk_std_core__is_Cons(zs0)) {
    struct kk_std_core_Cons* _con20514 = kk_std_core__as_Cons(zs0);
    kk_box_t z0 = _con20514->head;
    kk_std_core__list zz2 = _con20514->tail;
    if (kk_likely(kk_std_core__list_is_unique(zs0))) {
      kk_std_core__list_free(zs0, _ctx);
    }
    else {
      kk_box_dup(z0);
      kk_std_core__list_dup(zz2);
      kk_std_core__list_decref(zs0, _ctx);
    }
    kk_std_core__list x0_18069;
    kk_function_t _x20515 = kk_function_dup(f2); /*(2456) -> 2458 list<2457>*/
    x0_18069 = kk_function_call(kk_std_core__list, (kk_function_t, kk_box_t, kk_context_t*), _x20515, (_x20515, z0, _ctx)); /*list<2457>*/
    if (kk_yielding(kk_context())) {
      kk_std_core__list_drop(x0_18069, _ctx);
      kk_box_t _x20516 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_lift17273_flatmap_fun20517(_accm0, f2, zz2, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20516, _ctx);
    }
    { // tailcall
      ys0 = x0_18069;
      zs0 = zz2;
      goto kk__tailcall;
    }
  }
  {
    kk_function_drop(f2, _ctx);
    return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
  }
}
 
// lifted local: flatmap, flatmap-pre


// lift anonymous function
struct kk_std_core__lift17273_flatmap_fun20520__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core__lift17273_flatmap_fun20520(kk_function_t _fself, kk_std_core__list _ctail_17516, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17273_flatmap_fun20520(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core__lift17273_flatmap_fun20520, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core__lift17273_flatmap_fun20520(kk_function_t _fself, kk_std_core__list _ctail_17516, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17516;
}

kk_std_core__list kk_std_core__lift17273_flatmap(kk_function_t f3, kk_std_core__list ys1, kk_std_core__list zs1, kk_context_t* _ctx) { /* forall<a,b,e> (f : (a) -> e list<b>, ys : list<b>, zs : list<a>) -> e list<b> */ 
  bool _match_20345 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_20345) {
    kk_std_core_types__ctail _x20519 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_lift17273_flatmap(f3, ys1, zs1, _x20519, _ctx);
  }
  {
    return kk_std_core__ctailm_lift17273_flatmap(f3, ys1, zs1, kk_std_core__new_lift17273_flatmap_fun20520(_ctx), _ctx);
  }
}
 
// lifted local: reverse-append, reverse-acc

kk_std_core__list kk_std_core__lift17274_reverse_append(kk_std_core__list acc, kk_std_core__list ys, kk_context_t* _ctx) { /* forall<a> (acc : list<a>, ys : list<a>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con20521 = kk_std_core__as_Cons(ys);
    kk_box_t x = _con20521->head;
    kk_std_core__list xx = _con20521->tail;
    kk_reuse_t _ru_20357 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      _ru_20357 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(ys, _ctx);
    }
    { // tailcall
      kk_std_core__list _x20522 = kk_std_core__new_Cons(_ru_20357, x, acc, _ctx); /*list<61>*/
      acc = _x20522;
      ys = xx;
      goto kk__tailcall;
    }
  }
  {
    return acc;
  }
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift17987_force_fun20526__t {
  struct kk_function_s _base;
  kk_ref_t r;
};
static kk_box_t kk_std_core__mlift17987_force_fun20526(kk_function_t _fself, kk_box_t x0, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17987_force_fun20526(kk_ref_t r, kk_context_t* _ctx) {
  struct kk_std_core__mlift17987_force_fun20526__t* _self = kk_function_alloc_as(struct kk_std_core__mlift17987_force_fun20526__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17987_force_fun20526, kk_context());
  _self->r = r;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift17987_force_fun20526(kk_function_t _fself, kk_box_t x0, kk_context_t* _ctx) {
  struct kk_std_core__mlift17987_force_fun20526__t* _self = kk_function_as(struct kk_std_core__mlift17987_force_fun20526__t*, _fself);
  kk_ref_t r = _self->r; /* ref<global,either<() -> 2593 2592,2592>> */
  kk_drop_match(_self, {kk_ref_dup(r);}, {}, _ctx)
  kk_unit_t __ = kk_Unit;
  kk_unit_t _brw_20343 = kk_Unit;
  kk_box_t _x20527;
  kk_std_core_types__either _x20528;
  kk_box_t _x20529 = kk_box_dup(x0); /*2592*/
  _x20528 = kk_std_core_types__new_Right(_x20529, _ctx); /*either<1024,1025>*/
  _x20527 = kk_std_core_types__either_box(_x20528, _ctx); /*1000*/
  kk_ref_set_borrow(r,_x20527,kk_context());
  kk_ref_drop(r, _ctx);
  _brw_20343;
  return x0;
}

kk_box_t kk_std_core__mlift17987_force(kk_ref_t r, kk_function_t _y_17639, kk_context_t* _ctx) { /* forall<a,e> (r : ref<global,either<() -> e a,a>>, () -> <st<global>,div|e> a) -> <alloc<global>,div,read<global>,write<global>|e> a */ 
  kk_box_t x_18072 = kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), _y_17639, (_y_17639, _ctx)); /*2592*/;
  kk_function_t next0_18073 = kk_std_core__new_mlift17987_force_fun20526(r, _ctx); /*(2592) -> <st<global>,div|2593> 2592*/;
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_18072, _ctx);
    return kk_std_core_hnd_yield_extend(next0_18073, _ctx);
  }
  {
    return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), next0_18073, (next0_18073, x_18072, _ctx));
  }
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift17988_force_fun20530__t {
  struct kk_function_s _base;
  kk_ref_t r;
};
static kk_box_t kk_std_core__mlift17988_force_fun20530(kk_function_t _fself, kk_box_t _b_18607, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17988_force_fun20530(kk_ref_t r, kk_context_t* _ctx) {
  struct kk_std_core__mlift17988_force_fun20530__t* _self = kk_function_alloc_as(struct kk_std_core__mlift17988_force_fun20530__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17988_force_fun20530, kk_context());
  _self->r = r;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift17988_force_fun20530(kk_function_t _fself, kk_box_t _b_18607, kk_context_t* _ctx) {
  struct kk_std_core__mlift17988_force_fun20530__t* _self = kk_function_as(struct kk_std_core__mlift17988_force_fun20530__t*, _fself);
  kk_ref_t r = _self->r; /* ref<global,either<() -> 2593 2592,2592>> */
  kk_drop_match(_self, {kk_ref_dup(r);}, {}, _ctx)
  kk_function_t _x20531 = kk_function_unbox(_b_18607); /*() -> <st<global>,div|2593> 18608*/
  return kk_std_core__mlift17987_force(r, _x20531, _ctx);
}

kk_box_t kk_std_core__mlift17988_force(kk_ref_t r, kk_std_core_types__either _y_17637, kk_context_t* _ctx) { /* forall<a,e> (r : ref<global,either<() -> e a,a>>, either<() -> e a,a>) -> <read<global>,div,alloc<global>,write<global>|e> a */ 
  if (kk_std_core_types__is_Right(_y_17637)) {
    kk_box_t x = _y_17637._cons.Right.right;
    kk_ref_drop(r, _ctx);
    return x;
  }
  {
    kk_box_t _fun_unbox_x18604 = _y_17637._cons.Left.left;
    if (kk_yielding(kk_context())) {
      kk_box_drop(_fun_unbox_x18604, _ctx);
      return kk_std_core_hnd_yield_extend(kk_std_core__new_mlift17988_force_fun20530(r, _ctx), _ctx);
    }
    {
      kk_function_t _x20532 = kk_function_unbox(_fun_unbox_x18604); /*() -> 2593 18605*/
      return kk_std_core__mlift17987_force(r, _x20532, _ctx);
    }
  }
}
 
// Force a delayed value; the value is computed only on the first
// call to `force` and cached afterwards.


// lift anonymous function
struct kk_std_core_force_fun20535__t {
  struct kk_function_s _base;
  kk_std_core__delayed delayed;
};
static kk_box_t kk_std_core_force_fun20535(kk_function_t _fself, kk_box_t _b_18614, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_force_fun20535(kk_std_core__delayed delayed, kk_context_t* _ctx) {
  struct kk_std_core_force_fun20535__t* _self = kk_function_alloc_as(struct kk_std_core_force_fun20535__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_force_fun20535, kk_context());
  _self->delayed = delayed;
  return &_self->_base;
}

static kk_box_t kk_std_core_force_fun20535(kk_function_t _fself, kk_box_t _b_18614, kk_context_t* _ctx) {
  struct kk_std_core_force_fun20535__t* _self = kk_function_as(struct kk_std_core_force_fun20535__t*, _fself);
  kk_std_core__delayed delayed = _self->delayed; /* delayed<2593,2592> */
  kk_drop_match(_self, {kk_std_core__delayed_dup(delayed);}, {}, _ctx)
  kk_std_core_types__either _y_18616_17637 = kk_std_core_types__either_unbox(_b_18614, _ctx); /*either<() -> 2593 2592,2592>*/;
  kk_ref_t _x20536;
  {
    kk_ref_t _x0 = delayed.dref;
    _x20536 = _x0; /*ref<global,either<() -> 2593 2592,2592>>*/
  }
  return kk_std_core__mlift17988_force(_x20536, _y_18616_17637, _ctx);
}

kk_box_t kk_std_core_force(kk_std_core__delayed delayed, kk_context_t* _ctx) { /* forall<a,e> (delayed : delayed<e,a>) -> e a */ 
  kk_std_core_types__either x_18081;
  kk_box_t _x20533;
  kk_ref_t _x20534;
  {
    kk_ref_t _x = delayed.dref;
    kk_ref_dup(_x);
    _x20534 = _x; /*ref<global,either<() -> 2593 2592,2592>>*/
  }
  _x20533 = kk_ref_get(_x20534,kk_context()); /*1001*/
  x_18081 = kk_std_core_types__either_unbox(_x20533, _ctx); /*either<() -> 2593 2592,2592>*/
  if (kk_yielding(kk_context())) {
    kk_std_core_types__either_drop(x_18081, _ctx);
    return kk_std_core_hnd_yield_extend(kk_std_core_new_force_fun20535(delayed, _ctx), _ctx);
  }
  {
    kk_std_core_types__either _y_18617_17637 = x_18081; /*either<() -> 2593 2592,2592>*/;
    kk_ref_t _x20537;
    {
      kk_ref_t _x0 = delayed.dref;
      _x20537 = _x0; /*ref<global,either<() -> 2593 2592,2592>>*/
    }
    return kk_std_core__mlift17988_force(_x20537, _y_18617_17637, _ctx);
  }
}
 
// Generic show: shows the internal representation of an object as a string
// Note: this breaks parametricity so it should not be pub

kk_string_t kk_std_core_gshow(kk_box_t _arg1, kk_context_t* _ctx) { /* forall<a> (a) -> string */ 
  return kk_show_any(_arg1,kk_context());
}
 
// Return the host environment: `dotnet`, `browser`, `webworker`, `node`, or `libc`.

kk_string_t kk_std_core_host(kk_context_t* _ctx) { /* () -> ndet string */ 
  return kk_get_host(kk_context());
}
 
// clamp an `:int` to fit in an `:int64_t`.

int64_t kk_std_core_int64(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> int64 */ 
  return kk_integer_clamp64(i,kk_context());
}
 
// clamp an `:int` to fit in an `:int16`.

int16_t kk_std_core_int16(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> int16 */ 
  return kk_integer_clamp_int16(i,kk_context());
}
 
// clamp an `:int` to fit in an `:int8`.

int8_t kk_std_core_int8(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> int8 */ 
  return kk_integer_clamp_int8(i,kk_context());
}
 
// lifted local: intersperse, before0

kk_std_core__list kk_std_core__ctail_lift17275_intersperse(kk_box_t sep, kk_std_core__list ys, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* forall<a> (sep : a, ys : list<a>, ctail<list<a>>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con20538 = kk_std_core__as_Cons(ys);
    kk_box_t y = _con20538->head;
    kk_std_core__list yy = _con20538->tail;
    kk_reuse_t _ru_20358 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      _ru_20358 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(y);
      kk_std_core__list_dup(yy);
      kk_std_core__list_decref(ys, _ctx);
    }
    kk_std_core__list _ctail_17518;
    kk_std_core__list _x20539 = kk_std_core__list_hole(); /*list<2679>*/
    _ctail_17518 = kk_std_core__new_Cons(_ru_20358, y, _x20539, _ctx); /*list<2679>*/
    kk_box_t* _b_18628_18623 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17518)->tail)); /*cfield<list<2679>>*/;
    { // tailcall
      kk_box_t _x20540 = kk_box_dup(sep); /*2679*/
      kk_std_core_types__ctail _x20541;
      kk_box_t _x20542;
      kk_std_core__list _x20543 = kk_std_core__new_Cons(kk_reuse_null, sep, _ctail_17518, _ctx); /*list<61>*/
      _x20542 = kk_std_core__list_box(_x20543, _ctx); /*0*/
      _x20541 = kk_ctail_link(_acc,_x20542,_b_18628_18623); /*ctail<0>*/
      sep = _x20540;
      ys = yy;
      _acc = _x20541;
      goto kk__tailcall;
    }
  }
  {
    kk_box_drop(sep, _ctx);
    kk_box_t _x20544 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20544, _ctx);
  }
}
 
// lifted local: intersperse, before0

kk_std_core__list kk_std_core__lift17275_intersperse(kk_box_t sep0, kk_std_core__list ys0, kk_context_t* _ctx) { /* forall<a> (sep : a, ys : list<a>) -> list<a> */ 
  kk_std_core_types__ctail _x20545 = kk_ctail_nil(); /*ctail<0>*/
  return kk_std_core__ctail_lift17275_intersperse(sep0, ys0, _x20545, _ctx);
}
 
// Insert a separator `sep`  between all elements of a list `xs` .

kk_std_core__list kk_std_core_intersperse(kk_std_core__list xs, kk_box_t sep, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, sep : a) -> list<a> */ 
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con20546 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con20546->head;
    kk_std_core__list xx = _con20546->tail;
    kk_reuse_t _ru_20359 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      _ru_20359 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_std_core__list _x20547 = kk_std_core__lift17275_intersperse(sep, xx, _ctx); /*list<2679>*/
    return kk_std_core__new_Cons(_ru_20359, x, _x20547, _ctx);
  }
  {
    kk_box_drop(sep, _ctx);
    return kk_std_core__new_Nil(_ctx);
  }
}
 
// clamp an `:int` to fit in an `:intptr_t`.

intptr_t kk_std_core_intptr__t(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> intptr_t */ 
  return kk_integer_clamp_intptr_t(i,kk_context());
}
 
// Return the number of ending `0` digits of `i`. Return `0` when `i==0`.

kk_integer_t kk_std_core_is_exp10(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> int */ 
  return kk_integer_ctz(i,kk_context());
}

kk_std_core__sslice kk_std_core_last1(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> sslice */ 
  return kk_slice_last(s,kk_context());
}
 
// Used by the compiler to wrap main console applications

kk_box_t kk_std_core_main_console(kk_function_t main, kk_context_t* _ctx) { /* forall<a,e> (main : () -> e a) -> e a */ 
  return kk_main_console(main,kk_context());
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift17989_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_std_core__list yy, kk_box_t _ctail_17520, kk_context_t* _ctx) { /* forall<a,b,e> (ctail<list<b>>, f : (value : a, rest : list<a>) -> e b, yy : list<a>, b) -> e list<b> */ 
  kk_std_core__list _ctail_17521 = kk_std_core__list_hole(); /*list<2770>*/;
  kk_std_core__list _ctail_17522 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17520, _ctail_17521, _ctx); /*list<2770>*/;
  kk_box_t* _b_18642_18639 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17522)->tail)); /*cfield<list<2770>>*/;
  kk_std_core_types__ctail _x20548 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17522, _ctx)),_b_18642_18639); /*ctail<0>*/
  return kk_std_core__ctail_lift17276_map_peek(f, yy, _x20548, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift17990_op_fun20549__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t _ctail_17525;
};
static kk_std_core__list kk_std_core__mlift17990_op_fun20549(kk_function_t _fself, kk_std_core__list _ctail_17524, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17990_op_fun20549(kk_function_t _accm, kk_box_t _ctail_17525, kk_context_t* _ctx) {
  struct kk_std_core__mlift17990_op_fun20549__t* _self = kk_function_alloc_as(struct kk_std_core__mlift17990_op_fun20549__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17990_op_fun20549, kk_context());
  _self->_accm = _accm;
  _self->_ctail_17525 = _ctail_17525;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift17990_op_fun20549(kk_function_t _fself, kk_std_core__list _ctail_17524, kk_context_t* _ctx) {
  struct kk_std_core__mlift17990_op_fun20549__t* _self = kk_function_as(struct kk_std_core__mlift17990_op_fun20549__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<2770>) -> list<2770> */
  kk_box_t _ctail_17525 = _self->_ctail_17525; /* 2770 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(_ctail_17525);}, {}, _ctx)
  kk_std_core__list _x20550 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17525, _ctail_17524, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x20550, _ctx));
}

kk_std_core__list kk_std_core__mlift17990_op(kk_function_t _accm, kk_function_t f0, kk_std_core__list yy0, kk_box_t _ctail_17525, kk_context_t* _ctx) { /* forall<a,b,e> ((list<b>) -> list<b>, f : (value : a, rest : list<a>) -> e b, yy : list<a>, b) -> e list<b> */ 
  return kk_std_core__ctailm_lift17276_map_peek(f0, yy0, kk_std_core__new_mlift17990_op_fun20549(_accm, _ctail_17525, _ctx), _ctx);
}
 
// lifted local: map-peek, mappeek


// lift anonymous function
struct kk_std_core__ctail_lift17276_map_peek_fun20555__t {
  struct kk_function_s _base;
  kk_function_t f1;
  kk_std_core__list yy1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_lift17276_map_peek_fun20555(kk_function_t _fself, kk_box_t _b_18647, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_lift17276_map_peek_fun20555(kk_function_t f1, kk_std_core__list yy1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_lift17276_map_peek_fun20555__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_lift17276_map_peek_fun20555__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_lift17276_map_peek_fun20555, kk_context());
  _self->f1 = f1;
  _self->yy1 = yy1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_lift17276_map_peek_fun20555(kk_function_t _fself, kk_box_t _b_18647, kk_context_t* _ctx) {
  struct kk_std_core__ctail_lift17276_map_peek_fun20555__t* _self = kk_function_as(struct kk_std_core__ctail_lift17276_map_peek_fun20555__t*, _fself);
  kk_function_t f1 = _self->f1; /* (value : 2769, rest : list<2769>) -> 2771 2770 */
  kk_std_core__list yy1 = _self->yy1; /* list<2769> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<2770>> */
  kk_drop_match(_self, {kk_function_dup(f1);kk_std_core__list_dup(yy1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_box_t _ctail_18665_175200 = _b_18647; /*2770*/;
  kk_std_core__list _x20556 = kk_std_core__mlift17989_op(_acc0, f1, yy1, _ctail_18665_175200, _ctx); /*list<2770>*/
  return kk_std_core__list_box(_x20556, _ctx);
}

kk_std_core__list kk_std_core__ctail_lift17276_map_peek(kk_function_t f1, kk_std_core__list ys, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,b,e> (f : (value : a, rest : list<a>) -> e b, ys : list<a>, ctail<list<b>>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con20551 = kk_std_core__as_Cons(ys);
    kk_box_t y = _con20551->head;
    kk_std_core__list yy1 = _con20551->tail;
    kk_reuse_t _ru_20360 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      _ru_20360 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(y);
      kk_std_core__list_dup(yy1);
      kk_std_core__list_decref(ys, _ctx);
    }
    kk_box_t x_18083;
    kk_function_t _x20553 = kk_function_dup(f1); /*(value : 2769, rest : list<2769>) -> 2771 2770*/
    kk_std_core__list _x20552 = kk_std_core__list_dup(yy1); /*list<2769>*/
    x_18083 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_std_core__list, kk_context_t*), _x20553, (_x20553, y, _x20552, _ctx)); /*2770*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_20360,kk_context());
      kk_box_drop(x_18083, _ctx);
      kk_box_t _x20554 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_lift17276_map_peek_fun20555(f1, yy1, _acc0, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20554, _ctx);
    }
    {
      kk_std_core__list _ctail_175210 = kk_std_core__list_hole(); /*list<2770>*/;
      kk_std_core__list _ctail_175220 = kk_std_core__new_Cons(_ru_20360, x_18083, _ctail_175210, _ctx); /*list<2770>*/;
      kk_box_t* _b_18659_18653 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_175220)->tail)); /*cfield<list<2770>>*/;
      { // tailcall
        kk_std_core_types__ctail _x20557 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_175220, _ctx)),_b_18659_18653); /*ctail<0>*/
        ys = yy1;
        _acc0 = _x20557;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_function_drop(f1, _ctx);
    kk_box_t _x20558 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20558, _ctx);
  }
}
 
// lifted local: map-peek, mappeek


// lift anonymous function
struct kk_std_core__ctailm_lift17276_map_peek_fun20563__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t f2;
  kk_std_core__list yy2;
};
static kk_box_t kk_std_core__ctailm_lift17276_map_peek_fun20563(kk_function_t _fself, kk_box_t _b_18667, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_lift17276_map_peek_fun20563(kk_function_t _accm0, kk_function_t f2, kk_std_core__list yy2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17276_map_peek_fun20563__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_lift17276_map_peek_fun20563__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_lift17276_map_peek_fun20563, kk_context());
  _self->_accm0 = _accm0;
  _self->f2 = f2;
  _self->yy2 = yy2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_lift17276_map_peek_fun20563(kk_function_t _fself, kk_box_t _b_18667, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17276_map_peek_fun20563__t* _self = kk_function_as(struct kk_std_core__ctailm_lift17276_map_peek_fun20563__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<2770>) -> list<2770> */
  kk_function_t f2 = _self->f2; /* (value : 2769, rest : list<2769>) -> 2771 2770 */
  kk_std_core__list yy2 = _self->yy2; /* list<2769> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(f2);kk_std_core__list_dup(yy2);}, {}, _ctx)
  kk_box_t _ctail_18669_175250 = _b_18667; /*2770*/;
  kk_std_core__list _x20564 = kk_std_core__mlift17990_op(_accm0, f2, yy2, _ctail_18669_175250, _ctx); /*list<2770>*/
  return kk_std_core__list_box(_x20564, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_lift17276_map_peek_fun20566__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t x0_18086;
};
static kk_std_core__list kk_std_core__ctailm_lift17276_map_peek_fun20566(kk_function_t _fself, kk_std_core__list _ctail_175240, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_lift17276_map_peek_fun20566(kk_function_t _accm0, kk_box_t x0_18086, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17276_map_peek_fun20566__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_lift17276_map_peek_fun20566__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_lift17276_map_peek_fun20566, kk_context());
  _self->_accm0 = _accm0;
  _self->x0_18086 = x0_18086;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_lift17276_map_peek_fun20566(kk_function_t _fself, kk_std_core__list _ctail_175240, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17276_map_peek_fun20566__t* _self = kk_function_as(struct kk_std_core__ctailm_lift17276_map_peek_fun20566__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<2770>) -> list<2770> */
  kk_box_t x0_18086 = _self->x0_18086; /* 2770 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(x0_18086);}, {}, _ctx)
  kk_std_core__list _x20567 = kk_std_core__new_Cons(kk_reuse_null, x0_18086, _ctail_175240, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x20567, _ctx));
}

kk_std_core__list kk_std_core__ctailm_lift17276_map_peek(kk_function_t f2, kk_std_core__list ys0, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,b,e> (f : (value : a, rest : list<a>) -> e b, ys : list<a>, (list<b>) -> list<b>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys0)) {
    struct kk_std_core_Cons* _con20559 = kk_std_core__as_Cons(ys0);
    kk_box_t y0 = _con20559->head;
    kk_std_core__list yy2 = _con20559->tail;
    if (kk_likely(kk_std_core__list_is_unique(ys0))) {
      kk_std_core__list_free(ys0, _ctx);
    }
    else {
      kk_box_dup(y0);
      kk_std_core__list_dup(yy2);
      kk_std_core__list_decref(ys0, _ctx);
    }
    kk_box_t x0_18086;
    kk_function_t _x20561 = kk_function_dup(f2); /*(value : 2769, rest : list<2769>) -> 2771 2770*/
    kk_std_core__list _x20560 = kk_std_core__list_dup(yy2); /*list<2769>*/
    x0_18086 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_std_core__list, kk_context_t*), _x20561, (_x20561, y0, _x20560, _ctx)); /*2770*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x0_18086, _ctx);
      kk_box_t _x20562 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_lift17276_map_peek_fun20563(_accm0, f2, yy2, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20562, _ctx);
    }
    { // tailcall
      kk_function_t _x20565 = kk_std_core__new_ctailm_lift17276_map_peek_fun20566(_accm0, x0_18086, _ctx); /*(list<2770>) -> list<2770>*/
      ys0 = yy2;
      _accm0 = _x20565;
      goto kk__tailcall;
    }
  }
  {
    kk_function_drop(f2, _ctx);
    return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
  }
}
 
// lifted local: map-peek, mappeek


// lift anonymous function
struct kk_std_core__lift17276_map_peek_fun20569__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core__lift17276_map_peek_fun20569(kk_function_t _fself, kk_std_core__list _ctail_17523, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17276_map_peek_fun20569(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core__lift17276_map_peek_fun20569, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core__lift17276_map_peek_fun20569(kk_function_t _fself, kk_std_core__list _ctail_17523, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17523;
}

kk_std_core__list kk_std_core__lift17276_map_peek(kk_function_t f3, kk_std_core__list ys1, kk_context_t* _ctx) { /* forall<a,b,e> (f : (value : a, rest : list<a>) -> e b, ys : list<a>) -> e list<b> */ 
  bool _match_20337 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_20337) {
    kk_std_core_types__ctail _x20568 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_lift17276_map_peek(f3, ys1, _x20568, _ctx);
  }
  {
    return kk_std_core__ctailm_lift17276_map_peek(f3, ys1, kk_std_core__new_lift17276_map_peek_fun20569(_ctx), _ctx);
  }
}

kk_ref_t kk_std_core_trace_enabled;
 
// Compose two funs `f` and `g`.


// lift anonymous function
struct kk_std_core_o_fun20571__t {
  struct kk_function_s _base;
  kk_function_t f;
  kk_function_t g;
};
static kk_box_t kk_std_core_o_fun20571(kk_function_t _fself, kk_box_t x, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_o_fun20571(kk_function_t f, kk_function_t g, kk_context_t* _ctx) {
  struct kk_std_core_o_fun20571__t* _self = kk_function_alloc_as(struct kk_std_core_o_fun20571__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_o_fun20571, kk_context());
  _self->f = f;
  _self->g = g;
  return &_self->_base;
}

static kk_box_t kk_std_core_o_fun20571(kk_function_t _fself, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_o_fun20571__t* _self = kk_function_as(struct kk_std_core_o_fun20571__t*, _fself);
  kk_function_t f = _self->f; /* (2812) -> 2809 2810 */
  kk_function_t g = _self->g; /* (2808) -> 2809 2812 */
  kk_drop_match(_self, {kk_function_dup(f);kk_function_dup(g);}, {}, _ctx)
  kk_box_t x0_18089 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), g, (g, x, _ctx)); /*2812*/;
  if (kk_yielding(kk_context())) {
    kk_box_drop(x0_18089, _ctx);
    return kk_std_core_hnd_yield_extend(f, _ctx);
  }
  {
    return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, x0_18089, _ctx));
  }
}

kk_function_t kk_std_core_o(kk_function_t f, kk_function_t g, kk_context_t* _ctx) { /* forall<a,b,c,e> (f : (a) -> e b, g : (c) -> e a) -> ((x : c) -> e b) */ 
  return kk_std_core_new_o_fun20571(f, g, _ctx);
}
 
// monadic lift

kk_box_t kk_std_core__mlift17991_once(kk_function_t calc, kk_ref_t r, kk_std_core_types__maybe _y_17656, kk_context_t* _ctx) { /* forall<_h,_e,a> (calc : () -> a, r : ref<_h,maybe<a>>, maybe<a>) -> <read<_h>,write<_h>,div|_e> a */ 
  if (kk_std_core_types__is_Just(_y_17656)) {
    kk_box_t x = _y_17656._cons.Just.value;
    kk_ref_drop(r, _ctx);
    kk_function_drop(calc, _ctx);
    return x;
  }
  {
    kk_box_t x0 = kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), calc, (calc, _ctx)); /*2933*/;
    kk_unit_t __ = kk_Unit;
    kk_unit_t _brw_20335 = kk_Unit;
    kk_box_t _x20573;
    kk_std_core_types__maybe _x20574;
    kk_box_t _x20575 = kk_box_dup(x0); /*2933*/
    _x20574 = kk_std_core_types__new_Just(_x20575, _ctx); /*maybe<1034>*/
    _x20573 = kk_std_core_types__maybe_box(_x20574, _ctx); /*1000*/
    kk_ref_set_borrow(r,_x20573,kk_context());
    kk_ref_drop(r, _ctx);
    _brw_20335;
    return x0;
  }
}
 
// Given a total function to calculate a value `:a`, return
// a total function that only calculates the value once and then
// returns the cached result.


// lift anonymous function
struct kk_std_core_once_fun20576__t {
  struct kk_function_s _base;
  kk_function_t calc;
  kk_ref_t r;
};
static kk_box_t kk_std_core_once_fun20576(kk_function_t _fself, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_once_fun20576(kk_function_t calc, kk_ref_t r, kk_context_t* _ctx) {
  struct kk_std_core_once_fun20576__t* _self = kk_function_alloc_as(struct kk_std_core_once_fun20576__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_once_fun20576, kk_context());
  _self->calc = calc;
  _self->r = r;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_once_fun20579__t {
  struct kk_function_s _base;
  kk_function_t calc;
  kk_ref_t r;
};
static kk_box_t kk_std_core_once_fun20579(kk_function_t _fself, kk_std_core_types__maybe _y_17656, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_once_fun20579(kk_function_t calc, kk_ref_t r, kk_context_t* _ctx) {
  struct kk_std_core_once_fun20579__t* _self = kk_function_alloc_as(struct kk_std_core_once_fun20579__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_once_fun20579, kk_context());
  _self->calc = calc;
  _self->r = r;
  return &_self->_base;
}

static kk_box_t kk_std_core_once_fun20579(kk_function_t _fself, kk_std_core_types__maybe _y_17656, kk_context_t* _ctx) {
  struct kk_std_core_once_fun20579__t* _self = kk_function_as(struct kk_std_core_once_fun20579__t*, _fself);
  kk_function_t calc = _self->calc; /* () -> 2933 */
  kk_ref_t r = _self->r; /* ref<_2842,maybe<2933>> */
  kk_drop_match(_self, {kk_function_dup(calc);kk_ref_dup(r);}, {}, _ctx)
  if (kk_std_core_types__is_Just(_y_17656)) {
    kk_box_t x0 = _y_17656._cons.Just.value;
    kk_ref_drop(r, _ctx);
    kk_function_drop(calc, _ctx);
    return x0;
  }
  {
    kk_box_t x00 = kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), calc, (calc, _ctx)); /*2933*/;
    kk_unit_t __ = kk_Unit;
    kk_unit_t _brw_20334 = kk_Unit;
    kk_box_t _x20580;
    kk_std_core_types__maybe _x20581;
    kk_box_t _x20582 = kk_box_dup(x00); /*2933*/
    _x20581 = kk_std_core_types__new_Just(_x20582, _ctx); /*maybe<1034>*/
    _x20580 = kk_std_core_types__maybe_box(_x20581, _ctx); /*1000*/
    kk_ref_set_borrow(r,_x20580,kk_context());
    kk_ref_drop(r, _ctx);
    _brw_20334;
    return x00;
  }
}


// lift anonymous function
struct kk_std_core_once_fun20583__t {
  struct kk_function_s _base;
  kk_function_t next0_18094;
};
static kk_box_t kk_std_core_once_fun20583(kk_function_t _fself, kk_box_t _b_18686, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_once_fun20583(kk_function_t next0_18094, kk_context_t* _ctx) {
  struct kk_std_core_once_fun20583__t* _self = kk_function_alloc_as(struct kk_std_core_once_fun20583__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_once_fun20583, kk_context());
  _self->next0_18094 = next0_18094;
  return &_self->_base;
}

static kk_box_t kk_std_core_once_fun20583(kk_function_t _fself, kk_box_t _b_18686, kk_context_t* _ctx) {
  struct kk_std_core_once_fun20583__t* _self = kk_function_as(struct kk_std_core_once_fun20583__t*, _fself);
  kk_function_t next0_18094 = _self->next0_18094; /* (maybe<2933>) -> <read<_2842>,write<_2842>,div|_2925> 2933 */
  kk_drop_match(_self, {kk_function_dup(next0_18094);}, {}, _ctx)
  kk_std_core_types__maybe _x20584 = kk_std_core_types__maybe_unbox(_b_18686, _ctx); /*maybe<2933>*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_types__maybe, kk_context_t*), next0_18094, (next0_18094, _x20584, _ctx));
}
static kk_box_t kk_std_core_once_fun20576(kk_function_t _fself, kk_context_t* _ctx) {
  struct kk_std_core_once_fun20576__t* _self = kk_function_as(struct kk_std_core_once_fun20576__t*, _fself);
  kk_function_t calc = _self->calc; /* () -> 2933 */
  kk_ref_t r = _self->r; /* ref<_2842,maybe<2933>> */
  kk_drop_match(_self, {kk_function_dup(calc);kk_ref_dup(r);}, {}, _ctx)
  kk_std_core_types__maybe x_18093;
  kk_box_t _x20577;
  kk_ref_t _x20578 = kk_ref_dup(r); /*ref<_2842,maybe<2933>>*/
  _x20577 = kk_ref_get(_x20578,kk_context()); /*1001*/
  x_18093 = kk_std_core_types__maybe_unbox(_x20577, _ctx); /*maybe<2933>*/
  kk_function_t next0_18094 = kk_std_core_new_once_fun20579(calc, r, _ctx); /*(maybe<2933>) -> <read<_2842>,write<_2842>,div|_2925> 2933*/;
  if (kk_yielding(kk_context())) {
    kk_std_core_types__maybe_drop(x_18093, _ctx);
    return kk_std_core_hnd_yield_extend(kk_std_core_new_once_fun20583(next0_18094, _ctx), _ctx);
  }
  {
    return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_types__maybe, kk_context_t*), next0_18094, (next0_18094, x_18093, _ctx));
  }
}

kk_function_t kk_std_core_once(kk_function_t calc, kk_context_t* _ctx) { /* forall<a> (calc : () -> a) -> (() -> a) */ 
  kk_ref_t r = kk_ref_alloc((kk_std_core_types__maybe_box(kk_std_core_types__new_Nothing(_ctx), _ctx)),kk_context()); /*ref<_2842,maybe<2933>>*/;
  return kk_std_core_new_once_fun20576(calc, r, _ctx);
}

kk_std_core_types__maybe kk_std_core_xparse_int(kk_string_t s, bool hex, kk_context_t* _ctx) { /* (s : string, hex : bool) -> maybe<int> */ 
  return kk_integer_xparse(s,hex,kk_context());
}
 
// Internal: used for value effects
// TODO: revisit value effects codegen

kk_box_t kk_std_core_phantom(kk_context_t* _ctx) { /* forall<a> () -> a */ 
  return kk_box_null;
}
 
// Convert a string to lower-case

kk_string_t kk_std_core_to_lower(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> string */ 
  return kk_string_to_lower(s,kk_context());
}

kk_unit_t kk_std_core_xtrace(kk_string_t message0, kk_context_t* _ctx) { /* (message : string) -> () */ 
  kk_trace(message0,kk_context()); return kk_Unit;
}

kk_unit_t kk_std_core_xtrace_any(kk_string_t message0, kk_box_t x, kk_context_t* _ctx) { /* forall<a> (message : string, x : a) -> () */ 
  kk_trace_any(message0,x,kk_context()); return kk_Unit;
}
 
// clamp an `:int` to fit in an `:int8` but interpret the `:int` as an unsigned 8-bit value,
// and clamp between 0 and 255.

int8_t kk_std_core_uint8(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> int8 */ 
  return kk_integer_clamp_byte(i,kk_context());
}

kk_ref_t kk_std_core_unique_count;

kk_vector_t kk_std_core_unvlist(kk_std_core__list xs, kk_context_t* _ctx) { /* forall<a> (xs : list<a>) -> vector<a> */ 
  return kk_list_to_vector(xs,kk_context());
}
 
// Create a new vector of length `n`  with initial elements given by function `f` .

kk_vector_t kk_std_core_vector_initz(kk_ssize_t n, kk_function_t f, kk_context_t* _ctx) { /* forall<a> (n : ssize_t, f : (ssize_t) -> a) -> vector<a> */ 
  return kk_vector_init(n,f,kk_context());
}

bool kk_std_core__lp__excl__eq__4_rp_(kk_std_core_types__order x, kk_std_core_types__order y, kk_context_t* _ctx) { /* (x : order, y : order) -> bool */ 
  kk_integer_t _brw_20331;
  if (kk_std_core_types__is_Lt(x)) {
    _brw_20331 = kk_integer_from_small(-1); /*int*/
    goto _match20585;
  }
  if (kk_std_core_types__is_Eq(x)) {
    _brw_20331 = kk_integer_from_small(0); /*int*/
    goto _match20585;
  }
  {
    _brw_20331 = kk_integer_from_small(1); /*int*/
  }
  _match20585: ;
  kk_integer_t _brw_20330;
  if (kk_std_core_types__is_Lt(y)) {
    _brw_20330 = kk_integer_from_small(-1); /*int*/
    goto _match20586;
  }
  if (kk_std_core_types__is_Eq(y)) {
    _brw_20330 = kk_integer_from_small(0); /*int*/
    goto _match20586;
  }
  {
    _brw_20330 = kk_integer_from_small(1); /*int*/
  }
  _match20586: ;
  bool _brw_20332 = kk_integer_neq_borrow(_brw_20331,_brw_20330,kk_context()); /*bool*/;
  kk_integer_drop(_brw_20331, _ctx);
  kk_integer_drop(_brw_20330, _ctx);
  return _brw_20332;
}
 
// Append two lists.

kk_std_core__list kk_std_core__ctail_append(kk_std_core__list xs, kk_std_core__list ys, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, ys : list<a>, ctail<list<a>>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con20588 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con20588->head;
    kk_std_core__list xx = _con20588->tail;
    kk_reuse_t _ru_20362 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      _ru_20362 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_std_core__list _ctail_17526 = kk_std_core__list_hole(); /*list<3224>*/;
    kk_std_core__list _ctail_17527 = kk_std_core__new_Cons(_ru_20362, x, _ctail_17526, _ctx); /*list<3224>*/;
    kk_box_t* _b_18700_18695 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17527)->tail)); /*cfield<list<3224>>*/;
    { // tailcall
      kk_std_core_types__ctail _x20589 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17527, _ctx)),_b_18700_18695); /*ctail<0>*/
      xs = xx;
      _acc = _x20589;
      goto kk__tailcall;
    }
  }
  {
    kk_box_t _x20590 = kk_ctail_resolve(_acc,(kk_std_core__list_box(ys, _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20590, _ctx);
  }
}
 
// Append two lists.

kk_std_core__list kk_std_core_append(kk_std_core__list xs0, kk_std_core__list ys0, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, ys : list<a>) -> list<a> */ 
  kk_std_core_types__ctail _x20591 = kk_ctail_nil(); /*ctail<0>*/
  return kk_std_core__ctail_append(xs0, ys0, _x20591, _ctx);
}
 
// Append two strings.

kk_string_t kk_std_core__lp__plus__plus__1_rp_(kk_string_t x, kk_string_t y, kk_context_t* _ctx) { /* (x : string, y : string) -> string */ 
  return kk_string_cat(x,y,kk_context());
}

kk_unit_t kk_std_core_printsln(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> console () */ 
  kk_std_core_types__maybe _match_20329;
  kk_box_t _x20593;
  kk_ref_t _x20594 = kk_ref_dup(kk_std_core_redirect); /*ref<global,maybe<(string) -> console ()>>*/
  _x20593 = kk_ref_get(_x20594,kk_context()); /*1001*/
  _match_20329 = kk_std_core_types__maybe_unbox(_x20593, _ctx); /*maybe<(string) -> console ()>*/
  if (kk_std_core_types__is_Nothing(_match_20329)) {
    kk_std_core_xprintsln(s, _ctx); return kk_Unit;
  }
  {
    kk_box_t _fun_unbox_x18710 = _match_20329._cons.Just.value;
    kk_string_t _b_18713;
    kk_string_t _x20595;
    kk_define_string_literal(, _s20596, 1, "\n")
    _x20595 = kk_string_dup(_s20596); /*string*/
    _b_18713 = kk_std_core__lp__plus__plus__1_rp_(s, _x20595, _ctx); /*string*/
    kk_box_t _x20597;
    kk_function_t _x20598 = kk_function_unbox(_fun_unbox_x18710); /*(18711) -> console 18712*/
    _x20597 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x20598, (_x20598, kk_string_box(_b_18713), _ctx)); /*18712*/
    kk_unit_unbox(_x20597); return kk_Unit;
  }
}

bool kk_std_core__lp__eq__eq__4_rp_(kk_std_core_types__order x, kk_std_core_types__order y, kk_context_t* _ctx) { /* (x : order, y : order) -> bool */ 
  kk_integer_t _brw_20327;
  if (kk_std_core_types__is_Lt(x)) {
    _brw_20327 = kk_integer_from_small(-1); /*int*/
    goto _match20599;
  }
  if (kk_std_core_types__is_Eq(x)) {
    _brw_20327 = kk_integer_from_small(0); /*int*/
    goto _match20599;
  }
  {
    _brw_20327 = kk_integer_from_small(1); /*int*/
  }
  _match20599: ;
  kk_integer_t _brw_20326;
  if (kk_std_core_types__is_Lt(y)) {
    _brw_20326 = kk_integer_from_small(-1); /*int*/
    goto _match20600;
  }
  if (kk_std_core_types__is_Eq(y)) {
    _brw_20326 = kk_integer_from_small(0); /*int*/
    goto _match20600;
  }
  {
    _brw_20326 = kk_integer_from_small(1); /*int*/
  }
  _match20600: ;
  bool _brw_20328 = kk_integer_eq_borrow(_brw_20327,_brw_20326,kk_context()); /*bool*/;
  kk_integer_drop(_brw_20327, _ctx);
  kk_integer_drop(_brw_20326, _ctx);
  return _brw_20328;
}

bool kk_std_core__lp__lt__5_rp_(kk_std_core_types__order x, kk_std_core_types__order y, kk_context_t* _ctx) { /* (x : order, y : order) -> bool */ 
  kk_integer_t _brw_20324;
  if (kk_std_core_types__is_Lt(x)) {
    _brw_20324 = kk_integer_from_small(-1); /*int*/
    goto _match20601;
  }
  if (kk_std_core_types__is_Eq(x)) {
    _brw_20324 = kk_integer_from_small(0); /*int*/
    goto _match20601;
  }
  {
    _brw_20324 = kk_integer_from_small(1); /*int*/
  }
  _match20601: ;
  kk_integer_t _brw_20323;
  if (kk_std_core_types__is_Lt(y)) {
    _brw_20323 = kk_integer_from_small(-1); /*int*/
    goto _match20602;
  }
  if (kk_std_core_types__is_Eq(y)) {
    _brw_20323 = kk_integer_from_small(0); /*int*/
    goto _match20602;
  }
  {
    _brw_20323 = kk_integer_from_small(1); /*int*/
  }
  _match20602: ;
  bool _brw_20325 = kk_integer_lt_borrow(_brw_20324,_brw_20323,kk_context()); /*bool*/;
  kk_integer_drop(_brw_20324, _ctx);
  kk_integer_drop(_brw_20323, _ctx);
  return _brw_20325;
}

kk_std_core_types__order kk_std_core_order(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> order */ 
  bool _match_20320 = kk_integer_lt_borrow(i,(kk_integer_from_small(0)),kk_context()); /*bool*/;
  if (_match_20320) {
    kk_integer_drop(i, _ctx);
    return kk_std_core_types__new_Lt(_ctx);
  }
  {
    bool _match_20321;
    bool _brw_20322 = kk_integer_gt_borrow(i,(kk_integer_from_small(0)),kk_context()); /*bool*/;
    kk_integer_drop(i, _ctx);
    _match_20321 = _brw_20322; /*bool*/
    if (_match_20321) {
      return kk_std_core_types__new_Gt(_ctx);
    }
    {
      return kk_std_core_types__new_Eq(_ctx);
    }
  }
}
 
// Compare two strings.
// Uses the character codes directly for comparison

kk_std_core_types__order kk_std_core_compare_3(kk_string_t x, kk_string_t y, kk_context_t* _ctx) { /* (x : string, y : string) -> order */ 
  kk_integer_t i_16964 = kk_std_core_string_compare(x, y, _ctx); /*int*/;
  bool _match_20317 = kk_integer_lt_borrow(i_16964,(kk_integer_from_small(0)),kk_context()); /*bool*/;
  if (_match_20317) {
    kk_integer_drop(i_16964, _ctx);
    return kk_std_core_types__new_Lt(_ctx);
  }
  {
    bool _match_20318;
    bool _brw_20319 = kk_integer_gt_borrow(i_16964,(kk_integer_from_small(0)),kk_context()); /*bool*/;
    kk_integer_drop(i_16964, _ctx);
    _match_20318 = _brw_20319; /*bool*/
    if (_match_20318) {
      return kk_std_core_types__new_Gt(_ctx);
    }
    {
      return kk_std_core_types__new_Eq(_ctx);
    }
  }
}

bool kk_std_core__lp__gt__3_rp_(kk_std_core_types__order x, kk_std_core_types__order y, kk_context_t* _ctx) { /* (x : order, y : order) -> bool */ 
  kk_integer_t _brw_20313;
  if (kk_std_core_types__is_Lt(x)) {
    _brw_20313 = kk_integer_from_small(-1); /*int*/
    goto _match20604;
  }
  if (kk_std_core_types__is_Eq(x)) {
    _brw_20313 = kk_integer_from_small(0); /*int*/
    goto _match20604;
  }
  {
    _brw_20313 = kk_integer_from_small(1); /*int*/
  }
  _match20604: ;
  kk_integer_t _brw_20312;
  if (kk_std_core_types__is_Lt(y)) {
    _brw_20312 = kk_integer_from_small(-1); /*int*/
    goto _match20605;
  }
  if (kk_std_core_types__is_Eq(y)) {
    _brw_20312 = kk_integer_from_small(0); /*int*/
    goto _match20605;
  }
  {
    _brw_20312 = kk_integer_from_small(1); /*int*/
  }
  _match20605: ;
  bool _brw_20314 = kk_integer_gt_borrow(_brw_20313,_brw_20312,kk_context()); /*bool*/;
  kk_integer_drop(_brw_20313, _ctx);
  kk_integer_drop(_brw_20312, _ctx);
  return _brw_20314;
}

kk_std_core_types__order kk_std_core_compare_2(bool x, bool y, kk_context_t* _ctx) { /* (x : bool, y : bool) -> order */ 
  if (x) {
    if (x) {
      if (y) {
        return kk_std_core_types__new_Eq(_ctx);
      }
      {
        return kk_std_core_types__new_Gt(_ctx);
      }
    }
    {
      return kk_std_core_types__new_Eq(_ctx);
    }
  }
  if (y) {
    return kk_std_core_types__new_Lt(_ctx);
  }
  if (x) {
    if (y) {
      return kk_std_core_types__new_Eq(_ctx);
    }
    {
      return kk_std_core_types__new_Gt(_ctx);
    }
  }
  {
    return kk_std_core_types__new_Eq(_ctx);
  }
}

bool kk_std_core__lp__gt__eq__4_rp_(kk_std_core_types__order x, kk_std_core_types__order y, kk_context_t* _ctx) { /* (x : order, y : order) -> bool */ 
  kk_integer_t _brw_20306;
  if (kk_std_core_types__is_Lt(x)) {
    _brw_20306 = kk_integer_from_small(-1); /*int*/
    goto _match20607;
  }
  if (kk_std_core_types__is_Eq(x)) {
    _brw_20306 = kk_integer_from_small(0); /*int*/
    goto _match20607;
  }
  {
    _brw_20306 = kk_integer_from_small(1); /*int*/
  }
  _match20607: ;
  kk_integer_t _brw_20305;
  if (kk_std_core_types__is_Lt(y)) {
    _brw_20305 = kk_integer_from_small(-1); /*int*/
    goto _match20608;
  }
  if (kk_std_core_types__is_Eq(y)) {
    _brw_20305 = kk_integer_from_small(0); /*int*/
    goto _match20608;
  }
  {
    _brw_20305 = kk_integer_from_small(1); /*int*/
  }
  _match20608: ;
  bool _brw_20307 = kk_integer_gte_borrow(_brw_20306,_brw_20305,kk_context()); /*bool*/;
  kk_integer_drop(_brw_20306, _ctx);
  kk_integer_drop(_brw_20305, _ctx);
  return _brw_20307;
}
 
// lifted local: joinsep, join-acc

kk_string_t kk_std_core__lift17277_joinsep(kk_string_t sep, kk_std_core__list ys, kk_string_t acc, kk_context_t* _ctx) { /* (sep : string, ys : list<string>, acc : string) -> string */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con20610 = kk_std_core__as_Cons(ys);
    kk_box_t _box_x18715 = _con20610->head;
    kk_std_core__list yy = _con20610->tail;
    kk_string_t y = kk_string_unbox(_box_x18715);
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      kk_std_core__list_free(ys, _ctx);
    }
    else {
      kk_string_dup(y);
      kk_std_core__list_dup(yy);
      kk_std_core__list_decref(ys, _ctx);
    }
    kk_string_t acc0_17447;
    kk_string_t _x20612;
    kk_string_t _x20613 = kk_string_dup(sep); /*string*/
    _x20612 = kk_std_core__lp__plus__plus__1_rp_(_x20613, y, _ctx); /*string*/
    acc0_17447 = kk_std_core__lp__plus__plus__1_rp_(acc, _x20612, _ctx); /*string*/
    { // tailcall
      ys = yy;
      acc = acc0_17447;
      goto kk__tailcall;
    }
  }
  {
    kk_string_drop(sep, _ctx);
    return acc;
  }
}
 
// Concatenate all strings in a list

kk_string_t kk_std_core_joinsep(kk_std_core__list xs, kk_string_t sep, kk_context_t* _ctx) { /* (xs : list<string>, sep : string) -> string */ 
  if (kk_std_core__is_Nil(xs)) {
    kk_string_drop(sep, _ctx);
    return kk_string_empty();
  }
  {
    struct kk_std_core_Cons* _con20615 = kk_std_core__as_Cons(xs);
    kk_box_t _box_x18716 = _con20615->head;
    kk_std_core__list xx = _con20615->tail;
    kk_string_t x = kk_string_unbox(_box_x18716);
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_string_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    return kk_std_core__lift17277_joinsep(sep, xx, x, _ctx);
  }
}
 
// lifted local: join.2, join-acc

kk_string_t kk_std_core__lift17278_join_2(kk_std_core__list ys, kk_string_t acc, kk_context_t* _ctx) { /* (ys : list<string>, acc : string) -> string */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con20617 = kk_std_core__as_Cons(ys);
    kk_box_t _box_x18717 = _con20617->head;
    kk_std_core__list yy = _con20617->tail;
    kk_string_t y = kk_string_unbox(_box_x18717);
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      kk_std_core__list_free(ys, _ctx);
    }
    else {
      kk_string_dup(y);
      kk_std_core__list_dup(yy);
      kk_std_core__list_decref(ys, _ctx);
    }
    { // tailcall
      kk_string_t _x20619;
      kk_string_t _x20620;
      kk_string_t _x20621 = kk_string_empty(); /*string*/
      _x20620 = kk_std_core__lp__plus__plus__1_rp_(_x20621, y, _ctx); /*string*/
      _x20619 = kk_std_core__lp__plus__plus__1_rp_(acc, _x20620, _ctx); /*string*/
      ys = yy;
      acc = _x20619;
      goto kk__tailcall;
    }
  }
  {
    return acc;
  }
}
 
// Concatenate all strings in a list

kk_string_t kk_std_core_join_2(kk_std_core__list xs, kk_context_t* _ctx) { /* (xs : list<string>) -> string */ 
  if (kk_std_core__is_Nil(xs)) {
    return kk_string_empty();
  }
  {
    struct kk_std_core_Cons* _con20624 = kk_std_core__as_Cons(xs);
    kk_box_t _box_x18718 = _con20624->head;
    kk_std_core__list xx = _con20624->tail;
    kk_string_t x = kk_string_unbox(_box_x18718);
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_string_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    return kk_std_core__lift17278_join_2(xx, x, _ctx);
  }
}
 
// Concatenate all strings in a list using a specific separator

kk_string_t kk_std_core_join_3(kk_std_core__list xs, kk_string_t sep, kk_context_t* _ctx) { /* (xs : list<string>, sep : string) -> string */ 
  if (kk_std_core__is_Nil(xs)) {
    kk_string_drop(sep, _ctx);
    return kk_string_empty();
  }
  {
    struct kk_std_core_Cons* _con20627 = kk_std_core__as_Cons(xs);
    kk_box_t _box_x18719 = _con20627->head;
    kk_std_core__list xx = _con20627->tail;
    kk_string_t x = kk_string_unbox(_box_x18719);
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_string_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    return kk_std_core__lift17277_joinsep(sep, xx, x, _ctx);
  }
}

bool kk_std_core__lp__lt__eq__5_rp_(kk_std_core_types__order x, kk_std_core_types__order y, kk_context_t* _ctx) { /* (x : order, y : order) -> bool */ 
  kk_integer_t _brw_20301;
  if (kk_std_core_types__is_Lt(x)) {
    _brw_20301 = kk_integer_from_small(-1); /*int*/
    goto _match20629;
  }
  if (kk_std_core_types__is_Eq(x)) {
    _brw_20301 = kk_integer_from_small(0); /*int*/
    goto _match20629;
  }
  {
    _brw_20301 = kk_integer_from_small(1); /*int*/
  }
  _match20629: ;
  kk_integer_t _brw_20300;
  if (kk_std_core_types__is_Lt(y)) {
    _brw_20300 = kk_integer_from_small(-1); /*int*/
    goto _match20630;
  }
  if (kk_std_core_types__is_Eq(y)) {
    _brw_20300 = kk_integer_from_small(0); /*int*/
    goto _match20630;
  }
  {
    _brw_20300 = kk_integer_from_small(1); /*int*/
  }
  _match20630: ;
  bool _brw_20302 = kk_integer_lte_borrow(_brw_20301,_brw_20300,kk_context()); /*bool*/;
  kk_integer_drop(_brw_20301, _ctx);
  kk_integer_drop(_brw_20300, _ctx);
  return _brw_20302;
}

kk_ssize_t kk_std_core_decr_1(kk_ssize_t i, kk_context_t* _ctx) { /* (i : ssize_t) -> ssize_t */ 
  return (i - 1);
}

kk_ssize_t kk_std_core_incr_1(kk_ssize_t i, kk_context_t* _ctx) { /* (i : ssize_t) -> ssize_t */ 
  return (i + 1);
}
 
// monadic lift

kk_unit_t kk_std_core__mlift17992_op(kk_function_t action, kk_ssize_t end, kk_ssize_t i, kk_unit_t wild__, kk_context_t* _ctx) { /* forall<e> (action : (ssize_t) -> e (), end : ssize_t, i : ssize_t, wild_ : ()) -> e () */ 
  kk_ssize_t i0_17450 = kk_std_core_incr_1(i, _ctx); /*ssize_t*/;
  kk_std_core__lift17279_forz(action, end, i0_17450, _ctx); return kk_Unit;
}
 
// lifted local: forz, rep


// lift anonymous function
struct kk_std_core__lift17279_forz_fun20634__t {
  struct kk_function_s _base;
  kk_function_t action0;
  kk_ssize_t end0;
  kk_ssize_t i0;
};
static kk_box_t kk_std_core__lift17279_forz_fun20634(kk_function_t _fself, kk_box_t _b_18721, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17279_forz_fun20634(kk_function_t action0, kk_ssize_t end0, kk_ssize_t i0, kk_context_t* _ctx) {
  struct kk_std_core__lift17279_forz_fun20634__t* _self = kk_function_alloc_as(struct kk_std_core__lift17279_forz_fun20634__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift17279_forz_fun20634, kk_context());
  _self->action0 = action0;
  _self->end0 = end0;
  _self->i0 = i0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift17279_forz_fun20634(kk_function_t _fself, kk_box_t _b_18721, kk_context_t* _ctx) {
  struct kk_std_core__lift17279_forz_fun20634__t* _self = kk_function_as(struct kk_std_core__lift17279_forz_fun20634__t*, _fself);
  kk_function_t action0 = _self->action0; /* (ssize_t) -> 4394 () */
  kk_ssize_t end0 = _self->end0; /* ssize_t */
  kk_ssize_t i0 = _self->i0; /* ssize_t */
  kk_drop_match(_self, {kk_function_dup(action0);;;}, {}, _ctx)
  kk_unit_t wild__0_18723 = kk_Unit;
  kk_unit_unbox(_b_18721);
  kk_unit_t _x20635 = kk_Unit;
  kk_std_core__mlift17992_op(action0, end0, i0, wild__0_18723, _ctx);
  return kk_unit_box(_x20635);
}

kk_unit_t kk_std_core__lift17279_forz(kk_function_t action0, kk_ssize_t end0, kk_ssize_t i0, kk_context_t* _ctx) { /* forall<e> (action : (ssize_t) -> e (), end : ssize_t, i : ssize_t) -> e () */ 
  kk__tailcall: ;
  bool _match_20296 = (i0 <= end0); /*bool*/;
  if (_match_20296) {
    kk_unit_t x_18101 = kk_Unit;
    kk_function_t _x20632 = kk_function_dup(action0); /*(ssize_t) -> 4394 ()*/
    kk_function_call(kk_unit_t, (kk_function_t, kk_ssize_t, kk_context_t*), _x20632, (_x20632, i0, _ctx));
    if (kk_yielding(kk_context())) {
      kk_box_t _x20633 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift17279_forz_fun20634(action0, end0, i0, _ctx), _ctx); /*3926*/
      kk_unit_unbox(_x20633); return kk_Unit;
    }
    {
      kk_ssize_t i0_174500 = kk_std_core_incr_1(i0, _ctx); /*ssize_t*/;
      { // tailcall
        i0 = i0_174500;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_function_drop(action0, _ctx);
    kk_Unit; return kk_Unit;
  }
}
 
// monadic lift

kk_unit_t kk_std_core__mlift17993_op(kk_ssize_t end_17007, kk_function_t f, kk_ssize_t i, kk_vector_t v, kk_unit_t wild__, kk_context_t* _ctx) { /* forall<a,e> (end.17007 : ssize_t, f : (ssize_t, a) -> e (), i : ssize_t, v : vector<a>, wild_ : ()) -> e () */ 
  kk_ssize_t i0_17451 = kk_std_core_incr_1(i, _ctx); /*ssize_t*/;
  kk_std_core__lift17280_foreach_indexedz(end_17007, f, v, i0_17451, _ctx); return kk_Unit;
}
 
// lifted local: foreach-indexedz, rep


// lift anonymous function
struct kk_std_core__lift17280_foreach_indexedz_fun20639__t {
  struct kk_function_s _base;
  kk_function_t f0;
  kk_vector_t v0;
  kk_ssize_t end_170070;
  kk_ssize_t i0;
};
static kk_box_t kk_std_core__lift17280_foreach_indexedz_fun20639(kk_function_t _fself, kk_box_t _b_18725, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17280_foreach_indexedz_fun20639(kk_function_t f0, kk_vector_t v0, kk_ssize_t end_170070, kk_ssize_t i0, kk_context_t* _ctx) {
  struct kk_std_core__lift17280_foreach_indexedz_fun20639__t* _self = kk_function_alloc_as(struct kk_std_core__lift17280_foreach_indexedz_fun20639__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift17280_foreach_indexedz_fun20639, kk_context());
  _self->f0 = f0;
  _self->v0 = v0;
  _self->end_170070 = end_170070;
  _self->i0 = i0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift17280_foreach_indexedz_fun20639(kk_function_t _fself, kk_box_t _b_18725, kk_context_t* _ctx) {
  struct kk_std_core__lift17280_foreach_indexedz_fun20639__t* _self = kk_function_as(struct kk_std_core__lift17280_foreach_indexedz_fun20639__t*, _fself);
  kk_function_t f0 = _self->f0; /* (ssize_t, 4428) -> 4429 () */
  kk_vector_t v0 = _self->v0; /* vector<4428> */
  kk_ssize_t end_170070 = _self->end_170070; /* ssize_t */
  kk_ssize_t i0 = _self->i0; /* ssize_t */
  kk_drop_match(_self, {kk_function_dup(f0);kk_vector_dup(v0);;;}, {}, _ctx)
  kk_unit_t wild__0_18727 = kk_Unit;
  kk_unit_unbox(_b_18725);
  kk_unit_t _x20640 = kk_Unit;
  kk_std_core__mlift17993_op(end_170070, f0, i0, v0, wild__0_18727, _ctx);
  return kk_unit_box(_x20640);
}

kk_unit_t kk_std_core__lift17280_foreach_indexedz(kk_ssize_t end_170070, kk_function_t f0, kk_vector_t v0, kk_ssize_t i0, kk_context_t* _ctx) { /* forall<a,e> (end.17007 : ssize_t, f : (ssize_t, a) -> e (), v : vector<a>, i : ssize_t) -> e () */ 
  kk__tailcall: ;
  bool _match_20294 = (i0 <= end_170070); /*bool*/;
  if (_match_20294) {
    kk_unit_t x_18104 = kk_Unit;
    kk_function_t _x20637 = kk_function_dup(f0); /*(ssize_t, 4428) -> 4429 ()*/
    kk_box_t _x20636 = kk_vector_at_borrow(v0,i0); /*228*/
    kk_function_call(kk_unit_t, (kk_function_t, kk_ssize_t, kk_box_t, kk_context_t*), _x20637, (_x20637, i0, _x20636, _ctx));
    if (kk_yielding(kk_context())) {
      kk_box_t _x20638 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift17280_foreach_indexedz_fun20639(f0, v0, end_170070, i0, _ctx), _ctx); /*3926*/
      kk_unit_unbox(_x20638); return kk_Unit;
    }
    {
      kk_ssize_t i0_174510 = kk_std_core_incr_1(i0, _ctx); /*ssize_t*/;
      { // tailcall
        i0 = i0_174510;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_vector_drop(v0, _ctx);
    kk_function_drop(f0, _ctx);
    kk_Unit; return kk_Unit;
  }
}
 
// lifted local: length.1, len0

kk_integer_t kk_std_core__lift17281_length_1(kk_std_core__list ys, kk_integer_t acc, kk_context_t* _ctx) { /* forall<a> (ys : list<a>, acc : int) -> int */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con20642 = kk_std_core__as_Cons(ys);
    kk_box_t _pat0 = _con20642->head;
    kk_std_core__list yy = _con20642->tail;
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      kk_box_drop(_pat0, _ctx);
      kk_std_core__list_free(ys, _ctx);
    }
    else {
      kk_std_core__list_dup(yy);
      kk_std_core__list_decref(ys, _ctx);
    }
    { // tailcall
      kk_integer_t _x20643 = kk_integer_add_small_const(acc, 1, _ctx); /*int*/
      ys = yy;
      acc = _x20643;
      goto kk__tailcall;
    }
  }
  {
    return acc;
  }
}
 
// Returns an integer list of increasing elements from `lo`  to `hi`
// (including both `lo`  and `hi` ).
// If `lo > hi`  the function returns the empty list.

kk_std_core__list kk_std_core__ctail_list(kk_integer_t lo, kk_integer_t hi, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* (lo : int, hi : int, ctail<list<int>>) -> total list<int> */ 
  kk__tailcall: ;
  bool _match_20293 = kk_integer_lte_borrow(lo,hi,kk_context()); /*bool*/;
  if (_match_20293) {
    kk_std_core__list _ctail_17528 = kk_std_core__list_hole(); /*list<int>*/;
    kk_std_core__list _ctail_17529;
    kk_box_t _x20645;
    kk_integer_t _x20646 = kk_integer_dup(lo); /*int*/
    _x20645 = kk_integer_box(_x20646); /*61*/
    _ctail_17529 = kk_std_core__new_Cons(kk_reuse_null, _x20645, _ctail_17528, _ctx); /*list<int>*/
    kk_box_t* _b_18742_18737 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17529)->tail)); /*cfield<list<int>>*/;
    { // tailcall
      kk_integer_t _x20647 = kk_integer_add_small_const(lo, 1, _ctx); /*int*/
      kk_std_core_types__ctail _x20648 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17529, _ctx)),_b_18742_18737); /*ctail<0>*/
      lo = _x20647;
      _acc = _x20648;
      goto kk__tailcall;
    }
  }
  {
    kk_integer_drop(lo, _ctx);
    kk_integer_drop(hi, _ctx);
    kk_box_t _x20649 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20649, _ctx);
  }
}
 
// Returns an integer list of increasing elements from `lo`  to `hi`
// (including both `lo`  and `hi` ).
// If `lo > hi`  the function returns the empty list.

kk_std_core__list kk_std_core_list(kk_integer_t lo0, kk_integer_t hi0, kk_context_t* _ctx) { /* (lo : int, hi : int) -> total list<int> */ 
  kk_std_core_types__ctail _x20650 = kk_ctail_nil(); /*ctail<0>*/
  return kk_std_core__ctail_list(lo0, hi0, _x20650, _ctx);
}
 
// Returns an integer list of increasing elements from `lo`  to `hi` with stride `stride`.
// If `lo > hi`  the function returns the empty list.

kk_std_core__list kk_std_core__ctail_list_1(kk_integer_t lo, kk_integer_t hi, kk_integer_t stride, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* (lo : int, hi : int, stride : int, ctail<list<int>>) -> total list<int> */ 
  kk__tailcall: ;
  bool _match_20292 = kk_integer_lte_borrow(lo,hi,kk_context()); /*bool*/;
  if (_match_20292) {
    kk_std_core__list _ctail_17530 = kk_std_core__list_hole(); /*list<int>*/;
    kk_std_core__list _ctail_17531;
    kk_box_t _x20651;
    kk_integer_t _x20652 = kk_integer_dup(lo); /*int*/
    _x20651 = kk_integer_box(_x20652); /*61*/
    _ctail_17531 = kk_std_core__new_Cons(kk_reuse_null, _x20651, _ctail_17530, _ctx); /*list<int>*/
    kk_box_t* _b_18762_18757 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17531)->tail)); /*cfield<list<int>>*/;
    { // tailcall
      kk_integer_t _x20653;
      kk_integer_t _x20654 = kk_integer_dup(stride); /*int*/
      _x20653 = kk_integer_add(lo,_x20654,kk_context()); /*int*/
      kk_std_core_types__ctail _x20655 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17531, _ctx)),_b_18762_18757); /*ctail<0>*/
      lo = _x20653;
      _acc = _x20655;
      goto kk__tailcall;
    }
  }
  {
    kk_integer_drop(stride, _ctx);
    kk_integer_drop(lo, _ctx);
    kk_integer_drop(hi, _ctx);
    kk_box_t _x20656 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20656, _ctx);
  }
}
 
// Returns an integer list of increasing elements from `lo`  to `hi` with stride `stride`.
// If `lo > hi`  the function returns the empty list.

kk_std_core__list kk_std_core_list_1(kk_integer_t lo0, kk_integer_t hi0, kk_integer_t stride0, kk_context_t* _ctx) { /* (lo : int, hi : int, stride : int) -> total list<int> */ 
  kk_std_core_types__ctail _x20657 = kk_ctail_nil(); /*ctail<0>*/
  return kk_std_core__ctail_list_1(lo0, hi0, stride0, _x20657, _ctx);
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift17994_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_integer_t hi, kk_integer_t lo, kk_box_t _ctail_17532, kk_context_t* _ctx) { /* forall<a,e> (ctail<list<a>>, f : (int) -> e a, hi : int, lo : int, a) -> e list<a> */ 
  kk_std_core__list _ctail_17533 = kk_std_core__list_hole(); /*list<4982>*/;
  kk_std_core__list _ctail_17534 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17532, _ctail_17533, _ctx); /*list<4982>*/;
  kk_box_t* _b_18776_18773 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17534)->tail)); /*cfield<list<4982>>*/;
  kk_integer_t _x20658 = kk_integer_add_small_const(lo, 1, _ctx); /*int*/
  kk_std_core_types__ctail _x20659 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17534, _ctx)),_b_18776_18773); /*ctail<0>*/
  return kk_std_core__ctail_list_2(_x20658, hi, f, _x20659, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift17995_op_fun20661__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t _ctail_17537;
};
static kk_std_core__list kk_std_core__mlift17995_op_fun20661(kk_function_t _fself, kk_std_core__list _ctail_17536, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17995_op_fun20661(kk_function_t _accm, kk_box_t _ctail_17537, kk_context_t* _ctx) {
  struct kk_std_core__mlift17995_op_fun20661__t* _self = kk_function_alloc_as(struct kk_std_core__mlift17995_op_fun20661__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17995_op_fun20661, kk_context());
  _self->_accm = _accm;
  _self->_ctail_17537 = _ctail_17537;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift17995_op_fun20661(kk_function_t _fself, kk_std_core__list _ctail_17536, kk_context_t* _ctx) {
  struct kk_std_core__mlift17995_op_fun20661__t* _self = kk_function_as(struct kk_std_core__mlift17995_op_fun20661__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<4982>) -> list<4982> */
  kk_box_t _ctail_17537 = _self->_ctail_17537; /* 4982 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(_ctail_17537);}, {}, _ctx)
  kk_std_core__list _x20662 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17537, _ctail_17536, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x20662, _ctx));
}

kk_std_core__list kk_std_core__mlift17995_op(kk_function_t _accm, kk_function_t f0, kk_integer_t hi0, kk_integer_t lo0, kk_box_t _ctail_17537, kk_context_t* _ctx) { /* forall<a,e> ((list<a>) -> list<a>, f : (int) -> e a, hi : int, lo : int, a) -> e list<a> */ 
  kk_integer_t _x20660 = kk_integer_add_small_const(lo0, 1, _ctx); /*int*/
  return kk_std_core__ctailm_list_2(_x20660, hi0, f0, kk_std_core__new_mlift17995_op_fun20661(_accm, _ctail_17537, _ctx), _ctx);
}
 
// Applies a function `f` to list of increasing elements from `lo`  to `hi`
// (including both `lo`  and `hi` ).
// If `lo > hi`  the function returns the empty list.


// lift anonymous function
struct kk_std_core__ctail_list_fun20666__t_2 {
  struct kk_function_s _base;
  kk_function_t f1;
  kk_integer_t hi1;
  kk_integer_t lo1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_list_fun20666_2(kk_function_t _fself, kk_box_t _b_18781, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_list_fun20666_2(kk_function_t f1, kk_integer_t hi1, kk_integer_t lo1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_list_fun20666__t_2* _self = kk_function_alloc_as(struct kk_std_core__ctail_list_fun20666__t_2, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_list_fun20666_2, kk_context());
  _self->f1 = f1;
  _self->hi1 = hi1;
  _self->lo1 = lo1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_list_fun20666_2(kk_function_t _fself, kk_box_t _b_18781, kk_context_t* _ctx) {
  struct kk_std_core__ctail_list_fun20666__t_2* _self = kk_function_as(struct kk_std_core__ctail_list_fun20666__t_2*, _fself);
  kk_function_t f1 = _self->f1; /* (int) -> 4983 4982 */
  kk_integer_t hi1 = _self->hi1; /* int */
  kk_integer_t lo1 = _self->lo1; /* int */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<4982>> */
  kk_drop_match(_self, {kk_function_dup(f1);kk_integer_dup(hi1);kk_integer_dup(lo1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_box_t _ctail_18799_175320 = _b_18781; /*4982*/;
  kk_std_core__list _x20667 = kk_std_core__mlift17994_op(_acc0, f1, hi1, lo1, _ctail_18799_175320, _ctx); /*list<4982>*/
  return kk_std_core__list_box(_x20667, _ctx);
}

kk_std_core__list kk_std_core__ctail_list_2(kk_integer_t lo1, kk_integer_t hi1, kk_function_t f1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,e> (lo : int, hi : int, f : (int) -> e a, ctail<list<a>>) -> e list<a> */ 
  kk__tailcall: ;
  bool _match_20290 = kk_integer_lte_borrow(lo1,hi1,kk_context()); /*bool*/;
  if (_match_20290) {
    kk_box_t x_18107;
    kk_function_t _x20664 = kk_function_dup(f1); /*(int) -> 4983 4982*/
    kk_integer_t _x20663 = kk_integer_dup(lo1); /*int*/
    x_18107 = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_context_t*), _x20664, (_x20664, _x20663, _ctx)); /*4982*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x_18107, _ctx);
      kk_box_t _x20665 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_list_fun20666_2(f1, hi1, lo1, _acc0, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20665, _ctx);
    }
    {
      kk_std_core__list _ctail_175330 = kk_std_core__list_hole(); /*list<4982>*/;
      kk_std_core__list _ctail_175340 = kk_std_core__new_Cons(kk_reuse_null, x_18107, _ctail_175330, _ctx); /*list<4982>*/;
      kk_box_t* _b_18793_18787 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_175340)->tail)); /*cfield<list<4982>>*/;
      { // tailcall
        kk_integer_t _x20668 = kk_integer_add_small_const(lo1, 1, _ctx); /*int*/
        kk_std_core_types__ctail _x20669 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_175340, _ctx)),_b_18793_18787); /*ctail<0>*/
        lo1 = _x20668;
        _acc0 = _x20669;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_integer_drop(lo1, _ctx);
    kk_integer_drop(hi1, _ctx);
    kk_function_drop(f1, _ctx);
    kk_box_t _x20670 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20670, _ctx);
  }
}
 
// Applies a function `f` to list of increasing elements from `lo`  to `hi`
// (including both `lo`  and `hi` ).
// If `lo > hi`  the function returns the empty list.


// lift anonymous function
struct kk_std_core__ctailm_list_fun20674__t_2 {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t f2;
  kk_integer_t hi2;
  kk_integer_t lo2;
};
static kk_box_t kk_std_core__ctailm_list_fun20674_2(kk_function_t _fself, kk_box_t _b_18801, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_list_fun20674_2(kk_function_t _accm0, kk_function_t f2, kk_integer_t hi2, kk_integer_t lo2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_list_fun20674__t_2* _self = kk_function_alloc_as(struct kk_std_core__ctailm_list_fun20674__t_2, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_list_fun20674_2, kk_context());
  _self->_accm0 = _accm0;
  _self->f2 = f2;
  _self->hi2 = hi2;
  _self->lo2 = lo2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_list_fun20674_2(kk_function_t _fself, kk_box_t _b_18801, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_list_fun20674__t_2* _self = kk_function_as(struct kk_std_core__ctailm_list_fun20674__t_2*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<4982>) -> list<4982> */
  kk_function_t f2 = _self->f2; /* (int) -> 4983 4982 */
  kk_integer_t hi2 = _self->hi2; /* int */
  kk_integer_t lo2 = _self->lo2; /* int */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(f2);kk_integer_dup(hi2);kk_integer_dup(lo2);}, {}, _ctx)
  kk_box_t _ctail_18803_175370 = _b_18801; /*4982*/;
  kk_std_core__list _x20675 = kk_std_core__mlift17995_op(_accm0, f2, hi2, lo2, _ctail_18803_175370, _ctx); /*list<4982>*/
  return kk_std_core__list_box(_x20675, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_list_fun20678__t_2 {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t x0_18110;
};
static kk_std_core__list kk_std_core__ctailm_list_fun20678_2(kk_function_t _fself, kk_std_core__list _ctail_175360, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_list_fun20678_2(kk_function_t _accm0, kk_box_t x0_18110, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_list_fun20678__t_2* _self = kk_function_alloc_as(struct kk_std_core__ctailm_list_fun20678__t_2, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_list_fun20678_2, kk_context());
  _self->_accm0 = _accm0;
  _self->x0_18110 = x0_18110;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_list_fun20678_2(kk_function_t _fself, kk_std_core__list _ctail_175360, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_list_fun20678__t_2* _self = kk_function_as(struct kk_std_core__ctailm_list_fun20678__t_2*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<4982>) -> list<4982> */
  kk_box_t x0_18110 = _self->x0_18110; /* 4982 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(x0_18110);}, {}, _ctx)
  kk_std_core__list _x20679 = kk_std_core__new_Cons(kk_reuse_null, x0_18110, _ctail_175360, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x20679, _ctx));
}

kk_std_core__list kk_std_core__ctailm_list_2(kk_integer_t lo2, kk_integer_t hi2, kk_function_t f2, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,e> (lo : int, hi : int, f : (int) -> e a, (list<a>) -> list<a>) -> e list<a> */ 
  kk__tailcall: ;
  bool _match_20288 = kk_integer_lte_borrow(lo2,hi2,kk_context()); /*bool*/;
  if (_match_20288) {
    kk_box_t x0_18110;
    kk_function_t _x20672 = kk_function_dup(f2); /*(int) -> 4983 4982*/
    kk_integer_t _x20671 = kk_integer_dup(lo2); /*int*/
    x0_18110 = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_context_t*), _x20672, (_x20672, _x20671, _ctx)); /*4982*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x0_18110, _ctx);
      kk_box_t _x20673 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_list_fun20674_2(_accm0, f2, hi2, lo2, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20673, _ctx);
    }
    { // tailcall
      kk_integer_t _x20676 = kk_integer_add_small_const(lo2, 1, _ctx); /*int*/
      kk_function_t _x20677 = kk_std_core__new_ctailm_list_fun20678_2(_accm0, x0_18110, _ctx); /*(list<4982>) -> list<4982>*/
      lo2 = _x20676;
      _accm0 = _x20677;
      goto kk__tailcall;
    }
  }
  {
    kk_integer_drop(lo2, _ctx);
    kk_integer_drop(hi2, _ctx);
    kk_function_drop(f2, _ctx);
    return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
  }
}
 
// Applies a function `f` to list of increasing elements from `lo`  to `hi`
// (including both `lo`  and `hi` ).
// If `lo > hi`  the function returns the empty list.


// lift anonymous function
struct kk_std_core_list_fun20681__t_2 {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core_list_fun20681_2(kk_function_t _fself, kk_std_core__list _ctail_17535, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_list_fun20681_2(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_list_fun20681_2, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core_list_fun20681_2(kk_function_t _fself, kk_std_core__list _ctail_17535, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17535;
}

kk_std_core__list kk_std_core_list_2(kk_integer_t lo3, kk_integer_t hi3, kk_function_t f3, kk_context_t* _ctx) { /* forall<a,e> (lo : int, hi : int, f : (int) -> e a) -> e list<a> */ 
  bool _match_20287 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_20287) {
    kk_std_core_types__ctail _x20680 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_list_2(lo3, hi3, f3, _x20680, _ctx);
  }
  {
    return kk_std_core__ctailm_list_2(lo3, hi3, f3, kk_std_core_new_list_fun20681_2(_ctx), _ctx);
  }
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift17996_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_integer_t hi, kk_integer_t lo, kk_integer_t stride, kk_box_t _ctail_17538, kk_context_t* _ctx) { /* forall<a,e> (ctail<list<a>>, f : (int) -> e a, hi : int, lo : int, stride : int, a) -> e list<a> */ 
  kk_std_core__list _ctail_17539 = kk_std_core__list_hole(); /*list<5073>*/;
  kk_std_core__list _ctail_17540 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17538, _ctail_17539, _ctx); /*list<5073>*/;
  kk_box_t* _b_18812_18809 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17540)->tail)); /*cfield<list<5073>>*/;
  kk_integer_t _x20682;
  kk_integer_t _x20683 = kk_integer_dup(stride); /*int*/
  _x20682 = kk_integer_add(lo,_x20683,kk_context()); /*int*/
  kk_std_core_types__ctail _x20684 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17540, _ctx)),_b_18812_18809); /*ctail<0>*/
  return kk_std_core__ctail_list_3(_x20682, hi, stride, f, _x20684, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift17997_op_fun20687__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t _ctail_17543;
};
static kk_std_core__list kk_std_core__mlift17997_op_fun20687(kk_function_t _fself, kk_std_core__list _ctail_17542, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17997_op_fun20687(kk_function_t _accm, kk_box_t _ctail_17543, kk_context_t* _ctx) {
  struct kk_std_core__mlift17997_op_fun20687__t* _self = kk_function_alloc_as(struct kk_std_core__mlift17997_op_fun20687__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17997_op_fun20687, kk_context());
  _self->_accm = _accm;
  _self->_ctail_17543 = _ctail_17543;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift17997_op_fun20687(kk_function_t _fself, kk_std_core__list _ctail_17542, kk_context_t* _ctx) {
  struct kk_std_core__mlift17997_op_fun20687__t* _self = kk_function_as(struct kk_std_core__mlift17997_op_fun20687__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<5073>) -> list<5073> */
  kk_box_t _ctail_17543 = _self->_ctail_17543; /* 5073 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(_ctail_17543);}, {}, _ctx)
  kk_std_core__list _x20688 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17543, _ctail_17542, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x20688, _ctx));
}

kk_std_core__list kk_std_core__mlift17997_op(kk_function_t _accm, kk_function_t f0, kk_integer_t hi0, kk_integer_t lo0, kk_integer_t stride0, kk_box_t _ctail_17543, kk_context_t* _ctx) { /* forall<a,e> ((list<a>) -> list<a>, f : (int) -> e a, hi : int, lo : int, stride : int, a) -> e list<a> */ 
  kk_integer_t _x20685;
  kk_integer_t _x20686 = kk_integer_dup(stride0); /*int*/
  _x20685 = kk_integer_add(lo0,_x20686,kk_context()); /*int*/
  return kk_std_core__ctailm_list_3(_x20685, hi0, stride0, f0, kk_std_core__new_mlift17997_op_fun20687(_accm, _ctail_17543, _ctx), _ctx);
}
 
// Returns an integer list of increasing elements from `lo`  to `hi` with stride `stride`.
// If `lo > hi`  the function returns the empty list.


// lift anonymous function
struct kk_std_core__ctail_list_fun20692__t_3 {
  struct kk_function_s _base;
  kk_function_t f1;
  kk_integer_t hi1;
  kk_integer_t lo1;
  kk_integer_t stride1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_list_fun20692_3(kk_function_t _fself, kk_box_t _b_18817, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_list_fun20692_3(kk_function_t f1, kk_integer_t hi1, kk_integer_t lo1, kk_integer_t stride1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_list_fun20692__t_3* _self = kk_function_alloc_as(struct kk_std_core__ctail_list_fun20692__t_3, 6, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_list_fun20692_3, kk_context());
  _self->f1 = f1;
  _self->hi1 = hi1;
  _self->lo1 = lo1;
  _self->stride1 = stride1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_list_fun20692_3(kk_function_t _fself, kk_box_t _b_18817, kk_context_t* _ctx) {
  struct kk_std_core__ctail_list_fun20692__t_3* _self = kk_function_as(struct kk_std_core__ctail_list_fun20692__t_3*, _fself);
  kk_function_t f1 = _self->f1; /* (int) -> 5074 5073 */
  kk_integer_t hi1 = _self->hi1; /* int */
  kk_integer_t lo1 = _self->lo1; /* int */
  kk_integer_t stride1 = _self->stride1; /* int */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<5073>> */
  kk_drop_match(_self, {kk_function_dup(f1);kk_integer_dup(hi1);kk_integer_dup(lo1);kk_integer_dup(stride1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_box_t _ctail_18835_175380 = _b_18817; /*5073*/;
  kk_std_core__list _x20693 = kk_std_core__mlift17996_op(_acc0, f1, hi1, lo1, stride1, _ctail_18835_175380, _ctx); /*list<5073>*/
  return kk_std_core__list_box(_x20693, _ctx);
}

kk_std_core__list kk_std_core__ctail_list_3(kk_integer_t lo1, kk_integer_t hi1, kk_integer_t stride1, kk_function_t f1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,e> (lo : int, hi : int, stride : int, f : (int) -> e a, ctail<list<a>>) -> e list<a> */ 
  kk__tailcall: ;
  bool _match_20285 = kk_integer_lte_borrow(lo1,hi1,kk_context()); /*bool*/;
  if (_match_20285) {
    kk_box_t x_18113;
    kk_function_t _x20690 = kk_function_dup(f1); /*(int) -> 5074 5073*/
    kk_integer_t _x20689 = kk_integer_dup(lo1); /*int*/
    x_18113 = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_context_t*), _x20690, (_x20690, _x20689, _ctx)); /*5073*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x_18113, _ctx);
      kk_box_t _x20691 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_list_fun20692_3(f1, hi1, lo1, stride1, _acc0, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20691, _ctx);
    }
    {
      kk_std_core__list _ctail_175390 = kk_std_core__list_hole(); /*list<5073>*/;
      kk_std_core__list _ctail_175400 = kk_std_core__new_Cons(kk_reuse_null, x_18113, _ctail_175390, _ctx); /*list<5073>*/;
      kk_box_t* _b_18829_18823 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_175400)->tail)); /*cfield<list<5073>>*/;
      { // tailcall
        kk_integer_t _x20694;
        kk_integer_t _x20695 = kk_integer_dup(stride1); /*int*/
        _x20694 = kk_integer_add(lo1,_x20695,kk_context()); /*int*/
        kk_std_core_types__ctail _x20696 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_175400, _ctx)),_b_18829_18823); /*ctail<0>*/
        lo1 = _x20694;
        _acc0 = _x20696;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_integer_drop(stride1, _ctx);
    kk_integer_drop(lo1, _ctx);
    kk_integer_drop(hi1, _ctx);
    kk_function_drop(f1, _ctx);
    kk_box_t _x20697 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20697, _ctx);
  }
}
 
// Returns an integer list of increasing elements from `lo`  to `hi` with stride `stride`.
// If `lo > hi`  the function returns the empty list.


// lift anonymous function
struct kk_std_core__ctailm_list_fun20701__t_3 {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t f2;
  kk_integer_t hi2;
  kk_integer_t lo2;
  kk_integer_t stride2;
};
static kk_box_t kk_std_core__ctailm_list_fun20701_3(kk_function_t _fself, kk_box_t _b_18837, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_list_fun20701_3(kk_function_t _accm0, kk_function_t f2, kk_integer_t hi2, kk_integer_t lo2, kk_integer_t stride2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_list_fun20701__t_3* _self = kk_function_alloc_as(struct kk_std_core__ctailm_list_fun20701__t_3, 6, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_list_fun20701_3, kk_context());
  _self->_accm0 = _accm0;
  _self->f2 = f2;
  _self->hi2 = hi2;
  _self->lo2 = lo2;
  _self->stride2 = stride2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_list_fun20701_3(kk_function_t _fself, kk_box_t _b_18837, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_list_fun20701__t_3* _self = kk_function_as(struct kk_std_core__ctailm_list_fun20701__t_3*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<5073>) -> list<5073> */
  kk_function_t f2 = _self->f2; /* (int) -> 5074 5073 */
  kk_integer_t hi2 = _self->hi2; /* int */
  kk_integer_t lo2 = _self->lo2; /* int */
  kk_integer_t stride2 = _self->stride2; /* int */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(f2);kk_integer_dup(hi2);kk_integer_dup(lo2);kk_integer_dup(stride2);}, {}, _ctx)
  kk_box_t _ctail_18839_175430 = _b_18837; /*5073*/;
  kk_std_core__list _x20702 = kk_std_core__mlift17997_op(_accm0, f2, hi2, lo2, stride2, _ctail_18839_175430, _ctx); /*list<5073>*/
  return kk_std_core__list_box(_x20702, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_list_fun20706__t_3 {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t x0_18116;
};
static kk_std_core__list kk_std_core__ctailm_list_fun20706_3(kk_function_t _fself, kk_std_core__list _ctail_175420, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_list_fun20706_3(kk_function_t _accm0, kk_box_t x0_18116, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_list_fun20706__t_3* _self = kk_function_alloc_as(struct kk_std_core__ctailm_list_fun20706__t_3, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_list_fun20706_3, kk_context());
  _self->_accm0 = _accm0;
  _self->x0_18116 = x0_18116;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_list_fun20706_3(kk_function_t _fself, kk_std_core__list _ctail_175420, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_list_fun20706__t_3* _self = kk_function_as(struct kk_std_core__ctailm_list_fun20706__t_3*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<5073>) -> list<5073> */
  kk_box_t x0_18116 = _self->x0_18116; /* 5073 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(x0_18116);}, {}, _ctx)
  kk_std_core__list _x20707 = kk_std_core__new_Cons(kk_reuse_null, x0_18116, _ctail_175420, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x20707, _ctx));
}

kk_std_core__list kk_std_core__ctailm_list_3(kk_integer_t lo2, kk_integer_t hi2, kk_integer_t stride2, kk_function_t f2, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,e> (lo : int, hi : int, stride : int, f : (int) -> e a, (list<a>) -> list<a>) -> e list<a> */ 
  kk__tailcall: ;
  bool _match_20283 = kk_integer_lte_borrow(lo2,hi2,kk_context()); /*bool*/;
  if (_match_20283) {
    kk_box_t x0_18116;
    kk_function_t _x20699 = kk_function_dup(f2); /*(int) -> 5074 5073*/
    kk_integer_t _x20698 = kk_integer_dup(lo2); /*int*/
    x0_18116 = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_context_t*), _x20699, (_x20699, _x20698, _ctx)); /*5073*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x0_18116, _ctx);
      kk_box_t _x20700 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_list_fun20701_3(_accm0, f2, hi2, lo2, stride2, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20700, _ctx);
    }
    { // tailcall
      kk_integer_t _x20703;
      kk_integer_t _x20704 = kk_integer_dup(stride2); /*int*/
      _x20703 = kk_integer_add(lo2,_x20704,kk_context()); /*int*/
      kk_function_t _x20705 = kk_std_core__new_ctailm_list_fun20706_3(_accm0, x0_18116, _ctx); /*(list<5073>) -> list<5073>*/
      lo2 = _x20703;
      _accm0 = _x20705;
      goto kk__tailcall;
    }
  }
  {
    kk_integer_drop(stride2, _ctx);
    kk_integer_drop(lo2, _ctx);
    kk_integer_drop(hi2, _ctx);
    kk_function_drop(f2, _ctx);
    return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
  }
}
 
// Returns an integer list of increasing elements from `lo`  to `hi` with stride `stride`.
// If `lo > hi`  the function returns the empty list.


// lift anonymous function
struct kk_std_core_list_fun20709__t_3 {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core_list_fun20709_3(kk_function_t _fself, kk_std_core__list _ctail_17541, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_list_fun20709_3(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_list_fun20709_3, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core_list_fun20709_3(kk_function_t _fself, kk_std_core__list _ctail_17541, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17541;
}

kk_std_core__list kk_std_core_list_3(kk_integer_t lo3, kk_integer_t hi3, kk_integer_t stride3, kk_function_t f3, kk_context_t* _ctx) { /* forall<a,e> (lo : int, hi : int, stride : int, f : (int) -> e a) -> e list<a> */ 
  bool _match_20282 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_20282) {
    kk_std_core_types__ctail _x20708 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_list_3(lo3, hi3, stride3, f3, _x20708, _ctx);
  }
  {
    return kk_std_core__ctailm_list_3(lo3, hi3, stride3, f3, kk_std_core_new_list_fun20709_3(_ctx), _ctx);
  }
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift17998_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_std_core__list xx, kk_box_t _ctail_17544, kk_context_t* _ctx) { /* forall<a,b,e> (ctail<list<b>>, f : (a) -> e b, xx : list<a>, b) -> e list<b> */ 
  kk_std_core__list _ctail_17545 = kk_std_core__list_hole(); /*list<6195>*/;
  kk_std_core__list _ctail_17546 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17544, _ctail_17545, _ctx); /*list<6195>*/;
  kk_box_t* _b_18848_18845 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17546)->tail)); /*cfield<list<6195>>*/;
  kk_std_core_types__ctail _x20710 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17546, _ctx)),_b_18848_18845); /*ctail<0>*/
  return kk_std_core__ctail_map_5(xx, f, _x20710, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift17999_op_fun20711__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t _ctail_17549;
};
static kk_std_core__list kk_std_core__mlift17999_op_fun20711(kk_function_t _fself, kk_std_core__list _ctail_17548, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17999_op_fun20711(kk_function_t _accm, kk_box_t _ctail_17549, kk_context_t* _ctx) {
  struct kk_std_core__mlift17999_op_fun20711__t* _self = kk_function_alloc_as(struct kk_std_core__mlift17999_op_fun20711__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17999_op_fun20711, kk_context());
  _self->_accm = _accm;
  _self->_ctail_17549 = _ctail_17549;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift17999_op_fun20711(kk_function_t _fself, kk_std_core__list _ctail_17548, kk_context_t* _ctx) {
  struct kk_std_core__mlift17999_op_fun20711__t* _self = kk_function_as(struct kk_std_core__mlift17999_op_fun20711__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<6195>) -> list<6195> */
  kk_box_t _ctail_17549 = _self->_ctail_17549; /* 6195 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(_ctail_17549);}, {}, _ctx)
  kk_std_core__list _x20712 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17549, _ctail_17548, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x20712, _ctx));
}

kk_std_core__list kk_std_core__mlift17999_op(kk_function_t _accm, kk_function_t f0, kk_std_core__list xx0, kk_box_t _ctail_17549, kk_context_t* _ctx) { /* forall<a,b,e> ((list<b>) -> list<b>, f : (a) -> e b, xx : list<a>, b) -> e list<b> */ 
  return kk_std_core__ctailm_map_5(xx0, f0, kk_std_core__new_mlift17999_op_fun20711(_accm, _ctail_17549, _ctx), _ctx);
}
 
// Apply a function `f`  to each element of the input list in sequence.


// lift anonymous function
struct kk_std_core__ctail_map_fun20716__t_5 {
  struct kk_function_s _base;
  kk_function_t f1;
  kk_std_core__list xx1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_map_fun20716_5(kk_function_t _fself, kk_box_t _b_18853, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_map_fun20716_5(kk_function_t f1, kk_std_core__list xx1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_map_fun20716__t_5* _self = kk_function_alloc_as(struct kk_std_core__ctail_map_fun20716__t_5, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_map_fun20716_5, kk_context());
  _self->f1 = f1;
  _self->xx1 = xx1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_map_fun20716_5(kk_function_t _fself, kk_box_t _b_18853, kk_context_t* _ctx) {
  struct kk_std_core__ctail_map_fun20716__t_5* _self = kk_function_as(struct kk_std_core__ctail_map_fun20716__t_5*, _fself);
  kk_function_t f1 = _self->f1; /* (6194) -> 6196 6195 */
  kk_std_core__list xx1 = _self->xx1; /* list<6194> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<6195>> */
  kk_drop_match(_self, {kk_function_dup(f1);kk_std_core__list_dup(xx1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_box_t _ctail_18871_175440 = _b_18853; /*6195*/;
  kk_std_core__list _x20717 = kk_std_core__mlift17998_op(_acc0, f1, xx1, _ctail_18871_175440, _ctx); /*list<6195>*/
  return kk_std_core__list_box(_x20717, _ctx);
}

kk_std_core__list kk_std_core__ctail_map_5(kk_std_core__list xs, kk_function_t f1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, f : (a) -> e b, ctail<list<b>>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con20713 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con20713->head;
    kk_std_core__list xx1 = _con20713->tail;
    kk_reuse_t _ru_20369 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      _ru_20369 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx1);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_box_t x0_18119;
    kk_function_t _x20714 = kk_function_dup(f1); /*(6194) -> 6196 6195*/
    x0_18119 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x20714, (_x20714, x, _ctx)); /*6195*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_20369,kk_context());
      kk_box_drop(x0_18119, _ctx);
      kk_box_t _x20715 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_map_fun20716_5(f1, xx1, _acc0, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20715, _ctx);
    }
    {
      kk_std_core__list _ctail_175450 = kk_std_core__list_hole(); /*list<6195>*/;
      kk_std_core__list _ctail_175460 = kk_std_core__new_Cons(_ru_20369, x0_18119, _ctail_175450, _ctx); /*list<6195>*/;
      kk_box_t* _b_18865_18859 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_175460)->tail)); /*cfield<list<6195>>*/;
      { // tailcall
        kk_std_core_types__ctail _x20718 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_175460, _ctx)),_b_18865_18859); /*ctail<0>*/
        xs = xx1;
        _acc0 = _x20718;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_function_drop(f1, _ctx);
    kk_box_t _x20719 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20719, _ctx);
  }
}
 
// Apply a function `f`  to each element of the input list in sequence.


// lift anonymous function
struct kk_std_core__ctailm_map_fun20723__t_5 {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t f2;
  kk_std_core__list xx2;
};
static kk_box_t kk_std_core__ctailm_map_fun20723_5(kk_function_t _fself, kk_box_t _b_18873, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_map_fun20723_5(kk_function_t _accm0, kk_function_t f2, kk_std_core__list xx2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_map_fun20723__t_5* _self = kk_function_alloc_as(struct kk_std_core__ctailm_map_fun20723__t_5, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_map_fun20723_5, kk_context());
  _self->_accm0 = _accm0;
  _self->f2 = f2;
  _self->xx2 = xx2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_map_fun20723_5(kk_function_t _fself, kk_box_t _b_18873, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_map_fun20723__t_5* _self = kk_function_as(struct kk_std_core__ctailm_map_fun20723__t_5*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<6195>) -> list<6195> */
  kk_function_t f2 = _self->f2; /* (6194) -> 6196 6195 */
  kk_std_core__list xx2 = _self->xx2; /* list<6194> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(f2);kk_std_core__list_dup(xx2);}, {}, _ctx)
  kk_box_t _ctail_18875_175490 = _b_18873; /*6195*/;
  kk_std_core__list _x20724 = kk_std_core__mlift17999_op(_accm0, f2, xx2, _ctail_18875_175490, _ctx); /*list<6195>*/
  return kk_std_core__list_box(_x20724, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_map_fun20726__t_5 {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t x2_18122;
};
static kk_std_core__list kk_std_core__ctailm_map_fun20726_5(kk_function_t _fself, kk_std_core__list _ctail_175480, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_map_fun20726_5(kk_function_t _accm0, kk_box_t x2_18122, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_map_fun20726__t_5* _self = kk_function_alloc_as(struct kk_std_core__ctailm_map_fun20726__t_5, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_map_fun20726_5, kk_context());
  _self->_accm0 = _accm0;
  _self->x2_18122 = x2_18122;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_map_fun20726_5(kk_function_t _fself, kk_std_core__list _ctail_175480, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_map_fun20726__t_5* _self = kk_function_as(struct kk_std_core__ctailm_map_fun20726__t_5*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<6195>) -> list<6195> */
  kk_box_t x2_18122 = _self->x2_18122; /* 6195 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(x2_18122);}, {}, _ctx)
  kk_std_core__list _x20727 = kk_std_core__new_Cons(kk_reuse_null, x2_18122, _ctail_175480, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x20727, _ctx));
}

kk_std_core__list kk_std_core__ctailm_map_5(kk_std_core__list xs0, kk_function_t f2, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, f : (a) -> e b, (list<b>) -> list<b>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con20720 = kk_std_core__as_Cons(xs0);
    kk_box_t x1 = _con20720->head;
    kk_std_core__list xx2 = _con20720->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs0))) {
      kk_std_core__list_free(xs0, _ctx);
    }
    else {
      kk_box_dup(x1);
      kk_std_core__list_dup(xx2);
      kk_std_core__list_decref(xs0, _ctx);
    }
    kk_box_t x2_18122;
    kk_function_t _x20721 = kk_function_dup(f2); /*(6194) -> 6196 6195*/
    x2_18122 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x20721, (_x20721, x1, _ctx)); /*6195*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x2_18122, _ctx);
      kk_box_t _x20722 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_map_fun20723_5(_accm0, f2, xx2, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20722, _ctx);
    }
    { // tailcall
      kk_function_t _x20725 = kk_std_core__new_ctailm_map_fun20726_5(_accm0, x2_18122, _ctx); /*(list<6195>) -> list<6195>*/
      xs0 = xx2;
      _accm0 = _x20725;
      goto kk__tailcall;
    }
  }
  {
    kk_function_drop(f2, _ctx);
    return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
  }
}
 
// Apply a function `f`  to each element of the input list in sequence.


// lift anonymous function
struct kk_std_core_map_fun20729__t_5 {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core_map_fun20729_5(kk_function_t _fself, kk_std_core__list _ctail_17547, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20729_5(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_map_fun20729_5, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core_map_fun20729_5(kk_function_t _fself, kk_std_core__list _ctail_17547, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17547;
}

kk_std_core__list kk_std_core_map_5(kk_std_core__list xs1, kk_function_t f3, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, f : (a) -> e b) -> e list<b> */ 
  bool _match_20279 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_20279) {
    kk_std_core_types__ctail _x20728 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_map_5(xs1, f3, _x20728, _ctx);
  }
  {
    return kk_std_core__ctailm_map_5(xs1, f3, kk_std_core_new_map_fun20729_5(_ctx), _ctx);
  }
}
 
// lifted local: list.4, .spec.17403
// specialized: std/core/map.5, on parameters f, using:
// f = std/core/char

kk_std_core__list kk_std_core__ctail_lift17478_list_4(kk_std_core__list xs, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* (xs : list<int>, ctail<list<char>>) -> total list<char> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con20730 = kk_std_core__as_Cons(xs);
    kk_box_t _box_x18876 = _con20730->head;
    kk_std_core__list xx = _con20730->tail;
    kk_integer_t x = kk_integer_unbox(_box_x18876);
    kk_reuse_t _ru_20371 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      _ru_20371 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_integer_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_char_t _ctail_17550 = kk_integer_clamp32(x,kk_context()); /*char*/;
    kk_std_core__list _ctail_17551 = kk_std_core__list_hole(); /*list<char>*/;
    kk_std_core__list _ctail_17552 = kk_std_core__new_Cons(_ru_20371, kk_char_box(_ctail_17550, _ctx), _ctail_17551, _ctx); /*list<char>*/;
    kk_box_t* _b_18891_18886 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17552)->tail)); /*cfield<list<char>>*/;
    { // tailcall
      kk_std_core_types__ctail _x20732 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17552, _ctx)),_b_18891_18886); /*ctail<0>*/
      xs = xx;
      _acc = _x20732;
      goto kk__tailcall;
    }
  }
  {
    kk_box_t _x20733 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20733, _ctx);
  }
}
 
// lifted local: list.4, .spec.17403
// specialized: std/core/map.5, on parameters f, using:
// f = std/core/char

kk_std_core__list kk_std_core__lift17478_list_4(kk_std_core__list xs0, kk_context_t* _ctx) { /* (xs : list<int>) -> total list<char> */ 
  kk_std_core_types__ctail _x20734 = kk_ctail_nil(); /*ctail<0>*/
  return kk_std_core__ctail_lift17478_list_4(xs0, _x20734, _ctx);
}
 
// Convert a string to a list of characters

kk_std_core__list kk_std_core_list_6(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> total list<char> */ 
  return kk_string_to_list(s,kk_context());
}


// lift anonymous function
struct kk_std_core_map_fun20739__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_map_fun20739(kk_function_t _fself, kk_box_t _b_18898, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20739(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_map_fun20739, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_map_fun20739(kk_function_t _fself, kk_box_t _b_18898, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_box_t _y_18900_17693 = _b_18898; /*5375*/;
  kk_std_core_types__maybe _x20740 = kk_std_core_types__new_Just(_y_18900_17693, _ctx); /*maybe<1034>*/
  return kk_std_core_types__maybe_box(_x20740, _ctx);
}

kk_std_core_types__maybe kk_std_core_map(kk_std_core_types__maybe m, kk_function_t f, kk_context_t* _ctx) { /* forall<a,b,e> (m : maybe<a>, f : (a) -> e b) -> e maybe<b> */ 
  if (kk_std_core_types__is_Nothing(m)) {
    kk_function_drop(f, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  {
    kk_box_t x = m._cons.Just.value;
    kk_box_t x0_18125 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, x, _ctx)); /*5375*/;
    if (kk_yielding(kk_context())) {
      kk_box_drop(x0_18125, _ctx);
      kk_box_t _x20738 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun20739(_ctx), _ctx); /*3926*/
      return kk_std_core_types__maybe_unbox(_x20738, _ctx);
    }
    {
      return kk_std_core_types__new_Just(x0_18125, _ctx);
    }
  }
}
 
// Map over the `Right` component of an `:either` type.


// lift anonymous function
struct kk_std_core_map_fun20742__t_1 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_map_fun20742_1(kk_function_t _fself, kk_box_t _b_18902, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20742_1(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_map_fun20742_1, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_map_fun20742_1(kk_function_t _fself, kk_box_t _b_18902, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_box_t _y_18904_17695 = _b_18902; /*5413*/;
  kk_std_core_types__either _x20743 = kk_std_core_types__new_Right(_y_18904_17695, _ctx); /*either<1024,1025>*/
  return kk_std_core_types__either_box(_x20743, _ctx);
}

kk_std_core_types__either kk_std_core_map_1(kk_std_core_types__either e, kk_function_t f, kk_context_t* _ctx) { /* forall<a,b,c,e> (e : either<a,b>, f : (b) -> e c) -> e either<a,c> */ 
  if (kk_std_core_types__is_Right(e)) {
    kk_box_t x = e._cons.Right.right;
    kk_box_t x0_18129 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, x, _ctx)); /*5413*/;
    if (kk_yielding(kk_context())) {
      kk_box_drop(x0_18129, _ctx);
      kk_box_t _x20741 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun20742_1(_ctx), _ctx); /*3926*/
      return kk_std_core_types__either_unbox(_x20741, _ctx);
    }
    {
      return kk_std_core_types__new_Right(x0_18129, _ctx);
    }
  }
  {
    kk_box_t x00 = e._cons.Left.left;
    kk_function_drop(f, _ctx);
    return kk_std_core_types__new_Left(x00, _ctx);
  }
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift18003_map_fun20746__t_2 {
  struct kk_function_s _base;
  kk_box_t _y_17697;
};
static kk_box_t kk_std_core__mlift18003_map_fun20746_2(kk_function_t _fself, kk_box_t _b_18906, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift18003_map_fun20746_2(kk_box_t _y_17697, kk_context_t* _ctx) {
  struct kk_std_core__mlift18003_map_fun20746__t_2* _self = kk_function_alloc_as(struct kk_std_core__mlift18003_map_fun20746__t_2, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift18003_map_fun20746_2, kk_context());
  _self->_y_17697 = _y_17697;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift18003_map_fun20746_2(kk_function_t _fself, kk_box_t _b_18906, kk_context_t* _ctx) {
  struct kk_std_core__mlift18003_map_fun20746__t_2* _self = kk_function_as(struct kk_std_core__mlift18003_map_fun20746__t_2*, _fself);
  kk_box_t _y_17697 = _self->_y_17697; /* 5558 */
  kk_drop_match(_self, {kk_box_dup(_y_17697);}, {}, _ctx)
  kk_box_t _y_18908_17698 = _b_18906; /*5558*/;
  kk_std_core_types__tuple2_ _x20747 = kk_std_core_types__new_dash__lp__comma__rp_(_y_17697, _y_18908_17698, _ctx); /*(1004, 1005)*/
  return kk_std_core_types__tuple2__box(_x20747, _ctx);
}

kk_std_core_types__tuple2_ kk_std_core__mlift18003_map_2(kk_function_t f, kk_std_core_types__tuple2_ t, kk_box_t _y_17697, kk_context_t* _ctx) { /* forall<a,b,e> (f : (a) -> e b, t : (a, a), b) -> e (b, b) */ 
  kk_box_t x_18133;
  kk_box_t _x20744;
  {
    kk_box_t _x = t.snd;
    kk_box_dup(_x);
    kk_std_core_types__tuple2__drop(t, _ctx);
    _x20744 = _x; /*5557*/
  }
  x_18133 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, _x20744, _ctx)); /*5558*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_18133, _ctx);
    kk_box_t _x20745 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift18003_map_fun20746_2(_y_17697, _ctx), _ctx); /*3926*/
    return kk_std_core_types__tuple2__unbox(_x20745, _ctx);
  }
  {
    return kk_std_core_types__new_dash__lp__comma__rp_(_y_17697, x_18133, _ctx);
  }
}


// lift anonymous function
struct kk_std_core_map_fun20751__t_2 {
  struct kk_function_s _base;
  kk_function_t f;
  kk_std_core_types__tuple2_ t;
};
static kk_box_t kk_std_core_map_fun20751_2(kk_function_t _fself, kk_box_t _b_18910, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20751_2(kk_function_t f, kk_std_core_types__tuple2_ t, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20751__t_2* _self = kk_function_alloc_as(struct kk_std_core_map_fun20751__t_2, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun20751_2, kk_context());
  _self->f = f;
  _self->t = t;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun20751_2(kk_function_t _fself, kk_box_t _b_18910, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20751__t_2* _self = kk_function_as(struct kk_std_core_map_fun20751__t_2*, _fself);
  kk_function_t f = _self->f; /* (5557) -> 5559 5558 */
  kk_std_core_types__tuple2_ t = _self->t; /* (5557, 5557) */
  kk_drop_match(_self, {kk_function_dup(f);kk_std_core_types__tuple2__dup(t);}, {}, _ctx)
  kk_box_t _y_18915_17697 = _b_18910; /*5558*/;
  kk_std_core_types__tuple2_ _x20752 = kk_std_core__mlift18003_map_2(f, t, _y_18915_17697, _ctx); /*(5558, 5558)*/
  return kk_std_core_types__tuple2__box(_x20752, _ctx);
}


// lift anonymous function
struct kk_std_core_map_fun20755__t_2 {
  struct kk_function_s _base;
  kk_box_t x_18138;
};
static kk_box_t kk_std_core_map_fun20755_2(kk_function_t _fself, kk_box_t _b_18912, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20755_2(kk_box_t x_18138, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20755__t_2* _self = kk_function_alloc_as(struct kk_std_core_map_fun20755__t_2, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun20755_2, kk_context());
  _self->x_18138 = x_18138;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun20755_2(kk_function_t _fself, kk_box_t _b_18912, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20755__t_2* _self = kk_function_as(struct kk_std_core_map_fun20755__t_2*, _fself);
  kk_box_t x_18138 = _self->x_18138; /* 5558 */
  kk_drop_match(_self, {kk_box_dup(x_18138);}, {}, _ctx)
  kk_box_t _y_18916_17698 = _b_18912; /*5558*/;
  kk_std_core_types__tuple2_ _x20756 = kk_std_core_types__new_dash__lp__comma__rp_(x_18138, _y_18916_17698, _ctx); /*(1004, 1005)*/
  return kk_std_core_types__tuple2__box(_x20756, _ctx);
}

kk_std_core_types__tuple2_ kk_std_core_map_2(kk_std_core_types__tuple2_ t, kk_function_t f, kk_context_t* _ctx) { /* forall<a,b,e> (t : (a, a), f : (a) -> e b) -> e (b, b) */ 
  kk_box_t x_18138;
  kk_function_t _x20749 = kk_function_dup(f); /*(5557) -> 5559 5558*/
  kk_box_t _x20748;
  {
    kk_box_t _x = t.fst;
    kk_box_dup(_x);
    _x20748 = _x; /*5557*/
  }
  x_18138 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x20749, (_x20749, _x20748, _ctx)); /*5558*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_18138, _ctx);
    kk_box_t _x20750 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun20751_2(f, t, _ctx), _ctx); /*3926*/
    return kk_std_core_types__tuple2__unbox(_x20750, _ctx);
  }
  {
    kk_box_t x0_18142;
    kk_box_t _x20753;
    {
      kk_box_t _x0 = t.snd;
      kk_box_dup(_x0);
      kk_std_core_types__tuple2__drop(t, _ctx);
      _x20753 = _x0; /*5557*/
    }
    x0_18142 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, _x20753, _ctx)); /*5558*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x0_18142, _ctx);
      kk_box_t _x20754 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun20755_2(x_18138, _ctx), _ctx); /*3926*/
      return kk_std_core_types__tuple2__unbox(_x20754, _ctx);
    }
    {
      return kk_std_core_types__new_dash__lp__comma__rp_(x_18138, x0_18142, _ctx);
    }
  }
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift18005_map_fun20759__t_3 {
  struct kk_function_s _base;
  kk_box_t _y_17699;
  kk_box_t _y_17700;
};
static kk_box_t kk_std_core__mlift18005_map_fun20759_3(kk_function_t _fself, kk_box_t _b_18918, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift18005_map_fun20759_3(kk_box_t _y_17699, kk_box_t _y_17700, kk_context_t* _ctx) {
  struct kk_std_core__mlift18005_map_fun20759__t_3* _self = kk_function_alloc_as(struct kk_std_core__mlift18005_map_fun20759__t_3, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift18005_map_fun20759_3, kk_context());
  _self->_y_17699 = _y_17699;
  _self->_y_17700 = _y_17700;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift18005_map_fun20759_3(kk_function_t _fself, kk_box_t _b_18918, kk_context_t* _ctx) {
  struct kk_std_core__mlift18005_map_fun20759__t_3* _self = kk_function_as(struct kk_std_core__mlift18005_map_fun20759__t_3*, _fself);
  kk_box_t _y_17699 = _self->_y_17699; /* 5776 */
  kk_box_t _y_17700 = _self->_y_17700; /* 5776 */
  kk_drop_match(_self, {kk_box_dup(_y_17699);kk_box_dup(_y_17700);}, {}, _ctx)
  kk_box_t _y_18920_17701 = _b_18918; /*5776*/;
  kk_std_core_types__tuple3_ _x20760 = kk_std_core_types__new_dash__lp__comma__comma__rp_(_y_17699, _y_17700, _y_18920_17701, _ctx); /*(1008, 1009, 1010)*/
  return kk_std_core_types__tuple3__box(_x20760, _ctx);
}

kk_std_core_types__tuple3_ kk_std_core__mlift18005_map_3(kk_box_t _y_17699, kk_function_t f, kk_std_core_types__tuple3_ t, kk_box_t _y_17700, kk_context_t* _ctx) { /* forall<a,b,e> (b, f : (a) -> e b, t : (a, a, a), b) -> e (b, b, b) */ 
  kk_box_t x_18148;
  kk_box_t _x20757;
  {
    kk_box_t _x = t.thd;
    kk_box_dup(_x);
    kk_std_core_types__tuple3__drop(t, _ctx);
    _x20757 = _x; /*5775*/
  }
  x_18148 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, _x20757, _ctx)); /*5776*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_18148, _ctx);
    kk_box_t _x20758 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift18005_map_fun20759_3(_y_17699, _y_17700, _ctx), _ctx); /*3926*/
    return kk_std_core_types__tuple3__unbox(_x20758, _ctx);
  }
  {
    return kk_std_core_types__new_dash__lp__comma__comma__rp_(_y_17699, _y_17700, x_18148, _ctx);
  }
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift18006_map_fun20764__t_3 {
  struct kk_function_s _base;
  kk_box_t _y_17699;
  kk_function_t f;
  kk_std_core_types__tuple3_ t;
};
static kk_box_t kk_std_core__mlift18006_map_fun20764_3(kk_function_t _fself, kk_box_t _b_18922, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift18006_map_fun20764_3(kk_box_t _y_17699, kk_function_t f, kk_std_core_types__tuple3_ t, kk_context_t* _ctx) {
  struct kk_std_core__mlift18006_map_fun20764__t_3* _self = kk_function_alloc_as(struct kk_std_core__mlift18006_map_fun20764__t_3, 6, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift18006_map_fun20764_3, kk_context());
  _self->_y_17699 = _y_17699;
  _self->f = f;
  _self->t = t;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift18006_map_fun20764_3(kk_function_t _fself, kk_box_t _b_18922, kk_context_t* _ctx) {
  struct kk_std_core__mlift18006_map_fun20764__t_3* _self = kk_function_as(struct kk_std_core__mlift18006_map_fun20764__t_3*, _fself);
  kk_box_t _y_17699 = _self->_y_17699; /* 5776 */
  kk_function_t f = _self->f; /* (5775) -> 5777 5776 */
  kk_std_core_types__tuple3_ t = _self->t; /* (5775, 5775, 5775) */
  kk_drop_match(_self, {kk_box_dup(_y_17699);kk_function_dup(f);kk_std_core_types__tuple3__dup(t);}, {}, _ctx)
  kk_box_t _y_18924_17700 = _b_18922; /*5776*/;
  kk_std_core_types__tuple3_ _x20765 = kk_std_core__mlift18005_map_3(_y_17699, f, t, _y_18924_17700, _ctx); /*(5776, 5776, 5776)*/
  return kk_std_core_types__tuple3__box(_x20765, _ctx);
}

kk_std_core_types__tuple3_ kk_std_core__mlift18006_map_3(kk_function_t f, kk_std_core_types__tuple3_ t, kk_box_t _y_17699, kk_context_t* _ctx) { /* forall<a,b,e> (f : (a) -> e b, t : (a, a, a), b) -> e (b, b, b) */ 
  kk_box_t x_18154;
  kk_function_t _x20762 = kk_function_dup(f); /*(5775) -> 5777 5776*/
  kk_box_t _x20761;
  {
    kk_box_t _x = t.snd;
    kk_box_dup(_x);
    _x20761 = _x; /*5775*/
  }
  x_18154 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x20762, (_x20762, _x20761, _ctx)); /*5776*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_18154, _ctx);
    kk_box_t _x20763 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift18006_map_fun20764_3(_y_17699, f, t, _ctx), _ctx); /*3926*/
    return kk_std_core_types__tuple3__unbox(_x20763, _ctx);
  }
  {
    return kk_std_core__mlift18005_map_3(_y_17699, f, t, x_18154, _ctx);
  }
}


// lift anonymous function
struct kk_std_core_map_fun20769__t_3 {
  struct kk_function_s _base;
  kk_function_t f;
  kk_std_core_types__tuple3_ t;
};
static kk_box_t kk_std_core_map_fun20769_3(kk_function_t _fself, kk_box_t _b_18926, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20769_3(kk_function_t f, kk_std_core_types__tuple3_ t, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20769__t_3* _self = kk_function_alloc_as(struct kk_std_core_map_fun20769__t_3, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun20769_3, kk_context());
  _self->f = f;
  _self->t = t;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun20769_3(kk_function_t _fself, kk_box_t _b_18926, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20769__t_3* _self = kk_function_as(struct kk_std_core_map_fun20769__t_3*, _fself);
  kk_function_t f = _self->f; /* (5775) -> 5777 5776 */
  kk_std_core_types__tuple3_ t = _self->t; /* (5775, 5775, 5775) */
  kk_drop_match(_self, {kk_function_dup(f);kk_std_core_types__tuple3__dup(t);}, {}, _ctx)
  kk_box_t _y_18934_17699 = _b_18926; /*5776*/;
  kk_std_core_types__tuple3_ _x20770 = kk_std_core__mlift18006_map_3(f, t, _y_18934_17699, _ctx); /*(5776, 5776, 5776)*/
  return kk_std_core_types__tuple3__box(_x20770, _ctx);
}


// lift anonymous function
struct kk_std_core_map_fun20774__t_3 {
  struct kk_function_s _base;
  kk_function_t f;
  kk_std_core_types__tuple3_ t;
  kk_box_t x_18157;
};
static kk_box_t kk_std_core_map_fun20774_3(kk_function_t _fself, kk_box_t _b_18928, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20774_3(kk_function_t f, kk_std_core_types__tuple3_ t, kk_box_t x_18157, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20774__t_3* _self = kk_function_alloc_as(struct kk_std_core_map_fun20774__t_3, 6, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun20774_3, kk_context());
  _self->f = f;
  _self->t = t;
  _self->x_18157 = x_18157;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun20774_3(kk_function_t _fself, kk_box_t _b_18928, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20774__t_3* _self = kk_function_as(struct kk_std_core_map_fun20774__t_3*, _fself);
  kk_function_t f = _self->f; /* (5775) -> 5777 5776 */
  kk_std_core_types__tuple3_ t = _self->t; /* (5775, 5775, 5775) */
  kk_box_t x_18157 = _self->x_18157; /* 5776 */
  kk_drop_match(_self, {kk_function_dup(f);kk_std_core_types__tuple3__dup(t);kk_box_dup(x_18157);}, {}, _ctx)
  kk_box_t _y_18935_17700 = _b_18928; /*5776*/;
  kk_std_core_types__tuple3_ _x20775 = kk_std_core__mlift18005_map_3(x_18157, f, t, _y_18935_17700, _ctx); /*(5776, 5776, 5776)*/
  return kk_std_core_types__tuple3__box(_x20775, _ctx);
}


// lift anonymous function
struct kk_std_core_map_fun20778__t_3 {
  struct kk_function_s _base;
  kk_box_t x_18157;
  kk_box_t x0_18161;
};
static kk_box_t kk_std_core_map_fun20778_3(kk_function_t _fself, kk_box_t _b_18930, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20778_3(kk_box_t x_18157, kk_box_t x0_18161, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20778__t_3* _self = kk_function_alloc_as(struct kk_std_core_map_fun20778__t_3, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun20778_3, kk_context());
  _self->x_18157 = x_18157;
  _self->x0_18161 = x0_18161;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun20778_3(kk_function_t _fself, kk_box_t _b_18930, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20778__t_3* _self = kk_function_as(struct kk_std_core_map_fun20778__t_3*, _fself);
  kk_box_t x_18157 = _self->x_18157; /* 5776 */
  kk_box_t x0_18161 = _self->x0_18161; /* 5776 */
  kk_drop_match(_self, {kk_box_dup(x_18157);kk_box_dup(x0_18161);}, {}, _ctx)
  kk_box_t _y_18936_17701 = _b_18930; /*5776*/;
  kk_std_core_types__tuple3_ _x20779 = kk_std_core_types__new_dash__lp__comma__comma__rp_(x_18157, x0_18161, _y_18936_17701, _ctx); /*(1008, 1009, 1010)*/
  return kk_std_core_types__tuple3__box(_x20779, _ctx);
}

kk_std_core_types__tuple3_ kk_std_core_map_3(kk_std_core_types__tuple3_ t, kk_function_t f, kk_context_t* _ctx) { /* forall<a,b,e> (t : (a, a, a), f : (a) -> e b) -> e (b, b, b) */ 
  kk_box_t x_18157;
  kk_function_t _x20767 = kk_function_dup(f); /*(5775) -> 5777 5776*/
  kk_box_t _x20766;
  {
    kk_box_t _x = t.fst;
    kk_box_dup(_x);
    _x20766 = _x; /*5775*/
  }
  x_18157 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x20767, (_x20767, _x20766, _ctx)); /*5776*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_18157, _ctx);
    kk_box_t _x20768 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun20769_3(f, t, _ctx), _ctx); /*3926*/
    return kk_std_core_types__tuple3__unbox(_x20768, _ctx);
  }
  {
    kk_box_t x0_18161;
    kk_function_t _x20772 = kk_function_dup(f); /*(5775) -> 5777 5776*/
    kk_box_t _x20771;
    {
      kk_box_t _x0 = t.snd;
      kk_box_dup(_x0);
      _x20771 = _x0; /*5775*/
    }
    x0_18161 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x20772, (_x20772, _x20771, _ctx)); /*5776*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x0_18161, _ctx);
      kk_box_t _x20773 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun20774_3(f, t, x_18157, _ctx), _ctx); /*3926*/
      return kk_std_core_types__tuple3__unbox(_x20773, _ctx);
    }
    {
      kk_box_t x1_18165;
      kk_box_t _x20776;
      {
        kk_box_t _x1 = t.thd;
        kk_box_dup(_x1);
        kk_std_core_types__tuple3__drop(t, _ctx);
        _x20776 = _x1; /*5775*/
      }
      x1_18165 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, _x20776, _ctx)); /*5776*/
      if (kk_yielding(kk_context())) {
        kk_box_drop(x1_18165, _ctx);
        kk_box_t _x20777 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun20778_3(x_18157, x0_18161, _ctx), _ctx); /*3926*/
        return kk_std_core_types__tuple3__unbox(_x20777, _ctx);
      }
      {
        return kk_std_core_types__new_dash__lp__comma__comma__rp_(x_18157, x0_18161, x1_18165, _ctx);
      }
    }
  }
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift18008_map_fun20783__t_4 {
  struct kk_function_s _base;
  kk_box_t _y_17702;
  kk_box_t _y_17703;
  kk_box_t _y_17704;
};
static kk_box_t kk_std_core__mlift18008_map_fun20783_4(kk_function_t _fself, kk_box_t _b_18938, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift18008_map_fun20783_4(kk_box_t _y_17702, kk_box_t _y_17703, kk_box_t _y_17704, kk_context_t* _ctx) {
  struct kk_std_core__mlift18008_map_fun20783__t_4* _self = kk_function_alloc_as(struct kk_std_core__mlift18008_map_fun20783__t_4, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift18008_map_fun20783_4, kk_context());
  _self->_y_17702 = _y_17702;
  _self->_y_17703 = _y_17703;
  _self->_y_17704 = _y_17704;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift18008_map_fun20783_4(kk_function_t _fself, kk_box_t _b_18938, kk_context_t* _ctx) {
  struct kk_std_core__mlift18008_map_fun20783__t_4* _self = kk_function_as(struct kk_std_core__mlift18008_map_fun20783__t_4*, _fself);
  kk_box_t _y_17702 = _self->_y_17702; /* 6063 */
  kk_box_t _y_17703 = _self->_y_17703; /* 6063 */
  kk_box_t _y_17704 = _self->_y_17704; /* 6063 */
  kk_drop_match(_self, {kk_box_dup(_y_17702);kk_box_dup(_y_17703);kk_box_dup(_y_17704);}, {}, _ctx)
  kk_box_t _y_18940_17705 = _b_18938; /*6063*/;
  kk_std_core_types__tuple4_ _x20784 = kk_std_core_types__new_dash__lp__comma__comma__comma__rp_(kk_reuse_null, _y_17702, _y_17703, _y_17704, _y_18940_17705, _ctx); /*(1013, 1014, 1015, 1016)*/
  return kk_std_core_types__tuple4__box(_x20784, _ctx);
}

kk_std_core_types__tuple4_ kk_std_core__mlift18008_map_4(kk_box_t _y_17702, kk_box_t _y_17703, kk_function_t f, kk_std_core_types__tuple4_ t, kk_box_t _y_17704, kk_context_t* _ctx) { /* forall<a,b,e> (b, b, f : (a) -> e b, t : (a, a, a, a), b) -> e (b, b, b, b) */ 
  kk_box_t x_18172;
  kk_box_t _x20780;
  {
    struct kk_std_core_types__Tuple4_* _con20781 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(t);
    kk_box_t _pat00 = _con20781->fst;
    kk_box_t _pat10 = _con20781->snd;
    kk_box_t _pat2 = _con20781->thd;
    kk_box_t _x = _con20781->field4;
    if (kk_likely(kk_std_core_types__tuple4__is_unique(t))) {
      kk_box_drop(_pat2, _ctx);
      kk_box_drop(_pat10, _ctx);
      kk_box_drop(_pat00, _ctx);
      kk_std_core_types__tuple4__free(t, _ctx);
    }
    else {
      kk_box_dup(_x);
      kk_std_core_types__tuple4__decref(t, _ctx);
    }
    _x20780 = _x; /*6062*/
  }
  x_18172 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, _x20780, _ctx)); /*6063*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_18172, _ctx);
    kk_box_t _x20782 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift18008_map_fun20783_4(_y_17702, _y_17703, _y_17704, _ctx), _ctx); /*3926*/
    return kk_std_core_types__tuple4__unbox(_x20782, _ctx);
  }
  {
    return kk_std_core_types__new_dash__lp__comma__comma__comma__rp_(kk_reuse_null, _y_17702, _y_17703, _y_17704, x_18172, _ctx);
  }
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift18009_map_fun20789__t_4 {
  struct kk_function_s _base;
  kk_box_t _y_17702;
  kk_box_t _y_17703;
  kk_function_t f;
  kk_std_core_types__tuple4_ t;
};
static kk_box_t kk_std_core__mlift18009_map_fun20789_4(kk_function_t _fself, kk_box_t _b_18942, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift18009_map_fun20789_4(kk_box_t _y_17702, kk_box_t _y_17703, kk_function_t f, kk_std_core_types__tuple4_ t, kk_context_t* _ctx) {
  struct kk_std_core__mlift18009_map_fun20789__t_4* _self = kk_function_alloc_as(struct kk_std_core__mlift18009_map_fun20789__t_4, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift18009_map_fun20789_4, kk_context());
  _self->_y_17702 = _y_17702;
  _self->_y_17703 = _y_17703;
  _self->f = f;
  _self->t = t;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift18009_map_fun20789_4(kk_function_t _fself, kk_box_t _b_18942, kk_context_t* _ctx) {
  struct kk_std_core__mlift18009_map_fun20789__t_4* _self = kk_function_as(struct kk_std_core__mlift18009_map_fun20789__t_4*, _fself);
  kk_box_t _y_17702 = _self->_y_17702; /* 6063 */
  kk_box_t _y_17703 = _self->_y_17703; /* 6063 */
  kk_function_t f = _self->f; /* (6062) -> 6064 6063 */
  kk_std_core_types__tuple4_ t = _self->t; /* (6062, 6062, 6062, 6062) */
  kk_drop_match(_self, {kk_box_dup(_y_17702);kk_box_dup(_y_17703);kk_function_dup(f);kk_std_core_types__tuple4__dup(t);}, {}, _ctx)
  kk_box_t _y_18944_17704 = _b_18942; /*6063*/;
  kk_std_core_types__tuple4_ _x20790 = kk_std_core__mlift18008_map_4(_y_17702, _y_17703, f, t, _y_18944_17704, _ctx); /*(6063, 6063, 6063, 6063)*/
  return kk_std_core_types__tuple4__box(_x20790, _ctx);
}

kk_std_core_types__tuple4_ kk_std_core__mlift18009_map_4(kk_box_t _y_17702, kk_function_t f, kk_std_core_types__tuple4_ t, kk_box_t _y_17703, kk_context_t* _ctx) { /* forall<a,b,e> (b, f : (a) -> e b, t : (a, a, a, a), b) -> e (b, b, b, b) */ 
  kk_box_t x_18179;
  kk_function_t _x20787 = kk_function_dup(f); /*(6062) -> 6064 6063*/
  kk_box_t _x20785;
  {
    struct kk_std_core_types__Tuple4_* _con20786 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(t);
    kk_box_t _x = _con20786->thd;
    kk_box_dup(_x);
    _x20785 = _x; /*6062*/
  }
  x_18179 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x20787, (_x20787, _x20785, _ctx)); /*6063*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_18179, _ctx);
    kk_box_t _x20788 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift18009_map_fun20789_4(_y_17702, _y_17703, f, t, _ctx), _ctx); /*3926*/
    return kk_std_core_types__tuple4__unbox(_x20788, _ctx);
  }
  {
    return kk_std_core__mlift18008_map_4(_y_17702, _y_17703, f, t, x_18179, _ctx);
  }
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift18010_map_fun20795__t_4 {
  struct kk_function_s _base;
  kk_box_t _y_17702;
  kk_function_t f;
  kk_std_core_types__tuple4_ t;
};
static kk_box_t kk_std_core__mlift18010_map_fun20795_4(kk_function_t _fself, kk_box_t _b_18946, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift18010_map_fun20795_4(kk_box_t _y_17702, kk_function_t f, kk_std_core_types__tuple4_ t, kk_context_t* _ctx) {
  struct kk_std_core__mlift18010_map_fun20795__t_4* _self = kk_function_alloc_as(struct kk_std_core__mlift18010_map_fun20795__t_4, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift18010_map_fun20795_4, kk_context());
  _self->_y_17702 = _y_17702;
  _self->f = f;
  _self->t = t;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift18010_map_fun20795_4(kk_function_t _fself, kk_box_t _b_18946, kk_context_t* _ctx) {
  struct kk_std_core__mlift18010_map_fun20795__t_4* _self = kk_function_as(struct kk_std_core__mlift18010_map_fun20795__t_4*, _fself);
  kk_box_t _y_17702 = _self->_y_17702; /* 6063 */
  kk_function_t f = _self->f; /* (6062) -> 6064 6063 */
  kk_std_core_types__tuple4_ t = _self->t; /* (6062, 6062, 6062, 6062) */
  kk_drop_match(_self, {kk_box_dup(_y_17702);kk_function_dup(f);kk_std_core_types__tuple4__dup(t);}, {}, _ctx)
  kk_box_t _y_18948_17703 = _b_18946; /*6063*/;
  kk_std_core_types__tuple4_ _x20796 = kk_std_core__mlift18009_map_4(_y_17702, f, t, _y_18948_17703, _ctx); /*(6063, 6063, 6063, 6063)*/
  return kk_std_core_types__tuple4__box(_x20796, _ctx);
}

kk_std_core_types__tuple4_ kk_std_core__mlift18010_map_4(kk_function_t f, kk_std_core_types__tuple4_ t, kk_box_t _y_17702, kk_context_t* _ctx) { /* forall<a,b,e> (f : (a) -> e b, t : (a, a, a, a), b) -> e (b, b, b, b) */ 
  kk_box_t x_18182;
  kk_function_t _x20793 = kk_function_dup(f); /*(6062) -> 6064 6063*/
  kk_box_t _x20791;
  {
    struct kk_std_core_types__Tuple4_* _con20792 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(t);
    kk_box_t _x = _con20792->snd;
    kk_box_dup(_x);
    _x20791 = _x; /*6062*/
  }
  x_18182 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x20793, (_x20793, _x20791, _ctx)); /*6063*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_18182, _ctx);
    kk_box_t _x20794 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift18010_map_fun20795_4(_y_17702, f, t, _ctx), _ctx); /*3926*/
    return kk_std_core_types__tuple4__unbox(_x20794, _ctx);
  }
  {
    return kk_std_core__mlift18009_map_4(_y_17702, f, t, x_18182, _ctx);
  }
}


// lift anonymous function
struct kk_std_core_map_fun20801__t_4 {
  struct kk_function_s _base;
  kk_function_t f;
  kk_std_core_types__tuple4_ t;
};
static kk_box_t kk_std_core_map_fun20801_4(kk_function_t _fself, kk_box_t _b_18950, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20801_4(kk_function_t f, kk_std_core_types__tuple4_ t, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20801__t_4* _self = kk_function_alloc_as(struct kk_std_core_map_fun20801__t_4, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun20801_4, kk_context());
  _self->f = f;
  _self->t = t;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun20801_4(kk_function_t _fself, kk_box_t _b_18950, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20801__t_4* _self = kk_function_as(struct kk_std_core_map_fun20801__t_4*, _fself);
  kk_function_t f = _self->f; /* (6062) -> 6064 6063 */
  kk_std_core_types__tuple4_ t = _self->t; /* (6062, 6062, 6062, 6062) */
  kk_drop_match(_self, {kk_function_dup(f);kk_std_core_types__tuple4__dup(t);}, {}, _ctx)
  kk_box_t _y_18961_17702 = _b_18950; /*6063*/;
  kk_std_core_types__tuple4_ _x20802 = kk_std_core__mlift18010_map_4(f, t, _y_18961_17702, _ctx); /*(6063, 6063, 6063, 6063)*/
  return kk_std_core_types__tuple4__box(_x20802, _ctx);
}


// lift anonymous function
struct kk_std_core_map_fun20807__t_4 {
  struct kk_function_s _base;
  kk_function_t f;
  kk_std_core_types__tuple4_ t;
  kk_box_t x_18185;
};
static kk_box_t kk_std_core_map_fun20807_4(kk_function_t _fself, kk_box_t _b_18952, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20807_4(kk_function_t f, kk_std_core_types__tuple4_ t, kk_box_t x_18185, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20807__t_4* _self = kk_function_alloc_as(struct kk_std_core_map_fun20807__t_4, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun20807_4, kk_context());
  _self->f = f;
  _self->t = t;
  _self->x_18185 = x_18185;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun20807_4(kk_function_t _fself, kk_box_t _b_18952, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20807__t_4* _self = kk_function_as(struct kk_std_core_map_fun20807__t_4*, _fself);
  kk_function_t f = _self->f; /* (6062) -> 6064 6063 */
  kk_std_core_types__tuple4_ t = _self->t; /* (6062, 6062, 6062, 6062) */
  kk_box_t x_18185 = _self->x_18185; /* 6063 */
  kk_drop_match(_self, {kk_function_dup(f);kk_std_core_types__tuple4__dup(t);kk_box_dup(x_18185);}, {}, _ctx)
  kk_box_t _y_18962_17703 = _b_18952; /*6063*/;
  kk_std_core_types__tuple4_ _x20808 = kk_std_core__mlift18009_map_4(x_18185, f, t, _y_18962_17703, _ctx); /*(6063, 6063, 6063, 6063)*/
  return kk_std_core_types__tuple4__box(_x20808, _ctx);
}


// lift anonymous function
struct kk_std_core_map_fun20813__t_4 {
  struct kk_function_s _base;
  kk_function_t f;
  kk_std_core_types__tuple4_ t;
  kk_box_t x_18185;
  kk_box_t x0_18189;
};
static kk_box_t kk_std_core_map_fun20813_4(kk_function_t _fself, kk_box_t _b_18954, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20813_4(kk_function_t f, kk_std_core_types__tuple4_ t, kk_box_t x_18185, kk_box_t x0_18189, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20813__t_4* _self = kk_function_alloc_as(struct kk_std_core_map_fun20813__t_4, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun20813_4, kk_context());
  _self->f = f;
  _self->t = t;
  _self->x_18185 = x_18185;
  _self->x0_18189 = x0_18189;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun20813_4(kk_function_t _fself, kk_box_t _b_18954, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20813__t_4* _self = kk_function_as(struct kk_std_core_map_fun20813__t_4*, _fself);
  kk_function_t f = _self->f; /* (6062) -> 6064 6063 */
  kk_std_core_types__tuple4_ t = _self->t; /* (6062, 6062, 6062, 6062) */
  kk_box_t x_18185 = _self->x_18185; /* 6063 */
  kk_box_t x0_18189 = _self->x0_18189; /* 6063 */
  kk_drop_match(_self, {kk_function_dup(f);kk_std_core_types__tuple4__dup(t);kk_box_dup(x_18185);kk_box_dup(x0_18189);}, {}, _ctx)
  kk_box_t _y_18963_17704 = _b_18954; /*6063*/;
  kk_std_core_types__tuple4_ _x20814 = kk_std_core__mlift18008_map_4(x_18185, x0_18189, f, t, _y_18963_17704, _ctx); /*(6063, 6063, 6063, 6063)*/
  return kk_std_core_types__tuple4__box(_x20814, _ctx);
}


// lift anonymous function
struct kk_std_core_map_fun20818__t_4 {
  struct kk_function_s _base;
  kk_box_t x_18185;
  kk_box_t x0_18189;
  kk_box_t x1_18193;
};
static kk_box_t kk_std_core_map_fun20818_4(kk_function_t _fself, kk_box_t _b_18956, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20818_4(kk_box_t x_18185, kk_box_t x0_18189, kk_box_t x1_18193, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20818__t_4* _self = kk_function_alloc_as(struct kk_std_core_map_fun20818__t_4, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun20818_4, kk_context());
  _self->x_18185 = x_18185;
  _self->x0_18189 = x0_18189;
  _self->x1_18193 = x1_18193;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun20818_4(kk_function_t _fself, kk_box_t _b_18956, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20818__t_4* _self = kk_function_as(struct kk_std_core_map_fun20818__t_4*, _fself);
  kk_box_t x_18185 = _self->x_18185; /* 6063 */
  kk_box_t x0_18189 = _self->x0_18189; /* 6063 */
  kk_box_t x1_18193 = _self->x1_18193; /* 6063 */
  kk_drop_match(_self, {kk_box_dup(x_18185);kk_box_dup(x0_18189);kk_box_dup(x1_18193);}, {}, _ctx)
  kk_box_t _y_18964_17705 = _b_18956; /*6063*/;
  kk_std_core_types__tuple4_ _x20819 = kk_std_core_types__new_dash__lp__comma__comma__comma__rp_(kk_reuse_null, x_18185, x0_18189, x1_18193, _y_18964_17705, _ctx); /*(1013, 1014, 1015, 1016)*/
  return kk_std_core_types__tuple4__box(_x20819, _ctx);
}

kk_std_core_types__tuple4_ kk_std_core_map_4(kk_std_core_types__tuple4_ t, kk_function_t f, kk_context_t* _ctx) { /* forall<a,b,e> (t : (a, a, a, a), f : (a) -> e b) -> e (b, b, b, b) */ 
  kk_box_t x_18185;
  kk_function_t _x20799 = kk_function_dup(f); /*(6062) -> 6064 6063*/
  kk_box_t _x20797;
  {
    struct kk_std_core_types__Tuple4_* _con20798 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(t);
    kk_box_t _x = _con20798->fst;
    kk_box_dup(_x);
    _x20797 = _x; /*6062*/
  }
  x_18185 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x20799, (_x20799, _x20797, _ctx)); /*6063*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_18185, _ctx);
    kk_box_t _x20800 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun20801_4(f, t, _ctx), _ctx); /*3926*/
    return kk_std_core_types__tuple4__unbox(_x20800, _ctx);
  }
  {
    kk_box_t x0_18189;
    kk_function_t _x20805 = kk_function_dup(f); /*(6062) -> 6064 6063*/
    kk_box_t _x20803;
    {
      struct kk_std_core_types__Tuple4_* _con20804 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(t);
      kk_box_t _x0 = _con20804->snd;
      kk_box_dup(_x0);
      _x20803 = _x0; /*6062*/
    }
    x0_18189 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x20805, (_x20805, _x20803, _ctx)); /*6063*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x0_18189, _ctx);
      kk_box_t _x20806 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun20807_4(f, t, x_18185, _ctx), _ctx); /*3926*/
      return kk_std_core_types__tuple4__unbox(_x20806, _ctx);
    }
    {
      kk_box_t x1_18193;
      kk_function_t _x20811 = kk_function_dup(f); /*(6062) -> 6064 6063*/
      kk_box_t _x20809;
      {
        struct kk_std_core_types__Tuple4_* _con20810 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(t);
        kk_box_t _x1 = _con20810->thd;
        kk_box_dup(_x1);
        _x20809 = _x1; /*6062*/
      }
      x1_18193 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x20811, (_x20811, _x20809, _ctx)); /*6063*/
      if (kk_yielding(kk_context())) {
        kk_box_drop(x1_18193, _ctx);
        kk_box_t _x20812 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun20813_4(f, t, x_18185, x0_18189, _ctx), _ctx); /*3926*/
        return kk_std_core_types__tuple4__unbox(_x20812, _ctx);
      }
      {
        kk_box_t x2_18197;
        kk_box_t _x20815;
        {
          struct kk_std_core_types__Tuple4_* _con20816 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(t);
          kk_box_t _pat06 = _con20816->fst;
          kk_box_t _pat13 = _con20816->snd;
          kk_box_t _pat22 = _con20816->thd;
          kk_box_t _x2 = _con20816->field4;
          if (kk_likely(kk_std_core_types__tuple4__is_unique(t))) {
            kk_box_drop(_pat22, _ctx);
            kk_box_drop(_pat13, _ctx);
            kk_box_drop(_pat06, _ctx);
            kk_std_core_types__tuple4__free(t, _ctx);
          }
          else {
            kk_box_dup(_x2);
            kk_std_core_types__tuple4__decref(t, _ctx);
          }
          _x20815 = _x2; /*6062*/
        }
        x2_18197 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, _x20815, _ctx)); /*6063*/
        if (kk_yielding(kk_context())) {
          kk_box_drop(x2_18197, _ctx);
          kk_box_t _x20817 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun20818_4(x_18185, x0_18189, x1_18193, _ctx), _ctx); /*3926*/
          return kk_std_core_types__tuple4__unbox(_x20817, _ctx);
        }
        {
          return kk_std_core_types__new_dash__lp__comma__comma__comma__rp_(kk_reuse_null, x_18185, x0_18189, x1_18193, x2_18197, _ctx);
        }
      }
    }
  }
}
 
// Apply a function `f` to each character in a string


// lift anonymous function
struct kk_std_core_map_fun20820__t_6 {
  struct kk_function_s _base;
  kk_function_t f;
};
static kk_box_t kk_std_core_map_fun20820_6(kk_function_t _fself, kk_box_t _b_18967, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20820_6(kk_function_t f, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20820__t_6* _self = kk_function_alloc_as(struct kk_std_core_map_fun20820__t_6, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun20820_6, kk_context());
  _self->f = f;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun20820_6(kk_function_t _fself, kk_box_t _b_18967, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20820__t_6* _self = kk_function_as(struct kk_std_core_map_fun20820__t_6*, _fself);
  kk_function_t f = _self->f; /* (char) -> 6531 char */
  kk_drop_match(_self, {kk_function_dup(f);}, {}, _ctx)
  kk_char_t _x20821;
  kk_char_t _x20822 = kk_char_unbox(_b_18967, _ctx); /*char*/
  _x20821 = kk_function_call(kk_char_t, (kk_function_t, kk_char_t, kk_context_t*), f, (f, _x20822, _ctx)); /*char*/
  return kk_char_box(_x20821, _ctx);
}


// lift anonymous function
struct kk_std_core_map_fun20824__t_6 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_map_fun20824_6(kk_function_t _fself, kk_box_t _b_18971, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20824_6(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_map_fun20824_6, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_map_fun20824_6(kk_function_t _fself, kk_box_t _b_18971, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_string_t _x20825;
  kk_std_core__list _x20826 = kk_std_core__list_unbox(_b_18971, _ctx); /*list<char>*/
  _x20825 = kk_std_core_string_2(_x20826, _ctx); /*string*/
  return kk_string_box(_x20825);
}

kk_string_t kk_std_core_map_6(kk_string_t s, kk_function_t f, kk_context_t* _ctx) { /* forall<e> (s : string, f : (char) -> e char) -> e string */ 
  kk_std_core__list _b_18968_18965 = kk_std_core_list_6(s, _ctx); /*list<char>*/;
  kk_std_core__list x_18205 = kk_std_core_map_5(_b_18968_18965, kk_std_core_new_map_fun20820_6(f, _ctx), _ctx); /*list<char>*/;
  if (kk_yielding(kk_context())) {
    kk_std_core__list_drop(x_18205, _ctx);
    kk_box_t _x20823 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun20824_6(_ctx), _ctx); /*3926*/
    return kk_string_unbox(_x20823);
  }
  {
    return kk_std_core_string_2(x_18205, _ctx);
  }
}
 
// monadic lift

kk_unit_t kk_std_core__mlift18011_op(kk_ssize_t end_17021, kk_function_t f, kk_ssize_t i, kk_vector_t v, kk_vector_t w, kk_box_t _y_17707, kk_context_t* _ctx) { /* forall<a,b,e> (end.17021 : ssize_t, f : (a) -> e b, i : ssize_t, v : vector<a>, w : vector<b>, b) -> e () */ 
  kk_unit_t __ = kk_Unit;
  kk_vector_t _x20827 = kk_vector_dup(w); /*vector<6580>*/
  kk_vector_unsafe_assign(_x20827,i,_y_17707,kk_context());
  kk_ssize_t i0_17453 = kk_std_core_incr_1(i, _ctx); /*ssize_t*/;
  kk_std_core__lift17282_map_7(end_17021, f, v, w, i0_17453, _ctx); return kk_Unit;
}
 
// lifted local: map.7, rep


// lift anonymous function
struct kk_std_core__lift17282_map_fun20830__t_7 {
  struct kk_function_s _base;
  kk_function_t f0;
  kk_vector_t v0;
  kk_vector_t w0;
  kk_ssize_t end_170210;
  kk_ssize_t i0;
};
static kk_box_t kk_std_core__lift17282_map_fun20830_7(kk_function_t _fself, kk_box_t _b_18974, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17282_map_fun20830_7(kk_function_t f0, kk_vector_t v0, kk_vector_t w0, kk_ssize_t end_170210, kk_ssize_t i0, kk_context_t* _ctx) {
  struct kk_std_core__lift17282_map_fun20830__t_7* _self = kk_function_alloc_as(struct kk_std_core__lift17282_map_fun20830__t_7, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift17282_map_fun20830_7, kk_context());
  _self->f0 = f0;
  _self->v0 = v0;
  _self->w0 = w0;
  _self->end_170210 = end_170210;
  _self->i0 = i0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift17282_map_fun20830_7(kk_function_t _fself, kk_box_t _b_18974, kk_context_t* _ctx) {
  struct kk_std_core__lift17282_map_fun20830__t_7* _self = kk_function_as(struct kk_std_core__lift17282_map_fun20830__t_7*, _fself);
  kk_function_t f0 = _self->f0; /* (6579) -> 6581 6580 */
  kk_vector_t v0 = _self->v0; /* vector<6579> */
  kk_vector_t w0 = _self->w0; /* vector<6580> */
  kk_ssize_t end_170210 = _self->end_170210; /* ssize_t */
  kk_ssize_t i0 = _self->i0; /* ssize_t */
  kk_drop_match(_self, {kk_function_dup(f0);kk_vector_dup(v0);kk_vector_dup(w0);;;}, {}, _ctx)
  kk_box_t _y_18976_177070 = _b_18974; /*6580*/;
  kk_unit_t _x20831 = kk_Unit;
  kk_std_core__mlift18011_op(end_170210, f0, i0, v0, w0, _y_18976_177070, _ctx);
  return kk_unit_box(_x20831);
}

kk_unit_t kk_std_core__lift17282_map_7(kk_ssize_t end_170210, kk_function_t f0, kk_vector_t v0, kk_vector_t w0, kk_ssize_t i0, kk_context_t* _ctx) { /* forall<a,b,e> (end.17021 : ssize_t, f : (a) -> e b, v : vector<a>, w : vector<b>, i : ssize_t) -> e () */ 
  kk__tailcall: ;
  bool _match_20259 = (i0 <= end_170210); /*bool*/;
  if (_match_20259) {
    kk_box_t x_17256 = kk_vector_at_borrow(v0,i0); /*6579*/;
    kk_box_t x_18207;
    kk_function_t _x20828 = kk_function_dup(f0); /*(6579) -> 6581 6580*/
    x_18207 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x20828, (_x20828, x_17256, _ctx)); /*6580*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x_18207, _ctx);
      kk_box_t _x20829 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift17282_map_fun20830_7(f0, v0, w0, end_170210, i0, _ctx), _ctx); /*3926*/
      kk_unit_unbox(_x20829); return kk_Unit;
    }
    {
      kk_unit_t __0 = kk_Unit;
      kk_vector_t _x20832 = kk_vector_dup(w0); /*vector<6580>*/
      kk_vector_unsafe_assign(_x20832,i0,x_18207,kk_context());
      kk_ssize_t i0_174530 = kk_std_core_incr_1(i0, _ctx); /*ssize_t*/;
      { // tailcall
        i0 = i0_174530;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_vector_drop(w0, _ctx);
    kk_vector_drop(v0, _ctx);
    kk_function_drop(f0, _ctx);
    kk_Unit; return kk_Unit;
  }
}
 
// Apply a total function `f` to each element in a vector `v`


// lift anonymous function
struct kk_std_core_map_fun20839__t_7 {
  struct kk_function_s _base;
  kk_vector_t w;
};
static kk_box_t kk_std_core_map_fun20839_7(kk_function_t _fself, kk_box_t _b_18978, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20839_7(kk_vector_t w, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20839__t_7* _self = kk_function_alloc_as(struct kk_std_core_map_fun20839__t_7, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun20839_7, kk_context());
  _self->w = w;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun20839_7(kk_function_t _fself, kk_box_t _b_18978, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20839__t_7* _self = kk_function_as(struct kk_std_core_map_fun20839__t_7*, _fself);
  kk_vector_t w = _self->w; /* vector<6580> */
  kk_drop_match(_self, {kk_vector_dup(w);}, {}, _ctx)
  kk_unit_t wild__0_18980 = kk_Unit;
  kk_unit_unbox(_b_18978);
  return kk_vector_box(w, _ctx);
}

kk_vector_t kk_std_core_map_7(kk_vector_t v, kk_function_t f, kk_context_t* _ctx) { /* forall<a,b,e> (v : vector<a>, f : (a) -> e b) -> e vector<b> */ 
  kk_vector_t w;
  kk_ssize_t _x20833;
  kk_integer_t _x20834;
  kk_ssize_t _x20835 = kk_vector_len_borrow(v); /*ssize_t*/
  _x20834 = kk_integer_from_ssize_t(_x20835,kk_context()); /*int*/
  _x20833 = kk_std_core_ssize__t(_x20834, _ctx); /*ssize_t*/
  w = kk_vector_alloc_uninit(_x20833,NULL,kk_context()); /*vector<6580>*/
  kk_ssize_t start0_17020 = (KK_IZ(0)); /*ssize_t*/;
  kk_ssize_t end_17021;
  kk_ssize_t _x20836 = kk_vector_len_borrow(v); /*ssize_t*/
  end_17021 = kk_std_core_decr_1(_x20836, _ctx); /*ssize_t*/
  kk_unit_t x_18211 = kk_Unit;
  kk_vector_t _x20837 = kk_vector_dup(w); /*vector<6580>*/
  kk_std_core__lift17282_map_7(end_17021, f, v, _x20837, start0_17020, _ctx);
  if (kk_yielding(kk_context())) {
    kk_box_t _x20838 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun20839_7(w, _ctx), _ctx); /*3926*/
    return kk_vector_unbox(_x20838, _ctx);
  }
  {
    return w;
  }
}
 
// Right-align a string to width `width`  using `fill`  (default is a space) to fill from the left.

kk_string_t kk_std_core_pad_left(kk_string_t s, kk_integer_t width, kk_std_core_types__optional fill, kk_context_t* _ctx) { /* (s : string, width : int, fill : optional<char>) -> string */ 
  kk_ssize_t w;
  kk_integer_t _x20840 = kk_integer_dup(width); /*int*/
  w = kk_std_core_ssize__t(_x20840, _ctx); /*ssize_t*/
  kk_ssize_t n;
  kk_string_t _x20841 = kk_string_dup(s); /*string*/
  n = kk_string_len(_x20841,kk_context()); /*ssize_t*/
  bool _match_20257 = (w <= n); /*bool*/;
  if (_match_20257) {
    kk_std_core_types__optional_drop(fill, _ctx);
    return s;
  }
  {
    kk_string_t _x20842;
    kk_string_t _x20843;
    kk_char_t _x20844;
    if (kk_std_core_types__is_Optional(fill)) {
      kk_box_t _box_x18981 = fill._cons.Optional.value;
      kk_char_t _fill_7329 = kk_char_unbox(_box_x18981, NULL);
      _x20844 = _fill_7329; /*char*/
    }
    else {
      _x20844 = ' '; /*char*/
    }
    _x20843 = kk_std_core_string(_x20844, _ctx); /*string*/
    kk_ssize_t _x20846 = (w - n); /*ssize_t*/
    _x20842 = kk_std_core_repeatz(_x20843, _x20846, _ctx); /*string*/
    return kk_std_core__lp__plus__plus__1_rp_(_x20842, s, _ctx);
  }
}
 
// Show an `:int` as a hexadecimal value.
// The `width`  parameter specifies how wide the hex value is where `"0"`  is used to align.
// The `use-capitals` parameter (= `True`) determines if captical letters should be used to display the hexadecimal digits.
// The `pre` (=`"0x"`) is an optional prefix for the number (goes between the sign and the number).

kk_string_t kk_std_core_show_hex(kk_integer_t i, kk_std_core_types__optional width, kk_std_core_types__optional use_capitals, kk_std_core_types__optional pre, kk_context_t* _ctx) { /* (i : int, width : optional<int>, use-capitals : optional<bool>, pre : optional<string>) -> string */ 
  kk_string_t s_17023;
  kk_integer_t _x20847;
  kk_integer_t _x20848 = kk_integer_dup(i); /*int*/
  _x20847 = kk_integer_abs(_x20848,kk_context()); /*int*/
  bool _x20849;
  if (kk_std_core_types__is_Optional(use_capitals)) {
    kk_box_t _box_x18982 = use_capitals._cons.Optional.value;
    bool _use_capitals_7433 = kk_bool_unbox(_box_x18982);
    _x20849 = _use_capitals_7433; /*bool*/
  }
  else {
    _x20849 = true; /*bool*/
  }
  s_17023 = kk_std_core_int_show_hex(_x20847, _x20849, _ctx); /*string*/
  kk_ssize_t w;
  kk_integer_t _x20851;
  if (kk_std_core_types__is_Optional(width)) {
    kk_box_t _box_x18983 = width._cons.Optional.value;
    kk_integer_t _width_7429 = kk_integer_unbox(_box_x18983);
    _x20851 = _width_7429; /*int*/
  }
  else {
    _x20851 = kk_integer_from_small(1); /*int*/
  }
  w = kk_std_core_ssize__t(_x20851, _ctx); /*ssize_t*/
  kk_ssize_t n;
  kk_string_t _x20853 = kk_string_dup(s_17023); /*string*/
  n = kk_string_len(_x20853,kk_context()); /*ssize_t*/
  kk_string_t _x20854;
  bool _match_20255;
  bool _brw_20256 = kk_integer_lt_borrow(i,(kk_integer_from_small(0)),kk_context()); /*bool*/;
  kk_integer_drop(i, _ctx);
  _match_20255 = _brw_20256; /*bool*/
  if (_match_20255) {
    kk_define_string_literal(, _s20855, 1, "-")
    _x20854 = kk_string_dup(_s20855); /*string*/
  }
  else {
    _x20854 = kk_string_empty(); /*string*/
  }
  kk_string_t _x20857;
  kk_string_t _x20858;
  if (kk_std_core_types__is_Optional(pre)) {
    kk_box_t _box_x18984 = pre._cons.Optional.value;
    kk_string_t _pre_7437 = kk_string_unbox(_box_x18984);
    _x20858 = _pre_7437; /*string*/
  }
  else {
    kk_define_string_literal(, _s20860, 2, "0x")
    _x20858 = kk_string_dup(_s20860); /*string*/
  }
  kk_string_t _x20861;
  bool _match_20254 = (w <= n); /*bool*/;
  if (_match_20254) {
    _x20861 = s_17023; /*string*/
  }
  else {
    kk_string_t _x20862;
    kk_string_t _x20863 = kk_std_core_string('0', _ctx); /*string*/
    kk_ssize_t _x20864 = (w - n); /*ssize_t*/
    _x20862 = kk_std_core_repeatz(_x20863, _x20864, _ctx); /*string*/
    _x20861 = kk_std_core__lp__plus__plus__1_rp_(_x20862, s_17023, _ctx); /*string*/
  }
  _x20857 = kk_std_core__lp__plus__plus__1_rp_(_x20858, _x20861, _ctx); /*string*/
  return kk_std_core__lp__plus__plus__1_rp_(_x20854, _x20857, _ctx);
}
 
// Is the integer positive (stricly greater than zero)

bool kk_std_core_is_pos_2(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> bool */ 
  kk_std_core_types__order x_17026;
  kk_std_core_types__order _brw_20251 = kk_int_as_order(kk_integer_signum_borrow(i),kk_context()); /*order*/;
  kk_integer_drop(i, _ctx);
  x_17026 = _brw_20251; /*order*/
  kk_integer_t _brw_20249;
  if (kk_std_core_types__is_Lt(x_17026)) {
    _brw_20249 = kk_integer_from_small(-1); /*int*/
    goto _match20865;
  }
  if (kk_std_core_types__is_Eq(x_17026)) {
    _brw_20249 = kk_integer_from_small(0); /*int*/
    goto _match20865;
  }
  {
    _brw_20249 = kk_integer_from_small(1); /*int*/
  }
  _match20865: ;
  bool _brw_20250 = kk_integer_eq_borrow(_brw_20249,(kk_integer_from_small(1)),kk_context()); /*bool*/;
  kk_integer_drop(_brw_20249, _ctx);
  return _brw_20250;
}
 
// Show a character as a string

kk_string_t kk_std_core_show_char(kk_char_t c, kk_context_t* _ctx) { /* (c : char) -> string */ 
  bool _match_20199 = (c < (' ')); /*bool*/;
  if (_match_20199) {
    bool _match_20226 = (c == 0x000A); /*bool*/;
    if (_match_20226) {
      kk_define_string_literal(, _s20872, 2, "\\n")
      return kk_string_dup(_s20872);
    }
    {
      bool _match_20227 = (c == 0x000D); /*bool*/;
      if (_match_20227) {
        kk_define_string_literal(, _s20873, 2, "\\r")
        return kk_string_dup(_s20873);
      }
      {
        bool _match_20228 = (c == 0x0009); /*bool*/;
        if (_match_20228) {
          kk_define_string_literal(, _s20874, 2, "\\t")
          return kk_string_dup(_s20874);
        }
        {
          bool _match_20229;
          kk_integer_t _brw_20246 = kk_integer_from_int(c,kk_context()); /*int*/;
          bool _brw_20247 = kk_integer_lte_borrow(_brw_20246,(kk_integer_from_small(255)),kk_context()); /*bool*/;
          kk_integer_drop(_brw_20246, _ctx);
          _match_20229 = _brw_20247; /*bool*/
          if (_match_20229) {
            kk_integer_t _arg_7906 = kk_integer_from_int(c,kk_context()); /*int*/;
            kk_string_t s_17037;
            kk_integer_t _x20875;
            kk_integer_t _x20876 = kk_integer_dup(_arg_7906); /*int*/
            _x20875 = kk_integer_abs(_x20876,kk_context()); /*int*/
            bool _x20877;
            kk_std_core_types__optional _x20878 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
            if (kk_std_core_types__is_Optional(_x20878)) {
              kk_box_t _box_x18985 = _x20878._cons.Optional.value;
              bool _use_capitals_7433 = kk_bool_unbox(_box_x18985);
              _x20877 = _use_capitals_7433; /*bool*/
            }
            else {
              _x20877 = true; /*bool*/
            }
            s_17037 = kk_std_core_int_show_hex(_x20875, _x20877, _ctx); /*string*/
            kk_ssize_t w = (KK_IZ(2)); /*ssize_t*/;
            kk_ssize_t n;
            kk_string_t _x20880 = kk_string_dup(s_17037); /*string*/
            n = kk_string_len(_x20880,kk_context()); /*ssize_t*/
            kk_string_t _x20881;
            kk_define_string_literal(, _s20882, 2, "\\x")
            _x20881 = kk_string_dup(_s20882); /*string*/
            kk_string_t _x20883;
            kk_string_t _x20884;
            bool _match_20243;
            bool _brw_20244 = kk_integer_lt_borrow(_arg_7906,(kk_integer_from_small(0)),kk_context()); /*bool*/;
            kk_integer_drop(_arg_7906, _ctx);
            _match_20243 = _brw_20244; /*bool*/
            if (_match_20243) {
              kk_define_string_literal(, _s20885, 1, "-")
              _x20884 = kk_string_dup(_s20885); /*string*/
            }
            else {
              _x20884 = kk_string_empty(); /*string*/
            }
            kk_string_t _x20887;
            kk_string_t _x20888 = kk_string_empty(); /*string*/
            kk_string_t _x20890;
            bool _match_20242 = (w <= n); /*bool*/;
            if (_match_20242) {
              _x20890 = s_17037; /*string*/
            }
            else {
              kk_string_t _x20891;
              kk_string_t _x20892 = kk_std_core_string('0', _ctx); /*string*/
              kk_ssize_t _x20893 = (w - n); /*ssize_t*/
              _x20891 = kk_std_core_repeatz(_x20892, _x20893, _ctx); /*string*/
              _x20890 = kk_std_core__lp__plus__plus__1_rp_(_x20891, s_17037, _ctx); /*string*/
            }
            _x20887 = kk_std_core__lp__plus__plus__1_rp_(_x20888, _x20890, _ctx); /*string*/
            _x20883 = kk_std_core__lp__plus__plus__1_rp_(_x20884, _x20887, _ctx); /*string*/
            return kk_std_core__lp__plus__plus__1_rp_(_x20881, _x20883, _ctx);
          }
          {
            bool _match_20230;
            kk_integer_t _brw_20240 = kk_integer_from_int(c,kk_context()); /*int*/;
            kk_integer_t _brw_20239 = kk_integer_from_int(65535, _ctx); /*int*/;
            bool _brw_20241 = kk_integer_lte_borrow(_brw_20240,_brw_20239,kk_context()); /*bool*/;
            kk_integer_drop(_brw_20240, _ctx);
            kk_integer_drop(_brw_20239, _ctx);
            _match_20230 = _brw_20241; /*bool*/
            if (_match_20230) {
              kk_integer_t _arg_7994 = kk_integer_from_int(c,kk_context()); /*int*/;
              kk_string_t s0_17044;
              kk_integer_t _x20894;
              kk_integer_t _x20895 = kk_integer_dup(_arg_7994); /*int*/
              _x20894 = kk_integer_abs(_x20895,kk_context()); /*int*/
              bool _x20896;
              kk_std_core_types__optional _x20897 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
              if (kk_std_core_types__is_Optional(_x20897)) {
                kk_box_t _box_x18986 = _x20897._cons.Optional.value;
                bool _use_capitals_74330 = kk_bool_unbox(_box_x18986);
                _x20896 = _use_capitals_74330; /*bool*/
              }
              else {
                _x20896 = true; /*bool*/
              }
              s0_17044 = kk_std_core_int_show_hex(_x20894, _x20896, _ctx); /*string*/
              kk_ssize_t w0 = (KK_IZ(4)); /*ssize_t*/;
              kk_ssize_t n0;
              kk_string_t _x20899 = kk_string_dup(s0_17044); /*string*/
              n0 = kk_string_len(_x20899,kk_context()); /*ssize_t*/
              kk_string_t _x20900;
              kk_define_string_literal(, _s20901, 2, "\\u")
              _x20900 = kk_string_dup(_s20901); /*string*/
              kk_string_t _x20902;
              kk_string_t _x20903;
              bool _match_20236;
              bool _brw_20237 = kk_integer_lt_borrow(_arg_7994,(kk_integer_from_small(0)),kk_context()); /*bool*/;
              kk_integer_drop(_arg_7994, _ctx);
              _match_20236 = _brw_20237; /*bool*/
              if (_match_20236) {
                kk_define_string_literal(, _s20904, 1, "-")
                _x20903 = kk_string_dup(_s20904); /*string*/
              }
              else {
                _x20903 = kk_string_empty(); /*string*/
              }
              kk_string_t _x20906;
              kk_string_t _x20907 = kk_string_empty(); /*string*/
              kk_string_t _x20909;
              bool _match_20235 = (w0 <= n0); /*bool*/;
              if (_match_20235) {
                _x20909 = s0_17044; /*string*/
              }
              else {
                kk_string_t _x20910;
                kk_string_t _x20911 = kk_std_core_string('0', _ctx); /*string*/
                kk_ssize_t _x20912 = (w0 - n0); /*ssize_t*/
                _x20910 = kk_std_core_repeatz(_x20911, _x20912, _ctx); /*string*/
                _x20909 = kk_std_core__lp__plus__plus__1_rp_(_x20910, s0_17044, _ctx); /*string*/
              }
              _x20906 = kk_std_core__lp__plus__plus__1_rp_(_x20907, _x20909, _ctx); /*string*/
              _x20902 = kk_std_core__lp__plus__plus__1_rp_(_x20903, _x20906, _ctx); /*string*/
              return kk_std_core__lp__plus__plus__1_rp_(_x20900, _x20902, _ctx);
            }
            {
              kk_integer_t _arg_8038 = kk_integer_from_int(c,kk_context()); /*int*/;
              kk_string_t s1_17051;
              kk_integer_t _x20913;
              kk_integer_t _x20914 = kk_integer_dup(_arg_8038); /*int*/
              _x20913 = kk_integer_abs(_x20914,kk_context()); /*int*/
              bool _x20915;
              kk_std_core_types__optional _x20916 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
              if (kk_std_core_types__is_Optional(_x20916)) {
                kk_box_t _box_x18987 = _x20916._cons.Optional.value;
                bool _use_capitals_74331 = kk_bool_unbox(_box_x18987);
                _x20915 = _use_capitals_74331; /*bool*/
              }
              else {
                _x20915 = true; /*bool*/
              }
              s1_17051 = kk_std_core_int_show_hex(_x20913, _x20915, _ctx); /*string*/
              kk_ssize_t w1 = (KK_IZ(6)); /*ssize_t*/;
              kk_ssize_t n1;
              kk_string_t _x20918 = kk_string_dup(s1_17051); /*string*/
              n1 = kk_string_len(_x20918,kk_context()); /*ssize_t*/
              kk_string_t _x20919;
              kk_define_string_literal(, _s20920, 2, "\\U")
              _x20919 = kk_string_dup(_s20920); /*string*/
              kk_string_t _x20921;
              kk_string_t _x20922;
              bool _match_20232;
              bool _brw_20233 = kk_integer_lt_borrow(_arg_8038,(kk_integer_from_small(0)),kk_context()); /*bool*/;
              kk_integer_drop(_arg_8038, _ctx);
              _match_20232 = _brw_20233; /*bool*/
              if (_match_20232) {
                kk_define_string_literal(, _s20923, 1, "-")
                _x20922 = kk_string_dup(_s20923); /*string*/
              }
              else {
                _x20922 = kk_string_empty(); /*string*/
              }
              kk_string_t _x20925;
              kk_string_t _x20926 = kk_string_empty(); /*string*/
              kk_string_t _x20928;
              bool _match_20231 = (w1 <= n1); /*bool*/;
              if (_match_20231) {
                _x20928 = s1_17051; /*string*/
              }
              else {
                kk_string_t _x20929;
                kk_string_t _x20930 = kk_std_core_string('0', _ctx); /*string*/
                kk_ssize_t _x20931 = (w1 - n1); /*ssize_t*/
                _x20929 = kk_std_core_repeatz(_x20930, _x20931, _ctx); /*string*/
                _x20928 = kk_std_core__lp__plus__plus__1_rp_(_x20929, s1_17051, _ctx); /*string*/
              }
              _x20925 = kk_std_core__lp__plus__plus__1_rp_(_x20926, _x20928, _ctx); /*string*/
              _x20921 = kk_std_core__lp__plus__plus__1_rp_(_x20922, _x20925, _ctx); /*string*/
              return kk_std_core__lp__plus__plus__1_rp_(_x20919, _x20921, _ctx);
            }
          }
        }
      }
    }
  }
  {
    bool _match_20200 = (c > ('~')); /*bool*/;
    if (_match_20200) {
      bool _match_20204 = (c == 0x000A); /*bool*/;
      if (_match_20204) {
        kk_define_string_literal(, _s20932, 2, "\\n")
        return kk_string_dup(_s20932);
      }
      {
        bool _match_20205 = (c == 0x000D); /*bool*/;
        if (_match_20205) {
          kk_define_string_literal(, _s20933, 2, "\\r")
          return kk_string_dup(_s20933);
        }
        {
          bool _match_20206 = (c == 0x0009); /*bool*/;
          if (_match_20206) {
            kk_define_string_literal(, _s20934, 2, "\\t")
            return kk_string_dup(_s20934);
          }
          {
            bool _match_20207;
            kk_integer_t _brw_20224 = kk_integer_from_int(c,kk_context()); /*int*/;
            bool _brw_20225 = kk_integer_lte_borrow(_brw_20224,(kk_integer_from_small(255)),kk_context()); /*bool*/;
            kk_integer_drop(_brw_20224, _ctx);
            _match_20207 = _brw_20225; /*bool*/
            if (_match_20207) {
              kk_integer_t _arg_79060 = kk_integer_from_int(c,kk_context()); /*int*/;
              kk_string_t s2_17058;
              kk_integer_t _x20935;
              kk_integer_t _x20936 = kk_integer_dup(_arg_79060); /*int*/
              _x20935 = kk_integer_abs(_x20936,kk_context()); /*int*/
              bool _x20937;
              kk_std_core_types__optional _x20938 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
              if (kk_std_core_types__is_Optional(_x20938)) {
                kk_box_t _box_x18988 = _x20938._cons.Optional.value;
                bool _use_capitals_74332 = kk_bool_unbox(_box_x18988);
                _x20937 = _use_capitals_74332; /*bool*/
              }
              else {
                _x20937 = true; /*bool*/
              }
              s2_17058 = kk_std_core_int_show_hex(_x20935, _x20937, _ctx); /*string*/
              kk_ssize_t w2 = (KK_IZ(2)); /*ssize_t*/;
              kk_ssize_t n2;
              kk_string_t _x20940 = kk_string_dup(s2_17058); /*string*/
              n2 = kk_string_len(_x20940,kk_context()); /*ssize_t*/
              kk_string_t _x20941;
              kk_define_string_literal(, _s20942, 2, "\\x")
              _x20941 = kk_string_dup(_s20942); /*string*/
              kk_string_t _x20943;
              kk_string_t _x20944;
              bool _match_20221;
              bool _brw_20222 = kk_integer_lt_borrow(_arg_79060,(kk_integer_from_small(0)),kk_context()); /*bool*/;
              kk_integer_drop(_arg_79060, _ctx);
              _match_20221 = _brw_20222; /*bool*/
              if (_match_20221) {
                kk_define_string_literal(, _s20945, 1, "-")
                _x20944 = kk_string_dup(_s20945); /*string*/
              }
              else {
                _x20944 = kk_string_empty(); /*string*/
              }
              kk_string_t _x20947;
              kk_string_t _x20948 = kk_string_empty(); /*string*/
              kk_string_t _x20950;
              bool _match_20220 = (w2 <= n2); /*bool*/;
              if (_match_20220) {
                _x20950 = s2_17058; /*string*/
              }
              else {
                kk_string_t _x20951;
                kk_string_t _x20952 = kk_std_core_string('0', _ctx); /*string*/
                kk_ssize_t _x20953 = (w2 - n2); /*ssize_t*/
                _x20951 = kk_std_core_repeatz(_x20952, _x20953, _ctx); /*string*/
                _x20950 = kk_std_core__lp__plus__plus__1_rp_(_x20951, s2_17058, _ctx); /*string*/
              }
              _x20947 = kk_std_core__lp__plus__plus__1_rp_(_x20948, _x20950, _ctx); /*string*/
              _x20943 = kk_std_core__lp__plus__plus__1_rp_(_x20944, _x20947, _ctx); /*string*/
              return kk_std_core__lp__plus__plus__1_rp_(_x20941, _x20943, _ctx);
            }
            {
              bool _match_20208;
              kk_integer_t _brw_20218 = kk_integer_from_int(c,kk_context()); /*int*/;
              kk_integer_t _brw_20217 = kk_integer_from_int(65535, _ctx); /*int*/;
              bool _brw_20219 = kk_integer_lte_borrow(_brw_20218,_brw_20217,kk_context()); /*bool*/;
              kk_integer_drop(_brw_20218, _ctx);
              kk_integer_drop(_brw_20217, _ctx);
              _match_20208 = _brw_20219; /*bool*/
              if (_match_20208) {
                kk_integer_t _arg_79940 = kk_integer_from_int(c,kk_context()); /*int*/;
                kk_string_t s3_17065;
                kk_integer_t _x20954;
                kk_integer_t _x20955 = kk_integer_dup(_arg_79940); /*int*/
                _x20954 = kk_integer_abs(_x20955,kk_context()); /*int*/
                bool _x20956;
                kk_std_core_types__optional _x20957 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
                if (kk_std_core_types__is_Optional(_x20957)) {
                  kk_box_t _box_x18989 = _x20957._cons.Optional.value;
                  bool _use_capitals_74333 = kk_bool_unbox(_box_x18989);
                  _x20956 = _use_capitals_74333; /*bool*/
                }
                else {
                  _x20956 = true; /*bool*/
                }
                s3_17065 = kk_std_core_int_show_hex(_x20954, _x20956, _ctx); /*string*/
                kk_ssize_t w3 = (KK_IZ(4)); /*ssize_t*/;
                kk_ssize_t n3;
                kk_string_t _x20959 = kk_string_dup(s3_17065); /*string*/
                n3 = kk_string_len(_x20959,kk_context()); /*ssize_t*/
                kk_string_t _x20960;
                kk_define_string_literal(, _s20961, 2, "\\u")
                _x20960 = kk_string_dup(_s20961); /*string*/
                kk_string_t _x20962;
                kk_string_t _x20963;
                bool _match_20214;
                bool _brw_20215 = kk_integer_lt_borrow(_arg_79940,(kk_integer_from_small(0)),kk_context()); /*bool*/;
                kk_integer_drop(_arg_79940, _ctx);
                _match_20214 = _brw_20215; /*bool*/
                if (_match_20214) {
                  kk_define_string_literal(, _s20964, 1, "-")
                  _x20963 = kk_string_dup(_s20964); /*string*/
                }
                else {
                  _x20963 = kk_string_empty(); /*string*/
                }
                kk_string_t _x20966;
                kk_string_t _x20967 = kk_string_empty(); /*string*/
                kk_string_t _x20969;
                bool _match_20213 = (w3 <= n3); /*bool*/;
                if (_match_20213) {
                  _x20969 = s3_17065; /*string*/
                }
                else {
                  kk_string_t _x20970;
                  kk_string_t _x20971 = kk_std_core_string('0', _ctx); /*string*/
                  kk_ssize_t _x20972 = (w3 - n3); /*ssize_t*/
                  _x20970 = kk_std_core_repeatz(_x20971, _x20972, _ctx); /*string*/
                  _x20969 = kk_std_core__lp__plus__plus__1_rp_(_x20970, s3_17065, _ctx); /*string*/
                }
                _x20966 = kk_std_core__lp__plus__plus__1_rp_(_x20967, _x20969, _ctx); /*string*/
                _x20962 = kk_std_core__lp__plus__plus__1_rp_(_x20963, _x20966, _ctx); /*string*/
                return kk_std_core__lp__plus__plus__1_rp_(_x20960, _x20962, _ctx);
              }
              {
                kk_integer_t _arg_80380 = kk_integer_from_int(c,kk_context()); /*int*/;
                kk_string_t s4_17072;
                kk_integer_t _x20973;
                kk_integer_t _x20974 = kk_integer_dup(_arg_80380); /*int*/
                _x20973 = kk_integer_abs(_x20974,kk_context()); /*int*/
                bool _x20975;
                kk_std_core_types__optional _x20976 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
                if (kk_std_core_types__is_Optional(_x20976)) {
                  kk_box_t _box_x18990 = _x20976._cons.Optional.value;
                  bool _use_capitals_74334 = kk_bool_unbox(_box_x18990);
                  _x20975 = _use_capitals_74334; /*bool*/
                }
                else {
                  _x20975 = true; /*bool*/
                }
                s4_17072 = kk_std_core_int_show_hex(_x20973, _x20975, _ctx); /*string*/
                kk_ssize_t w4 = (KK_IZ(6)); /*ssize_t*/;
                kk_ssize_t n4;
                kk_string_t _x20978 = kk_string_dup(s4_17072); /*string*/
                n4 = kk_string_len(_x20978,kk_context()); /*ssize_t*/
                kk_string_t _x20979;
                kk_define_string_literal(, _s20980, 2, "\\U")
                _x20979 = kk_string_dup(_s20980); /*string*/
                kk_string_t _x20981;
                kk_string_t _x20982;
                bool _match_20210;
                bool _brw_20211 = kk_integer_lt_borrow(_arg_80380,(kk_integer_from_small(0)),kk_context()); /*bool*/;
                kk_integer_drop(_arg_80380, _ctx);
                _match_20210 = _brw_20211; /*bool*/
                if (_match_20210) {
                  kk_define_string_literal(, _s20983, 1, "-")
                  _x20982 = kk_string_dup(_s20983); /*string*/
                }
                else {
                  _x20982 = kk_string_empty(); /*string*/
                }
                kk_string_t _x20985;
                kk_string_t _x20986 = kk_string_empty(); /*string*/
                kk_string_t _x20988;
                bool _match_20209 = (w4 <= n4); /*bool*/;
                if (_match_20209) {
                  _x20988 = s4_17072; /*string*/
                }
                else {
                  kk_string_t _x20989;
                  kk_string_t _x20990 = kk_std_core_string('0', _ctx); /*string*/
                  kk_ssize_t _x20991 = (w4 - n4); /*ssize_t*/
                  _x20989 = kk_std_core_repeatz(_x20990, _x20991, _ctx); /*string*/
                  _x20988 = kk_std_core__lp__plus__plus__1_rp_(_x20989, s4_17072, _ctx); /*string*/
                }
                _x20985 = kk_std_core__lp__plus__plus__1_rp_(_x20986, _x20988, _ctx); /*string*/
                _x20981 = kk_std_core__lp__plus__plus__1_rp_(_x20982, _x20985, _ctx); /*string*/
                return kk_std_core__lp__plus__plus__1_rp_(_x20979, _x20981, _ctx);
              }
            }
          }
        }
      }
    }
    {
      bool _match_20201 = (c == ('\'')); /*bool*/;
      if (_match_20201) {
        kk_define_string_literal(, _s20992, 2, "\\\'")
        return kk_string_dup(_s20992);
      }
      {
        bool _match_20202 = (c == ('"')); /*bool*/;
        if (_match_20202) {
          kk_define_string_literal(, _s20993, 2, "\\\"")
          return kk_string_dup(_s20993);
        }
        {
          bool _match_20203 = (c == ('\\')); /*bool*/;
          if (_match_20203) {
            kk_define_string_literal(, _s20994, 2, "\\\\")
            return kk_string_dup(_s20994);
          }
          {
            return kk_std_core_string(c, _ctx);
          }
        }
      }
    }
  }
}
 
// Show a `:float64` fixed-point notation.
// The optional `precision` (= `-2`) specifies the maximum precision.
// If `>=0` it specifies the number of digits behind the dot (up to `20` max).
// If negative, then at most the absolute value of `precision` digits behind the dot are used.
// This may still show a number in exponential notation if the it is too small or large,
// in particular, for  a `d` where `d > 1.0e21` or `d < 1.0e-15`, or if
// `precision.abs > 17`, the `show-exp` routine is used.

kk_string_t kk_std_core_show_fixed(double d, kk_std_core_types__optional precision, kk_context_t* _ctx) { /* (d : float64, precision : optional<int>) -> string */ 
  double dabs = kk_double_abs(d); /*float64*/;
  bool _match_20197 = (dabs < (1.0e-15)); /*bool*/;
  if (_match_20197) {
    int32_t _x20998;
    kk_integer_t _x20999;
    if (kk_std_core_types__is_Optional(precision)) {
      kk_box_t _box_x18992 = precision._cons.Optional.value;
      kk_integer_t _precision_8127 = kk_integer_unbox(_box_x18992);
      _x20999 = _precision_8127; /*int*/
    }
    else {
      _x20999 = kk_integer_from_small(-2); /*int*/
    }
    _x20998 = kk_std_core_int32(_x20999, _ctx); /*int32*/
    return kk_std_core_show_expx(d, _x20998, _ctx);
  }
  {
    bool _match_20198 = (dabs > (1.0e21)); /*bool*/;
    if (_match_20198) {
      int32_t _x21001;
      kk_integer_t _x21002;
      if (kk_std_core_types__is_Optional(precision)) {
        kk_box_t _box_x18993 = precision._cons.Optional.value;
        kk_integer_t _precision_81270 = kk_integer_unbox(_box_x18993);
        _x21002 = _precision_81270; /*int*/
      }
      else {
        _x21002 = kk_integer_from_small(-2); /*int*/
      }
      _x21001 = kk_std_core_int32(_x21002, _ctx); /*int32*/
      return kk_std_core_show_expx(d, _x21001, _ctx);
    }
    {
      int32_t _x21004;
      kk_integer_t _x21005;
      if (kk_std_core_types__is_Optional(precision)) {
        kk_box_t _box_x18994 = precision._cons.Optional.value;
        kk_integer_t _precision_81271 = kk_integer_unbox(_box_x18994);
        _x21005 = _precision_81271; /*int*/
      }
      else {
        _x21005 = kk_integer_from_small(-2); /*int*/
      }
      _x21004 = kk_std_core_int32(_x21005, _ctx); /*int32*/
      return kk_std_core_show_fixedx(d, _x21004, _ctx);
    }
  }
}
 
// lifted local: show-list, join-acc

kk_string_t kk_std_core__lift17283_show_list(kk_string_t sep, kk_std_core__list ys, kk_string_t acc, kk_context_t* _ctx) { /* (sep : string, ys : list<string>, acc : string) -> string */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con21007 = kk_std_core__as_Cons(ys);
    kk_box_t _box_x18995 = _con21007->head;
    kk_std_core__list yy = _con21007->tail;
    kk_string_t y = kk_string_unbox(_box_x18995);
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      kk_std_core__list_free(ys, _ctx);
    }
    else {
      kk_string_dup(y);
      kk_std_core__list_dup(yy);
      kk_std_core__list_decref(ys, _ctx);
    }
    kk_string_t acc0_17456;
    kk_string_t _x21009;
    kk_string_t _x21010 = kk_string_dup(sep); /*string*/
    _x21009 = kk_std_core__lp__plus__plus__1_rp_(_x21010, y, _ctx); /*string*/
    acc0_17456 = kk_std_core__lp__plus__plus__1_rp_(acc, _x21009, _ctx); /*string*/
    { // tailcall
      ys = yy;
      acc = acc0_17456;
      goto kk__tailcall;
    }
  }
  {
    kk_string_drop(sep, _ctx);
    return acc;
  }
}
 
// monadic lift

kk_string_t kk_std_core__mlift18013_show_list(kk_std_core__list _y_17711, kk_context_t* _ctx) { /* forall<e> (list<string>) -> e string */ 
  kk_string_t _x21011;
  kk_define_string_literal(, _s21012, 1, "[")
  _x21011 = kk_string_dup(_s21012); /*string*/
  kk_string_t _x21013;
  kk_string_t _x21014;
  if (kk_std_core__is_Nil(_y_17711)) {
    _x21014 = kk_string_empty(); /*string*/
  }
  else {
    struct kk_std_core_Cons* _con21016 = kk_std_core__as_Cons(_y_17711);
    kk_box_t _box_x18996 = _con21016->head;
    kk_std_core__list xx = _con21016->tail;
    kk_string_t x = kk_string_unbox(_box_x18996);
    if (kk_likely(kk_std_core__list_is_unique(_y_17711))) {
      kk_std_core__list_free(_y_17711, _ctx);
    }
    else {
      kk_string_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(_y_17711, _ctx);
    }
    kk_string_t _x21018;
    kk_define_string_literal(, _s21019, 1, ",")
    _x21018 = kk_string_dup(_s21019); /*string*/
    _x21014 = kk_std_core__lift17283_show_list(_x21018, xx, x, _ctx); /*string*/
  }
  kk_string_t _x21020;
  kk_define_string_literal(, _s21021, 1, "]")
  _x21020 = kk_string_dup(_s21021); /*string*/
  _x21013 = kk_std_core__lp__plus__plus__1_rp_(_x21014, _x21020, _ctx); /*string*/
  return kk_std_core__lp__plus__plus__1_rp_(_x21011, _x21013, _ctx);
}
 
// Convert a list to a string


// lift anonymous function
struct kk_std_core_show_list_fun21022__t {
  struct kk_function_s _base;
  kk_function_t show_elem;
};
static kk_box_t kk_std_core_show_list_fun21022(kk_function_t _fself, kk_box_t _b_18999, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_show_list_fun21022(kk_function_t show_elem, kk_context_t* _ctx) {
  struct kk_std_core_show_list_fun21022__t* _self = kk_function_alloc_as(struct kk_std_core_show_list_fun21022__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_show_list_fun21022, kk_context());
  _self->show_elem = show_elem;
  return &_self->_base;
}

static kk_box_t kk_std_core_show_list_fun21022(kk_function_t _fself, kk_box_t _b_18999, kk_context_t* _ctx) {
  struct kk_std_core_show_list_fun21022__t* _self = kk_function_as(struct kk_std_core_show_list_fun21022__t*, _fself);
  kk_function_t show_elem = _self->show_elem; /* (8672) -> 8673 string */
  kk_drop_match(_self, {kk_function_dup(show_elem);}, {}, _ctx)
  kk_string_t _x21023 = kk_function_call(kk_string_t, (kk_function_t, kk_box_t, kk_context_t*), show_elem, (show_elem, _b_18999, _ctx)); /*string*/
  return kk_string_box(_x21023);
}


// lift anonymous function
struct kk_std_core_show_list_fun21025__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_show_list_fun21025(kk_function_t _fself, kk_box_t _b_19004, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_show_list_fun21025(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_show_list_fun21025, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_show_list_fun21025(kk_function_t _fself, kk_box_t _b_19004, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_std_core__list _y_19007_17711 = kk_std_core__list_unbox(_b_19004, _ctx); /*list<string>*/;
  kk_string_t _x21026;
  kk_string_t _x21027;
  kk_define_string_literal(, _s21028, 1, "[")
  _x21027 = kk_string_dup(_s21028); /*string*/
  kk_string_t _x21029;
  kk_string_t _x21030;
  if (kk_std_core__is_Nil(_y_19007_17711)) {
    _x21030 = kk_string_empty(); /*string*/
  }
  else {
    struct kk_std_core_Cons* _con21032 = kk_std_core__as_Cons(_y_19007_17711);
    kk_box_t _box_x19002 = _con21032->head;
    kk_std_core__list xx = _con21032->tail;
    kk_string_t x0 = kk_string_unbox(_box_x19002);
    if (kk_likely(kk_std_core__list_is_unique(_y_19007_17711))) {
      kk_std_core__list_free(_y_19007_17711, _ctx);
    }
    else {
      kk_string_dup(x0);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(_y_19007_17711, _ctx);
    }
    kk_string_t _x21034;
    kk_define_string_literal(, _s21035, 1, ",")
    _x21034 = kk_string_dup(_s21035); /*string*/
    _x21030 = kk_std_core__lift17283_show_list(_x21034, xx, x0, _ctx); /*string*/
  }
  kk_string_t _x21036;
  kk_define_string_literal(, _s21037, 1, "]")
  _x21036 = kk_string_dup(_s21037); /*string*/
  _x21029 = kk_std_core__lp__plus__plus__1_rp_(_x21030, _x21036, _ctx); /*string*/
  _x21026 = kk_std_core__lp__plus__plus__1_rp_(_x21027, _x21029, _ctx); /*string*/
  return kk_string_box(_x21026);
}

kk_string_t kk_std_core_show_list(kk_std_core__list xs, kk_function_t show_elem, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, show-elem : (a) -> e string) -> e string */ 
  kk_std_core__list x_18216 = kk_std_core_map_5(xs, kk_std_core_new_show_list_fun21022(show_elem, _ctx), _ctx); /*list<string>*/;
  if (kk_yielding(kk_context())) {
    kk_std_core__list_drop(x_18216, _ctx);
    kk_box_t _x21024 = kk_std_core_hnd_yield_extend(kk_std_core_new_show_list_fun21025(_ctx), _ctx); /*3926*/
    return kk_string_unbox(_x21024);
  }
  {
    kk_string_t _x21038;
    kk_define_string_literal(, _s21039, 1, "[")
    _x21038 = kk_string_dup(_s21039); /*string*/
    kk_string_t _x21040;
    kk_string_t _x21041;
    if (kk_std_core__is_Nil(x_18216)) {
      _x21041 = kk_string_empty(); /*string*/
    }
    else {
      struct kk_std_core_Cons* _con21043 = kk_std_core__as_Cons(x_18216);
      kk_box_t _box_x19005 = _con21043->head;
      kk_std_core__list xx0 = _con21043->tail;
      kk_string_t x1 = kk_string_unbox(_box_x19005);
      if (kk_likely(kk_std_core__list_is_unique(x_18216))) {
        kk_std_core__list_free(x_18216, _ctx);
      }
      else {
        kk_string_dup(x1);
        kk_std_core__list_dup(xx0);
        kk_std_core__list_decref(x_18216, _ctx);
      }
      kk_string_t _x21045;
      kk_define_string_literal(, _s21046, 1, ",")
      _x21045 = kk_string_dup(_s21046); /*string*/
      _x21041 = kk_std_core__lift17283_show_list(_x21045, xx0, x1, _ctx); /*string*/
    }
    kk_string_t _x21047;
    kk_define_string_literal(, _s21048, 1, "]")
    _x21047 = kk_string_dup(_s21048); /*string*/
    _x21040 = kk_std_core__lp__plus__plus__1_rp_(_x21041, _x21047, _ctx); /*string*/
    return kk_std_core__lp__plus__plus__1_rp_(_x21038, _x21040, _ctx);
  }
}
 
// Convert an `:int` to a string

kk_string_t kk_std_core_show(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> string */ 
  return kk_integer_to_string(i,kk_context());
}
 
// Show a `:float64` as a string.
// If `d >= 1.0e-5` and `d < 1.0e+21`, `show-fixed` is used and otherwise `show-exp`.
// Default `precision` is `-17`.

kk_string_t kk_std_core_show_1(double d, kk_std_core_types__optional precision, kk_context_t* _ctx) { /* (d : float64, precision : optional<int>) -> string */ 
  double dabs = kk_double_abs(d); /*float64*/;
  bool _match_20192 = (dabs >= (1.0e-5)); /*bool*/;
  if (_match_20192) {
    bool _match_20193 = (dabs < (1.0e21)); /*bool*/;
    if (_match_20193) {
      double dabs0 = kk_double_abs(d); /*float64*/;
      bool _match_20194 = (dabs0 < (1.0e-15)); /*bool*/;
      if (_match_20194) {
        int32_t _x21049;
        kk_integer_t _x21050;
        if (kk_std_core_types__is_Optional(precision)) {
          kk_box_t _box_x19008 = precision._cons.Optional.value;
          kk_integer_t _precision_8682 = kk_integer_unbox(_box_x19008);
          _x21050 = _precision_8682; /*int*/
        }
        else {
          _x21050 = kk_integer_from_small(-17); /*int*/
        }
        _x21049 = kk_std_core_int32(_x21050, _ctx); /*int32*/
        return kk_std_core_show_expx(d, _x21049, _ctx);
      }
      {
        bool _match_20195 = (dabs0 > (1.0e21)); /*bool*/;
        if (_match_20195) {
          int32_t _x21052;
          kk_integer_t _x21053;
          if (kk_std_core_types__is_Optional(precision)) {
            kk_box_t _box_x19009 = precision._cons.Optional.value;
            kk_integer_t _precision_86820 = kk_integer_unbox(_box_x19009);
            _x21053 = _precision_86820; /*int*/
          }
          else {
            _x21053 = kk_integer_from_small(-17); /*int*/
          }
          _x21052 = kk_std_core_int32(_x21053, _ctx); /*int32*/
          return kk_std_core_show_expx(d, _x21052, _ctx);
        }
        {
          int32_t _x21055;
          kk_integer_t _x21056;
          if (kk_std_core_types__is_Optional(precision)) {
            kk_box_t _box_x19010 = precision._cons.Optional.value;
            kk_integer_t _precision_86821 = kk_integer_unbox(_box_x19010);
            _x21056 = _precision_86821; /*int*/
          }
          else {
            _x21056 = kk_integer_from_small(-17); /*int*/
          }
          _x21055 = kk_std_core_int32(_x21056, _ctx); /*int32*/
          return kk_std_core_show_fixedx(d, _x21055, _ctx);
        }
      }
    }
    {
      int32_t _x21058;
      kk_integer_t _x21059;
      if (kk_std_core_types__is_Optional(precision)) {
        kk_box_t _box_x19011 = precision._cons.Optional.value;
        kk_integer_t _precision_86822 = kk_integer_unbox(_box_x19011);
        _x21059 = _precision_86822; /*int*/
      }
      else {
        _x21059 = kk_integer_from_small(-17); /*int*/
      }
      _x21058 = kk_std_core_int32(_x21059, _ctx); /*int32*/
      return kk_std_core_show_expx(d, _x21058, _ctx);
    }
  }
  {
    int32_t _x21061;
    kk_integer_t _x21062;
    if (kk_std_core_types__is_Optional(precision)) {
      kk_box_t _box_x19012 = precision._cons.Optional.value;
      kk_integer_t _precision_86823 = kk_integer_unbox(_box_x19012);
      _x21062 = _precision_86823; /*int*/
    }
    else {
      _x21062 = kk_integer_from_small(-17); /*int*/
    }
    _x21061 = kk_std_core_int32(_x21062, _ctx); /*int32*/
    return kk_std_core_show_expx(d, _x21061, _ctx);
  }
}
 
// lifted local: show.3, join-acc

kk_string_t kk_std_core__lift17284_show_3(kk_std_core__list ys, kk_string_t acc, kk_context_t* _ctx) { /* (ys : list<string>, acc : string) -> string */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con21070 = kk_std_core__as_Cons(ys);
    kk_box_t _box_x19013 = _con21070->head;
    kk_std_core__list yy = _con21070->tail;
    kk_string_t y = kk_string_unbox(_box_x19013);
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      kk_std_core__list_free(ys, _ctx);
    }
    else {
      kk_string_dup(y);
      kk_std_core__list_dup(yy);
      kk_std_core__list_decref(ys, _ctx);
    }
    { // tailcall
      kk_string_t _x21072;
      kk_string_t _x21073;
      kk_string_t _x21074 = kk_string_empty(); /*string*/
      _x21073 = kk_std_core__lp__plus__plus__1_rp_(_x21074, y, _ctx); /*string*/
      _x21072 = kk_std_core__lp__plus__plus__1_rp_(acc, _x21073, _ctx); /*string*/
      ys = yy;
      acc = _x21072;
      goto kk__tailcall;
    }
  }
  {
    return acc;
  }
}
 
// lifted local: show.3, .spec.17404
// specialized: std/core/map.5, on parameters f, using:
// f = std/core/show-char

kk_std_core__list kk_std_core__ctail_lift17479_show_3(kk_std_core__list xs, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* (xs : list<char>, ctail<list<string>>) -> list<string> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21076 = kk_std_core__as_Cons(xs);
    kk_box_t _box_x19014 = _con21076->head;
    kk_std_core__list xx = _con21076->tail;
    kk_char_t x = kk_char_unbox(_box_x19014, NULL);
    kk_reuse_t _ru_20379 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      _ru_20379 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_string_t _ctail_17553 = kk_std_core_show_char(x, _ctx); /*string*/;
    kk_std_core__list _ctail_17554 = kk_std_core__list_hole(); /*list<string>*/;
    kk_std_core__list _ctail_17555 = kk_std_core__new_Cons(_ru_20379, kk_string_box(_ctail_17553), _ctail_17554, _ctx); /*list<string>*/;
    kk_box_t* _b_19029_19024 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17555)->tail)); /*cfield<list<string>>*/;
    { // tailcall
      kk_std_core_types__ctail _x21078 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17555, _ctx)),_b_19029_19024); /*ctail<0>*/
      xs = xx;
      _acc = _x21078;
      goto kk__tailcall;
    }
  }
  {
    kk_box_t _x21079 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21079, _ctx);
  }
}
 
// lifted local: show.3, .spec.17404
// specialized: std/core/map.5, on parameters f, using:
// f = std/core/show-char

kk_std_core__list kk_std_core__lift17479_show_3(kk_std_core__list xs0, kk_context_t* _ctx) { /* (xs : list<char>) -> list<string> */ 
  kk_std_core_types__ctail _x21080 = kk_ctail_nil(); /*ctail<0>*/
  return kk_std_core__ctail_lift17479_show_3(xs0, _x21080, _ctx);
}
 
// Show a string as a string literal

kk_string_t kk_std_core_show_3(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> string */ 
  kk_std_core__list xs_17090;
  kk_std_core__list _x21081 = kk_std_core_list_6(s, _ctx); /*list<char>*/
  xs_17090 = kk_std_core__lift17479_show_3(_x21081, _ctx); /*list<string>*/
  kk_string_t _x21082;
  kk_define_string_literal(, _s21083, 1, "\"")
  _x21082 = kk_string_dup(_s21083); /*string*/
  kk_string_t _x21084;
  kk_string_t _x21085;
  if (kk_std_core__is_Nil(xs_17090)) {
    _x21085 = kk_string_empty(); /*string*/
  }
  else {
    struct kk_std_core_Cons* _con21087 = kk_std_core__as_Cons(xs_17090);
    kk_box_t _box_x19035 = _con21087->head;
    kk_std_core__list xx0 = _con21087->tail;
    kk_string_t x0 = kk_string_unbox(_box_x19035);
    if (kk_likely(kk_std_core__list_is_unique(xs_17090))) {
      kk_std_core__list_free(xs_17090, _ctx);
    }
    else {
      kk_string_dup(x0);
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(xs_17090, _ctx);
    }
    _x21085 = kk_std_core__lift17284_show_3(xx0, x0, _ctx); /*string*/
  }
  kk_string_t _x21089;
  kk_define_string_literal(, _s21090, 1, "\"")
  _x21089 = kk_string_dup(_s21090); /*string*/
  _x21084 = kk_std_core__lp__plus__plus__1_rp_(_x21085, _x21089, _ctx); /*string*/
  return kk_std_core__lp__plus__plus__1_rp_(_x21082, _x21084, _ctx);
}
 
// lifted local: .lift17480-show.7, show.7, .spec.17408
// specialized: std/core/map.5, on parameters f, using:
// f = std/core/show.3

kk_std_core__list kk_std_core__ctail_lift17481_show_7(kk_std_core__list xs1, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* (xs1 : list<string>, ctail<list<string>>) -> list<string> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs1)) {
    struct kk_std_core_Cons* _con21095 = kk_std_core__as_Cons(xs1);
    kk_box_t _box_x19036 = _con21095->head;
    kk_std_core__list xx = _con21095->tail;
    kk_string_t x = kk_string_unbox(_box_x19036);
    kk_reuse_t _ru_20381 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(xs1))) {
      _ru_20381 = (kk_std_core__list_reuse(xs1));
    }
    else {
      kk_string_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs1, _ctx);
    }
    kk_string_t _ctail_17556 = kk_std_core_show_3(x, _ctx); /*string*/;
    kk_std_core__list _ctail_17557 = kk_std_core__list_hole(); /*list<string>*/;
    kk_std_core__list _ctail_17558 = kk_std_core__new_Cons(_ru_20381, kk_string_box(_ctail_17556), _ctail_17557, _ctx); /*list<string>*/;
    kk_box_t* _b_19051_19046 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17558)->tail)); /*cfield<list<string>>*/;
    { // tailcall
      kk_std_core_types__ctail _x21097 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17558, _ctx)),_b_19051_19046); /*ctail<0>*/
      xs1 = xx;
      _acc = _x21097;
      goto kk__tailcall;
    }
  }
  {
    kk_box_t _x21098 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21098, _ctx);
  }
}
 
// lifted local: .lift17480-show.7, show.7, .spec.17408
// specialized: std/core/map.5, on parameters f, using:
// f = std/core/show.3

kk_std_core__list kk_std_core__lift17481_show_7(kk_std_core__list xs10, kk_context_t* _ctx) { /* (xs1 : list<string>) -> list<string> */ 
  kk_std_core_types__ctail _x21099 = kk_ctail_nil(); /*ctail<0>*/
  return kk_std_core__ctail_lift17481_show_7(xs10, _x21099, _ctx);
}
 
// lifted local: show.7, .spec.17405
// specialized: std/core/show-list, on parameters show-elem, using:
// show-elem = std/core/show.3

kk_string_t kk_std_core__lift17480_show_7(kk_std_core__list xs0, kk_context_t* _ctx) { /* (xs0 : list<string>) -> string */ 
  kk_std_core__list xs00 = kk_std_core__lift17481_show_7(xs0, _ctx); /*list<string>*/;
  kk_string_t _x21100;
  kk_define_string_literal(, _s21101, 1, "[")
  _x21100 = kk_string_dup(_s21101); /*string*/
  kk_string_t _x21102;
  kk_string_t _x21103;
  if (kk_std_core__is_Nil(xs00)) {
    _x21103 = kk_string_empty(); /*string*/
  }
  else {
    struct kk_std_core_Cons* _con21105 = kk_std_core__as_Cons(xs00);
    kk_box_t _box_x19057 = _con21105->head;
    kk_std_core__list xx0 = _con21105->tail;
    kk_string_t x0 = kk_string_unbox(_box_x19057);
    if (kk_likely(kk_std_core__list_is_unique(xs00))) {
      kk_std_core__list_free(xs00, _ctx);
    }
    else {
      kk_string_dup(x0);
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(xs00, _ctx);
    }
    kk_string_t _x21107;
    kk_define_string_literal(, _s21108, 1, ",")
    _x21107 = kk_string_dup(_s21108); /*string*/
    _x21103 = kk_std_core__lift17283_show_list(_x21107, xx0, x0, _ctx); /*string*/
  }
  kk_string_t _x21109;
  kk_define_string_literal(, _s21110, 1, "]")
  _x21109 = kk_string_dup(_s21110); /*string*/
  _x21102 = kk_std_core__lp__plus__plus__1_rp_(_x21103, _x21109, _ctx); /*string*/
  return kk_std_core__lp__plus__plus__1_rp_(_x21100, _x21102, _ctx);
}
 
// lifted local: .lift17482-show.8, show.8, .spec.17412
// specialized: std/core/map.5, on parameters f, using:
// f = std/core/show

kk_std_core__list kk_std_core__ctail_lift17483_show_8(kk_std_core__list xs1, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* (xs1 : list<int>, ctail<list<string>>) -> list<string> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs1)) {
    struct kk_std_core_Cons* _con21111 = kk_std_core__as_Cons(xs1);
    kk_box_t _box_x19058 = _con21111->head;
    kk_std_core__list xx = _con21111->tail;
    kk_integer_t x = kk_integer_unbox(_box_x19058);
    kk_reuse_t _ru_20383 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(xs1))) {
      _ru_20383 = (kk_std_core__list_reuse(xs1));
    }
    else {
      kk_integer_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs1, _ctx);
    }
    kk_string_t _ctail_17559 = kk_std_core_show(x, _ctx); /*string*/;
    kk_std_core__list _ctail_17560 = kk_std_core__list_hole(); /*list<string>*/;
    kk_std_core__list _ctail_17561 = kk_std_core__new_Cons(_ru_20383, kk_string_box(_ctail_17559), _ctail_17560, _ctx); /*list<string>*/;
    kk_box_t* _b_19073_19068 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17561)->tail)); /*cfield<list<string>>*/;
    { // tailcall
      kk_std_core_types__ctail _x21113 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17561, _ctx)),_b_19073_19068); /*ctail<0>*/
      xs1 = xx;
      _acc = _x21113;
      goto kk__tailcall;
    }
  }
  {
    kk_box_t _x21114 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21114, _ctx);
  }
}
 
// lifted local: .lift17482-show.8, show.8, .spec.17412
// specialized: std/core/map.5, on parameters f, using:
// f = std/core/show

kk_std_core__list kk_std_core__lift17483_show_8(kk_std_core__list xs10, kk_context_t* _ctx) { /* (xs1 : list<int>) -> list<string> */ 
  kk_std_core_types__ctail _x21115 = kk_ctail_nil(); /*ctail<0>*/
  return kk_std_core__ctail_lift17483_show_8(xs10, _x21115, _ctx);
}
 
// lifted local: show.8, .spec.17409
// specialized: std/core/show-list, on parameters show-elem, using:
// show-elem = std/core/show

kk_string_t kk_std_core__lift17482_show_8(kk_std_core__list xs0, kk_context_t* _ctx) { /* (xs0 : list<int>) -> string */ 
  kk_std_core__list xs00 = kk_std_core__lift17483_show_8(xs0, _ctx); /*list<string>*/;
  kk_string_t _x21116;
  kk_define_string_literal(, _s21117, 1, "[")
  _x21116 = kk_string_dup(_s21117); /*string*/
  kk_string_t _x21118;
  kk_string_t _x21119;
  if (kk_std_core__is_Nil(xs00)) {
    _x21119 = kk_string_empty(); /*string*/
  }
  else {
    struct kk_std_core_Cons* _con21121 = kk_std_core__as_Cons(xs00);
    kk_box_t _box_x19079 = _con21121->head;
    kk_std_core__list xx0 = _con21121->tail;
    kk_string_t x0 = kk_string_unbox(_box_x19079);
    if (kk_likely(kk_std_core__list_is_unique(xs00))) {
      kk_std_core__list_free(xs00, _ctx);
    }
    else {
      kk_string_dup(x0);
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(xs00, _ctx);
    }
    kk_string_t _x21123;
    kk_define_string_literal(, _s21124, 1, ",")
    _x21123 = kk_string_dup(_s21124); /*string*/
    _x21119 = kk_std_core__lift17283_show_list(_x21123, xx0, x0, _ctx); /*string*/
  }
  kk_string_t _x21125;
  kk_define_string_literal(, _s21126, 1, "]")
  _x21125 = kk_string_dup(_s21126); /*string*/
  _x21118 = kk_std_core__lp__plus__plus__1_rp_(_x21119, _x21125, _ctx); /*string*/
  return kk_std_core__lp__plus__plus__1_rp_(_x21116, _x21118, _ctx);
}
 
// lifted local: .lift17484-show.9, show.9, .spec.17416
// specialized: std/core/map.5, on parameters f, using:
// f = std/core/show.4

kk_std_core__list kk_std_core__ctail_lift17485_show_9(kk_std_core__list xs1, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* (xs1 : list<bool>, ctail<list<string>>) -> list<string> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs1)) {
    struct kk_std_core_Cons* _con21127 = kk_std_core__as_Cons(xs1);
    kk_box_t _box_x19080 = _con21127->head;
    kk_std_core__list xx = _con21127->tail;
    bool x = kk_bool_unbox(_box_x19080);
    kk_reuse_t _ru_20385 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(xs1))) {
      _ru_20385 = (kk_std_core__list_reuse(xs1));
    }
    else {
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs1, _ctx);
    }
    kk_std_core__list _ctail_17563 = kk_std_core__list_hole(); /*list<string>*/;
    kk_std_core__list _ctail_17564;
    kk_box_t _x21129;
    kk_string_t _x21130;
    if (x) {
      kk_define_string_literal(, _s21131, 4, "True")
      _x21130 = kk_string_dup(_s21131); /*string*/
    }
    else {
      kk_define_string_literal(, _s21132, 5, "False")
      _x21130 = kk_string_dup(_s21132); /*string*/
    }
    _x21129 = kk_string_box(_x21130); /*61*/
    _ctail_17564 = kk_std_core__new_Cons(_ru_20385, _x21129, _ctail_17563, _ctx); /*list<string>*/
    kk_box_t* _b_19095_19090 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17564)->tail)); /*cfield<list<string>>*/;
    { // tailcall
      kk_std_core_types__ctail _x21133 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17564, _ctx)),_b_19095_19090); /*ctail<0>*/
      xs1 = xx;
      _acc = _x21133;
      goto kk__tailcall;
    }
  }
  {
    kk_box_t _x21134 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21134, _ctx);
  }
}
 
// lifted local: .lift17484-show.9, show.9, .spec.17416
// specialized: std/core/map.5, on parameters f, using:
// f = std/core/show.4

kk_std_core__list kk_std_core__lift17485_show_9(kk_std_core__list xs10, kk_context_t* _ctx) { /* (xs1 : list<bool>) -> list<string> */ 
  kk_std_core_types__ctail _x21135 = kk_ctail_nil(); /*ctail<0>*/
  return kk_std_core__ctail_lift17485_show_9(xs10, _x21135, _ctx);
}
 
// lifted local: show.9, .spec.17413
// specialized: std/core/show-list, on parameters show-elem, using:
// show-elem = std/core/show.4

kk_string_t kk_std_core__lift17484_show_9(kk_std_core__list xs0, kk_context_t* _ctx) { /* (xs0 : list<bool>) -> string */ 
  kk_std_core__list xs00 = kk_std_core__lift17485_show_9(xs0, _ctx); /*list<string>*/;
  kk_string_t _x21136;
  kk_define_string_literal(, _s21137, 1, "[")
  _x21136 = kk_string_dup(_s21137); /*string*/
  kk_string_t _x21138;
  kk_string_t _x21139;
  if (kk_std_core__is_Nil(xs00)) {
    _x21139 = kk_string_empty(); /*string*/
  }
  else {
    struct kk_std_core_Cons* _con21141 = kk_std_core__as_Cons(xs00);
    kk_box_t _box_x19101 = _con21141->head;
    kk_std_core__list xx0 = _con21141->tail;
    kk_string_t x0 = kk_string_unbox(_box_x19101);
    if (kk_likely(kk_std_core__list_is_unique(xs00))) {
      kk_std_core__list_free(xs00, _ctx);
    }
    else {
      kk_string_dup(x0);
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(xs00, _ctx);
    }
    kk_string_t _x21143;
    kk_define_string_literal(, _s21144, 1, ",")
    _x21143 = kk_string_dup(_s21144); /*string*/
    _x21139 = kk_std_core__lift17283_show_list(_x21143, xx0, x0, _ctx); /*string*/
  }
  kk_string_t _x21145;
  kk_define_string_literal(, _s21146, 1, "]")
  _x21145 = kk_string_dup(_s21146); /*string*/
  _x21138 = kk_std_core__lp__plus__plus__1_rp_(_x21139, _x21145, _ctx); /*string*/
  return kk_std_core__lp__plus__plus__1_rp_(_x21136, _x21138, _ctx);
}

kk_unit_t kk_std_core_prints(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> console () */ 
  kk_std_core_types__maybe _match_20191;
  kk_box_t _x21155;
  kk_ref_t _x21156 = kk_ref_dup(kk_std_core_redirect); /*ref<global,maybe<(string) -> console ()>>*/
  _x21155 = kk_ref_get(_x21156,kk_context()); /*1001*/
  _match_20191 = kk_std_core_types__maybe_unbox(_x21155, _ctx); /*maybe<(string) -> console ()>*/
  if (kk_std_core_types__is_Nothing(_match_20191)) {
    kk_std_core_xprints(s, _ctx); return kk_Unit;
  }
  {
    kk_box_t _fun_unbox_x19106 = _match_20191._cons.Just.value;
    kk_box_t _x21157;
    kk_function_t _x21158 = kk_function_unbox(_fun_unbox_x19106); /*(19107) -> console 19108*/
    _x21157 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x21158, (_x21158, kk_string_box(s), _ctx)); /*19108*/
    kk_unit_unbox(_x21157); return kk_Unit;
  }
}


// lift anonymous function
struct kk_std_core__default_exn_fun21163__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core__default_exn_fun21163(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__525__16, kk_std_core__exception x, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_default_exn_fun21163(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core__default_exn_fun21163, _ctx)
  return kk_function_dup(_fself);
}



// lift anonymous function
struct kk_std_core__default_exn_fun21164__t {
  struct kk_function_s _base;
  kk_std_core__exception x;
};
static kk_box_t kk_std_core__default_exn_fun21164(kk_function_t _fself, kk_function_t _b_19121, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_default_exn_fun21164(kk_std_core__exception x, kk_context_t* _ctx) {
  struct kk_std_core__default_exn_fun21164__t* _self = kk_function_alloc_as(struct kk_std_core__default_exn_fun21164__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__default_exn_fun21164, kk_context());
  _self->x = x;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core__default_exn_fun21165__t {
  struct kk_function_s _base;
  kk_function_t _b_19121;
};
static kk_unit_t kk_std_core__default_exn_fun21165(kk_function_t _fself, kk_std_core_hnd__resume_result _b_19122, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_default_exn_fun21165(kk_function_t _b_19121, kk_context_t* _ctx) {
  struct kk_std_core__default_exn_fun21165__t* _self = kk_function_alloc_as(struct kk_std_core__default_exn_fun21165__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__default_exn_fun21165, kk_context());
  _self->_b_19121 = _b_19121;
  return &_self->_base;
}

static kk_unit_t kk_std_core__default_exn_fun21165(kk_function_t _fself, kk_std_core_hnd__resume_result _b_19122, kk_context_t* _ctx) {
  struct kk_std_core__default_exn_fun21165__t* _self = kk_function_as(struct kk_std_core__default_exn_fun21165__t*, _fself);
  kk_function_t _b_19121 = _self->_b_19121; /* (std/core/hnd/resume-result<3990,3993>) -> 3992 3993 */
  kk_drop_match(_self, {kk_function_dup(_b_19121);}, {}, _ctx)
  kk_box_t _x21166 = kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__resume_result, kk_context_t*), _b_19121, (_b_19121, _b_19122, _ctx)); /*3993*/
  return kk_unit_unbox(_x21166);
}
static kk_box_t kk_std_core__default_exn_fun21164(kk_function_t _fself, kk_function_t _b_19121, kk_context_t* _ctx) {
  struct kk_std_core__default_exn_fun21164__t* _self = kk_function_as(struct kk_std_core__default_exn_fun21164__t*, _fself);
  kk_std_core__exception x = _self->x; /* exception */
  kk_drop_match(_self, {kk_std_core__exception_dup(x);}, {}, _ctx)
  kk_function_t ___wildcard__525__45_19139 = kk_std_core__new_default_exn_fun21165(_b_19121, _ctx); /*(std/core/hnd/resume-result<9736,()>) -> <console|9734> ()*/;
  kk_function_drop(___wildcard__525__45_19139, _ctx);
  kk_unit_t __ = kk_Unit;
  kk_string_t _x21167;
  kk_define_string_literal(, _s21168, 20, "uncaught exception: ")
  _x21167 = kk_string_dup(_s21168); /*string*/
  kk_std_core_prints(_x21167, _ctx);
  kk_unit_t _x21169 = kk_Unit;
  kk_string_t _x21170;
  {
    kk_string_t _x = x.message;
    kk_string_dup(_x);
    kk_std_core__exception_drop(x, _ctx);
    _x21170 = _x; /*string*/
  }
  kk_std_core_printsln(_x21170, _ctx);
  return kk_unit_box(_x21169);
}
static kk_box_t kk_std_core__default_exn_fun21163(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__525__16, kk_std_core__exception x, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_std_core_hnd__ev_dropn(___wildcard__525__16, (KK_I32(3)), _ctx);
  return kk_std_core_hnd_yield_to_final(m0, kk_std_core__new_default_exn_fun21164(x, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core__default_exn_fun21173__t {
  struct kk_function_s _base;
  kk_function_t _b_19136_19123;
};
static kk_box_t kk_std_core__default_exn_fun21173(kk_function_t _fself, kk_std_core_hnd__marker _b_19124, kk_std_core_hnd__ev _b_19125, kk_box_t _b_19126, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_default_exn_fun21173(kk_function_t _b_19136_19123, kk_context_t* _ctx) {
  struct kk_std_core__default_exn_fun21173__t* _self = kk_function_alloc_as(struct kk_std_core__default_exn_fun21173__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__default_exn_fun21173, kk_context());
  _self->_b_19136_19123 = _b_19136_19123;
  return &_self->_base;
}

static kk_box_t kk_std_core__default_exn_fun21173(kk_function_t _fself, kk_std_core_hnd__marker _b_19124, kk_std_core_hnd__ev _b_19125, kk_box_t _b_19126, kk_context_t* _ctx) {
  struct kk_std_core__default_exn_fun21173__t* _self = kk_function_as(struct kk_std_core__default_exn_fun21173__t*, _fself);
  kk_function_t _b_19136_19123 = _self->_b_19136_19123; /* (m0 : std/core/hnd/marker<<console|9734>,()>, std/core/hnd/ev<.hnd-exn>, x : exception) -> <console|9734> 9736 */
  kk_drop_match(_self, {kk_function_dup(_b_19136_19123);}, {}, _ctx)
  kk_std_core__exception _x21174 = kk_std_core__exception_unbox(_b_19126, _ctx); /*exception*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_std_core__exception, kk_context_t*), _b_19136_19123, (_b_19136_19123, _b_19124, _b_19125, _x21174, _ctx));
}


// lift anonymous function
struct kk_std_core__default_exn_fun21175__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core__default_exn_fun21175(kk_function_t _fself, kk_box_t _b_19131, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_default_exn_fun21175(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core__default_exn_fun21175, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core__default_exn_fun21175(kk_function_t _fself, kk_box_t _b_19131, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_unit_t _x0_19140 = kk_Unit;
  kk_unit_unbox(_b_19131);
  return kk_unit_box(_x0_19140);
}


// lift anonymous function
struct kk_std_core__default_exn_fun21176__t {
  struct kk_function_s _base;
  kk_function_t action;
};
static kk_box_t kk_std_core__default_exn_fun21176(kk_function_t _fself, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_default_exn_fun21176(kk_function_t action, kk_context_t* _ctx) {
  struct kk_std_core__default_exn_fun21176__t* _self = kk_function_alloc_as(struct kk_std_core__default_exn_fun21176__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__default_exn_fun21176, kk_context());
  _self->action = action;
  return &_self->_base;
}

static kk_box_t kk_std_core__default_exn_fun21176(kk_function_t _fself, kk_context_t* _ctx) {
  struct kk_std_core__default_exn_fun21176__t* _self = kk_function_as(struct kk_std_core__default_exn_fun21176__t*, _fself);
  kk_function_t action = _self->action; /* () -> <exn,console|9734> () */
  kk_drop_match(_self, {kk_function_dup(action);}, {}, _ctx)
  kk_unit_t _x21177 = kk_Unit;
  kk_function_call(kk_unit_t, (kk_function_t, kk_context_t*), action, (action, _ctx));
  return kk_unit_box(_x21177);
}

kk_unit_t kk_std_core__default_exn(kk_function_t action, kk_context_t* _ctx) { /* forall<e> (action : () -> <console,exn|e> ()) -> <console|e> () */ 
  int32_t _b_19132_19127 = (KK_I32(0)); /*int32*/;
  kk_box_t _x21162;
  kk_function_t _b_19136_19123 = kk_std_core__new_default_exn_fun21163(_ctx); /*(m0 : std/core/hnd/marker<<console|9734>,()>, std/core/hnd/ev<.hnd-exn>, x : exception) -> <console|9734> 9736*/;
  kk_std_core__hnd_exn _x21171;
  kk_std_core_hnd__clause1 _x21172 = kk_std_core_hnd__new_Clause1(kk_std_core__new_default_exn_fun21173(_b_19136_19123, _ctx), _ctx); /*std/core/hnd/clause1<51,52,53,54,55>*/
  _x21171 = kk_std_core__new_Hnd_exn(kk_reuse_null, _x21172, _ctx); /*.hnd-exn<11,12>*/
  _x21162 = kk_std_core__handle_exn(_b_19132_19127, _x21171, kk_std_core__new_default_exn_fun21175(_ctx), kk_std_core__new_default_exn_fun21176(action, _ctx), _ctx); /*1969*/
  kk_unit_unbox(_x21162); return kk_Unit;
}
 
// Get (zero-based) element `n`  of a list. Return a `:maybe` type.

kk_std_core_types__maybe kk_std_core__lp__lb__rb__2_rp_(kk_std_core__list xs, kk_integer_t n, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, n : int) -> maybe<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21178 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con21178->head;
    kk_std_core__list xx = _con21178->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    bool _match_20186 = kk_integer_gt_borrow(n,(kk_integer_from_small(0)),kk_context()); /*bool*/;
    if (_match_20186) {
      kk_box_drop(x, _ctx);
      { // tailcall
        kk_integer_t _x21179 = kk_integer_add_small_const(n, -1, _ctx); /*int*/
        xs = xx;
        n = _x21179;
        goto kk__tailcall;
      }
    }
    {
      kk_std_core__list_drop(xx, _ctx);
      bool _match_20187;
      bool _brw_20188 = kk_integer_eq_borrow(n,(kk_integer_from_small(0)),kk_context()); /*bool*/;
      kk_integer_drop(n, _ctx);
      _match_20187 = _brw_20188; /*bool*/
      if (_match_20187) {
        return kk_std_core_types__new_Just(x, _ctx);
      }
      {
        kk_box_drop(x, _ctx);
        return kk_std_core_types__new_Nothing(_ctx);
      }
    }
  }
  {
    kk_integer_drop(n, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
}
 
// O(1). Return the string slice from the end of `slice` argument
// to the end of the string.

kk_std_core__sslice kk_std_core_after(kk_std_core__sslice slice0, kk_context_t* _ctx) { /* (slice : sslice) -> sslice */ 
  {
    kk_string_t s = slice0.str;
    kk_ssize_t start0 = slice0.start;
    kk_ssize_t len0 = slice0.len;
    kk_string_dup(s);
    kk_std_core__sslice_drop(slice0, _ctx);
    kk_string_t _x21180 = kk_string_dup(s); /*string*/
    kk_ssize_t _x21181 = (start0 + len0); /*ssize_t*/
    kk_ssize_t _x21182;
    kk_ssize_t _x21183 = kk_string_len(s,kk_context()); /*ssize_t*/
    kk_ssize_t _x21184 = (start0 + len0); /*ssize_t*/
    _x21182 = (_x21183 - _x21184); /*ssize_t*/
    return kk_std_core__new_Sslice(_x21180, _x21181, _x21182, _ctx);
  }
}
 
// monadic lift

bool kk_std_core__mlift18014_all(kk_function_t predicate, kk_std_core__list xx, bool _y_17714, kk_context_t* _ctx) { /* forall<a,e> (predicate : (a) -> e bool, xx : list<a>, bool) -> e bool */ 
  if (_y_17714) {
    return kk_std_core_all(xx, predicate, _ctx);
  }
  {
    kk_std_core__list_drop(xx, _ctx);
    kk_function_drop(predicate, _ctx);
    return false;
  }
}
 
// Do all elements satisfy a predicate ?


// lift anonymous function
struct kk_std_core_all_fun21188__t {
  struct kk_function_s _base;
  kk_function_t predicate0;
  kk_std_core__list xx0;
};
static kk_box_t kk_std_core_all_fun21188(kk_function_t _fself, kk_box_t _b_19142, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_all_fun21188(kk_function_t predicate0, kk_std_core__list xx0, kk_context_t* _ctx) {
  struct kk_std_core_all_fun21188__t* _self = kk_function_alloc_as(struct kk_std_core_all_fun21188__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_all_fun21188, kk_context());
  _self->predicate0 = predicate0;
  _self->xx0 = xx0;
  return &_self->_base;
}

static kk_box_t kk_std_core_all_fun21188(kk_function_t _fself, kk_box_t _b_19142, kk_context_t* _ctx) {
  struct kk_std_core_all_fun21188__t* _self = kk_function_as(struct kk_std_core_all_fun21188__t*, _fself);
  kk_function_t predicate0 = _self->predicate0; /* (9971) -> 9972 bool */
  kk_std_core__list xx0 = _self->xx0; /* list<9971> */
  kk_drop_match(_self, {kk_function_dup(predicate0);kk_std_core__list_dup(xx0);}, {}, _ctx)
  bool _y_19144_177140 = kk_bool_unbox(_b_19142); /*bool*/;
  bool _x21189 = kk_std_core__mlift18014_all(predicate0, xx0, _y_19144_177140, _ctx); /*bool*/
  return kk_bool_box(_x21189);
}

bool kk_std_core_all(kk_std_core__list xs, kk_function_t predicate0, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, predicate : (a) -> e bool) -> e bool */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21185 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con21185->head;
    kk_std_core__list xx0 = _con21185->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(xs, _ctx);
    }
    bool x0_18225;
    kk_function_t _x21186 = kk_function_dup(predicate0); /*(9971) -> 9972 bool*/
    x0_18225 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x21186, (_x21186, x, _ctx)); /*bool*/
    if (kk_yielding(kk_context())) {
      kk_box_t _x21187 = kk_std_core_hnd_yield_extend(kk_std_core_new_all_fun21188(predicate0, xx0, _ctx), _ctx); /*3926*/
      return kk_bool_unbox(_x21187);
    }
    if (x0_18225) { // tailcall
                    xs = xx0;
                    goto kk__tailcall;
    }
    {
      kk_std_core__list_drop(xx0, _ctx);
      kk_function_drop(predicate0, _ctx);
      return false;
    }
  }
  {
    kk_function_drop(predicate0, _ctx);
    return true;
  }
}
 
// monadic lift

bool kk_std_core__mlift18015_any(kk_function_t predicate, kk_std_core__list xx, bool _y_17718, kk_context_t* _ctx) { /* forall<a,e> (predicate : (a) -> e bool, xx : list<a>, bool) -> e bool */ 
  if (_y_17718) {
    kk_std_core__list_drop(xx, _ctx);
    kk_function_drop(predicate, _ctx);
    return true;
  }
  {
    return kk_std_core_any(xx, predicate, _ctx);
  }
}
 
// Are there any elements in a list that satisfy a predicate ?


// lift anonymous function
struct kk_std_core_any_fun21193__t {
  struct kk_function_s _base;
  kk_function_t predicate0;
  kk_std_core__list xx0;
};
static kk_box_t kk_std_core_any_fun21193(kk_function_t _fself, kk_box_t _b_19146, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_any_fun21193(kk_function_t predicate0, kk_std_core__list xx0, kk_context_t* _ctx) {
  struct kk_std_core_any_fun21193__t* _self = kk_function_alloc_as(struct kk_std_core_any_fun21193__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_any_fun21193, kk_context());
  _self->predicate0 = predicate0;
  _self->xx0 = xx0;
  return &_self->_base;
}

static kk_box_t kk_std_core_any_fun21193(kk_function_t _fself, kk_box_t _b_19146, kk_context_t* _ctx) {
  struct kk_std_core_any_fun21193__t* _self = kk_function_as(struct kk_std_core_any_fun21193__t*, _fself);
  kk_function_t predicate0 = _self->predicate0; /* (9999) -> 10000 bool */
  kk_std_core__list xx0 = _self->xx0; /* list<9999> */
  kk_drop_match(_self, {kk_function_dup(predicate0);kk_std_core__list_dup(xx0);}, {}, _ctx)
  bool _y_19148_177180 = kk_bool_unbox(_b_19146); /*bool*/;
  bool _x21194 = kk_std_core__mlift18015_any(predicate0, xx0, _y_19148_177180, _ctx); /*bool*/
  return kk_bool_box(_x21194);
}

bool kk_std_core_any(kk_std_core__list xs, kk_function_t predicate0, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, predicate : (a) -> e bool) -> e bool */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21190 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con21190->head;
    kk_std_core__list xx0 = _con21190->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(xs, _ctx);
    }
    bool x0_18228;
    kk_function_t _x21191 = kk_function_dup(predicate0); /*(9999) -> 10000 bool*/
    x0_18228 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x21191, (_x21191, x, _ctx)); /*bool*/
    if (kk_yielding(kk_context())) {
      kk_box_t _x21192 = kk_std_core_hnd_yield_extend(kk_std_core_new_any_fun21193(predicate0, xx0, _ctx), _ctx); /*3926*/
      return kk_bool_unbox(_x21192);
    }
    if (x0_18228) {
      kk_std_core__list_drop(xx0, _ctx);
      kk_function_drop(predicate0, _ctx);
      return true;
    }
    { // tailcall
      xs = xx0;
      goto kk__tailcall;
    }
  }
  {
    kk_function_drop(predicate0, _ctx);
    return false;
  }
}
 
// Return the element at position `index` in vector `v`, or `Nothing` if out of bounds

kk_std_core_types__maybe kk_std_core_at(kk_vector_t v, kk_integer_t index, kk_context_t* _ctx) { /* forall<a> (v : vector<a>, index : int) -> maybe<a> */ 
  kk_ssize_t idx;
  kk_integer_t _x21195 = kk_integer_dup(index); /*int*/
  idx = kk_std_core_ssize__t(_x21195, _ctx); /*ssize_t*/
  bool _match_20183;
  kk_ssize_t _x21196 = kk_vector_len_borrow(v); /*ssize_t*/
  _match_20183 = (_x21196 <= idx); /*bool*/
  if (_match_20183) {
    return kk_std_core_types__new_Nothing(_ctx);
  }
  {
    kk_box_t _x21197 = kk_vector_at_borrow(v,idx); /*228*/
    return kk_std_core_types__new_Just(_x21197, _ctx);
  }
}
 
// O(`n`). The first `n` (default = `1`) characters in a string.

kk_std_core__sslice kk_std_core_first(kk_string_t s, kk_std_core_types__optional n, kk_context_t* _ctx) { /* (s : string, n : optional<int>) -> sslice */ 
  kk_std_core__sslice slice0 = kk_std_core_first1(s, _ctx); /*sslice*/;
  bool _match_20177;
  kk_integer_t _brw_20180;
  if (kk_std_core_types__is_Optional(n)) {
    kk_box_t _box_x19149 = n._cons.Optional.value;
    kk_integer_t _n_10142 = kk_integer_unbox(_box_x19149);
    kk_integer_dup(_n_10142);
    _brw_20180 = _n_10142; /*int*/
  }
  else {
    _brw_20180 = kk_integer_from_small(1); /*int*/
  }
  bool _brw_20181 = kk_integer_eq_borrow(_brw_20180,(kk_integer_from_small(1)),kk_context()); /*bool*/;
  kk_integer_drop(_brw_20180, _ctx);
  _match_20177 = _brw_20181; /*bool*/
  if (_match_20177) {
    kk_std_core_types__optional_drop(n, _ctx);
    return slice0;
  }
  {
    kk_integer_t _brw_20178;
    kk_integer_t _x21202;
    if (kk_std_core_types__is_Optional(n)) {
      kk_box_t _box_x19150 = n._cons.Optional.value;
      kk_integer_t _n_101420 = kk_integer_unbox(_box_x19150);
      _x21202 = _n_101420; /*int*/
    }
    else {
      _x21202 = kk_integer_from_small(1); /*int*/
    }
    _brw_20178 = kk_integer_add_small_const(_x21202, -1, _ctx); /*int*/
    kk_std_core__sslice _brw_20179 = kk_std_core_extend(slice0, _brw_20178, _ctx); /*sslice*/;
    kk_integer_drop(_brw_20178, _ctx);
    return _brw_20179;
  }
}
 
// Convert the first character of a string to uppercase.

kk_string_t kk_std_core_capitalize(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> string */ 
  kk_std_core__sslice slice2;
  kk_string_t _x21204 = kk_string_dup(s); /*string*/
  slice2 = kk_std_core_first1(_x21204, _ctx); /*sslice*/
  kk_std_core__sslice slice1_17105;
  bool _match_20170;
  kk_integer_t _brw_20175;
  kk_std_core_types__optional _x21205 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
  if (kk_std_core_types__is_Optional(_x21205)) {
    kk_box_t _box_x19151 = _x21205._cons.Optional.value;
    kk_integer_t _n_101420 = kk_integer_unbox(_box_x19151);
    _brw_20175 = _n_101420; /*int*/
  }
  else {
    _brw_20175 = kk_integer_from_small(1); /*int*/
  }
  bool _brw_20176 = kk_integer_eq_borrow(_brw_20175,(kk_integer_from_small(1)),kk_context()); /*bool*/;
  kk_integer_drop(_brw_20175, _ctx);
  _match_20170 = _brw_20176; /*bool*/
  if (_match_20170) {
    slice1_17105 = slice2; /*sslice*/
  }
  else {
    kk_integer_t _brw_20172;
    kk_integer_t _x21207;
    kk_std_core_types__optional _x21208 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
    if (kk_std_core_types__is_Optional(_x21208)) {
      kk_box_t _box_x19152 = _x21208._cons.Optional.value;
      kk_integer_t _n_1014200 = kk_integer_unbox(_box_x19152);
      _x21207 = _n_1014200; /*int*/
    }
    else {
      _x21207 = kk_integer_from_small(1); /*int*/
    }
    _brw_20172 = kk_integer_add_small_const(_x21207, -1, _ctx); /*int*/
    kk_std_core__sslice _brw_20173 = kk_std_core_extend(slice2, _brw_20172, _ctx); /*sslice*/;
    kk_integer_drop(_brw_20172, _ctx);
    slice1_17105 = _brw_20173; /*sslice*/
  }
  kk_std_core__sslice slice0 = kk_std_core_first1(s, _ctx); /*sslice*/;
  kk_string_t _x21210;
  kk_string_t _x21211;
  kk_std_core__sslice _x21212;
  bool _match_20163;
  kk_integer_t _brw_20168;
  kk_std_core_types__optional _x21213 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
  if (kk_std_core_types__is_Optional(_x21213)) {
    kk_box_t _box_x19153 = _x21213._cons.Optional.value;
    kk_integer_t _n_10142 = kk_integer_unbox(_box_x19153);
    _brw_20168 = _n_10142; /*int*/
  }
  else {
    _brw_20168 = kk_integer_from_small(1); /*int*/
  }
  bool _brw_20169 = kk_integer_eq_borrow(_brw_20168,(kk_integer_from_small(1)),kk_context()); /*bool*/;
  kk_integer_drop(_brw_20168, _ctx);
  _match_20163 = _brw_20169; /*bool*/
  if (_match_20163) {
    _x21212 = slice0; /*sslice*/
  }
  else {
    kk_integer_t _brw_20165;
    kk_integer_t _x21215;
    kk_std_core_types__optional _x21216 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
    if (kk_std_core_types__is_Optional(_x21216)) {
      kk_box_t _box_x19154 = _x21216._cons.Optional.value;
      kk_integer_t _n_101421 = kk_integer_unbox(_box_x19154);
      _x21215 = _n_101421; /*int*/
    }
    else {
      _x21215 = kk_integer_from_small(1); /*int*/
    }
    _brw_20165 = kk_integer_add_small_const(_x21215, -1, _ctx); /*int*/
    kk_std_core__sslice _brw_20166 = kk_std_core_extend(slice0, _brw_20165, _ctx); /*sslice*/;
    kk_integer_drop(_brw_20165, _ctx);
    _x21212 = _brw_20166; /*sslice*/
  }
  _x21211 = kk_std_core_string_3(_x21212, _ctx); /*string*/
  _x21210 = kk_std_core_to_upper(_x21211, _ctx); /*string*/
  kk_string_t _x21218;
  kk_std_core__sslice _x21219;
  {
    kk_string_t s1 = slice1_17105.str;
    kk_ssize_t start0 = slice1_17105.start;
    kk_ssize_t len0 = slice1_17105.len;
    kk_string_dup(s1);
    kk_std_core__sslice_drop(slice1_17105, _ctx);
    kk_string_t _x21220 = kk_string_dup(s1); /*string*/
    kk_ssize_t _x21221 = (start0 + len0); /*ssize_t*/
    kk_ssize_t _x21222;
    kk_ssize_t _x21223 = kk_string_len(s1,kk_context()); /*ssize_t*/
    kk_ssize_t _x21224 = (start0 + len0); /*ssize_t*/
    _x21222 = (_x21223 - _x21224); /*ssize_t*/
    _x21219 = kk_std_core__new_Sslice(_x21220, _x21221, _x21222, _ctx); /*sslice*/
  }
  _x21218 = kk_std_core_string_3(_x21219, _ctx); /*string*/
  return kk_std_core__lp__plus__plus__1_rp_(_x21210, _x21218, _ctx);
}
 
// Catch any exception raised in `action` and handle it.
// Use `on-exn` or `on-exit` when appropiate.


// lift anonymous function
struct kk_std_core_try_fun21225__t {
  struct kk_function_s _base;
  kk_function_t hndl;
};
static kk_box_t kk_std_core_try_fun21225(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__525__16, kk_std_core__exception x, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun21225(kk_function_t hndl, kk_context_t* _ctx) {
  struct kk_std_core_try_fun21225__t* _self = kk_function_alloc_as(struct kk_std_core_try_fun21225__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_try_fun21225, kk_context());
  _self->hndl = hndl;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_try_fun21226__t {
  struct kk_function_s _base;
  kk_function_t hndl;
  kk_std_core__exception x;
};
static kk_box_t kk_std_core_try_fun21226(kk_function_t _fself, kk_function_t ___wildcard__525__45, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun21226(kk_function_t hndl, kk_std_core__exception x, kk_context_t* _ctx) {
  struct kk_std_core_try_fun21226__t* _self = kk_function_alloc_as(struct kk_std_core_try_fun21226__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_try_fun21226, kk_context());
  _self->hndl = hndl;
  _self->x = x;
  return &_self->_base;
}

static kk_box_t kk_std_core_try_fun21226(kk_function_t _fself, kk_function_t ___wildcard__525__45, kk_context_t* _ctx) {
  struct kk_std_core_try_fun21226__t* _self = kk_function_as(struct kk_std_core_try_fun21226__t*, _fself);
  kk_function_t hndl = _self->hndl; /* (exception) -> 10309 10308 */
  kk_std_core__exception x = _self->x; /* exception */
  kk_drop_match(_self, {kk_function_dup(hndl);kk_std_core__exception_dup(x);}, {}, _ctx)
  kk_function_drop(___wildcard__525__45, _ctx);
  return kk_function_call(kk_box_t, (kk_function_t, kk_std_core__exception, kk_context_t*), hndl, (hndl, x, _ctx));
}
static kk_box_t kk_std_core_try_fun21225(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__525__16, kk_std_core__exception x, kk_context_t* _ctx) {
  struct kk_std_core_try_fun21225__t* _self = kk_function_as(struct kk_std_core_try_fun21225__t*, _fself);
  kk_function_t hndl = _self->hndl; /* (exception) -> 10309 10308 */
  kk_drop_match(_self, {kk_function_dup(hndl);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(___wildcard__525__16, (KK_I32(3)), _ctx);
  return kk_std_core_hnd_yield_to_final(m0, kk_std_core_new_try_fun21226(hndl, x, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_try_fun21229__t {
  struct kk_function_s _base;
  kk_function_t _b_19159_19155;
};
static kk_box_t kk_std_core_try_fun21229(kk_function_t _fself, kk_std_core_hnd__marker _b_19156, kk_std_core_hnd__ev _b_19157, kk_box_t _b_19158, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun21229(kk_function_t _b_19159_19155, kk_context_t* _ctx) {
  struct kk_std_core_try_fun21229__t* _self = kk_function_alloc_as(struct kk_std_core_try_fun21229__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_try_fun21229, kk_context());
  _self->_b_19159_19155 = _b_19159_19155;
  return &_self->_base;
}

static kk_box_t kk_std_core_try_fun21229(kk_function_t _fself, kk_std_core_hnd__marker _b_19156, kk_std_core_hnd__ev _b_19157, kk_box_t _b_19158, kk_context_t* _ctx) {
  struct kk_std_core_try_fun21229__t* _self = kk_function_as(struct kk_std_core_try_fun21229__t*, _fself);
  kk_function_t _b_19159_19155 = _self->_b_19159_19155; /* (m0 : std/core/hnd/marker<10309,10308>, std/core/hnd/ev<.hnd-exn>, x : exception) -> 10309 10295 */
  kk_drop_match(_self, {kk_function_dup(_b_19159_19155);}, {}, _ctx)
  kk_std_core__exception _x21230 = kk_std_core__exception_unbox(_b_19158, _ctx); /*exception*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_std_core__exception, kk_context_t*), _b_19159_19155, (_b_19159_19155, _b_19156, _b_19157, _x21230, _ctx));
}


// lift anonymous function
struct kk_std_core_try_fun21231__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_try_fun21231(kk_function_t _fself, kk_box_t _x, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun21231(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_try_fun21231, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_try_fun21231(kk_function_t _fself, kk_box_t _x, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _x;
}

kk_box_t kk_std_core_try(kk_function_t action, kk_function_t hndl, kk_context_t* _ctx) { /* forall<a,e> (action : () -> <exn|e> a, hndl : (exception) -> e a) -> e a */ 
  kk_function_t _b_19159_19155 = kk_std_core_new_try_fun21225(hndl, _ctx); /*(m0 : std/core/hnd/marker<10309,10308>, std/core/hnd/ev<.hnd-exn>, x : exception) -> 10309 10295*/;
  kk_std_core__hnd_exn _x21227;
  kk_std_core_hnd__clause1 _x21228 = kk_std_core_hnd__new_Clause1(kk_std_core_new_try_fun21229(_b_19159_19155, _ctx), _ctx); /*std/core/hnd/clause1<51,52,53,54,55>*/
  _x21227 = kk_std_core__new_Hnd_exn(kk_reuse_null, _x21228, _ctx); /*.hnd-exn<11,12>*/
  return kk_std_core__handle_exn((KK_I32(0)), _x21227, kk_std_core_new_try_fun21231(_ctx), action, _ctx);
}
 
// Transform an exception effect to an  `:error` type.


// lift anonymous function
struct kk_std_core_try_fun21233__t_1 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_try_fun21233_1(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__525__16, kk_std_core__exception x, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun21233_1(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_try_fun21233_1, _ctx)
  return kk_function_dup(_fself);
}



// lift anonymous function
struct kk_std_core_try_fun21234__t_1 {
  struct kk_function_s _base;
  kk_std_core__exception x;
};
static kk_box_t kk_std_core_try_fun21234_1(kk_function_t _fself, kk_function_t _b_19162, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun21234_1(kk_std_core__exception x, kk_context_t* _ctx) {
  struct kk_std_core_try_fun21234__t_1* _self = kk_function_alloc_as(struct kk_std_core_try_fun21234__t_1, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_try_fun21234_1, kk_context());
  _self->x = x;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_try_fun21235__t_1 {
  struct kk_function_s _base;
  kk_function_t _b_19162;
};
static kk_std_core__error kk_std_core_try_fun21235_1(kk_function_t _fself, kk_std_core_hnd__resume_result _b_19163, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun21235_1(kk_function_t _b_19162, kk_context_t* _ctx) {
  struct kk_std_core_try_fun21235__t_1* _self = kk_function_alloc_as(struct kk_std_core_try_fun21235__t_1, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_try_fun21235_1, kk_context());
  _self->_b_19162 = _b_19162;
  return &_self->_base;
}

static kk_std_core__error kk_std_core_try_fun21235_1(kk_function_t _fself, kk_std_core_hnd__resume_result _b_19163, kk_context_t* _ctx) {
  struct kk_std_core_try_fun21235__t_1* _self = kk_function_as(struct kk_std_core_try_fun21235__t_1*, _fself);
  kk_function_t _b_19162 = _self->_b_19162; /* (std/core/hnd/resume-result<3990,3993>) -> 3992 3993 */
  kk_drop_match(_self, {kk_function_dup(_b_19162);}, {}, _ctx)
  kk_box_t _x21236 = kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__resume_result, kk_context_t*), _b_19162, (_b_19162, _b_19163, _ctx)); /*3993*/
  return kk_std_core__error_unbox(_x21236, _ctx);
}
static kk_box_t kk_std_core_try_fun21234_1(kk_function_t _fself, kk_function_t _b_19162, kk_context_t* _ctx) {
  struct kk_std_core_try_fun21234__t_1* _self = kk_function_as(struct kk_std_core_try_fun21234__t_1*, _fself);
  kk_std_core__exception x = _self->x; /* exception */
  kk_drop_match(_self, {kk_std_core__exception_dup(x);}, {}, _ctx)
  kk_function_t ___wildcard__525__45_19183 = kk_std_core_new_try_fun21235_1(_b_19162, _ctx); /*(std/core/hnd/resume-result<10295,error<10347>>) -> 10348 error<10347>*/;
  kk_function_drop(___wildcard__525__45_19183, _ctx);
  kk_std_core__error _x21237 = kk_std_core__new_Error(x, _ctx); /*error<30>*/
  return kk_std_core__error_box(_x21237, _ctx);
}
static kk_box_t kk_std_core_try_fun21233_1(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__525__16, kk_std_core__exception x, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_std_core_hnd__ev_dropn(___wildcard__525__16, (KK_I32(3)), _ctx);
  return kk_std_core_hnd_yield_to_final(m0, kk_std_core_new_try_fun21234_1(x, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_try_fun21240__t_1 {
  struct kk_function_s _base;
  kk_function_t _b_19179_19164;
};
static kk_box_t kk_std_core_try_fun21240_1(kk_function_t _fself, kk_std_core_hnd__marker _b_19165, kk_std_core_hnd__ev _b_19166, kk_box_t _b_19167, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun21240_1(kk_function_t _b_19179_19164, kk_context_t* _ctx) {
  struct kk_std_core_try_fun21240__t_1* _self = kk_function_alloc_as(struct kk_std_core_try_fun21240__t_1, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_try_fun21240_1, kk_context());
  _self->_b_19179_19164 = _b_19179_19164;
  return &_self->_base;
}

static kk_box_t kk_std_core_try_fun21240_1(kk_function_t _fself, kk_std_core_hnd__marker _b_19165, kk_std_core_hnd__ev _b_19166, kk_box_t _b_19167, kk_context_t* _ctx) {
  struct kk_std_core_try_fun21240__t_1* _self = kk_function_as(struct kk_std_core_try_fun21240__t_1*, _fself);
  kk_function_t _b_19179_19164 = _self->_b_19179_19164; /* (m0 : std/core/hnd/marker<10348,error<10347>>, std/core/hnd/ev<.hnd-exn>, x : exception) -> 10348 10295 */
  kk_drop_match(_self, {kk_function_dup(_b_19179_19164);}, {}, _ctx)
  kk_std_core__exception _x21241 = kk_std_core__exception_unbox(_b_19167, _ctx); /*exception*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_std_core__exception, kk_context_t*), _b_19179_19164, (_b_19179_19164, _b_19165, _b_19166, _x21241, _ctx));
}


// lift anonymous function
struct kk_std_core_try_fun21242__t_1 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_try_fun21242_1(kk_function_t _fself, kk_box_t _b_19174, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun21242_1(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_try_fun21242_1, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_try_fun21242_1(kk_function_t _fself, kk_box_t _b_19174, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_std_core__error _x_19184 = kk_std_core__error_unbox(_b_19174, _ctx); /*error<10347>*/;
  return kk_std_core__error_box(_x_19184, _ctx);
}


// lift anonymous function
struct kk_std_core_try_fun21243__t_1 {
  struct kk_function_s _base;
  kk_function_t action;
};
static kk_box_t kk_std_core_try_fun21243_1(kk_function_t _fself, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun21243_1(kk_function_t action, kk_context_t* _ctx) {
  struct kk_std_core_try_fun21243__t_1* _self = kk_function_alloc_as(struct kk_std_core_try_fun21243__t_1, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_try_fun21243_1, kk_context());
  _self->action = action;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_try_fun21246__t_1 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_try_fun21246_1(kk_function_t _fself, kk_box_t _b_19169, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun21246_1(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_try_fun21246_1, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_try_fun21246_1(kk_function_t _fself, kk_box_t _b_19169, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_box_t _y_19185_17723 = _b_19169; /*10347*/;
  kk_std_core__error _x21247 = kk_std_core__new_Ok(_y_19185_17723, _ctx); /*error<30>*/
  return kk_std_core__error_box(_x21247, _ctx);
}
static kk_box_t kk_std_core_try_fun21243_1(kk_function_t _fself, kk_context_t* _ctx) {
  struct kk_std_core_try_fun21243__t_1* _self = kk_function_as(struct kk_std_core_try_fun21243__t_1*, _fself);
  kk_function_t action = _self->action; /* () -> <exn|10348> 10347 */
  kk_drop_match(_self, {kk_function_dup(action);}, {}, _ctx)
  kk_box_t x0_18233 = kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), action, (action, _ctx)); /*10347*/;
  kk_std_core__error _x21244;
  if (kk_yielding(kk_context())) {
    kk_box_drop(x0_18233, _ctx);
    kk_box_t _x21245 = kk_std_core_hnd_yield_extend(kk_std_core_new_try_fun21246_1(_ctx), _ctx); /*3926*/
    _x21244 = kk_std_core__error_unbox(_x21245, _ctx); /*error<10347>*/
  }
  else {
    _x21244 = kk_std_core__new_Ok(x0_18233, _ctx); /*error<10347>*/
  }
  return kk_std_core__error_box(_x21244, _ctx);
}

kk_std_core__error kk_std_core_try_1(kk_function_t action, kk_context_t* _ctx) { /* forall<a,e> (action : () -> <exn|e> a) -> e error<a> */ 
  int32_t _b_19175_19170 = (KK_I32(0)); /*int32*/;
  kk_box_t _x21232;
  kk_function_t _b_19179_19164 = kk_std_core_new_try_fun21233_1(_ctx); /*(m0 : std/core/hnd/marker<10348,error<10347>>, std/core/hnd/ev<.hnd-exn>, x : exception) -> 10348 10295*/;
  kk_std_core__hnd_exn _x21238;
  kk_std_core_hnd__clause1 _x21239 = kk_std_core_hnd__new_Clause1(kk_std_core_new_try_fun21240_1(_b_19179_19164, _ctx), _ctx); /*std/core/hnd/clause1<51,52,53,54,55>*/
  _x21238 = kk_std_core__new_Hnd_exn(kk_reuse_null, _x21239, _ctx); /*.hnd-exn<11,12>*/
  _x21232 = kk_std_core__handle_exn(_b_19175_19170, _x21238, kk_std_core_new_try_fun21242_1(_ctx), kk_std_core_new_try_fun21243_1(action, _ctx), _ctx); /*1969*/
  return kk_std_core__error_unbox(_x21232, _ctx);
}

kk_std_core_types__tuple2_ kk_std_core_cdivmod_exp10(kk_integer_t i, kk_integer_t n, kk_context_t* _ctx) { /* (i : int, n : int) -> (int, int) */ 
  bool _match_20161 = kk_integer_lte_borrow(n,(kk_integer_from_small(0)),kk_context()); /*bool*/;
  if (_match_20161) {
    kk_integer_drop(n, _ctx);
    return kk_std_core_types__new_dash__lp__comma__rp_(kk_integer_box(i), kk_integer_box(kk_integer_from_small(0)), _ctx);
  }
  {
    kk_integer_t cq;
    kk_integer_t _x21248 = kk_integer_dup(i); /*int*/
    kk_integer_t _x21249 = kk_integer_dup(n); /*int*/
    cq = kk_std_core_cdiv_exp10(_x21248, _x21249, _ctx); /*int*/
    kk_integer_t y_17113;
    kk_integer_t _x21250 = kk_integer_dup(cq); /*int*/
    y_17113 = kk_std_core_mul_exp10(_x21250, n, _ctx); /*int*/
    kk_integer_t cr = kk_integer_sub(i,y_17113,kk_context()); /*int*/;
    return kk_std_core_types__new_dash__lp__comma__rp_(kk_integer_box(cq), kk_integer_box(cr), _ctx);
  }
}
 
// Concatenate a list of `:maybe` values

kk_std_core__list kk_std_core__ctail_concat_maybe(kk_std_core__list xs, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* forall<a> (xs : list<maybe<a>>, ctail<list<a>>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21251 = kk_std_core__as_Cons(xs);
    kk_box_t _box_x19194 = _con21251->head;
    kk_std_core__list xx = _con21251->tail;
    kk_std_core_types__maybe x = kk_std_core_types__maybe_unbox(_box_x19194, NULL);
    kk_reuse_t _ru_20390 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core_types__maybe_dup(x);
      kk_box_drop(_box_x19194, _ctx);
      _ru_20390 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_std_core_types__maybe_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    if (kk_std_core_types__is_Just(x)) {
      kk_box_t y = x._cons.Just.value;
      kk_std_core__list _ctail_17565 = kk_std_core__list_hole(); /*list<10487>*/;
      kk_std_core__list _ctail_17566 = kk_std_core__new_Cons(_ru_20390, y, _ctail_17565, _ctx); /*list<10487>*/;
      kk_box_t* _b_19205_19200 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17566)->tail)); /*cfield<list<10487>>*/;
      { // tailcall
        kk_std_core_types__ctail _x21253 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17566, _ctx)),_b_19205_19200); /*ctail<0>*/
        xs = xx;
        _acc = _x21253;
        goto kk__tailcall;
      }
    }
    {
      kk_reuse_drop(_ru_20390,kk_context());
      { // tailcall
        xs = xx;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_box_t _x21254 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21254, _ctx);
  }
}
 
// Concatenate a list of `:maybe` values

kk_std_core__list kk_std_core_concat_maybe(kk_std_core__list xs0, kk_context_t* _ctx) { /* forall<a> (xs : list<maybe<a>>) -> list<a> */ 
  kk_std_core_types__ctail _x21255 = kk_ctail_nil(); /*ctail<0>*/
  return kk_std_core__ctail_concat_maybe(xs0, _x21255, _ctx);
}
 
// monadic lift

kk_std_core_types__maybe kk_std_core__mlift18017_op(kk_function_t action, kk_ssize_t end, kk_ssize_t i, kk_std_core_types__maybe _y_17726, kk_context_t* _ctx) { /* forall<a,e> (action : (ssize_t) -> e maybe<a>, end : ssize_t, i : ssize_t, maybe<a>) -> e maybe<a> */ 
  if (kk_std_core_types__is_Nothing(_y_17726)) {
    kk_ssize_t i0_17459 = kk_std_core_incr_1(i, _ctx); /*ssize_t*/;
    return kk_std_core__lift17285_for_whilez(action, end, i0_17459, _ctx);
  }
  {
    kk_box_t x = _y_17726._cons.Just.value;
    kk_function_drop(action, _ctx);
    return kk_std_core_types__new_Just(x, _ctx);
  }
}
 
// lifted local: for-whilez, rep


// lift anonymous function
struct kk_std_core__lift17285_for_whilez_fun21258__t {
  struct kk_function_s _base;
  kk_function_t action0;
  kk_ssize_t end0;
  kk_ssize_t i0;
};
static kk_box_t kk_std_core__lift17285_for_whilez_fun21258(kk_function_t _fself, kk_box_t _b_19212, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17285_for_whilez_fun21258(kk_function_t action0, kk_ssize_t end0, kk_ssize_t i0, kk_context_t* _ctx) {
  struct kk_std_core__lift17285_for_whilez_fun21258__t* _self = kk_function_alloc_as(struct kk_std_core__lift17285_for_whilez_fun21258__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift17285_for_whilez_fun21258, kk_context());
  _self->action0 = action0;
  _self->end0 = end0;
  _self->i0 = i0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift17285_for_whilez_fun21258(kk_function_t _fself, kk_box_t _b_19212, kk_context_t* _ctx) {
  struct kk_std_core__lift17285_for_whilez_fun21258__t* _self = kk_function_as(struct kk_std_core__lift17285_for_whilez_fun21258__t*, _fself);
  kk_function_t action0 = _self->action0; /* (ssize_t) -> 10542 maybe<10541> */
  kk_ssize_t end0 = _self->end0; /* ssize_t */
  kk_ssize_t i0 = _self->i0; /* ssize_t */
  kk_drop_match(_self, {kk_function_dup(action0);;;}, {}, _ctx)
  kk_std_core_types__maybe _y_19214_177260 = kk_std_core_types__maybe_unbox(_b_19212, _ctx); /*maybe<10541>*/;
  kk_std_core_types__maybe _x21259 = kk_std_core__mlift18017_op(action0, end0, i0, _y_19214_177260, _ctx); /*maybe<10541>*/
  return kk_std_core_types__maybe_box(_x21259, _ctx);
}

kk_std_core_types__maybe kk_std_core__lift17285_for_whilez(kk_function_t action0, kk_ssize_t end0, kk_ssize_t i0, kk_context_t* _ctx) { /* forall<a,e> (action : (ssize_t) -> e maybe<a>, end : ssize_t, i : ssize_t) -> e maybe<a> */ 
  kk__tailcall: ;
  bool _match_20159 = (i0 <= end0); /*bool*/;
  if (_match_20159) {
    kk_std_core_types__maybe x0_18236;
    kk_function_t _x21256 = kk_function_dup(action0); /*(ssize_t) -> 10542 maybe<10541>*/
    x0_18236 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_ssize_t, kk_context_t*), _x21256, (_x21256, i0, _ctx)); /*maybe<10541>*/
    if (kk_yielding(kk_context())) {
      kk_std_core_types__maybe_drop(x0_18236, _ctx);
      kk_box_t _x21257 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift17285_for_whilez_fun21258(action0, end0, i0, _ctx), _ctx); /*3926*/
      return kk_std_core_types__maybe_unbox(_x21257, _ctx);
    }
    if (kk_std_core_types__is_Nothing(x0_18236)) {
      kk_ssize_t i0_174590 = kk_std_core_incr_1(i0, _ctx); /*ssize_t*/;
      { // tailcall
        i0 = i0_174590;
        goto kk__tailcall;
      }
    }
    {
      kk_box_t x1 = x0_18236._cons.Just.value;
      kk_function_drop(action0, _ctx);
      return kk_std_core_types__new_Just(x1, _ctx);
    }
  }
  {
    kk_function_drop(action0, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
}
 
// monadic lift

kk_std_core_types__maybe kk_std_core__mlift18018_foreach_while(kk_function_t action, kk_std_core__list xx, kk_std_core_types__maybe _y_17731, kk_context_t* _ctx) { /* forall<a,b,e> (action : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e maybe<b> */ 
  if (kk_std_core_types__is_Nothing(_y_17731)) {
    return kk_std_core_foreach_while(xx, action, _ctx);
  }
  {
    kk_std_core__list_drop(xx, _ctx);
    kk_function_drop(action, _ctx);
    return _y_17731;
  }
}
 
// Invoke `action` for each element of a list while `action` return `Nothing`


// lift anonymous function
struct kk_std_core_foreach_while_fun21266__t {
  struct kk_function_s _base;
  kk_function_t action0;
  kk_std_core__list xx0;
};
static kk_box_t kk_std_core_foreach_while_fun21266(kk_function_t _fself, kk_box_t _b_19216, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_while_fun21266(kk_function_t action0, kk_std_core__list xx0, kk_context_t* _ctx) {
  struct kk_std_core_foreach_while_fun21266__t* _self = kk_function_alloc_as(struct kk_std_core_foreach_while_fun21266__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foreach_while_fun21266, kk_context());
  _self->action0 = action0;
  _self->xx0 = xx0;
  return &_self->_base;
}

static kk_box_t kk_std_core_foreach_while_fun21266(kk_function_t _fself, kk_box_t _b_19216, kk_context_t* _ctx) {
  struct kk_std_core_foreach_while_fun21266__t* _self = kk_function_as(struct kk_std_core_foreach_while_fun21266__t*, _fself);
  kk_function_t action0 = _self->action0; /* (10675) -> 10677 maybe<10676> */
  kk_std_core__list xx0 = _self->xx0; /* list<10675> */
  kk_drop_match(_self, {kk_function_dup(action0);kk_std_core__list_dup(xx0);}, {}, _ctx)
  kk_std_core_types__maybe _y_19218_177310 = kk_std_core_types__maybe_unbox(_b_19216, _ctx); /*maybe<10676>*/;
  kk_std_core_types__maybe _x21267 = kk_std_core__mlift18018_foreach_while(action0, xx0, _y_19218_177310, _ctx); /*maybe<10676>*/
  return kk_std_core_types__maybe_box(_x21267, _ctx);
}

kk_std_core_types__maybe kk_std_core_foreach_while(kk_std_core__list xs, kk_function_t action0, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, action : (a) -> e maybe<b>) -> e maybe<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Nil(xs)) {
    kk_function_drop(action0, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  {
    struct kk_std_core_Cons* _con21263 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con21263->head;
    kk_std_core__list xx0 = _con21263->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_std_core_types__maybe x0_18239;
    kk_function_t _x21264 = kk_function_dup(action0); /*(10675) -> 10677 maybe<10676>*/
    x0_18239 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_box_t, kk_context_t*), _x21264, (_x21264, x, _ctx)); /*maybe<10676>*/
    if (kk_yielding(kk_context())) {
      kk_std_core_types__maybe_drop(x0_18239, _ctx);
      kk_box_t _x21265 = kk_std_core_hnd_yield_extend(kk_std_core_new_foreach_while_fun21266(action0, xx0, _ctx), _ctx); /*3926*/
      return kk_std_core_types__maybe_unbox(_x21265, _ctx);
    }
    if (kk_std_core_types__is_Nothing(x0_18239)) { // tailcall
                                                   xs = xx0;
                                                   goto kk__tailcall;
    }
    {
      kk_std_core__list_drop(xx0, _ctx);
      kk_function_drop(action0, _ctx);
      return x0_18239;
    }
  }
}
 
// monadic lift

kk_std_core_types__maybe kk_std_core__mlift18019_foreach_while_1(kk_function_t action, kk_std_core__sslice rest, kk_std_core_types__maybe _y_17735, kk_context_t* _ctx) { /* forall<a,e> (action : (c : char) -> e maybe<a>, rest : sslice, maybe<a>) -> e maybe<a> */ 
  if (kk_std_core_types__is_Nothing(_y_17735)) {
    return kk_std_core_foreach_while_1(rest, action, _ctx);
  }
  {
    kk_std_core__sslice_drop(rest, _ctx);
    kk_function_drop(action, _ctx);
    return _y_17735;
  }
}
 
// Apply a function for each character in a string slice.
// If `action` returns `Just`, the function returns immediately with that result.


// lift anonymous function
struct kk_std_core_foreach_while_fun21273__t_1 {
  struct kk_function_s _base;
  kk_function_t action0;
  kk_std_core__sslice rest0;
};
static kk_box_t kk_std_core_foreach_while_fun21273_1(kk_function_t _fself, kk_box_t _b_19223, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_while_fun21273_1(kk_function_t action0, kk_std_core__sslice rest0, kk_context_t* _ctx) {
  struct kk_std_core_foreach_while_fun21273__t_1* _self = kk_function_alloc_as(struct kk_std_core_foreach_while_fun21273__t_1, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foreach_while_fun21273_1, kk_context());
  _self->action0 = action0;
  _self->rest0 = rest0;
  return &_self->_base;
}

static kk_box_t kk_std_core_foreach_while_fun21273_1(kk_function_t _fself, kk_box_t _b_19223, kk_context_t* _ctx) {
  struct kk_std_core_foreach_while_fun21273__t_1* _self = kk_function_as(struct kk_std_core_foreach_while_fun21273__t_1*, _fself);
  kk_function_t action0 = _self->action0; /* (c : char) -> 10762 maybe<10761> */
  kk_std_core__sslice rest0 = _self->rest0; /* sslice */
  kk_drop_match(_self, {kk_function_dup(action0);kk_std_core__sslice_dup(rest0);}, {}, _ctx)
  kk_std_core_types__maybe _y_19225_177350 = kk_std_core_types__maybe_unbox(_b_19223, _ctx); /*maybe<10761>*/;
  kk_std_core_types__maybe _x21274 = kk_std_core__mlift18019_foreach_while_1(action0, rest0, _y_19225_177350, _ctx); /*maybe<10761>*/
  return kk_std_core_types__maybe_box(_x21274, _ctx);
}

kk_std_core_types__maybe kk_std_core_foreach_while_1(kk_std_core__sslice slice0, kk_function_t action0, kk_context_t* _ctx) { /* forall<a,e> (slice : sslice, action : (c : char) -> e maybe<a>) -> e maybe<a> */ 
  kk__tailcall: ;
  kk_std_core_types__maybe _match_20156 = kk_std_core_next(slice0, _ctx); /*maybe<(char, sslice)>*/;
  if (kk_std_core_types__is_Nothing(_match_20156)) {
    kk_function_drop(action0, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  {
    kk_box_t _box_x19219 = _match_20156._cons.Just.value;
    kk_std_core_types__tuple2_ _pat1 = kk_std_core_types__tuple2__unbox(_box_x19219, NULL);
    kk_box_t _box_x19220 = _pat1.fst;
    kk_box_t _box_x19221 = _pat1.snd;
    kk_char_t c = kk_char_unbox(_box_x19220, NULL);
    kk_std_core__sslice rest0 = kk_std_core__sslice_unbox(_box_x19221, NULL);
    kk_std_core__sslice_dup(rest0);
    kk_std_core_types__maybe_drop(_match_20156, _ctx);
    kk_std_core_types__maybe x_18242;
    kk_function_t _x21271 = kk_function_dup(action0); /*(c : char) -> 10762 maybe<10761>*/
    x_18242 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_char_t, kk_context_t*), _x21271, (_x21271, c, _ctx)); /*maybe<10761>*/
    if (kk_yielding(kk_context())) {
      kk_std_core_types__maybe_drop(x_18242, _ctx);
      kk_box_t _x21272 = kk_std_core_hnd_yield_extend(kk_std_core_new_foreach_while_fun21273_1(action0, rest0, _ctx), _ctx); /*3926*/
      return kk_std_core_types__maybe_unbox(_x21272, _ctx);
    }
    if (kk_std_core_types__is_Nothing(x_18242)) { // tailcall
                                                  slice0 = rest0;
                                                  goto kk__tailcall;
    }
    {
      kk_std_core__sslice_drop(rest0, _ctx);
      kk_function_drop(action0, _ctx);
      return x_18242;
    }
  }
}
 
// monadic lift

kk_std_core_types__maybe kk_std_core__mlift18020_op(kk_ssize_t end_17115, kk_function_t f, kk_ssize_t i, kk_vector_t v, kk_std_core_types__maybe _y_17740, kk_context_t* _ctx) { /* forall<a,b,e> (end.17115 : ssize_t, f : (a) -> e maybe<b>, i : ssize_t, v : vector<a>, maybe<b>) -> e maybe<b> */ 
  if (kk_std_core_types__is_Nothing(_y_17740)) {
    kk_ssize_t i0_17460 = kk_std_core_incr_1(i, _ctx); /*ssize_t*/;
    return kk_std_core__lift17286_foreach_while_3(end_17115, f, v, i0_17460, _ctx);
  }
  {
    kk_box_t x = _y_17740._cons.Just.value;
    kk_vector_drop(v, _ctx);
    kk_function_drop(f, _ctx);
    return kk_std_core_types__new_Just(x, _ctx);
  }
}
 
// lifted local: foreach-while.3, rep


// lift anonymous function
struct kk_std_core__lift17286_foreach_while_fun21282__t_3 {
  struct kk_function_s _base;
  kk_function_t f0;
  kk_vector_t v0;
  kk_ssize_t end_171150;
  kk_ssize_t i0;
};
static kk_box_t kk_std_core__lift17286_foreach_while_fun21282_3(kk_function_t _fself, kk_box_t _b_19227, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17286_foreach_while_fun21282_3(kk_function_t f0, kk_vector_t v0, kk_ssize_t end_171150, kk_ssize_t i0, kk_context_t* _ctx) {
  struct kk_std_core__lift17286_foreach_while_fun21282__t_3* _self = kk_function_alloc_as(struct kk_std_core__lift17286_foreach_while_fun21282__t_3, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift17286_foreach_while_fun21282_3, kk_context());
  _self->f0 = f0;
  _self->v0 = v0;
  _self->end_171150 = end_171150;
  _self->i0 = i0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift17286_foreach_while_fun21282_3(kk_function_t _fself, kk_box_t _b_19227, kk_context_t* _ctx) {
  struct kk_std_core__lift17286_foreach_while_fun21282__t_3* _self = kk_function_as(struct kk_std_core__lift17286_foreach_while_fun21282__t_3*, _fself);
  kk_function_t f0 = _self->f0; /* (10860) -> 10862 maybe<10861> */
  kk_vector_t v0 = _self->v0; /* vector<10860> */
  kk_ssize_t end_171150 = _self->end_171150; /* ssize_t */
  kk_ssize_t i0 = _self->i0; /* ssize_t */
  kk_drop_match(_self, {kk_function_dup(f0);kk_vector_dup(v0);;;}, {}, _ctx)
  kk_std_core_types__maybe _y_19229_177400 = kk_std_core_types__maybe_unbox(_b_19227, _ctx); /*maybe<10861>*/;
  kk_std_core_types__maybe _x21283 = kk_std_core__mlift18020_op(end_171150, f0, i0, v0, _y_19229_177400, _ctx); /*maybe<10861>*/
  return kk_std_core_types__maybe_box(_x21283, _ctx);
}

kk_std_core_types__maybe kk_std_core__lift17286_foreach_while_3(kk_ssize_t end_171150, kk_function_t f0, kk_vector_t v0, kk_ssize_t i0, kk_context_t* _ctx) { /* forall<a,b,e> (end.17115 : ssize_t, f : (a) -> e maybe<b>, v : vector<a>, i : ssize_t) -> e maybe<b> */ 
  kk__tailcall: ;
  bool _match_20154 = (i0 <= end_171150); /*bool*/;
  if (_match_20154) {
    kk_std_core_types__maybe x0_18245;
    kk_function_t _x21280 = kk_function_dup(f0); /*(10860) -> 10862 maybe<10861>*/
    kk_box_t _x21279 = kk_vector_at_borrow(v0,i0); /*228*/
    x0_18245 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_box_t, kk_context_t*), _x21280, (_x21280, _x21279, _ctx)); /*maybe<10861>*/
    if (kk_yielding(kk_context())) {
      kk_std_core_types__maybe_drop(x0_18245, _ctx);
      kk_box_t _x21281 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift17286_foreach_while_fun21282_3(f0, v0, end_171150, i0, _ctx), _ctx); /*3926*/
      return kk_std_core_types__maybe_unbox(_x21281, _ctx);
    }
    if (kk_std_core_types__is_Nothing(x0_18245)) {
      kk_ssize_t i0_174600 = kk_std_core_incr_1(i0, _ctx); /*ssize_t*/;
      { // tailcall
        i0 = i0_174600;
        goto kk__tailcall;
      }
    }
    {
      kk_box_t x1 = x0_18245._cons.Just.value;
      kk_vector_drop(v0, _ctx);
      kk_function_drop(f0, _ctx);
      return kk_std_core_types__new_Just(x1, _ctx);
    }
  }
  {
    kk_vector_drop(v0, _ctx);
    kk_function_drop(f0, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
}
 
// monadic lift

kk_unit_t kk_std_core__mlift18021_foreach(kk_function_t action, kk_std_core__list xx, kk_unit_t wild__, kk_context_t* _ctx) { /* forall<a,e> (action : (a) -> e (), xx : list<a>, wild_ : ()) -> e () */ 
  kk_std_core_foreach(xx, action, _ctx); return kk_Unit;
}
 
// Invoke `action` for each element of a list


// lift anonymous function
struct kk_std_core_foreach_fun21288__t {
  struct kk_function_s _base;
  kk_function_t action0;
  kk_std_core__list xx0;
};
static kk_box_t kk_std_core_foreach_fun21288(kk_function_t _fself, kk_box_t _b_19231, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_fun21288(kk_function_t action0, kk_std_core__list xx0, kk_context_t* _ctx) {
  struct kk_std_core_foreach_fun21288__t* _self = kk_function_alloc_as(struct kk_std_core_foreach_fun21288__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foreach_fun21288, kk_context());
  _self->action0 = action0;
  _self->xx0 = xx0;
  return &_self->_base;
}

static kk_box_t kk_std_core_foreach_fun21288(kk_function_t _fself, kk_box_t _b_19231, kk_context_t* _ctx) {
  struct kk_std_core_foreach_fun21288__t* _self = kk_function_as(struct kk_std_core_foreach_fun21288__t*, _fself);
  kk_function_t action0 = _self->action0; /* (11054) -> 11055 () */
  kk_std_core__list xx0 = _self->xx0; /* list<11054> */
  kk_drop_match(_self, {kk_function_dup(action0);kk_std_core__list_dup(xx0);}, {}, _ctx)
  kk_unit_t wild__0_19233 = kk_Unit;
  kk_unit_unbox(_b_19231);
  kk_unit_t _x21289 = kk_Unit;
  kk_std_core__mlift18021_foreach(action0, xx0, wild__0_19233, _ctx);
  return kk_unit_box(_x21289);
}

kk_unit_t kk_std_core_foreach(kk_std_core__list xs, kk_function_t action0, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, action : (a) -> e ()) -> e () */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21285 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con21285->head;
    kk_std_core__list xx0 = _con21285->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_unit_t x0_18248 = kk_Unit;
    kk_function_t _x21286 = kk_function_dup(action0); /*(11054) -> 11055 ()*/
    kk_function_call(kk_unit_t, (kk_function_t, kk_box_t, kk_context_t*), _x21286, (_x21286, x, _ctx));
    if (kk_yielding(kk_context())) {
      kk_box_t _x21287 = kk_std_core_hnd_yield_extend(kk_std_core_new_foreach_fun21288(action0, xx0, _ctx), _ctx); /*3926*/
      kk_unit_unbox(_x21287); return kk_Unit;
    }
    { // tailcall
      xs = xx0;
      goto kk__tailcall;
    }
  }
  {
    kk_function_drop(action0, _ctx);
    kk_Unit; return kk_Unit;
  }
}
 
// monadic lift

kk_std_core_types__maybe kk_std_core__mlift18022_op(kk_function_t action, kk_std_core__sslice rest, kk_unit_t wild__, kk_context_t* _ctx) { /* forall<_a,e> (action : (c : char) -> e (), rest : sslice, wild_ : ()) -> e maybe<_a> */ 
  return kk_std_core__lift17486_foreach_1(action, rest, _ctx);
}
 
// lifted local: foreach.1, .spec.17417
// specialized: std/core/foreach-while.1, on parameters action, using:
// action = fn<(e :: E)>(c: char){
//   val _ : ()
//         = action(c);
//   std/core/types/Nothing<_a>;
// }


// lift anonymous function
struct kk_std_core__lift17486_foreach_fun21295__t_1 {
  struct kk_function_s _base;
  kk_function_t action0;
  kk_std_core__sslice rest0;
};
static kk_box_t kk_std_core__lift17486_foreach_fun21295_1(kk_function_t _fself, kk_box_t _b_19238, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17486_foreach_fun21295_1(kk_function_t action0, kk_std_core__sslice rest0, kk_context_t* _ctx) {
  struct kk_std_core__lift17486_foreach_fun21295__t_1* _self = kk_function_alloc_as(struct kk_std_core__lift17486_foreach_fun21295__t_1, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift17486_foreach_fun21295_1, kk_context());
  _self->action0 = action0;
  _self->rest0 = rest0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift17486_foreach_fun21295_1(kk_function_t _fself, kk_box_t _b_19238, kk_context_t* _ctx) {
  struct kk_std_core__lift17486_foreach_fun21295__t_1* _self = kk_function_as(struct kk_std_core__lift17486_foreach_fun21295__t_1*, _fself);
  kk_function_t action0 = _self->action0; /* (c : char) -> 11113 () */
  kk_std_core__sslice rest0 = _self->rest0; /* sslice */
  kk_drop_match(_self, {kk_function_dup(action0);kk_std_core__sslice_dup(rest0);}, {}, _ctx)
  kk_unit_t wild__0_19240 = kk_Unit;
  kk_unit_unbox(_b_19238);
  kk_std_core_types__maybe _x21296 = kk_std_core__mlift18022_op(action0, rest0, wild__0_19240, _ctx); /*maybe<_11100>*/
  return kk_std_core_types__maybe_box(_x21296, _ctx);
}

kk_std_core_types__maybe kk_std_core__lift17486_foreach_1(kk_function_t action0, kk_std_core__sslice slice00, kk_context_t* _ctx) { /* forall<_a,e> (action : (c : char) -> e (), slice00 : sslice) -> e maybe<_a> */ 
  kk__tailcall: ;
  kk_std_core_types__maybe _match_20151 = kk_std_core_next(slice00, _ctx); /*maybe<(char, sslice)>*/;
  if (kk_std_core_types__is_Nothing(_match_20151)) {
    kk_function_drop(action0, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  {
    kk_box_t _box_x19234 = _match_20151._cons.Just.value;
    kk_std_core_types__tuple2_ _pat1 = kk_std_core_types__tuple2__unbox(_box_x19234, NULL);
    kk_box_t _box_x19235 = _pat1.fst;
    kk_box_t _box_x19236 = _pat1.snd;
    kk_char_t c0 = kk_char_unbox(_box_x19235, NULL);
    kk_std_core__sslice rest0 = kk_std_core__sslice_unbox(_box_x19236, NULL);
    kk_std_core__sslice_dup(rest0);
    kk_std_core_types__maybe_drop(_match_20151, _ctx);
    kk_unit_t x_18251 = kk_Unit;
    kk_function_t _x21293 = kk_function_dup(action0); /*(c : char) -> 11113 ()*/
    kk_function_call(kk_unit_t, (kk_function_t, kk_char_t, kk_context_t*), _x21293, (_x21293, c0, _ctx));
    if (kk_yielding(kk_context())) {
      kk_box_t _x21294 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift17486_foreach_fun21295_1(action0, rest0, _ctx), _ctx); /*3926*/
      return kk_std_core_types__maybe_unbox(_x21294, _ctx);
    }
    { // tailcall
      slice00 = rest0;
      goto kk__tailcall;
    }
  }
}
 
// Apply a function for each character in a string slice.


// lift anonymous function
struct kk_std_core_foreach_fun21298__t_1 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_foreach_fun21298_1(kk_function_t _fself, kk_box_t _b_19242, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_fun21298_1(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_foreach_fun21298_1, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_foreach_fun21298_1(kk_function_t _fself, kk_box_t _b_19242, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_std_core_types__maybe wild__0_19244 = kk_std_core_types__maybe_unbox(_b_19242, _ctx); /*maybe<_11100>*/;
  kk_std_core_types__maybe_drop(wild__0_19244, _ctx);
  return kk_unit_box(kk_Unit);
}

kk_unit_t kk_std_core_foreach_1(kk_std_core__sslice slice0, kk_function_t action, kk_context_t* _ctx) { /* forall<e> (slice : sslice, action : (c : char) -> e ()) -> e () */ 
  kk_std_core_types__maybe x_18254 = kk_std_core__lift17486_foreach_1(action, slice0, _ctx); /*maybe<_11100>*/;
  kk_std_core_types__maybe_drop(x_18254, _ctx);
  if (kk_yielding(kk_context())) {
    kk_box_t _x21297 = kk_std_core_hnd_yield_extend(kk_std_core_new_foreach_fun21298_1(_ctx), _ctx); /*3926*/
    kk_unit_unbox(_x21297); return kk_Unit;
  }
  {
    kk_Unit; return kk_Unit;
  }
}
 
// monadic lift

kk_std_core_types__maybe kk_std_core__mlift18024_op(kk_function_t action, kk_std_core__sslice rest, kk_unit_t wild__, kk_context_t* _ctx) { /* forall<_a,e> (action : (c : char) -> e (), rest : sslice, wild_ : ()) -> e maybe<_a> */ 
  return kk_std_core__lift17487_foreach_2(action, rest, _ctx);
}
 
// lifted local: foreach.2, .spec.17419
// specialized: std/core/foreach-while.1, on parameters action, using:
// action = fn<(e :: E)>(c: char){
//   val _ : ()
//         = action(c);
//   std/core/types/Nothing<_a>;
// }


// lift anonymous function
struct kk_std_core__lift17487_foreach_fun21304__t_2 {
  struct kk_function_s _base;
  kk_function_t action0;
  kk_std_core__sslice rest0;
};
static kk_box_t kk_std_core__lift17487_foreach_fun21304_2(kk_function_t _fself, kk_box_t _b_19249, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17487_foreach_fun21304_2(kk_function_t action0, kk_std_core__sslice rest0, kk_context_t* _ctx) {
  struct kk_std_core__lift17487_foreach_fun21304__t_2* _self = kk_function_alloc_as(struct kk_std_core__lift17487_foreach_fun21304__t_2, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift17487_foreach_fun21304_2, kk_context());
  _self->action0 = action0;
  _self->rest0 = rest0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift17487_foreach_fun21304_2(kk_function_t _fself, kk_box_t _b_19249, kk_context_t* _ctx) {
  struct kk_std_core__lift17487_foreach_fun21304__t_2* _self = kk_function_as(struct kk_std_core__lift17487_foreach_fun21304__t_2*, _fself);
  kk_function_t action0 = _self->action0; /* (c : char) -> 11152 () */
  kk_std_core__sslice rest0 = _self->rest0; /* sslice */
  kk_drop_match(_self, {kk_function_dup(action0);kk_std_core__sslice_dup(rest0);}, {}, _ctx)
  kk_unit_t wild__0_19251 = kk_Unit;
  kk_unit_unbox(_b_19249);
  kk_std_core_types__maybe _x21305 = kk_std_core__mlift18024_op(action0, rest0, wild__0_19251, _ctx); /*maybe<_11100>*/
  return kk_std_core_types__maybe_box(_x21305, _ctx);
}

kk_std_core_types__maybe kk_std_core__lift17487_foreach_2(kk_function_t action0, kk_std_core__sslice slice0, kk_context_t* _ctx) { /* forall<_a,e> (action : (c : char) -> e (), slice0 : sslice) -> e maybe<_a> */ 
  kk__tailcall: ;
  kk_std_core_types__maybe _match_20148 = kk_std_core_next(slice0, _ctx); /*maybe<(char, sslice)>*/;
  if (kk_std_core_types__is_Nothing(_match_20148)) {
    kk_function_drop(action0, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  {
    kk_box_t _box_x19245 = _match_20148._cons.Just.value;
    kk_std_core_types__tuple2_ _pat1 = kk_std_core_types__tuple2__unbox(_box_x19245, NULL);
    kk_box_t _box_x19246 = _pat1.fst;
    kk_box_t _box_x19247 = _pat1.snd;
    kk_char_t c0 = kk_char_unbox(_box_x19246, NULL);
    kk_std_core__sslice rest0 = kk_std_core__sslice_unbox(_box_x19247, NULL);
    kk_std_core__sslice_dup(rest0);
    kk_std_core_types__maybe_drop(_match_20148, _ctx);
    kk_unit_t x_18258 = kk_Unit;
    kk_function_t _x21302 = kk_function_dup(action0); /*(c : char) -> 11152 ()*/
    kk_function_call(kk_unit_t, (kk_function_t, kk_char_t, kk_context_t*), _x21302, (_x21302, c0, _ctx));
    if (kk_yielding(kk_context())) {
      kk_box_t _x21303 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift17487_foreach_fun21304_2(action0, rest0, _ctx), _ctx); /*3926*/
      return kk_std_core_types__maybe_unbox(_x21303, _ctx);
    }
    { // tailcall
      slice0 = rest0;
      goto kk__tailcall;
    }
  }
}
 
// Invoke a function for each character in a string


// lift anonymous function
struct kk_std_core_foreach_fun21310__t_2 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_foreach_fun21310_2(kk_function_t _fself, kk_box_t _b_19253, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_fun21310_2(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_foreach_fun21310_2, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_foreach_fun21310_2(kk_function_t _fself, kk_box_t _b_19253, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_std_core_types__maybe wild__0_19255 = kk_std_core_types__maybe_unbox(_b_19253, _ctx); /*maybe<_11100>*/;
  kk_std_core_types__maybe_drop(wild__0_19255, _ctx);
  return kk_unit_box(kk_Unit);
}

kk_unit_t kk_std_core_foreach_2(kk_string_t s, kk_function_t action, kk_context_t* _ctx) { /* forall<e> (s : string, action : (c : char) -> e ()) -> e () */ 
  kk_std_core__sslice slice0_17117;
  kk_string_t _x21306 = kk_string_dup(s); /*string*/
  kk_ssize_t _x21307 = (KK_IZ(0)); /*ssize_t*/
  kk_ssize_t _x21308 = kk_string_len(s,kk_context()); /*ssize_t*/
  slice0_17117 = kk_std_core__new_Sslice(_x21306, _x21307, _x21308, _ctx); /*sslice*/
  kk_std_core_types__maybe x_18261 = kk_std_core__lift17487_foreach_2(action, slice0_17117, _ctx); /*maybe<_11100>*/;
  kk_std_core_types__maybe_drop(x_18261, _ctx);
  if (kk_yielding(kk_context())) {
    kk_box_t _x21309 = kk_std_core_hnd_yield_extend(kk_std_core_new_foreach_fun21310_2(_ctx), _ctx); /*3926*/
    kk_unit_unbox(_x21309); return kk_Unit;
  }
  {
    kk_Unit; return kk_Unit;
  }
}
 
// monadic lift

kk_unit_t kk_std_core__mlift18026_op(kk_ssize_t end_17122, kk_function_t f, kk_ssize_t i, kk_vector_t v, kk_unit_t wild__, kk_context_t* _ctx) { /* forall<a,e> (end.17122 : ssize_t, f : (a) -> e (), i : ssize_t, v : vector<a>, wild_ : ()) -> e () */ 
  kk_ssize_t i0_17462 = kk_std_core_incr_1(i, _ctx); /*ssize_t*/;
  kk_std_core__lift17287_foreach_3(end_17122, f, v, i0_17462, _ctx); return kk_Unit;
}
 
// lifted local: foreach.3, rep


// lift anonymous function
struct kk_std_core__lift17287_foreach_fun21313__t_3 {
  struct kk_function_s _base;
  kk_function_t f0;
  kk_vector_t v0;
  kk_ssize_t end_171220;
  kk_ssize_t i0;
};
static kk_box_t kk_std_core__lift17287_foreach_fun21313_3(kk_function_t _fself, kk_box_t _b_19257, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17287_foreach_fun21313_3(kk_function_t f0, kk_vector_t v0, kk_ssize_t end_171220, kk_ssize_t i0, kk_context_t* _ctx) {
  struct kk_std_core__lift17287_foreach_fun21313__t_3* _self = kk_function_alloc_as(struct kk_std_core__lift17287_foreach_fun21313__t_3, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift17287_foreach_fun21313_3, kk_context());
  _self->f0 = f0;
  _self->v0 = v0;
  _self->end_171220 = end_171220;
  _self->i0 = i0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift17287_foreach_fun21313_3(kk_function_t _fself, kk_box_t _b_19257, kk_context_t* _ctx) {
  struct kk_std_core__lift17287_foreach_fun21313__t_3* _self = kk_function_as(struct kk_std_core__lift17287_foreach_fun21313__t_3*, _fself);
  kk_function_t f0 = _self->f0; /* (11167) -> 11168 () */
  kk_vector_t v0 = _self->v0; /* vector<11167> */
  kk_ssize_t end_171220 = _self->end_171220; /* ssize_t */
  kk_ssize_t i0 = _self->i0; /* ssize_t */
  kk_drop_match(_self, {kk_function_dup(f0);kk_vector_dup(v0);;;}, {}, _ctx)
  kk_unit_t wild__0_19259 = kk_Unit;
  kk_unit_unbox(_b_19257);
  kk_unit_t _x21314 = kk_Unit;
  kk_std_core__mlift18026_op(end_171220, f0, i0, v0, wild__0_19259, _ctx);
  return kk_unit_box(_x21314);
}

kk_unit_t kk_std_core__lift17287_foreach_3(kk_ssize_t end_171220, kk_function_t f0, kk_vector_t v0, kk_ssize_t i0, kk_context_t* _ctx) { /* forall<a,e> (end.17122 : ssize_t, f : (a) -> e (), v : vector<a>, i : ssize_t) -> e () */ 
  kk__tailcall: ;
  bool _match_20145 = (i0 <= end_171220); /*bool*/;
  if (_match_20145) {
    kk_box_t x_17260 = kk_vector_at_borrow(v0,i0); /*11167*/;
    kk_unit_t x_18265 = kk_Unit;
    kk_function_t _x21311 = kk_function_dup(f0); /*(11167) -> 11168 ()*/
    kk_function_call(kk_unit_t, (kk_function_t, kk_box_t, kk_context_t*), _x21311, (_x21311, x_17260, _ctx));
    if (kk_yielding(kk_context())) {
      kk_box_t _x21312 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift17287_foreach_fun21313_3(f0, v0, end_171220, i0, _ctx), _ctx); /*3926*/
      kk_unit_unbox(_x21312); return kk_Unit;
    }
    {
      kk_ssize_t i0_174620 = kk_std_core_incr_1(i0, _ctx); /*ssize_t*/;
      { // tailcall
        i0 = i0_174620;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_vector_drop(v0, _ctx);
    kk_function_drop(f0, _ctx);
    kk_Unit; return kk_Unit;
  }
}
 
// O(n). Return the number of characters in a string.

kk_integer_t kk_std_core_count_1(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> int */ 
  return kk_string_count_int(s,kk_context());
}
 
// O(n). Return the number of characters in a string slice

kk_integer_t kk_std_core_count_2(kk_std_core__sslice slice0, kk_context_t* _ctx) { /* (slice : sslice) -> int */ 
  return kk_slice_count(slice0,kk_context());
}
 
// lifted local: count.3, .spec.17421
// specialized: std/core/foreach-while.1, on parameters action, using:
// action = fn<((local :: H -> X)<(h :: H)>)>(c: char){
//   val _ : ()
//         = match ((pred(c))) {
//           ((std/core/types/True() : bool ) as .pat: bool)
//              -> val x.17127 : int
//                       = std/core/types/local-get<int,(h :: H),((<>) :: E)>(cnt);
//             std/core/types/local-set<int,((<>) :: E),(h :: H)>(cnt, (std/core/int-add(x.17127, 1)));
//           ((.skip std/core/types/False() : bool ) as .pat0: bool)
//              -> std/core/types/();
//         };
//   std/core/types/Nothing<_a>;
// }

kk_std_core_types__maybe kk_std_core__lift17488_count_3(kk_ref_t cnt, kk_function_t pred, kk_std_core__sslice slice0, kk_context_t* _ctx) { /* forall<_a,h> (cnt : local-var<h,int>, pred : (char) -> bool, slice0 : sslice) -> (local<h>) maybe<_a> */ 
  kk__tailcall: ;
  kk_std_core_types__maybe _match_20143 = kk_std_core_next(slice0, _ctx); /*maybe<(char, sslice)>*/;
  if (kk_std_core_types__is_Nothing(_match_20143)) {
    kk_function_drop(pred, _ctx);
    kk_ref_drop(cnt, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  {
    kk_box_t _box_x19260 = _match_20143._cons.Just.value;
    kk_std_core_types__tuple2_ _pat10 = kk_std_core_types__tuple2__unbox(_box_x19260, NULL);
    kk_box_t _box_x19261 = _pat10.fst;
    kk_box_t _box_x19262 = _pat10.snd;
    kk_char_t c0 = kk_char_unbox(_box_x19261, NULL);
    kk_std_core__sslice rest = kk_std_core__sslice_unbox(_box_x19262, NULL);
    kk_std_core__sslice_dup(rest);
    kk_std_core_types__maybe_drop(_match_20143, _ctx);
    kk_unit_t __ = kk_Unit;
    bool _match_20144;
    kk_function_t _x21319 = kk_function_dup(pred); /*(char) -> bool*/
    _match_20144 = kk_function_call(bool, (kk_function_t, kk_char_t, kk_context_t*), _x21319, (_x21319, c0, _ctx)); /*bool*/
    if (_match_20144) {
      kk_integer_t x_17127;
      kk_box_t _x21320;
      kk_ref_t _x21321 = kk_ref_dup(cnt); /*local-var<11361,int>*/
      _x21320 = kk_ref_get(_x21321,kk_context()); /*1000*/
      x_17127 = kk_integer_unbox(_x21320); /*int*/
      kk_integer_t _b_19268_19266 = kk_integer_add_small_const(x_17127, 1, _ctx); /*int*/;
      kk_ref_set_borrow(cnt,(kk_integer_box(_b_19268_19266)),kk_context());
    }
    else {
      
    }
    { // tailcall
      slice0 = rest;
      goto kk__tailcall;
    }
  }
}
 
// Count the number of times a predicate is true for each character in a string

kk_integer_t kk_std_core_count_3(kk_string_t s, kk_function_t pred, kk_context_t* _ctx) { /* (s : string, pred : (char) -> bool) -> int */ 
  kk_ref_t loc = kk_ref_alloc((kk_integer_box(kk_integer_from_small(0))),kk_context()); /*local-var<11361,int>*/;
  kk_std_core__sslice slice0_17129;
  kk_string_t _x21322 = kk_string_dup(s); /*string*/
  kk_ssize_t _x21323 = (KK_IZ(0)); /*ssize_t*/
  kk_ssize_t _x21324 = kk_string_len(s,kk_context()); /*ssize_t*/
  slice0_17129 = kk_std_core__new_Sslice(_x21322, _x21323, _x21324, _ctx); /*sslice*/
  kk_std_core_types__maybe __0;
  kk_ref_t _x21325 = kk_ref_dup(loc); /*local-var<11361,int>*/
  __0 = kk_std_core__lift17488_count_3(_x21325, pred, slice0_17129, _ctx); /*maybe<_11100>*/
  kk_std_core_types__maybe_drop(__0, _ctx);
  kk_integer_t res;
  kk_box_t _x21326;
  kk_ref_t _x21327 = kk_ref_dup(loc); /*local-var<11361,int>*/
  _x21326 = kk_ref_get(_x21327,kk_context()); /*1000*/
  res = kk_integer_unbox(_x21326); /*int*/
  kk_box_t _x21328 = kk_std_core_hnd_prompt_local_var(loc, kk_integer_box(res), _ctx); /*10459*/
  return kk_integer_unbox(_x21328);
}
 
// Is the integer negative (stricly smaller than zero)

bool kk_std_core_is_neg_2(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> bool */ 
  kk_std_core_types__order x_17134;
  kk_std_core_types__order _brw_20142 = kk_int_as_order(kk_integer_signum_borrow(i),kk_context()); /*order*/;
  kk_integer_drop(i, _ctx);
  x_17134 = _brw_20142; /*order*/
  kk_integer_t _brw_20140;
  if (kk_std_core_types__is_Lt(x_17134)) {
    _brw_20140 = kk_integer_from_small(-1); /*int*/
    goto _match21332;
  }
  if (kk_std_core_types__is_Eq(x_17134)) {
    _brw_20140 = kk_integer_from_small(0); /*int*/
    goto _match21332;
  }
  {
    _brw_20140 = kk_integer_from_small(1); /*int*/
  }
  _match21332: ;
  bool _brw_20141 = kk_integer_eq_borrow(_brw_20140,(kk_integer_from_small(-1)),kk_context()); /*bool*/;
  kk_integer_drop(_brw_20140, _ctx);
  return _brw_20141;
}

kk_std_core_types__tuple2_ kk_std_core_divmod_exp10(kk_integer_t i, kk_integer_t n, kk_context_t* _ctx) { /* (i : int, n : int) -> (int, int) */ 
  kk_std_core_types__tuple2_ _match_20137;
  kk_integer_t _x21333 = kk_integer_dup(n); /*int*/
  _match_20137 = kk_std_core_cdivmod_exp10(i, _x21333, _ctx); /*(int, int)*/
  {
    kk_box_t _box_x19282 = _match_20137.fst;
    kk_box_t _box_x19283 = _match_20137.snd;
    kk_integer_t cq = kk_integer_unbox(_box_x19282);
    kk_integer_t cr = kk_integer_unbox(_box_x19283);
    kk_integer_dup(cq);
    kk_integer_dup(cr);
    kk_std_core_types__tuple2__drop(_match_20137, _ctx);
    kk_std_core_types__order x_17134 = kk_int_as_order(kk_integer_signum_borrow(cr),kk_context()); /*order*/;
    bool b_17138;
    kk_integer_t _brw_20138;
    if (kk_std_core_types__is_Lt(x_17134)) {
      _brw_20138 = kk_integer_from_small(-1); /*int*/
      goto _match21336;
    }
    if (kk_std_core_types__is_Eq(x_17134)) {
      _brw_20138 = kk_integer_from_small(0); /*int*/
      goto _match21336;
    }
    {
      _brw_20138 = kk_integer_from_small(1); /*int*/
    }
    _match21336: ;
    bool _brw_20139 = kk_integer_eq_borrow(_brw_20138,(kk_integer_from_small(-1)),kk_context()); /*bool*/;
    kk_integer_drop(_brw_20138, _ctx);
    b_17138 = _brw_20139; /*bool*/
    if (b_17138) {
      kk_integer_t y0_17143 = kk_std_core_mul_exp10(kk_integer_from_small(1), n, _ctx); /*int*/;
      kk_integer_t _b_19288_19284 = kk_integer_add_small_const(cq, -1, _ctx); /*int*/;
      kk_integer_t _b_19289_19285 = kk_integer_add(cr,y0_17143,kk_context()); /*int*/;
      return kk_std_core_types__new_dash__lp__comma__rp_(kk_integer_box(_b_19288_19284), kk_integer_box(_b_19289_19285), _ctx);
    }
    {
      kk_integer_drop(n, _ctx);
      return kk_std_core_types__new_dash__lp__comma__rp_(kk_integer_box(cq), kk_integer_box(cr), _ctx);
    }
  }
}
 
// Drop the first `n` elements of a list (or fewer if the list is shorter than `n`)

kk_std_core__list kk_std_core_drop(kk_std_core__list xs, kk_integer_t n, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, n : int) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21337 = kk_std_core__as_Cons(xs);
    kk_box_t _pat0 = _con21337->head;
    kk_std_core__list xx = _con21337->tail;
    if (kk_integer_gt_borrow(n,(kk_integer_from_small(0)),kk_context())) {
      if (kk_likely(kk_std_core__list_is_unique(xs))) {
        kk_box_drop(_pat0, _ctx);
        kk_std_core__list_free(xs, _ctx);
      }
      else {
        kk_std_core__list_dup(xx);
        kk_std_core__list_decref(xs, _ctx);
      }
      { // tailcall
        kk_integer_t _x21338 = kk_integer_add_small_const(n, -1, _ctx); /*int*/
        xs = xx;
        n = _x21338;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_integer_drop(n, _ctx);
    return xs;
  }
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift18027_drop_while(kk_function_t predicate, kk_std_core__list xs, kk_std_core__list xx, bool _y_17762, kk_context_t* _ctx) { /* forall<a,e> (predicate : (a) -> e bool, xs : list<a>, xx : list<a>, bool) -> e list<a> */ 
  if (_y_17762) {
    kk_std_core__list_drop(xs, _ctx);
    return kk_std_core_drop_while(xx, predicate, _ctx);
  }
  {
    kk_std_core__list_drop(xx, _ctx);
    kk_function_drop(predicate, _ctx);
    return xs;
  }
}
 
// Drop all initial elements that satisfy `predicate`


// lift anonymous function
struct kk_std_core_drop_while_fun21342__t {
  struct kk_function_s _base;
  kk_function_t predicate0;
  kk_std_core__list xs0;
  kk_std_core__list xx0;
};
static kk_box_t kk_std_core_drop_while_fun21342(kk_function_t _fself, kk_box_t _b_19293, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_drop_while_fun21342(kk_function_t predicate0, kk_std_core__list xs0, kk_std_core__list xx0, kk_context_t* _ctx) {
  struct kk_std_core_drop_while_fun21342__t* _self = kk_function_alloc_as(struct kk_std_core_drop_while_fun21342__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_drop_while_fun21342, kk_context());
  _self->predicate0 = predicate0;
  _self->xs0 = xs0;
  _self->xx0 = xx0;
  return &_self->_base;
}

static kk_box_t kk_std_core_drop_while_fun21342(kk_function_t _fself, kk_box_t _b_19293, kk_context_t* _ctx) {
  struct kk_std_core_drop_while_fun21342__t* _self = kk_function_as(struct kk_std_core_drop_while_fun21342__t*, _fself);
  kk_function_t predicate0 = _self->predicate0; /* (11629) -> 11630 bool */
  kk_std_core__list xs0 = _self->xs0; /* list<11629> */
  kk_std_core__list xx0 = _self->xx0; /* list<11629> */
  kk_drop_match(_self, {kk_function_dup(predicate0);kk_std_core__list_dup(xs0);kk_std_core__list_dup(xx0);}, {}, _ctx)
  bool _y_19295_177620 = kk_bool_unbox(_b_19293); /*bool*/;
  kk_std_core__list _x21343 = kk_std_core__mlift18027_drop_while(predicate0, xs0, xx0, _y_19295_177620, _ctx); /*list<11629>*/
  return kk_std_core__list_box(_x21343, _ctx);
}

kk_std_core__list kk_std_core_drop_while(kk_std_core__list xs0, kk_function_t predicate0, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, predicate : (a) -> e bool) -> e list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con21339 = kk_std_core__as_Cons(xs0);
    kk_box_t x = _con21339->head;
    kk_std_core__list xx0 = _con21339->tail;
    kk_box_dup(x);
    kk_std_core__list_dup(xx0);
    bool x0_18270;
    kk_function_t _x21340 = kk_function_dup(predicate0); /*(11629) -> 11630 bool*/
    x0_18270 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x21340, (_x21340, x, _ctx)); /*bool*/
    if (kk_yielding(kk_context())) {
      kk_box_t _x21341 = kk_std_core_hnd_yield_extend(kk_std_core_new_drop_while_fun21342(predicate0, xs0, xx0, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x21341, _ctx);
    }
    if (x0_18270) {
      if (kk_likely(kk_std_core__list_is_unique(xs0))) {
        kk_std_core__list_drop(xx0, _ctx);
        kk_box_drop(x, _ctx);
        kk_std_core__list_free(xs0, _ctx);
      }
      else {
        kk_std_core__list_decref(xs0, _ctx);
      }
      { // tailcall
        xs0 = xx0;
        goto kk__tailcall;
      }
    }
    {
      kk_std_core__list_drop(xx0, _ctx);
      kk_function_drop(predicate0, _ctx);
      return xs0;
    }
  }
  {
    kk_function_drop(predicate0, _ctx);
    return kk_std_core__new_Nil(_ctx);
  }
}
 
// An empty slice

kk_std_core__sslice kk_std_core_empty;
 
// Does string `s`  end with `post`?
// If so, returns a slice of `s` from the start up to the `post` string at the end.

kk_std_core_types__maybe kk_std_core_ends_with(kk_string_t s, kk_string_t post, kk_context_t* _ctx) { /* (s : string, post : string) -> maybe<sslice> */ 
  bool _match_20135;
  kk_string_t _x21348 = kk_string_dup(s); /*string*/
  kk_string_t _x21349 = kk_string_dup(post); /*string*/
  _match_20135 = kk_std_core_xends_with(_x21348, _x21349, _ctx); /*bool*/
  if (_match_20135) {
    kk_std_core__sslice _b_19297_19296;
    kk_string_t _x21350 = kk_string_dup(s); /*string*/
    kk_ssize_t _x21351 = (KK_IZ(0)); /*ssize_t*/
    kk_ssize_t _x21352;
    kk_ssize_t _x21353 = kk_string_len(s,kk_context()); /*ssize_t*/
    kk_ssize_t _x21354 = kk_string_len(post,kk_context()); /*ssize_t*/
    _x21352 = (_x21353 - _x21354); /*ssize_t*/
    _b_19297_19296 = kk_std_core__new_Sslice(_x21350, _x21351, _x21352, _ctx); /*sslice*/
    return kk_std_core_types__new_Just(kk_std_core__sslice_box(_b_19297_19296, _ctx), _ctx);
  }
  {
    kk_string_drop(s, _ctx);
    kk_string_drop(post, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
}
 
// Throw an exception with a specified message.

kk_box_t kk_std_core_throw(kk_string_t message0, kk_std_core_types__optional info0, kk_context_t* _ctx) { /* forall<a> (message : string, info : optional<exception-info>) -> exn a */ 
  kk_std_core_hnd__ev ev_18273;
  kk_ssize_t _x21355 = (KK_IZ(0)); /*ssize_t*/
  ev_18273 = kk_evv_at(_x21355,kk_context()); /*std/core/hnd/ev<.hnd-exn>*/
  {
    struct kk_std_core_hnd_Ev* _con21356 = kk_std_core_hnd__as_Ev(ev_18273);
    kk_std_core_hnd__marker m0 = _con21356->marker;
    kk_box_t _box_x19298 = _con21356->hnd;
    kk_std_core__hnd_exn h = kk_std_core__hnd_exn_unbox(_box_x19298, NULL);
    kk_std_core__hnd_exn_dup(h);
    kk_std_core_hnd__clause1 _match_20133;
    kk_std_core_hnd__clause1 _brw_20134 = kk_std_core__select_throw_exn(h, _ctx); /*std/core/hnd/clause1<exception,1938,.hnd-exn,1939,1940>*/;
    kk_std_core__hnd_exn_drop(h, _ctx);
    _match_20133 = _brw_20134; /*std/core/hnd/clause1<exception,1938,.hnd-exn,1939,1940>*/
    {
      kk_function_t _fun_unbox_x19302 = _match_20133.clause;
      kk_box_t _x21358;
      kk_std_core__exception _x21359;
      kk_std_core__exception_info _x21360;
      if (kk_std_core_types__is_Optional(info0)) {
        kk_box_t _box_x19306 = info0._cons.Optional.value;
        kk_std_core__exception_info _info_11715 = kk_std_core__exception_info_unbox(_box_x19306, NULL);
        _x21360 = _info_11715; /*exception-info*/
      }
      else {
        _x21360 = kk_std_core__new_ExnError(_ctx); /*exception-info*/
      }
      _x21359 = kk_std_core__new_Exception(message0, _x21360, _ctx); /*exception*/
      _x21358 = kk_std_core__exception_box(_x21359, _ctx); /*51*/
      return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x19302, (_fun_unbox_x19302, m0, ev_18273, _x21358, _ctx));
    }
  }
}
 
// monadic lift

kk_box_t kk_std_core__mlift18028_error_pattern(kk_string_t definition, kk_string_t location, kk_string_t _c_17767, kk_context_t* _ctx) { /* forall<a> (definition : string, location : string, string) -> a */ 
  kk_string_t message0_17148;
  kk_string_t _x21362 = kk_string_dup(location); /*string*/
  kk_string_t _x21363;
  kk_string_t _x21364;
  kk_define_string_literal(, _s21365, 23, ": pattern match failure")
  _x21364 = kk_string_dup(_s21365); /*string*/
  _x21363 = kk_std_core__lp__plus__plus__1_rp_(_c_17767, _x21364, _ctx); /*string*/
  message0_17148 = kk_std_core__lp__plus__plus__1_rp_(_x21362, _x21363, _ctx); /*string*/
  kk_std_core_hnd__ev ev_18276;
  kk_ssize_t _x21366 = (KK_IZ(0)); /*ssize_t*/
  ev_18276 = kk_evv_at(_x21366,kk_context()); /*std/core/hnd/ev<.hnd-exn>*/
  {
    struct kk_std_core_hnd_Ev* _con21367 = kk_std_core_hnd__as_Ev(ev_18276);
    kk_std_core_hnd__marker m0 = _con21367->marker;
    kk_box_t _box_x19307 = _con21367->hnd;
    kk_std_core__hnd_exn h = kk_std_core__hnd_exn_unbox(_box_x19307, NULL);
    kk_std_core__hnd_exn_dup(h);
    kk_std_core_hnd__clause1 _match_20131;
    kk_std_core_hnd__clause1 _brw_20132 = kk_std_core__select_throw_exn(h, _ctx); /*std/core/hnd/clause1<exception,1938,.hnd-exn,1939,1940>*/;
    kk_std_core__hnd_exn_drop(h, _ctx);
    _match_20131 = _brw_20132; /*std/core/hnd/clause1<exception,1938,.hnd-exn,1939,1940>*/
    {
      kk_function_t _fun_unbox_x19311 = _match_20131.clause;
      kk_box_t _x21369;
      kk_std_core__exception _x21370;
      kk_std_core__exception_info _x21371 = kk_std_core__new_ExnPattern(kk_reuse_null, location, definition, _ctx); /*exception-info*/
      _x21370 = kk_std_core__new_Exception(message0_17148, _x21371, _ctx); /*exception*/
      _x21369 = kk_std_core__exception_box(_x21370, _ctx); /*51*/
      return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x19311, (_fun_unbox_x19311, m0, ev_18276, _x21369, _ctx));
    }
  }
}
 
// Raise a pattern match exception. This is function is used internally by the
// compiler to generate error messages on pattern match failures.


// lift anonymous function
struct kk_std_core_error_pattern_fun21379__t {
  struct kk_function_s _base;
  kk_string_t definition;
  kk_string_t location;
};
static kk_box_t kk_std_core_error_pattern_fun21379(kk_function_t _fself, kk_box_t _b_19316, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_error_pattern_fun21379(kk_string_t definition, kk_string_t location, kk_context_t* _ctx) {
  struct kk_std_core_error_pattern_fun21379__t* _self = kk_function_alloc_as(struct kk_std_core_error_pattern_fun21379__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_error_pattern_fun21379, kk_context());
  _self->definition = definition;
  _self->location = location;
  return &_self->_base;
}

static kk_box_t kk_std_core_error_pattern_fun21379(kk_function_t _fself, kk_box_t _b_19316, kk_context_t* _ctx) {
  struct kk_std_core_error_pattern_fun21379__t* _self = kk_function_as(struct kk_std_core_error_pattern_fun21379__t*, _fself);
  kk_string_t definition = _self->definition; /* string */
  kk_string_t location = _self->location; /* string */
  kk_drop_match(_self, {kk_string_dup(definition);kk_string_dup(location);}, {}, _ctx)
  kk_string_t _x21380 = kk_string_unbox(_b_19316); /*string*/
  return kk_std_core__mlift18028_error_pattern(definition, location, _x21380, _ctx);
}

kk_box_t kk_std_core_error_pattern(kk_string_t location, kk_string_t definition, kk_context_t* _ctx) { /* forall<a> (location : string, definition : string) -> exn a */ 
  kk_string_t x_18279;
  bool _match_20130;
  kk_string_t _x21372 = kk_string_dup(definition); /*string*/
  kk_string_t _x21373 = kk_string_empty(); /*string*/
  _match_20130 = kk_string_is_eq(_x21372,_x21373,kk_context()); /*bool*/
  if (_match_20130) {
    x_18279 = kk_string_empty(); /*string*/
  }
  else {
    kk_string_t _x21376;
    kk_define_string_literal(, _s21377, 2, ": ")
    _x21376 = kk_string_dup(_s21377); /*string*/
    kk_string_t _x21378 = kk_string_dup(definition); /*string*/
    x_18279 = kk_std_core__lp__plus__plus__1_rp_(_x21376, _x21378, _ctx); /*string*/
  }
  if (kk_yielding(kk_context())) {
    kk_string_drop(x_18279, _ctx);
    return kk_std_core_hnd_yield_extend(kk_std_core_new_error_pattern_fun21379(definition, location, _ctx), _ctx);
  }
  {
    kk_string_t message0_17148;
    kk_string_t _x21381 = kk_string_dup(location); /*string*/
    kk_string_t _x21382;
    kk_string_t _x21383;
    kk_define_string_literal(, _s21384, 23, ": pattern match failure")
    _x21383 = kk_string_dup(_s21384); /*string*/
    _x21382 = kk_std_core__lp__plus__plus__1_rp_(x_18279, _x21383, _ctx); /*string*/
    message0_17148 = kk_std_core__lp__plus__plus__1_rp_(_x21381, _x21382, _ctx); /*string*/
    kk_std_core_hnd__ev ev_18282;
    kk_ssize_t _x21385 = (KK_IZ(0)); /*ssize_t*/
    ev_18282 = kk_evv_at(_x21385,kk_context()); /*std/core/hnd/ev<.hnd-exn>*/
    {
      struct kk_std_core_hnd_Ev* _con21386 = kk_std_core_hnd__as_Ev(ev_18282);
      kk_std_core_hnd__marker m0 = _con21386->marker;
      kk_box_t _box_x19317 = _con21386->hnd;
      kk_std_core__hnd_exn h = kk_std_core__hnd_exn_unbox(_box_x19317, NULL);
      kk_std_core__hnd_exn_dup(h);
      kk_std_core_hnd__clause1 _match_20128;
      kk_std_core_hnd__clause1 _brw_20129 = kk_std_core__select_throw_exn(h, _ctx); /*std/core/hnd/clause1<exception,1938,.hnd-exn,1939,1940>*/;
      kk_std_core__hnd_exn_drop(h, _ctx);
      _match_20128 = _brw_20129; /*std/core/hnd/clause1<exception,1938,.hnd-exn,1939,1940>*/
      {
        kk_function_t _fun_unbox_x19321 = _match_20128.clause;
        kk_box_t _x21388;
        kk_std_core__exception _x21389;
        kk_std_core__exception_info _x21390 = kk_std_core__new_ExnPattern(kk_reuse_null, location, definition, _ctx); /*exception-info*/
        _x21389 = kk_std_core__new_Exception(message0_17148, _x21390, _ctx); /*exception*/
        _x21388 = kk_std_core__exception_box(_x21389, _ctx); /*51*/
        return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x19321, (_fun_unbox_x19321, m0, ev_18282, _x21388, _ctx));
      }
    }
  }
}
 
// Transform an `:error` type back to an `exn` effect.

kk_box_t kk_std_core_untry(kk_std_core__error err, kk_context_t* _ctx) { /* forall<a> (err : error<a>) -> exn a */ 
  if (kk_std_core__is_Error(err)) {
    kk_std_core__exception exn0 = err._cons.Error.exception;
    kk_std_core__exception_dup(exn0);
    kk_std_core__error_drop(err, _ctx);
    kk_std_core_hnd__ev ev_18285;
    kk_ssize_t _x21391 = (KK_IZ(0)); /*ssize_t*/
    ev_18285 = kk_evv_at(_x21391,kk_context()); /*std/core/hnd/ev<.hnd-exn>*/
    {
      struct kk_std_core_hnd_Ev* _con21392 = kk_std_core_hnd__as_Ev(ev_18285);
      kk_std_core_hnd__marker m0 = _con21392->marker;
      kk_box_t _box_x19327 = _con21392->hnd;
      kk_std_core__hnd_exn h = kk_std_core__hnd_exn_unbox(_box_x19327, NULL);
      kk_std_core__hnd_exn_dup(h);
      kk_std_core_hnd__clause1 _match_20125;
      kk_std_core_hnd__clause1 _brw_20126 = kk_std_core__select_throw_exn(h, _ctx); /*std/core/hnd/clause1<exception,1938,.hnd-exn,1939,1940>*/;
      kk_std_core__hnd_exn_drop(h, _ctx);
      _match_20125 = _brw_20126; /*std/core/hnd/clause1<exception,1938,.hnd-exn,1939,1940>*/
      {
        kk_function_t _fun_unbox_x19331 = _match_20125.clause;
        return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x19331, (_fun_unbox_x19331, m0, ev_18285, kk_std_core__exception_box(exn0, _ctx), _ctx));
      }
    }
  }
  {
    kk_box_t x0 = err._cons.Ok.result;
    return x0;
  }
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift18029_op(kk_std_core_types__ctail _acc, kk_function_t pred, kk_box_t x, kk_std_core__list xx, bool _y_17772, kk_context_t* _ctx) { /* forall<a,e> (ctail<list<a>>, pred : (a) -> e bool, x : a, xx : list<a>, bool) -> e list<a> */ 
  if (_y_17772) {
    kk_std_core__list _ctail_17567 = kk_std_core__list_hole(); /*list<11895>*/;
    kk_std_core__list _ctail_17568 = kk_std_core__new_Cons(kk_reuse_null, x, _ctail_17567, _ctx); /*list<11895>*/;
    kk_box_t* _b_19343_19340 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17568)->tail)); /*cfield<list<11895>>*/;
    kk_std_core_types__ctail _x21394 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17568, _ctx)),_b_19343_19340); /*ctail<0>*/
    return kk_std_core__ctail_filter(xx, pred, _x21394, _ctx);
  }
  {
    kk_box_drop(x, _ctx);
    return kk_std_core__ctail_filter(xx, pred, _acc, _ctx);
  }
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift18030_op_fun21395__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t x0;
};
static kk_std_core__list kk_std_core__mlift18030_op_fun21395(kk_function_t _fself, kk_std_core__list _ctail_17570, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift18030_op_fun21395(kk_function_t _accm, kk_box_t x0, kk_context_t* _ctx) {
  struct kk_std_core__mlift18030_op_fun21395__t* _self = kk_function_alloc_as(struct kk_std_core__mlift18030_op_fun21395__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift18030_op_fun21395, kk_context());
  _self->_accm = _accm;
  _self->x0 = x0;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift18030_op_fun21395(kk_function_t _fself, kk_std_core__list _ctail_17570, kk_context_t* _ctx) {
  struct kk_std_core__mlift18030_op_fun21395__t* _self = kk_function_as(struct kk_std_core__mlift18030_op_fun21395__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<11895>) -> list<11895> */
  kk_box_t x0 = _self->x0; /* 11895 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(x0);}, {}, _ctx)
  kk_std_core__list _x21396 = kk_std_core__new_Cons(kk_reuse_null, x0, _ctail_17570, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x21396, _ctx));
}

kk_std_core__list kk_std_core__mlift18030_op(kk_function_t _accm, kk_function_t pred0, kk_box_t x0, kk_std_core__list xx0, bool _y_17777, kk_context_t* _ctx) { /* forall<a,e> ((list<a>) -> list<a>, pred : (a) -> e bool, x : a, xx : list<a>, bool) -> e list<a> */ 
  if (_y_17777) {
    return kk_std_core__ctailm_filter(xx0, pred0, kk_std_core__new_mlift18030_op_fun21395(_accm, x0, _ctx), _ctx);
  }
  {
    kk_box_drop(x0, _ctx);
    return kk_std_core__ctailm_filter(xx0, pred0, _accm, _ctx);
  }
}
 
// Retain only those elements of a list that satisfy the given predicate `pred`.
// For example: `filter([1,2,3],odd?) == [1,3]`


// lift anonymous function
struct kk_std_core__ctail_filter_fun21401__t {
  struct kk_function_s _base;
  kk_function_t pred1;
  kk_box_t x1;
  kk_std_core__list xx1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_filter_fun21401(kk_function_t _fself, kk_box_t _b_19348, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_filter_fun21401(kk_function_t pred1, kk_box_t x1, kk_std_core__list xx1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_filter_fun21401__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_filter_fun21401__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_filter_fun21401, kk_context());
  _self->pred1 = pred1;
  _self->x1 = x1;
  _self->xx1 = xx1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_filter_fun21401(kk_function_t _fself, kk_box_t _b_19348, kk_context_t* _ctx) {
  struct kk_std_core__ctail_filter_fun21401__t* _self = kk_function_as(struct kk_std_core__ctail_filter_fun21401__t*, _fself);
  kk_function_t pred1 = _self->pred1; /* (11895) -> 11896 bool */
  kk_box_t x1 = _self->x1; /* 11895 */
  kk_std_core__list xx1 = _self->xx1; /* list<11895> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<11895>> */
  kk_drop_match(_self, {kk_function_dup(pred1);kk_box_dup(x1);kk_std_core__list_dup(xx1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  bool _y_19366_177720 = kk_bool_unbox(_b_19348); /*bool*/;
  kk_std_core__list _x21402 = kk_std_core__mlift18029_op(_acc0, pred1, x1, xx1, _y_19366_177720, _ctx); /*list<11895>*/
  return kk_std_core__list_box(_x21402, _ctx);
}

kk_std_core__list kk_std_core__ctail_filter(kk_std_core__list xs, kk_function_t pred1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, pred : (a) -> e bool, ctail<list<a>>) -> e list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21397 = kk_std_core__as_Cons(xs);
    kk_box_t x1 = _con21397->head;
    kk_std_core__list xx1 = _con21397->tail;
    kk_reuse_t _ru_20395 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      _ru_20395 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_box_dup(x1);
      kk_std_core__list_dup(xx1);
      kk_std_core__list_decref(xs, _ctx);
    }
    bool x2_18288;
    kk_function_t _x21399 = kk_function_dup(pred1); /*(11895) -> 11896 bool*/
    kk_box_t _x21398 = kk_box_dup(x1); /*11895*/
    x2_18288 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x21399, (_x21399, _x21398, _ctx)); /*bool*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_20395,kk_context());
      kk_box_t _x21400 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_filter_fun21401(pred1, x1, xx1, _acc0, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x21400, _ctx);
    }
    if (x2_18288) {
      kk_std_core__list _ctail_175670 = kk_std_core__list_hole(); /*list<11895>*/;
      kk_std_core__list _ctail_175680 = kk_std_core__new_Cons(_ru_20395, x1, _ctail_175670, _ctx); /*list<11895>*/;
      kk_box_t* _b_19360_19354 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_175680)->tail)); /*cfield<list<11895>>*/;
      { // tailcall
        kk_std_core_types__ctail _x21403 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_175680, _ctx)),_b_19360_19354); /*ctail<0>*/
        xs = xx1;
        _acc0 = _x21403;
        goto kk__tailcall;
      }
    }
    {
      kk_reuse_drop(_ru_20395,kk_context());
      kk_box_drop(x1, _ctx);
      { // tailcall
        xs = xx1;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_function_drop(pred1, _ctx);
    kk_box_t _x21404 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21404, _ctx);
  }
}
 
// Retain only those elements of a list that satisfy the given predicate `pred`.
// For example: `filter([1,2,3],odd?) == [1,3]`


// lift anonymous function
struct kk_std_core__ctailm_filter_fun21409__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t pred2;
  kk_box_t x3;
  kk_std_core__list xx2;
};
static kk_box_t kk_std_core__ctailm_filter_fun21409(kk_function_t _fself, kk_box_t _b_19368, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_filter_fun21409(kk_function_t _accm0, kk_function_t pred2, kk_box_t x3, kk_std_core__list xx2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_filter_fun21409__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_filter_fun21409__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_filter_fun21409, kk_context());
  _self->_accm0 = _accm0;
  _self->pred2 = pred2;
  _self->x3 = x3;
  _self->xx2 = xx2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_filter_fun21409(kk_function_t _fself, kk_box_t _b_19368, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_filter_fun21409__t* _self = kk_function_as(struct kk_std_core__ctailm_filter_fun21409__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<11895>) -> list<11895> */
  kk_function_t pred2 = _self->pred2; /* (11895) -> 11896 bool */
  kk_box_t x3 = _self->x3; /* 11895 */
  kk_std_core__list xx2 = _self->xx2; /* list<11895> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(pred2);kk_box_dup(x3);kk_std_core__list_dup(xx2);}, {}, _ctx)
  bool _y_19370_177770 = kk_bool_unbox(_b_19368); /*bool*/;
  kk_std_core__list _x21410 = kk_std_core__mlift18030_op(_accm0, pred2, x3, xx2, _y_19370_177770, _ctx); /*list<11895>*/
  return kk_std_core__list_box(_x21410, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_filter_fun21412__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t x3;
};
static kk_std_core__list kk_std_core__ctailm_filter_fun21412(kk_function_t _fself, kk_std_core__list _ctail_175700, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_filter_fun21412(kk_function_t _accm0, kk_box_t x3, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_filter_fun21412__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_filter_fun21412__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_filter_fun21412, kk_context());
  _self->_accm0 = _accm0;
  _self->x3 = x3;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_filter_fun21412(kk_function_t _fself, kk_std_core__list _ctail_175700, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_filter_fun21412__t* _self = kk_function_as(struct kk_std_core__ctailm_filter_fun21412__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<11895>) -> list<11895> */
  kk_box_t x3 = _self->x3; /* 11895 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(x3);}, {}, _ctx)
  kk_std_core__list _x21413 = kk_std_core__new_Cons(kk_reuse_null, x3, _ctail_175700, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x21413, _ctx));
}

kk_std_core__list kk_std_core__ctailm_filter(kk_std_core__list xs0, kk_function_t pred2, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, pred : (a) -> e bool, (list<a>) -> list<a>) -> e list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con21405 = kk_std_core__as_Cons(xs0);
    kk_box_t x3 = _con21405->head;
    kk_std_core__list xx2 = _con21405->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs0))) {
      kk_std_core__list_free(xs0, _ctx);
    }
    else {
      kk_box_dup(x3);
      kk_std_core__list_dup(xx2);
      kk_std_core__list_decref(xs0, _ctx);
    }
    bool x4_18291;
    kk_function_t _x21407 = kk_function_dup(pred2); /*(11895) -> 11896 bool*/
    kk_box_t _x21406 = kk_box_dup(x3); /*11895*/
    x4_18291 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x21407, (_x21407, _x21406, _ctx)); /*bool*/
    if (kk_yielding(kk_context())) {
      kk_box_t _x21408 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_filter_fun21409(_accm0, pred2, x3, xx2, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x21408, _ctx);
    }
    if (x4_18291) { // tailcall
                    kk_function_t _x21411 = kk_std_core__new_ctailm_filter_fun21412(_accm0, x3, _ctx); /*(list<11895>) -> list<11895>*/
                    xs0 = xx2;
                    _accm0 = _x21411;
                    goto kk__tailcall;
    }
    {
      kk_box_drop(x3, _ctx);
      { // tailcall
        xs0 = xx2;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_function_drop(pred2, _ctx);
    return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
  }
}
 
// Retain only those elements of a list that satisfy the given predicate `pred`.
// For example: `filter([1,2,3],odd?) == [1,3]`


// lift anonymous function
struct kk_std_core_filter_fun21415__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core_filter_fun21415(kk_function_t _fself, kk_std_core__list _ctail_17569, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_filter_fun21415(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_filter_fun21415, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core_filter_fun21415(kk_function_t _fself, kk_std_core__list _ctail_17569, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17569;
}

kk_std_core__list kk_std_core_filter(kk_std_core__list xs1, kk_function_t pred3, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, pred : (a) -> e bool) -> e list<a> */ 
  bool _match_20122 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_20122) {
    kk_std_core_types__ctail _x21414 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_filter(xs1, pred3, _x21414, _ctx);
  }
  {
    return kk_std_core__ctailm_filter(xs1, pred3, kk_std_core_new_filter_fun21415(_ctx), _ctx);
  }
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift18031_op(kk_std_core_types__ctail _acc, kk_function_t pred, kk_std_core__list xx, kk_std_core_types__maybe _y_17785, kk_context_t* _ctx) { /* forall<a,b,e> (ctail<list<b>>, pred : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e list<b> */ 
  if (kk_std_core_types__is_Nothing(_y_17785)) {
    return kk_std_core__ctail_filter_map(xx, pred, _acc, _ctx);
  }
  {
    kk_box_t y = _y_17785._cons.Just.value;
    kk_std_core__list _ctail_17571 = kk_std_core__list_hole(); /*list<11941>*/;
    kk_std_core__list _ctail_17572 = kk_std_core__new_Cons(kk_reuse_null, y, _ctail_17571, _ctx); /*list<11941>*/;
    kk_box_t* _b_19379_19376 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17572)->tail)); /*cfield<list<11941>>*/;
    kk_std_core_types__ctail _x21416 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17572, _ctx)),_b_19379_19376); /*ctail<0>*/
    return kk_std_core__ctail_filter_map(xx, pred, _x21416, _ctx);
  }
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift18032_op_fun21417__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t y0;
};
static kk_std_core__list kk_std_core__mlift18032_op_fun21417(kk_function_t _fself, kk_std_core__list _ctail_17574, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift18032_op_fun21417(kk_function_t _accm, kk_box_t y0, kk_context_t* _ctx) {
  struct kk_std_core__mlift18032_op_fun21417__t* _self = kk_function_alloc_as(struct kk_std_core__mlift18032_op_fun21417__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift18032_op_fun21417, kk_context());
  _self->_accm = _accm;
  _self->y0 = y0;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift18032_op_fun21417(kk_function_t _fself, kk_std_core__list _ctail_17574, kk_context_t* _ctx) {
  struct kk_std_core__mlift18032_op_fun21417__t* _self = kk_function_as(struct kk_std_core__mlift18032_op_fun21417__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<11941>) -> list<11941> */
  kk_box_t y0 = _self->y0; /* 11941 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(y0);}, {}, _ctx)
  kk_std_core__list _x21418 = kk_std_core__new_Cons(kk_reuse_null, y0, _ctail_17574, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x21418, _ctx));
}

kk_std_core__list kk_std_core__mlift18032_op(kk_function_t _accm, kk_function_t pred0, kk_std_core__list xx0, kk_std_core_types__maybe _y_17790, kk_context_t* _ctx) { /* forall<a,b,e> ((list<b>) -> list<b>, pred : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e list<b> */ 
  if (kk_std_core_types__is_Nothing(_y_17790)) {
    return kk_std_core__ctailm_filter_map(xx0, pred0, _accm, _ctx);
  }
  {
    kk_box_t y0 = _y_17790._cons.Just.value;
    return kk_std_core__ctailm_filter_map(xx0, pred0, kk_std_core__new_mlift18032_op_fun21417(_accm, y0, _ctx), _ctx);
  }
}
 
// Retain only those elements of a list that satisfy the given predicate `pred`.
// For example: `filterMap([1,2,3],fn(i) { if i.odd? then Nothing else Just(i*i) }) == [4]`


// lift anonymous function
struct kk_std_core__ctail_filter_map_fun21423__t {
  struct kk_function_s _base;
  kk_function_t pred1;
  kk_std_core__list xx1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_filter_map_fun21423(kk_function_t _fself, kk_box_t _b_19386, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_filter_map_fun21423(kk_function_t pred1, kk_std_core__list xx1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_filter_map_fun21423__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_filter_map_fun21423__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_filter_map_fun21423, kk_context());
  _self->pred1 = pred1;
  _self->xx1 = xx1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_filter_map_fun21423(kk_function_t _fself, kk_box_t _b_19386, kk_context_t* _ctx) {
  struct kk_std_core__ctail_filter_map_fun21423__t* _self = kk_function_as(struct kk_std_core__ctail_filter_map_fun21423__t*, _fself);
  kk_function_t pred1 = _self->pred1; /* (11940) -> 11942 maybe<11941> */
  kk_std_core__list xx1 = _self->xx1; /* list<11940> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<11941>> */
  kk_drop_match(_self, {kk_function_dup(pred1);kk_std_core__list_dup(xx1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_std_core_types__maybe _y_19402_177850 = kk_std_core_types__maybe_unbox(_b_19386, _ctx); /*maybe<11941>*/;
  kk_std_core__list _x21424 = kk_std_core__mlift18031_op(_acc0, pred1, xx1, _y_19402_177850, _ctx); /*list<11941>*/
  return kk_std_core__list_box(_x21424, _ctx);
}

kk_std_core__list kk_std_core__ctail_filter_map(kk_std_core__list xs, kk_function_t pred1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, pred : (a) -> e maybe<b>, ctail<list<b>>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Nil(xs)) {
    kk_function_drop(pred1, _ctx);
    kk_box_t _x21419 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21419, _ctx);
  }
  {
    struct kk_std_core_Cons* _con21420 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con21420->head;
    kk_std_core__list xx1 = _con21420->tail;
    kk_reuse_t _ru_20397 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      _ru_20397 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx1);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_std_core_types__maybe x0_18294;
    kk_function_t _x21421 = kk_function_dup(pred1); /*(11940) -> 11942 maybe<11941>*/
    x0_18294 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_box_t, kk_context_t*), _x21421, (_x21421, x, _ctx)); /*maybe<11941>*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_20397,kk_context());
      kk_std_core_types__maybe_drop(x0_18294, _ctx);
      kk_box_t _x21422 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_filter_map_fun21423(pred1, xx1, _acc0, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x21422, _ctx);
    }
    if (kk_std_core_types__is_Nothing(x0_18294)) {
      kk_reuse_drop(_ru_20397,kk_context());
      { // tailcall
        xs = xx1;
        goto kk__tailcall;
      }
    }
    {
      kk_box_t y1 = x0_18294._cons.Just.value;
      kk_std_core__list _ctail_175710 = kk_std_core__list_hole(); /*list<11941>*/;
      kk_std_core__list _ctail_175720 = kk_std_core__new_Cons(_ru_20397, y1, _ctail_175710, _ctx); /*list<11941>*/;
      kk_box_t* _b_19398_19392 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_175720)->tail)); /*cfield<list<11941>>*/;
      { // tailcall
        kk_std_core_types__ctail _x21425 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_175720, _ctx)),_b_19398_19392); /*ctail<0>*/
        xs = xx1;
        _acc0 = _x21425;
        goto kk__tailcall;
      }
    }
  }
}
 
// Retain only those elements of a list that satisfy the given predicate `pred`.
// For example: `filterMap([1,2,3],fn(i) { if i.odd? then Nothing else Just(i*i) }) == [4]`


// lift anonymous function
struct kk_std_core__ctailm_filter_map_fun21429__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t pred2;
  kk_std_core__list xx2;
};
static kk_box_t kk_std_core__ctailm_filter_map_fun21429(kk_function_t _fself, kk_box_t _b_19404, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_filter_map_fun21429(kk_function_t _accm0, kk_function_t pred2, kk_std_core__list xx2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_filter_map_fun21429__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_filter_map_fun21429__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_filter_map_fun21429, kk_context());
  _self->_accm0 = _accm0;
  _self->pred2 = pred2;
  _self->xx2 = xx2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_filter_map_fun21429(kk_function_t _fself, kk_box_t _b_19404, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_filter_map_fun21429__t* _self = kk_function_as(struct kk_std_core__ctailm_filter_map_fun21429__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<11941>) -> list<11941> */
  kk_function_t pred2 = _self->pred2; /* (11940) -> 11942 maybe<11941> */
  kk_std_core__list xx2 = _self->xx2; /* list<11940> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(pred2);kk_std_core__list_dup(xx2);}, {}, _ctx)
  kk_std_core_types__maybe _y_19406_177900 = kk_std_core_types__maybe_unbox(_b_19404, _ctx); /*maybe<11941>*/;
  kk_std_core__list _x21430 = kk_std_core__mlift18032_op(_accm0, pred2, xx2, _y_19406_177900, _ctx); /*list<11941>*/
  return kk_std_core__list_box(_x21430, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_filter_map_fun21432__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t y2;
};
static kk_std_core__list kk_std_core__ctailm_filter_map_fun21432(kk_function_t _fself, kk_std_core__list _ctail_175740, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_filter_map_fun21432(kk_function_t _accm0, kk_box_t y2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_filter_map_fun21432__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_filter_map_fun21432__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_filter_map_fun21432, kk_context());
  _self->_accm0 = _accm0;
  _self->y2 = y2;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_filter_map_fun21432(kk_function_t _fself, kk_std_core__list _ctail_175740, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_filter_map_fun21432__t* _self = kk_function_as(struct kk_std_core__ctailm_filter_map_fun21432__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<11941>) -> list<11941> */
  kk_box_t y2 = _self->y2; /* 11941 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(y2);}, {}, _ctx)
  kk_std_core__list _x21433 = kk_std_core__new_Cons(kk_reuse_null, y2, _ctail_175740, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x21433, _ctx));
}

kk_std_core__list kk_std_core__ctailm_filter_map(kk_std_core__list xs0, kk_function_t pred2, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, pred : (a) -> e maybe<b>, (list<b>) -> list<b>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Nil(xs0)) {
    kk_function_drop(pred2, _ctx);
    return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
  }
  {
    struct kk_std_core_Cons* _con21426 = kk_std_core__as_Cons(xs0);
    kk_box_t x1 = _con21426->head;
    kk_std_core__list xx2 = _con21426->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs0))) {
      kk_std_core__list_free(xs0, _ctx);
    }
    else {
      kk_box_dup(x1);
      kk_std_core__list_dup(xx2);
      kk_std_core__list_decref(xs0, _ctx);
    }
    kk_std_core_types__maybe x2_18297;
    kk_function_t _x21427 = kk_function_dup(pred2); /*(11940) -> 11942 maybe<11941>*/
    x2_18297 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_box_t, kk_context_t*), _x21427, (_x21427, x1, _ctx)); /*maybe<11941>*/
    if (kk_yielding(kk_context())) {
      kk_std_core_types__maybe_drop(x2_18297, _ctx);
      kk_box_t _x21428 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_filter_map_fun21429(_accm0, pred2, xx2, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x21428, _ctx);
    }
    if (kk_std_core_types__is_Nothing(x2_18297)) { // tailcall
                                                   xs0 = xx2;
                                                   goto kk__tailcall;
    }
    {
      kk_box_t y2 = x2_18297._cons.Just.value;
      { // tailcall
        kk_function_t _x21431 = kk_std_core__new_ctailm_filter_map_fun21432(_accm0, y2, _ctx); /*(list<11941>) -> list<11941>*/
        xs0 = xx2;
        _accm0 = _x21431;
        goto kk__tailcall;
      }
    }
  }
}
 
// Retain only those elements of a list that satisfy the given predicate `pred`.
// For example: `filterMap([1,2,3],fn(i) { if i.odd? then Nothing else Just(i*i) }) == [4]`


// lift anonymous function
struct kk_std_core_filter_map_fun21435__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core_filter_map_fun21435(kk_function_t _fself, kk_std_core__list _ctail_17573, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_filter_map_fun21435(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_filter_map_fun21435, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core_filter_map_fun21435(kk_function_t _fself, kk_std_core__list _ctail_17573, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17573;
}

kk_std_core__list kk_std_core_filter_map(kk_std_core__list xs1, kk_function_t pred3, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, pred : (a) -> e maybe<b>) -> e list<b> */ 
  bool _match_20119 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_20119) {
    kk_std_core_types__ctail _x21434 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_filter_map(xs1, pred3, _x21434, _ctx);
  }
  {
    return kk_std_core__ctailm_filter_map(xs1, pred3, kk_std_core_new_filter_map_fun21435(_ctx), _ctx);
  }
}

bool kk_std_core_is_zero_1(kk_ssize_t i, kk_context_t* _ctx) { /* (i : ssize_t) -> bool */ 
  return (i == 0);
}
 
// lifted local: find, .spec.17423
// specialized: std/core/foreach-while, on parameters action, using:
// action = fn(x: 12039){
//   match ((pred(x))) {
//     ((std/core/types/True() : bool ) as .pat: bool)
//        -> std/core/types/Just<a>(x);
//     ((.skip std/core/types/False() : bool ) as .pat0: bool)
//        -> std/core/types/Nothing<a>;
//   };
// }

kk_std_core_types__maybe kk_std_core__lift17489_find(kk_function_t pred, kk_std_core__list xs0, kk_context_t* _ctx) { /* forall<a> (pred : (a) -> bool, xs0 : list<a>) -> maybe<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Nil(xs0)) {
    kk_function_drop(pred, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  {
    struct kk_std_core_Cons* _con21436 = kk_std_core__as_Cons(xs0);
    kk_box_t x0 = _con21436->head;
    kk_std_core__list xx = _con21436->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs0))) {
      kk_std_core__list_free(xs0, _ctx);
    }
    else {
      kk_box_dup(x0);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs0, _ctx);
    }
    bool _match_20118;
    kk_function_t _x21438 = kk_function_dup(pred); /*(12039) -> bool*/
    kk_box_t _x21437 = kk_box_dup(x0); /*12039*/
    _match_20118 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x21438, (_x21438, _x21437, _ctx)); /*bool*/
    if (_match_20118) {
      kk_std_core__list_drop(xx, _ctx);
      kk_function_drop(pred, _ctx);
      return kk_std_core_types__new_Just(x0, _ctx);
    }
    {
      kk_box_drop(x0, _ctx);
      { // tailcall
        xs0 = xx;
        goto kk__tailcall;
      }
    }
  }
}
 
// O(n). If it occurs, return the position of substring `sub` in `s`, tupled with
// the position just following the substring `sub`.

kk_std_core_types__maybe kk_std_core_find_1(kk_string_t s, kk_string_t sub, kk_context_t* _ctx) { /* (s : string, sub : string) -> maybe<sslice> */ 
  kk_ssize_t i;
  kk_string_t _x21439 = kk_string_dup(s); /*string*/
  kk_string_t _x21440 = kk_string_dup(sub); /*string*/
  i = kk_string_index_of1(_x21439,_x21440,kk_context()); /*ssize_t*/
  bool _match_20117 = kk_std_core_is_zero_1(i, _ctx); /*bool*/;
  if (_match_20117) {
    kk_string_drop(sub, _ctx);
    kk_string_drop(s, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  {
    kk_std_core__sslice _b_19408_19407;
    kk_ssize_t _x21441 = kk_std_core_decr_1(i, _ctx); /*ssize_t*/
    kk_ssize_t _x21442 = kk_string_len(sub,kk_context()); /*ssize_t*/
    _b_19408_19407 = kk_std_core__new_Sslice(s, _x21441, _x21442, _ctx); /*sslice*/
    return kk_std_core_types__new_Just(kk_std_core__sslice_box(_b_19408_19407, _ctx), _ctx);
  }
}
 
// Return the last index of substring `sub` in `s` if it occurs.

kk_std_core_types__maybe kk_std_core_find_last(kk_string_t s, kk_string_t sub, kk_context_t* _ctx) { /* (s : string, sub : string) -> maybe<sslice> */ 
  kk_ssize_t i;
  kk_string_t _x21443 = kk_string_dup(s); /*string*/
  kk_string_t _x21444 = kk_string_dup(sub); /*string*/
  i = kk_string_last_index_of1(_x21443,_x21444,kk_context()); /*ssize_t*/
  bool _match_20116 = kk_std_core_is_zero_1(i, _ctx); /*bool*/;
  if (_match_20116) {
    kk_string_drop(sub, _ctx);
    kk_string_drop(s, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  {
    kk_std_core__sslice _b_19410_19409;
    kk_ssize_t _x21445 = kk_std_core_decr_1(i, _ctx); /*ssize_t*/
    kk_ssize_t _x21446 = kk_string_len(sub,kk_context()); /*ssize_t*/
    _b_19410_19409 = kk_std_core__new_Sslice(s, _x21445, _x21446, _ctx); /*sslice*/
    return kk_std_core_types__new_Just(kk_std_core__sslice_box(_b_19410_19409, _ctx), _ctx);
  }
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift18033_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_std_core__list xx, kk_std_core_types__maybe _y_17798, kk_context_t* _ctx) { /* forall<a,b,e> (ctail<list<b>>, f : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e list<b> */ 
  if (kk_std_core_types__is_Just(_y_17798)) {
    kk_box_t y = _y_17798._cons.Just.value;
    kk_std_core__list _ctail_17575 = kk_std_core__list_hole(); /*list<12245>*/;
    kk_std_core__list _ctail_17576 = kk_std_core__new_Cons(kk_reuse_null, y, _ctail_17575, _ctx); /*list<12245>*/;
    kk_box_t* _b_19419_19416 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17576)->tail)); /*cfield<list<12245>>*/;
    kk_std_core_types__ctail _x21447 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17576, _ctx)),_b_19419_19416); /*ctail<0>*/
    return kk_std_core__ctail_flatmap_maybe(xx, f, _x21447, _ctx);
  }
  {
    return kk_std_core__ctail_flatmap_maybe(xx, f, _acc, _ctx);
  }
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift18034_op_fun21448__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t y0;
};
static kk_std_core__list kk_std_core__mlift18034_op_fun21448(kk_function_t _fself, kk_std_core__list _ctail_17578, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift18034_op_fun21448(kk_function_t _accm, kk_box_t y0, kk_context_t* _ctx) {
  struct kk_std_core__mlift18034_op_fun21448__t* _self = kk_function_alloc_as(struct kk_std_core__mlift18034_op_fun21448__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift18034_op_fun21448, kk_context());
  _self->_accm = _accm;
  _self->y0 = y0;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift18034_op_fun21448(kk_function_t _fself, kk_std_core__list _ctail_17578, kk_context_t* _ctx) {
  struct kk_std_core__mlift18034_op_fun21448__t* _self = kk_function_as(struct kk_std_core__mlift18034_op_fun21448__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<12245>) -> list<12245> */
  kk_box_t y0 = _self->y0; /* 12245 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(y0);}, {}, _ctx)
  kk_std_core__list _x21449 = kk_std_core__new_Cons(kk_reuse_null, y0, _ctail_17578, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x21449, _ctx));
}

kk_std_core__list kk_std_core__mlift18034_op(kk_function_t _accm, kk_function_t f0, kk_std_core__list xx0, kk_std_core_types__maybe _y_17803, kk_context_t* _ctx) { /* forall<a,b,e> ((list<b>) -> list<b>, f : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e list<b> */ 
  if (kk_std_core_types__is_Just(_y_17803)) {
    kk_box_t y0 = _y_17803._cons.Just.value;
    return kk_std_core__ctailm_flatmap_maybe(xx0, f0, kk_std_core__new_mlift18034_op_fun21448(_accm, y0, _ctx), _ctx);
  }
  {
    return kk_std_core__ctailm_flatmap_maybe(xx0, f0, _accm, _ctx);
  }
}
 
// Concatenate the `Just` result elements from applying a function to all elements.


// lift anonymous function
struct kk_std_core__ctail_flatmap_maybe_fun21453__t {
  struct kk_function_s _base;
  kk_function_t f1;
  kk_std_core__list xx1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_flatmap_maybe_fun21453(kk_function_t _fself, kk_box_t _b_19424, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_flatmap_maybe_fun21453(kk_function_t f1, kk_std_core__list xx1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_flatmap_maybe_fun21453__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_flatmap_maybe_fun21453__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_flatmap_maybe_fun21453, kk_context());
  _self->f1 = f1;
  _self->xx1 = xx1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_flatmap_maybe_fun21453(kk_function_t _fself, kk_box_t _b_19424, kk_context_t* _ctx) {
  struct kk_std_core__ctail_flatmap_maybe_fun21453__t* _self = kk_function_as(struct kk_std_core__ctail_flatmap_maybe_fun21453__t*, _fself);
  kk_function_t f1 = _self->f1; /* (12244) -> 12246 maybe<12245> */
  kk_std_core__list xx1 = _self->xx1; /* list<12244> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<12245>> */
  kk_drop_match(_self, {kk_function_dup(f1);kk_std_core__list_dup(xx1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_std_core_types__maybe _y_19442_177980 = kk_std_core_types__maybe_unbox(_b_19424, _ctx); /*maybe<12245>*/;
  kk_std_core__list _x21454 = kk_std_core__mlift18033_op(_acc0, f1, xx1, _y_19442_177980, _ctx); /*list<12245>*/
  return kk_std_core__list_box(_x21454, _ctx);
}

kk_std_core__list kk_std_core__ctail_flatmap_maybe(kk_std_core__list xs, kk_function_t f1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, f : (a) -> e maybe<b>, ctail<list<b>>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21450 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con21450->head;
    kk_std_core__list xx1 = _con21450->tail;
    kk_reuse_t _ru_20400 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      _ru_20400 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx1);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_std_core_types__maybe x0_18300;
    kk_function_t _x21451 = kk_function_dup(f1); /*(12244) -> 12246 maybe<12245>*/
    x0_18300 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_box_t, kk_context_t*), _x21451, (_x21451, x, _ctx)); /*maybe<12245>*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_20400,kk_context());
      kk_std_core_types__maybe_drop(x0_18300, _ctx);
      kk_box_t _x21452 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_flatmap_maybe_fun21453(f1, xx1, _acc0, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x21452, _ctx);
    }
    if (kk_std_core_types__is_Just(x0_18300)) {
      kk_box_t y1 = x0_18300._cons.Just.value;
      kk_std_core__list _ctail_175750 = kk_std_core__list_hole(); /*list<12245>*/;
      kk_std_core__list _ctail_175760 = kk_std_core__new_Cons(_ru_20400, y1, _ctail_175750, _ctx); /*list<12245>*/;
      kk_box_t* _b_19436_19430 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_175760)->tail)); /*cfield<list<12245>>*/;
      { // tailcall
        kk_std_core_types__ctail _x21455 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_175760, _ctx)),_b_19436_19430); /*ctail<0>*/
        xs = xx1;
        _acc0 = _x21455;
        goto kk__tailcall;
      }
    }
    {
      kk_reuse_drop(_ru_20400,kk_context());
      { // tailcall
        xs = xx1;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_function_drop(f1, _ctx);
    kk_box_t _x21456 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21456, _ctx);
  }
}
 
// Concatenate the `Just` result elements from applying a function to all elements.


// lift anonymous function
struct kk_std_core__ctailm_flatmap_maybe_fun21460__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t f2;
  kk_std_core__list xx2;
};
static kk_box_t kk_std_core__ctailm_flatmap_maybe_fun21460(kk_function_t _fself, kk_box_t _b_19444, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_flatmap_maybe_fun21460(kk_function_t _accm0, kk_function_t f2, kk_std_core__list xx2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_flatmap_maybe_fun21460__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_flatmap_maybe_fun21460__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_flatmap_maybe_fun21460, kk_context());
  _self->_accm0 = _accm0;
  _self->f2 = f2;
  _self->xx2 = xx2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_flatmap_maybe_fun21460(kk_function_t _fself, kk_box_t _b_19444, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_flatmap_maybe_fun21460__t* _self = kk_function_as(struct kk_std_core__ctailm_flatmap_maybe_fun21460__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<12245>) -> list<12245> */
  kk_function_t f2 = _self->f2; /* (12244) -> 12246 maybe<12245> */
  kk_std_core__list xx2 = _self->xx2; /* list<12244> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(f2);kk_std_core__list_dup(xx2);}, {}, _ctx)
  kk_std_core_types__maybe _y_19446_178030 = kk_std_core_types__maybe_unbox(_b_19444, _ctx); /*maybe<12245>*/;
  kk_std_core__list _x21461 = kk_std_core__mlift18034_op(_accm0, f2, xx2, _y_19446_178030, _ctx); /*list<12245>*/
  return kk_std_core__list_box(_x21461, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_flatmap_maybe_fun21463__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t y2;
};
static kk_std_core__list kk_std_core__ctailm_flatmap_maybe_fun21463(kk_function_t _fself, kk_std_core__list _ctail_175780, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_flatmap_maybe_fun21463(kk_function_t _accm0, kk_box_t y2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_flatmap_maybe_fun21463__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_flatmap_maybe_fun21463__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_flatmap_maybe_fun21463, kk_context());
  _self->_accm0 = _accm0;
  _self->y2 = y2;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_flatmap_maybe_fun21463(kk_function_t _fself, kk_std_core__list _ctail_175780, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_flatmap_maybe_fun21463__t* _self = kk_function_as(struct kk_std_core__ctailm_flatmap_maybe_fun21463__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<12245>) -> list<12245> */
  kk_box_t y2 = _self->y2; /* 12245 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(y2);}, {}, _ctx)
  kk_std_core__list _x21464 = kk_std_core__new_Cons(kk_reuse_null, y2, _ctail_175780, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x21464, _ctx));
}

kk_std_core__list kk_std_core__ctailm_flatmap_maybe(kk_std_core__list xs0, kk_function_t f2, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, f : (a) -> e maybe<b>, (list<b>) -> list<b>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con21457 = kk_std_core__as_Cons(xs0);
    kk_box_t x1 = _con21457->head;
    kk_std_core__list xx2 = _con21457->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs0))) {
      kk_std_core__list_free(xs0, _ctx);
    }
    else {
      kk_box_dup(x1);
      kk_std_core__list_dup(xx2);
      kk_std_core__list_decref(xs0, _ctx);
    }
    kk_std_core_types__maybe x2_18303;
    kk_function_t _x21458 = kk_function_dup(f2); /*(12244) -> 12246 maybe<12245>*/
    x2_18303 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_box_t, kk_context_t*), _x21458, (_x21458, x1, _ctx)); /*maybe<12245>*/
    if (kk_yielding(kk_context())) {
      kk_std_core_types__maybe_drop(x2_18303, _ctx);
      kk_box_t _x21459 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_flatmap_maybe_fun21460(_accm0, f2, xx2, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x21459, _ctx);
    }
    if (kk_std_core_types__is_Just(x2_18303)) {
      kk_box_t y2 = x2_18303._cons.Just.value;
      { // tailcall
        kk_function_t _x21462 = kk_std_core__new_ctailm_flatmap_maybe_fun21463(_accm0, y2, _ctx); /*(list<12245>) -> list<12245>*/
        xs0 = xx2;
        _accm0 = _x21462;
        goto kk__tailcall;
      }
    }
    { // tailcall
      xs0 = xx2;
      goto kk__tailcall;
    }
  }
  {
    kk_function_drop(f2, _ctx);
    return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
  }
}
 
// Concatenate the `Just` result elements from applying a function to all elements.


// lift anonymous function
struct kk_std_core_flatmap_maybe_fun21466__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core_flatmap_maybe_fun21466(kk_function_t _fself, kk_std_core__list _ctail_17577, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_flatmap_maybe_fun21466(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_flatmap_maybe_fun21466, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core_flatmap_maybe_fun21466(kk_function_t _fself, kk_std_core__list _ctail_17577, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17577;
}

kk_std_core__list kk_std_core_flatmap_maybe(kk_std_core__list xs1, kk_function_t f3, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, f : (a) -> e maybe<b>) -> e list<b> */ 
  bool _match_20113 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_20113) {
    kk_std_core_types__ctail _x21465 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_flatmap_maybe(xs1, f3, _x21465, _ctx);
  }
  {
    return kk_std_core__ctailm_flatmap_maybe(xs1, f3, kk_std_core_new_flatmap_maybe_fun21466(_ctx), _ctx);
  }
}
 
// monadic lift

kk_box_t kk_std_core__mlift18035_fold_int(kk_integer_t end, kk_function_t f, kk_integer_t start0, kk_box_t x, kk_context_t* _ctx) { /* forall<a,e> (end : int, f : (int, a) -> e a, start0 : int, x : a) -> e a */ 
  kk_integer_t _x21467 = kk_integer_add_small_const(start0, 1, _ctx); /*int*/
  return kk_std_core_fold_int(_x21467, end, x, f, _ctx);
}
 
// fold over the integers between [`start`,`end`] (inclusive).


// lift anonymous function
struct kk_std_core_fold_int_fun21470__t {
  struct kk_function_s _base;
  kk_integer_t end0;
  kk_function_t f0;
  kk_integer_t start00;
};
static kk_box_t kk_std_core_fold_int_fun21470(kk_function_t _fself, kk_box_t x1, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_fold_int_fun21470(kk_integer_t end0, kk_function_t f0, kk_integer_t start00, kk_context_t* _ctx) {
  struct kk_std_core_fold_int_fun21470__t* _self = kk_function_alloc_as(struct kk_std_core_fold_int_fun21470__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_fold_int_fun21470, kk_context());
  _self->end0 = end0;
  _self->f0 = f0;
  _self->start00 = start00;
  return &_self->_base;
}

static kk_box_t kk_std_core_fold_int_fun21470(kk_function_t _fself, kk_box_t x1, kk_context_t* _ctx) {
  struct kk_std_core_fold_int_fun21470__t* _self = kk_function_as(struct kk_std_core_fold_int_fun21470__t*, _fself);
  kk_integer_t end0 = _self->end0; /* int */
  kk_function_t f0 = _self->f0; /* (int, 12305) -> 12306 12305 */
  kk_integer_t start00 = _self->start00; /* int */
  kk_drop_match(_self, {kk_integer_dup(end0);kk_function_dup(f0);kk_integer_dup(start00);}, {}, _ctx)
  return kk_std_core__mlift18035_fold_int(end0, f0, start00, x1, _ctx);
}

kk_box_t kk_std_core_fold_int(kk_integer_t start00, kk_integer_t end0, kk_box_t init0, kk_function_t f0, kk_context_t* _ctx) { /* forall<a,e> (start : int, end : int, init : a, f : (int, a) -> e a) -> e a */ 
  kk__tailcall: ;
  bool _match_20111 = kk_integer_gte_borrow(start00,end0,kk_context()); /*bool*/;
  if (_match_20111) {
    kk_integer_drop(start00, _ctx);
    kk_function_drop(f0, _ctx);
    kk_integer_drop(end0, _ctx);
    return init0;
  }
  {
    kk_box_t x0_18306;
    kk_function_t _x21469 = kk_function_dup(f0); /*(int, 12305) -> 12306 12305*/
    kk_integer_t _x21468 = kk_integer_dup(start00); /*int*/
    x0_18306 = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_box_t, kk_context_t*), _x21469, (_x21469, _x21468, init0, _ctx)); /*12305*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x0_18306, _ctx);
      return kk_std_core_hnd_yield_extend(kk_std_core_new_fold_int_fun21470(end0, f0, start00, _ctx), _ctx);
    }
    { // tailcall
      kk_integer_t _x21471 = kk_integer_add_small_const(start00, 1, _ctx); /*int*/
      start00 = _x21471;
      init0 = x0_18306;
      goto kk__tailcall;
    }
  }
}
 
// monadic lift

kk_box_t kk_std_core__mlift18036_foldl(kk_function_t f, kk_std_core__list xx, kk_box_t _y_17815, kk_context_t* _ctx) { /* forall<a,e,b> (f : (a, b) -> e a, xx : list<b>, a) -> e a */ 
  return kk_std_core_foldl(xx, _y_17815, f, _ctx);
}
 
// Fold a list from the left, i.e. `foldl([1,2],0,(+)) == (0+1)+2`
// Since `foldl` is tail recursive, it is preferred over `foldr` when using an associative function `f`


// lift anonymous function
struct kk_std_core_foldl_fun21475__t {
  struct kk_function_s _base;
  kk_function_t f0;
  kk_std_core__list xx0;
};
static kk_box_t kk_std_core_foldl_fun21475(kk_function_t _fself, kk_box_t _y_178150, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foldl_fun21475(kk_function_t f0, kk_std_core__list xx0, kk_context_t* _ctx) {
  struct kk_std_core_foldl_fun21475__t* _self = kk_function_alloc_as(struct kk_std_core_foldl_fun21475__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foldl_fun21475, kk_context());
  _self->f0 = f0;
  _self->xx0 = xx0;
  return &_self->_base;
}

static kk_box_t kk_std_core_foldl_fun21475(kk_function_t _fself, kk_box_t _y_178150, kk_context_t* _ctx) {
  struct kk_std_core_foldl_fun21475__t* _self = kk_function_as(struct kk_std_core_foldl_fun21475__t*, _fself);
  kk_function_t f0 = _self->f0; /* (12365, 12370) -> 12367 12365 */
  kk_std_core__list xx0 = _self->xx0; /* list<12370> */
  kk_drop_match(_self, {kk_function_dup(f0);kk_std_core__list_dup(xx0);}, {}, _ctx)
  return kk_std_core__mlift18036_foldl(f0, xx0, _y_178150, _ctx);
}

kk_box_t kk_std_core_foldl(kk_std_core__list xs, kk_box_t z, kk_function_t f0, kk_context_t* _ctx) { /* forall<a,b,e> (list<a>, b, (b, a) -> e b) -> e b */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21473 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con21473->head;
    kk_std_core__list xx0 = _con21473->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_box_t x0_18309;
    kk_function_t _x21474 = kk_function_dup(f0); /*(12365, 12370) -> 12367 12365*/
    x0_18309 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_context_t*), _x21474, (_x21474, z, x, _ctx)); /*12365*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x0_18309, _ctx);
      return kk_std_core_hnd_yield_extend(kk_std_core_new_foldl_fun21475(f0, xx0, _ctx), _ctx);
    }
    { // tailcall
      xs = xx0;
      z = x0_18309;
      goto kk__tailcall;
    }
  }
  {
    kk_function_drop(f0, _ctx);
    return z;
  }
}


// lift anonymous function
struct kk_std_core_foldl1_fun21480__t {
  struct kk_function_s _base;
  kk_ssize_t i_18312;
};
static kk_box_t kk_std_core_foldl1_fun21480(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foldl1_fun21480(kk_ssize_t i_18312, kk_context_t* _ctx) {
  struct kk_std_core_foldl1_fun21480__t* _self = kk_function_alloc_as(struct kk_std_core_foldl1_fun21480__t, 1, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foldl1_fun21480, kk_context());
  _self->i_18312 = i_18312;
  return &_self->_base;
}

static kk_box_t kk_std_core_foldl1_fun21480(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_foldl1_fun21480__t* _self = kk_function_as(struct kk_std_core_foldl1_fun21480__t*, _fself);
  kk_ssize_t i_18312 = _self->i_18312; /* std/core/hnd/ev-index */
  kk_drop_match(_self, {;}, {}, _ctx)
  return kk_std_core_hnd_open_at1(i_18312, cont, res, _ctx);
}

kk_box_t kk_std_core_foldl1(kk_std_core__list xs, kk_function_t f, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, f : (a, a) -> <exn|e> a) -> <exn|e> a */ 
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21476 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con21476->head;
    kk_std_core__list xx = _con21476->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    return kk_std_core_foldl(xx, x, f, _ctx);
  }
  {
    kk_function_drop(f, _ctx);
    kk_ssize_t i_18312;
    kk_std_core_hnd__htag _x21477 = kk_std_core_hnd__htag_dup(kk_std_core__tag_exn); /*std/core/hnd/htag<.hnd-exn>*/
    i_18312 = kk_std_core_hnd__evv_index(_x21477, _ctx); /*std/core/hnd/ev-index*/
    kk_evv_t w = kk_evv_swap_create1(i_18312,kk_context()); /*std/core/hnd/evv<<exn|12425>>*/;
    kk_box_t y;
    kk_string_t _x21478;
    kk_define_string_literal(, _s21479, 33, "unexpected Nil in std/core/foldl1")
    _x21478 = kk_string_dup(_s21479); /*string*/
    y = kk_std_core_throw(_x21478, kk_std_core_types__new_None(_ctx), _ctx); /*12424*/
    kk_unit_t __ = kk_Unit;
    kk_evv_set(w,kk_context());
    if (kk_yielding(kk_context())) {
      kk_box_drop(y, _ctx);
      return kk_std_core_hnd_yield_cont(kk_std_core_new_foldl1_fun21480(i_18312, _ctx), _ctx);
    }
    {
      return y;
    }
  }
}
 
// lifted local: reverse, reverse-acc

kk_std_core__list kk_std_core__lift17288_reverse(kk_std_core__list acc, kk_std_core__list ys, kk_context_t* _ctx) { /* forall<a> (acc : list<a>, ys : list<a>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con21481 = kk_std_core__as_Cons(ys);
    kk_box_t x = _con21481->head;
    kk_std_core__list xx = _con21481->tail;
    kk_reuse_t _ru_20404 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      _ru_20404 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(ys, _ctx);
    }
    { // tailcall
      kk_std_core__list _x21482 = kk_std_core__new_Cons(_ru_20404, x, acc, _ctx); /*list<61>*/
      acc = _x21482;
      ys = xx;
      goto kk__tailcall;
    }
  }
  {
    return acc;
  }
}
 
// monadic lift

kk_box_t kk_std_core__mlift18037_op(kk_function_t f, kk_std_core__list xx, kk_box_t z00_17507, kk_context_t* _ctx) { /* forall<a,e,b> (f : (b, a) -> e a, xx : list<b>, z00.17507 : a) -> e a */ 
  return kk_std_core__lift17490_foldr(f, xx, z00_17507, _ctx);
}
 
// lifted local: foldr, .spec.17425
// specialized: std/core/foldl, on parameters f, using:
// f = fn<(e :: E)>(x: 12439, y: 12443){
//   f(y, x);
// }


// lift anonymous function
struct kk_std_core__lift17490_foldr_fun21485__t {
  struct kk_function_s _base;
  kk_function_t f0;
  kk_std_core__list xx0;
};
static kk_box_t kk_std_core__lift17490_foldr_fun21485(kk_function_t _fself, kk_box_t z00_175070, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17490_foldr_fun21485(kk_function_t f0, kk_std_core__list xx0, kk_context_t* _ctx) {
  struct kk_std_core__lift17490_foldr_fun21485__t* _self = kk_function_alloc_as(struct kk_std_core__lift17490_foldr_fun21485__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift17490_foldr_fun21485, kk_context());
  _self->f0 = f0;
  _self->xx0 = xx0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift17490_foldr_fun21485(kk_function_t _fself, kk_box_t z00_175070, kk_context_t* _ctx) {
  struct kk_std_core__lift17490_foldr_fun21485__t* _self = kk_function_as(struct kk_std_core__lift17490_foldr_fun21485__t*, _fself);
  kk_function_t f0 = _self->f0; /* (12443, 12439) -> 12441 12439 */
  kk_std_core__list xx0 = _self->xx0; /* list<12443> */
  kk_drop_match(_self, {kk_function_dup(f0);kk_std_core__list_dup(xx0);}, {}, _ctx)
  return kk_std_core__mlift18037_op(f0, xx0, z00_175070, _ctx);
}

kk_box_t kk_std_core__lift17490_foldr(kk_function_t f0, kk_std_core__list xs0, kk_box_t z0, kk_context_t* _ctx) { /* forall<a,e,b> (f : (b, a) -> e a, xs0 : list<b>, z0 : a) -> e a */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con21483 = kk_std_core__as_Cons(xs0);
    kk_box_t x0 = _con21483->head;
    kk_std_core__list xx0 = _con21483->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs0))) {
      kk_std_core__list_free(xs0, _ctx);
    }
    else {
      kk_box_dup(x0);
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(xs0, _ctx);
    }
    kk_box_t x_18316;
    kk_function_t _x21484 = kk_function_dup(f0); /*(12443, 12439) -> 12441 12439*/
    x_18316 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_context_t*), _x21484, (_x21484, x0, z0, _ctx)); /*12439*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x_18316, _ctx);
      return kk_std_core_hnd_yield_extend(kk_std_core__new_lift17490_foldr_fun21485(f0, xx0, _ctx), _ctx);
    }
    { // tailcall
      xs0 = xx0;
      z0 = x_18316;
      goto kk__tailcall;
    }
  }
  {
    kk_function_drop(f0, _ctx);
    return z0;
  }
}


// lift anonymous function
struct kk_std_core_foldr1_fun21490__t {
  struct kk_function_s _base;
  kk_ssize_t i_18321;
};
static kk_box_t kk_std_core_foldr1_fun21490(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foldr1_fun21490(kk_ssize_t i_18321, kk_context_t* _ctx) {
  struct kk_std_core_foldr1_fun21490__t* _self = kk_function_alloc_as(struct kk_std_core_foldr1_fun21490__t, 1, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foldr1_fun21490, kk_context());
  _self->i_18321 = i_18321;
  return &_self->_base;
}

static kk_box_t kk_std_core_foldr1_fun21490(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_foldr1_fun21490__t* _self = kk_function_as(struct kk_std_core_foldr1_fun21490__t*, _fself);
  kk_ssize_t i_18321 = _self->i_18321; /* std/core/hnd/ev-index */
  kk_drop_match(_self, {;}, {}, _ctx)
  return kk_std_core_hnd_open_at1(i_18321, cont, res, _ctx);
}

kk_box_t kk_std_core_foldr1(kk_std_core__list xs, kk_function_t f, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, f : (a, a) -> <exn|e> a) -> <exn|e> a */ 
  kk_std_core__list xs0_17158 = kk_std_core__lift17288_reverse(kk_std_core__new_Nil(_ctx), xs, _ctx); /*list<12477>*/;
  if (kk_std_core__is_Cons(xs0_17158)) {
    struct kk_std_core_Cons* _con21486 = kk_std_core__as_Cons(xs0_17158);
    kk_box_t x = _con21486->head;
    kk_std_core__list xx = _con21486->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs0_17158))) {
      kk_std_core__list_free(xs0_17158, _ctx);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs0_17158, _ctx);
    }
    return kk_std_core_foldl(xx, x, f, _ctx);
  }
  {
    kk_function_drop(f, _ctx);
    kk_ssize_t i_18321;
    kk_std_core_hnd__htag _x21487 = kk_std_core_hnd__htag_dup(kk_std_core__tag_exn); /*std/core/hnd/htag<.hnd-exn>*/
    i_18321 = kk_std_core_hnd__evv_index(_x21487, _ctx); /*std/core/hnd/ev-index*/
    kk_evv_t w = kk_evv_swap_create1(i_18321,kk_context()); /*std/core/hnd/evv<<exn|12478>>*/;
    kk_box_t y;
    kk_string_t _x21488;
    kk_define_string_literal(, _s21489, 33, "unexpected Nil in std/core/foldl1")
    _x21488 = kk_string_dup(_s21489); /*string*/
    y = kk_std_core_throw(_x21488, kk_std_core_types__new_None(_ctx), _ctx); /*12477*/
    kk_unit_t __ = kk_Unit;
    kk_evv_set(w,kk_context());
    if (kk_yielding(kk_context())) {
      kk_box_drop(y, _ctx);
      return kk_std_core_hnd_yield_cont(kk_std_core_new_foldr1_fun21490(i_18321, _ctx), _ctx);
    }
    {
      return y;
    }
  }
}
 
// monadic lift

kk_unit_t kk_std_core__mlift18038_op(kk_function_t action, kk_integer_t end, kk_integer_t i, kk_unit_t wild__, kk_context_t* _ctx) { /* forall<e> (action : (int) -> e (), end : int, i : int, wild_ : ()) -> e () */ 
  kk_integer_t i0_17464 = kk_integer_add_small_const(i, 1, _ctx); /*int*/;
  kk_std_core__lift17289_for(action, end, i0_17464, _ctx); return kk_Unit;
}
 
// lifted local: for, rep


// lift anonymous function
struct kk_std_core__lift17289_for_fun21494__t {
  struct kk_function_s _base;
  kk_function_t action0;
  kk_integer_t end0;
  kk_integer_t i0;
};
static kk_box_t kk_std_core__lift17289_for_fun21494(kk_function_t _fself, kk_box_t _b_19448, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17289_for_fun21494(kk_function_t action0, kk_integer_t end0, kk_integer_t i0, kk_context_t* _ctx) {
  struct kk_std_core__lift17289_for_fun21494__t* _self = kk_function_alloc_as(struct kk_std_core__lift17289_for_fun21494__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift17289_for_fun21494, kk_context());
  _self->action0 = action0;
  _self->end0 = end0;
  _self->i0 = i0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift17289_for_fun21494(kk_function_t _fself, kk_box_t _b_19448, kk_context_t* _ctx) {
  struct kk_std_core__lift17289_for_fun21494__t* _self = kk_function_as(struct kk_std_core__lift17289_for_fun21494__t*, _fself);
  kk_function_t action0 = _self->action0; /* (int) -> 12514 () */
  kk_integer_t end0 = _self->end0; /* int */
  kk_integer_t i0 = _self->i0; /* int */
  kk_drop_match(_self, {kk_function_dup(action0);kk_integer_dup(end0);kk_integer_dup(i0);}, {}, _ctx)
  kk_unit_t wild__0_19450 = kk_Unit;
  kk_unit_unbox(_b_19448);
  kk_unit_t _x21495 = kk_Unit;
  kk_std_core__mlift18038_op(action0, end0, i0, wild__0_19450, _ctx);
  return kk_unit_box(_x21495);
}

kk_unit_t kk_std_core__lift17289_for(kk_function_t action0, kk_integer_t end0, kk_integer_t i0, kk_context_t* _ctx) { /* forall<e> (action : (int) -> e (), end : int, i : int) -> e () */ 
  kk__tailcall: ;
  bool _match_20105 = kk_integer_lte_borrow(i0,end0,kk_context()); /*bool*/;
  if (_match_20105) {
    kk_unit_t x_18325 = kk_Unit;
    kk_function_t _x21492 = kk_function_dup(action0); /*(int) -> 12514 ()*/
    kk_integer_t _x21491 = kk_integer_dup(i0); /*int*/
    kk_function_call(kk_unit_t, (kk_function_t, kk_integer_t, kk_context_t*), _x21492, (_x21492, _x21491, _ctx));
    if (kk_yielding(kk_context())) {
      kk_box_t _x21493 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift17289_for_fun21494(action0, end0, i0, _ctx), _ctx); /*3926*/
      kk_unit_unbox(_x21493); return kk_Unit;
    }
    {
      kk_integer_t i0_174640 = kk_integer_add_small_const(i0, 1, _ctx); /*int*/;
      { // tailcall
        i0 = i0_174640;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_integer_drop(i0, _ctx);
    kk_integer_drop(end0, _ctx);
    kk_function_drop(action0, _ctx);
    kk_Unit; return kk_Unit;
  }
}
 
// monadic lift

kk_std_core_types__maybe kk_std_core__mlift18039_op(kk_function_t action, kk_integer_t end, kk_integer_t i, kk_std_core_types__maybe _y_17832, kk_context_t* _ctx) { /* forall<a,e> (action : (int) -> e maybe<a>, end : int, i : int, maybe<a>) -> e maybe<a> */ 
  if (kk_std_core_types__is_Nothing(_y_17832)) {
    kk_integer_t i0_17465 = kk_integer_add_small_const(i, 1, _ctx); /*int*/;
    return kk_std_core__lift17290_for_while(action, end, i0_17465, _ctx);
  }
  {
    kk_box_t x0 = _y_17832._cons.Just.value;
    kk_integer_drop(i, _ctx);
    kk_integer_drop(end, _ctx);
    kk_function_drop(action, _ctx);
    return kk_std_core_types__new_Just(x0, _ctx);
  }
}
 
// lifted local: for-while, rep


// lift anonymous function
struct kk_std_core__lift17290_for_while_fun21500__t {
  struct kk_function_s _base;
  kk_function_t action0;
  kk_integer_t end0;
  kk_integer_t i0;
};
static kk_box_t kk_std_core__lift17290_for_while_fun21500(kk_function_t _fself, kk_box_t _b_19452, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17290_for_while_fun21500(kk_function_t action0, kk_integer_t end0, kk_integer_t i0, kk_context_t* _ctx) {
  struct kk_std_core__lift17290_for_while_fun21500__t* _self = kk_function_alloc_as(struct kk_std_core__lift17290_for_while_fun21500__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift17290_for_while_fun21500, kk_context());
  _self->action0 = action0;
  _self->end0 = end0;
  _self->i0 = i0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift17290_for_while_fun21500(kk_function_t _fself, kk_box_t _b_19452, kk_context_t* _ctx) {
  struct kk_std_core__lift17290_for_while_fun21500__t* _self = kk_function_as(struct kk_std_core__lift17290_for_while_fun21500__t*, _fself);
  kk_function_t action0 = _self->action0; /* (int) -> 12565 maybe<12564> */
  kk_integer_t end0 = _self->end0; /* int */
  kk_integer_t i0 = _self->i0; /* int */
  kk_drop_match(_self, {kk_function_dup(action0);kk_integer_dup(end0);kk_integer_dup(i0);}, {}, _ctx)
  kk_std_core_types__maybe _y_19454_178320 = kk_std_core_types__maybe_unbox(_b_19452, _ctx); /*maybe<12564>*/;
  kk_std_core_types__maybe _x21501 = kk_std_core__mlift18039_op(action0, end0, i0, _y_19454_178320, _ctx); /*maybe<12564>*/
  return kk_std_core_types__maybe_box(_x21501, _ctx);
}

kk_std_core_types__maybe kk_std_core__lift17290_for_while(kk_function_t action0, kk_integer_t end0, kk_integer_t i0, kk_context_t* _ctx) { /* forall<a,e> (action : (int) -> e maybe<a>, end : int, i : int) -> e maybe<a> */ 
  kk__tailcall: ;
  bool _match_20103 = kk_integer_lte_borrow(i0,end0,kk_context()); /*bool*/;
  if (_match_20103) {
    kk_std_core_types__maybe x_18328;
    kk_function_t _x21498 = kk_function_dup(action0); /*(int) -> 12565 maybe<12564>*/
    kk_integer_t _x21497 = kk_integer_dup(i0); /*int*/
    x_18328 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_integer_t, kk_context_t*), _x21498, (_x21498, _x21497, _ctx)); /*maybe<12564>*/
    if (kk_yielding(kk_context())) {
      kk_std_core_types__maybe_drop(x_18328, _ctx);
      kk_box_t _x21499 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift17290_for_while_fun21500(action0, end0, i0, _ctx), _ctx); /*3926*/
      return kk_std_core_types__maybe_unbox(_x21499, _ctx);
    }
    if (kk_std_core_types__is_Nothing(x_18328)) {
      kk_integer_t i0_174650 = kk_integer_add_small_const(i0, 1, _ctx); /*int*/;
      { // tailcall
        i0 = i0_174650;
        goto kk__tailcall;
      }
    }
    {
      kk_box_t x00 = x_18328._cons.Just.value;
      kk_integer_drop(i0, _ctx);
      kk_integer_drop(end0, _ctx);
      kk_function_drop(action0, _ctx);
      return kk_std_core_types__new_Just(x00, _ctx);
    }
  }
  {
    kk_integer_drop(i0, _ctx);
    kk_integer_drop(end0, _ctx);
    kk_function_drop(action0, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
}
 
// monadic lift

kk_unit_t kk_std_core__mlift18040_op(kk_function_t action, kk_ref_t i, kk_std_core__list xx, kk_unit_t wild__0, kk_context_t* _ctx) { /* forall<h,a,e> (action : (int, a) -> e (), i : local-var<h,int>, xx : list<a>, wild_0 : ()) -> <local<h>|e> () */ 
  kk_std_core__lift17491_foreach_indexed(action, i, xx, _ctx); return kk_Unit;
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift18041_op_fun21503__t {
  struct kk_function_s _base;
  kk_function_t action0;
  kk_ref_t i0;
  kk_std_core__list xx0;
};
static kk_box_t kk_std_core__mlift18041_op_fun21503(kk_function_t _fself, kk_box_t _b_19460, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift18041_op_fun21503(kk_function_t action0, kk_ref_t i0, kk_std_core__list xx0, kk_context_t* _ctx) {
  struct kk_std_core__mlift18041_op_fun21503__t* _self = kk_function_alloc_as(struct kk_std_core__mlift18041_op_fun21503__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift18041_op_fun21503, kk_context());
  _self->action0 = action0;
  _self->i0 = i0;
  _self->xx0 = xx0;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift18041_op_fun21503(kk_function_t _fself, kk_box_t _b_19460, kk_context_t* _ctx) {
  struct kk_std_core__mlift18041_op_fun21503__t* _self = kk_function_as(struct kk_std_core__mlift18041_op_fun21503__t*, _fself);
  kk_function_t action0 = _self->action0; /* (int, 12675) -> 12676 () */
  kk_ref_t i0 = _self->i0; /* local-var<12669,int> */
  kk_std_core__list xx0 = _self->xx0; /* list<12675> */
  kk_drop_match(_self, {kk_function_dup(action0);kk_ref_dup(i0);kk_std_core__list_dup(xx0);}, {}, _ctx)
  kk_unit_t wild__00_19462 = kk_Unit;
  kk_unit_unbox(_b_19460);
  kk_unit_t _x21504 = kk_Unit;
  kk_std_core__mlift18040_op(action0, i0, xx0, wild__00_19462, _ctx);
  return kk_unit_box(_x21504);
}

kk_unit_t kk_std_core__mlift18041_op(kk_function_t action0, kk_ref_t i0, kk_std_core__list xx0, kk_integer_t _y_17840, kk_context_t* _ctx) { /* forall<h,a,e> (action : (int, a) -> e (), i : local-var<h,int>, xx : list<a>, int) -> <local<h>|e> () */ 
  kk_integer_t _b_19458_19456 = kk_integer_add_small_const(_y_17840, 1, _ctx); /*int*/;
  kk_unit_t x_18331 = kk_Unit;
  kk_ref_set_borrow(i0,(kk_integer_box(_b_19458_19456)),kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_t _x21502 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift18041_op_fun21503(action0, i0, xx0, _ctx), _ctx); /*3926*/
    kk_unit_unbox(_x21502); return kk_Unit;
  }
  {
    kk_std_core__mlift18040_op(action0, i0, xx0, x_18331, _ctx); return kk_Unit;
  }
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift18042_op_fun21508__t {
  struct kk_function_s _base;
  kk_function_t action1;
  kk_ref_t i1;
  kk_std_core__list xx1;
};
static kk_box_t kk_std_core__mlift18042_op_fun21508(kk_function_t _fself, kk_box_t _b_19466, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift18042_op_fun21508(kk_function_t action1, kk_ref_t i1, kk_std_core__list xx1, kk_context_t* _ctx) {
  struct kk_std_core__mlift18042_op_fun21508__t* _self = kk_function_alloc_as(struct kk_std_core__mlift18042_op_fun21508__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift18042_op_fun21508, kk_context());
  _self->action1 = action1;
  _self->i1 = i1;
  _self->xx1 = xx1;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift18042_op_fun21508(kk_function_t _fself, kk_box_t _b_19466, kk_context_t* _ctx) {
  struct kk_std_core__mlift18042_op_fun21508__t* _self = kk_function_as(struct kk_std_core__mlift18042_op_fun21508__t*, _fself);
  kk_function_t action1 = _self->action1; /* (int, 12675) -> 12676 () */
  kk_ref_t i1 = _self->i1; /* local-var<12669,int> */
  kk_std_core__list xx1 = _self->xx1; /* list<12675> */
  kk_drop_match(_self, {kk_function_dup(action1);kk_ref_dup(i1);kk_std_core__list_dup(xx1);}, {}, _ctx)
  kk_integer_t _y_19468_178400 = kk_integer_unbox(_b_19466); /*int*/;
  kk_unit_t _x21509 = kk_Unit;
  kk_std_core__mlift18041_op(action1, i1, xx1, _y_19468_178400, _ctx);
  return kk_unit_box(_x21509);
}

kk_unit_t kk_std_core__mlift18042_op(kk_function_t action1, kk_ref_t i1, kk_std_core__list xx1, kk_unit_t wild__, kk_context_t* _ctx) { /* forall<h,a,e> (action : (int, a) -> e (), i : local-var<h,int>, xx : list<a>, wild_ : ()) -> <local<h>|e> () */ 
  kk_integer_t x0_18333;
  kk_box_t _x21505;
  kk_ref_t _x21506 = kk_ref_dup(i1); /*local-var<12669,int>*/
  _x21505 = kk_ref_get(_x21506,kk_context()); /*1000*/
  x0_18333 = kk_integer_unbox(_x21505); /*int*/
  if (kk_yielding(kk_context())) {
    kk_integer_drop(x0_18333, _ctx);
    kk_box_t _x21507 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift18042_op_fun21508(action1, i1, xx1, _ctx), _ctx); /*3926*/
    kk_unit_unbox(_x21507); return kk_Unit;
  }
  {
    kk_std_core__mlift18041_op(action1, i1, xx1, x0_18333, _ctx); return kk_Unit;
  }
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift18043_op_fun21512__t {
  struct kk_function_s _base;
  kk_function_t action2;
  kk_ref_t i2;
  kk_std_core__list xx2;
};
static kk_box_t kk_std_core__mlift18043_op_fun21512(kk_function_t _fself, kk_box_t _b_19470, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift18043_op_fun21512(kk_function_t action2, kk_ref_t i2, kk_std_core__list xx2, kk_context_t* _ctx) {
  struct kk_std_core__mlift18043_op_fun21512__t* _self = kk_function_alloc_as(struct kk_std_core__mlift18043_op_fun21512__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift18043_op_fun21512, kk_context());
  _self->action2 = action2;
  _self->i2 = i2;
  _self->xx2 = xx2;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift18043_op_fun21512(kk_function_t _fself, kk_box_t _b_19470, kk_context_t* _ctx) {
  struct kk_std_core__mlift18043_op_fun21512__t* _self = kk_function_as(struct kk_std_core__mlift18043_op_fun21512__t*, _fself);
  kk_function_t action2 = _self->action2; /* (int, 12675) -> 12676 () */
  kk_ref_t i2 = _self->i2; /* local-var<12669,int> */
  kk_std_core__list xx2 = _self->xx2; /* list<12675> */
  kk_drop_match(_self, {kk_function_dup(action2);kk_ref_dup(i2);kk_std_core__list_dup(xx2);}, {}, _ctx)
  kk_unit_t wild__1_19472 = kk_Unit;
  kk_unit_unbox(_b_19470);
  kk_unit_t _x21513 = kk_Unit;
  kk_std_core__mlift18042_op(action2, i2, xx2, wild__1_19472, _ctx);
  return kk_unit_box(_x21513);
}

kk_unit_t kk_std_core__mlift18043_op(kk_function_t action2, kk_ref_t i2, kk_box_t x1, kk_std_core__list xx2, kk_integer_t j, kk_context_t* _ctx) { /* forall<h,a,e> (action : (int, a) -> e (), i : local-var<h,int>, x1 : a, xx : list<a>, j : int) -> <local<h>|e> () */ 
  kk_unit_t x2_18335 = kk_Unit;
  kk_function_t _x21510 = kk_function_dup(action2); /*(int, 12675) -> 12676 ()*/
  kk_function_call(kk_unit_t, (kk_function_t, kk_integer_t, kk_box_t, kk_context_t*), _x21510, (_x21510, j, x1, _ctx));
  if (kk_yielding(kk_context())) {
    kk_box_t _x21511 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift18043_op_fun21512(action2, i2, xx2, _ctx), _ctx); /*3926*/
    kk_unit_unbox(_x21511); return kk_Unit;
  }
  {
    kk_std_core__mlift18042_op(action2, i2, xx2, x2_18335, _ctx); return kk_Unit;
  }
}
 
// lifted local: foreach-indexed, .spec.17428
// specialized: std/core/foreach, on parameters action, using:
// action = fn<<(local :: H -> X)<(h :: H)>|(e :: E)>>(x: 12675){
//   val j : int;
//   val _ : ();
//   std/core/types/local-set<int,(e :: E),(h :: H)>(i, (std/core/types/.open<((<>) :: E),<(local :: H -> X)<(h :: H)>|(e :: E)>,(x : int, y : int) -> int,(x : int, y : int) -> <(local :: H -> X)<(h :: H)>|(e :: E)> int>((fn(x0: int, y: int){
//       (std/core/int-add(x0, y));
//     }))((std/core/types/local-get<int,(h :: H),(e :: E)>(i)), 1)));
// }


// lift anonymous function
struct kk_std_core__lift17491_foreach_indexed_fun21518__t {
  struct kk_function_s _base;
  kk_function_t action4;
  kk_ref_t i3;
  kk_box_t x10;
  kk_std_core__list xx3;
};
static kk_box_t kk_std_core__lift17491_foreach_indexed_fun21518(kk_function_t _fself, kk_box_t _b_19476, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17491_foreach_indexed_fun21518(kk_function_t action4, kk_ref_t i3, kk_box_t x10, kk_std_core__list xx3, kk_context_t* _ctx) {
  struct kk_std_core__lift17491_foreach_indexed_fun21518__t* _self = kk_function_alloc_as(struct kk_std_core__lift17491_foreach_indexed_fun21518__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift17491_foreach_indexed_fun21518, kk_context());
  _self->action4 = action4;
  _self->i3 = i3;
  _self->x10 = x10;
  _self->xx3 = xx3;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift17491_foreach_indexed_fun21518(kk_function_t _fself, kk_box_t _b_19476, kk_context_t* _ctx) {
  struct kk_std_core__lift17491_foreach_indexed_fun21518__t* _self = kk_function_as(struct kk_std_core__lift17491_foreach_indexed_fun21518__t*, _fself);
  kk_function_t action4 = _self->action4; /* (int, 12675) -> 12676 () */
  kk_ref_t i3 = _self->i3; /* local-var<12669,int> */
  kk_box_t x10 = _self->x10; /* 12675 */
  kk_std_core__list xx3 = _self->xx3; /* list<12675> */
  kk_drop_match(_self, {kk_function_dup(action4);kk_ref_dup(i3);kk_box_dup(x10);kk_std_core__list_dup(xx3);}, {}, _ctx)
  kk_integer_t j0_19493 = kk_integer_unbox(_b_19476); /*int*/;
  kk_unit_t _x21519 = kk_Unit;
  kk_std_core__mlift18043_op(action4, i3, x10, xx3, j0_19493, _ctx);
  return kk_unit_box(_x21519);
}


// lift anonymous function
struct kk_std_core__lift17491_foreach_indexed_fun21522__t {
  struct kk_function_s _base;
  kk_function_t action4;
  kk_ref_t i3;
  kk_std_core__list xx3;
};
static kk_box_t kk_std_core__lift17491_foreach_indexed_fun21522(kk_function_t _fself, kk_box_t _b_19478, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17491_foreach_indexed_fun21522(kk_function_t action4, kk_ref_t i3, kk_std_core__list xx3, kk_context_t* _ctx) {
  struct kk_std_core__lift17491_foreach_indexed_fun21522__t* _self = kk_function_alloc_as(struct kk_std_core__lift17491_foreach_indexed_fun21522__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift17491_foreach_indexed_fun21522, kk_context());
  _self->action4 = action4;
  _self->i3 = i3;
  _self->xx3 = xx3;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift17491_foreach_indexed_fun21522(kk_function_t _fself, kk_box_t _b_19478, kk_context_t* _ctx) {
  struct kk_std_core__lift17491_foreach_indexed_fun21522__t* _self = kk_function_as(struct kk_std_core__lift17491_foreach_indexed_fun21522__t*, _fself);
  kk_function_t action4 = _self->action4; /* (int, 12675) -> 12676 () */
  kk_ref_t i3 = _self->i3; /* local-var<12669,int> */
  kk_std_core__list xx3 = _self->xx3; /* list<12675> */
  kk_drop_match(_self, {kk_function_dup(action4);kk_ref_dup(i3);kk_std_core__list_dup(xx3);}, {}, _ctx)
  kk_unit_t wild__2_19494 = kk_Unit;
  kk_unit_unbox(_b_19478);
  kk_unit_t _x21523 = kk_Unit;
  kk_std_core__mlift18042_op(action4, i3, xx3, wild__2_19494, _ctx);
  return kk_unit_box(_x21523);
}


// lift anonymous function
struct kk_std_core__lift17491_foreach_indexed_fun21527__t {
  struct kk_function_s _base;
  kk_function_t action4;
  kk_ref_t i3;
  kk_std_core__list xx3;
};
static kk_box_t kk_std_core__lift17491_foreach_indexed_fun21527(kk_function_t _fself, kk_box_t _b_19482, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17491_foreach_indexed_fun21527(kk_function_t action4, kk_ref_t i3, kk_std_core__list xx3, kk_context_t* _ctx) {
  struct kk_std_core__lift17491_foreach_indexed_fun21527__t* _self = kk_function_alloc_as(struct kk_std_core__lift17491_foreach_indexed_fun21527__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift17491_foreach_indexed_fun21527, kk_context());
  _self->action4 = action4;
  _self->i3 = i3;
  _self->xx3 = xx3;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift17491_foreach_indexed_fun21527(kk_function_t _fself, kk_box_t _b_19482, kk_context_t* _ctx) {
  struct kk_std_core__lift17491_foreach_indexed_fun21527__t* _self = kk_function_as(struct kk_std_core__lift17491_foreach_indexed_fun21527__t*, _fself);
  kk_function_t action4 = _self->action4; /* (int, 12675) -> 12676 () */
  kk_ref_t i3 = _self->i3; /* local-var<12669,int> */
  kk_std_core__list xx3 = _self->xx3; /* list<12675> */
  kk_drop_match(_self, {kk_function_dup(action4);kk_ref_dup(i3);kk_std_core__list_dup(xx3);}, {}, _ctx)
  kk_integer_t _y_19495_178401 = kk_integer_unbox(_b_19482); /*int*/;
  kk_unit_t _x21528 = kk_Unit;
  kk_std_core__mlift18041_op(action4, i3, xx3, _y_19495_178401, _ctx);
  return kk_unit_box(_x21528);
}


// lift anonymous function
struct kk_std_core__lift17491_foreach_indexed_fun21530__t {
  struct kk_function_s _base;
  kk_function_t action4;
  kk_ref_t i3;
  kk_std_core__list xx3;
};
static kk_box_t kk_std_core__lift17491_foreach_indexed_fun21530(kk_function_t _fself, kk_box_t _b_19488, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17491_foreach_indexed_fun21530(kk_function_t action4, kk_ref_t i3, kk_std_core__list xx3, kk_context_t* _ctx) {
  struct kk_std_core__lift17491_foreach_indexed_fun21530__t* _self = kk_function_alloc_as(struct kk_std_core__lift17491_foreach_indexed_fun21530__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift17491_foreach_indexed_fun21530, kk_context());
  _self->action4 = action4;
  _self->i3 = i3;
  _self->xx3 = xx3;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift17491_foreach_indexed_fun21530(kk_function_t _fself, kk_box_t _b_19488, kk_context_t* _ctx) {
  struct kk_std_core__lift17491_foreach_indexed_fun21530__t* _self = kk_function_as(struct kk_std_core__lift17491_foreach_indexed_fun21530__t*, _fself);
  kk_function_t action4 = _self->action4; /* (int, 12675) -> 12676 () */
  kk_ref_t i3 = _self->i3; /* local-var<12669,int> */
  kk_std_core__list xx3 = _self->xx3; /* list<12675> */
  kk_drop_match(_self, {kk_function_dup(action4);kk_ref_dup(i3);kk_std_core__list_dup(xx3);}, {}, _ctx)
  kk_unit_t wild__01_19496 = kk_Unit;
  kk_unit_unbox(_b_19488);
  kk_unit_t _x21531 = kk_Unit;
  kk_std_core__mlift18040_op(action4, i3, xx3, wild__01_19496, _ctx);
  return kk_unit_box(_x21531);
}

kk_unit_t kk_std_core__lift17491_foreach_indexed(kk_function_t action4, kk_ref_t i3, kk_std_core__list xs0, kk_context_t* _ctx) { /* forall<h,a,e> (action : (int, a) -> e (), i : local-var<h,int>, xs0 : list<a>) -> <local<h>|e> () */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con21514 = kk_std_core__as_Cons(xs0);
    kk_box_t x10 = _con21514->head;
    kk_std_core__list xx3 = _con21514->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs0))) {
      kk_std_core__list_free(xs0, _ctx);
    }
    else {
      kk_box_dup(x10);
      kk_std_core__list_dup(xx3);
      kk_std_core__list_decref(xs0, _ctx);
    }
    kk_integer_t x3_18338;
    kk_box_t _x21515;
    kk_ref_t _x21516 = kk_ref_dup(i3); /*local-var<12669,int>*/
    _x21515 = kk_ref_get(_x21516,kk_context()); /*1000*/
    x3_18338 = kk_integer_unbox(_x21515); /*int*/
    if (kk_yielding(kk_context())) {
      kk_integer_drop(x3_18338, _ctx);
      kk_box_t _x21517 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift17491_foreach_indexed_fun21518(action4, i3, x10, xx3, _ctx), _ctx); /*3926*/
      kk_unit_unbox(_x21517); return kk_Unit;
    }
    {
      kk_unit_t x4_18341 = kk_Unit;
      kk_function_t _x21520 = kk_function_dup(action4); /*(int, 12675) -> 12676 ()*/
      kk_function_call(kk_unit_t, (kk_function_t, kk_integer_t, kk_box_t, kk_context_t*), _x21520, (_x21520, x3_18338, x10, _ctx));
      if (kk_yielding(kk_context())) {
        kk_box_t _x21521 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift17491_foreach_indexed_fun21522(action4, i3, xx3, _ctx), _ctx); /*3926*/
        kk_unit_unbox(_x21521); return kk_Unit;
      }
      {
        kk_integer_t x5_18345;
        kk_box_t _x21524;
        kk_ref_t _x21525 = kk_ref_dup(i3); /*local-var<12669,int>*/
        _x21524 = kk_ref_get(_x21525,kk_context()); /*1000*/
        x5_18345 = kk_integer_unbox(_x21524); /*int*/
        if (kk_yielding(kk_context())) {
          kk_integer_drop(x5_18345, _ctx);
          kk_box_t _x21526 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift17491_foreach_indexed_fun21527(action4, i3, xx3, _ctx), _ctx); /*3926*/
          kk_unit_unbox(_x21526); return kk_Unit;
        }
        {
          kk_integer_t _b_19486_19484 = kk_integer_add_small_const(x5_18345, 1, _ctx); /*int*/;
          kk_unit_t x6_18348 = kk_Unit;
          kk_ref_set_borrow(i3,(kk_integer_box(_b_19486_19484)),kk_context());
          if (kk_yielding(kk_context())) {
            kk_box_t _x21529 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift17491_foreach_indexed_fun21530(action4, i3, xx3, _ctx), _ctx); /*3926*/
            kk_unit_unbox(_x21529); return kk_Unit;
          }
          { // tailcall
            xs0 = xx3;
            goto kk__tailcall;
          }
        }
      }
    }
  }
  {
    kk_ref_drop(i3, _ctx);
    kk_function_drop(action4, _ctx);
    kk_Unit; return kk_Unit;
  }
}
 
// Invoke `action` for each element of a list, passing also the position of the element.

kk_unit_t kk_std_core_foreach_indexed(kk_std_core__list xs, kk_function_t action, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, action : (int, a) -> e ()) -> e () */ 
  kk_ref_t loc = kk_ref_alloc((kk_integer_box(kk_integer_from_small(0))),kk_context()); /*local-var<12669,int>*/;
  kk_unit_t res = kk_Unit;
  kk_ref_t _x21532 = kk_ref_dup(loc); /*local-var<12669,int>*/
  kk_std_core__lift17491_foreach_indexed(action, _x21532, xs, _ctx);
  kk_box_t _x21533 = kk_std_core_hnd_prompt_local_var(loc, kk_unit_box(res), _ctx); /*10459*/
  kk_unit_unbox(_x21533); return kk_Unit;
}
 
// monadic lift

kk_unit_t kk_std_core__mlift18044_op(kk_ssize_t end_17168, kk_function_t f, kk_ssize_t i, kk_vector_t v, kk_unit_t wild__, kk_context_t* _ctx) { /* forall<a,e> (end.17168 : ssize_t, f : (int, a) -> e (), i : ssize_t, v : vector<a>, wild_ : ()) -> e () */ 
  kk_ssize_t i0_17466 = kk_std_core_incr_1(i, _ctx); /*ssize_t*/;
  kk_std_core__lift17291_foreach_indexed_1(end_17168, f, v, i0_17466, _ctx); return kk_Unit;
}
 
// lifted local: foreach-indexed.1, rep


// lift anonymous function
struct kk_std_core__lift17291_foreach_indexed_fun21537__t_1 {
  struct kk_function_s _base;
  kk_function_t f0;
  kk_vector_t v0;
  kk_ssize_t end_171680;
  kk_ssize_t i0;
};
static kk_box_t kk_std_core__lift17291_foreach_indexed_fun21537_1(kk_function_t _fself, kk_box_t _b_19504, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17291_foreach_indexed_fun21537_1(kk_function_t f0, kk_vector_t v0, kk_ssize_t end_171680, kk_ssize_t i0, kk_context_t* _ctx) {
  struct kk_std_core__lift17291_foreach_indexed_fun21537__t_1* _self = kk_function_alloc_as(struct kk_std_core__lift17291_foreach_indexed_fun21537__t_1, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift17291_foreach_indexed_fun21537_1, kk_context());
  _self->f0 = f0;
  _self->v0 = v0;
  _self->end_171680 = end_171680;
  _self->i0 = i0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift17291_foreach_indexed_fun21537_1(kk_function_t _fself, kk_box_t _b_19504, kk_context_t* _ctx) {
  struct kk_std_core__lift17291_foreach_indexed_fun21537__t_1* _self = kk_function_as(struct kk_std_core__lift17291_foreach_indexed_fun21537__t_1*, _fself);
  kk_function_t f0 = _self->f0; /* (int, 12706) -> 12707 () */
  kk_vector_t v0 = _self->v0; /* vector<12706> */
  kk_ssize_t end_171680 = _self->end_171680; /* ssize_t */
  kk_ssize_t i0 = _self->i0; /* ssize_t */
  kk_drop_match(_self, {kk_function_dup(f0);kk_vector_dup(v0);;;}, {}, _ctx)
  kk_unit_t wild__0_19506 = kk_Unit;
  kk_unit_unbox(_b_19504);
  kk_unit_t _x21538 = kk_Unit;
  kk_std_core__mlift18044_op(end_171680, f0, i0, v0, wild__0_19506, _ctx);
  return kk_unit_box(_x21538);
}

kk_unit_t kk_std_core__lift17291_foreach_indexed_1(kk_ssize_t end_171680, kk_function_t f0, kk_vector_t v0, kk_ssize_t i0, kk_context_t* _ctx) { /* forall<a,e> (end.17168 : ssize_t, f : (int, a) -> e (), v : vector<a>, i : ssize_t) -> e () */ 
  kk__tailcall: ;
  bool _match_20094 = (i0 <= end_171680); /*bool*/;
  if (_match_20094) {
    kk_box_t x_17264 = kk_vector_at_borrow(v0,i0); /*12706*/;
    kk_unit_t x_18353 = kk_Unit;
    kk_function_t _x21535 = kk_function_dup(f0); /*(int, 12706) -> 12707 ()*/
    kk_integer_t _x21534 = kk_integer_from_ssize_t(i0,kk_context()); /*int*/
    kk_function_call(kk_unit_t, (kk_function_t, kk_integer_t, kk_box_t, kk_context_t*), _x21535, (_x21535, _x21534, x_17264, _ctx));
    if (kk_yielding(kk_context())) {
      kk_box_t _x21536 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift17291_foreach_indexed_fun21537_1(f0, v0, end_171680, i0, _ctx), _ctx); /*3926*/
      kk_unit_unbox(_x21536); return kk_Unit;
    }
    {
      kk_ssize_t i0_174660 = kk_std_core_incr_1(i0, _ctx); /*ssize_t*/;
      { // tailcall
        i0 = i0_174660;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_vector_drop(v0, _ctx);
    kk_function_drop(f0, _ctx);
    kk_Unit; return kk_Unit;
  }
}
 
// Return the head of list if the list is not empty.

kk_std_core_types__maybe kk_std_core_head_1(kk_std_core__list xs, kk_context_t* _ctx) { /* forall<a> (xs : list<a>) -> maybe<a> */ 
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21548 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con21548->head;
    kk_std_core__list _pat0 = _con21548->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_drop(_pat0, _ctx);
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_decref(xs, _ctx);
    }
    return kk_std_core_types__new_Just(x, _ctx);
  }
  {
    return kk_std_core_types__new_Nothing(_ctx);
  }
}
 
// Return the head of list if the list is not empty, or use `default` otherwise

kk_box_t kk_std_core_head_2(kk_std_core__list xs, kk_box_t default0, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, default : a) -> a */ 
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21549 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con21549->head;
    kk_std_core__list _pat0 = _con21549->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_drop(_pat0, _ctx);
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_box_drop(default0, _ctx);
    return x;
  }
  {
    return default0;
  }
}
 
// Return the first character of a string as a string (or the empty string)

kk_string_t kk_std_core_head_3(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> string */ 
  kk_std_core__sslice slice0 = kk_std_core_first1(s, _ctx); /*sslice*/;
  kk_std_core__sslice _x21550;
  bool _match_20087;
  kk_integer_t _brw_20092;
  kk_std_core_types__optional _x21551 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
  if (kk_std_core_types__is_Optional(_x21551)) {
    kk_box_t _box_x19507 = _x21551._cons.Optional.value;
    kk_integer_t _n_10142 = kk_integer_unbox(_box_x19507);
    _brw_20092 = _n_10142; /*int*/
  }
  else {
    _brw_20092 = kk_integer_from_small(1); /*int*/
  }
  bool _brw_20093 = kk_integer_eq_borrow(_brw_20092,(kk_integer_from_small(1)),kk_context()); /*bool*/;
  kk_integer_drop(_brw_20092, _ctx);
  _match_20087 = _brw_20093; /*bool*/
  if (_match_20087) {
    _x21550 = slice0; /*sslice*/
  }
  else {
    kk_integer_t _brw_20089;
    kk_integer_t _x21553;
    kk_std_core_types__optional _x21554 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
    if (kk_std_core_types__is_Optional(_x21554)) {
      kk_box_t _box_x19508 = _x21554._cons.Optional.value;
      kk_integer_t _n_101420 = kk_integer_unbox(_box_x19508);
      _x21553 = _n_101420; /*int*/
    }
    else {
      _x21553 = kk_integer_from_small(1); /*int*/
    }
    _brw_20089 = kk_integer_add_small_const(_x21553, -1, _ctx); /*int*/
    kk_std_core__sslice _brw_20090 = kk_std_core_extend(slice0, _brw_20089, _ctx); /*sslice*/;
    kk_integer_drop(_brw_20089, _ctx);
    _x21550 = _brw_20090; /*sslice*/
  }
  return kk_std_core_string_3(_x21550, _ctx);
}
 
// Return the first character of a string (or `Nothing` for the empty string).


// lift anonymous function
struct kk_std_core_head_char_fun21560__t {
  struct kk_function_s _base;
};
static kk_std_core_types__maybe kk_std_core_head_char_fun21560(kk_function_t _fself, kk_char_t _b_19509, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_head_char_fun21560(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_head_char_fun21560, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core_types__maybe kk_std_core_head_char_fun21560(kk_function_t _fself, kk_char_t _b_19509, kk_context_t* _ctx) {
  kk_unused(_fself);
  return kk_std_core_types__new_Just(kk_char_box(_b_19509, _ctx), _ctx);
}

kk_std_core_types__maybe kk_std_core_head_char(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> maybe<char> */ 
  kk_std_core__sslice _x21556;
  kk_string_t _x21557 = kk_string_dup(s); /*string*/
  kk_ssize_t _x21558 = (KK_IZ(0)); /*ssize_t*/
  kk_ssize_t _x21559 = kk_string_len(s,kk_context()); /*ssize_t*/
  _x21556 = kk_std_core__new_Sslice(_x21557, _x21558, _x21559, _ctx); /*sslice*/
  return kk_std_core_foreach_while_1(_x21556, kk_std_core_new_head_char_fun21560(_ctx), _ctx);
}

kk_integer_t kk_std_core_index_of_acc(kk_std_core__list xs, kk_function_t pred, kk_integer_t idx, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, pred : (a) -> bool, idx : int) -> int */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21561 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con21561->head;
    kk_std_core__list xx = _con21561->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    bool _match_20086;
    kk_function_t _x21562 = kk_function_dup(pred); /*(13082) -> bool*/
    _match_20086 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x21562, (_x21562, x, _ctx)); /*bool*/
    if (_match_20086) {
      kk_std_core__list_drop(xx, _ctx);
      kk_function_drop(pred, _ctx);
      return idx;
    }
    { // tailcall
      kk_integer_t _x21563 = kk_integer_add_small_const(idx, 1, _ctx); /*int*/
      xs = xx;
      idx = _x21563;
      goto kk__tailcall;
    }
  }
  {
    kk_function_drop(pred, _ctx);
    kk_integer_drop(idx, _ctx);
    return kk_integer_from_small(-1);
  }
}
 
// Return the list without its last element.
// Return an empty list for an empty list.

kk_std_core__list kk_std_core__ctail_init(kk_std_core__list xs, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, ctail<list<a>>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21564 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con21564->head;
    kk_std_core__list xx = _con21564->tail;
    if (kk_std_core__is_Cons(xx)) {
      struct kk_std_core_Cons* _con21565 = kk_std_core__as_Cons(xx);
      kk_reuse_t _ru_20411 = kk_reuse_null; /*reuse*/;
      if (kk_likely(kk_std_core__list_is_unique(xs))) {
        _ru_20411 = (kk_std_core__list_reuse(xs));
      }
      else {
        kk_box_dup(x);
        kk_std_core__list_dup(xx);
        kk_std_core__list_decref(xs, _ctx);
      }
      kk_std_core__list _ctail_17579 = kk_std_core__list_hole(); /*list<13119>*/;
      kk_std_core__list _ctail_17580 = kk_std_core__new_Cons(_ru_20411, x, _ctail_17579, _ctx); /*list<13119>*/;
      kk_box_t* _b_19520_19515 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17580)->tail)); /*cfield<list<13119>>*/;
      { // tailcall
        kk_std_core_types__ctail _x21566 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17580, _ctx)),_b_19520_19515); /*ctail<0>*/
        xs = xx;
        _acc = _x21566;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_std_core__list_drop(xs, _ctx);
    kk_box_t _x21567 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21567, _ctx);
  }
}
 
// Return the list without its last element.
// Return an empty list for an empty list.

kk_std_core__list kk_std_core_init(kk_std_core__list xs0, kk_context_t* _ctx) { /* forall<a> (xs : list<a>) -> list<a> */ 
  kk_std_core_types__ctail _x21568 = kk_ctail_nil(); /*ctail<0>*/
  return kk_std_core__ctail_init(xs0, _x21568, _ctx);
}
 
// An invalid slice

kk_std_core__sslice kk_std_core_invalid;
 
// Is the character an ASCII letter is-

bool kk_std_core_is_alpha(kk_char_t c, kk_context_t* _ctx) { /* (c : char) -> bool */ 
  bool _match_20080 = (c >= ('a')); /*bool*/;
  if (_match_20080) {
    bool _match_20082 = (c <= ('z')); /*bool*/;
    if (_match_20082) {
      return true;
    }
    {
      bool _match_20083 = (c >= ('A')); /*bool*/;
      if (_match_20083) {
        return (c <= ('Z'));
      }
      {
        return false;
      }
    }
  }
  {
    bool _match_20081 = (c >= ('A')); /*bool*/;
    if (_match_20081) {
      return (c <= ('Z'));
    }
    {
      return false;
    }
  }
}
 
// Is the character an ASCII hexa-decimal digit ?

bool kk_std_core_is_hex_digit(kk_char_t c, kk_context_t* _ctx) { /* (c : char) -> bool */ 
  bool _match_20067 = (c >= ('0')); /*bool*/;
  if (_match_20067) {
    bool _match_20072 = (c <= ('9')); /*bool*/;
    if (_match_20072) {
      return true;
    }
    {
      bool _match_20073 = (c >= ('a')); /*bool*/;
      if (_match_20073) {
        bool _match_20075 = (c <= ('f')); /*bool*/;
        if (_match_20075) {
          return true;
        }
        {
          bool _match_20076 = (c >= ('A')); /*bool*/;
          if (_match_20076) {
            return (c <= ('F'));
          }
          {
            return false;
          }
        }
      }
      {
        bool _match_20074 = (c >= ('A')); /*bool*/;
        if (_match_20074) {
          return (c <= ('F'));
        }
        {
          return false;
        }
      }
    }
  }
  {
    bool _match_20068 = (c >= ('a')); /*bool*/;
    if (_match_20068) {
      bool _match_20070 = (c <= ('f')); /*bool*/;
      if (_match_20070) {
        return true;
      }
      {
        bool _match_20071 = (c >= ('A')); /*bool*/;
        if (_match_20071) {
          return (c <= ('F'));
        }
        {
          return false;
        }
      }
    }
    {
      bool _match_20069 = (c >= ('A')); /*bool*/;
      if (_match_20069) {
        return (c <= ('F'));
      }
      {
        return false;
      }
    }
  }
}
 
// Tests if a character is an element of `" \t\n\r"`

bool kk_std_core_is_white(kk_char_t c, kk_context_t* _ctx) { /* (c : char) -> bool */ 
  bool _match_20064 = (c == (' ')); /*bool*/;
  if (_match_20064) {
    return true;
  }
  {
    bool _match_20065 = (c == 0x0009); /*bool*/;
    if (_match_20065) {
      return true;
    }
    {
      bool _match_20066 = (c == 0x000A); /*bool*/;
      if (_match_20066) {
        return true;
      }
      {
        return (c == 0x000D);
      }
    }
  }
}
 
// Append `end` to each string in the list `xs` and join them all together.
// `join-end([],end) === ""`
// `join-end(["a","b"],"/") === "a/b/"`

kk_string_t kk_std_core_join_end(kk_std_core__list xs, kk_string_t end, kk_context_t* _ctx) { /* (xs : list<string>, end : string) -> string */ 
  if (kk_std_core__is_Nil(xs)) {
    kk_string_drop(end, _ctx);
    return kk_string_empty();
  }
  {
    kk_string_t _x21579;
    if (kk_std_core__is_Nil(xs)) {
      _x21579 = kk_string_empty(); /*string*/
    }
    else {
      struct kk_std_core_Cons* _con21581 = kk_std_core__as_Cons(xs);
      kk_box_t _box_x19526 = _con21581->head;
      kk_std_core__list xx = _con21581->tail;
      kk_string_t x = kk_string_unbox(_box_x19526);
      if (kk_likely(kk_std_core__list_is_unique(xs))) {
        kk_std_core__list_free(xs, _ctx);
      }
      else {
        kk_string_dup(x);
        kk_std_core__list_dup(xx);
        kk_std_core__list_decref(xs, _ctx);
      }
      kk_string_t _x21583 = kk_string_dup(end); /*string*/
      _x21579 = kk_std_core__lift17277_joinsep(_x21583, xx, x, _ctx); /*string*/
    }
    return kk_std_core__lp__plus__plus__1_rp_(_x21579, end, _ctx);
  }
}
 
// Return the last element of a list (or `Nothing` for the empty list)

kk_std_core_types__maybe kk_std_core_last(kk_std_core__list xs, kk_context_t* _ctx) { /* forall<a> (xs : list<a>) -> maybe<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21584 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con21584->head;
    kk_std_core__list _pat0 = _con21584->tail;
    if (kk_std_core__is_Nil(_pat0)) {
      if (kk_likely(kk_std_core__list_is_unique(xs))) {
        kk_std_core__list_free(xs, _ctx);
      }
      else {
        kk_box_dup(x);
        kk_std_core__list_decref(xs, _ctx);
      }
      return kk_std_core_types__new_Just(x, _ctx);
    }
  }
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21585 = kk_std_core__as_Cons(xs);
    kk_box_t _pat2 = _con21585->head;
    kk_std_core__list xx = _con21585->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_box_drop(_pat2, _ctx);
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    { // tailcall
      xs = xx;
      goto kk__tailcall;
    }
  }
  {
    return kk_std_core_types__new_Nothing(_ctx);
  }
}
 
// Return the last element of a list (or `default` for the empty list)

kk_box_t kk_std_core_last_1(kk_std_core__list xs, kk_box_t default0, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, default : a) -> a */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21586 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con21586->head;
    kk_std_core__list _pat0 = _con21586->tail;
    if (kk_std_core__is_Nil(_pat0)) {
      if (kk_likely(kk_std_core__list_is_unique(xs))) {
        kk_std_core__list_free(xs, _ctx);
      }
      else {
        kk_box_dup(x);
        kk_std_core__list_decref(xs, _ctx);
      }
      kk_box_drop(default0, _ctx);
      return x;
    }
  }
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21587 = kk_std_core__as_Cons(xs);
    kk_box_t _pat2 = _con21587->head;
    kk_std_core__list xx = _con21587->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_box_drop(_pat2, _ctx);
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    { // tailcall
      xs = xx;
      goto kk__tailcall;
    }
  }
  {
    return default0;
  }
}
 
// O(`n`). The last `n` (default = `1`) characters in a string

kk_std_core__sslice kk_std_core_last_2(kk_string_t s, kk_std_core_types__optional n, kk_context_t* _ctx) { /* (s : string, n : optional<int>) -> sslice */ 
  kk_std_core__sslice slice0 = kk_std_core_last1(s, _ctx); /*sslice*/;
  bool _match_20056;
  kk_integer_t _brw_20062;
  if (kk_std_core_types__is_Optional(n)) {
    kk_box_t _box_x19527 = n._cons.Optional.value;
    kk_integer_t _n_13713 = kk_integer_unbox(_box_x19527);
    kk_integer_dup(_n_13713);
    _brw_20062 = _n_13713; /*int*/
  }
  else {
    _brw_20062 = kk_integer_from_small(1); /*int*/
  }
  bool _brw_20063 = kk_integer_eq_borrow(_brw_20062,(kk_integer_from_small(1)),kk_context()); /*bool*/;
  kk_integer_drop(_brw_20062, _ctx);
  _match_20056 = _brw_20063; /*bool*/
  if (_match_20056) {
    kk_std_core_types__optional_drop(n, _ctx);
    return slice0;
  }
  {
    kk_std_core__sslice _own_20058;
    kk_integer_t _brw_20059;
    kk_integer_t _x21589;
    if (kk_std_core_types__is_Optional(n)) {
      kk_box_t _box_x19528 = n._cons.Optional.value;
      kk_integer_t _n_137130 = kk_integer_unbox(_box_x19528);
      kk_integer_dup(_n_137130);
      _x21589 = _n_137130; /*int*/
    }
    else {
      _x21589 = kk_integer_from_small(1); /*int*/
    }
    _brw_20059 = kk_integer_sub((kk_integer_from_small(1)),_x21589,kk_context()); /*int*/
    kk_std_core__sslice _brw_20060 = kk_std_core_advance(slice0, _brw_20059, _ctx); /*sslice*/;
    kk_integer_drop(_brw_20059, _ctx);
    _own_20058 = _brw_20060; /*sslice*/
    kk_integer_t _brw_20057;
    kk_integer_t _x21591;
    if (kk_std_core_types__is_Optional(n)) {
      kk_box_t _box_x19529 = n._cons.Optional.value;
      kk_integer_t _n_137131 = kk_integer_unbox(_box_x19529);
      _x21591 = _n_137131; /*int*/
    }
    else {
      _x21591 = kk_integer_from_small(1); /*int*/
    }
    _brw_20057 = kk_integer_add_small_const(_x21591, -1, _ctx); /*int*/
    kk_std_core__sslice _brw_20061 = kk_std_core_extend(_own_20058, _brw_20057, _ctx); /*sslice*/;
    kk_integer_drop(_brw_20057, _ctx);
    return _brw_20061;
  }
}
 
// Take the first `n` elements of a list (or fewer if the list is shorter than `n`)

kk_std_core__list kk_std_core__ctail_take(kk_std_core__list xs, kk_integer_t n, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, n : int, ctail<list<a>>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21593 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con21593->head;
    kk_std_core__list xx = _con21593->tail;
    if (kk_integer_gt_borrow(n,(kk_integer_from_small(0)),kk_context())) {
      kk_reuse_t _ru_20417 = kk_reuse_null; /*reuse*/;
      if (kk_likely(kk_std_core__list_is_unique(xs))) {
        _ru_20417 = (kk_std_core__list_reuse(xs));
      }
      else {
        kk_box_dup(x);
        kk_std_core__list_dup(xx);
        kk_std_core__list_decref(xs, _ctx);
      }
      kk_std_core__list _ctail_17581 = kk_std_core__list_hole(); /*list<13837>*/;
      kk_std_core__list _ctail_17582 = kk_std_core__new_Cons(_ru_20417, x, _ctail_17581, _ctx); /*list<13837>*/;
      kk_box_t* _b_19540_19535 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17582)->tail)); /*cfield<list<13837>>*/;
      { // tailcall
        kk_integer_t _x21594 = kk_integer_add_small_const(n, -1, _ctx); /*int*/
        kk_std_core_types__ctail _x21595 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17582, _ctx)),_b_19540_19535); /*ctail<0>*/
        xs = xx;
        n = _x21594;
        _acc = _x21595;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_std_core__list_drop(xs, _ctx);
    kk_integer_drop(n, _ctx);
    kk_box_t _x21596 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21596, _ctx);
  }
}
 
// Take the first `n` elements of a list (or fewer if the list is shorter than `n`)

kk_std_core__list kk_std_core_take(kk_std_core__list xs0, kk_integer_t n0, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, n : int) -> list<a> */ 
  kk_std_core_types__ctail _x21597 = kk_ctail_nil(); /*ctail<0>*/
  return kk_std_core__ctail_take(xs0, n0, _x21597, _ctx);
}
 
// split a list at position `n`

kk_std_core_types__tuple2_ kk_std_core_split(kk_std_core__list xs, kk_integer_t n, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, n : int) -> (list<a>, list<a>) */ 
  kk_std_core__list _b_19548_19546;
  kk_std_core__list _x21598 = kk_std_core__list_dup(xs); /*list<13857>*/
  kk_integer_t _x21599 = kk_integer_dup(n); /*int*/
  _b_19548_19546 = kk_std_core_take(_x21598, _x21599, _ctx); /*list<13857>*/
  kk_std_core__list _b_19549_19547 = kk_std_core_drop(xs, n, _ctx); /*list<13857>*/;
  return kk_std_core_types__new_dash__lp__comma__rp_(kk_std_core__list_box(_b_19548_19546, _ctx), kk_std_core__list_box(_b_19549_19547, _ctx), _ctx);
}
 
// lifted local: lookup, .spec.17430
// specialized: std/core/foreach-while, on parameters action, using:
// action = fn(kv: (14189, 14190)){
//   match ((pred((match (kv) {
//     ((.skip std/core/types/(,)((.x: 14189) : a, (.pat0: 14190) : a) : (a, b) ) as .pat: (14189, 14190))
//        -> .x;
//   })))) {
//     ((std/core/types/True() : bool ) as .pat1: bool)
//        -> std/core/types/Just<a>((match (kv) {
//         ((.skip std/core/types/(,)((.pat00: 14189) : a, (.x0: 14190) : a) : (a, b) ) as .pat2: (14189, 14190))
//            -> .x0;
//       }));
//     ((.skip std/core/types/False() : bool ) as .pat01: bool)
//        -> std/core/types/Nothing<a>;
//   };
// }

kk_std_core_types__maybe kk_std_core__lift17492_lookup(kk_function_t pred, kk_std_core__list xs0, kk_context_t* _ctx) { /* forall<a,b> (pred : (a) -> bool, xs0 : list<(a, b)>) -> maybe<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Nil(xs0)) {
    kk_function_drop(pred, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  {
    struct kk_std_core_Cons* _con21604 = kk_std_core__as_Cons(xs0);
    kk_box_t _box_x19550 = _con21604->head;
    kk_std_core__list xx = _con21604->tail;
    kk_std_core_types__tuple2_ x = kk_std_core_types__tuple2__unbox(_box_x19550, NULL);
    if (kk_likely(kk_std_core__list_is_unique(xs0))) {
      kk_std_core_types__tuple2__dup(x);
      kk_box_drop(_box_x19550, _ctx);
      kk_std_core__list_free(xs0, _ctx);
    }
    else {
      kk_std_core_types__tuple2__dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs0, _ctx);
    }
    bool _match_20055;
    kk_function_t _x21607 = kk_function_dup(pred); /*(14189) -> bool*/
    kk_box_t _x21606;
    {
      kk_box_t _x = x.fst;
      kk_box_dup(_x);
      _x21606 = _x; /*14189*/
    }
    _match_20055 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x21607, (_x21607, _x21606, _ctx)); /*bool*/
    if (_match_20055) {
      kk_std_core__list_drop(xx, _ctx);
      kk_function_drop(pred, _ctx);
      kk_box_t _x21608;
      {
        kk_box_t _x0 = x.snd;
        kk_box_dup(_x0);
        kk_std_core_types__tuple2__drop(x, _ctx);
        _x21608 = _x0; /*14190*/
      }
      return kk_std_core_types__new_Just(_x21608, _ctx);
    }
    {
      kk_std_core_types__tuple2__drop(x, _ctx);
      { // tailcall
        xs0 = xx;
        goto kk__tailcall;
      }
    }
  }
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift18045_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_integer_t i0_17469, kk_std_core__list yy, kk_box_t _ctail_17583, kk_context_t* _ctx) { /* forall<a,b,e> (ctail<list<b>>, f : (idx : int, value : a) -> e b, i0.17469 : int, yy : list<a>, b) -> e list<b> */ 
  kk_std_core__list _ctail_17584 = kk_std_core__list_hole(); /*list<14245>*/;
  kk_std_core__list _ctail_17585 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17583, _ctail_17584, _ctx); /*list<14245>*/;
  kk_box_t* _b_19559_19556 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17585)->tail)); /*cfield<list<14245>>*/;
  kk_std_core_types__ctail _x21609 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17585, _ctx)),_b_19559_19556); /*ctail<0>*/
  return kk_std_core__ctail_lift17292_map_indexed(f, yy, i0_17469, _x21609, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift18046_op_fun21610__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t _ctail_17588;
};
static kk_std_core__list kk_std_core__mlift18046_op_fun21610(kk_function_t _fself, kk_std_core__list _ctail_17587, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift18046_op_fun21610(kk_function_t _accm, kk_box_t _ctail_17588, kk_context_t* _ctx) {
  struct kk_std_core__mlift18046_op_fun21610__t* _self = kk_function_alloc_as(struct kk_std_core__mlift18046_op_fun21610__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift18046_op_fun21610, kk_context());
  _self->_accm = _accm;
  _self->_ctail_17588 = _ctail_17588;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift18046_op_fun21610(kk_function_t _fself, kk_std_core__list _ctail_17587, kk_context_t* _ctx) {
  struct kk_std_core__mlift18046_op_fun21610__t* _self = kk_function_as(struct kk_std_core__mlift18046_op_fun21610__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<14245>) -> list<14245> */
  kk_box_t _ctail_17588 = _self->_ctail_17588; /* 14245 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(_ctail_17588);}, {}, _ctx)
  kk_std_core__list _x21611 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17588, _ctail_17587, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x21611, _ctx));
}

kk_std_core__list kk_std_core__mlift18046_op(kk_function_t _accm, kk_function_t f0, kk_integer_t i0_174690, kk_std_core__list yy0, kk_box_t _ctail_17588, kk_context_t* _ctx) { /* forall<a,b,e> ((list<b>) -> list<b>, f : (idx : int, value : a) -> e b, i0.17469 : int, yy : list<a>, b) -> e list<b> */ 
  return kk_std_core__ctailm_lift17292_map_indexed(f0, yy0, i0_174690, kk_std_core__new_mlift18046_op_fun21610(_accm, _ctail_17588, _ctx), _ctx);
}
 
// lifted local: map-indexed, map-idx


// lift anonymous function
struct kk_std_core__ctail_lift17292_map_indexed_fun21616__t {
  struct kk_function_s _base;
  kk_function_t f1;
  kk_integer_t i0_174691;
  kk_std_core__list yy1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_lift17292_map_indexed_fun21616(kk_function_t _fself, kk_box_t _b_19564, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_lift17292_map_indexed_fun21616(kk_function_t f1, kk_integer_t i0_174691, kk_std_core__list yy1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_lift17292_map_indexed_fun21616__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_lift17292_map_indexed_fun21616__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_lift17292_map_indexed_fun21616, kk_context());
  _self->f1 = f1;
  _self->i0_174691 = i0_174691;
  _self->yy1 = yy1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_lift17292_map_indexed_fun21616(kk_function_t _fself, kk_box_t _b_19564, kk_context_t* _ctx) {
  struct kk_std_core__ctail_lift17292_map_indexed_fun21616__t* _self = kk_function_as(struct kk_std_core__ctail_lift17292_map_indexed_fun21616__t*, _fself);
  kk_function_t f1 = _self->f1; /* (idx : int, value : 14244) -> 14246 14245 */
  kk_integer_t i0_174691 = _self->i0_174691; /* int */
  kk_std_core__list yy1 = _self->yy1; /* list<14244> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<14245>> */
  kk_drop_match(_self, {kk_function_dup(f1);kk_integer_dup(i0_174691);kk_std_core__list_dup(yy1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_box_t _ctail_19582_175830 = _b_19564; /*14245*/;
  kk_std_core__list _x21617 = kk_std_core__mlift18045_op(_acc0, f1, i0_174691, yy1, _ctail_19582_175830, _ctx); /*list<14245>*/
  return kk_std_core__list_box(_x21617, _ctx);
}

kk_std_core__list kk_std_core__ctail_lift17292_map_indexed(kk_function_t f1, kk_std_core__list ys, kk_integer_t i, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,b,e> (f : (idx : int, value : a) -> e b, ys : list<a>, i : int, ctail<list<b>>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con21612 = kk_std_core__as_Cons(ys);
    kk_box_t y = _con21612->head;
    kk_std_core__list yy1 = _con21612->tail;
    kk_reuse_t _ru_20419 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      _ru_20419 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(y);
      kk_std_core__list_dup(yy1);
      kk_std_core__list_decref(ys, _ctx);
    }
    kk_integer_t i0_174691;
    kk_integer_t _x21613 = kk_integer_dup(i); /*int*/
    i0_174691 = kk_integer_add_small_const(_x21613, 1, _ctx); /*int*/
    kk_box_t x_18358;
    kk_function_t _x21614 = kk_function_dup(f1); /*(idx : int, value : 14244) -> 14246 14245*/
    x_18358 = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_box_t, kk_context_t*), _x21614, (_x21614, i, y, _ctx)); /*14245*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_20419,kk_context());
      kk_box_drop(x_18358, _ctx);
      kk_box_t _x21615 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_lift17292_map_indexed_fun21616(f1, i0_174691, yy1, _acc0, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x21615, _ctx);
    }
    {
      kk_std_core__list _ctail_175840 = kk_std_core__list_hole(); /*list<14245>*/;
      kk_std_core__list _ctail_175850 = kk_std_core__new_Cons(_ru_20419, x_18358, _ctail_175840, _ctx); /*list<14245>*/;
      kk_box_t* _b_19576_19570 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_175850)->tail)); /*cfield<list<14245>>*/;
      { // tailcall
        kk_std_core_types__ctail _x21618 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_175850, _ctx)),_b_19576_19570); /*ctail<0>*/
        ys = yy1;
        i = i0_174691;
        _acc0 = _x21618;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_integer_drop(i, _ctx);
    kk_function_drop(f1, _ctx);
    kk_box_t _x21619 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21619, _ctx);
  }
}
 
// lifted local: map-indexed, map-idx


// lift anonymous function
struct kk_std_core__ctailm_lift17292_map_indexed_fun21624__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t f2;
  kk_integer_t i0_174692;
  kk_std_core__list yy2;
};
static kk_box_t kk_std_core__ctailm_lift17292_map_indexed_fun21624(kk_function_t _fself, kk_box_t _b_19584, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_lift17292_map_indexed_fun21624(kk_function_t _accm0, kk_function_t f2, kk_integer_t i0_174692, kk_std_core__list yy2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17292_map_indexed_fun21624__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_lift17292_map_indexed_fun21624__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_lift17292_map_indexed_fun21624, kk_context());
  _self->_accm0 = _accm0;
  _self->f2 = f2;
  _self->i0_174692 = i0_174692;
  _self->yy2 = yy2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_lift17292_map_indexed_fun21624(kk_function_t _fself, kk_box_t _b_19584, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17292_map_indexed_fun21624__t* _self = kk_function_as(struct kk_std_core__ctailm_lift17292_map_indexed_fun21624__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<14245>) -> list<14245> */
  kk_function_t f2 = _self->f2; /* (idx : int, value : 14244) -> 14246 14245 */
  kk_integer_t i0_174692 = _self->i0_174692; /* int */
  kk_std_core__list yy2 = _self->yy2; /* list<14244> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(f2);kk_integer_dup(i0_174692);kk_std_core__list_dup(yy2);}, {}, _ctx)
  kk_box_t _ctail_19586_175880 = _b_19584; /*14245*/;
  kk_std_core__list _x21625 = kk_std_core__mlift18046_op(_accm0, f2, i0_174692, yy2, _ctail_19586_175880, _ctx); /*list<14245>*/
  return kk_std_core__list_box(_x21625, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_lift17292_map_indexed_fun21627__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t x0_18361;
};
static kk_std_core__list kk_std_core__ctailm_lift17292_map_indexed_fun21627(kk_function_t _fself, kk_std_core__list _ctail_175870, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_lift17292_map_indexed_fun21627(kk_function_t _accm0, kk_box_t x0_18361, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17292_map_indexed_fun21627__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_lift17292_map_indexed_fun21627__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_lift17292_map_indexed_fun21627, kk_context());
  _self->_accm0 = _accm0;
  _self->x0_18361 = x0_18361;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_lift17292_map_indexed_fun21627(kk_function_t _fself, kk_std_core__list _ctail_175870, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17292_map_indexed_fun21627__t* _self = kk_function_as(struct kk_std_core__ctailm_lift17292_map_indexed_fun21627__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<14245>) -> list<14245> */
  kk_box_t x0_18361 = _self->x0_18361; /* 14245 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(x0_18361);}, {}, _ctx)
  kk_std_core__list _x21628 = kk_std_core__new_Cons(kk_reuse_null, x0_18361, _ctail_175870, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x21628, _ctx));
}

kk_std_core__list kk_std_core__ctailm_lift17292_map_indexed(kk_function_t f2, kk_std_core__list ys0, kk_integer_t i0, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,b,e> (f : (idx : int, value : a) -> e b, ys : list<a>, i : int, (list<b>) -> list<b>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys0)) {
    struct kk_std_core_Cons* _con21620 = kk_std_core__as_Cons(ys0);
    kk_box_t y0 = _con21620->head;
    kk_std_core__list yy2 = _con21620->tail;
    if (kk_likely(kk_std_core__list_is_unique(ys0))) {
      kk_std_core__list_free(ys0, _ctx);
    }
    else {
      kk_box_dup(y0);
      kk_std_core__list_dup(yy2);
      kk_std_core__list_decref(ys0, _ctx);
    }
    kk_integer_t i0_174692;
    kk_integer_t _x21621 = kk_integer_dup(i0); /*int*/
    i0_174692 = kk_integer_add_small_const(_x21621, 1, _ctx); /*int*/
    kk_box_t x0_18361;
    kk_function_t _x21622 = kk_function_dup(f2); /*(idx : int, value : 14244) -> 14246 14245*/
    x0_18361 = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_box_t, kk_context_t*), _x21622, (_x21622, i0, y0, _ctx)); /*14245*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x0_18361, _ctx);
      kk_box_t _x21623 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_lift17292_map_indexed_fun21624(_accm0, f2, i0_174692, yy2, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x21623, _ctx);
    }
    { // tailcall
      kk_function_t _x21626 = kk_std_core__new_ctailm_lift17292_map_indexed_fun21627(_accm0, x0_18361, _ctx); /*(list<14245>) -> list<14245>*/
      ys0 = yy2;
      i0 = i0_174692;
      _accm0 = _x21626;
      goto kk__tailcall;
    }
  }
  {
    kk_integer_drop(i0, _ctx);
    kk_function_drop(f2, _ctx);
    return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
  }
}
 
// lifted local: map-indexed, map-idx


// lift anonymous function
struct kk_std_core__lift17292_map_indexed_fun21630__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core__lift17292_map_indexed_fun21630(kk_function_t _fself, kk_std_core__list _ctail_17586, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17292_map_indexed_fun21630(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core__lift17292_map_indexed_fun21630, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core__lift17292_map_indexed_fun21630(kk_function_t _fself, kk_std_core__list _ctail_17586, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17586;
}

kk_std_core__list kk_std_core__lift17292_map_indexed(kk_function_t f3, kk_std_core__list ys1, kk_integer_t i1, kk_context_t* _ctx) { /* forall<a,b,e> (f : (idx : int, value : a) -> e b, ys : list<a>, i : int) -> e list<b> */ 
  bool _match_20052 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_20052) {
    kk_std_core_types__ctail _x21629 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_lift17292_map_indexed(f3, ys1, i1, _x21629, _ctx);
  }
  {
    return kk_std_core__ctailm_lift17292_map_indexed(f3, ys1, i1, kk_std_core__new_lift17292_map_indexed_fun21630(_ctx), _ctx);
  }
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift18047_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_integer_t i0_17471, kk_std_core__list yy, kk_box_t _ctail_17589, kk_context_t* _ctx) { /* forall<a,b,e> (ctail<list<b>>, f : (idx : int, value : a, rest : list<a>) -> e b, i0.17471 : int, yy : list<a>, b) -> e list<b> */ 
  kk_std_core__list _ctail_17590 = kk_std_core__list_hole(); /*list<14302>*/;
  kk_std_core__list _ctail_17591 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17589, _ctail_17590, _ctx); /*list<14302>*/;
  kk_box_t* _b_19595_19592 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17591)->tail)); /*cfield<list<14302>>*/;
  kk_std_core_types__ctail _x21631 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17591, _ctx)),_b_19595_19592); /*ctail<0>*/
  return kk_std_core__ctail_lift17293_map_indexed_peek(f, yy, i0_17471, _x21631, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift18048_op_fun21632__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t _ctail_17594;
};
static kk_std_core__list kk_std_core__mlift18048_op_fun21632(kk_function_t _fself, kk_std_core__list _ctail_17593, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift18048_op_fun21632(kk_function_t _accm, kk_box_t _ctail_17594, kk_context_t* _ctx) {
  struct kk_std_core__mlift18048_op_fun21632__t* _self = kk_function_alloc_as(struct kk_std_core__mlift18048_op_fun21632__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift18048_op_fun21632, kk_context());
  _self->_accm = _accm;
  _self->_ctail_17594 = _ctail_17594;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift18048_op_fun21632(kk_function_t _fself, kk_std_core__list _ctail_17593, kk_context_t* _ctx) {
  struct kk_std_core__mlift18048_op_fun21632__t* _self = kk_function_as(struct kk_std_core__mlift18048_op_fun21632__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<14302>) -> list<14302> */
  kk_box_t _ctail_17594 = _self->_ctail_17594; /* 14302 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(_ctail_17594);}, {}, _ctx)
  kk_std_core__list _x21633 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17594, _ctail_17593, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x21633, _ctx));
}

kk_std_core__list kk_std_core__mlift18048_op(kk_function_t _accm, kk_function_t f0, kk_integer_t i0_174710, kk_std_core__list yy0, kk_box_t _ctail_17594, kk_context_t* _ctx) { /* forall<a,b,e> ((list<b>) -> list<b>, f : (idx : int, value : a, rest : list<a>) -> e b, i0.17471 : int, yy : list<a>, b) -> e list<b> */ 
  return kk_std_core__ctailm_lift17293_map_indexed_peek(f0, yy0, i0_174710, kk_std_core__new_mlift18048_op_fun21632(_accm, _ctail_17594, _ctx), _ctx);
}
 
// lifted local: map-indexed-peek, mapidx


// lift anonymous function
struct kk_std_core__ctail_lift17293_map_indexed_peek_fun21639__t {
  struct kk_function_s _base;
  kk_function_t f1;
  kk_integer_t i0_174711;
  kk_std_core__list yy1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_lift17293_map_indexed_peek_fun21639(kk_function_t _fself, kk_box_t _b_19600, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_lift17293_map_indexed_peek_fun21639(kk_function_t f1, kk_integer_t i0_174711, kk_std_core__list yy1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_lift17293_map_indexed_peek_fun21639__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_lift17293_map_indexed_peek_fun21639__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_lift17293_map_indexed_peek_fun21639, kk_context());
  _self->f1 = f1;
  _self->i0_174711 = i0_174711;
  _self->yy1 = yy1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_lift17293_map_indexed_peek_fun21639(kk_function_t _fself, kk_box_t _b_19600, kk_context_t* _ctx) {
  struct kk_std_core__ctail_lift17293_map_indexed_peek_fun21639__t* _self = kk_function_as(struct kk_std_core__ctail_lift17293_map_indexed_peek_fun21639__t*, _fself);
  kk_function_t f1 = _self->f1; /* (idx : int, value : 14301, rest : list<14301>) -> 14303 14302 */
  kk_integer_t i0_174711 = _self->i0_174711; /* int */
  kk_std_core__list yy1 = _self->yy1; /* list<14301> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<14302>> */
  kk_drop_match(_self, {kk_function_dup(f1);kk_integer_dup(i0_174711);kk_std_core__list_dup(yy1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_box_t _ctail_19618_175890 = _b_19600; /*14302*/;
  kk_std_core__list _x21640 = kk_std_core__mlift18047_op(_acc0, f1, i0_174711, yy1, _ctail_19618_175890, _ctx); /*list<14302>*/
  return kk_std_core__list_box(_x21640, _ctx);
}

kk_std_core__list kk_std_core__ctail_lift17293_map_indexed_peek(kk_function_t f1, kk_std_core__list ys, kk_integer_t i, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,b,e> (f : (idx : int, value : a, rest : list<a>) -> e b, ys : list<a>, i : int, ctail<list<b>>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con21634 = kk_std_core__as_Cons(ys);
    kk_box_t y = _con21634->head;
    kk_std_core__list yy1 = _con21634->tail;
    kk_reuse_t _ru_20421 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      _ru_20421 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(y);
      kk_std_core__list_dup(yy1);
      kk_std_core__list_decref(ys, _ctx);
    }
    kk_integer_t i0_174711;
    kk_integer_t _x21635 = kk_integer_dup(i); /*int*/
    i0_174711 = kk_integer_add_small_const(_x21635, 1, _ctx); /*int*/
    kk_box_t x_18364;
    kk_function_t _x21637 = kk_function_dup(f1); /*(idx : int, value : 14301, rest : list<14301>) -> 14303 14302*/
    kk_std_core__list _x21636 = kk_std_core__list_dup(yy1); /*list<14301>*/
    x_18364 = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_box_t, kk_std_core__list, kk_context_t*), _x21637, (_x21637, i, y, _x21636, _ctx)); /*14302*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_20421,kk_context());
      kk_box_drop(x_18364, _ctx);
      kk_box_t _x21638 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_lift17293_map_indexed_peek_fun21639(f1, i0_174711, yy1, _acc0, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x21638, _ctx);
    }
    {
      kk_std_core__list _ctail_175900 = kk_std_core__list_hole(); /*list<14302>*/;
      kk_std_core__list _ctail_175910 = kk_std_core__new_Cons(_ru_20421, x_18364, _ctail_175900, _ctx); /*list<14302>*/;
      kk_box_t* _b_19612_19606 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_175910)->tail)); /*cfield<list<14302>>*/;
      { // tailcall
        kk_std_core_types__ctail _x21641 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_175910, _ctx)),_b_19612_19606); /*ctail<0>*/
        ys = yy1;
        i = i0_174711;
        _acc0 = _x21641;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_integer_drop(i, _ctx);
    kk_function_drop(f1, _ctx);
    kk_box_t _x21642 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21642, _ctx);
  }
}
 
// lifted local: map-indexed-peek, mapidx


// lift anonymous function
struct kk_std_core__ctailm_lift17293_map_indexed_peek_fun21648__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t f2;
  kk_integer_t i0_174712;
  kk_std_core__list yy2;
};
static kk_box_t kk_std_core__ctailm_lift17293_map_indexed_peek_fun21648(kk_function_t _fself, kk_box_t _b_19620, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_lift17293_map_indexed_peek_fun21648(kk_function_t _accm0, kk_function_t f2, kk_integer_t i0_174712, kk_std_core__list yy2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17293_map_indexed_peek_fun21648__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_lift17293_map_indexed_peek_fun21648__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_lift17293_map_indexed_peek_fun21648, kk_context());
  _self->_accm0 = _accm0;
  _self->f2 = f2;
  _self->i0_174712 = i0_174712;
  _self->yy2 = yy2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_lift17293_map_indexed_peek_fun21648(kk_function_t _fself, kk_box_t _b_19620, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17293_map_indexed_peek_fun21648__t* _self = kk_function_as(struct kk_std_core__ctailm_lift17293_map_indexed_peek_fun21648__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<14302>) -> list<14302> */
  kk_function_t f2 = _self->f2; /* (idx : int, value : 14301, rest : list<14301>) -> 14303 14302 */
  kk_integer_t i0_174712 = _self->i0_174712; /* int */
  kk_std_core__list yy2 = _self->yy2; /* list<14301> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(f2);kk_integer_dup(i0_174712);kk_std_core__list_dup(yy2);}, {}, _ctx)
  kk_box_t _ctail_19622_175940 = _b_19620; /*14302*/;
  kk_std_core__list _x21649 = kk_std_core__mlift18048_op(_accm0, f2, i0_174712, yy2, _ctail_19622_175940, _ctx); /*list<14302>*/
  return kk_std_core__list_box(_x21649, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_lift17293_map_indexed_peek_fun21651__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t x0_18367;
};
static kk_std_core__list kk_std_core__ctailm_lift17293_map_indexed_peek_fun21651(kk_function_t _fself, kk_std_core__list _ctail_175930, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_lift17293_map_indexed_peek_fun21651(kk_function_t _accm0, kk_box_t x0_18367, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17293_map_indexed_peek_fun21651__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_lift17293_map_indexed_peek_fun21651__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_lift17293_map_indexed_peek_fun21651, kk_context());
  _self->_accm0 = _accm0;
  _self->x0_18367 = x0_18367;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_lift17293_map_indexed_peek_fun21651(kk_function_t _fself, kk_std_core__list _ctail_175930, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17293_map_indexed_peek_fun21651__t* _self = kk_function_as(struct kk_std_core__ctailm_lift17293_map_indexed_peek_fun21651__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<14302>) -> list<14302> */
  kk_box_t x0_18367 = _self->x0_18367; /* 14302 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(x0_18367);}, {}, _ctx)
  kk_std_core__list _x21652 = kk_std_core__new_Cons(kk_reuse_null, x0_18367, _ctail_175930, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x21652, _ctx));
}

kk_std_core__list kk_std_core__ctailm_lift17293_map_indexed_peek(kk_function_t f2, kk_std_core__list ys0, kk_integer_t i0, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,b,e> (f : (idx : int, value : a, rest : list<a>) -> e b, ys : list<a>, i : int, (list<b>) -> list<b>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys0)) {
    struct kk_std_core_Cons* _con21643 = kk_std_core__as_Cons(ys0);
    kk_box_t y0 = _con21643->head;
    kk_std_core__list yy2 = _con21643->tail;
    if (kk_likely(kk_std_core__list_is_unique(ys0))) {
      kk_std_core__list_free(ys0, _ctx);
    }
    else {
      kk_box_dup(y0);
      kk_std_core__list_dup(yy2);
      kk_std_core__list_decref(ys0, _ctx);
    }
    kk_integer_t i0_174712;
    kk_integer_t _x21644 = kk_integer_dup(i0); /*int*/
    i0_174712 = kk_integer_add_small_const(_x21644, 1, _ctx); /*int*/
    kk_box_t x0_18367;
    kk_function_t _x21646 = kk_function_dup(f2); /*(idx : int, value : 14301, rest : list<14301>) -> 14303 14302*/
    kk_std_core__list _x21645 = kk_std_core__list_dup(yy2); /*list<14301>*/
    x0_18367 = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_box_t, kk_std_core__list, kk_context_t*), _x21646, (_x21646, i0, y0, _x21645, _ctx)); /*14302*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x0_18367, _ctx);
      kk_box_t _x21647 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_lift17293_map_indexed_peek_fun21648(_accm0, f2, i0_174712, yy2, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x21647, _ctx);
    }
    { // tailcall
      kk_function_t _x21650 = kk_std_core__new_ctailm_lift17293_map_indexed_peek_fun21651(_accm0, x0_18367, _ctx); /*(list<14302>) -> list<14302>*/
      ys0 = yy2;
      i0 = i0_174712;
      _accm0 = _x21650;
      goto kk__tailcall;
    }
  }
  {
    kk_integer_drop(i0, _ctx);
    kk_function_drop(f2, _ctx);
    return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
  }
}
 
// lifted local: map-indexed-peek, mapidx


// lift anonymous function
struct kk_std_core__lift17293_map_indexed_peek_fun21654__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core__lift17293_map_indexed_peek_fun21654(kk_function_t _fself, kk_std_core__list _ctail_17592, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17293_map_indexed_peek_fun21654(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core__lift17293_map_indexed_peek_fun21654, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core__lift17293_map_indexed_peek_fun21654(kk_function_t _fself, kk_std_core__list _ctail_17592, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17592;
}

kk_std_core__list kk_std_core__lift17293_map_indexed_peek(kk_function_t f3, kk_std_core__list ys1, kk_integer_t i1, kk_context_t* _ctx) { /* forall<a,b,e> (f : (idx : int, value : a, rest : list<a>) -> e b, ys : list<a>, i : int) -> e list<b> */ 
  bool _match_20049 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_20049) {
    kk_std_core_types__ctail _x21653 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_lift17293_map_indexed_peek(f3, ys1, i1, _x21653, _ctx);
  }
  {
    return kk_std_core__ctailm_lift17293_map_indexed_peek(f3, ys1, i1, kk_std_core__new_lift17293_map_indexed_peek_fun21654(_ctx), _ctx);
  }
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift18049_op(kk_std_core_types__ctail _acc, kk_function_t action, kk_std_core__list xx, kk_std_core_types__maybe _y_17871, kk_context_t* _ctx) { /* forall<a,b,e> (ctail<list<b>>, action : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e list<b> */ 
  if (kk_std_core_types__is_Just(_y_17871)) {
    kk_box_t y = _y_17871._cons.Just.value;
    kk_std_core__list _ctail_17595 = kk_std_core__list_hole(); /*list<14344>*/;
    kk_std_core__list _ctail_17596 = kk_std_core__new_Cons(kk_reuse_null, y, _ctail_17595, _ctx); /*list<14344>*/;
    kk_box_t* _b_19633_19628 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17596)->tail)); /*cfield<list<14344>>*/;
    kk_std_core_types__ctail _x21655 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17596, _ctx)),_b_19633_19628); /*ctail<0>*/
    return kk_std_core__ctail_map_while(xx, action, _x21655, _ctx);
  }
  {
    kk_std_core__list_drop(xx, _ctx);
    kk_function_drop(action, _ctx);
    kk_box_t _x21656 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21656, _ctx);
  }
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift18050_op_fun21657__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t y0;
};
static kk_std_core__list kk_std_core__mlift18050_op_fun21657(kk_function_t _fself, kk_std_core__list _ctail_17598, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift18050_op_fun21657(kk_function_t _accm, kk_box_t y0, kk_context_t* _ctx) {
  struct kk_std_core__mlift18050_op_fun21657__t* _self = kk_function_alloc_as(struct kk_std_core__mlift18050_op_fun21657__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift18050_op_fun21657, kk_context());
  _self->_accm = _accm;
  _self->y0 = y0;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift18050_op_fun21657(kk_function_t _fself, kk_std_core__list _ctail_17598, kk_context_t* _ctx) {
  struct kk_std_core__mlift18050_op_fun21657__t* _self = kk_function_as(struct kk_std_core__mlift18050_op_fun21657__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<14344>) -> list<14344> */
  kk_box_t y0 = _self->y0; /* 14344 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(y0);}, {}, _ctx)
  kk_std_core__list _x21658 = kk_std_core__new_Cons(kk_reuse_null, y0, _ctail_17598, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x21658, _ctx));
}

kk_std_core__list kk_std_core__mlift18050_op(kk_function_t _accm, kk_function_t action0, kk_std_core__list xx0, kk_std_core_types__maybe _y_17875, kk_context_t* _ctx) { /* forall<a,b,e> ((list<b>) -> list<b>, action : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e list<b> */ 
  if (kk_std_core_types__is_Just(_y_17875)) {
    kk_box_t y0 = _y_17875._cons.Just.value;
    return kk_std_core__ctailm_map_while(xx0, action0, kk_std_core__new_mlift18050_op_fun21657(_accm, y0, _ctx), _ctx);
  }
  {
    kk_std_core__list_drop(xx0, _ctx);
    kk_function_drop(action0, _ctx);
    return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, kk_std_core__new_Nil(_ctx), _ctx));
  }
}
 
// Invoke `action` on each element of a list while `action` returns `Just`


// lift anonymous function
struct kk_std_core__ctail_map_while_fun21663__t {
  struct kk_function_s _base;
  kk_function_t action1;
  kk_std_core__list xx1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_map_while_fun21663(kk_function_t _fself, kk_box_t _b_19642, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_map_while_fun21663(kk_function_t action1, kk_std_core__list xx1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_map_while_fun21663__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_map_while_fun21663__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_map_while_fun21663, kk_context());
  _self->action1 = action1;
  _self->xx1 = xx1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_map_while_fun21663(kk_function_t _fself, kk_box_t _b_19642, kk_context_t* _ctx) {
  struct kk_std_core__ctail_map_while_fun21663__t* _self = kk_function_as(struct kk_std_core__ctail_map_while_fun21663__t*, _fself);
  kk_function_t action1 = _self->action1; /* (14343) -> 14345 maybe<14344> */
  kk_std_core__list xx1 = _self->xx1; /* list<14343> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<14344>> */
  kk_drop_match(_self, {kk_function_dup(action1);kk_std_core__list_dup(xx1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_std_core_types__maybe _y_19662_178710 = kk_std_core_types__maybe_unbox(_b_19642, _ctx); /*maybe<14344>*/;
  kk_std_core__list _x21664 = kk_std_core__mlift18049_op(_acc0, action1, xx1, _y_19662_178710, _ctx); /*list<14344>*/
  return kk_std_core__list_box(_x21664, _ctx);
}

kk_std_core__list kk_std_core__ctail_map_while(kk_std_core__list xs, kk_function_t action1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, action : (a) -> e maybe<b>, ctail<list<b>>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Nil(xs)) {
    kk_function_drop(action1, _ctx);
    kk_box_t _x21659 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21659, _ctx);
  }
  {
    struct kk_std_core_Cons* _con21660 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con21660->head;
    kk_std_core__list xx1 = _con21660->tail;
    kk_reuse_t _ru_20423 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      _ru_20423 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx1);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_std_core_types__maybe x0_18370;
    kk_function_t _x21661 = kk_function_dup(action1); /*(14343) -> 14345 maybe<14344>*/
    x0_18370 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_box_t, kk_context_t*), _x21661, (_x21661, x, _ctx)); /*maybe<14344>*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_20423,kk_context());
      kk_std_core_types__maybe_drop(x0_18370, _ctx);
      kk_box_t _x21662 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_map_while_fun21663(action1, xx1, _acc0, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x21662, _ctx);
    }
    if (kk_std_core_types__is_Just(x0_18370)) {
      kk_box_t y1 = x0_18370._cons.Just.value;
      kk_std_core__list _ctail_175950 = kk_std_core__list_hole(); /*list<14344>*/;
      kk_std_core__list _ctail_175960 = kk_std_core__new_Cons(_ru_20423, y1, _ctail_175950, _ctx); /*list<14344>*/;
      kk_box_t* _b_19656_19648 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_175960)->tail)); /*cfield<list<14344>>*/;
      { // tailcall
        kk_std_core_types__ctail _x21665 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_175960, _ctx)),_b_19656_19648); /*ctail<0>*/
        xs = xx1;
        _acc0 = _x21665;
        goto kk__tailcall;
      }
    }
    {
      kk_reuse_drop(_ru_20423,kk_context());
      kk_std_core__list_drop(xx1, _ctx);
      kk_function_drop(action1, _ctx);
      kk_box_t _x21666 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
      return kk_std_core__list_unbox(_x21666, _ctx);
    }
  }
}
 
// Invoke `action` on each element of a list while `action` returns `Just`


// lift anonymous function
struct kk_std_core__ctailm_map_while_fun21670__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t action2;
  kk_std_core__list xx2;
};
static kk_box_t kk_std_core__ctailm_map_while_fun21670(kk_function_t _fself, kk_box_t _b_19664, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_map_while_fun21670(kk_function_t _accm0, kk_function_t action2, kk_std_core__list xx2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_map_while_fun21670__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_map_while_fun21670__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_map_while_fun21670, kk_context());
  _self->_accm0 = _accm0;
  _self->action2 = action2;
  _self->xx2 = xx2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_map_while_fun21670(kk_function_t _fself, kk_box_t _b_19664, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_map_while_fun21670__t* _self = kk_function_as(struct kk_std_core__ctailm_map_while_fun21670__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<14344>) -> list<14344> */
  kk_function_t action2 = _self->action2; /* (14343) -> 14345 maybe<14344> */
  kk_std_core__list xx2 = _self->xx2; /* list<14343> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(action2);kk_std_core__list_dup(xx2);}, {}, _ctx)
  kk_std_core_types__maybe _y_19666_178750 = kk_std_core_types__maybe_unbox(_b_19664, _ctx); /*maybe<14344>*/;
  kk_std_core__list _x21671 = kk_std_core__mlift18050_op(_accm0, action2, xx2, _y_19666_178750, _ctx); /*list<14344>*/
  return kk_std_core__list_box(_x21671, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_map_while_fun21673__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t y2;
};
static kk_std_core__list kk_std_core__ctailm_map_while_fun21673(kk_function_t _fself, kk_std_core__list _ctail_175980, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_map_while_fun21673(kk_function_t _accm0, kk_box_t y2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_map_while_fun21673__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_map_while_fun21673__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_map_while_fun21673, kk_context());
  _self->_accm0 = _accm0;
  _self->y2 = y2;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_map_while_fun21673(kk_function_t _fself, kk_std_core__list _ctail_175980, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_map_while_fun21673__t* _self = kk_function_as(struct kk_std_core__ctailm_map_while_fun21673__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<14344>) -> list<14344> */
  kk_box_t y2 = _self->y2; /* 14344 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(y2);}, {}, _ctx)
  kk_std_core__list _x21674 = kk_std_core__new_Cons(kk_reuse_null, y2, _ctail_175980, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x21674, _ctx));
}

kk_std_core__list kk_std_core__ctailm_map_while(kk_std_core__list xs0, kk_function_t action2, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, action : (a) -> e maybe<b>, (list<b>) -> list<b>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Nil(xs0)) {
    kk_function_drop(action2, _ctx);
    return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
  }
  {
    struct kk_std_core_Cons* _con21667 = kk_std_core__as_Cons(xs0);
    kk_box_t x1 = _con21667->head;
    kk_std_core__list xx2 = _con21667->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs0))) {
      kk_std_core__list_free(xs0, _ctx);
    }
    else {
      kk_box_dup(x1);
      kk_std_core__list_dup(xx2);
      kk_std_core__list_decref(xs0, _ctx);
    }
    kk_std_core_types__maybe x2_18373;
    kk_function_t _x21668 = kk_function_dup(action2); /*(14343) -> 14345 maybe<14344>*/
    x2_18373 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_box_t, kk_context_t*), _x21668, (_x21668, x1, _ctx)); /*maybe<14344>*/
    if (kk_yielding(kk_context())) {
      kk_std_core_types__maybe_drop(x2_18373, _ctx);
      kk_box_t _x21669 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_map_while_fun21670(_accm0, action2, xx2, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x21669, _ctx);
    }
    if (kk_std_core_types__is_Just(x2_18373)) {
      kk_box_t y2 = x2_18373._cons.Just.value;
      { // tailcall
        kk_function_t _x21672 = kk_std_core__new_ctailm_map_while_fun21673(_accm0, y2, _ctx); /*(list<14344>) -> list<14344>*/
        xs0 = xx2;
        _accm0 = _x21672;
        goto kk__tailcall;
      }
    }
    {
      kk_std_core__list_drop(xx2, _ctx);
      kk_function_drop(action2, _ctx);
      return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
    }
  }
}
 
// Invoke `action` on each element of a list while `action` returns `Just`


// lift anonymous function
struct kk_std_core_map_while_fun21676__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core_map_while_fun21676(kk_function_t _fself, kk_std_core__list _ctail_17597, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_while_fun21676(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_map_while_fun21676, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core_map_while_fun21676(kk_function_t _fself, kk_std_core__list _ctail_17597, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17597;
}

kk_std_core__list kk_std_core_map_while(kk_std_core__list xs1, kk_function_t action3, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, action : (a) -> e maybe<b>) -> e list<b> */ 
  bool _match_20046 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_20046) {
    kk_std_core_types__ctail _x21675 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_map_while(xs1, action3, _x21675, _ctx);
  }
  {
    return kk_std_core__ctailm_map_while(xs1, action3, kk_std_core_new_map_while_fun21676(_ctx), _ctx);
  }
}
 
// lifted local: maximum, .spec.17432
// specialized: std/core/foldl, on parameters f, using:
// f = std/core/max

kk_integer_t kk_std_core__lift17493_maximum(kk_std_core__list xs0, kk_integer_t z, kk_context_t* _ctx) { /* (xs0 : list<int>, z : int) -> int */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con21677 = kk_std_core__as_Cons(xs0);
    kk_box_t _box_x19667 = _con21677->head;
    kk_std_core__list xx0 = _con21677->tail;
    kk_integer_t x0 = kk_integer_unbox(_box_x19667);
    if (kk_likely(kk_std_core__list_is_unique(xs0))) {
      kk_std_core__list_free(xs0, _ctx);
    }
    else {
      kk_integer_dup(x0);
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(xs0, _ctx);
    }
    { // tailcall
      kk_integer_t _x21679;
      bool _match_20043 = kk_integer_gte_borrow(z,x0,kk_context()); /*bool*/;
      if (_match_20043) {
        kk_integer_drop(x0, _ctx);
        _x21679 = z; /*int*/
      }
      else {
        kk_integer_drop(z, _ctx);
        _x21679 = x0; /*int*/
      }
      xs0 = xx0;
      z = _x21679;
      goto kk__tailcall;
    }
  }
  {
    return z;
  }
}
 
// Returns the largest element of a list of integers (or `default` (=`0`) for the empty list)

kk_integer_t kk_std_core_maximum(kk_std_core__list xs, kk_std_core_types__optional default0, kk_context_t* _ctx) { /* (xs : list<int>, default : optional<int>) -> int */ 
  if (kk_std_core__is_Nil(xs)) {
    if (kk_std_core_types__is_Optional(default0)) {
      kk_box_t _box_x19668 = default0._cons.Optional.value;
      kk_integer_t _default_14393 = kk_integer_unbox(_box_x19668);
      return _default_14393;
    }
    {
      return kk_integer_from_small(0);
    }
  }
  {
    struct kk_std_core_Cons* _con21681 = kk_std_core__as_Cons(xs);
    kk_box_t _box_x19669 = _con21681->head;
    kk_std_core__list xx = _con21681->tail;
    kk_integer_t x = kk_integer_unbox(_box_x19669);
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_integer_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_std_core_types__optional_drop(default0, _ctx);
    return kk_std_core__lift17493_maximum(xx, x, _ctx);
  }
}
 
// lifted local: maximum.1, .spec.17433
// specialized: std/core/foldl, on parameters f, using:
// f = std/core/max.1

double kk_std_core__lift17494_maximum_1(kk_std_core__list xs0, double z, kk_context_t* _ctx) { /* (xs0 : list<float64>, z : float64) -> float64 */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con21683 = kk_std_core__as_Cons(xs0);
    kk_box_t _box_x19670 = _con21683->head;
    kk_std_core__list xx0 = _con21683->tail;
    double x0 = kk_double_unbox(_box_x19670, NULL);
    if (kk_likely(kk_std_core__list_is_unique(xs0))) {
      kk_box_drop(_box_x19670, _ctx);
      kk_std_core__list_free(xs0, _ctx);
    }
    else {
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(xs0, _ctx);
    }
    { // tailcall
      double _x21685;
      bool _match_20042 = (z >= x0); /*bool*/;
      if (_match_20042) {
        _x21685 = z; /*float64*/
      }
      else {
        _x21685 = x0; /*float64*/
      }
      xs0 = xx0;
      z = _x21685;
      goto kk__tailcall;
    }
  }
  {
    return z;
  }
}
 
// Returns the largest element of a list of floats (or `0` for the empty list)

double kk_std_core_maximum_1(kk_std_core__list xs, kk_context_t* _ctx) { /* (xs : list<float64>) -> float64 */ 
  if (kk_std_core__is_Nil(xs)) {
    return 0.0;
  }
  {
    struct kk_std_core_Cons* _con21686 = kk_std_core__as_Cons(xs);
    kk_box_t _box_x19671 = _con21686->head;
    kk_std_core__list xx = _con21686->tail;
    double x = kk_double_unbox(_box_x19671, NULL);
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_box_drop(_box_x19671, _ctx);
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    return kk_std_core__lift17494_maximum_1(xx, x, _ctx);
  }
}
 
// Convert a list to a `:maybe` type, using `Nothing` for an empty list, and otherwise `Just` on the head element.
// Note: this is just `head`.

kk_std_core_types__maybe kk_std_core_maybe_3(kk_std_core__list xs, kk_context_t* _ctx) { /* forall<a> (xs : list<a>) -> maybe<a> */ 
  if (kk_std_core__is_Nil(xs)) {
    return kk_std_core_types__new_Nothing(_ctx);
  }
  {
    struct kk_std_core_Cons* _con21688 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con21688->head;
    kk_std_core__list _pat1 = _con21688->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_drop(_pat1, _ctx);
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_decref(xs, _ctx);
    }
    return kk_std_core_types__new_Just(x, _ctx);
  }
}
 
// Transform a string to a maybe type, using `Nothing` for an empty string

kk_std_core_types__maybe kk_std_core_maybe_6(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> maybe<string> */ 
  bool _match_20040;
  kk_string_t _x21689 = kk_string_dup(s); /*string*/
  kk_string_t _x21690 = kk_string_empty(); /*string*/
  _match_20040 = kk_string_is_eq(_x21689,_x21690,kk_context()); /*bool*/
  if (_match_20040) {
    kk_string_drop(s, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  {
    return kk_std_core_types__new_Just(kk_string_box(s), _ctx);
  }
}
 
// Transform a `:null` type to a `:maybe` type. Note that it is not
// always the case that `id(x) == maybe(null(x))` (e.g. when `x = Just(Nothing)`).

kk_std_core_types__maybe kk_std_core_maybe_8(kk_std_core__null n, kk_context_t* _ctx) { /* forall<a> (n : null<a>) -> maybe<a> */ 
  return (kk_datatype_as_ptr(n) == NULL ? kk_std_core_types__new_Nothing(kk_context()) : kk_std_core_types__new_Just(kk_datatype_box(n),kk_context()));
}
 
// lifted local: minimum, .spec.17434
// specialized: std/core/foldl, on parameters f, using:
// f = std/core/min

kk_integer_t kk_std_core__lift17495_minimum(kk_std_core__list xs0, kk_integer_t z, kk_context_t* _ctx) { /* (xs0 : list<int>, z : int) -> int */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con21692 = kk_std_core__as_Cons(xs0);
    kk_box_t _box_x19678 = _con21692->head;
    kk_std_core__list xx0 = _con21692->tail;
    kk_integer_t x0 = kk_integer_unbox(_box_x19678);
    if (kk_likely(kk_std_core__list_is_unique(xs0))) {
      kk_std_core__list_free(xs0, _ctx);
    }
    else {
      kk_integer_dup(x0);
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(xs0, _ctx);
    }
    { // tailcall
      kk_integer_t _x21694;
      bool _match_20037 = kk_integer_lte_borrow(z,x0,kk_context()); /*bool*/;
      if (_match_20037) {
        kk_integer_drop(x0, _ctx);
        _x21694 = z; /*int*/
      }
      else {
        kk_integer_drop(z, _ctx);
        _x21694 = x0; /*int*/
      }
      xs0 = xx0;
      z = _x21694;
      goto kk__tailcall;
    }
  }
  {
    return z;
  }
}
 
// Returns the smallest element of a list of integers (or `default` (=`0`) for the empty list)

kk_integer_t kk_std_core_minimum(kk_std_core__list xs, kk_std_core_types__optional default0, kk_context_t* _ctx) { /* (xs : list<int>, default : optional<int>) -> int */ 
  if (kk_std_core__is_Nil(xs)) {
    if (kk_std_core_types__is_Optional(default0)) {
      kk_box_t _box_x19679 = default0._cons.Optional.value;
      kk_integer_t _default_14872 = kk_integer_unbox(_box_x19679);
      return _default_14872;
    }
    {
      return kk_integer_from_small(0);
    }
  }
  {
    struct kk_std_core_Cons* _con21696 = kk_std_core__as_Cons(xs);
    kk_box_t _box_x19680 = _con21696->head;
    kk_std_core__list xx = _con21696->tail;
    kk_integer_t x = kk_integer_unbox(_box_x19680);
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_integer_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_std_core_types__optional_drop(default0, _ctx);
    return kk_std_core__lift17495_minimum(xx, x, _ctx);
  }
}
 
// lifted local: minimum.1, .spec.17435
// specialized: std/core/foldl, on parameters f, using:
// f = std/core/min.1

double kk_std_core__lift17496_minimum_1(kk_std_core__list xs0, double z, kk_context_t* _ctx) { /* (xs0 : list<float64>, z : float64) -> float64 */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con21698 = kk_std_core__as_Cons(xs0);
    kk_box_t _box_x19681 = _con21698->head;
    kk_std_core__list xx0 = _con21698->tail;
    double x0 = kk_double_unbox(_box_x19681, NULL);
    if (kk_likely(kk_std_core__list_is_unique(xs0))) {
      kk_box_drop(_box_x19681, _ctx);
      kk_std_core__list_free(xs0, _ctx);
    }
    else {
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(xs0, _ctx);
    }
    { // tailcall
      double _x21700;
      bool _match_20036 = (z <= x0); /*bool*/;
      if (_match_20036) {
        _x21700 = z; /*float64*/
      }
      else {
        _x21700 = x0; /*float64*/
      }
      xs0 = xx0;
      z = _x21700;
      goto kk__tailcall;
    }
  }
  {
    return z;
  }
}
 
// Returns the smallest element of a list of floats (or `0` for the empty list)

double kk_std_core_minimum_1(kk_std_core__list xs, kk_context_t* _ctx) { /* (xs : list<float64>) -> float64 */ 
  if (kk_std_core__is_Nil(xs)) {
    return 0.0;
  }
  {
    struct kk_std_core_Cons* _con21701 = kk_std_core__as_Cons(xs);
    kk_box_t _box_x19682 = _con21701->head;
    kk_std_core__list xx = _con21701->tail;
    double x = kk_double_unbox(_box_x19682, NULL);
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_box_drop(_box_x19682, _ctx);
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    return kk_std_core__lift17496_minimum_1(xx, x, _ctx);
  }
}
 
// Disable tracing completely.

kk_unit_t kk_std_core_notrace(kk_context_t* _ctx) { /* () -> (st<global>) () */ 
  kk_ref_set_borrow(kk_std_core_trace_enabled,(kk_bool_box(false)),kk_context()); return kk_Unit;
}
 
// Transform a `:maybe` type to a `:null` type (using `null` for `Nothing`).

kk_std_core__null kk_std_core_null(kk_std_core_types__maybe x, kk_context_t* _ctx) { /* forall<a> (x : maybe<a>) -> null<a> */ 
  return (kk_std_core_types__is_Nothing(x) ? kk_datatype_from_ptr(NULL) : kk_datatype_unbox((x)._cons.Just.value) /* kk_datatype_unbox(kk_datatype_unjust(x,kk_context())) */ );
}
 
// Cast a integer that is zero to a null

kk_std_core__null kk_std_core_null_1(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> null<int> */ 
  kk_std_core_types__maybe _x21703;
  bool _match_20035 = kk_integer_eq_borrow(i,(kk_integer_from_small(0)),kk_context()); /*bool*/;
  if (_match_20035) {
    kk_integer_drop(i, _ctx);
    _x21703 = kk_std_core_types__new_Nothing(_ctx); /*forall<a> maybe<a>*/
  }
  else {
    _x21703 = kk_std_core_types__new_Just(kk_integer_box(i), _ctx); /*forall<a> maybe<a>*/
  }
  return kk_std_core_null(_x21703, _ctx);
}
 
// Cast an empty string a null

kk_std_core__null kk_std_core_null_2(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> null<string> */ 
  kk_std_core_types__maybe _x21704;
  bool _match_20034;
  kk_string_t _x21705 = kk_string_dup(s); /*string*/
  kk_string_t _x21706 = kk_string_empty(); /*string*/
  _match_20034 = kk_string_is_eq(_x21705,_x21706,kk_context()); /*bool*/
  if (_match_20034) {
    kk_string_drop(s, _ctx);
    _x21704 = kk_std_core_types__new_Nothing(_ctx); /*forall<a> maybe<a>*/
  }
  else {
    _x21704 = kk_std_core_types__new_Just(kk_string_box(s), _ctx); /*forall<a> maybe<a>*/
  }
  return kk_std_core_null(_x21704, _ctx);
}
 
// Left-align a string to width `width`  using `fill`  (default is a space) to fill on the right.

kk_string_t kk_std_core_pad_right(kk_string_t s, kk_integer_t width, kk_std_core_types__optional fill, kk_context_t* _ctx) { /* (s : string, width : int, fill : optional<char>) -> string */ 
  kk_ssize_t w;
  kk_integer_t _x21709 = kk_integer_dup(width); /*int*/
  w = kk_std_core_ssize__t(_x21709, _ctx); /*ssize_t*/
  kk_ssize_t n;
  kk_string_t _x21710 = kk_string_dup(s); /*string*/
  n = kk_string_len(_x21710,kk_context()); /*ssize_t*/
  bool _match_20033 = (w <= n); /*bool*/;
  if (_match_20033) {
    kk_std_core_types__optional_drop(fill, _ctx);
    return s;
  }
  {
    kk_string_t _x21711;
    kk_string_t _x21712;
    kk_char_t _x21713;
    if (kk_std_core_types__is_Optional(fill)) {
      kk_box_t _box_x19693 = fill._cons.Optional.value;
      kk_char_t _fill_15274 = kk_char_unbox(_box_x19693, NULL);
      _x21713 = _fill_15274; /*char*/
    }
    else {
      _x21713 = ' '; /*char*/
    }
    _x21712 = kk_std_core_string(_x21713, _ctx); /*string*/
    kk_ssize_t _x21715 = (w - n); /*ssize_t*/
    _x21711 = kk_std_core_repeatz(_x21712, _x21715, _ctx); /*string*/
    return kk_std_core__lp__plus__plus__1_rp_(s, _x21711, _ctx);
  }
}
 
// Is `pre`  a prefix of `s`? If so, returns a slice
// of `s` following `pre` up to the end of `s`.

kk_std_core_types__maybe kk_std_core_starts_with(kk_string_t s, kk_string_t pre, kk_context_t* _ctx) { /* (s : string, pre : string) -> maybe<sslice> */ 
  bool _match_20032;
  kk_string_t _x21716 = kk_string_dup(s); /*string*/
  kk_string_t _x21717 = kk_string_dup(pre); /*string*/
  _match_20032 = kk_string_starts_with(_x21716,_x21717,kk_context()); /*bool*/
  if (_match_20032) {
    kk_std_core__sslice _b_19695_19694;
    kk_string_t _x21718 = kk_string_dup(s); /*string*/
    kk_ssize_t _x21719;
    kk_string_t _x21720 = kk_string_dup(pre); /*string*/
    _x21719 = kk_string_len(_x21720,kk_context()); /*ssize_t*/
    kk_ssize_t _x21721;
    kk_ssize_t _x21722 = kk_string_len(s,kk_context()); /*ssize_t*/
    kk_ssize_t _x21723 = kk_string_len(pre,kk_context()); /*ssize_t*/
    _x21721 = (_x21722 - _x21723); /*ssize_t*/
    _b_19695_19694 = kk_std_core__new_Sslice(_x21718, _x21719, _x21721, _ctx); /*sslice*/
    return kk_std_core_types__new_Just(kk_std_core__sslice_box(_b_19695_19694, _ctx), _ctx);
  }
  {
    kk_string_drop(s, _ctx);
    kk_string_drop(pre, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
}
 
// Trim off a substring `sub` while `s` starts with that string.

kk_string_t kk_std_core_trim_left_1(kk_string_t s, kk_string_t sub, kk_context_t* _ctx) { /* (s : string, sub : string) -> string */ 
  kk__tailcall: ;
  bool _match_20030;
  kk_string_t _x21724 = kk_string_dup(sub); /*string*/
  kk_string_t _x21725 = kk_string_empty(); /*string*/
  _match_20030 = kk_string_is_eq(_x21724,_x21725,kk_context()); /*bool*/
  if (_match_20030) {
    kk_string_drop(sub, _ctx);
    return s;
  }
  {
    kk_std_core_types__maybe _match_20031;
    kk_string_t _x21727 = kk_string_dup(s); /*string*/
    kk_string_t _x21728 = kk_string_dup(sub); /*string*/
    _match_20031 = kk_std_core_starts_with(_x21727, _x21728, _ctx); /*maybe<sslice>*/
    if (kk_std_core_types__is_Just(_match_20031)) {
      kk_box_t _box_x19696 = _match_20031._cons.Just.value;
      kk_std_core__sslice slice0 = kk_std_core__sslice_unbox(_box_x19696, NULL);
      kk_string_drop(s, _ctx);
      kk_std_core__sslice_dup(slice0);
      kk_std_core_types__maybe_drop(_match_20031, _ctx);
      { // tailcall
        kk_string_t _x21730 = kk_std_core_string_3(slice0, _ctx); /*string*/
        s = _x21730;
        goto kk__tailcall;
      }
    }
    {
      kk_string_drop(sub, _ctx);
      return s;
    }
  }
}
 
// Trim off a substring `sub` while `s` ends with that string.

kk_string_t kk_std_core_trim_right_1(kk_string_t s, kk_string_t sub, kk_context_t* _ctx) { /* (s : string, sub : string) -> string */ 
  kk__tailcall: ;
  bool _match_20028;
  kk_string_t _x21731 = kk_string_dup(sub); /*string*/
  kk_string_t _x21732 = kk_string_empty(); /*string*/
  _match_20028 = kk_string_is_eq(_x21731,_x21732,kk_context()); /*bool*/
  if (_match_20028) {
    kk_string_drop(sub, _ctx);
    return s;
  }
  {
    kk_std_core_types__maybe _match_20029;
    kk_string_t _x21734 = kk_string_dup(s); /*string*/
    kk_string_t _x21735 = kk_string_dup(sub); /*string*/
    _match_20029 = kk_std_core_ends_with(_x21734, _x21735, _ctx); /*maybe<sslice>*/
    if (kk_std_core_types__is_Just(_match_20029)) {
      kk_box_t _box_x19697 = _match_20029._cons.Just.value;
      kk_std_core__sslice slice0 = kk_std_core__sslice_unbox(_box_x19697, NULL);
      kk_string_drop(s, _ctx);
      kk_std_core__sslice_dup(slice0);
      kk_std_core_types__maybe_drop(_match_20029, _ctx);
      { // tailcall
        kk_string_t _x21737 = kk_std_core_string_3(slice0, _ctx); /*string*/
        s = _x21737;
        goto kk__tailcall;
      }
    }
    {
      kk_string_drop(sub, _ctx);
      return s;
    }
  }
}
 
// Parse an integer using `parseInt`. If an illegal digit character is encountered the
// `default` value is returned. An empty string will also result in `default`.

kk_integer_t kk_std_core_parse_int_default(kk_string_t s, kk_std_core_types__optional default0, kk_std_core_types__optional hex, kk_context_t* _ctx) { /* (s : string, default : optional<int>, hex : optional<bool>) -> int */ 
  bool _match_20027;
  kk_string_t _x21743 = kk_string_dup(s); /*string*/
  kk_string_t _x21744 = kk_string_empty(); /*string*/
  _match_20027 = kk_string_is_eq(_x21743,_x21744,kk_context()); /*bool*/
  if (_match_20027) {
    kk_string_drop(s, _ctx);
    kk_std_core_types__optional_drop(hex, _ctx);
    if (kk_std_core_types__is_Optional(default0)) {
      kk_box_t _box_x19699 = default0._cons.Optional.value;
      kk_integer_t _default_15556 = kk_integer_unbox(_box_x19699);
      return _default_15556;
    }
    {
      return kk_integer_from_small(0);
    }
  }
  {
    kk_std_core_types__maybe m_17212;
    kk_string_t _x21747;
    kk_string_t _x21748 = kk_string_trim_left(s,kk_context()); /*string*/
    _x21747 = kk_string_trim_right(_x21748,kk_context()); /*string*/
    bool _x21749;
    if (kk_std_core_types__is_Optional(hex)) {
      kk_box_t _box_x19700 = hex._cons.Optional.value;
      bool _hex_15560 = kk_bool_unbox(_box_x19700);
      _x21749 = _hex_15560; /*bool*/
    }
    else {
      _x21749 = false; /*bool*/
    }
    m_17212 = kk_std_core_xparse_int(_x21747, _x21749, _ctx); /*maybe<int>*/
    if (kk_std_core_types__is_Nothing(m_17212)) {
      if (kk_std_core_types__is_Optional(default0)) {
        kk_box_t _box_x19701 = default0._cons.Optional.value;
        kk_integer_t _default_155560 = kk_integer_unbox(_box_x19701);
        return _default_155560;
      }
      {
        return kk_integer_from_small(0);
      }
    }
    {
      kk_box_t _box_x19702 = m_17212._cons.Just.value;
      kk_integer_t x = kk_integer_unbox(_box_x19702);
      kk_std_core_types__optional_drop(default0, _ctx);
      return x;
    }
  }
}
 
// monadic lift

kk_std_core_types__tuple2_ kk_std_core__mlift18051_partition_acc(kk_std_core__list acc1, kk_std_core__list acc2, kk_function_t pred, kk_box_t x, kk_std_core__list xx, bool _y_17884, kk_context_t* _ctx) { /* forall<a,e> (acc1 : list<a>, acc2 : list<a>, pred : (a) -> e bool, x : a, xx : list<a>, bool) -> e (list<a>, list<a>) */ 
  if (_y_17884) {
    kk_std_core__list _x21753 = kk_std_core__new_Cons(kk_reuse_null, x, acc1, _ctx); /*list<61>*/
    return kk_std_core_partition_acc(xx, pred, _x21753, acc2, _ctx);
  }
  {
    kk_std_core__list _x21754 = kk_std_core__new_Cons(kk_reuse_null, x, acc2, _ctx); /*list<61>*/
    return kk_std_core_partition_acc(xx, pred, acc1, _x21754, _ctx);
  }
}


// lift anonymous function
struct kk_std_core_partition_acc_fun21759__t {
  struct kk_function_s _base;
  kk_std_core__list acc10;
  kk_std_core__list acc20;
  kk_function_t pred0;
  kk_box_t x0;
  kk_std_core__list xx0;
};
static kk_box_t kk_std_core_partition_acc_fun21759(kk_function_t _fself, kk_box_t _b_19706, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_partition_acc_fun21759(kk_std_core__list acc10, kk_std_core__list acc20, kk_function_t pred0, kk_box_t x0, kk_std_core__list xx0, kk_context_t* _ctx) {
  struct kk_std_core_partition_acc_fun21759__t* _self = kk_function_alloc_as(struct kk_std_core_partition_acc_fun21759__t, 6, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_partition_acc_fun21759, kk_context());
  _self->acc10 = acc10;
  _self->acc20 = acc20;
  _self->pred0 = pred0;
  _self->x0 = x0;
  _self->xx0 = xx0;
  return &_self->_base;
}

static kk_box_t kk_std_core_partition_acc_fun21759(kk_function_t _fself, kk_box_t _b_19706, kk_context_t* _ctx) {
  struct kk_std_core_partition_acc_fun21759__t* _self = kk_function_as(struct kk_std_core_partition_acc_fun21759__t*, _fself);
  kk_std_core__list acc10 = _self->acc10; /* list<15678> */
  kk_std_core__list acc20 = _self->acc20; /* list<15678> */
  kk_function_t pred0 = _self->pred0; /* (15678) -> 15679 bool */
  kk_box_t x0 = _self->x0; /* 15678 */
  kk_std_core__list xx0 = _self->xx0; /* list<15678> */
  kk_drop_match(_self, {kk_std_core__list_dup(acc10);kk_std_core__list_dup(acc20);kk_function_dup(pred0);kk_box_dup(x0);kk_std_core__list_dup(xx0);}, {}, _ctx)
  bool _y_19710_178840 = kk_bool_unbox(_b_19706); /*bool*/;
  kk_std_core_types__tuple2_ _x21760 = kk_std_core__mlift18051_partition_acc(acc10, acc20, pred0, x0, xx0, _y_19710_178840, _ctx); /*(list<15678>, list<15678>)*/
  return kk_std_core_types__tuple2__box(_x21760, _ctx);
}

kk_std_core_types__tuple2_ kk_std_core_partition_acc(kk_std_core__list xs, kk_function_t pred0, kk_std_core__list acc10, kk_std_core__list acc20, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, pred : (a) -> e bool, acc1 : list<a>, acc2 : list<a>) -> e (list<a>, list<a>) */ 
  kk__tailcall: ;
  if (kk_std_core__is_Nil(xs)) {
    kk_function_drop(pred0, _ctx);
    kk_std_core__list _b_19707_19703 = kk_std_core__lift17288_reverse(kk_std_core__new_Nil(_ctx), acc10, _ctx); /*list<15678>*/;
    kk_std_core__list _b_19708_19704 = kk_std_core__lift17288_reverse(kk_std_core__new_Nil(_ctx), acc20, _ctx); /*list<15678>*/;
    return kk_std_core_types__new_dash__lp__comma__rp_(kk_std_core__list_box(_b_19707_19703, _ctx), kk_std_core__list_box(_b_19708_19704, _ctx), _ctx);
  }
  {
    struct kk_std_core_Cons* _con21755 = kk_std_core__as_Cons(xs);
    kk_box_t x0 = _con21755->head;
    kk_std_core__list xx0 = _con21755->tail;
    kk_reuse_t _ru_20434 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      _ru_20434 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_box_dup(x0);
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(xs, _ctx);
    }
    bool x1_18386;
    kk_function_t _x21757 = kk_function_dup(pred0); /*(15678) -> 15679 bool*/
    kk_box_t _x21756 = kk_box_dup(x0); /*15678*/
    x1_18386 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x21757, (_x21757, _x21756, _ctx)); /*bool*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_20434,kk_context());
      kk_box_t _x21758 = kk_std_core_hnd_yield_extend(kk_std_core_new_partition_acc_fun21759(acc10, acc20, pred0, x0, xx0, _ctx), _ctx); /*3926*/
      return kk_std_core_types__tuple2__unbox(_x21758, _ctx);
    }
    if (x1_18386) { // tailcall
                    kk_std_core__list _x21761 = kk_std_core__new_Cons(_ru_20434, x0, acc10, _ctx); /*list<61>*/
                    xs = xx0;
                    acc10 = _x21761;
                    goto kk__tailcall;
    }
    { // tailcall
      kk_std_core__list _x21762 = kk_std_core__new_Cons(_ru_20434, x0, acc20, _ctx); /*list<61>*/
      xs = xx0;
      acc20 = _x21762;
      goto kk__tailcall;
    }
  }
}
 
// redirect `print` and `println` calls to a specified function.


// lift anonymous function
struct kk_std_core_print_redirect_fun21765__t {
  struct kk_function_s _base;
  kk_function_t print0;
};
static kk_box_t kk_std_core_print_redirect_fun21765(kk_function_t _fself, kk_box_t _b_19714, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_print_redirect_fun21765(kk_function_t print0, kk_context_t* _ctx) {
  struct kk_std_core_print_redirect_fun21765__t* _self = kk_function_alloc_as(struct kk_std_core_print_redirect_fun21765__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_print_redirect_fun21765, kk_context());
  _self->print0 = print0;
  return &_self->_base;
}

static kk_box_t kk_std_core_print_redirect_fun21765(kk_function_t _fself, kk_box_t _b_19714, kk_context_t* _ctx) {
  struct kk_std_core_print_redirect_fun21765__t* _self = kk_function_as(struct kk_std_core_print_redirect_fun21765__t*, _fself);
  kk_function_t print0 = _self->print0; /* (msg : string) -> console () */
  kk_drop_match(_self, {kk_function_dup(print0);}, {}, _ctx)
  kk_unit_t _x21766 = kk_Unit;
  kk_string_t _x21767 = kk_string_unbox(_b_19714); /*string*/
  kk_function_call(kk_unit_t, (kk_function_t, kk_string_t, kk_context_t*), print0, (print0, _x21767, _ctx));
  return kk_unit_box(_x21766);
}

kk_unit_t kk_std_core_print_redirect(kk_function_t print0, kk_context_t* _ctx) { /* (print : (msg : string) -> console ()) -> io () */ 
  kk_box_t _x21763;
  kk_std_core_types__maybe _x21764 = kk_std_core_types__new_Just(kk_function_box(kk_std_core_new_print_redirect_fun21765(print0, _ctx)), _ctx); /*maybe<1034>*/
  _x21763 = kk_std_core_types__maybe_box(_x21764, _ctx); /*1000*/
  kk_ref_set_borrow(kk_std_core_redirect,_x21763,kk_context()); return kk_Unit;
}
 
// lifted local: remove, .spec.17436
// specialized: std/core/filter, on parameters pred, using:
// pred = fn(x: 15747){
//   val b.17219 : bool
//         = pred(x);
//   match (b.17219) {
//     ((std/core/types/True() : bool ) as .pat: bool)
//        -> std/core/types/False;
//     ((.skip std/core/types/False() : bool ) as .pat0: bool)
//        -> std/core/types/True;
//   };
// }

kk_std_core__list kk_std_core__ctail_lift17497_remove(kk_function_t pred, kk_std_core__list xs0, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* forall<a> (pred : (a) -> bool, xs0 : list<a>, ctail<list<a>>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con21768 = kk_std_core__as_Cons(xs0);
    kk_box_t x0 = _con21768->head;
    kk_std_core__list xx = _con21768->tail;
    kk_reuse_t _ru_20435 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(xs0))) {
      _ru_20435 = (kk_std_core__list_reuse(xs0));
    }
    else {
      kk_box_dup(x0);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs0, _ctx);
    }
    bool b_17219;
    kk_function_t _x21770 = kk_function_dup(pred); /*(15747) -> bool*/
    kk_box_t _x21769 = kk_box_dup(x0); /*15747*/
    b_17219 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x21770, (_x21770, _x21769, _ctx)); /*bool*/
    if (b_17219) {
      kk_reuse_drop(_ru_20435,kk_context());
      kk_box_drop(x0, _ctx);
      { // tailcall
        xs0 = xx;
        goto kk__tailcall;
      }
    }
    {
      kk_std_core__list _ctail_17599 = kk_std_core__list_hole(); /*list<15747>*/;
      kk_std_core__list _ctail_17600 = kk_std_core__new_Cons(_ru_20435, x0, _ctail_17599, _ctx); /*list<15747>*/;
      kk_box_t* _b_19730_19725 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17600)->tail)); /*cfield<list<15747>>*/;
      { // tailcall
        kk_std_core_types__ctail _x21771 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17600, _ctx)),_b_19730_19725); /*ctail<0>*/
        xs0 = xx;
        _acc = _x21771;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_function_drop(pred, _ctx);
    kk_box_t _x21772 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21772, _ctx);
  }
}
 
// lifted local: remove, .spec.17436
// specialized: std/core/filter, on parameters pred, using:
// pred = fn(x: 15747){
//   val b.17219 : bool
//         = pred(x);
//   match (b.17219) {
//     ((std/core/types/True() : bool ) as .pat: bool)
//        -> std/core/types/False;
//     ((.skip std/core/types/False() : bool ) as .pat0: bool)
//        -> std/core/types/True;
//   };
// }

kk_std_core__list kk_std_core__lift17497_remove(kk_function_t pred0, kk_std_core__list xs00, kk_context_t* _ctx) { /* forall<a> (pred : (a) -> bool, xs0 : list<a>) -> list<a> */ 
  kk_std_core_types__ctail _x21773 = kk_ctail_nil(); /*ctail<0>*/
  return kk_std_core__ctail_lift17497_remove(pred0, xs00, _x21773, _ctx);
}
 
// monadic lift

kk_unit_t kk_std_core__mlift18052_op(kk_function_t action, kk_integer_t i, kk_integer_t n, kk_unit_t wild__, kk_context_t* _ctx) { /* forall<e> (action : () -> e (), i : int, n : int, wild_ : ()) -> e () */ 
  kk_integer_t i0_17472 = kk_integer_add_small_const(i, 1, _ctx); /*int*/;
  kk_std_core__lift17294_repeat_1(action, n, i0_17472, _ctx); return kk_Unit;
}
 
// lifted local: repeat.1, rep


// lift anonymous function
struct kk_std_core__lift17294_repeat_fun21778__t_1 {
  struct kk_function_s _base;
  kk_function_t action0;
  kk_integer_t i0;
  kk_integer_t n0;
};
static kk_box_t kk_std_core__lift17294_repeat_fun21778_1(kk_function_t _fself, kk_box_t _b_19737, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17294_repeat_fun21778_1(kk_function_t action0, kk_integer_t i0, kk_integer_t n0, kk_context_t* _ctx) {
  struct kk_std_core__lift17294_repeat_fun21778__t_1* _self = kk_function_alloc_as(struct kk_std_core__lift17294_repeat_fun21778__t_1, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift17294_repeat_fun21778_1, kk_context());
  _self->action0 = action0;
  _self->i0 = i0;
  _self->n0 = n0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift17294_repeat_fun21778_1(kk_function_t _fself, kk_box_t _b_19737, kk_context_t* _ctx) {
  struct kk_std_core__lift17294_repeat_fun21778__t_1* _self = kk_function_as(struct kk_std_core__lift17294_repeat_fun21778__t_1*, _fself);
  kk_function_t action0 = _self->action0; /* () -> 15766 () */
  kk_integer_t i0 = _self->i0; /* int */
  kk_integer_t n0 = _self->n0; /* int */
  kk_drop_match(_self, {kk_function_dup(action0);kk_integer_dup(i0);kk_integer_dup(n0);}, {}, _ctx)
  kk_unit_t wild__0_19739 = kk_Unit;
  kk_unit_unbox(_b_19737);
  kk_unit_t _x21779 = kk_Unit;
  kk_std_core__mlift18052_op(action0, i0, n0, wild__0_19739, _ctx);
  return kk_unit_box(_x21779);
}

kk_unit_t kk_std_core__lift17294_repeat_1(kk_function_t action0, kk_integer_t n0, kk_integer_t i0, kk_context_t* _ctx) { /* forall<e> (action : () -> e (), n : int, i : int) -> e () */ 
  kk__tailcall: ;
  bool _match_20024 = kk_integer_lte_borrow(i0,n0,kk_context()); /*bool*/;
  if (_match_20024) {
    kk_unit_t x_18389 = kk_Unit;
    kk_function_t _x21776 = kk_function_dup(action0); /*() -> 15766 ()*/
    kk_function_call(kk_unit_t, (kk_function_t, kk_context_t*), _x21776, (_x21776, _ctx));
    if (kk_yielding(kk_context())) {
      kk_box_t _x21777 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift17294_repeat_fun21778_1(action0, i0, n0, _ctx), _ctx); /*3926*/
      kk_unit_unbox(_x21777); return kk_Unit;
    }
    {
      kk_integer_t i0_174720 = kk_integer_add_small_const(i0, 1, _ctx); /*int*/;
      { // tailcall
        i0 = i0_174720;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_integer_drop(n0, _ctx);
    kk_integer_drop(i0, _ctx);
    kk_function_drop(action0, _ctx);
    kk_Unit; return kk_Unit;
  }
}
 
// Create a list of `n`  repeated elementes `x`

kk_std_core__list kk_std_core__ctail_replicate(kk_box_t x, kk_integer_t n, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* forall<a> (x : a, n : int, ctail<list<a>>) -> list<a> */ 
  kk__tailcall: ;
  bool _match_20023 = kk_integer_gt_borrow(n,(kk_integer_from_small(0)),kk_context()); /*bool*/;
  if (_match_20023) {
    kk_std_core__list _ctail_17601 = kk_std_core__list_hole(); /*list<15810>*/;
    kk_std_core__list _ctail_17602;
    kk_box_t _x21781 = kk_box_dup(x); /*15810*/
    _ctail_17602 = kk_std_core__new_Cons(kk_reuse_null, _x21781, _ctail_17601, _ctx); /*list<15810>*/
    kk_box_t* _b_19750_19745 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17602)->tail)); /*cfield<list<15810>>*/;
    { // tailcall
      kk_integer_t _x21782 = kk_integer_add_small_const(n, -1, _ctx); /*int*/
      kk_std_core_types__ctail _x21783 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17602, _ctx)),_b_19750_19745); /*ctail<0>*/
      n = _x21782;
      _acc = _x21783;
      goto kk__tailcall;
    }
  }
  {
    kk_box_drop(x, _ctx);
    kk_integer_drop(n, _ctx);
    kk_box_t _x21784 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21784, _ctx);
  }
}
 
// Create a list of `n`  repeated elementes `x`

kk_std_core__list kk_std_core_replicate(kk_box_t x0, kk_integer_t n0, kk_context_t* _ctx) { /* forall<a> (x : a, n : int) -> list<a> */ 
  kk_std_core_types__ctail _x21785 = kk_ctail_nil(); /*ctail<0>*/
  return kk_std_core__ctail_replicate(x0, n0, _x21785, _ctx);
}
 
// lifted local: reverse-join, reverse-acc

kk_std_core__list kk_std_core__lift17295_reverse_join(kk_std_core__list acc, kk_std_core__list ys, kk_context_t* _ctx) { /* forall<a> (acc : list<a>, ys : list<a>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con21786 = kk_std_core__as_Cons(ys);
    kk_box_t x = _con21786->head;
    kk_std_core__list xx = _con21786->tail;
    kk_reuse_t _ru_20436 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      _ru_20436 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(ys, _ctx);
    }
    { // tailcall
      kk_std_core__list _x21787 = kk_std_core__new_Cons(_ru_20436, x, acc, _ctx); /*list<61>*/
      acc = _x21787;
      ys = xx;
      goto kk__tailcall;
    }
  }
  {
    return acc;
  }
}
 
// lifted local: reverse-join, join-acc

kk_string_t kk_std_core__lift17296_reverse_join(kk_std_core__list ys0, kk_string_t acc0, kk_context_t* _ctx) { /* (ys0 : list<string>, acc0 : string) -> string */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys0)) {
    struct kk_std_core_Cons* _con21788 = kk_std_core__as_Cons(ys0);
    kk_box_t _box_x19756 = _con21788->head;
    kk_std_core__list yy = _con21788->tail;
    kk_string_t y = kk_string_unbox(_box_x19756);
    if (kk_likely(kk_std_core__list_is_unique(ys0))) {
      kk_std_core__list_free(ys0, _ctx);
    }
    else {
      kk_string_dup(y);
      kk_std_core__list_dup(yy);
      kk_std_core__list_decref(ys0, _ctx);
    }
    { // tailcall
      kk_string_t _x21790;
      kk_string_t _x21791;
      kk_string_t _x21792 = kk_string_empty(); /*string*/
      _x21791 = kk_std_core__lp__plus__plus__1_rp_(_x21792, y, _ctx); /*string*/
      _x21790 = kk_std_core__lp__plus__plus__1_rp_(acc0, _x21791, _ctx); /*string*/
      ys0 = yy;
      acc0 = _x21790;
      goto kk__tailcall;
    }
  }
  {
    return acc0;
  }
}
 
// Concatenate all strings in a list in reverse order

kk_string_t kk_std_core_reverse_join(kk_std_core__list xs, kk_context_t* _ctx) { /* (xs : list<string>) -> string */ 
  kk_std_core__list xs0_17228 = kk_std_core__lift17295_reverse_join(kk_std_core__new_Nil(_ctx), xs, _ctx); /*list<string>*/;
  if (kk_std_core__is_Nil(xs0_17228)) {
    return kk_string_empty();
  }
  {
    struct kk_std_core_Cons* _con21795 = kk_std_core__as_Cons(xs0_17228);
    kk_box_t _box_x19757 = _con21795->head;
    kk_std_core__list xx0 = _con21795->tail;
    kk_string_t x0 = kk_string_unbox(_box_x19757);
    if (kk_likely(kk_std_core__list_is_unique(xs0_17228))) {
      kk_std_core__list_free(xs0_17228, _ctx);
    }
    else {
      kk_string_dup(x0);
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(xs0_17228, _ctx);
    }
    return kk_std_core__lift17296_reverse_join(xx0, x0, _ctx);
  }
}

kk_string_t kk_std_core_show_tuple(kk_std_core_types__tuple2_ x, kk_function_t showfst, kk_function_t showsnd, kk_context_t* _ctx) { /* forall<a,b> (x : (a, b), showfst : (a) -> string, showsnd : (b) -> string) -> string */ 
  kk_string_t _x21797;
  kk_define_string_literal(, _s21798, 1, "(")
  _x21797 = kk_string_dup(_s21798); /*string*/
  kk_string_t _x21799;
  kk_string_t _x21800;
  kk_box_t _x21801;
  {
    kk_box_t _x = x.fst;
    kk_box_dup(_x);
    _x21801 = _x; /*16129*/
  }
  _x21800 = kk_function_call(kk_string_t, (kk_function_t, kk_box_t, kk_context_t*), showfst, (showfst, _x21801, _ctx)); /*string*/
  kk_string_t _x21802;
  kk_string_t _x21803;
  kk_define_string_literal(, _s21804, 1, ",")
  _x21803 = kk_string_dup(_s21804); /*string*/
  kk_string_t _x21805;
  kk_string_t _x21806;
  kk_box_t _x21807;
  {
    kk_box_t _x0 = x.snd;
    kk_box_dup(_x0);
    kk_std_core_types__tuple2__drop(x, _ctx);
    _x21807 = _x0; /*16130*/
  }
  _x21806 = kk_function_call(kk_string_t, (kk_function_t, kk_box_t, kk_context_t*), showsnd, (showsnd, _x21807, _ctx)); /*string*/
  kk_string_t _x21808;
  kk_define_string_literal(, _s21809, 1, ")")
  _x21808 = kk_string_dup(_s21809); /*string*/
  _x21805 = kk_std_core__lp__plus__plus__1_rp_(_x21806, _x21808, _ctx); /*string*/
  _x21802 = kk_std_core__lp__plus__plus__1_rp_(_x21803, _x21805, _ctx); /*string*/
  _x21799 = kk_std_core__lp__plus__plus__1_rp_(_x21800, _x21802, _ctx); /*string*/
  return kk_std_core__lp__plus__plus__1_rp_(_x21797, _x21799, _ctx);
}
 
// monadic lift

kk_std_core_types__tuple2_ kk_std_core__mlift18053_op(kk_std_core__list acc, kk_function_t predicate, kk_box_t y, kk_std_core__list ys, kk_std_core__list yy, bool _y_17894, kk_context_t* _ctx) { /* forall<a,e> (acc : list<a>, predicate : (a) -> e bool, y : a, ys : list<a>, yy : list<a>, bool) -> e (list<a>, list<a>) */ 
  if (_y_17894) {
    kk_std_core__list_drop(ys, _ctx);
    kk_std_core__list _x21810 = kk_std_core__new_Cons(kk_reuse_null, y, acc, _ctx); /*list<61>*/
    return kk_std_core__lift17297_span(predicate, yy, _x21810, _ctx);
  }
  {
    kk_std_core__list_drop(yy, _ctx);
    kk_box_drop(y, _ctx);
    kk_function_drop(predicate, _ctx);
    kk_std_core__list _b_19760_19758 = kk_std_core__lift17288_reverse(kk_std_core__new_Nil(_ctx), acc, _ctx); /*list<16188>*/;
    return kk_std_core_types__new_dash__lp__comma__rp_(kk_std_core__list_box(_b_19760_19758, _ctx), kk_std_core__list_box(ys, _ctx), _ctx);
  }
}
 
// lifted local: span, span-acc
// todo: implement TRMC with multiple results to avoid the reverse


// lift anonymous function
struct kk_std_core__lift17297_span_fun21815__t {
  struct kk_function_s _base;
  kk_std_core__list acc0;
  kk_function_t predicate0;
  kk_box_t y0;
  kk_std_core__list ys0;
  kk_std_core__list yy0;
};
static kk_box_t kk_std_core__lift17297_span_fun21815(kk_function_t _fself, kk_box_t _b_19763, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17297_span_fun21815(kk_std_core__list acc0, kk_function_t predicate0, kk_box_t y0, kk_std_core__list ys0, kk_std_core__list yy0, kk_context_t* _ctx) {
  struct kk_std_core__lift17297_span_fun21815__t* _self = kk_function_alloc_as(struct kk_std_core__lift17297_span_fun21815__t, 6, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift17297_span_fun21815, kk_context());
  _self->acc0 = acc0;
  _self->predicate0 = predicate0;
  _self->y0 = y0;
  _self->ys0 = ys0;
  _self->yy0 = yy0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift17297_span_fun21815(kk_function_t _fself, kk_box_t _b_19763, kk_context_t* _ctx) {
  struct kk_std_core__lift17297_span_fun21815__t* _self = kk_function_as(struct kk_std_core__lift17297_span_fun21815__t*, _fself);
  kk_std_core__list acc0 = _self->acc0; /* list<16188> */
  kk_function_t predicate0 = _self->predicate0; /* (16188) -> 16189 bool */
  kk_box_t y0 = _self->y0; /* 16188 */
  kk_std_core__list ys0 = _self->ys0; /* list<16188> */
  kk_std_core__list yy0 = _self->yy0; /* list<16188> */
  kk_drop_match(_self, {kk_std_core__list_dup(acc0);kk_function_dup(predicate0);kk_box_dup(y0);kk_std_core__list_dup(ys0);kk_std_core__list_dup(yy0);}, {}, _ctx)
  bool _y_19773_178940 = kk_bool_unbox(_b_19763); /*bool*/;
  kk_std_core_types__tuple2_ _x21816 = kk_std_core__mlift18053_op(acc0, predicate0, y0, ys0, yy0, _y_19773_178940, _ctx); /*(list<16188>, list<16188>)*/
  return kk_std_core_types__tuple2__box(_x21816, _ctx);
}

kk_std_core_types__tuple2_ kk_std_core__lift17297_span(kk_function_t predicate0, kk_std_core__list ys0, kk_std_core__list acc0, kk_context_t* _ctx) { /* forall<a,e> (predicate : (a) -> e bool, ys : list<a>, acc : list<a>) -> e (list<a>, list<a>) */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys0)) {
    struct kk_std_core_Cons* _con21811 = kk_std_core__as_Cons(ys0);
    kk_box_t y0 = _con21811->head;
    kk_std_core__list yy0 = _con21811->tail;
    kk_box_dup(y0);
    kk_std_core__list_dup(yy0);
    bool x_18393;
    kk_function_t _x21813 = kk_function_dup(predicate0); /*(16188) -> 16189 bool*/
    kk_box_t _x21812 = kk_box_dup(y0); /*16188*/
    x_18393 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x21813, (_x21813, _x21812, _ctx)); /*bool*/
    if (kk_yielding(kk_context())) {
      kk_box_t _x21814 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift17297_span_fun21815(acc0, predicate0, y0, ys0, yy0, _ctx), _ctx); /*3926*/
      return kk_std_core_types__tuple2__unbox(_x21814, _ctx);
    }
    if (x_18393) {
      kk_reuse_t _ru_20439 = kk_reuse_null; /*reuse*/;
      if (kk_likely(kk_std_core__list_is_unique(ys0))) {
        kk_std_core__list_drop(yy0, _ctx);
        kk_box_drop(y0, _ctx);
        _ru_20439 = (kk_std_core__list_reuse(ys0));
      }
      else {
        kk_std_core__list_decref(ys0, _ctx);
      }
      { // tailcall
        kk_std_core__list _x21817 = kk_std_core__new_Cons(_ru_20439, y0, acc0, _ctx); /*list<61>*/
        ys0 = yy0;
        acc0 = _x21817;
        goto kk__tailcall;
      }
    }
    {
      kk_std_core__list_drop(yy0, _ctx);
      kk_box_drop(y0, _ctx);
      kk_function_drop(predicate0, _ctx);
      kk_std_core__list _b_19769_19764 = kk_std_core__lift17288_reverse(kk_std_core__new_Nil(_ctx), acc0, _ctx); /*list<16188>*/;
      return kk_std_core_types__new_dash__lp__comma__rp_(kk_std_core__list_box(_b_19769_19764, _ctx), kk_std_core__list_box(ys0, _ctx), _ctx);
    }
  }
  {
    kk_function_drop(predicate0, _ctx);
    kk_std_core__list _b_19771_19766 = kk_std_core__lift17288_reverse(kk_std_core__new_Nil(_ctx), acc0, _ctx); /*list<16188>*/;
    return kk_std_core_types__new_dash__lp__comma__rp_(kk_std_core__list_box(_b_19771_19766, _ctx), kk_std_core__list_box(ys0, _ctx), _ctx);
  }
}
 
// lifted local: sum, .spec.17438
// specialized: std/core/foldl, on parameters f, using:
// f = std/core/(+.4)

kk_integer_t kk_std_core__lift17498_sum(kk_std_core__list xs0, kk_integer_t z, kk_context_t* _ctx) { /* (xs0 : list<int>, z : int) -> int */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con21818 = kk_std_core__as_Cons(xs0);
    kk_box_t _box_x19774 = _con21818->head;
    kk_std_core__list xx = _con21818->tail;
    kk_integer_t x = kk_integer_unbox(_box_x19774);
    if (kk_likely(kk_std_core__list_is_unique(xs0))) {
      kk_std_core__list_free(xs0, _ctx);
    }
    else {
      kk_integer_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs0, _ctx);
    }
    { // tailcall
      kk_integer_t _x21820 = kk_integer_add(z,x,kk_context()); /*int*/
      xs0 = xx;
      z = _x21820;
      goto kk__tailcall;
    }
  }
  {
    return z;
  }
}
 
// Return the tail of list. Returns the empty list if `xs` is empty.

kk_std_core__list kk_std_core_tail_1(kk_std_core__list xs, kk_context_t* _ctx) { /* forall<a> (xs : list<a>) -> list<a> */ 
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21821 = kk_std_core__as_Cons(xs);
    kk_box_t _pat0 = _con21821->head;
    kk_std_core__list xx = _con21821->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_box_drop(_pat0, _ctx);
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    return xx;
  }
  {
    return kk_std_core__new_Nil(_ctx);
  }
}
 
// Return the tail of a string (or the empty string)

kk_string_t kk_std_core_tail_2(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> string */ 
  kk_std_core__sslice slice1 = kk_std_core_first1(s, _ctx); /*sslice*/;
  kk_std_core__sslice slice0_17232;
  bool _match_20015;
  kk_integer_t _brw_20020;
  kk_std_core_types__optional _x21822 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
  if (kk_std_core_types__is_Optional(_x21822)) {
    kk_box_t _box_x19775 = _x21822._cons.Optional.value;
    kk_integer_t _n_10142 = kk_integer_unbox(_box_x19775);
    _brw_20020 = _n_10142; /*int*/
  }
  else {
    _brw_20020 = kk_integer_from_small(1); /*int*/
  }
  bool _brw_20021 = kk_integer_eq_borrow(_brw_20020,(kk_integer_from_small(1)),kk_context()); /*bool*/;
  kk_integer_drop(_brw_20020, _ctx);
  _match_20015 = _brw_20021; /*bool*/
  if (_match_20015) {
    slice0_17232 = slice1; /*sslice*/
  }
  else {
    kk_integer_t _brw_20017;
    kk_integer_t _x21824;
    kk_std_core_types__optional _x21825 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
    if (kk_std_core_types__is_Optional(_x21825)) {
      kk_box_t _box_x19776 = _x21825._cons.Optional.value;
      kk_integer_t _n_101420 = kk_integer_unbox(_box_x19776);
      _x21824 = _n_101420; /*int*/
    }
    else {
      _x21824 = kk_integer_from_small(1); /*int*/
    }
    _brw_20017 = kk_integer_add_small_const(_x21824, -1, _ctx); /*int*/
    kk_std_core__sslice _brw_20018 = kk_std_core_extend(slice1, _brw_20017, _ctx); /*sslice*/;
    kk_integer_drop(_brw_20017, _ctx);
    slice0_17232 = _brw_20018; /*sslice*/
  }
  kk_std_core__sslice _x21827;
  {
    kk_string_t s0 = slice0_17232.str;
    kk_ssize_t start0 = slice0_17232.start;
    kk_ssize_t len0 = slice0_17232.len;
    kk_string_dup(s0);
    kk_std_core__sslice_drop(slice0_17232, _ctx);
    kk_string_t _x21828 = kk_string_dup(s0); /*string*/
    kk_ssize_t _x21829 = (start0 + len0); /*ssize_t*/
    kk_ssize_t _x21830;
    kk_ssize_t _x21831 = kk_string_len(s0,kk_context()); /*ssize_t*/
    kk_ssize_t _x21832 = (start0 + len0); /*ssize_t*/
    _x21830 = (_x21831 - _x21832); /*ssize_t*/
    _x21827 = kk_std_core__new_Sslice(_x21828, _x21829, _x21830, _ctx); /*sslice*/
  }
  return kk_std_core_string_3(_x21827, _ctx);
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift18054_op(kk_std_core_types__ctail _acc, kk_function_t predicate, kk_box_t x, kk_std_core__list xx, bool _y_17899, kk_context_t* _ctx) { /* forall<a,e> (ctail<list<a>>, predicate : (a) -> e bool, x : a, xx : list<a>, bool) -> e list<a> */ 
  if (_y_17899) {
    kk_std_core__list _ctail_17603 = kk_std_core__list_hole(); /*list<16299>*/;
    kk_std_core__list _ctail_17604 = kk_std_core__new_Cons(kk_reuse_null, x, _ctail_17603, _ctx); /*list<16299>*/;
    kk_box_t* _b_19787_19782 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17604)->tail)); /*cfield<list<16299>>*/;
    kk_std_core_types__ctail _x21833 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17604, _ctx)),_b_19787_19782); /*ctail<0>*/
    return kk_std_core__ctail_take_while(xx, predicate, _x21833, _ctx);
  }
  {
    kk_std_core__list_drop(xx, _ctx);
    kk_box_drop(x, _ctx);
    kk_function_drop(predicate, _ctx);
    kk_box_t _x21834 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21834, _ctx);
  }
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift18055_op_fun21835__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t x0;
};
static kk_std_core__list kk_std_core__mlift18055_op_fun21835(kk_function_t _fself, kk_std_core__list _ctail_17606, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift18055_op_fun21835(kk_function_t _accm, kk_box_t x0, kk_context_t* _ctx) {
  struct kk_std_core__mlift18055_op_fun21835__t* _self = kk_function_alloc_as(struct kk_std_core__mlift18055_op_fun21835__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift18055_op_fun21835, kk_context());
  _self->_accm = _accm;
  _self->x0 = x0;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift18055_op_fun21835(kk_function_t _fself, kk_std_core__list _ctail_17606, kk_context_t* _ctx) {
  struct kk_std_core__mlift18055_op_fun21835__t* _self = kk_function_as(struct kk_std_core__mlift18055_op_fun21835__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<16299>) -> list<16299> */
  kk_box_t x0 = _self->x0; /* 16299 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(x0);}, {}, _ctx)
  kk_std_core__list _x21836 = kk_std_core__new_Cons(kk_reuse_null, x0, _ctail_17606, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x21836, _ctx));
}

kk_std_core__list kk_std_core__mlift18055_op(kk_function_t _accm, kk_function_t predicate0, kk_box_t x0, kk_std_core__list xx0, bool _y_17903, kk_context_t* _ctx) { /* forall<a,e> ((list<a>) -> list<a>, predicate : (a) -> e bool, x : a, xx : list<a>, bool) -> e list<a> */ 
  if (_y_17903) {
    return kk_std_core__ctailm_take_while(xx0, predicate0, kk_std_core__new_mlift18055_op_fun21835(_accm, x0, _ctx), _ctx);
  }
  {
    kk_std_core__list_drop(xx0, _ctx);
    kk_box_drop(x0, _ctx);
    kk_function_drop(predicate0, _ctx);
    return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, kk_std_core__new_Nil(_ctx), _ctx));
  }
}
 
// Keep only those initial elements that satisfy `predicate`


// lift anonymous function
struct kk_std_core__ctail_take_while_fun21841__t {
  struct kk_function_s _base;
  kk_function_t predicate1;
  kk_box_t x1;
  kk_std_core__list xx1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_take_while_fun21841(kk_function_t _fself, kk_box_t _b_19794, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_take_while_fun21841(kk_function_t predicate1, kk_box_t x1, kk_std_core__list xx1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_take_while_fun21841__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_take_while_fun21841__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_take_while_fun21841, kk_context());
  _self->predicate1 = predicate1;
  _self->x1 = x1;
  _self->xx1 = xx1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_take_while_fun21841(kk_function_t _fself, kk_box_t _b_19794, kk_context_t* _ctx) {
  struct kk_std_core__ctail_take_while_fun21841__t* _self = kk_function_as(struct kk_std_core__ctail_take_while_fun21841__t*, _fself);
  kk_function_t predicate1 = _self->predicate1; /* (16299) -> 16300 bool */
  kk_box_t x1 = _self->x1; /* 16299 */
  kk_std_core__list xx1 = _self->xx1; /* list<16299> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<16299>> */
  kk_drop_match(_self, {kk_function_dup(predicate1);kk_box_dup(x1);kk_std_core__list_dup(xx1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  bool _y_19816_178990 = kk_bool_unbox(_b_19794); /*bool*/;
  kk_std_core__list _x21842 = kk_std_core__mlift18054_op(_acc0, predicate1, x1, xx1, _y_19816_178990, _ctx); /*list<16299>*/
  return kk_std_core__list_box(_x21842, _ctx);
}

kk_std_core__list kk_std_core__ctail_take_while(kk_std_core__list xs, kk_function_t predicate1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, predicate : (a) -> e bool, ctail<list<a>>) -> e list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21837 = kk_std_core__as_Cons(xs);
    kk_box_t x1 = _con21837->head;
    kk_std_core__list xx1 = _con21837->tail;
    kk_reuse_t _ru_20442 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      _ru_20442 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_box_dup(x1);
      kk_std_core__list_dup(xx1);
      kk_std_core__list_decref(xs, _ctx);
    }
    bool x2_18400;
    kk_function_t _x21839 = kk_function_dup(predicate1); /*(16299) -> 16300 bool*/
    kk_box_t _x21838 = kk_box_dup(x1); /*16299*/
    x2_18400 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x21839, (_x21839, _x21838, _ctx)); /*bool*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_20442,kk_context());
      kk_box_t _x21840 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_take_while_fun21841(predicate1, x1, xx1, _acc0, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x21840, _ctx);
    }
    if (x2_18400) {
      kk_std_core__list _ctail_176030 = kk_std_core__list_hole(); /*list<16299>*/;
      kk_std_core__list _ctail_176040 = kk_std_core__new_Cons(_ru_20442, x1, _ctail_176030, _ctx); /*list<16299>*/;
      kk_box_t* _b_19808_19800 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_176040)->tail)); /*cfield<list<16299>>*/;
      { // tailcall
        kk_std_core_types__ctail _x21843 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_176040, _ctx)),_b_19808_19800); /*ctail<0>*/
        xs = xx1;
        _acc0 = _x21843;
        goto kk__tailcall;
      }
    }
    {
      kk_reuse_drop(_ru_20442,kk_context());
      kk_std_core__list_drop(xx1, _ctx);
      kk_box_drop(x1, _ctx);
      kk_function_drop(predicate1, _ctx);
      kk_box_t _x21844 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
      return kk_std_core__list_unbox(_x21844, _ctx);
    }
  }
  {
    kk_function_drop(predicate1, _ctx);
    kk_box_t _x21845 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21845, _ctx);
  }
}
 
// Keep only those initial elements that satisfy `predicate`


// lift anonymous function
struct kk_std_core__ctailm_take_while_fun21850__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t predicate2;
  kk_box_t x3;
  kk_std_core__list xx2;
};
static kk_box_t kk_std_core__ctailm_take_while_fun21850(kk_function_t _fself, kk_box_t _b_19818, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_take_while_fun21850(kk_function_t _accm0, kk_function_t predicate2, kk_box_t x3, kk_std_core__list xx2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_take_while_fun21850__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_take_while_fun21850__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_take_while_fun21850, kk_context());
  _self->_accm0 = _accm0;
  _self->predicate2 = predicate2;
  _self->x3 = x3;
  _self->xx2 = xx2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_take_while_fun21850(kk_function_t _fself, kk_box_t _b_19818, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_take_while_fun21850__t* _self = kk_function_as(struct kk_std_core__ctailm_take_while_fun21850__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<16299>) -> list<16299> */
  kk_function_t predicate2 = _self->predicate2; /* (16299) -> 16300 bool */
  kk_box_t x3 = _self->x3; /* 16299 */
  kk_std_core__list xx2 = _self->xx2; /* list<16299> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(predicate2);kk_box_dup(x3);kk_std_core__list_dup(xx2);}, {}, _ctx)
  bool _y_19820_179030 = kk_bool_unbox(_b_19818); /*bool*/;
  kk_std_core__list _x21851 = kk_std_core__mlift18055_op(_accm0, predicate2, x3, xx2, _y_19820_179030, _ctx); /*list<16299>*/
  return kk_std_core__list_box(_x21851, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_take_while_fun21853__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t x3;
};
static kk_std_core__list kk_std_core__ctailm_take_while_fun21853(kk_function_t _fself, kk_std_core__list _ctail_176060, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_take_while_fun21853(kk_function_t _accm0, kk_box_t x3, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_take_while_fun21853__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_take_while_fun21853__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_take_while_fun21853, kk_context());
  _self->_accm0 = _accm0;
  _self->x3 = x3;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_take_while_fun21853(kk_function_t _fself, kk_std_core__list _ctail_176060, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_take_while_fun21853__t* _self = kk_function_as(struct kk_std_core__ctailm_take_while_fun21853__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<16299>) -> list<16299> */
  kk_box_t x3 = _self->x3; /* 16299 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(x3);}, {}, _ctx)
  kk_std_core__list _x21854 = kk_std_core__new_Cons(kk_reuse_null, x3, _ctail_176060, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x21854, _ctx));
}

kk_std_core__list kk_std_core__ctailm_take_while(kk_std_core__list xs0, kk_function_t predicate2, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, predicate : (a) -> e bool, (list<a>) -> list<a>) -> e list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con21846 = kk_std_core__as_Cons(xs0);
    kk_box_t x3 = _con21846->head;
    kk_std_core__list xx2 = _con21846->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs0))) {
      kk_std_core__list_free(xs0, _ctx);
    }
    else {
      kk_box_dup(x3);
      kk_std_core__list_dup(xx2);
      kk_std_core__list_decref(xs0, _ctx);
    }
    bool x4_18403;
    kk_function_t _x21848 = kk_function_dup(predicate2); /*(16299) -> 16300 bool*/
    kk_box_t _x21847 = kk_box_dup(x3); /*16299*/
    x4_18403 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x21848, (_x21848, _x21847, _ctx)); /*bool*/
    if (kk_yielding(kk_context())) {
      kk_box_t _x21849 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_take_while_fun21850(_accm0, predicate2, x3, xx2, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x21849, _ctx);
    }
    if (x4_18403) { // tailcall
                    kk_function_t _x21852 = kk_std_core__new_ctailm_take_while_fun21853(_accm0, x3, _ctx); /*(list<16299>) -> list<16299>*/
                    xs0 = xx2;
                    _accm0 = _x21852;
                    goto kk__tailcall;
    }
    {
      kk_std_core__list_drop(xx2, _ctx);
      kk_box_drop(x3, _ctx);
      kk_function_drop(predicate2, _ctx);
      return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
    }
  }
  {
    kk_function_drop(predicate2, _ctx);
    return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
  }
}
 
// Keep only those initial elements that satisfy `predicate`


// lift anonymous function
struct kk_std_core_take_while_fun21856__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core_take_while_fun21856(kk_function_t _fself, kk_std_core__list _ctail_17605, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_take_while_fun21856(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_take_while_fun21856, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core_take_while_fun21856(kk_function_t _fself, kk_std_core__list _ctail_17605, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17605;
}

kk_std_core__list kk_std_core_take_while(kk_std_core__list xs1, kk_function_t predicate3, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, predicate : (a) -> e bool) -> e list<a> */ 
  bool _match_20012 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_20012) {
    kk_std_core_types__ctail _x21855 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_take_while(xs1, predicate3, _x21855, _ctx);
  }
  {
    return kk_std_core__ctailm_take_while(xs1, predicate3, kk_std_core_new_take_while_fun21856(_ctx), _ctx);
  }
}
 
// Trace a message used for debug purposes.
// The behaviour is system dependent. On a browser and node it uses
// `console.log`  by default.
// Disabled if `notrace` is called.


// lift anonymous function
struct kk_std_core_trace_fun21860__t {
  struct kk_function_s _base;
  kk_string_t message0;
};
static kk_box_t kk_std_core_trace_fun21860(kk_function_t _fself, kk_box_t _b_19824, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_trace_fun21860(kk_string_t message0, kk_context_t* _ctx) {
  struct kk_std_core_trace_fun21860__t* _self = kk_function_alloc_as(struct kk_std_core_trace_fun21860__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_trace_fun21860, kk_context());
  _self->message0 = message0;
  return &_self->_base;
}

static kk_box_t kk_std_core_trace_fun21860(kk_function_t _fself, kk_box_t _b_19824, kk_context_t* _ctx) {
  struct kk_std_core_trace_fun21860__t* _self = kk_function_as(struct kk_std_core_trace_fun21860__t*, _fself);
  kk_string_t message0 = _self->message0; /* string */
  kk_drop_match(_self, {kk_string_dup(message0);}, {}, _ctx)
  kk_unit_t _x21861 = kk_Unit;
  bool _y_19826_17910 = kk_bool_unbox(_b_19824); /*bool*/;
  if (_y_19826_17910) {
    kk_std_core_xtrace(message0, _ctx);
  }
  else {
    kk_string_drop(message0, _ctx);
  }
  return kk_unit_box(_x21861);
}

kk_unit_t kk_std_core_trace(kk_string_t message0, kk_context_t* _ctx) { /* (message : string) -> () */ 
  bool x_18406;
  kk_box_t _x21857;
  kk_ref_t _x21858 = kk_ref_dup(kk_std_core_trace_enabled); /*ref<global,bool>*/
  _x21857 = kk_ref_get(_x21858,kk_context()); /*1001*/
  x_18406 = kk_bool_unbox(_x21857); /*bool*/
  if (kk_yielding(kk_context())) {
    kk_box_t _x21859 = kk_std_core_hnd_yield_extend(kk_std_core_new_trace_fun21860(message0, _ctx), _ctx); /*3926*/
    kk_unit_unbox(_x21859); return kk_Unit;
  }
  if (x_18406) {
    kk_std_core_xtrace(message0, _ctx); return kk_Unit;
  }
  {
    kk_string_drop(message0, _ctx);
    kk_Unit; return kk_Unit;
  }
}


// lift anonymous function
struct kk_std_core_trace_any_fun21865__t {
  struct kk_function_s _base;
  kk_string_t message0;
  kk_box_t x;
};
static kk_box_t kk_std_core_trace_any_fun21865(kk_function_t _fself, kk_box_t _b_19831, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_trace_any_fun21865(kk_string_t message0, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_trace_any_fun21865__t* _self = kk_function_alloc_as(struct kk_std_core_trace_any_fun21865__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_trace_any_fun21865, kk_context());
  _self->message0 = message0;
  _self->x = x;
  return &_self->_base;
}

static kk_box_t kk_std_core_trace_any_fun21865(kk_function_t _fself, kk_box_t _b_19831, kk_context_t* _ctx) {
  struct kk_std_core_trace_any_fun21865__t* _self = kk_function_as(struct kk_std_core_trace_any_fun21865__t*, _fself);
  kk_string_t message0 = _self->message0; /* string */
  kk_box_t x = _self->x; /* 16408 */
  kk_drop_match(_self, {kk_string_dup(message0);kk_box_dup(x);}, {}, _ctx)
  kk_unit_t _x21866 = kk_Unit;
  bool _y_19833_17912 = kk_bool_unbox(_b_19831); /*bool*/;
  if (_y_19833_17912) {
    kk_std_core_xtrace_any(message0, x, _ctx);
  }
  else {
    kk_box_drop(x, _ctx);
    kk_string_drop(message0, _ctx);
  }
  return kk_unit_box(_x21866);
}

kk_unit_t kk_std_core_trace_any(kk_string_t message0, kk_box_t x, kk_context_t* _ctx) { /* forall<a> (message : string, x : a) -> () */ 
  bool x0_18410;
  kk_box_t _x21862;
  kk_ref_t _x21863 = kk_ref_dup(kk_std_core_trace_enabled); /*ref<global,bool>*/
  _x21862 = kk_ref_get(_x21863,kk_context()); /*1001*/
  x0_18410 = kk_bool_unbox(_x21862); /*bool*/
  if (kk_yielding(kk_context())) {
    kk_box_t _x21864 = kk_std_core_hnd_yield_extend(kk_std_core_new_trace_any_fun21865(message0, x, _ctx), _ctx); /*3926*/
    kk_unit_unbox(_x21864); return kk_Unit;
  }
  if (x0_18410) {
    kk_std_core_xtrace_any(message0, x, _ctx); return kk_Unit;
  }
  {
    kk_box_drop(x, _ctx);
    kk_string_drop(message0, _ctx);
    kk_Unit; return kk_Unit;
  }
}
 
// Truncate a string to `count` characters.

kk_string_t kk_std_core_truncate(kk_string_t s, kk_integer_t count, kk_context_t* _ctx) { /* (s : string, count : int) -> string */ 
  kk_std_core__sslice slice0 = kk_std_core_first1(s, _ctx); /*sslice*/;
  kk_std_core__sslice _x21867;
  kk_std_core__sslice _own_20001;
  bool _match_20002;
  kk_integer_t _brw_20007;
  kk_std_core_types__optional _x21868 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
  if (kk_std_core_types__is_Optional(_x21868)) {
    kk_box_t _box_x19835 = _x21868._cons.Optional.value;
    kk_integer_t _n_10142 = kk_integer_unbox(_box_x19835);
    _brw_20007 = _n_10142; /*int*/
  }
  else {
    _brw_20007 = kk_integer_from_small(1); /*int*/
  }
  bool _brw_20008 = kk_integer_eq_borrow(_brw_20007,(kk_integer_from_small(1)),kk_context()); /*bool*/;
  kk_integer_drop(_brw_20007, _ctx);
  _match_20002 = _brw_20008; /*bool*/
  if (_match_20002) {
    _own_20001 = slice0; /*sslice*/
  }
  else {
    kk_integer_t _brw_20004;
    kk_integer_t _x21870;
    kk_std_core_types__optional _x21871 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
    if (kk_std_core_types__is_Optional(_x21871)) {
      kk_box_t _box_x19836 = _x21871._cons.Optional.value;
      kk_integer_t _n_101420 = kk_integer_unbox(_box_x19836);
      _x21870 = _n_101420; /*int*/
    }
    else {
      _x21870 = kk_integer_from_small(1); /*int*/
    }
    _brw_20004 = kk_integer_add_small_const(_x21870, -1, _ctx); /*int*/
    kk_std_core__sslice _brw_20005 = kk_std_core_extend(slice0, _brw_20004, _ctx); /*sslice*/;
    kk_integer_drop(_brw_20004, _ctx);
    _own_20001 = _brw_20005; /*sslice*/
  }
  kk_integer_t _brw_20000 = kk_integer_add_small_const(count, -1, _ctx); /*int*/;
  kk_std_core__sslice _brw_20009 = kk_std_core_extend(_own_20001, _brw_20000, _ctx); /*sslice*/;
  kk_integer_drop(_brw_20000, _ctx);
  _x21867 = _brw_20009; /*sslice*/
  return kk_std_core_string_3(_x21867, _ctx);
}
 
// Return a default value when an exception is raised


// lift anonymous function
struct kk_std_core_try_default_fun21873__t {
  struct kk_function_s _base;
  kk_box_t value;
};
static kk_box_t kk_std_core_try_default_fun21873(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__525__16, kk_std_core__exception x, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_default_fun21873(kk_box_t value, kk_context_t* _ctx) {
  struct kk_std_core_try_default_fun21873__t* _self = kk_function_alloc_as(struct kk_std_core_try_default_fun21873__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_try_default_fun21873, kk_context());
  _self->value = value;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_try_default_fun21874__t {
  struct kk_function_s _base;
  kk_box_t value;
};
static kk_box_t kk_std_core_try_default_fun21874(kk_function_t _fself, kk_function_t ___wildcard__525__45, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_default_fun21874(kk_box_t value, kk_context_t* _ctx) {
  struct kk_std_core_try_default_fun21874__t* _self = kk_function_alloc_as(struct kk_std_core_try_default_fun21874__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_try_default_fun21874, kk_context());
  _self->value = value;
  return &_self->_base;
}

static kk_box_t kk_std_core_try_default_fun21874(kk_function_t _fself, kk_function_t ___wildcard__525__45, kk_context_t* _ctx) {
  struct kk_std_core_try_default_fun21874__t* _self = kk_function_as(struct kk_std_core_try_default_fun21874__t*, _fself);
  kk_box_t value = _self->value; /* 16484 */
  kk_drop_match(_self, {kk_box_dup(value);}, {}, _ctx)
  kk_function_drop(___wildcard__525__45, _ctx);
  return value;
}
static kk_box_t kk_std_core_try_default_fun21873(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__525__16, kk_std_core__exception x, kk_context_t* _ctx) {
  struct kk_std_core_try_default_fun21873__t* _self = kk_function_as(struct kk_std_core_try_default_fun21873__t*, _fself);
  kk_box_t value = _self->value; /* 16484 */
  kk_drop_match(_self, {kk_box_dup(value);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(___wildcard__525__16, (KK_I32(3)), _ctx);
  kk_std_core__exception_drop(x, _ctx);
  return kk_std_core_hnd_yield_to_final(m0, kk_std_core_new_try_default_fun21874(value, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_try_default_fun21877__t {
  struct kk_function_s _base;
  kk_function_t _b_19841_19837;
};
static kk_box_t kk_std_core_try_default_fun21877(kk_function_t _fself, kk_std_core_hnd__marker _b_19838, kk_std_core_hnd__ev _b_19839, kk_box_t _b_19840, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_default_fun21877(kk_function_t _b_19841_19837, kk_context_t* _ctx) {
  struct kk_std_core_try_default_fun21877__t* _self = kk_function_alloc_as(struct kk_std_core_try_default_fun21877__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_try_default_fun21877, kk_context());
  _self->_b_19841_19837 = _b_19841_19837;
  return &_self->_base;
}

static kk_box_t kk_std_core_try_default_fun21877(kk_function_t _fself, kk_std_core_hnd__marker _b_19838, kk_std_core_hnd__ev _b_19839, kk_box_t _b_19840, kk_context_t* _ctx) {
  struct kk_std_core_try_default_fun21877__t* _self = kk_function_as(struct kk_std_core_try_default_fun21877__t*, _fself);
  kk_function_t _b_19841_19837 = _self->_b_19841_19837; /* (m0 : std/core/hnd/marker<16485,16484>, std/core/hnd/ev<.hnd-exn>, x : exception) -> 16485 10295 */
  kk_drop_match(_self, {kk_function_dup(_b_19841_19837);}, {}, _ctx)
  kk_std_core__exception _x21878 = kk_std_core__exception_unbox(_b_19840, _ctx); /*exception*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_std_core__exception, kk_context_t*), _b_19841_19837, (_b_19841_19837, _b_19838, _b_19839, _x21878, _ctx));
}


// lift anonymous function
struct kk_std_core_try_default_fun21879__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_try_default_fun21879(kk_function_t _fself, kk_box_t _x, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_default_fun21879(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_try_default_fun21879, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_try_default_fun21879(kk_function_t _fself, kk_box_t _x, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _x;
}

kk_box_t kk_std_core_try_default(kk_box_t value, kk_function_t action, kk_context_t* _ctx) { /* forall<a,e> (value : a, action : () -> <exn|e> a) -> e a */ 
  kk_function_t _b_19841_19837 = kk_std_core_new_try_default_fun21873(value, _ctx); /*(m0 : std/core/hnd/marker<16485,16484>, std/core/hnd/ev<.hnd-exn>, x : exception) -> 16485 10295*/;
  kk_std_core__hnd_exn _x21875;
  kk_std_core_hnd__clause1 _x21876 = kk_std_core_hnd__new_Clause1(kk_std_core_new_try_default_fun21877(_b_19841_19837, _ctx), _ctx); /*std/core/hnd/clause1<51,52,53,54,55>*/
  _x21875 = kk_std_core__new_Hnd_exn(kk_reuse_null, _x21876, _ctx); /*.hnd-exn<11,12>*/
  return kk_std_core__handle_exn((KK_I32(0)), _x21875, kk_std_core_new_try_default_fun21879(_ctx), action, _ctx);
}
 
// Returns a unique integer (modulo 32-bits).


// lift anonymous function
struct kk_std_core_unique_fun21883__t {
  struct kk_function_s _base;
};
static kk_integer_t kk_std_core_unique_fun21883(kk_function_t _fself, kk_integer_t u, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_unique_fun21883(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_unique_fun21883, _ctx)
  return kk_function_dup(_fself);
}

static kk_integer_t kk_std_core_unique_fun21883(kk_function_t _fself, kk_integer_t u, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_integer_t _b_19851_19849;
  kk_integer_t _x21884 = kk_integer_dup(u); /*int*/
  _b_19851_19849 = kk_integer_add_small_const(_x21884, 1, _ctx); /*int*/
  kk_unit_t __ = kk_Unit;
  kk_ref_set_borrow(kk_std_core_unique_count,(kk_integer_box(_b_19851_19849)),kk_context());
  return u;
}


// lift anonymous function
struct kk_std_core_unique_fun21886__t {
  struct kk_function_s _base;
  kk_function_t next0_18417;
};
static kk_box_t kk_std_core_unique_fun21886(kk_function_t _fself, kk_box_t _b_19853, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_unique_fun21886(kk_function_t next0_18417, kk_context_t* _ctx) {
  struct kk_std_core_unique_fun21886__t* _self = kk_function_alloc_as(struct kk_std_core_unique_fun21886__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_unique_fun21886, kk_context());
  _self->next0_18417 = next0_18417;
  return &_self->_base;
}

static kk_box_t kk_std_core_unique_fun21886(kk_function_t _fself, kk_box_t _b_19853, kk_context_t* _ctx) {
  struct kk_std_core_unique_fun21886__t* _self = kk_function_as(struct kk_std_core_unique_fun21886__t*, _fself);
  kk_function_t next0_18417 = _self->next0_18417; /* (int) -> <read<global>,write<global>|_16545> int */
  kk_drop_match(_self, {kk_function_dup(next0_18417);}, {}, _ctx)
  kk_integer_t _x21887;
  kk_integer_t _x21888 = kk_integer_unbox(_b_19853); /*int*/
  _x21887 = kk_function_call(kk_integer_t, (kk_function_t, kk_integer_t, kk_context_t*), next0_18417, (next0_18417, _x21888, _ctx)); /*int*/
  return kk_integer_box(_x21887);
}

kk_integer_t kk_std_core_unique(kk_context_t* _ctx) { /* () -> ndet int */ 
  kk_integer_t x_18416;
  kk_box_t _x21881;
  kk_ref_t _x21882 = kk_ref_dup(kk_std_core_unique_count); /*ref<global,int>*/
  _x21881 = kk_ref_get(_x21882,kk_context()); /*1001*/
  x_18416 = kk_integer_unbox(_x21881); /*int*/
  kk_function_t next0_18417 = kk_std_core_new_unique_fun21883(_ctx); /*(int) -> <read<global>,write<global>|_16545> int*/;
  if (kk_yielding(kk_context())) {
    kk_integer_drop(x_18416, _ctx);
    kk_box_t _x21885 = kk_std_core_hnd_yield_extend(kk_std_core_new_unique_fun21886(next0_18417, _ctx), _ctx); /*3926*/
    return kk_integer_unbox(_x21885);
  }
  {
    return kk_function_call(kk_integer_t, (kk_function_t, kk_integer_t, kk_context_t*), next0_18417, (next0_18417, x_18416, _ctx));
  }
}
 
// Get the value of the `Just` constructor or raise an exception

kk_box_t kk_std_core_unjust(kk_std_core_types__maybe m, kk_context_t* _ctx) { /* forall<a> (m : maybe<a>) -> exn a */ 
  if (kk_std_core_types__is_Just(m)) {
    kk_box_t x = m._cons.Just.value;
    return x;
  }
  {
    kk_std_core_hnd__ev ev_18419;
    kk_ssize_t _x21889 = (KK_IZ(0)); /*ssize_t*/
    ev_18419 = kk_evv_at(_x21889,kk_context()); /*std/core/hnd/ev<.hnd-exn>*/
    {
      struct kk_std_core_hnd_Ev* _con21890 = kk_std_core_hnd__as_Ev(ev_18419);
      kk_std_core_hnd__marker m0 = _con21890->marker;
      kk_box_t _box_x19855 = _con21890->hnd;
      kk_std_core__hnd_exn h = kk_std_core__hnd_exn_unbox(_box_x19855, NULL);
      kk_std_core__hnd_exn_dup(h);
      kk_std_core_hnd__clause1 _match_19996;
      kk_std_core_hnd__clause1 _brw_19998 = kk_std_core__select_throw_exn(h, _ctx); /*std/core/hnd/clause1<exception,1938,.hnd-exn,1939,1940>*/;
      kk_std_core__hnd_exn_drop(h, _ctx);
      _match_19996 = _brw_19998; /*std/core/hnd/clause1<exception,1938,.hnd-exn,1939,1940>*/
      {
        kk_function_t _fun_unbox_x19859 = _match_19996.clause;
        kk_box_t _x21892;
        kk_std_core__exception _x21893;
        kk_string_t _x21894;
        kk_define_string_literal(, _s21895, 37, "unexpected Nothing in std/core/unjust")
        _x21894 = kk_string_dup(_s21895); /*string*/
        kk_std_core__exception_info _x21896;
        kk_std_core_types__optional _x21897 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
        if (kk_std_core_types__is_Optional(_x21897)) {
          kk_box_t _box_x19863 = _x21897._cons.Optional.value;
          kk_std_core__exception_info _info_11715 = kk_std_core__exception_info_unbox(_box_x19863, NULL);
          _x21896 = _info_11715; /*exception-info*/
        }
        else {
          _x21896 = kk_std_core__new_ExnError(_ctx); /*exception-info*/
        }
        _x21893 = kk_std_core__new_Exception(_x21894, _x21896, _ctx); /*exception*/
        _x21892 = kk_std_core__exception_box(_x21893, _ctx); /*51*/
        return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x19859, (_fun_unbox_x19859, m0, ev_18419, _x21892, _ctx));
      }
    }
  }
}
 
// lifted local: unlines, join-acc

kk_string_t kk_std_core__lift17298_unlines(kk_std_core__list ys, kk_string_t acc, kk_context_t* _ctx) { /* (ys : list<string>, acc : string) -> string */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con21899 = kk_std_core__as_Cons(ys);
    kk_box_t _box_x19864 = _con21899->head;
    kk_std_core__list yy = _con21899->tail;
    kk_string_t y = kk_string_unbox(_box_x19864);
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      kk_std_core__list_free(ys, _ctx);
    }
    else {
      kk_string_dup(y);
      kk_std_core__list_dup(yy);
      kk_std_core__list_decref(ys, _ctx);
    }
    { // tailcall
      kk_string_t _x21901;
      kk_string_t _x21902;
      kk_string_t _x21903;
      kk_define_string_literal(, _s21904, 1, "\n")
      _x21903 = kk_string_dup(_s21904); /*string*/
      _x21902 = kk_std_core__lp__plus__plus__1_rp_(_x21903, y, _ctx); /*string*/
      _x21901 = kk_std_core__lp__plus__plus__1_rp_(acc, _x21902, _ctx); /*string*/
      ys = yy;
      acc = _x21901;
      goto kk__tailcall;
    }
  }
  {
    return acc;
  }
}
 
// Join a list of strings with newlines

kk_string_t kk_std_core_unlines(kk_std_core__list xs, kk_context_t* _ctx) { /* (xs : list<string>) -> string */ 
  if (kk_std_core__is_Nil(xs)) {
    return kk_string_empty();
  }
  {
    struct kk_std_core_Cons* _con21906 = kk_std_core__as_Cons(xs);
    kk_box_t _box_x19865 = _con21906->head;
    kk_std_core__list xx = _con21906->tail;
    kk_string_t x = kk_string_unbox(_box_x19865);
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_string_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    return kk_std_core__lift17298_unlines(xx, x, _ctx);
  }
}
 
// lifted local: .lift17299-unzip, unzip, reverse-acc

kk_std_core__list kk_std_core__lift17300_unzip(kk_std_core__list acc, kk_std_core__list ys0, kk_context_t* _ctx) { /* forall<a> (acc : list<a>, ys0 : list<a>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys0)) {
    struct kk_std_core_Cons* _con21908 = kk_std_core__as_Cons(ys0);
    kk_box_t x0 = _con21908->head;
    kk_std_core__list xx0 = _con21908->tail;
    kk_reuse_t _ru_20446 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(ys0))) {
      _ru_20446 = (kk_std_core__list_reuse(ys0));
    }
    else {
      kk_box_dup(x0);
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(ys0, _ctx);
    }
    { // tailcall
      kk_std_core__list _x21909 = kk_std_core__new_Cons(_ru_20446, x0, acc, _ctx); /*list<61>*/
      acc = _x21909;
      ys0 = xx0;
      goto kk__tailcall;
    }
  }
  {
    return acc;
  }
}
 
// lifted local: .lift17299-unzip, unzip, reverse-acc0

kk_std_core__list kk_std_core__lift17301_unzip(kk_std_core__list acc0, kk_std_core__list ys1, kk_context_t* _ctx) { /* forall<a> (acc0 : list<a>, ys1 : list<a>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys1)) {
    struct kk_std_core_Cons* _con21910 = kk_std_core__as_Cons(ys1);
    kk_box_t x1 = _con21910->head;
    kk_std_core__list xx1 = _con21910->tail;
    kk_reuse_t _ru_20447 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(ys1))) {
      _ru_20447 = (kk_std_core__list_reuse(ys1));
    }
    else {
      kk_box_dup(x1);
      kk_std_core__list_dup(xx1);
      kk_std_core__list_decref(ys1, _ctx);
    }
    { // tailcall
      kk_std_core__list _x21911 = kk_std_core__new_Cons(_ru_20447, x1, acc0, _ctx); /*list<61>*/
      acc0 = _x21911;
      ys1 = xx1;
      goto kk__tailcall;
    }
  }
  {
    return acc0;
  }
}
 
// lifted local: unzip, iter
// todo: implement TRMC for multiple results

kk_std_core_types__tuple2_ kk_std_core__lift17299_unzip(kk_std_core__list ys, kk_std_core__list acc1, kk_std_core__list acc2, kk_context_t* _ctx) { /* forall<a,b> (ys : list<(a, b)>, acc1 : list<a>, acc2 : list<b>) -> (list<a>, list<b>) */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con21912 = kk_std_core__as_Cons(ys);
    kk_box_t _box_x19866 = _con21912->head;
    kk_std_core__list xx = _con21912->tail;
    kk_std_core_types__tuple2_ _pat0 = kk_std_core_types__tuple2__unbox(_box_x19866, NULL);
    kk_box_t x = _pat0.fst;
    kk_box_t y = _pat0.snd;
    kk_reuse_t _ru_20448 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      kk_box_dup(x);
      kk_box_dup(y);
      kk_box_drop(_box_x19866, _ctx);
      _ru_20448 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_box_dup(y);
      kk_std_core__list_decref(ys, _ctx);
    }
    { // tailcall
      kk_std_core__list _x21914 = kk_std_core__new_Cons(_ru_20448, x, acc1, _ctx); /*list<61>*/
      kk_std_core__list _x21915 = kk_std_core__new_Cons(kk_reuse_null, y, acc2, _ctx); /*list<61>*/
      ys = xx;
      acc1 = _x21914;
      acc2 = _x21915;
      goto kk__tailcall;
    }
  }
  {
    kk_std_core__list _b_19869_19867 = kk_std_core__lift17300_unzip(kk_std_core__new_Nil(_ctx), acc1, _ctx); /*list<16600>*/;
    kk_std_core__list _b_19870_19868 = kk_std_core__lift17301_unzip(kk_std_core__new_Nil(_ctx), acc2, _ctx); /*list<16601>*/;
    return kk_std_core_types__new_dash__lp__comma__rp_(kk_std_core__list_box(_b_19869_19867, _ctx), kk_std_core__list_box(_b_19870_19868, _ctx), _ctx);
  }
}
 
// Convert a string to a vector of characters.

kk_vector_t kk_std_core_vector_1(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> vector<char> */ 
  return kk_string_to_chars(s,kk_context());
}
extern kk_box_t kk_std_core_vector_fun21918_2(kk_function_t _fself, kk_ssize_t ___wildcard__1854__30, kk_context_t* _ctx) {
  struct kk_std_core_vector_fun21918__t_2* _self = kk_function_as(struct kk_std_core_vector_fun21918__t_2*, _fself);
  kk_box_t default0 = _self->default0; /* 16677 */
  kk_drop_match(_self, {kk_box_dup(default0);}, {}, _ctx)
  return default0;
}
extern kk_box_t kk_std_core_vector_init_fun21921(kk_function_t _fself, kk_ssize_t i, kk_context_t* _ctx) {
  struct kk_std_core_vector_init_fun21921__t* _self = kk_function_as(struct kk_std_core_vector_init_fun21921__t*, _fself);
  kk_function_t f = _self->f; /* (int) -> 16747 */
  kk_drop_match(_self, {kk_function_dup(f);}, {}, _ctx)
  kk_integer_t _x21922 = kk_integer_from_ssize_t(i,kk_context()); /*int*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_context_t*), f, (f, _x21922, _ctx));
}
 
// monadic lift

kk_unit_t kk_std_core__mlift18059_while(kk_function_t action, kk_function_t predicate, kk_unit_t wild__, kk_context_t* _ctx) { /* forall<e> (action : () -> <div|e> (), predicate : () -> <div|e> bool, wild_ : ()) -> <div|e> () */ 
  kk_std_core_while(predicate, action, _ctx); return kk_Unit;
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift18060_while_fun21925__t {
  struct kk_function_s _base;
  kk_function_t action0;
  kk_function_t predicate0;
};
static kk_box_t kk_std_core__mlift18060_while_fun21925(kk_function_t _fself, kk_box_t _b_19872, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift18060_while_fun21925(kk_function_t action0, kk_function_t predicate0, kk_context_t* _ctx) {
  struct kk_std_core__mlift18060_while_fun21925__t* _self = kk_function_alloc_as(struct kk_std_core__mlift18060_while_fun21925__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift18060_while_fun21925, kk_context());
  _self->action0 = action0;
  _self->predicate0 = predicate0;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift18060_while_fun21925(kk_function_t _fself, kk_box_t _b_19872, kk_context_t* _ctx) {
  struct kk_std_core__mlift18060_while_fun21925__t* _self = kk_function_as(struct kk_std_core__mlift18060_while_fun21925__t*, _fself);
  kk_function_t action0 = _self->action0; /* () -> <div|16768> () */
  kk_function_t predicate0 = _self->predicate0; /* () -> <div|16768> bool */
  kk_drop_match(_self, {kk_function_dup(action0);kk_function_dup(predicate0);}, {}, _ctx)
  kk_unit_t wild__0_19874 = kk_Unit;
  kk_unit_unbox(_b_19872);
  kk_unit_t _x21926 = kk_Unit;
  kk_std_core__mlift18059_while(action0, predicate0, wild__0_19874, _ctx);
  return kk_unit_box(_x21926);
}

kk_unit_t kk_std_core__mlift18060_while(kk_function_t action0, kk_function_t predicate0, bool _y_17918, kk_context_t* _ctx) { /* forall<e> (action : () -> <div|e> (), predicate : () -> <div|e> bool, bool) -> <div|e> () */ 
  if (_y_17918) {
    kk_unit_t x_18422 = kk_Unit;
    kk_function_t _x21923 = kk_function_dup(action0); /*() -> <div|16768> ()*/
    kk_function_call(kk_unit_t, (kk_function_t, kk_context_t*), _x21923, (_x21923, _ctx));
    if (kk_yielding(kk_context())) {
      kk_box_t _x21924 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift18060_while_fun21925(action0, predicate0, _ctx), _ctx); /*3926*/
      kk_unit_unbox(_x21924); return kk_Unit;
    }
    {
      kk_std_core__mlift18059_while(action0, predicate0, x_18422, _ctx); return kk_Unit;
    }
  }
  {
    kk_function_drop(predicate0, _ctx);
    kk_function_drop(action0, _ctx);
    kk_Unit; return kk_Unit;
  }
}
 
// The `while` fun executes `action`  as long as `pred`  is `true`.


// lift anonymous function
struct kk_std_core_while_fun21929__t {
  struct kk_function_s _base;
  kk_function_t action1;
  kk_function_t predicate1;
};
static kk_box_t kk_std_core_while_fun21929(kk_function_t _fself, kk_box_t _b_19876, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_while_fun21929(kk_function_t action1, kk_function_t predicate1, kk_context_t* _ctx) {
  struct kk_std_core_while_fun21929__t* _self = kk_function_alloc_as(struct kk_std_core_while_fun21929__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_while_fun21929, kk_context());
  _self->action1 = action1;
  _self->predicate1 = predicate1;
  return &_self->_base;
}

static kk_box_t kk_std_core_while_fun21929(kk_function_t _fself, kk_box_t _b_19876, kk_context_t* _ctx) {
  struct kk_std_core_while_fun21929__t* _self = kk_function_as(struct kk_std_core_while_fun21929__t*, _fself);
  kk_function_t action1 = _self->action1; /* () -> <div|16768> () */
  kk_function_t predicate1 = _self->predicate1; /* () -> <div|16768> bool */
  kk_drop_match(_self, {kk_function_dup(action1);kk_function_dup(predicate1);}, {}, _ctx)
  bool _y_19881_179180 = kk_bool_unbox(_b_19876); /*bool*/;
  kk_unit_t _x21930 = kk_Unit;
  kk_std_core__mlift18060_while(action1, predicate1, _y_19881_179180, _ctx);
  return kk_unit_box(_x21930);
}


// lift anonymous function
struct kk_std_core_while_fun21933__t {
  struct kk_function_s _base;
  kk_function_t action1;
  kk_function_t predicate1;
};
static kk_box_t kk_std_core_while_fun21933(kk_function_t _fself, kk_box_t _b_19878, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_while_fun21933(kk_function_t action1, kk_function_t predicate1, kk_context_t* _ctx) {
  struct kk_std_core_while_fun21933__t* _self = kk_function_alloc_as(struct kk_std_core_while_fun21933__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_while_fun21933, kk_context());
  _self->action1 = action1;
  _self->predicate1 = predicate1;
  return &_self->_base;
}

static kk_box_t kk_std_core_while_fun21933(kk_function_t _fself, kk_box_t _b_19878, kk_context_t* _ctx) {
  struct kk_std_core_while_fun21933__t* _self = kk_function_as(struct kk_std_core_while_fun21933__t*, _fself);
  kk_function_t action1 = _self->action1; /* () -> <div|16768> () */
  kk_function_t predicate1 = _self->predicate1; /* () -> <div|16768> bool */
  kk_drop_match(_self, {kk_function_dup(action1);kk_function_dup(predicate1);}, {}, _ctx)
  kk_unit_t wild__1_19882 = kk_Unit;
  kk_unit_unbox(_b_19878);
  kk_unit_t _x21934 = kk_Unit;
  kk_std_core__mlift18059_while(action1, predicate1, wild__1_19882, _ctx);
  return kk_unit_box(_x21934);
}

kk_unit_t kk_std_core_while(kk_function_t predicate1, kk_function_t action1, kk_context_t* _ctx) { /* forall<e> (predicate : () -> <div|e> bool, action : () -> <div|e> ()) -> <div|e> () */ 
  kk__tailcall: ;
  bool x0_18424;
  kk_function_t _x21927 = kk_function_dup(predicate1); /*() -> <div|16768> bool*/
  x0_18424 = kk_function_call(bool, (kk_function_t, kk_context_t*), _x21927, (_x21927, _ctx)); /*bool*/
  if (kk_yielding(kk_context())) {
    kk_box_t _x21928 = kk_std_core_hnd_yield_extend(kk_std_core_new_while_fun21929(action1, predicate1, _ctx), _ctx); /*3926*/
    kk_unit_unbox(_x21928); return kk_Unit;
  }
  if (x0_18424) {
    kk_unit_t x1_18427 = kk_Unit;
    kk_function_t _x21931 = kk_function_dup(action1); /*() -> <div|16768> ()*/
    kk_function_call(kk_unit_t, (kk_function_t, kk_context_t*), _x21931, (_x21931, _ctx));
    if (kk_yielding(kk_context())) {
      kk_box_t _x21932 = kk_std_core_hnd_yield_extend(kk_std_core_new_while_fun21933(action1, predicate1, _ctx), _ctx); /*3926*/
      kk_unit_unbox(_x21932); return kk_Unit;
    }
    { // tailcall
      goto kk__tailcall;
    }
  }
  {
    kk_function_drop(predicate1, _ctx);
    kk_function_drop(action1, _ctx);
    kk_Unit; return kk_Unit;
  }
}
 
// Zip two lists together by pairing the corresponding elements.
// The returned list is only as long as the smallest input list.

kk_std_core__list kk_std_core__ctail_zip(kk_std_core__list xs, kk_std_core__list ys, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* forall<a,b> (xs : list<a>, ys : list<b>, ctail<list<(a, b)>>) -> list<(a, b)> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21935 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con21935->head;
    kk_std_core__list xx = _con21935->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    if (kk_std_core__is_Cons(ys)) {
      struct kk_std_core_Cons* _con21936 = kk_std_core__as_Cons(ys);
      kk_box_t y = _con21936->head;
      kk_std_core__list yy = _con21936->tail;
      kk_reuse_t _ru_20450 = kk_reuse_null; /*reuse*/;
      if (kk_likely(kk_std_core__list_is_unique(ys))) {
        _ru_20450 = (kk_std_core__list_reuse(ys));
      }
      else {
        kk_box_dup(y);
        kk_std_core__list_dup(yy);
        kk_std_core__list_decref(ys, _ctx);
      }
      kk_std_core__list _ctail_17607 = kk_std_core__list_hole(); /*list<(16811, 16812)>*/;
      kk_std_core__list _ctail_17608;
      kk_box_t _x21937;
      kk_std_core_types__tuple2_ _x21938 = kk_std_core_types__new_dash__lp__comma__rp_(x, y, _ctx); /*(1004, 1005)*/
      _x21937 = kk_std_core_types__tuple2__box(_x21938, _ctx); /*61*/
      _ctail_17608 = kk_std_core__new_Cons(_ru_20450, _x21937, _ctail_17607, _ctx); /*list<(16811, 16812)>*/
      kk_box_t* _b_19899_19892 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17608)->tail)); /*cfield<list<(16811, 16812)>>*/;
      { // tailcall
        kk_std_core_types__ctail _x21939 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17608, _ctx)),_b_19899_19892); /*ctail<0>*/
        xs = xx;
        ys = yy;
        _acc = _x21939;
        goto kk__tailcall;
      }
    }
    {
      kk_std_core__list_drop(xx, _ctx);
      kk_box_drop(x, _ctx);
      kk_box_t _x21940 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
      return kk_std_core__list_unbox(_x21940, _ctx);
    }
  }
  {
    kk_std_core__list_drop(ys, _ctx);
    kk_box_t _x21941 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21941, _ctx);
  }
}
 
// Zip two lists together by pairing the corresponding elements.
// The returned list is only as long as the smallest input list.

kk_std_core__list kk_std_core_zip(kk_std_core__list xs0, kk_std_core__list ys0, kk_context_t* _ctx) { /* forall<a,b> (xs : list<a>, ys : list<b>) -> list<(a, b)> */ 
  kk_std_core_types__ctail _x21942 = kk_ctail_nil(); /*ctail<0>*/
  return kk_std_core__ctail_zip(xs0, ys0, _x21942, _ctx);
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift18061_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_std_core__list xx, kk_std_core__list yy, kk_box_t _ctail_17609, kk_context_t* _ctx) { /* forall<a,b,c,e> (ctail<list<c>>, f : (a, b) -> e c, xx : list<a>, yy : list<b>, c) -> e list<c> */ 
  kk_std_core__list _ctail_17610 = kk_std_core__list_hole(); /*list<16859>*/;
  kk_std_core__list _ctail_17611 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17609, _ctail_17610, _ctx); /*list<16859>*/;
  kk_box_t* _b_19915_19912 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17611)->tail)); /*cfield<list<16859>>*/;
  kk_std_core_types__ctail _x21943 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17611, _ctx)),_b_19915_19912); /*ctail<0>*/
  return kk_std_core__ctail_zipwith(xx, yy, f, _x21943, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift18062_op_fun21944__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t _ctail_17614;
};
static kk_std_core__list kk_std_core__mlift18062_op_fun21944(kk_function_t _fself, kk_std_core__list _ctail_17613, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift18062_op_fun21944(kk_function_t _accm, kk_box_t _ctail_17614, kk_context_t* _ctx) {
  struct kk_std_core__mlift18062_op_fun21944__t* _self = kk_function_alloc_as(struct kk_std_core__mlift18062_op_fun21944__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift18062_op_fun21944, kk_context());
  _self->_accm = _accm;
  _self->_ctail_17614 = _ctail_17614;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift18062_op_fun21944(kk_function_t _fself, kk_std_core__list _ctail_17613, kk_context_t* _ctx) {
  struct kk_std_core__mlift18062_op_fun21944__t* _self = kk_function_as(struct kk_std_core__mlift18062_op_fun21944__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<16859>) -> list<16859> */
  kk_box_t _ctail_17614 = _self->_ctail_17614; /* 16859 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(_ctail_17614);}, {}, _ctx)
  kk_std_core__list _x21945 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17614, _ctail_17613, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x21945, _ctx));
}

kk_std_core__list kk_std_core__mlift18062_op(kk_function_t _accm, kk_function_t f0, kk_std_core__list xx0, kk_std_core__list yy0, kk_box_t _ctail_17614, kk_context_t* _ctx) { /* forall<a,b,c,e> ((list<c>) -> list<c>, f : (a, b) -> e c, xx : list<a>, yy : list<b>, c) -> e list<c> */ 
  return kk_std_core__ctailm_zipwith(xx0, yy0, f0, kk_std_core__new_mlift18062_op_fun21944(_accm, _ctail_17614, _ctx), _ctx);
}
 
// Zip two lists together by apply a function `f` to all corresponding elements.
// The returned list is only as long as the smallest input list.


// lift anonymous function
struct kk_std_core__ctail_zipwith_fun21950__t {
  struct kk_function_s _base;
  kk_function_t f1;
  kk_std_core__list xx1;
  kk_std_core__list yy1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_zipwith_fun21950(kk_function_t _fself, kk_box_t _b_19920, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_zipwith_fun21950(kk_function_t f1, kk_std_core__list xx1, kk_std_core__list yy1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_zipwith_fun21950__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_zipwith_fun21950__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_zipwith_fun21950, kk_context());
  _self->f1 = f1;
  _self->xx1 = xx1;
  _self->yy1 = yy1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_zipwith_fun21950(kk_function_t _fself, kk_box_t _b_19920, kk_context_t* _ctx) {
  struct kk_std_core__ctail_zipwith_fun21950__t* _self = kk_function_as(struct kk_std_core__ctail_zipwith_fun21950__t*, _fself);
  kk_function_t f1 = _self->f1; /* (16857, 16858) -> 16860 16859 */
  kk_std_core__list xx1 = _self->xx1; /* list<16857> */
  kk_std_core__list yy1 = _self->yy1; /* list<16858> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<16859>> */
  kk_drop_match(_self, {kk_function_dup(f1);kk_std_core__list_dup(xx1);kk_std_core__list_dup(yy1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_box_t _ctail_19942_176090 = _b_19920; /*16859*/;
  kk_std_core__list _x21951 = kk_std_core__mlift18061_op(_acc0, f1, xx1, yy1, _ctail_19942_176090, _ctx); /*list<16859>*/
  return kk_std_core__list_box(_x21951, _ctx);
}

kk_std_core__list kk_std_core__ctail_zipwith(kk_std_core__list xs, kk_std_core__list ys, kk_function_t f1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,b,c,e> (xs : list<a>, ys : list<b>, f : (a, b) -> e c, ctail<list<c>>) -> e list<c> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21946 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con21946->head;
    kk_std_core__list xx1 = _con21946->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx1);
      kk_std_core__list_decref(xs, _ctx);
    }
    if (kk_std_core__is_Cons(ys)) {
      struct kk_std_core_Cons* _con21947 = kk_std_core__as_Cons(ys);
      kk_box_t y = _con21947->head;
      kk_std_core__list yy1 = _con21947->tail;
      kk_reuse_t _ru_20452 = kk_reuse_null; /*reuse*/;
      if (kk_likely(kk_std_core__list_is_unique(ys))) {
        _ru_20452 = (kk_std_core__list_reuse(ys));
      }
      else {
        kk_box_dup(y);
        kk_std_core__list_dup(yy1);
        kk_std_core__list_decref(ys, _ctx);
      }
      kk_box_t x0_18430;
      kk_function_t _x21948 = kk_function_dup(f1); /*(16857, 16858) -> 16860 16859*/
      x0_18430 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_context_t*), _x21948, (_x21948, x, y, _ctx)); /*16859*/
      if (kk_yielding(kk_context())) {
        kk_reuse_drop(_ru_20452,kk_context());
        kk_box_drop(x0_18430, _ctx);
        kk_box_t _x21949 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_zipwith_fun21950(f1, xx1, yy1, _acc0, _ctx), _ctx); /*3926*/
        return kk_std_core__list_unbox(_x21949, _ctx);
      }
      {
        kk_std_core__list _ctail_176100 = kk_std_core__list_hole(); /*list<16859>*/;
        kk_std_core__list _ctail_176110 = kk_std_core__new_Cons(_ru_20452, x0_18430, _ctail_176100, _ctx); /*list<16859>*/;
        kk_box_t* _b_19934_19926 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_176110)->tail)); /*cfield<list<16859>>*/;
        { // tailcall
          kk_std_core_types__ctail _x21952 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_176110, _ctx)),_b_19934_19926); /*ctail<0>*/
          xs = xx1;
          ys = yy1;
          _acc0 = _x21952;
          goto kk__tailcall;
        }
      }
    }
    {
      kk_std_core__list_drop(xx1, _ctx);
      kk_box_drop(x, _ctx);
      kk_function_drop(f1, _ctx);
      kk_box_t _x21953 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
      return kk_std_core__list_unbox(_x21953, _ctx);
    }
  }
  {
    kk_std_core__list_drop(ys, _ctx);
    kk_function_drop(f1, _ctx);
    kk_box_t _x21954 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21954, _ctx);
  }
}
 
// Zip two lists together by apply a function `f` to all corresponding elements.
// The returned list is only as long as the smallest input list.


// lift anonymous function
struct kk_std_core__ctailm_zipwith_fun21959__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t f2;
  kk_std_core__list xx2;
  kk_std_core__list yy2;
};
static kk_box_t kk_std_core__ctailm_zipwith_fun21959(kk_function_t _fself, kk_box_t _b_19944, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_zipwith_fun21959(kk_function_t _accm0, kk_function_t f2, kk_std_core__list xx2, kk_std_core__list yy2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_zipwith_fun21959__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_zipwith_fun21959__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_zipwith_fun21959, kk_context());
  _self->_accm0 = _accm0;
  _self->f2 = f2;
  _self->xx2 = xx2;
  _self->yy2 = yy2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_zipwith_fun21959(kk_function_t _fself, kk_box_t _b_19944, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_zipwith_fun21959__t* _self = kk_function_as(struct kk_std_core__ctailm_zipwith_fun21959__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<16859>) -> list<16859> */
  kk_function_t f2 = _self->f2; /* (16857, 16858) -> 16860 16859 */
  kk_std_core__list xx2 = _self->xx2; /* list<16857> */
  kk_std_core__list yy2 = _self->yy2; /* list<16858> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(f2);kk_std_core__list_dup(xx2);kk_std_core__list_dup(yy2);}, {}, _ctx)
  kk_box_t _ctail_19946_176140 = _b_19944; /*16859*/;
  kk_std_core__list _x21960 = kk_std_core__mlift18062_op(_accm0, f2, xx2, yy2, _ctail_19946_176140, _ctx); /*list<16859>*/
  return kk_std_core__list_box(_x21960, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_zipwith_fun21962__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t x2_18433;
};
static kk_std_core__list kk_std_core__ctailm_zipwith_fun21962(kk_function_t _fself, kk_std_core__list _ctail_176130, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_zipwith_fun21962(kk_function_t _accm0, kk_box_t x2_18433, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_zipwith_fun21962__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_zipwith_fun21962__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_zipwith_fun21962, kk_context());
  _self->_accm0 = _accm0;
  _self->x2_18433 = x2_18433;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_zipwith_fun21962(kk_function_t _fself, kk_std_core__list _ctail_176130, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_zipwith_fun21962__t* _self = kk_function_as(struct kk_std_core__ctailm_zipwith_fun21962__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<16859>) -> list<16859> */
  kk_box_t x2_18433 = _self->x2_18433; /* 16859 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(x2_18433);}, {}, _ctx)
  kk_std_core__list _x21963 = kk_std_core__new_Cons(kk_reuse_null, x2_18433, _ctail_176130, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x21963, _ctx));
}

kk_std_core__list kk_std_core__ctailm_zipwith(kk_std_core__list xs0, kk_std_core__list ys0, kk_function_t f2, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,b,c,e> (xs : list<a>, ys : list<b>, f : (a, b) -> e c, (list<c>) -> list<c>) -> e list<c> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con21955 = kk_std_core__as_Cons(xs0);
    kk_box_t x1 = _con21955->head;
    kk_std_core__list xx2 = _con21955->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs0))) {
      kk_std_core__list_free(xs0, _ctx);
    }
    else {
      kk_box_dup(x1);
      kk_std_core__list_dup(xx2);
      kk_std_core__list_decref(xs0, _ctx);
    }
    if (kk_std_core__is_Cons(ys0)) {
      struct kk_std_core_Cons* _con21956 = kk_std_core__as_Cons(ys0);
      kk_box_t y0 = _con21956->head;
      kk_std_core__list yy2 = _con21956->tail;
      if (kk_likely(kk_std_core__list_is_unique(ys0))) {
        kk_std_core__list_free(ys0, _ctx);
      }
      else {
        kk_box_dup(y0);
        kk_std_core__list_dup(yy2);
        kk_std_core__list_decref(ys0, _ctx);
      }
      kk_box_t x2_18433;
      kk_function_t _x21957 = kk_function_dup(f2); /*(16857, 16858) -> 16860 16859*/
      x2_18433 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_context_t*), _x21957, (_x21957, x1, y0, _ctx)); /*16859*/
      if (kk_yielding(kk_context())) {
        kk_box_drop(x2_18433, _ctx);
        kk_box_t _x21958 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_zipwith_fun21959(_accm0, f2, xx2, yy2, _ctx), _ctx); /*3926*/
        return kk_std_core__list_unbox(_x21958, _ctx);
      }
      { // tailcall
        kk_function_t _x21961 = kk_std_core__new_ctailm_zipwith_fun21962(_accm0, x2_18433, _ctx); /*(list<16859>) -> list<16859>*/
        xs0 = xx2;
        ys0 = yy2;
        _accm0 = _x21961;
        goto kk__tailcall;
      }
    }
    {
      kk_std_core__list_drop(xx2, _ctx);
      kk_box_drop(x1, _ctx);
      kk_function_drop(f2, _ctx);
      return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
    }
  }
  {
    kk_std_core__list_drop(ys0, _ctx);
    kk_function_drop(f2, _ctx);
    return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
  }
}
 
// Zip two lists together by apply a function `f` to all corresponding elements.
// The returned list is only as long as the smallest input list.


// lift anonymous function
struct kk_std_core_zipwith_fun21965__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core_zipwith_fun21965(kk_function_t _fself, kk_std_core__list _ctail_17612, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_zipwith_fun21965(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_zipwith_fun21965, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core_zipwith_fun21965(kk_function_t _fself, kk_std_core__list _ctail_17612, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17612;
}

kk_std_core__list kk_std_core_zipwith(kk_std_core__list xs1, kk_std_core__list ys1, kk_function_t f3, kk_context_t* _ctx) { /* forall<a,b,c,e> (xs : list<a>, ys : list<b>, f : (a, b) -> e c) -> e list<c> */ 
  bool _match_19990 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_19990) {
    kk_std_core_types__ctail _x21964 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_zipwith(xs1, ys1, f3, _x21964, _ctx);
  }
  {
    return kk_std_core__ctailm_zipwith(xs1, ys1, f3, kk_std_core_new_zipwith_fun21965(_ctx), _ctx);
  }
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift18063_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_integer_t i0_17475, kk_std_core__list xx, kk_std_core__list yy, kk_box_t _ctail_17615, kk_context_t* _ctx) { /* forall<a,b,c,e> (ctail<list<c>>, f : (int, a, b) -> e c, i0.17475 : int, xx : list<a>, yy : list<b>, c) -> e list<c> */ 
  kk_std_core__list _ctail_17616 = kk_std_core__list_hole(); /*list<16933>*/;
  kk_std_core__list _ctail_17617 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17615, _ctail_17616, _ctx); /*list<16933>*/;
  kk_box_t* _b_19955_19952 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17617)->tail)); /*cfield<list<16933>>*/;
  kk_std_core_types__ctail _x21966 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17617, _ctx)),_b_19955_19952); /*ctail<0>*/
  return kk_std_core__ctail_lift17302_zipwith_indexed(f, i0_17475, xx, yy, _x21966, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift18064_op_fun21967__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t _ctail_17620;
};
static kk_std_core__list kk_std_core__mlift18064_op_fun21967(kk_function_t _fself, kk_std_core__list _ctail_17619, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift18064_op_fun21967(kk_function_t _accm, kk_box_t _ctail_17620, kk_context_t* _ctx) {
  struct kk_std_core__mlift18064_op_fun21967__t* _self = kk_function_alloc_as(struct kk_std_core__mlift18064_op_fun21967__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift18064_op_fun21967, kk_context());
  _self->_accm = _accm;
  _self->_ctail_17620 = _ctail_17620;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift18064_op_fun21967(kk_function_t _fself, kk_std_core__list _ctail_17619, kk_context_t* _ctx) {
  struct kk_std_core__mlift18064_op_fun21967__t* _self = kk_function_as(struct kk_std_core__mlift18064_op_fun21967__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<16933>) -> list<16933> */
  kk_box_t _ctail_17620 = _self->_ctail_17620; /* 16933 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(_ctail_17620);}, {}, _ctx)
  kk_std_core__list _x21968 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17620, _ctail_17619, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x21968, _ctx));
}

kk_std_core__list kk_std_core__mlift18064_op(kk_function_t _accm, kk_function_t f0, kk_integer_t i0_174750, kk_std_core__list xx0, kk_std_core__list yy0, kk_box_t _ctail_17620, kk_context_t* _ctx) { /* forall<a,b,c,e> ((list<c>) -> list<c>, f : (int, a, b) -> e c, i0.17475 : int, xx : list<a>, yy : list<b>, c) -> e list<c> */ 
  return kk_std_core__ctailm_lift17302_zipwith_indexed(f0, i0_174750, xx0, yy0, kk_std_core__new_mlift18064_op_fun21967(_accm, _ctail_17620, _ctx), _ctx);
}
 
// lifted local: zipwith-indexed, zipwith-iter


// lift anonymous function
struct kk_std_core__ctail_lift17302_zipwith_indexed_fun21974__t {
  struct kk_function_s _base;
  kk_function_t f1;
  kk_integer_t i0_174751;
  kk_std_core__list xx1;
  kk_std_core__list yy1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_lift17302_zipwith_indexed_fun21974(kk_function_t _fself, kk_box_t _b_19960, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_lift17302_zipwith_indexed_fun21974(kk_function_t f1, kk_integer_t i0_174751, kk_std_core__list xx1, kk_std_core__list yy1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_lift17302_zipwith_indexed_fun21974__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_lift17302_zipwith_indexed_fun21974__t, 6, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_lift17302_zipwith_indexed_fun21974, kk_context());
  _self->f1 = f1;
  _self->i0_174751 = i0_174751;
  _self->xx1 = xx1;
  _self->yy1 = yy1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_lift17302_zipwith_indexed_fun21974(kk_function_t _fself, kk_box_t _b_19960, kk_context_t* _ctx) {
  struct kk_std_core__ctail_lift17302_zipwith_indexed_fun21974__t* _self = kk_function_as(struct kk_std_core__ctail_lift17302_zipwith_indexed_fun21974__t*, _fself);
  kk_function_t f1 = _self->f1; /* (int, 16931, 16932) -> 16934 16933 */
  kk_integer_t i0_174751 = _self->i0_174751; /* int */
  kk_std_core__list xx1 = _self->xx1; /* list<16931> */
  kk_std_core__list yy1 = _self->yy1; /* list<16932> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<16933>> */
  kk_drop_match(_self, {kk_function_dup(f1);kk_integer_dup(i0_174751);kk_std_core__list_dup(xx1);kk_std_core__list_dup(yy1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_box_t _ctail_19982_176150 = _b_19960; /*16933*/;
  kk_std_core__list _x21975 = kk_std_core__mlift18063_op(_acc0, f1, i0_174751, xx1, yy1, _ctail_19982_176150, _ctx); /*list<16933>*/
  return kk_std_core__list_box(_x21975, _ctx);
}

kk_std_core__list kk_std_core__ctail_lift17302_zipwith_indexed(kk_function_t f1, kk_integer_t i, kk_std_core__list xs, kk_std_core__list ys, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,b,c,e> (f : (int, a, b) -> e c, i : int, xs : list<a>, ys : list<b>, ctail<list<c>>) -> e list<c> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21969 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con21969->head;
    kk_std_core__list xx1 = _con21969->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx1);
      kk_std_core__list_decref(xs, _ctx);
    }
    if (kk_std_core__is_Cons(ys)) {
      struct kk_std_core_Cons* _con21970 = kk_std_core__as_Cons(ys);
      kk_box_t y = _con21970->head;
      kk_std_core__list yy1 = _con21970->tail;
      kk_reuse_t _ru_20456 = kk_reuse_null; /*reuse*/;
      if (kk_likely(kk_std_core__list_is_unique(ys))) {
        _ru_20456 = (kk_std_core__list_reuse(ys));
      }
      else {
        kk_box_dup(y);
        kk_std_core__list_dup(yy1);
        kk_std_core__list_decref(ys, _ctx);
      }
      kk_integer_t i0_174751;
      kk_integer_t _x21971 = kk_integer_dup(i); /*int*/
      i0_174751 = kk_integer_add_small_const(_x21971, 1, _ctx); /*int*/
      kk_box_t x0_18436;
      kk_function_t _x21972 = kk_function_dup(f1); /*(int, 16931, 16932) -> 16934 16933*/
      x0_18436 = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_box_t, kk_box_t, kk_context_t*), _x21972, (_x21972, i, x, y, _ctx)); /*16933*/
      if (kk_yielding(kk_context())) {
        kk_reuse_drop(_ru_20456,kk_context());
        kk_box_drop(x0_18436, _ctx);
        kk_box_t _x21973 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_lift17302_zipwith_indexed_fun21974(f1, i0_174751, xx1, yy1, _acc0, _ctx), _ctx); /*3926*/
        return kk_std_core__list_unbox(_x21973, _ctx);
      }
      {
        kk_std_core__list _ctail_176160 = kk_std_core__list_hole(); /*list<16933>*/;
        kk_std_core__list _ctail_176170 = kk_std_core__new_Cons(_ru_20456, x0_18436, _ctail_176160, _ctx); /*list<16933>*/;
        kk_box_t* _b_19974_19966 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_176170)->tail)); /*cfield<list<16933>>*/;
        { // tailcall
          kk_std_core_types__ctail _x21976 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_176170, _ctx)),_b_19974_19966); /*ctail<0>*/
          i = i0_174751;
          xs = xx1;
          ys = yy1;
          _acc0 = _x21976;
          goto kk__tailcall;
        }
      }
    }
    {
      kk_std_core__list_drop(xx1, _ctx);
      kk_box_drop(x, _ctx);
      kk_integer_drop(i, _ctx);
      kk_function_drop(f1, _ctx);
      kk_box_t _x21977 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
      return kk_std_core__list_unbox(_x21977, _ctx);
    }
  }
  {
    kk_std_core__list_drop(ys, _ctx);
    kk_integer_drop(i, _ctx);
    kk_function_drop(f1, _ctx);
    kk_box_t _x21978 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21978, _ctx);
  }
}
 
// lifted local: zipwith-indexed, zipwith-iter


// lift anonymous function
struct kk_std_core__ctailm_lift17302_zipwith_indexed_fun21984__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t f2;
  kk_integer_t i0_174752;
  kk_std_core__list xx2;
  kk_std_core__list yy2;
};
static kk_box_t kk_std_core__ctailm_lift17302_zipwith_indexed_fun21984(kk_function_t _fself, kk_box_t _b_19984, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_lift17302_zipwith_indexed_fun21984(kk_function_t _accm0, kk_function_t f2, kk_integer_t i0_174752, kk_std_core__list xx2, kk_std_core__list yy2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17302_zipwith_indexed_fun21984__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_lift17302_zipwith_indexed_fun21984__t, 6, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_lift17302_zipwith_indexed_fun21984, kk_context());
  _self->_accm0 = _accm0;
  _self->f2 = f2;
  _self->i0_174752 = i0_174752;
  _self->xx2 = xx2;
  _self->yy2 = yy2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_lift17302_zipwith_indexed_fun21984(kk_function_t _fself, kk_box_t _b_19984, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17302_zipwith_indexed_fun21984__t* _self = kk_function_as(struct kk_std_core__ctailm_lift17302_zipwith_indexed_fun21984__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<16933>) -> list<16933> */
  kk_function_t f2 = _self->f2; /* (int, 16931, 16932) -> 16934 16933 */
  kk_integer_t i0_174752 = _self->i0_174752; /* int */
  kk_std_core__list xx2 = _self->xx2; /* list<16931> */
  kk_std_core__list yy2 = _self->yy2; /* list<16932> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(f2);kk_integer_dup(i0_174752);kk_std_core__list_dup(xx2);kk_std_core__list_dup(yy2);}, {}, _ctx)
  kk_box_t _ctail_19986_176200 = _b_19984; /*16933*/;
  kk_std_core__list _x21985 = kk_std_core__mlift18064_op(_accm0, f2, i0_174752, xx2, yy2, _ctail_19986_176200, _ctx); /*list<16933>*/
  return kk_std_core__list_box(_x21985, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_lift17302_zipwith_indexed_fun21987__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t x2_18439;
};
static kk_std_core__list kk_std_core__ctailm_lift17302_zipwith_indexed_fun21987(kk_function_t _fself, kk_std_core__list _ctail_176190, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_lift17302_zipwith_indexed_fun21987(kk_function_t _accm0, kk_box_t x2_18439, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17302_zipwith_indexed_fun21987__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_lift17302_zipwith_indexed_fun21987__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_lift17302_zipwith_indexed_fun21987, kk_context());
  _self->_accm0 = _accm0;
  _self->x2_18439 = x2_18439;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_lift17302_zipwith_indexed_fun21987(kk_function_t _fself, kk_std_core__list _ctail_176190, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17302_zipwith_indexed_fun21987__t* _self = kk_function_as(struct kk_std_core__ctailm_lift17302_zipwith_indexed_fun21987__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<16933>) -> list<16933> */
  kk_box_t x2_18439 = _self->x2_18439; /* 16933 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(x2_18439);}, {}, _ctx)
  kk_std_core__list _x21988 = kk_std_core__new_Cons(kk_reuse_null, x2_18439, _ctail_176190, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x21988, _ctx));
}

kk_std_core__list kk_std_core__ctailm_lift17302_zipwith_indexed(kk_function_t f2, kk_integer_t i0, kk_std_core__list xs0, kk_std_core__list ys0, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,b,c,e> (f : (int, a, b) -> e c, i : int, xs : list<a>, ys : list<b>, (list<c>) -> list<c>) -> e list<c> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con21979 = kk_std_core__as_Cons(xs0);
    kk_box_t x1 = _con21979->head;
    kk_std_core__list xx2 = _con21979->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs0))) {
      kk_std_core__list_free(xs0, _ctx);
    }
    else {
      kk_box_dup(x1);
      kk_std_core__list_dup(xx2);
      kk_std_core__list_decref(xs0, _ctx);
    }
    if (kk_std_core__is_Cons(ys0)) {
      struct kk_std_core_Cons* _con21980 = kk_std_core__as_Cons(ys0);
      kk_box_t y0 = _con21980->head;
      kk_std_core__list yy2 = _con21980->tail;
      if (kk_likely(kk_std_core__list_is_unique(ys0))) {
        kk_std_core__list_free(ys0, _ctx);
      }
      else {
        kk_box_dup(y0);
        kk_std_core__list_dup(yy2);
        kk_std_core__list_decref(ys0, _ctx);
      }
      kk_integer_t i0_174752;
      kk_integer_t _x21981 = kk_integer_dup(i0); /*int*/
      i0_174752 = kk_integer_add_small_const(_x21981, 1, _ctx); /*int*/
      kk_box_t x2_18439;
      kk_function_t _x21982 = kk_function_dup(f2); /*(int, 16931, 16932) -> 16934 16933*/
      x2_18439 = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_box_t, kk_box_t, kk_context_t*), _x21982, (_x21982, i0, x1, y0, _ctx)); /*16933*/
      if (kk_yielding(kk_context())) {
        kk_box_drop(x2_18439, _ctx);
        kk_box_t _x21983 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_lift17302_zipwith_indexed_fun21984(_accm0, f2, i0_174752, xx2, yy2, _ctx), _ctx); /*3926*/
        return kk_std_core__list_unbox(_x21983, _ctx);
      }
      { // tailcall
        kk_function_t _x21986 = kk_std_core__new_ctailm_lift17302_zipwith_indexed_fun21987(_accm0, x2_18439, _ctx); /*(list<16933>) -> list<16933>*/
        i0 = i0_174752;
        xs0 = xx2;
        ys0 = yy2;
        _accm0 = _x21986;
        goto kk__tailcall;
      }
    }
    {
      kk_std_core__list_drop(xx2, _ctx);
      kk_box_drop(x1, _ctx);
      kk_integer_drop(i0, _ctx);
      kk_function_drop(f2, _ctx);
      return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
    }
  }
  {
    kk_std_core__list_drop(ys0, _ctx);
    kk_integer_drop(i0, _ctx);
    kk_function_drop(f2, _ctx);
    return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
  }
}
 
// lifted local: zipwith-indexed, zipwith-iter


// lift anonymous function
struct kk_std_core__lift17302_zipwith_indexed_fun21990__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core__lift17302_zipwith_indexed_fun21990(kk_function_t _fself, kk_std_core__list _ctail_17618, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17302_zipwith_indexed_fun21990(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core__lift17302_zipwith_indexed_fun21990, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core__lift17302_zipwith_indexed_fun21990(kk_function_t _fself, kk_std_core__list _ctail_17618, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17618;
}

kk_std_core__list kk_std_core__lift17302_zipwith_indexed(kk_function_t f3, kk_integer_t i1, kk_std_core__list xs1, kk_std_core__list ys1, kk_context_t* _ctx) { /* forall<a,b,c,e> (f : (int, a, b) -> e c, i : int, xs : list<a>, ys : list<b>) -> e list<c> */ 
  bool _match_19987 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_19987) {
    kk_std_core_types__ctail _x21989 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_lift17302_zipwith_indexed(f3, i1, xs1, ys1, _x21989, _ctx);
  }
  {
    return kk_std_core__ctailm_lift17302_zipwith_indexed(f3, i1, xs1, ys1, kk_std_core__new_lift17302_zipwith_indexed_fun21990(_ctx), _ctx);
  }
}

// initialization
void kk_std_core__init(kk_context_t* _ctx){
  static bool _kk_initialized = false;
  if (_kk_initialized) return;
  _kk_initialized = true;
  kk_std_core_types__init(_ctx);
  kk_std_core_hnd__init(_ctx);
  #if defined(KK_CUSTOM_INIT)
    KK_CUSTOM_INIT (_ctx);
  #endif
  {
    kk_string_t _x20487;
    kk_define_string_literal(, _s20488, 8, "exn.core")
    _x20487 = kk_string_dup(_s20488); /*string*/
    kk_std_core__tag_exn = kk_std_core_hnd__new_Htag(_x20487, _ctx); /*std/core/hnd/htag<.hnd-exn>*/
  }
  {
    kk_std_core_redirect = kk_ref_alloc((kk_std_core_types__maybe_box(kk_std_core_types__new_Nothing(_ctx), _ctx)),kk_context()); /*ref<global,maybe<(string) -> console ()>>*/
  }
  {
    kk_std_core_trace_enabled = kk_ref_alloc((kk_bool_box(true)),kk_context()); /*ref<global,bool>*/
  }
  {
    kk_std_core_unique_count = kk_ref_alloc((kk_integer_box(kk_integer_from_small(0))),kk_context()); /*ref<global,int>*/
  }
  {
    kk_string_t _x21344 = kk_string_empty(); /*string*/
    kk_ssize_t _x21346 = (KK_IZ(0)); /*ssize_t*/
    kk_ssize_t _x21347 = (KK_IZ(0)); /*ssize_t*/
    kk_std_core_empty = kk_std_core__new_Sslice(_x21344, _x21346, _x21347, _ctx); /*sslice*/
  }
  {
    kk_string_t _x21569 = kk_string_empty(); /*string*/
    kk_ssize_t _x21571 = (KK_IZ(-1)); /*ssize_t*/
    kk_ssize_t _x21572 = (KK_IZ(0)); /*ssize_t*/
    kk_std_core_invalid = kk_std_core__new_Sslice(_x21569, _x21571, _x21572, _ctx); /*sslice*/
  }
}

// termination
void kk_std_core__done(kk_context_t* _ctx){
  static bool _kk_done = false;
  if (_kk_done) return;
  _kk_done = true;
  #if defined(KK_CUSTOM_DONE)
    KK_CUSTOM_DONE (_ctx);
  #endif
  kk_std_core__sslice_drop(kk_std_core_invalid, _ctx);
  kk_std_core__sslice_drop(kk_std_core_empty, _ctx);
  kk_ref_drop(kk_std_core_unique_count, _ctx);
  kk_ref_drop(kk_std_core_trace_enabled, _ctx);
  kk_ref_drop(kk_std_core_redirect, _ctx);
  kk_std_core_hnd__htag_drop(kk_std_core__tag_exn, _ctx);
  kk_std_core_hnd__done(_ctx);
  kk_std_core_types__done(_ctx);
}
