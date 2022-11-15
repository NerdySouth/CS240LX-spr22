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
  kk_string_t _x19990;
  if (kk_std_core_types__is_Optional(message0)) {
    kk_box_t _box_x18179 = message0._cons.Optional.value;
    kk_string_t _message_1688 = kk_string_unbox(_box_x18179);
    _x19990 = _message_1688; /*string*/
  }
  else {
    kk_string_t _x = _this.message;
    kk_string_dup(_x);
    _x19990 = _x; /*string*/
  }
  kk_std_core__exception_info _x19992;
  if (kk_std_core_types__is_Optional(info0)) {
    kk_box_t _box_x18180 = info0._cons.Optional.value;
    kk_std_core__exception_info _info_1694 = kk_std_core__exception_info_unbox(_box_x18180, NULL);
    kk_std_core__exception_drop(_this, _ctx);
    _x19992 = _info_1694; /*exception-info*/
  }
  else {
    kk_std_core__exception_info _x0 = _this.info;
    kk_std_core__exception_info_dup(_x0);
    kk_std_core__exception_drop(_this, _ctx);
    _x19992 = _x0; /*exception-info*/
  }
  return kk_std_core__new_Exception(_x19990, _x19992, _ctx);
}

kk_std_core__sslice kk_std_core__copy_2(kk_std_core__sslice _this, kk_std_core_types__optional str0, kk_std_core_types__optional start0, kk_std_core_types__optional len0, kk_context_t* _ctx) { /* (sslice, str : optional<string>, start : optional<ssize_t>, len : optional<ssize_t>) -> sslice */ 
  kk_string_t _x19997;
  if (kk_std_core_types__is_Optional(str0)) {
    kk_box_t _box_x18182 = str0._cons.Optional.value;
    kk_string_t _str_1826 = kk_string_unbox(_box_x18182);
    _x19997 = _str_1826; /*string*/
  }
  else {
    kk_string_t _x = _this.str;
    kk_string_dup(_x);
    _x19997 = _x; /*string*/
  }
  kk_ssize_t _x19999;
  if (kk_std_core_types__is_Optional(start0)) {
    kk_box_t _box_x18183 = start0._cons.Optional.value;
    kk_ssize_t _start_1832 = kk_ssize_unbox(_box_x18183, NULL);
    kk_std_core_types__optional_drop(start0, _ctx);
    _x19999 = _start_1832; /*ssize_t*/
  }
  else {
    kk_ssize_t _x0 = _this.start;
    _x19999 = _x0; /*ssize_t*/
  }
  kk_ssize_t _x20001;
  if (kk_std_core_types__is_Optional(len0)) {
    kk_box_t _box_x18184 = len0._cons.Optional.value;
    kk_ssize_t _len_1838 = kk_ssize_unbox(_box_x18184, NULL);
    kk_std_core_types__optional_drop(len0, _ctx);
    kk_std_core__sslice_drop(_this, _ctx);
    _x20001 = _len_1838; /*ssize_t*/
  }
  else {
    kk_ssize_t _x1 = _this.len;
    kk_std_core__sslice_drop(_this, _ctx);
    _x20001 = _x1; /*ssize_t*/
  }
  return kk_std_core__new_Sslice(_x19997, _x19999, _x20001, _ctx);
}

kk_std_core__stream kk_std_core__copy_3(kk_std_core__stream _this, kk_std_core_types__optional head0, kk_std_core_types__optional tail0, kk_context_t* _ctx) { /* forall<a> (stream<a>, head : optional<a>, tail : optional<stream<a>>) -> stream<a> */ 
  kk_box_t _x20005;
  if (kk_std_core_types__is_Optional(head0)) {
    kk_box_t _head_1884 = head0._cons.Optional.value;
    _x20005 = _head_1884; /*1901*/
  }
  else {
    struct kk_std_core_Next* _con20006 = kk_std_core__as_Next(_this);
    kk_box_t _x = _con20006->head;
    kk_box_dup(_x);
    _x20005 = _x; /*1901*/
  }
  kk_std_core__stream _x20007;
  if (kk_std_core_types__is_Optional(tail0)) {
    kk_box_t _box_x18185 = tail0._cons.Optional.value;
    kk_std_core__stream _tail_1891 = kk_std_core__stream_unbox(_box_x18185, NULL);
    kk_std_core__stream_drop(_this, _ctx);
    _x20007 = _tail_1891; /*stream<1901>*/
  }
  else {
    struct kk_std_core_Next* _con20009 = kk_std_core__as_Next(_this);
    kk_box_t _pat01 = _con20009->head;
    kk_std_core__stream _x0 = _con20009->tail;
    if (kk_likely(kk_std_core__stream_is_unique(_this))) {
      kk_box_drop(_pat01, _ctx);
      kk_std_core__stream_free(_this, _ctx);
    }
    else {
      kk_std_core__stream_dup(_x0);
      kk_std_core__stream_decref(_this, _ctx);
    }
    _x20007 = _x0; /*stream<1901>*/
  }
  return kk_std_core__new_Next(kk_reuse_null, _x20005, _x20007, _ctx);
}
 
// runtime tag for the `:exn` effect

kk_std_core_hnd__htag kk_std_core__tag_exn;
 
// handler for the `:exn` effect

kk_box_t kk_std_core__handle_exn(int32_t cfc, kk_std_core__hnd_exn hnd, kk_function_t ret, kk_function_t action, kk_context_t* _ctx) { /* forall<a,e,b> (cfc : int32, hnd : .hnd-exn<e,b>, ret : (res : a) -> e b, action : () -> <exn|e> a) -> e b */ 
  kk_std_core_hnd__htag _x20013 = kk_std_core_hnd__htag_dup(kk_std_core__tag_exn); /*std/core/hnd/htag<.hnd-exn>*/
  return kk_std_core_hnd__hhandle(_x20013, cfc, kk_std_core__hnd_exn_box(hnd, _ctx), ret, action, _ctx);
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
  kk_std_core__list _x20016;
  if (kk_std_core_types__is_Optional(tail0)) {
    kk_box_t _box_x18199 = tail0._cons.Optional.value;
    kk_std_core__list _tail_2132 = kk_std_core__list_unbox(_box_x18199, NULL);
    _x20016 = _tail_2132; /*list<2142>*/
  }
  else {
    _x20016 = kk_std_core__new_Nil(_ctx); /*list<2142>*/
  }
  return kk_vector_to_list(v,_x20016,kk_context());
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
  kk_integer_t _brw_19890;
  if (kk_std_core_types__is_Optional(upto)) {
    kk_box_t _box_x18200 = upto._cons.Optional.value;
    kk_integer_t _upto_2231 = kk_integer_unbox(_box_x18200);
    _brw_19890 = kk_integer_dup(_upto_2231); /*int*/
  }
  else {
    _brw_19890 = kk_integer_from_small(-1); /*int*/
  }
  kk_std_core__sslice _brw_19891 = kk_slice_common_prefix_borrow(s,t,_brw_19890,kk_context()); /*sslice*/;
  kk_integer_drop(_brw_19890, _ctx);
  return _brw_19891;
}
 
// lifted local: concat, concat-pre

kk_std_core__list kk_std_core__ctail_lift17183_concat(kk_std_core__list ys, kk_std_core__list zss, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* forall<a> (ys : list<a>, zss : list<list<a>>, ctail<list<a>>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con20019 = kk_std_core__as_Cons(ys);
    kk_box_t y = _con20019->head;
    kk_std_core__list yy = _con20019->tail;
    kk_reuse_t _ru_19893 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      _ru_19893 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(y);
      kk_std_core__list_dup(yy);
      kk_std_core__list_decref(ys, _ctx);
    }
    kk_std_core__list _ctail_17236 = kk_std_core__list_hole(); /*list<2279>*/;
    kk_std_core__list _ctail_17237 = kk_std_core__new_Cons(_ru_19893, y, _ctail_17236, _ctx); /*list<2279>*/;
    kk_box_t* _b_18212_18206 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17237)->tail)); /*cfield<list<2279>>*/;
    { // tailcall
      kk_std_core_types__ctail _x20020 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17237, _ctx)),_b_18212_18206); /*ctail<0>*/
      ys = yy;
      _acc = _x20020;
      goto kk__tailcall;
    }
  }
  if (kk_std_core__is_Cons(zss)) {
    struct kk_std_core_Cons* _con20021 = kk_std_core__as_Cons(zss);
    kk_box_t _box_x18207 = _con20021->head;
    kk_std_core__list zzs = _con20021->tail;
    kk_std_core__list zs = kk_std_core__list_unbox(_box_x18207, NULL);
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
    kk_box_t _x20023 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20023, _ctx);
  }
}
 
// lifted local: concat, concat-pre

kk_std_core__list kk_std_core__lift17183_concat(kk_std_core__list ys0, kk_std_core__list zss0, kk_context_t* _ctx) { /* forall<a> (ys : list<a>, zss : list<list<a>>) -> list<a> */ 
  kk_std_core_types__ctail _x20024 = kk_ctail_nil(); /*ctail<0>*/
  return kk_std_core__ctail_lift17183_concat(ys0, zss0, _x20024, _ctx);
}
extern kk_box_t kk_std_core_const_fun20025_1(kk_function_t _fself, kk_box_t ___wildcard__110__6, kk_context_t* _ctx) {
  struct kk_std_core_const_fun20025__t_1* _self = kk_function_as(struct kk_std_core_const_fun20025__t_1*, _fself);
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

kk_std_core__list kk_std_core__mlift17667_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_std_core__list zz, kk_std_core__list ys1_17210, kk_context_t* _ctx) { /* forall<a,b,e> (ctail<list<b>>, f : (a) -> e list<b>, zz : list<a>, ys1.17210 : list<b>) -> e list<b> */ 
  return kk_std_core__ctail_lift17184_flatmap(f, ys1_17210, zz, _acc, _ctx);
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift17668_op(kk_function_t _accm, kk_function_t f0, kk_std_core__list zz0, kk_std_core__list ys1_172100, kk_context_t* _ctx) { /* forall<a,b,e> ((list<b>) -> list<b>, f : (a) -> e list<b>, zz : list<a>, ys1.17210 : list<b>) -> e list<b> */ 
  return kk_std_core__ctailm_lift17184_flatmap(f0, ys1_172100, zz0, _accm, _ctx);
}
 
// lifted local: flatmap, flatmap-pre


// lift anonymous function
struct kk_std_core__ctail_lift17184_flatmap_fun20031__t {
  struct kk_function_s _base;
  kk_function_t f1;
  kk_std_core__list zz1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_lift17184_flatmap_fun20031(kk_function_t _fself, kk_box_t _b_18227, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_lift17184_flatmap_fun20031(kk_function_t f1, kk_std_core__list zz1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_lift17184_flatmap_fun20031__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_lift17184_flatmap_fun20031__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_lift17184_flatmap_fun20031, kk_context());
  _self->f1 = f1;
  _self->zz1 = zz1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_lift17184_flatmap_fun20031(kk_function_t _fself, kk_box_t _b_18227, kk_context_t* _ctx) {
  struct kk_std_core__ctail_lift17184_flatmap_fun20031__t* _self = kk_function_as(struct kk_std_core__ctail_lift17184_flatmap_fun20031__t*, _fself);
  kk_function_t f1 = _self->f1; /* (2456) -> 2458 list<2457> */
  kk_std_core__list zz1 = _self->zz1; /* list<2456> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<2457>> */
  kk_drop_match(_self, {kk_function_dup(f1);kk_std_core__list_dup(zz1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_std_core__list ys1_18239_172101 = kk_std_core__list_unbox(_b_18227, _ctx); /*list<2457>*/;
  kk_std_core__list _x20032 = kk_std_core__mlift17667_op(_acc0, f1, zz1, ys1_18239_172101, _ctx); /*list<2457>*/
  return kk_std_core__list_box(_x20032, _ctx);
}

kk_std_core__list kk_std_core__ctail_lift17184_flatmap(kk_function_t f1, kk_std_core__list ys, kk_std_core__list zs, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,b,e> (f : (a) -> e list<b>, ys : list<b>, zs : list<a>, ctail<list<b>>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con20026 = kk_std_core__as_Cons(ys);
    kk_box_t y = _con20026->head;
    kk_std_core__list yy = _con20026->tail;
    kk_reuse_t _ru_19895 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      _ru_19895 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(y);
      kk_std_core__list_dup(yy);
      kk_std_core__list_decref(ys, _ctx);
    }
    kk_std_core__list _ctail_17238 = kk_std_core__list_hole(); /*list<2457>*/;
    kk_std_core__list _ctail_17239 = kk_std_core__new_Cons(_ru_19895, y, _ctail_17238, _ctx); /*list<2457>*/;
    kk_box_t* _b_18232_18225 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17239)->tail)); /*cfield<list<2457>>*/;
    { // tailcall
      kk_std_core_types__ctail _x20027 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_17239, _ctx)),_b_18232_18225); /*ctail<0>*/
      ys = yy;
      _acc0 = _x20027;
      goto kk__tailcall;
    }
  }
  if (kk_std_core__is_Cons(zs)) {
    struct kk_std_core_Cons* _con20028 = kk_std_core__as_Cons(zs);
    kk_box_t z = _con20028->head;
    kk_std_core__list zz1 = _con20028->tail;
    if (kk_likely(kk_std_core__list_is_unique(zs))) {
      kk_std_core__list_free(zs, _ctx);
    }
    else {
      kk_box_dup(z);
      kk_std_core__list_dup(zz1);
      kk_std_core__list_decref(zs, _ctx);
    }
    kk_std_core__list x_17742;
    kk_function_t _x20029 = kk_function_dup(f1); /*(2456) -> 2458 list<2457>*/
    x_17742 = kk_function_call(kk_std_core__list, (kk_function_t, kk_box_t, kk_context_t*), _x20029, (_x20029, z, _ctx)); /*list<2457>*/
    if (kk_yielding(kk_context())) {
      kk_std_core__list_drop(x_17742, _ctx);
      kk_box_t _x20030 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_lift17184_flatmap_fun20031(f1, zz1, _acc0, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20030, _ctx);
    }
    { // tailcall
      ys = x_17742;
      zs = zz1;
      goto kk__tailcall;
    }
  }
  {
    kk_function_drop(f1, _ctx);
    kk_box_t _x20033 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20033, _ctx);
  }
}
 
// lifted local: flatmap, flatmap-pre


// lift anonymous function
struct kk_std_core__ctailm_lift17184_flatmap_fun20036__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t y0;
};
static kk_std_core__list kk_std_core__ctailm_lift17184_flatmap_fun20036(kk_function_t _fself, kk_std_core__list _ctail_17241, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_lift17184_flatmap_fun20036(kk_function_t _accm0, kk_box_t y0, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17184_flatmap_fun20036__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_lift17184_flatmap_fun20036__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_lift17184_flatmap_fun20036, kk_context());
  _self->_accm0 = _accm0;
  _self->y0 = y0;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_lift17184_flatmap_fun20036(kk_function_t _fself, kk_std_core__list _ctail_17241, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17184_flatmap_fun20036__t* _self = kk_function_as(struct kk_std_core__ctailm_lift17184_flatmap_fun20036__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<2457>) -> list<2457> */
  kk_box_t y0 = _self->y0; /* 2457 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(y0);}, {}, _ctx)
  kk_std_core__list _x20037 = kk_std_core__new_Cons(kk_reuse_null, y0, _ctail_17241, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x20037, _ctx));
}


// lift anonymous function
struct kk_std_core__ctailm_lift17184_flatmap_fun20041__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t f2;
  kk_std_core__list zz2;
};
static kk_box_t kk_std_core__ctailm_lift17184_flatmap_fun20041(kk_function_t _fself, kk_box_t _b_18241, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_lift17184_flatmap_fun20041(kk_function_t _accm0, kk_function_t f2, kk_std_core__list zz2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17184_flatmap_fun20041__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_lift17184_flatmap_fun20041__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_lift17184_flatmap_fun20041, kk_context());
  _self->_accm0 = _accm0;
  _self->f2 = f2;
  _self->zz2 = zz2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_lift17184_flatmap_fun20041(kk_function_t _fself, kk_box_t _b_18241, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17184_flatmap_fun20041__t* _self = kk_function_as(struct kk_std_core__ctailm_lift17184_flatmap_fun20041__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<2457>) -> list<2457> */
  kk_function_t f2 = _self->f2; /* (2456) -> 2458 list<2457> */
  kk_std_core__list zz2 = _self->zz2; /* list<2456> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(f2);kk_std_core__list_dup(zz2);}, {}, _ctx)
  kk_std_core__list ys1_18243_172103 = kk_std_core__list_unbox(_b_18241, _ctx); /*list<2457>*/;
  kk_std_core__list _x20042 = kk_std_core__mlift17668_op(_accm0, f2, zz2, ys1_18243_172103, _ctx); /*list<2457>*/
  return kk_std_core__list_box(_x20042, _ctx);
}

kk_std_core__list kk_std_core__ctailm_lift17184_flatmap(kk_function_t f2, kk_std_core__list ys0, kk_std_core__list zs0, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,b,e> (f : (a) -> e list<b>, ys : list<b>, zs : list<a>, (list<b>) -> list<b>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys0)) {
    struct kk_std_core_Cons* _con20034 = kk_std_core__as_Cons(ys0);
    kk_box_t y0 = _con20034->head;
    kk_std_core__list yy0 = _con20034->tail;
    if (kk_likely(kk_std_core__list_is_unique(ys0))) {
      kk_std_core__list_free(ys0, _ctx);
    }
    else {
      kk_box_dup(y0);
      kk_std_core__list_dup(yy0);
      kk_std_core__list_decref(ys0, _ctx);
    }
    { // tailcall
      kk_function_t _x20035 = kk_std_core__new_ctailm_lift17184_flatmap_fun20036(_accm0, y0, _ctx); /*(list<2457>) -> list<2457>*/
      ys0 = yy0;
      _accm0 = _x20035;
      goto kk__tailcall;
    }
  }
  if (kk_std_core__is_Cons(zs0)) {
    struct kk_std_core_Cons* _con20038 = kk_std_core__as_Cons(zs0);
    kk_box_t z0 = _con20038->head;
    kk_std_core__list zz2 = _con20038->tail;
    if (kk_likely(kk_std_core__list_is_unique(zs0))) {
      kk_std_core__list_free(zs0, _ctx);
    }
    else {
      kk_box_dup(z0);
      kk_std_core__list_dup(zz2);
      kk_std_core__list_decref(zs0, _ctx);
    }
    kk_std_core__list x0_17745;
    kk_function_t _x20039 = kk_function_dup(f2); /*(2456) -> 2458 list<2457>*/
    x0_17745 = kk_function_call(kk_std_core__list, (kk_function_t, kk_box_t, kk_context_t*), _x20039, (_x20039, z0, _ctx)); /*list<2457>*/
    if (kk_yielding(kk_context())) {
      kk_std_core__list_drop(x0_17745, _ctx);
      kk_box_t _x20040 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_lift17184_flatmap_fun20041(_accm0, f2, zz2, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20040, _ctx);
    }
    { // tailcall
      ys0 = x0_17745;
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
struct kk_std_core__lift17184_flatmap_fun20044__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core__lift17184_flatmap_fun20044(kk_function_t _fself, kk_std_core__list _ctail_17240, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17184_flatmap_fun20044(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core__lift17184_flatmap_fun20044, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core__lift17184_flatmap_fun20044(kk_function_t _fself, kk_std_core__list _ctail_17240, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17240;
}

kk_std_core__list kk_std_core__lift17184_flatmap(kk_function_t f3, kk_std_core__list ys1, kk_std_core__list zs1, kk_context_t* _ctx) { /* forall<a,b,e> (f : (a) -> e list<b>, ys : list<b>, zs : list<a>) -> e list<b> */ 
  bool _match_19887 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_19887) {
    kk_std_core_types__ctail _x20043 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_lift17184_flatmap(f3, ys1, zs1, _x20043, _ctx);
  }
  {
    return kk_std_core__ctailm_lift17184_flatmap(f3, ys1, zs1, kk_std_core__new_lift17184_flatmap_fun20044(_ctx), _ctx);
  }
}
 
// lifted local: reverse-append, reverse-acc

kk_std_core__list kk_std_core__lift17185_reverse_append(kk_std_core__list acc, kk_std_core__list ys, kk_context_t* _ctx) { /* forall<a> (acc : list<a>, ys : list<a>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con20045 = kk_std_core__as_Cons(ys);
    kk_box_t x = _con20045->head;
    kk_std_core__list xx = _con20045->tail;
    kk_reuse_t _ru_19899 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      _ru_19899 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(ys, _ctx);
    }
    { // tailcall
      kk_std_core__list _x20046 = kk_std_core__new_Cons(_ru_19899, x, acc, _ctx); /*list<61>*/
      acc = _x20046;
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
struct kk_std_core__mlift17670_force_fun20050__t {
  struct kk_function_s _base;
  kk_ref_t r;
};
static kk_box_t kk_std_core__mlift17670_force_fun20050(kk_function_t _fself, kk_box_t x0, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17670_force_fun20050(kk_ref_t r, kk_context_t* _ctx) {
  struct kk_std_core__mlift17670_force_fun20050__t* _self = kk_function_alloc_as(struct kk_std_core__mlift17670_force_fun20050__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17670_force_fun20050, kk_context());
  _self->r = r;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift17670_force_fun20050(kk_function_t _fself, kk_box_t x0, kk_context_t* _ctx) {
  struct kk_std_core__mlift17670_force_fun20050__t* _self = kk_function_as(struct kk_std_core__mlift17670_force_fun20050__t*, _fself);
  kk_ref_t r = _self->r; /* ref<global,either<() -> 2593 2592,2592>> */
  kk_drop_match(_self, {kk_ref_dup(r);}, {}, _ctx)
  kk_unit_t __ = kk_Unit;
  kk_unit_t _brw_19885 = kk_Unit;
  kk_box_t _x20051;
  kk_std_core_types__either _x20052;
  kk_box_t _x20053 = kk_box_dup(x0); /*2592*/
  _x20052 = kk_std_core_types__new_Right(_x20053, _ctx); /*either<1024,1025>*/
  _x20051 = kk_std_core_types__either_box(_x20052, _ctx); /*1000*/
  kk_ref_set_borrow(r,_x20051,kk_context());
  kk_ref_drop(r, _ctx);
  _brw_19885;
  return x0;
}

kk_box_t kk_std_core__mlift17670_force(kk_ref_t r, kk_function_t _y_17346, kk_context_t* _ctx) { /* forall<a,e> (r : ref<global,either<() -> e a,a>>, () -> <st<global>,div|e> a) -> <alloc<global>,div,read<global>,write<global>|e> a */ 
  kk_box_t x_17748 = kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), _y_17346, (_y_17346, _ctx)); /*2592*/;
  kk_function_t next0_17749 = kk_std_core__new_mlift17670_force_fun20050(r, _ctx); /*(2592) -> <st<global>,div|2593> 2592*/;
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_17748, _ctx);
    return kk_std_core_hnd_yield_extend(next0_17749, _ctx);
  }
  {
    return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), next0_17749, (next0_17749, x_17748, _ctx));
  }
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift17671_force_fun20054__t {
  struct kk_function_s _base;
  kk_ref_t r;
};
static kk_box_t kk_std_core__mlift17671_force_fun20054(kk_function_t _fself, kk_box_t _b_18256, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17671_force_fun20054(kk_ref_t r, kk_context_t* _ctx) {
  struct kk_std_core__mlift17671_force_fun20054__t* _self = kk_function_alloc_as(struct kk_std_core__mlift17671_force_fun20054__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17671_force_fun20054, kk_context());
  _self->r = r;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift17671_force_fun20054(kk_function_t _fself, kk_box_t _b_18256, kk_context_t* _ctx) {
  struct kk_std_core__mlift17671_force_fun20054__t* _self = kk_function_as(struct kk_std_core__mlift17671_force_fun20054__t*, _fself);
  kk_ref_t r = _self->r; /* ref<global,either<() -> 2593 2592,2592>> */
  kk_drop_match(_self, {kk_ref_dup(r);}, {}, _ctx)
  kk_function_t _x20055 = kk_function_unbox(_b_18256); /*() -> <st<global>,div|2593> 18257*/
  return kk_std_core__mlift17670_force(r, _x20055, _ctx);
}

kk_box_t kk_std_core__mlift17671_force(kk_ref_t r, kk_std_core_types__either _y_17344, kk_context_t* _ctx) { /* forall<a,e> (r : ref<global,either<() -> e a,a>>, either<() -> e a,a>) -> <read<global>,div,alloc<global>,write<global>|e> a */ 
  if (kk_std_core_types__is_Right(_y_17344)) {
    kk_box_t x = _y_17344._cons.Right.right;
    kk_ref_drop(r, _ctx);
    return x;
  }
  {
    kk_box_t _fun_unbox_x18253 = _y_17344._cons.Left.left;
    if (kk_yielding(kk_context())) {
      kk_box_drop(_fun_unbox_x18253, _ctx);
      return kk_std_core_hnd_yield_extend(kk_std_core__new_mlift17671_force_fun20054(r, _ctx), _ctx);
    }
    {
      kk_function_t _x20056 = kk_function_unbox(_fun_unbox_x18253); /*() -> 2593 18254*/
      return kk_std_core__mlift17670_force(r, _x20056, _ctx);
    }
  }
}
 
// Force a delayed value; the value is computed only on the first
// call to `force` and cached afterwards.


// lift anonymous function
struct kk_std_core_force_fun20059__t {
  struct kk_function_s _base;
  kk_std_core__delayed delayed;
};
static kk_box_t kk_std_core_force_fun20059(kk_function_t _fself, kk_box_t _b_18263, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_force_fun20059(kk_std_core__delayed delayed, kk_context_t* _ctx) {
  struct kk_std_core_force_fun20059__t* _self = kk_function_alloc_as(struct kk_std_core_force_fun20059__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_force_fun20059, kk_context());
  _self->delayed = delayed;
  return &_self->_base;
}

static kk_box_t kk_std_core_force_fun20059(kk_function_t _fself, kk_box_t _b_18263, kk_context_t* _ctx) {
  struct kk_std_core_force_fun20059__t* _self = kk_function_as(struct kk_std_core_force_fun20059__t*, _fself);
  kk_std_core__delayed delayed = _self->delayed; /* delayed<2593,2592> */
  kk_drop_match(_self, {kk_std_core__delayed_dup(delayed);}, {}, _ctx)
  kk_std_core_types__either _y_18265_17344 = kk_std_core_types__either_unbox(_b_18263, _ctx); /*either<() -> 2593 2592,2592>*/;
  kk_ref_t _x20060;
  {
    kk_ref_t _x0 = delayed.dref;
    _x20060 = _x0; /*ref<global,either<() -> 2593 2592,2592>>*/
  }
  return kk_std_core__mlift17671_force(_x20060, _y_18265_17344, _ctx);
}

kk_box_t kk_std_core_force(kk_std_core__delayed delayed, kk_context_t* _ctx) { /* forall<a,e> (delayed : delayed<e,a>) -> e a */ 
  kk_std_core_types__either x_17757;
  kk_box_t _x20057;
  kk_ref_t _x20058;
  {
    kk_ref_t _x = delayed.dref;
    kk_ref_dup(_x);
    _x20058 = _x; /*ref<global,either<() -> 2593 2592,2592>>*/
  }
  _x20057 = kk_ref_get(_x20058,kk_context()); /*1001*/
  x_17757 = kk_std_core_types__either_unbox(_x20057, _ctx); /*either<() -> 2593 2592,2592>*/
  if (kk_yielding(kk_context())) {
    kk_std_core_types__either_drop(x_17757, _ctx);
    return kk_std_core_hnd_yield_extend(kk_std_core_new_force_fun20059(delayed, _ctx), _ctx);
  }
  {
    kk_std_core_types__either _y_18266_17344 = x_17757; /*either<() -> 2593 2592,2592>*/;
    kk_ref_t _x20061;
    {
      kk_ref_t _x0 = delayed.dref;
      _x20061 = _x0; /*ref<global,either<() -> 2593 2592,2592>>*/
    }
    return kk_std_core__mlift17671_force(_x20061, _y_18266_17344, _ctx);
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

kk_std_core__list kk_std_core__ctail_lift17186_intersperse(kk_box_t sep, kk_std_core__list ys, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* forall<a> (sep : a, ys : list<a>, ctail<list<a>>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con20062 = kk_std_core__as_Cons(ys);
    kk_box_t y = _con20062->head;
    kk_std_core__list yy = _con20062->tail;
    kk_reuse_t _ru_19900 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      _ru_19900 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(y);
      kk_std_core__list_dup(yy);
      kk_std_core__list_decref(ys, _ctx);
    }
    kk_std_core__list _ctail_17242;
    kk_std_core__list _x20063 = kk_std_core__list_hole(); /*list<2679>*/
    _ctail_17242 = kk_std_core__new_Cons(_ru_19900, y, _x20063, _ctx); /*list<2679>*/
    kk_box_t* _b_18277_18272 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17242)->tail)); /*cfield<list<2679>>*/;
    { // tailcall
      kk_box_t _x20064 = kk_box_dup(sep); /*2679*/
      kk_std_core_types__ctail _x20065;
      kk_box_t _x20066;
      kk_std_core__list _x20067 = kk_std_core__new_Cons(kk_reuse_null, sep, _ctail_17242, _ctx); /*list<61>*/
      _x20066 = kk_std_core__list_box(_x20067, _ctx); /*0*/
      _x20065 = kk_ctail_link(_acc,_x20066,_b_18277_18272); /*ctail<0>*/
      sep = _x20064;
      ys = yy;
      _acc = _x20065;
      goto kk__tailcall;
    }
  }
  {
    kk_box_drop(sep, _ctx);
    kk_box_t _x20068 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20068, _ctx);
  }
}
 
// lifted local: intersperse, before0

kk_std_core__list kk_std_core__lift17186_intersperse(kk_box_t sep0, kk_std_core__list ys0, kk_context_t* _ctx) { /* forall<a> (sep : a, ys : list<a>) -> list<a> */ 
  kk_std_core_types__ctail _x20069 = kk_ctail_nil(); /*ctail<0>*/
  return kk_std_core__ctail_lift17186_intersperse(sep0, ys0, _x20069, _ctx);
}
 
// Insert a separator `sep`  between all elements of a list `xs` .

kk_std_core__list kk_std_core_intersperse(kk_std_core__list xs, kk_box_t sep, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, sep : a) -> list<a> */ 
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con20070 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con20070->head;
    kk_std_core__list xx = _con20070->tail;
    kk_reuse_t _ru_19901 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      _ru_19901 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_std_core__list _x20071 = kk_std_core__lift17186_intersperse(sep, xx, _ctx); /*list<2679>*/
    return kk_std_core__new_Cons(_ru_19901, x, _x20071, _ctx);
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

kk_std_core__list kk_std_core__mlift17672_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_std_core__list yy, kk_box_t _ctail_17244, kk_context_t* _ctx) { /* forall<a,b,e> (ctail<list<b>>, f : (value : a, rest : list<a>) -> e b, yy : list<a>, b) -> e list<b> */ 
  kk_std_core__list _ctail_17245 = kk_std_core__list_hole(); /*list<2770>*/;
  kk_std_core__list _ctail_17246 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17244, _ctail_17245, _ctx); /*list<2770>*/;
  kk_box_t* _b_18291_18288 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17246)->tail)); /*cfield<list<2770>>*/;
  kk_std_core_types__ctail _x20072 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17246, _ctx)),_b_18291_18288); /*ctail<0>*/
  return kk_std_core__ctail_lift17187_map_peek(f, yy, _x20072, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift17673_op_fun20073__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t _ctail_17249;
};
static kk_std_core__list kk_std_core__mlift17673_op_fun20073(kk_function_t _fself, kk_std_core__list _ctail_17248, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17673_op_fun20073(kk_function_t _accm, kk_box_t _ctail_17249, kk_context_t* _ctx) {
  struct kk_std_core__mlift17673_op_fun20073__t* _self = kk_function_alloc_as(struct kk_std_core__mlift17673_op_fun20073__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17673_op_fun20073, kk_context());
  _self->_accm = _accm;
  _self->_ctail_17249 = _ctail_17249;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift17673_op_fun20073(kk_function_t _fself, kk_std_core__list _ctail_17248, kk_context_t* _ctx) {
  struct kk_std_core__mlift17673_op_fun20073__t* _self = kk_function_as(struct kk_std_core__mlift17673_op_fun20073__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<2770>) -> list<2770> */
  kk_box_t _ctail_17249 = _self->_ctail_17249; /* 2770 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(_ctail_17249);}, {}, _ctx)
  kk_std_core__list _x20074 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17249, _ctail_17248, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x20074, _ctx));
}

kk_std_core__list kk_std_core__mlift17673_op(kk_function_t _accm, kk_function_t f0, kk_std_core__list yy0, kk_box_t _ctail_17249, kk_context_t* _ctx) { /* forall<a,b,e> ((list<b>) -> list<b>, f : (value : a, rest : list<a>) -> e b, yy : list<a>, b) -> e list<b> */ 
  return kk_std_core__ctailm_lift17187_map_peek(f0, yy0, kk_std_core__new_mlift17673_op_fun20073(_accm, _ctail_17249, _ctx), _ctx);
}
 
// lifted local: map-peek, mappeek


// lift anonymous function
struct kk_std_core__ctail_lift17187_map_peek_fun20079__t {
  struct kk_function_s _base;
  kk_function_t f1;
  kk_std_core__list yy1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_lift17187_map_peek_fun20079(kk_function_t _fself, kk_box_t _b_18296, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_lift17187_map_peek_fun20079(kk_function_t f1, kk_std_core__list yy1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_lift17187_map_peek_fun20079__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_lift17187_map_peek_fun20079__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_lift17187_map_peek_fun20079, kk_context());
  _self->f1 = f1;
  _self->yy1 = yy1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_lift17187_map_peek_fun20079(kk_function_t _fself, kk_box_t _b_18296, kk_context_t* _ctx) {
  struct kk_std_core__ctail_lift17187_map_peek_fun20079__t* _self = kk_function_as(struct kk_std_core__ctail_lift17187_map_peek_fun20079__t*, _fself);
  kk_function_t f1 = _self->f1; /* (value : 2769, rest : list<2769>) -> 2771 2770 */
  kk_std_core__list yy1 = _self->yy1; /* list<2769> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<2770>> */
  kk_drop_match(_self, {kk_function_dup(f1);kk_std_core__list_dup(yy1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_box_t _ctail_18314_172440 = _b_18296; /*2770*/;
  kk_std_core__list _x20080 = kk_std_core__mlift17672_op(_acc0, f1, yy1, _ctail_18314_172440, _ctx); /*list<2770>*/
  return kk_std_core__list_box(_x20080, _ctx);
}

kk_std_core__list kk_std_core__ctail_lift17187_map_peek(kk_function_t f1, kk_std_core__list ys, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,b,e> (f : (value : a, rest : list<a>) -> e b, ys : list<a>, ctail<list<b>>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con20075 = kk_std_core__as_Cons(ys);
    kk_box_t y = _con20075->head;
    kk_std_core__list yy1 = _con20075->tail;
    kk_reuse_t _ru_19902 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      _ru_19902 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(y);
      kk_std_core__list_dup(yy1);
      kk_std_core__list_decref(ys, _ctx);
    }
    kk_box_t x_17759;
    kk_function_t _x20077 = kk_function_dup(f1); /*(value : 2769, rest : list<2769>) -> 2771 2770*/
    kk_std_core__list _x20076 = kk_std_core__list_dup(yy1); /*list<2769>*/
    x_17759 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_std_core__list, kk_context_t*), _x20077, (_x20077, y, _x20076, _ctx)); /*2770*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_19902,kk_context());
      kk_box_drop(x_17759, _ctx);
      kk_box_t _x20078 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_lift17187_map_peek_fun20079(f1, yy1, _acc0, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20078, _ctx);
    }
    {
      kk_std_core__list _ctail_172450 = kk_std_core__list_hole(); /*list<2770>*/;
      kk_std_core__list _ctail_172460 = kk_std_core__new_Cons(_ru_19902, x_17759, _ctail_172450, _ctx); /*list<2770>*/;
      kk_box_t* _b_18308_18302 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_172460)->tail)); /*cfield<list<2770>>*/;
      { // tailcall
        kk_std_core_types__ctail _x20081 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_172460, _ctx)),_b_18308_18302); /*ctail<0>*/
        ys = yy1;
        _acc0 = _x20081;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_function_drop(f1, _ctx);
    kk_box_t _x20082 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20082, _ctx);
  }
}
 
// lifted local: map-peek, mappeek


// lift anonymous function
struct kk_std_core__ctailm_lift17187_map_peek_fun20087__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t f2;
  kk_std_core__list yy2;
};
static kk_box_t kk_std_core__ctailm_lift17187_map_peek_fun20087(kk_function_t _fself, kk_box_t _b_18316, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_lift17187_map_peek_fun20087(kk_function_t _accm0, kk_function_t f2, kk_std_core__list yy2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17187_map_peek_fun20087__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_lift17187_map_peek_fun20087__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_lift17187_map_peek_fun20087, kk_context());
  _self->_accm0 = _accm0;
  _self->f2 = f2;
  _self->yy2 = yy2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_lift17187_map_peek_fun20087(kk_function_t _fself, kk_box_t _b_18316, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17187_map_peek_fun20087__t* _self = kk_function_as(struct kk_std_core__ctailm_lift17187_map_peek_fun20087__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<2770>) -> list<2770> */
  kk_function_t f2 = _self->f2; /* (value : 2769, rest : list<2769>) -> 2771 2770 */
  kk_std_core__list yy2 = _self->yy2; /* list<2769> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(f2);kk_std_core__list_dup(yy2);}, {}, _ctx)
  kk_box_t _ctail_18318_172490 = _b_18316; /*2770*/;
  kk_std_core__list _x20088 = kk_std_core__mlift17673_op(_accm0, f2, yy2, _ctail_18318_172490, _ctx); /*list<2770>*/
  return kk_std_core__list_box(_x20088, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_lift17187_map_peek_fun20090__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t x0_17762;
};
static kk_std_core__list kk_std_core__ctailm_lift17187_map_peek_fun20090(kk_function_t _fself, kk_std_core__list _ctail_172480, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_lift17187_map_peek_fun20090(kk_function_t _accm0, kk_box_t x0_17762, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17187_map_peek_fun20090__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_lift17187_map_peek_fun20090__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_lift17187_map_peek_fun20090, kk_context());
  _self->_accm0 = _accm0;
  _self->x0_17762 = x0_17762;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_lift17187_map_peek_fun20090(kk_function_t _fself, kk_std_core__list _ctail_172480, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17187_map_peek_fun20090__t* _self = kk_function_as(struct kk_std_core__ctailm_lift17187_map_peek_fun20090__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<2770>) -> list<2770> */
  kk_box_t x0_17762 = _self->x0_17762; /* 2770 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(x0_17762);}, {}, _ctx)
  kk_std_core__list _x20091 = kk_std_core__new_Cons(kk_reuse_null, x0_17762, _ctail_172480, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x20091, _ctx));
}

kk_std_core__list kk_std_core__ctailm_lift17187_map_peek(kk_function_t f2, kk_std_core__list ys0, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,b,e> (f : (value : a, rest : list<a>) -> e b, ys : list<a>, (list<b>) -> list<b>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys0)) {
    struct kk_std_core_Cons* _con20083 = kk_std_core__as_Cons(ys0);
    kk_box_t y0 = _con20083->head;
    kk_std_core__list yy2 = _con20083->tail;
    if (kk_likely(kk_std_core__list_is_unique(ys0))) {
      kk_std_core__list_free(ys0, _ctx);
    }
    else {
      kk_box_dup(y0);
      kk_std_core__list_dup(yy2);
      kk_std_core__list_decref(ys0, _ctx);
    }
    kk_box_t x0_17762;
    kk_function_t _x20085 = kk_function_dup(f2); /*(value : 2769, rest : list<2769>) -> 2771 2770*/
    kk_std_core__list _x20084 = kk_std_core__list_dup(yy2); /*list<2769>*/
    x0_17762 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_std_core__list, kk_context_t*), _x20085, (_x20085, y0, _x20084, _ctx)); /*2770*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x0_17762, _ctx);
      kk_box_t _x20086 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_lift17187_map_peek_fun20087(_accm0, f2, yy2, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20086, _ctx);
    }
    { // tailcall
      kk_function_t _x20089 = kk_std_core__new_ctailm_lift17187_map_peek_fun20090(_accm0, x0_17762, _ctx); /*(list<2770>) -> list<2770>*/
      ys0 = yy2;
      _accm0 = _x20089;
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
struct kk_std_core__lift17187_map_peek_fun20093__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core__lift17187_map_peek_fun20093(kk_function_t _fself, kk_std_core__list _ctail_17247, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17187_map_peek_fun20093(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core__lift17187_map_peek_fun20093, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core__lift17187_map_peek_fun20093(kk_function_t _fself, kk_std_core__list _ctail_17247, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17247;
}

kk_std_core__list kk_std_core__lift17187_map_peek(kk_function_t f3, kk_std_core__list ys1, kk_context_t* _ctx) { /* forall<a,b,e> (f : (value : a, rest : list<a>) -> e b, ys : list<a>) -> e list<b> */ 
  bool _match_19879 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_19879) {
    kk_std_core_types__ctail _x20092 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_lift17187_map_peek(f3, ys1, _x20092, _ctx);
  }
  {
    return kk_std_core__ctailm_lift17187_map_peek(f3, ys1, kk_std_core__new_lift17187_map_peek_fun20093(_ctx), _ctx);
  }
}

kk_ref_t kk_std_core_trace_enabled;
 
// Compose two funs `f` and `g`.


// lift anonymous function
struct kk_std_core_o_fun20095__t {
  struct kk_function_s _base;
  kk_function_t f;
  kk_function_t g;
};
static kk_box_t kk_std_core_o_fun20095(kk_function_t _fself, kk_box_t x, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_o_fun20095(kk_function_t f, kk_function_t g, kk_context_t* _ctx) {
  struct kk_std_core_o_fun20095__t* _self = kk_function_alloc_as(struct kk_std_core_o_fun20095__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_o_fun20095, kk_context());
  _self->f = f;
  _self->g = g;
  return &_self->_base;
}

static kk_box_t kk_std_core_o_fun20095(kk_function_t _fself, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_o_fun20095__t* _self = kk_function_as(struct kk_std_core_o_fun20095__t*, _fself);
  kk_function_t f = _self->f; /* (2812) -> 2809 2810 */
  kk_function_t g = _self->g; /* (2808) -> 2809 2812 */
  kk_drop_match(_self, {kk_function_dup(f);kk_function_dup(g);}, {}, _ctx)
  kk_box_t x0_17765 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), g, (g, x, _ctx)); /*2812*/;
  if (kk_yielding(kk_context())) {
    kk_box_drop(x0_17765, _ctx);
    return kk_std_core_hnd_yield_extend(f, _ctx);
  }
  {
    return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, x0_17765, _ctx));
  }
}

kk_function_t kk_std_core_o(kk_function_t f, kk_function_t g, kk_context_t* _ctx) { /* forall<a,b,c,e> (f : (a) -> e b, g : (c) -> e a) -> ((x : c) -> e b) */ 
  return kk_std_core_new_o_fun20095(f, g, _ctx);
}
 
// monadic lift

kk_box_t kk_std_core__mlift17674_once(kk_function_t calc, kk_ref_t r, kk_std_core_types__maybe _y_17363, kk_context_t* _ctx) { /* forall<_h,_e,a> (calc : () -> a, r : ref<_h,maybe<a>>, maybe<a>) -> <read<_h>,write<_h>,div|_e> a */ 
  if (kk_std_core_types__is_Just(_y_17363)) {
    kk_box_t x = _y_17363._cons.Just.value;
    kk_ref_drop(r, _ctx);
    kk_function_drop(calc, _ctx);
    return x;
  }
  {
    kk_box_t x0 = kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), calc, (calc, _ctx)); /*2933*/;
    kk_unit_t __ = kk_Unit;
    kk_unit_t _brw_19877 = kk_Unit;
    kk_box_t _x20097;
    kk_std_core_types__maybe _x20098;
    kk_box_t _x20099 = kk_box_dup(x0); /*2933*/
    _x20098 = kk_std_core_types__new_Just(_x20099, _ctx); /*maybe<1034>*/
    _x20097 = kk_std_core_types__maybe_box(_x20098, _ctx); /*1000*/
    kk_ref_set_borrow(r,_x20097,kk_context());
    kk_ref_drop(r, _ctx);
    _brw_19877;
    return x0;
  }
}
 
// Given a total function to calculate a value `:a`, return
// a total function that only calculates the value once and then
// returns the cached result.


// lift anonymous function
struct kk_std_core_once_fun20100__t {
  struct kk_function_s _base;
  kk_function_t calc;
  kk_ref_t r;
};
static kk_box_t kk_std_core_once_fun20100(kk_function_t _fself, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_once_fun20100(kk_function_t calc, kk_ref_t r, kk_context_t* _ctx) {
  struct kk_std_core_once_fun20100__t* _self = kk_function_alloc_as(struct kk_std_core_once_fun20100__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_once_fun20100, kk_context());
  _self->calc = calc;
  _self->r = r;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_once_fun20103__t {
  struct kk_function_s _base;
  kk_function_t calc;
  kk_ref_t r;
};
static kk_box_t kk_std_core_once_fun20103(kk_function_t _fself, kk_std_core_types__maybe _y_17363, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_once_fun20103(kk_function_t calc, kk_ref_t r, kk_context_t* _ctx) {
  struct kk_std_core_once_fun20103__t* _self = kk_function_alloc_as(struct kk_std_core_once_fun20103__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_once_fun20103, kk_context());
  _self->calc = calc;
  _self->r = r;
  return &_self->_base;
}

static kk_box_t kk_std_core_once_fun20103(kk_function_t _fself, kk_std_core_types__maybe _y_17363, kk_context_t* _ctx) {
  struct kk_std_core_once_fun20103__t* _self = kk_function_as(struct kk_std_core_once_fun20103__t*, _fself);
  kk_function_t calc = _self->calc; /* () -> 2933 */
  kk_ref_t r = _self->r; /* ref<_2842,maybe<2933>> */
  kk_drop_match(_self, {kk_function_dup(calc);kk_ref_dup(r);}, {}, _ctx)
  if (kk_std_core_types__is_Just(_y_17363)) {
    kk_box_t x0 = _y_17363._cons.Just.value;
    kk_ref_drop(r, _ctx);
    kk_function_drop(calc, _ctx);
    return x0;
  }
  {
    kk_box_t x00 = kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), calc, (calc, _ctx)); /*2933*/;
    kk_unit_t __ = kk_Unit;
    kk_unit_t _brw_19876 = kk_Unit;
    kk_box_t _x20104;
    kk_std_core_types__maybe _x20105;
    kk_box_t _x20106 = kk_box_dup(x00); /*2933*/
    _x20105 = kk_std_core_types__new_Just(_x20106, _ctx); /*maybe<1034>*/
    _x20104 = kk_std_core_types__maybe_box(_x20105, _ctx); /*1000*/
    kk_ref_set_borrow(r,_x20104,kk_context());
    kk_ref_drop(r, _ctx);
    _brw_19876;
    return x00;
  }
}


// lift anonymous function
struct kk_std_core_once_fun20107__t {
  struct kk_function_s _base;
  kk_function_t next0_17770;
};
static kk_box_t kk_std_core_once_fun20107(kk_function_t _fself, kk_box_t _b_18335, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_once_fun20107(kk_function_t next0_17770, kk_context_t* _ctx) {
  struct kk_std_core_once_fun20107__t* _self = kk_function_alloc_as(struct kk_std_core_once_fun20107__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_once_fun20107, kk_context());
  _self->next0_17770 = next0_17770;
  return &_self->_base;
}

static kk_box_t kk_std_core_once_fun20107(kk_function_t _fself, kk_box_t _b_18335, kk_context_t* _ctx) {
  struct kk_std_core_once_fun20107__t* _self = kk_function_as(struct kk_std_core_once_fun20107__t*, _fself);
  kk_function_t next0_17770 = _self->next0_17770; /* (maybe<2933>) -> <read<_2842>,write<_2842>,div|_2925> 2933 */
  kk_drop_match(_self, {kk_function_dup(next0_17770);}, {}, _ctx)
  kk_std_core_types__maybe _x20108 = kk_std_core_types__maybe_unbox(_b_18335, _ctx); /*maybe<2933>*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_types__maybe, kk_context_t*), next0_17770, (next0_17770, _x20108, _ctx));
}
static kk_box_t kk_std_core_once_fun20100(kk_function_t _fself, kk_context_t* _ctx) {
  struct kk_std_core_once_fun20100__t* _self = kk_function_as(struct kk_std_core_once_fun20100__t*, _fself);
  kk_function_t calc = _self->calc; /* () -> 2933 */
  kk_ref_t r = _self->r; /* ref<_2842,maybe<2933>> */
  kk_drop_match(_self, {kk_function_dup(calc);kk_ref_dup(r);}, {}, _ctx)
  kk_std_core_types__maybe x_17769;
  kk_box_t _x20101;
  kk_ref_t _x20102 = kk_ref_dup(r); /*ref<_2842,maybe<2933>>*/
  _x20101 = kk_ref_get(_x20102,kk_context()); /*1001*/
  x_17769 = kk_std_core_types__maybe_unbox(_x20101, _ctx); /*maybe<2933>*/
  kk_function_t next0_17770 = kk_std_core_new_once_fun20103(calc, r, _ctx); /*(maybe<2933>) -> <read<_2842>,write<_2842>,div|_2925> 2933*/;
  if (kk_yielding(kk_context())) {
    kk_std_core_types__maybe_drop(x_17769, _ctx);
    return kk_std_core_hnd_yield_extend(kk_std_core_new_once_fun20107(next0_17770, _ctx), _ctx);
  }
  {
    return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_types__maybe, kk_context_t*), next0_17770, (next0_17770, x_17769, _ctx));
  }
}

kk_function_t kk_std_core_once(kk_function_t calc, kk_context_t* _ctx) { /* forall<a> (calc : () -> a) -> (() -> a) */ 
  kk_ref_t r = kk_ref_alloc((kk_std_core_types__maybe_box(kk_std_core_types__new_Nothing(_ctx), _ctx)),kk_context()); /*ref<_2842,maybe<2933>>*/;
  return kk_std_core_new_once_fun20100(calc, r, _ctx);
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
  kk_integer_t _brw_19873;
  if (kk_std_core_types__is_Lt(x)) {
    _brw_19873 = kk_integer_from_small(-1); /*int*/
    goto _match20109;
  }
  if (kk_std_core_types__is_Eq(x)) {
    _brw_19873 = kk_integer_from_small(0); /*int*/
    goto _match20109;
  }
  {
    _brw_19873 = kk_integer_from_small(1); /*int*/
  }
  _match20109: ;
  kk_integer_t _brw_19872;
  if (kk_std_core_types__is_Lt(y)) {
    _brw_19872 = kk_integer_from_small(-1); /*int*/
    goto _match20110;
  }
  if (kk_std_core_types__is_Eq(y)) {
    _brw_19872 = kk_integer_from_small(0); /*int*/
    goto _match20110;
  }
  {
    _brw_19872 = kk_integer_from_small(1); /*int*/
  }
  _match20110: ;
  bool _brw_19874 = kk_integer_neq_borrow(_brw_19873,_brw_19872,kk_context()); /*bool*/;
  kk_integer_drop(_brw_19873, _ctx);
  kk_integer_drop(_brw_19872, _ctx);
  return _brw_19874;
}
 
// Append two lists.

kk_std_core__list kk_std_core__ctail_append(kk_std_core__list xs, kk_std_core__list ys, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, ys : list<a>, ctail<list<a>>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con20112 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con20112->head;
    kk_std_core__list xx = _con20112->tail;
    kk_reuse_t _ru_19904 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      _ru_19904 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_std_core__list _ctail_17250 = kk_std_core__list_hole(); /*list<3224>*/;
    kk_std_core__list _ctail_17251 = kk_std_core__new_Cons(_ru_19904, x, _ctail_17250, _ctx); /*list<3224>*/;
    kk_box_t* _b_18349_18344 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17251)->tail)); /*cfield<list<3224>>*/;
    { // tailcall
      kk_std_core_types__ctail _x20113 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17251, _ctx)),_b_18349_18344); /*ctail<0>*/
      xs = xx;
      _acc = _x20113;
      goto kk__tailcall;
    }
  }
  {
    kk_box_t _x20114 = kk_ctail_resolve(_acc,(kk_std_core__list_box(ys, _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20114, _ctx);
  }
}
 
// Append two lists.

kk_std_core__list kk_std_core_append(kk_std_core__list xs0, kk_std_core__list ys0, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, ys : list<a>) -> list<a> */ 
  kk_std_core_types__ctail _x20115 = kk_ctail_nil(); /*ctail<0>*/
  return kk_std_core__ctail_append(xs0, ys0, _x20115, _ctx);
}
 
// Append two strings.

kk_string_t kk_std_core__lp__plus__plus__1_rp_(kk_string_t x, kk_string_t y, kk_context_t* _ctx) { /* (x : string, y : string) -> string */ 
  return kk_string_cat(x,y,kk_context());
}

kk_unit_t kk_std_core_printsln(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> console () */ 
  kk_std_core_types__maybe _match_19871;
  kk_box_t _x20117;
  kk_ref_t _x20118 = kk_ref_dup(kk_std_core_redirect); /*ref<global,maybe<(string) -> console ()>>*/
  _x20117 = kk_ref_get(_x20118,kk_context()); /*1001*/
  _match_19871 = kk_std_core_types__maybe_unbox(_x20117, _ctx); /*maybe<(string) -> console ()>*/
  if (kk_std_core_types__is_Nothing(_match_19871)) {
    kk_std_core_xprintsln(s, _ctx); return kk_Unit;
  }
  {
    kk_box_t _fun_unbox_x18359 = _match_19871._cons.Just.value;
    kk_string_t _b_18362;
    kk_string_t _x20119;
    kk_define_string_literal(, _s20120, 1, "\n")
    _x20119 = kk_string_dup(_s20120); /*string*/
    _b_18362 = kk_std_core__lp__plus__plus__1_rp_(s, _x20119, _ctx); /*string*/
    kk_box_t _x20121;
    kk_function_t _x20122 = kk_function_unbox(_fun_unbox_x18359); /*(18360) -> console 18361*/
    _x20121 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x20122, (_x20122, kk_string_box(_b_18362), _ctx)); /*18361*/
    kk_unit_unbox(_x20121); return kk_Unit;
  }
}

bool kk_std_core__lp__eq__eq__4_rp_(kk_std_core_types__order x, kk_std_core_types__order y, kk_context_t* _ctx) { /* (x : order, y : order) -> bool */ 
  kk_integer_t _brw_19869;
  if (kk_std_core_types__is_Lt(x)) {
    _brw_19869 = kk_integer_from_small(-1); /*int*/
    goto _match20123;
  }
  if (kk_std_core_types__is_Eq(x)) {
    _brw_19869 = kk_integer_from_small(0); /*int*/
    goto _match20123;
  }
  {
    _brw_19869 = kk_integer_from_small(1); /*int*/
  }
  _match20123: ;
  kk_integer_t _brw_19868;
  if (kk_std_core_types__is_Lt(y)) {
    _brw_19868 = kk_integer_from_small(-1); /*int*/
    goto _match20124;
  }
  if (kk_std_core_types__is_Eq(y)) {
    _brw_19868 = kk_integer_from_small(0); /*int*/
    goto _match20124;
  }
  {
    _brw_19868 = kk_integer_from_small(1); /*int*/
  }
  _match20124: ;
  bool _brw_19870 = kk_integer_eq_borrow(_brw_19869,_brw_19868,kk_context()); /*bool*/;
  kk_integer_drop(_brw_19869, _ctx);
  kk_integer_drop(_brw_19868, _ctx);
  return _brw_19870;
}

bool kk_std_core__lp__lt__5_rp_(kk_std_core_types__order x, kk_std_core_types__order y, kk_context_t* _ctx) { /* (x : order, y : order) -> bool */ 
  kk_integer_t _brw_19866;
  if (kk_std_core_types__is_Lt(x)) {
    _brw_19866 = kk_integer_from_small(-1); /*int*/
    goto _match20125;
  }
  if (kk_std_core_types__is_Eq(x)) {
    _brw_19866 = kk_integer_from_small(0); /*int*/
    goto _match20125;
  }
  {
    _brw_19866 = kk_integer_from_small(1); /*int*/
  }
  _match20125: ;
  kk_integer_t _brw_19865;
  if (kk_std_core_types__is_Lt(y)) {
    _brw_19865 = kk_integer_from_small(-1); /*int*/
    goto _match20126;
  }
  if (kk_std_core_types__is_Eq(y)) {
    _brw_19865 = kk_integer_from_small(0); /*int*/
    goto _match20126;
  }
  {
    _brw_19865 = kk_integer_from_small(1); /*int*/
  }
  _match20126: ;
  bool _brw_19867 = kk_integer_lt_borrow(_brw_19866,_brw_19865,kk_context()); /*bool*/;
  kk_integer_drop(_brw_19866, _ctx);
  kk_integer_drop(_brw_19865, _ctx);
  return _brw_19867;
}

kk_std_core_types__order kk_std_core_order(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> order */ 
  bool _match_19862 = kk_integer_lt_borrow(i,(kk_integer_from_small(0)),kk_context()); /*bool*/;
  if (_match_19862) {
    kk_integer_drop(i, _ctx);
    return kk_std_core_types__new_Lt(_ctx);
  }
  {
    bool _match_19863;
    bool _brw_19864 = kk_integer_gt_borrow(i,(kk_integer_from_small(0)),kk_context()); /*bool*/;
    kk_integer_drop(i, _ctx);
    _match_19863 = _brw_19864; /*bool*/
    if (_match_19863) {
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
  bool _match_19859 = kk_integer_lt_borrow(i_16964,(kk_integer_from_small(0)),kk_context()); /*bool*/;
  if (_match_19859) {
    kk_integer_drop(i_16964, _ctx);
    return kk_std_core_types__new_Lt(_ctx);
  }
  {
    bool _match_19860;
    bool _brw_19861 = kk_integer_gt_borrow(i_16964,(kk_integer_from_small(0)),kk_context()); /*bool*/;
    kk_integer_drop(i_16964, _ctx);
    _match_19860 = _brw_19861; /*bool*/
    if (_match_19860) {
      return kk_std_core_types__new_Gt(_ctx);
    }
    {
      return kk_std_core_types__new_Eq(_ctx);
    }
  }
}

bool kk_std_core__lp__gt__3_rp_(kk_std_core_types__order x, kk_std_core_types__order y, kk_context_t* _ctx) { /* (x : order, y : order) -> bool */ 
  kk_integer_t _brw_19855;
  if (kk_std_core_types__is_Lt(x)) {
    _brw_19855 = kk_integer_from_small(-1); /*int*/
    goto _match20128;
  }
  if (kk_std_core_types__is_Eq(x)) {
    _brw_19855 = kk_integer_from_small(0); /*int*/
    goto _match20128;
  }
  {
    _brw_19855 = kk_integer_from_small(1); /*int*/
  }
  _match20128: ;
  kk_integer_t _brw_19854;
  if (kk_std_core_types__is_Lt(y)) {
    _brw_19854 = kk_integer_from_small(-1); /*int*/
    goto _match20129;
  }
  if (kk_std_core_types__is_Eq(y)) {
    _brw_19854 = kk_integer_from_small(0); /*int*/
    goto _match20129;
  }
  {
    _brw_19854 = kk_integer_from_small(1); /*int*/
  }
  _match20129: ;
  bool _brw_19856 = kk_integer_gt_borrow(_brw_19855,_brw_19854,kk_context()); /*bool*/;
  kk_integer_drop(_brw_19855, _ctx);
  kk_integer_drop(_brw_19854, _ctx);
  return _brw_19856;
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
  kk_integer_t _brw_19848;
  if (kk_std_core_types__is_Lt(x)) {
    _brw_19848 = kk_integer_from_small(-1); /*int*/
    goto _match20131;
  }
  if (kk_std_core_types__is_Eq(x)) {
    _brw_19848 = kk_integer_from_small(0); /*int*/
    goto _match20131;
  }
  {
    _brw_19848 = kk_integer_from_small(1); /*int*/
  }
  _match20131: ;
  kk_integer_t _brw_19847;
  if (kk_std_core_types__is_Lt(y)) {
    _brw_19847 = kk_integer_from_small(-1); /*int*/
    goto _match20132;
  }
  if (kk_std_core_types__is_Eq(y)) {
    _brw_19847 = kk_integer_from_small(0); /*int*/
    goto _match20132;
  }
  {
    _brw_19847 = kk_integer_from_small(1); /*int*/
  }
  _match20132: ;
  bool _brw_19849 = kk_integer_gte_borrow(_brw_19848,_brw_19847,kk_context()); /*bool*/;
  kk_integer_drop(_brw_19848, _ctx);
  kk_integer_drop(_brw_19847, _ctx);
  return _brw_19849;
}
 
// lifted local: joinsep, join-acc

kk_string_t kk_std_core__lift17188_joinsep(kk_string_t sep, kk_std_core__list ys, kk_string_t acc, kk_context_t* _ctx) { /* (sep : string, ys : list<string>, acc : string) -> string */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con20134 = kk_std_core__as_Cons(ys);
    kk_box_t _box_x18364 = _con20134->head;
    kk_std_core__list yy = _con20134->tail;
    kk_string_t y = kk_string_unbox(_box_x18364);
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      kk_std_core__list_free(ys, _ctx);
    }
    else {
      kk_string_dup(y);
      kk_std_core__list_dup(yy);
      kk_std_core__list_decref(ys, _ctx);
    }
    kk_string_t acc0_17216;
    kk_string_t _x20136;
    kk_string_t _x20137 = kk_string_dup(sep); /*string*/
    _x20136 = kk_std_core__lp__plus__plus__1_rp_(_x20137, y, _ctx); /*string*/
    acc0_17216 = kk_std_core__lp__plus__plus__1_rp_(acc, _x20136, _ctx); /*string*/
    { // tailcall
      ys = yy;
      acc = acc0_17216;
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
    struct kk_std_core_Cons* _con20139 = kk_std_core__as_Cons(xs);
    kk_box_t _box_x18365 = _con20139->head;
    kk_std_core__list xx = _con20139->tail;
    kk_string_t x = kk_string_unbox(_box_x18365);
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_string_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    return kk_std_core__lift17188_joinsep(sep, xx, x, _ctx);
  }
}
 
// lifted local: join.2, join-acc

kk_string_t kk_std_core__lift17189_join_2(kk_std_core__list ys, kk_string_t acc, kk_context_t* _ctx) { /* (ys : list<string>, acc : string) -> string */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con20141 = kk_std_core__as_Cons(ys);
    kk_box_t _box_x18366 = _con20141->head;
    kk_std_core__list yy = _con20141->tail;
    kk_string_t y = kk_string_unbox(_box_x18366);
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      kk_std_core__list_free(ys, _ctx);
    }
    else {
      kk_string_dup(y);
      kk_std_core__list_dup(yy);
      kk_std_core__list_decref(ys, _ctx);
    }
    { // tailcall
      kk_string_t _x20143;
      kk_string_t _x20144;
      kk_string_t _x20145 = kk_string_empty(); /*string*/
      _x20144 = kk_std_core__lp__plus__plus__1_rp_(_x20145, y, _ctx); /*string*/
      _x20143 = kk_std_core__lp__plus__plus__1_rp_(acc, _x20144, _ctx); /*string*/
      ys = yy;
      acc = _x20143;
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
    struct kk_std_core_Cons* _con20148 = kk_std_core__as_Cons(xs);
    kk_box_t _box_x18367 = _con20148->head;
    kk_std_core__list xx = _con20148->tail;
    kk_string_t x = kk_string_unbox(_box_x18367);
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_string_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    return kk_std_core__lift17189_join_2(xx, x, _ctx);
  }
}
 
// Concatenate all strings in a list using a specific separator

kk_string_t kk_std_core_join_3(kk_std_core__list xs, kk_string_t sep, kk_context_t* _ctx) { /* (xs : list<string>, sep : string) -> string */ 
  if (kk_std_core__is_Nil(xs)) {
    kk_string_drop(sep, _ctx);
    return kk_string_empty();
  }
  {
    struct kk_std_core_Cons* _con20151 = kk_std_core__as_Cons(xs);
    kk_box_t _box_x18368 = _con20151->head;
    kk_std_core__list xx = _con20151->tail;
    kk_string_t x = kk_string_unbox(_box_x18368);
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_string_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    return kk_std_core__lift17188_joinsep(sep, xx, x, _ctx);
  }
}

bool kk_std_core__lp__lt__eq__5_rp_(kk_std_core_types__order x, kk_std_core_types__order y, kk_context_t* _ctx) { /* (x : order, y : order) -> bool */ 
  kk_integer_t _brw_19843;
  if (kk_std_core_types__is_Lt(x)) {
    _brw_19843 = kk_integer_from_small(-1); /*int*/
    goto _match20153;
  }
  if (kk_std_core_types__is_Eq(x)) {
    _brw_19843 = kk_integer_from_small(0); /*int*/
    goto _match20153;
  }
  {
    _brw_19843 = kk_integer_from_small(1); /*int*/
  }
  _match20153: ;
  kk_integer_t _brw_19842;
  if (kk_std_core_types__is_Lt(y)) {
    _brw_19842 = kk_integer_from_small(-1); /*int*/
    goto _match20154;
  }
  if (kk_std_core_types__is_Eq(y)) {
    _brw_19842 = kk_integer_from_small(0); /*int*/
    goto _match20154;
  }
  {
    _brw_19842 = kk_integer_from_small(1); /*int*/
  }
  _match20154: ;
  bool _brw_19844 = kk_integer_lte_borrow(_brw_19843,_brw_19842,kk_context()); /*bool*/;
  kk_integer_drop(_brw_19843, _ctx);
  kk_integer_drop(_brw_19842, _ctx);
  return _brw_19844;
}

kk_ssize_t kk_std_core_decr_1(kk_ssize_t i, kk_context_t* _ctx) { /* (i : ssize_t) -> ssize_t */ 
  return (i - 1);
}

kk_ssize_t kk_std_core_incr_1(kk_ssize_t i, kk_context_t* _ctx) { /* (i : ssize_t) -> ssize_t */ 
  return (i + 1);
}
 
// monadic lift

kk_unit_t kk_std_core__mlift17675_op(kk_function_t action, kk_ssize_t end, kk_ssize_t i, kk_unit_t wild__, kk_context_t* _ctx) { /* forall<e> (action : (ssize_t) -> e (), end : ssize_t, i : ssize_t, wild_ : ()) -> e () */ 
  kk_ssize_t i0_17219 = kk_std_core_incr_1(i, _ctx); /*ssize_t*/;
  kk_std_core__lift17190_forz(action, end, i0_17219, _ctx); return kk_Unit;
}
 
// lifted local: forz, rep


// lift anonymous function
struct kk_std_core__lift17190_forz_fun20158__t {
  struct kk_function_s _base;
  kk_function_t action0;
  kk_ssize_t end0;
  kk_ssize_t i0;
};
static kk_box_t kk_std_core__lift17190_forz_fun20158(kk_function_t _fself, kk_box_t _b_18370, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17190_forz_fun20158(kk_function_t action0, kk_ssize_t end0, kk_ssize_t i0, kk_context_t* _ctx) {
  struct kk_std_core__lift17190_forz_fun20158__t* _self = kk_function_alloc_as(struct kk_std_core__lift17190_forz_fun20158__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift17190_forz_fun20158, kk_context());
  _self->action0 = action0;
  _self->end0 = end0;
  _self->i0 = i0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift17190_forz_fun20158(kk_function_t _fself, kk_box_t _b_18370, kk_context_t* _ctx) {
  struct kk_std_core__lift17190_forz_fun20158__t* _self = kk_function_as(struct kk_std_core__lift17190_forz_fun20158__t*, _fself);
  kk_function_t action0 = _self->action0; /* (ssize_t) -> 4394 () */
  kk_ssize_t end0 = _self->end0; /* ssize_t */
  kk_ssize_t i0 = _self->i0; /* ssize_t */
  kk_drop_match(_self, {kk_function_dup(action0);;;}, {}, _ctx)
  kk_unit_t wild__0_18372 = kk_Unit;
  kk_unit_unbox(_b_18370);
  kk_unit_t _x20159 = kk_Unit;
  kk_std_core__mlift17675_op(action0, end0, i0, wild__0_18372, _ctx);
  return kk_unit_box(_x20159);
}

kk_unit_t kk_std_core__lift17190_forz(kk_function_t action0, kk_ssize_t end0, kk_ssize_t i0, kk_context_t* _ctx) { /* forall<e> (action : (ssize_t) -> e (), end : ssize_t, i : ssize_t) -> e () */ 
  kk__tailcall: ;
  bool _match_19838 = (i0 <= end0); /*bool*/;
  if (_match_19838) {
    kk_unit_t x_17777 = kk_Unit;
    kk_function_t _x20156 = kk_function_dup(action0); /*(ssize_t) -> 4394 ()*/
    kk_function_call(kk_unit_t, (kk_function_t, kk_ssize_t, kk_context_t*), _x20156, (_x20156, i0, _ctx));
    if (kk_yielding(kk_context())) {
      kk_box_t _x20157 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift17190_forz_fun20158(action0, end0, i0, _ctx), _ctx); /*3926*/
      kk_unit_unbox(_x20157); return kk_Unit;
    }
    {
      kk_ssize_t i0_172190 = kk_std_core_incr_1(i0, _ctx); /*ssize_t*/;
      { // tailcall
        i0 = i0_172190;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_function_drop(action0, _ctx);
    kk_Unit; return kk_Unit;
  }
}


// lift anonymous function
struct kk_std_core_foreach_indexedz_fun20161__t {
  struct kk_function_s _base;
  kk_function_t f;
  kk_vector_t v;
};
static kk_unit_t kk_std_core_foreach_indexedz_fun20161(kk_function_t _fself, kk_ssize_t i, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_indexedz_fun20161(kk_function_t f, kk_vector_t v, kk_context_t* _ctx) {
  struct kk_std_core_foreach_indexedz_fun20161__t* _self = kk_function_alloc_as(struct kk_std_core_foreach_indexedz_fun20161__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foreach_indexedz_fun20161, kk_context());
  _self->f = f;
  _self->v = v;
  return &_self->_base;
}

static kk_unit_t kk_std_core_foreach_indexedz_fun20161(kk_function_t _fself, kk_ssize_t i, kk_context_t* _ctx) {
  struct kk_std_core_foreach_indexedz_fun20161__t* _self = kk_function_as(struct kk_std_core_foreach_indexedz_fun20161__t*, _fself);
  kk_function_t f = _self->f; /* (ssize_t, 4428) -> 4429 () */
  kk_vector_t v = _self->v; /* vector<4428> */
  kk_drop_match(_self, {kk_function_dup(f);kk_vector_dup(v);}, {}, _ctx)
  kk_box_t _x20162;
  kk_box_t _brw_19837 = kk_vector_at_borrow(v,i); /*228*/;
  kk_vector_drop(v, _ctx);
  _x20162 = _brw_19837; /*228*/
  return kk_function_call(kk_unit_t, (kk_function_t, kk_ssize_t, kk_box_t, kk_context_t*), f, (f, i, _x20162, _ctx));
}

kk_unit_t kk_std_core_foreach_indexedz(kk_vector_t v, kk_function_t f, kk_context_t* _ctx) { /* forall<a,e> (v : vector<a>, f : (ssize_t, a) -> e ()) -> e () */ 
  kk_ssize_t start0_17780 = (KK_IZ(0)); /*ssize_t*/;
  kk_ssize_t end_17781;
  kk_ssize_t _x20160 = kk_vector_len_borrow(v); /*ssize_t*/
  end_17781 = kk_std_core_decr_1(_x20160, _ctx); /*ssize_t*/
  kk_std_core__lift17190_forz(kk_std_core_new_foreach_indexedz_fun20161(f, v, _ctx), end_17781, start0_17780, _ctx); return kk_Unit;
}
 
// lifted local: length.1, len0

kk_integer_t kk_std_core__lift17191_length_1(kk_std_core__list ys, kk_integer_t acc, kk_context_t* _ctx) { /* forall<a> (ys : list<a>, acc : int) -> int */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con20163 = kk_std_core__as_Cons(ys);
    kk_box_t _pat0 = _con20163->head;
    kk_std_core__list yy = _con20163->tail;
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      kk_box_drop(_pat0, _ctx);
      kk_std_core__list_free(ys, _ctx);
    }
    else {
      kk_std_core__list_dup(yy);
      kk_std_core__list_decref(ys, _ctx);
    }
    { // tailcall
      kk_integer_t _x20164 = kk_integer_add_small_const(acc, 1, _ctx); /*int*/
      ys = yy;
      acc = _x20164;
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
  bool _match_19836 = kk_integer_lte_borrow(lo,hi,kk_context()); /*bool*/;
  if (_match_19836) {
    kk_std_core__list _ctail_17252 = kk_std_core__list_hole(); /*list<int>*/;
    kk_std_core__list _ctail_17253;
    kk_box_t _x20166;
    kk_integer_t _x20167 = kk_integer_dup(lo); /*int*/
    _x20166 = kk_integer_box(_x20167); /*61*/
    _ctail_17253 = kk_std_core__new_Cons(kk_reuse_null, _x20166, _ctail_17252, _ctx); /*list<int>*/
    kk_box_t* _b_18387_18382 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17253)->tail)); /*cfield<list<int>>*/;
    { // tailcall
      kk_integer_t _x20168 = kk_integer_add_small_const(lo, 1, _ctx); /*int*/
      kk_std_core_types__ctail _x20169 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17253, _ctx)),_b_18387_18382); /*ctail<0>*/
      lo = _x20168;
      _acc = _x20169;
      goto kk__tailcall;
    }
  }
  {
    kk_integer_drop(lo, _ctx);
    kk_integer_drop(hi, _ctx);
    kk_box_t _x20170 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20170, _ctx);
  }
}
 
// Returns an integer list of increasing elements from `lo`  to `hi`
// (including both `lo`  and `hi` ).
// If `lo > hi`  the function returns the empty list.

kk_std_core__list kk_std_core_list(kk_integer_t lo0, kk_integer_t hi0, kk_context_t* _ctx) { /* (lo : int, hi : int) -> total list<int> */ 
  kk_std_core_types__ctail _x20171 = kk_ctail_nil(); /*ctail<0>*/
  return kk_std_core__ctail_list(lo0, hi0, _x20171, _ctx);
}
 
// Returns an integer list of increasing elements from `lo`  to `hi` with stride `stride`.
// If `lo > hi`  the function returns the empty list.

kk_std_core__list kk_std_core__ctail_list_1(kk_integer_t lo, kk_integer_t hi, kk_integer_t stride, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* (lo : int, hi : int, stride : int, ctail<list<int>>) -> total list<int> */ 
  kk__tailcall: ;
  bool _match_19835 = kk_integer_lte_borrow(lo,hi,kk_context()); /*bool*/;
  if (_match_19835) {
    kk_std_core__list _ctail_17254 = kk_std_core__list_hole(); /*list<int>*/;
    kk_std_core__list _ctail_17255;
    kk_box_t _x20172;
    kk_integer_t _x20173 = kk_integer_dup(lo); /*int*/
    _x20172 = kk_integer_box(_x20173); /*61*/
    _ctail_17255 = kk_std_core__new_Cons(kk_reuse_null, _x20172, _ctail_17254, _ctx); /*list<int>*/
    kk_box_t* _b_18407_18402 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17255)->tail)); /*cfield<list<int>>*/;
    { // tailcall
      kk_integer_t _x20174;
      kk_integer_t _x20175 = kk_integer_dup(stride); /*int*/
      _x20174 = kk_integer_add(lo,_x20175,kk_context()); /*int*/
      kk_std_core_types__ctail _x20176 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17255, _ctx)),_b_18407_18402); /*ctail<0>*/
      lo = _x20174;
      _acc = _x20176;
      goto kk__tailcall;
    }
  }
  {
    kk_integer_drop(stride, _ctx);
    kk_integer_drop(lo, _ctx);
    kk_integer_drop(hi, _ctx);
    kk_box_t _x20177 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20177, _ctx);
  }
}
 
// Returns an integer list of increasing elements from `lo`  to `hi` with stride `stride`.
// If `lo > hi`  the function returns the empty list.

kk_std_core__list kk_std_core_list_1(kk_integer_t lo0, kk_integer_t hi0, kk_integer_t stride0, kk_context_t* _ctx) { /* (lo : int, hi : int, stride : int) -> total list<int> */ 
  kk_std_core_types__ctail _x20178 = kk_ctail_nil(); /*ctail<0>*/
  return kk_std_core__ctail_list_1(lo0, hi0, stride0, _x20178, _ctx);
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift17676_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_integer_t hi, kk_integer_t lo, kk_box_t _ctail_17256, kk_context_t* _ctx) { /* forall<a,e> (ctail<list<a>>, f : (int) -> e a, hi : int, lo : int, a) -> e list<a> */ 
  kk_std_core__list _ctail_17257 = kk_std_core__list_hole(); /*list<4982>*/;
  kk_std_core__list _ctail_17258 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17256, _ctail_17257, _ctx); /*list<4982>*/;
  kk_box_t* _b_18421_18418 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17258)->tail)); /*cfield<list<4982>>*/;
  kk_integer_t _x20179 = kk_integer_add_small_const(lo, 1, _ctx); /*int*/
  kk_std_core_types__ctail _x20180 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17258, _ctx)),_b_18421_18418); /*ctail<0>*/
  return kk_std_core__ctail_list_2(_x20179, hi, f, _x20180, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift17677_op_fun20182__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t _ctail_17261;
};
static kk_std_core__list kk_std_core__mlift17677_op_fun20182(kk_function_t _fself, kk_std_core__list _ctail_17260, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17677_op_fun20182(kk_function_t _accm, kk_box_t _ctail_17261, kk_context_t* _ctx) {
  struct kk_std_core__mlift17677_op_fun20182__t* _self = kk_function_alloc_as(struct kk_std_core__mlift17677_op_fun20182__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17677_op_fun20182, kk_context());
  _self->_accm = _accm;
  _self->_ctail_17261 = _ctail_17261;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift17677_op_fun20182(kk_function_t _fself, kk_std_core__list _ctail_17260, kk_context_t* _ctx) {
  struct kk_std_core__mlift17677_op_fun20182__t* _self = kk_function_as(struct kk_std_core__mlift17677_op_fun20182__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<4982>) -> list<4982> */
  kk_box_t _ctail_17261 = _self->_ctail_17261; /* 4982 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(_ctail_17261);}, {}, _ctx)
  kk_std_core__list _x20183 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17261, _ctail_17260, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x20183, _ctx));
}

kk_std_core__list kk_std_core__mlift17677_op(kk_function_t _accm, kk_function_t f0, kk_integer_t hi0, kk_integer_t lo0, kk_box_t _ctail_17261, kk_context_t* _ctx) { /* forall<a,e> ((list<a>) -> list<a>, f : (int) -> e a, hi : int, lo : int, a) -> e list<a> */ 
  kk_integer_t _x20181 = kk_integer_add_small_const(lo0, 1, _ctx); /*int*/
  return kk_std_core__ctailm_list_2(_x20181, hi0, f0, kk_std_core__new_mlift17677_op_fun20182(_accm, _ctail_17261, _ctx), _ctx);
}
 
// Applies a function `f` to list of increasing elements from `lo`  to `hi`
// (including both `lo`  and `hi` ).
// If `lo > hi`  the function returns the empty list.


// lift anonymous function
struct kk_std_core__ctail_list_fun20187__t_2 {
  struct kk_function_s _base;
  kk_function_t f1;
  kk_integer_t hi1;
  kk_integer_t lo1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_list_fun20187_2(kk_function_t _fself, kk_box_t _b_18426, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_list_fun20187_2(kk_function_t f1, kk_integer_t hi1, kk_integer_t lo1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_list_fun20187__t_2* _self = kk_function_alloc_as(struct kk_std_core__ctail_list_fun20187__t_2, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_list_fun20187_2, kk_context());
  _self->f1 = f1;
  _self->hi1 = hi1;
  _self->lo1 = lo1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_list_fun20187_2(kk_function_t _fself, kk_box_t _b_18426, kk_context_t* _ctx) {
  struct kk_std_core__ctail_list_fun20187__t_2* _self = kk_function_as(struct kk_std_core__ctail_list_fun20187__t_2*, _fself);
  kk_function_t f1 = _self->f1; /* (int) -> 4983 4982 */
  kk_integer_t hi1 = _self->hi1; /* int */
  kk_integer_t lo1 = _self->lo1; /* int */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<4982>> */
  kk_drop_match(_self, {kk_function_dup(f1);kk_integer_dup(hi1);kk_integer_dup(lo1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_box_t _ctail_18444_172560 = _b_18426; /*4982*/;
  kk_std_core__list _x20188 = kk_std_core__mlift17676_op(_acc0, f1, hi1, lo1, _ctail_18444_172560, _ctx); /*list<4982>*/
  return kk_std_core__list_box(_x20188, _ctx);
}

kk_std_core__list kk_std_core__ctail_list_2(kk_integer_t lo1, kk_integer_t hi1, kk_function_t f1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,e> (lo : int, hi : int, f : (int) -> e a, ctail<list<a>>) -> e list<a> */ 
  kk__tailcall: ;
  bool _match_19833 = kk_integer_lte_borrow(lo1,hi1,kk_context()); /*bool*/;
  if (_match_19833) {
    kk_box_t x_17783;
    kk_function_t _x20185 = kk_function_dup(f1); /*(int) -> 4983 4982*/
    kk_integer_t _x20184 = kk_integer_dup(lo1); /*int*/
    x_17783 = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_context_t*), _x20185, (_x20185, _x20184, _ctx)); /*4982*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x_17783, _ctx);
      kk_box_t _x20186 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_list_fun20187_2(f1, hi1, lo1, _acc0, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20186, _ctx);
    }
    {
      kk_std_core__list _ctail_172570 = kk_std_core__list_hole(); /*list<4982>*/;
      kk_std_core__list _ctail_172580 = kk_std_core__new_Cons(kk_reuse_null, x_17783, _ctail_172570, _ctx); /*list<4982>*/;
      kk_box_t* _b_18438_18432 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_172580)->tail)); /*cfield<list<4982>>*/;
      { // tailcall
        kk_integer_t _x20189 = kk_integer_add_small_const(lo1, 1, _ctx); /*int*/
        kk_std_core_types__ctail _x20190 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_172580, _ctx)),_b_18438_18432); /*ctail<0>*/
        lo1 = _x20189;
        _acc0 = _x20190;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_integer_drop(lo1, _ctx);
    kk_integer_drop(hi1, _ctx);
    kk_function_drop(f1, _ctx);
    kk_box_t _x20191 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20191, _ctx);
  }
}
 
// Applies a function `f` to list of increasing elements from `lo`  to `hi`
// (including both `lo`  and `hi` ).
// If `lo > hi`  the function returns the empty list.


// lift anonymous function
struct kk_std_core__ctailm_list_fun20195__t_2 {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t f2;
  kk_integer_t hi2;
  kk_integer_t lo2;
};
static kk_box_t kk_std_core__ctailm_list_fun20195_2(kk_function_t _fself, kk_box_t _b_18446, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_list_fun20195_2(kk_function_t _accm0, kk_function_t f2, kk_integer_t hi2, kk_integer_t lo2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_list_fun20195__t_2* _self = kk_function_alloc_as(struct kk_std_core__ctailm_list_fun20195__t_2, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_list_fun20195_2, kk_context());
  _self->_accm0 = _accm0;
  _self->f2 = f2;
  _self->hi2 = hi2;
  _self->lo2 = lo2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_list_fun20195_2(kk_function_t _fself, kk_box_t _b_18446, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_list_fun20195__t_2* _self = kk_function_as(struct kk_std_core__ctailm_list_fun20195__t_2*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<4982>) -> list<4982> */
  kk_function_t f2 = _self->f2; /* (int) -> 4983 4982 */
  kk_integer_t hi2 = _self->hi2; /* int */
  kk_integer_t lo2 = _self->lo2; /* int */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(f2);kk_integer_dup(hi2);kk_integer_dup(lo2);}, {}, _ctx)
  kk_box_t _ctail_18448_172610 = _b_18446; /*4982*/;
  kk_std_core__list _x20196 = kk_std_core__mlift17677_op(_accm0, f2, hi2, lo2, _ctail_18448_172610, _ctx); /*list<4982>*/
  return kk_std_core__list_box(_x20196, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_list_fun20199__t_2 {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t x0_17786;
};
static kk_std_core__list kk_std_core__ctailm_list_fun20199_2(kk_function_t _fself, kk_std_core__list _ctail_172600, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_list_fun20199_2(kk_function_t _accm0, kk_box_t x0_17786, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_list_fun20199__t_2* _self = kk_function_alloc_as(struct kk_std_core__ctailm_list_fun20199__t_2, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_list_fun20199_2, kk_context());
  _self->_accm0 = _accm0;
  _self->x0_17786 = x0_17786;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_list_fun20199_2(kk_function_t _fself, kk_std_core__list _ctail_172600, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_list_fun20199__t_2* _self = kk_function_as(struct kk_std_core__ctailm_list_fun20199__t_2*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<4982>) -> list<4982> */
  kk_box_t x0_17786 = _self->x0_17786; /* 4982 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(x0_17786);}, {}, _ctx)
  kk_std_core__list _x20200 = kk_std_core__new_Cons(kk_reuse_null, x0_17786, _ctail_172600, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x20200, _ctx));
}

kk_std_core__list kk_std_core__ctailm_list_2(kk_integer_t lo2, kk_integer_t hi2, kk_function_t f2, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,e> (lo : int, hi : int, f : (int) -> e a, (list<a>) -> list<a>) -> e list<a> */ 
  kk__tailcall: ;
  bool _match_19831 = kk_integer_lte_borrow(lo2,hi2,kk_context()); /*bool*/;
  if (_match_19831) {
    kk_box_t x0_17786;
    kk_function_t _x20193 = kk_function_dup(f2); /*(int) -> 4983 4982*/
    kk_integer_t _x20192 = kk_integer_dup(lo2); /*int*/
    x0_17786 = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_context_t*), _x20193, (_x20193, _x20192, _ctx)); /*4982*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x0_17786, _ctx);
      kk_box_t _x20194 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_list_fun20195_2(_accm0, f2, hi2, lo2, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20194, _ctx);
    }
    { // tailcall
      kk_integer_t _x20197 = kk_integer_add_small_const(lo2, 1, _ctx); /*int*/
      kk_function_t _x20198 = kk_std_core__new_ctailm_list_fun20199_2(_accm0, x0_17786, _ctx); /*(list<4982>) -> list<4982>*/
      lo2 = _x20197;
      _accm0 = _x20198;
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
struct kk_std_core_list_fun20202__t_2 {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core_list_fun20202_2(kk_function_t _fself, kk_std_core__list _ctail_17259, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_list_fun20202_2(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_list_fun20202_2, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core_list_fun20202_2(kk_function_t _fself, kk_std_core__list _ctail_17259, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17259;
}

kk_std_core__list kk_std_core_list_2(kk_integer_t lo3, kk_integer_t hi3, kk_function_t f3, kk_context_t* _ctx) { /* forall<a,e> (lo : int, hi : int, f : (int) -> e a) -> e list<a> */ 
  bool _match_19830 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_19830) {
    kk_std_core_types__ctail _x20201 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_list_2(lo3, hi3, f3, _x20201, _ctx);
  }
  {
    return kk_std_core__ctailm_list_2(lo3, hi3, f3, kk_std_core_new_list_fun20202_2(_ctx), _ctx);
  }
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift17678_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_integer_t hi, kk_integer_t lo, kk_integer_t stride, kk_box_t _ctail_17262, kk_context_t* _ctx) { /* forall<a,e> (ctail<list<a>>, f : (int) -> e a, hi : int, lo : int, stride : int, a) -> e list<a> */ 
  kk_std_core__list _ctail_17263 = kk_std_core__list_hole(); /*list<5073>*/;
  kk_std_core__list _ctail_17264 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17262, _ctail_17263, _ctx); /*list<5073>*/;
  kk_box_t* _b_18457_18454 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17264)->tail)); /*cfield<list<5073>>*/;
  kk_integer_t _x20203;
  kk_integer_t _x20204 = kk_integer_dup(stride); /*int*/
  _x20203 = kk_integer_add(lo,_x20204,kk_context()); /*int*/
  kk_std_core_types__ctail _x20205 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17264, _ctx)),_b_18457_18454); /*ctail<0>*/
  return kk_std_core__ctail_list_3(_x20203, hi, stride, f, _x20205, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift17679_op_fun20208__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t _ctail_17267;
};
static kk_std_core__list kk_std_core__mlift17679_op_fun20208(kk_function_t _fself, kk_std_core__list _ctail_17266, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17679_op_fun20208(kk_function_t _accm, kk_box_t _ctail_17267, kk_context_t* _ctx) {
  struct kk_std_core__mlift17679_op_fun20208__t* _self = kk_function_alloc_as(struct kk_std_core__mlift17679_op_fun20208__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17679_op_fun20208, kk_context());
  _self->_accm = _accm;
  _self->_ctail_17267 = _ctail_17267;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift17679_op_fun20208(kk_function_t _fself, kk_std_core__list _ctail_17266, kk_context_t* _ctx) {
  struct kk_std_core__mlift17679_op_fun20208__t* _self = kk_function_as(struct kk_std_core__mlift17679_op_fun20208__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<5073>) -> list<5073> */
  kk_box_t _ctail_17267 = _self->_ctail_17267; /* 5073 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(_ctail_17267);}, {}, _ctx)
  kk_std_core__list _x20209 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17267, _ctail_17266, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x20209, _ctx));
}

kk_std_core__list kk_std_core__mlift17679_op(kk_function_t _accm, kk_function_t f0, kk_integer_t hi0, kk_integer_t lo0, kk_integer_t stride0, kk_box_t _ctail_17267, kk_context_t* _ctx) { /* forall<a,e> ((list<a>) -> list<a>, f : (int) -> e a, hi : int, lo : int, stride : int, a) -> e list<a> */ 
  kk_integer_t _x20206;
  kk_integer_t _x20207 = kk_integer_dup(stride0); /*int*/
  _x20206 = kk_integer_add(lo0,_x20207,kk_context()); /*int*/
  return kk_std_core__ctailm_list_3(_x20206, hi0, stride0, f0, kk_std_core__new_mlift17679_op_fun20208(_accm, _ctail_17267, _ctx), _ctx);
}
 
// Returns an integer list of increasing elements from `lo`  to `hi` with stride `stride`.
// If `lo > hi`  the function returns the empty list.


// lift anonymous function
struct kk_std_core__ctail_list_fun20213__t_3 {
  struct kk_function_s _base;
  kk_function_t f1;
  kk_integer_t hi1;
  kk_integer_t lo1;
  kk_integer_t stride1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_list_fun20213_3(kk_function_t _fself, kk_box_t _b_18462, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_list_fun20213_3(kk_function_t f1, kk_integer_t hi1, kk_integer_t lo1, kk_integer_t stride1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_list_fun20213__t_3* _self = kk_function_alloc_as(struct kk_std_core__ctail_list_fun20213__t_3, 6, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_list_fun20213_3, kk_context());
  _self->f1 = f1;
  _self->hi1 = hi1;
  _self->lo1 = lo1;
  _self->stride1 = stride1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_list_fun20213_3(kk_function_t _fself, kk_box_t _b_18462, kk_context_t* _ctx) {
  struct kk_std_core__ctail_list_fun20213__t_3* _self = kk_function_as(struct kk_std_core__ctail_list_fun20213__t_3*, _fself);
  kk_function_t f1 = _self->f1; /* (int) -> 5074 5073 */
  kk_integer_t hi1 = _self->hi1; /* int */
  kk_integer_t lo1 = _self->lo1; /* int */
  kk_integer_t stride1 = _self->stride1; /* int */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<5073>> */
  kk_drop_match(_self, {kk_function_dup(f1);kk_integer_dup(hi1);kk_integer_dup(lo1);kk_integer_dup(stride1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_box_t _ctail_18480_172620 = _b_18462; /*5073*/;
  kk_std_core__list _x20214 = kk_std_core__mlift17678_op(_acc0, f1, hi1, lo1, stride1, _ctail_18480_172620, _ctx); /*list<5073>*/
  return kk_std_core__list_box(_x20214, _ctx);
}

kk_std_core__list kk_std_core__ctail_list_3(kk_integer_t lo1, kk_integer_t hi1, kk_integer_t stride1, kk_function_t f1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,e> (lo : int, hi : int, stride : int, f : (int) -> e a, ctail<list<a>>) -> e list<a> */ 
  kk__tailcall: ;
  bool _match_19828 = kk_integer_lte_borrow(lo1,hi1,kk_context()); /*bool*/;
  if (_match_19828) {
    kk_box_t x_17789;
    kk_function_t _x20211 = kk_function_dup(f1); /*(int) -> 5074 5073*/
    kk_integer_t _x20210 = kk_integer_dup(lo1); /*int*/
    x_17789 = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_context_t*), _x20211, (_x20211, _x20210, _ctx)); /*5073*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x_17789, _ctx);
      kk_box_t _x20212 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_list_fun20213_3(f1, hi1, lo1, stride1, _acc0, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20212, _ctx);
    }
    {
      kk_std_core__list _ctail_172630 = kk_std_core__list_hole(); /*list<5073>*/;
      kk_std_core__list _ctail_172640 = kk_std_core__new_Cons(kk_reuse_null, x_17789, _ctail_172630, _ctx); /*list<5073>*/;
      kk_box_t* _b_18474_18468 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_172640)->tail)); /*cfield<list<5073>>*/;
      { // tailcall
        kk_integer_t _x20215;
        kk_integer_t _x20216 = kk_integer_dup(stride1); /*int*/
        _x20215 = kk_integer_add(lo1,_x20216,kk_context()); /*int*/
        kk_std_core_types__ctail _x20217 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_172640, _ctx)),_b_18474_18468); /*ctail<0>*/
        lo1 = _x20215;
        _acc0 = _x20217;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_integer_drop(stride1, _ctx);
    kk_integer_drop(lo1, _ctx);
    kk_integer_drop(hi1, _ctx);
    kk_function_drop(f1, _ctx);
    kk_box_t _x20218 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20218, _ctx);
  }
}
 
// Returns an integer list of increasing elements from `lo`  to `hi` with stride `stride`.
// If `lo > hi`  the function returns the empty list.


// lift anonymous function
struct kk_std_core__ctailm_list_fun20222__t_3 {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t f2;
  kk_integer_t hi2;
  kk_integer_t lo2;
  kk_integer_t stride2;
};
static kk_box_t kk_std_core__ctailm_list_fun20222_3(kk_function_t _fself, kk_box_t _b_18482, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_list_fun20222_3(kk_function_t _accm0, kk_function_t f2, kk_integer_t hi2, kk_integer_t lo2, kk_integer_t stride2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_list_fun20222__t_3* _self = kk_function_alloc_as(struct kk_std_core__ctailm_list_fun20222__t_3, 6, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_list_fun20222_3, kk_context());
  _self->_accm0 = _accm0;
  _self->f2 = f2;
  _self->hi2 = hi2;
  _self->lo2 = lo2;
  _self->stride2 = stride2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_list_fun20222_3(kk_function_t _fself, kk_box_t _b_18482, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_list_fun20222__t_3* _self = kk_function_as(struct kk_std_core__ctailm_list_fun20222__t_3*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<5073>) -> list<5073> */
  kk_function_t f2 = _self->f2; /* (int) -> 5074 5073 */
  kk_integer_t hi2 = _self->hi2; /* int */
  kk_integer_t lo2 = _self->lo2; /* int */
  kk_integer_t stride2 = _self->stride2; /* int */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(f2);kk_integer_dup(hi2);kk_integer_dup(lo2);kk_integer_dup(stride2);}, {}, _ctx)
  kk_box_t _ctail_18484_172670 = _b_18482; /*5073*/;
  kk_std_core__list _x20223 = kk_std_core__mlift17679_op(_accm0, f2, hi2, lo2, stride2, _ctail_18484_172670, _ctx); /*list<5073>*/
  return kk_std_core__list_box(_x20223, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_list_fun20227__t_3 {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t x0_17792;
};
static kk_std_core__list kk_std_core__ctailm_list_fun20227_3(kk_function_t _fself, kk_std_core__list _ctail_172660, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_list_fun20227_3(kk_function_t _accm0, kk_box_t x0_17792, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_list_fun20227__t_3* _self = kk_function_alloc_as(struct kk_std_core__ctailm_list_fun20227__t_3, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_list_fun20227_3, kk_context());
  _self->_accm0 = _accm0;
  _self->x0_17792 = x0_17792;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_list_fun20227_3(kk_function_t _fself, kk_std_core__list _ctail_172660, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_list_fun20227__t_3* _self = kk_function_as(struct kk_std_core__ctailm_list_fun20227__t_3*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<5073>) -> list<5073> */
  kk_box_t x0_17792 = _self->x0_17792; /* 5073 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(x0_17792);}, {}, _ctx)
  kk_std_core__list _x20228 = kk_std_core__new_Cons(kk_reuse_null, x0_17792, _ctail_172660, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x20228, _ctx));
}

kk_std_core__list kk_std_core__ctailm_list_3(kk_integer_t lo2, kk_integer_t hi2, kk_integer_t stride2, kk_function_t f2, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,e> (lo : int, hi : int, stride : int, f : (int) -> e a, (list<a>) -> list<a>) -> e list<a> */ 
  kk__tailcall: ;
  bool _match_19826 = kk_integer_lte_borrow(lo2,hi2,kk_context()); /*bool*/;
  if (_match_19826) {
    kk_box_t x0_17792;
    kk_function_t _x20220 = kk_function_dup(f2); /*(int) -> 5074 5073*/
    kk_integer_t _x20219 = kk_integer_dup(lo2); /*int*/
    x0_17792 = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_context_t*), _x20220, (_x20220, _x20219, _ctx)); /*5073*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x0_17792, _ctx);
      kk_box_t _x20221 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_list_fun20222_3(_accm0, f2, hi2, lo2, stride2, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20221, _ctx);
    }
    { // tailcall
      kk_integer_t _x20224;
      kk_integer_t _x20225 = kk_integer_dup(stride2); /*int*/
      _x20224 = kk_integer_add(lo2,_x20225,kk_context()); /*int*/
      kk_function_t _x20226 = kk_std_core__new_ctailm_list_fun20227_3(_accm0, x0_17792, _ctx); /*(list<5073>) -> list<5073>*/
      lo2 = _x20224;
      _accm0 = _x20226;
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
struct kk_std_core_list_fun20230__t_3 {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core_list_fun20230_3(kk_function_t _fself, kk_std_core__list _ctail_17265, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_list_fun20230_3(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_list_fun20230_3, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core_list_fun20230_3(kk_function_t _fself, kk_std_core__list _ctail_17265, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17265;
}

kk_std_core__list kk_std_core_list_3(kk_integer_t lo3, kk_integer_t hi3, kk_integer_t stride3, kk_function_t f3, kk_context_t* _ctx) { /* forall<a,e> (lo : int, hi : int, stride : int, f : (int) -> e a) -> e list<a> */ 
  bool _match_19825 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_19825) {
    kk_std_core_types__ctail _x20229 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_list_3(lo3, hi3, stride3, f3, _x20229, _ctx);
  }
  {
    return kk_std_core__ctailm_list_3(lo3, hi3, stride3, f3, kk_std_core_new_list_fun20230_3(_ctx), _ctx);
  }
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift17680_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_std_core__list xx, kk_box_t _ctail_17268, kk_context_t* _ctx) { /* forall<a,b,e> (ctail<list<b>>, f : (a) -> e b, xx : list<a>, b) -> e list<b> */ 
  kk_std_core__list _ctail_17269 = kk_std_core__list_hole(); /*list<6195>*/;
  kk_std_core__list _ctail_17270 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17268, _ctail_17269, _ctx); /*list<6195>*/;
  kk_box_t* _b_18493_18490 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17270)->tail)); /*cfield<list<6195>>*/;
  kk_std_core_types__ctail _x20231 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17270, _ctx)),_b_18493_18490); /*ctail<0>*/
  return kk_std_core__ctail_map_5(xx, f, _x20231, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift17681_op_fun20232__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t _ctail_17273;
};
static kk_std_core__list kk_std_core__mlift17681_op_fun20232(kk_function_t _fself, kk_std_core__list _ctail_17272, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17681_op_fun20232(kk_function_t _accm, kk_box_t _ctail_17273, kk_context_t* _ctx) {
  struct kk_std_core__mlift17681_op_fun20232__t* _self = kk_function_alloc_as(struct kk_std_core__mlift17681_op_fun20232__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17681_op_fun20232, kk_context());
  _self->_accm = _accm;
  _self->_ctail_17273 = _ctail_17273;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift17681_op_fun20232(kk_function_t _fself, kk_std_core__list _ctail_17272, kk_context_t* _ctx) {
  struct kk_std_core__mlift17681_op_fun20232__t* _self = kk_function_as(struct kk_std_core__mlift17681_op_fun20232__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<6195>) -> list<6195> */
  kk_box_t _ctail_17273 = _self->_ctail_17273; /* 6195 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(_ctail_17273);}, {}, _ctx)
  kk_std_core__list _x20233 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17273, _ctail_17272, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x20233, _ctx));
}

kk_std_core__list kk_std_core__mlift17681_op(kk_function_t _accm, kk_function_t f0, kk_std_core__list xx0, kk_box_t _ctail_17273, kk_context_t* _ctx) { /* forall<a,b,e> ((list<b>) -> list<b>, f : (a) -> e b, xx : list<a>, b) -> e list<b> */ 
  return kk_std_core__ctailm_map_5(xx0, f0, kk_std_core__new_mlift17681_op_fun20232(_accm, _ctail_17273, _ctx), _ctx);
}
 
// Apply a function `f`  to each element of the input list in sequence.


// lift anonymous function
struct kk_std_core__ctail_map_fun20237__t_5 {
  struct kk_function_s _base;
  kk_function_t f1;
  kk_std_core__list xx1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_map_fun20237_5(kk_function_t _fself, kk_box_t _b_18498, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_map_fun20237_5(kk_function_t f1, kk_std_core__list xx1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_map_fun20237__t_5* _self = kk_function_alloc_as(struct kk_std_core__ctail_map_fun20237__t_5, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_map_fun20237_5, kk_context());
  _self->f1 = f1;
  _self->xx1 = xx1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_map_fun20237_5(kk_function_t _fself, kk_box_t _b_18498, kk_context_t* _ctx) {
  struct kk_std_core__ctail_map_fun20237__t_5* _self = kk_function_as(struct kk_std_core__ctail_map_fun20237__t_5*, _fself);
  kk_function_t f1 = _self->f1; /* (6194) -> 6196 6195 */
  kk_std_core__list xx1 = _self->xx1; /* list<6194> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<6195>> */
  kk_drop_match(_self, {kk_function_dup(f1);kk_std_core__list_dup(xx1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_box_t _ctail_18516_172680 = _b_18498; /*6195*/;
  kk_std_core__list _x20238 = kk_std_core__mlift17680_op(_acc0, f1, xx1, _ctail_18516_172680, _ctx); /*list<6195>*/
  return kk_std_core__list_box(_x20238, _ctx);
}

kk_std_core__list kk_std_core__ctail_map_5(kk_std_core__list xs, kk_function_t f1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, f : (a) -> e b, ctail<list<b>>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con20234 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con20234->head;
    kk_std_core__list xx1 = _con20234->tail;
    kk_reuse_t _ru_19911 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      _ru_19911 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx1);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_box_t x0_17795;
    kk_function_t _x20235 = kk_function_dup(f1); /*(6194) -> 6196 6195*/
    x0_17795 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x20235, (_x20235, x, _ctx)); /*6195*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_19911,kk_context());
      kk_box_drop(x0_17795, _ctx);
      kk_box_t _x20236 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_map_fun20237_5(f1, xx1, _acc0, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20236, _ctx);
    }
    {
      kk_std_core__list _ctail_172690 = kk_std_core__list_hole(); /*list<6195>*/;
      kk_std_core__list _ctail_172700 = kk_std_core__new_Cons(_ru_19911, x0_17795, _ctail_172690, _ctx); /*list<6195>*/;
      kk_box_t* _b_18510_18504 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_172700)->tail)); /*cfield<list<6195>>*/;
      { // tailcall
        kk_std_core_types__ctail _x20239 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_172700, _ctx)),_b_18510_18504); /*ctail<0>*/
        xs = xx1;
        _acc0 = _x20239;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_function_drop(f1, _ctx);
    kk_box_t _x20240 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20240, _ctx);
  }
}
 
// Apply a function `f`  to each element of the input list in sequence.


// lift anonymous function
struct kk_std_core__ctailm_map_fun20244__t_5 {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t f2;
  kk_std_core__list xx2;
};
static kk_box_t kk_std_core__ctailm_map_fun20244_5(kk_function_t _fself, kk_box_t _b_18518, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_map_fun20244_5(kk_function_t _accm0, kk_function_t f2, kk_std_core__list xx2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_map_fun20244__t_5* _self = kk_function_alloc_as(struct kk_std_core__ctailm_map_fun20244__t_5, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_map_fun20244_5, kk_context());
  _self->_accm0 = _accm0;
  _self->f2 = f2;
  _self->xx2 = xx2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_map_fun20244_5(kk_function_t _fself, kk_box_t _b_18518, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_map_fun20244__t_5* _self = kk_function_as(struct kk_std_core__ctailm_map_fun20244__t_5*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<6195>) -> list<6195> */
  kk_function_t f2 = _self->f2; /* (6194) -> 6196 6195 */
  kk_std_core__list xx2 = _self->xx2; /* list<6194> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(f2);kk_std_core__list_dup(xx2);}, {}, _ctx)
  kk_box_t _ctail_18520_172730 = _b_18518; /*6195*/;
  kk_std_core__list _x20245 = kk_std_core__mlift17681_op(_accm0, f2, xx2, _ctail_18520_172730, _ctx); /*list<6195>*/
  return kk_std_core__list_box(_x20245, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_map_fun20247__t_5 {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t x2_17798;
};
static kk_std_core__list kk_std_core__ctailm_map_fun20247_5(kk_function_t _fself, kk_std_core__list _ctail_172720, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_map_fun20247_5(kk_function_t _accm0, kk_box_t x2_17798, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_map_fun20247__t_5* _self = kk_function_alloc_as(struct kk_std_core__ctailm_map_fun20247__t_5, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_map_fun20247_5, kk_context());
  _self->_accm0 = _accm0;
  _self->x2_17798 = x2_17798;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_map_fun20247_5(kk_function_t _fself, kk_std_core__list _ctail_172720, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_map_fun20247__t_5* _self = kk_function_as(struct kk_std_core__ctailm_map_fun20247__t_5*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<6195>) -> list<6195> */
  kk_box_t x2_17798 = _self->x2_17798; /* 6195 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(x2_17798);}, {}, _ctx)
  kk_std_core__list _x20248 = kk_std_core__new_Cons(kk_reuse_null, x2_17798, _ctail_172720, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x20248, _ctx));
}

kk_std_core__list kk_std_core__ctailm_map_5(kk_std_core__list xs0, kk_function_t f2, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, f : (a) -> e b, (list<b>) -> list<b>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con20241 = kk_std_core__as_Cons(xs0);
    kk_box_t x1 = _con20241->head;
    kk_std_core__list xx2 = _con20241->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs0))) {
      kk_std_core__list_free(xs0, _ctx);
    }
    else {
      kk_box_dup(x1);
      kk_std_core__list_dup(xx2);
      kk_std_core__list_decref(xs0, _ctx);
    }
    kk_box_t x2_17798;
    kk_function_t _x20242 = kk_function_dup(f2); /*(6194) -> 6196 6195*/
    x2_17798 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x20242, (_x20242, x1, _ctx)); /*6195*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x2_17798, _ctx);
      kk_box_t _x20243 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_map_fun20244_5(_accm0, f2, xx2, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20243, _ctx);
    }
    { // tailcall
      kk_function_t _x20246 = kk_std_core__new_ctailm_map_fun20247_5(_accm0, x2_17798, _ctx); /*(list<6195>) -> list<6195>*/
      xs0 = xx2;
      _accm0 = _x20246;
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
struct kk_std_core_map_fun20250__t_5 {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core_map_fun20250_5(kk_function_t _fself, kk_std_core__list _ctail_17271, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20250_5(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_map_fun20250_5, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core_map_fun20250_5(kk_function_t _fself, kk_std_core__list _ctail_17271, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17271;
}

kk_std_core__list kk_std_core_map_5(kk_std_core__list xs1, kk_function_t f3, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, f : (a) -> e b) -> e list<b> */ 
  bool _match_19822 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_19822) {
    kk_std_core_types__ctail _x20249 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_map_5(xs1, f3, _x20249, _ctx);
  }
  {
    return kk_std_core__ctailm_map_5(xs1, f3, kk_std_core_new_map_fun20250_5(_ctx), _ctx);
  }
}
 
// Create a list of characters from `lo`  to `hi`  (inclusive).


// lift anonymous function
struct kk_std_core_list_fun20253__t_4 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_list_fun20253_4(kk_function_t _fself, kk_box_t _b_18523, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_list_fun20253_4(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_list_fun20253_4, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_list_fun20253_4(kk_function_t _fself, kk_box_t _b_18523, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_char_t _x20254;
  kk_integer_t _x20255 = kk_integer_unbox(_b_18523); /*int*/
  _x20254 = kk_integer_clamp32(_x20255,kk_context()); /*char*/
  return kk_char_box(_x20254, _ctx);
}

kk_std_core__list kk_std_core_list_4(kk_char_t lo, kk_char_t hi, kk_context_t* _ctx) { /* (lo : char, hi : char) -> total list<char> */ 
  kk_std_core__list _b_18524_18521;
  kk_integer_t _x20251 = kk_integer_from_int(lo,kk_context()); /*int*/
  kk_integer_t _x20252 = kk_integer_from_int(hi,kk_context()); /*int*/
  _b_18524_18521 = kk_std_core_list(_x20251, _x20252, _ctx); /*list<int>*/
  return kk_std_core_map_5(_b_18524_18521, kk_std_core_new_list_fun20253_4(_ctx), _ctx);
}
 
// Convert a string to a list of characters

kk_std_core__list kk_std_core_list_6(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> total list<char> */ 
  return kk_string_to_list(s,kk_context());
}


// lift anonymous function
struct kk_std_core_map_fun20257__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_map_fun20257(kk_function_t _fself, kk_box_t _b_18527, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20257(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_map_fun20257, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_map_fun20257(kk_function_t _fself, kk_box_t _b_18527, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_box_t _y_18529_17398 = _b_18527; /*5375*/;
  kk_std_core_types__maybe _x20258 = kk_std_core_types__new_Just(_y_18529_17398, _ctx); /*maybe<1034>*/
  return kk_std_core_types__maybe_box(_x20258, _ctx);
}

kk_std_core_types__maybe kk_std_core_map(kk_std_core_types__maybe m, kk_function_t f, kk_context_t* _ctx) { /* forall<a,b,e> (m : maybe<a>, f : (a) -> e b) -> e maybe<b> */ 
  if (kk_std_core_types__is_Nothing(m)) {
    kk_function_drop(f, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  {
    kk_box_t x = m._cons.Just.value;
    kk_box_t x0_17801 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, x, _ctx)); /*5375*/;
    if (kk_yielding(kk_context())) {
      kk_box_drop(x0_17801, _ctx);
      kk_box_t _x20256 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun20257(_ctx), _ctx); /*3926*/
      return kk_std_core_types__maybe_unbox(_x20256, _ctx);
    }
    {
      return kk_std_core_types__new_Just(x0_17801, _ctx);
    }
  }
}
 
// Map over the `Right` component of an `:either` type.


// lift anonymous function
struct kk_std_core_map_fun20260__t_1 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_map_fun20260_1(kk_function_t _fself, kk_box_t _b_18531, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20260_1(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_map_fun20260_1, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_map_fun20260_1(kk_function_t _fself, kk_box_t _b_18531, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_box_t _y_18533_17400 = _b_18531; /*5413*/;
  kk_std_core_types__either _x20261 = kk_std_core_types__new_Right(_y_18533_17400, _ctx); /*either<1024,1025>*/
  return kk_std_core_types__either_box(_x20261, _ctx);
}

kk_std_core_types__either kk_std_core_map_1(kk_std_core_types__either e, kk_function_t f, kk_context_t* _ctx) { /* forall<a,b,c,e> (e : either<a,b>, f : (b) -> e c) -> e either<a,c> */ 
  if (kk_std_core_types__is_Right(e)) {
    kk_box_t x = e._cons.Right.right;
    kk_box_t x0_17805 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, x, _ctx)); /*5413*/;
    if (kk_yielding(kk_context())) {
      kk_box_drop(x0_17805, _ctx);
      kk_box_t _x20259 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun20260_1(_ctx), _ctx); /*3926*/
      return kk_std_core_types__either_unbox(_x20259, _ctx);
    }
    {
      return kk_std_core_types__new_Right(x0_17805, _ctx);
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
struct kk_std_core__mlift17685_map_fun20264__t_2 {
  struct kk_function_s _base;
  kk_box_t _y_17402;
};
static kk_box_t kk_std_core__mlift17685_map_fun20264_2(kk_function_t _fself, kk_box_t _b_18535, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17685_map_fun20264_2(kk_box_t _y_17402, kk_context_t* _ctx) {
  struct kk_std_core__mlift17685_map_fun20264__t_2* _self = kk_function_alloc_as(struct kk_std_core__mlift17685_map_fun20264__t_2, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17685_map_fun20264_2, kk_context());
  _self->_y_17402 = _y_17402;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift17685_map_fun20264_2(kk_function_t _fself, kk_box_t _b_18535, kk_context_t* _ctx) {
  struct kk_std_core__mlift17685_map_fun20264__t_2* _self = kk_function_as(struct kk_std_core__mlift17685_map_fun20264__t_2*, _fself);
  kk_box_t _y_17402 = _self->_y_17402; /* 5558 */
  kk_drop_match(_self, {kk_box_dup(_y_17402);}, {}, _ctx)
  kk_box_t _y_18537_17403 = _b_18535; /*5558*/;
  kk_std_core_types__tuple2_ _x20265 = kk_std_core_types__new_dash__lp__comma__rp_(_y_17402, _y_18537_17403, _ctx); /*(1004, 1005)*/
  return kk_std_core_types__tuple2__box(_x20265, _ctx);
}

kk_std_core_types__tuple2_ kk_std_core__mlift17685_map_2(kk_function_t f, kk_std_core_types__tuple2_ t, kk_box_t _y_17402, kk_context_t* _ctx) { /* forall<a,b,e> (f : (a) -> e b, t : (a, a), b) -> e (b, b) */ 
  kk_box_t x_17809;
  kk_box_t _x20262;
  {
    kk_box_t _x = t.snd;
    kk_box_dup(_x);
    kk_std_core_types__tuple2__drop(t, _ctx);
    _x20262 = _x; /*5557*/
  }
  x_17809 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, _x20262, _ctx)); /*5558*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_17809, _ctx);
    kk_box_t _x20263 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift17685_map_fun20264_2(_y_17402, _ctx), _ctx); /*3926*/
    return kk_std_core_types__tuple2__unbox(_x20263, _ctx);
  }
  {
    return kk_std_core_types__new_dash__lp__comma__rp_(_y_17402, x_17809, _ctx);
  }
}


// lift anonymous function
struct kk_std_core_map_fun20269__t_2 {
  struct kk_function_s _base;
  kk_function_t f;
  kk_std_core_types__tuple2_ t;
};
static kk_box_t kk_std_core_map_fun20269_2(kk_function_t _fself, kk_box_t _b_18539, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20269_2(kk_function_t f, kk_std_core_types__tuple2_ t, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20269__t_2* _self = kk_function_alloc_as(struct kk_std_core_map_fun20269__t_2, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun20269_2, kk_context());
  _self->f = f;
  _self->t = t;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun20269_2(kk_function_t _fself, kk_box_t _b_18539, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20269__t_2* _self = kk_function_as(struct kk_std_core_map_fun20269__t_2*, _fself);
  kk_function_t f = _self->f; /* (5557) -> 5559 5558 */
  kk_std_core_types__tuple2_ t = _self->t; /* (5557, 5557) */
  kk_drop_match(_self, {kk_function_dup(f);kk_std_core_types__tuple2__dup(t);}, {}, _ctx)
  kk_box_t _y_18544_17402 = _b_18539; /*5558*/;
  kk_std_core_types__tuple2_ _x20270 = kk_std_core__mlift17685_map_2(f, t, _y_18544_17402, _ctx); /*(5558, 5558)*/
  return kk_std_core_types__tuple2__box(_x20270, _ctx);
}


// lift anonymous function
struct kk_std_core_map_fun20273__t_2 {
  struct kk_function_s _base;
  kk_box_t x_17814;
};
static kk_box_t kk_std_core_map_fun20273_2(kk_function_t _fself, kk_box_t _b_18541, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20273_2(kk_box_t x_17814, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20273__t_2* _self = kk_function_alloc_as(struct kk_std_core_map_fun20273__t_2, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun20273_2, kk_context());
  _self->x_17814 = x_17814;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun20273_2(kk_function_t _fself, kk_box_t _b_18541, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20273__t_2* _self = kk_function_as(struct kk_std_core_map_fun20273__t_2*, _fself);
  kk_box_t x_17814 = _self->x_17814; /* 5558 */
  kk_drop_match(_self, {kk_box_dup(x_17814);}, {}, _ctx)
  kk_box_t _y_18545_17403 = _b_18541; /*5558*/;
  kk_std_core_types__tuple2_ _x20274 = kk_std_core_types__new_dash__lp__comma__rp_(x_17814, _y_18545_17403, _ctx); /*(1004, 1005)*/
  return kk_std_core_types__tuple2__box(_x20274, _ctx);
}

kk_std_core_types__tuple2_ kk_std_core_map_2(kk_std_core_types__tuple2_ t, kk_function_t f, kk_context_t* _ctx) { /* forall<a,b,e> (t : (a, a), f : (a) -> e b) -> e (b, b) */ 
  kk_box_t x_17814;
  kk_function_t _x20267 = kk_function_dup(f); /*(5557) -> 5559 5558*/
  kk_box_t _x20266;
  {
    kk_box_t _x = t.fst;
    kk_box_dup(_x);
    _x20266 = _x; /*5557*/
  }
  x_17814 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x20267, (_x20267, _x20266, _ctx)); /*5558*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_17814, _ctx);
    kk_box_t _x20268 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun20269_2(f, t, _ctx), _ctx); /*3926*/
    return kk_std_core_types__tuple2__unbox(_x20268, _ctx);
  }
  {
    kk_box_t x0_17818;
    kk_box_t _x20271;
    {
      kk_box_t _x0 = t.snd;
      kk_box_dup(_x0);
      kk_std_core_types__tuple2__drop(t, _ctx);
      _x20271 = _x0; /*5557*/
    }
    x0_17818 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, _x20271, _ctx)); /*5558*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x0_17818, _ctx);
      kk_box_t _x20272 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun20273_2(x_17814, _ctx), _ctx); /*3926*/
      return kk_std_core_types__tuple2__unbox(_x20272, _ctx);
    }
    {
      return kk_std_core_types__new_dash__lp__comma__rp_(x_17814, x0_17818, _ctx);
    }
  }
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift17687_map_fun20277__t_3 {
  struct kk_function_s _base;
  kk_box_t _y_17404;
  kk_box_t _y_17405;
};
static kk_box_t kk_std_core__mlift17687_map_fun20277_3(kk_function_t _fself, kk_box_t _b_18547, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17687_map_fun20277_3(kk_box_t _y_17404, kk_box_t _y_17405, kk_context_t* _ctx) {
  struct kk_std_core__mlift17687_map_fun20277__t_3* _self = kk_function_alloc_as(struct kk_std_core__mlift17687_map_fun20277__t_3, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17687_map_fun20277_3, kk_context());
  _self->_y_17404 = _y_17404;
  _self->_y_17405 = _y_17405;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift17687_map_fun20277_3(kk_function_t _fself, kk_box_t _b_18547, kk_context_t* _ctx) {
  struct kk_std_core__mlift17687_map_fun20277__t_3* _self = kk_function_as(struct kk_std_core__mlift17687_map_fun20277__t_3*, _fself);
  kk_box_t _y_17404 = _self->_y_17404; /* 5776 */
  kk_box_t _y_17405 = _self->_y_17405; /* 5776 */
  kk_drop_match(_self, {kk_box_dup(_y_17404);kk_box_dup(_y_17405);}, {}, _ctx)
  kk_box_t _y_18549_17406 = _b_18547; /*5776*/;
  kk_std_core_types__tuple3_ _x20278 = kk_std_core_types__new_dash__lp__comma__comma__rp_(_y_17404, _y_17405, _y_18549_17406, _ctx); /*(1008, 1009, 1010)*/
  return kk_std_core_types__tuple3__box(_x20278, _ctx);
}

kk_std_core_types__tuple3_ kk_std_core__mlift17687_map_3(kk_box_t _y_17404, kk_function_t f, kk_std_core_types__tuple3_ t, kk_box_t _y_17405, kk_context_t* _ctx) { /* forall<a,b,e> (b, f : (a) -> e b, t : (a, a, a), b) -> e (b, b, b) */ 
  kk_box_t x_17824;
  kk_box_t _x20275;
  {
    kk_box_t _x = t.thd;
    kk_box_dup(_x);
    kk_std_core_types__tuple3__drop(t, _ctx);
    _x20275 = _x; /*5775*/
  }
  x_17824 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, _x20275, _ctx)); /*5776*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_17824, _ctx);
    kk_box_t _x20276 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift17687_map_fun20277_3(_y_17404, _y_17405, _ctx), _ctx); /*3926*/
    return kk_std_core_types__tuple3__unbox(_x20276, _ctx);
  }
  {
    return kk_std_core_types__new_dash__lp__comma__comma__rp_(_y_17404, _y_17405, x_17824, _ctx);
  }
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift17688_map_fun20282__t_3 {
  struct kk_function_s _base;
  kk_box_t _y_17404;
  kk_function_t f;
  kk_std_core_types__tuple3_ t;
};
static kk_box_t kk_std_core__mlift17688_map_fun20282_3(kk_function_t _fself, kk_box_t _b_18551, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17688_map_fun20282_3(kk_box_t _y_17404, kk_function_t f, kk_std_core_types__tuple3_ t, kk_context_t* _ctx) {
  struct kk_std_core__mlift17688_map_fun20282__t_3* _self = kk_function_alloc_as(struct kk_std_core__mlift17688_map_fun20282__t_3, 6, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17688_map_fun20282_3, kk_context());
  _self->_y_17404 = _y_17404;
  _self->f = f;
  _self->t = t;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift17688_map_fun20282_3(kk_function_t _fself, kk_box_t _b_18551, kk_context_t* _ctx) {
  struct kk_std_core__mlift17688_map_fun20282__t_3* _self = kk_function_as(struct kk_std_core__mlift17688_map_fun20282__t_3*, _fself);
  kk_box_t _y_17404 = _self->_y_17404; /* 5776 */
  kk_function_t f = _self->f; /* (5775) -> 5777 5776 */
  kk_std_core_types__tuple3_ t = _self->t; /* (5775, 5775, 5775) */
  kk_drop_match(_self, {kk_box_dup(_y_17404);kk_function_dup(f);kk_std_core_types__tuple3__dup(t);}, {}, _ctx)
  kk_box_t _y_18553_17405 = _b_18551; /*5776*/;
  kk_std_core_types__tuple3_ _x20283 = kk_std_core__mlift17687_map_3(_y_17404, f, t, _y_18553_17405, _ctx); /*(5776, 5776, 5776)*/
  return kk_std_core_types__tuple3__box(_x20283, _ctx);
}

kk_std_core_types__tuple3_ kk_std_core__mlift17688_map_3(kk_function_t f, kk_std_core_types__tuple3_ t, kk_box_t _y_17404, kk_context_t* _ctx) { /* forall<a,b,e> (f : (a) -> e b, t : (a, a, a), b) -> e (b, b, b) */ 
  kk_box_t x_17830;
  kk_function_t _x20280 = kk_function_dup(f); /*(5775) -> 5777 5776*/
  kk_box_t _x20279;
  {
    kk_box_t _x = t.snd;
    kk_box_dup(_x);
    _x20279 = _x; /*5775*/
  }
  x_17830 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x20280, (_x20280, _x20279, _ctx)); /*5776*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_17830, _ctx);
    kk_box_t _x20281 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift17688_map_fun20282_3(_y_17404, f, t, _ctx), _ctx); /*3926*/
    return kk_std_core_types__tuple3__unbox(_x20281, _ctx);
  }
  {
    return kk_std_core__mlift17687_map_3(_y_17404, f, t, x_17830, _ctx);
  }
}


// lift anonymous function
struct kk_std_core_map_fun20287__t_3 {
  struct kk_function_s _base;
  kk_function_t f;
  kk_std_core_types__tuple3_ t;
};
static kk_box_t kk_std_core_map_fun20287_3(kk_function_t _fself, kk_box_t _b_18555, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20287_3(kk_function_t f, kk_std_core_types__tuple3_ t, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20287__t_3* _self = kk_function_alloc_as(struct kk_std_core_map_fun20287__t_3, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun20287_3, kk_context());
  _self->f = f;
  _self->t = t;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun20287_3(kk_function_t _fself, kk_box_t _b_18555, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20287__t_3* _self = kk_function_as(struct kk_std_core_map_fun20287__t_3*, _fself);
  kk_function_t f = _self->f; /* (5775) -> 5777 5776 */
  kk_std_core_types__tuple3_ t = _self->t; /* (5775, 5775, 5775) */
  kk_drop_match(_self, {kk_function_dup(f);kk_std_core_types__tuple3__dup(t);}, {}, _ctx)
  kk_box_t _y_18563_17404 = _b_18555; /*5776*/;
  kk_std_core_types__tuple3_ _x20288 = kk_std_core__mlift17688_map_3(f, t, _y_18563_17404, _ctx); /*(5776, 5776, 5776)*/
  return kk_std_core_types__tuple3__box(_x20288, _ctx);
}


// lift anonymous function
struct kk_std_core_map_fun20292__t_3 {
  struct kk_function_s _base;
  kk_function_t f;
  kk_std_core_types__tuple3_ t;
  kk_box_t x_17833;
};
static kk_box_t kk_std_core_map_fun20292_3(kk_function_t _fself, kk_box_t _b_18557, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20292_3(kk_function_t f, kk_std_core_types__tuple3_ t, kk_box_t x_17833, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20292__t_3* _self = kk_function_alloc_as(struct kk_std_core_map_fun20292__t_3, 6, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun20292_3, kk_context());
  _self->f = f;
  _self->t = t;
  _self->x_17833 = x_17833;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun20292_3(kk_function_t _fself, kk_box_t _b_18557, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20292__t_3* _self = kk_function_as(struct kk_std_core_map_fun20292__t_3*, _fself);
  kk_function_t f = _self->f; /* (5775) -> 5777 5776 */
  kk_std_core_types__tuple3_ t = _self->t; /* (5775, 5775, 5775) */
  kk_box_t x_17833 = _self->x_17833; /* 5776 */
  kk_drop_match(_self, {kk_function_dup(f);kk_std_core_types__tuple3__dup(t);kk_box_dup(x_17833);}, {}, _ctx)
  kk_box_t _y_18564_17405 = _b_18557; /*5776*/;
  kk_std_core_types__tuple3_ _x20293 = kk_std_core__mlift17687_map_3(x_17833, f, t, _y_18564_17405, _ctx); /*(5776, 5776, 5776)*/
  return kk_std_core_types__tuple3__box(_x20293, _ctx);
}


// lift anonymous function
struct kk_std_core_map_fun20296__t_3 {
  struct kk_function_s _base;
  kk_box_t x_17833;
  kk_box_t x0_17837;
};
static kk_box_t kk_std_core_map_fun20296_3(kk_function_t _fself, kk_box_t _b_18559, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20296_3(kk_box_t x_17833, kk_box_t x0_17837, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20296__t_3* _self = kk_function_alloc_as(struct kk_std_core_map_fun20296__t_3, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun20296_3, kk_context());
  _self->x_17833 = x_17833;
  _self->x0_17837 = x0_17837;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun20296_3(kk_function_t _fself, kk_box_t _b_18559, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20296__t_3* _self = kk_function_as(struct kk_std_core_map_fun20296__t_3*, _fself);
  kk_box_t x_17833 = _self->x_17833; /* 5776 */
  kk_box_t x0_17837 = _self->x0_17837; /* 5776 */
  kk_drop_match(_self, {kk_box_dup(x_17833);kk_box_dup(x0_17837);}, {}, _ctx)
  kk_box_t _y_18565_17406 = _b_18559; /*5776*/;
  kk_std_core_types__tuple3_ _x20297 = kk_std_core_types__new_dash__lp__comma__comma__rp_(x_17833, x0_17837, _y_18565_17406, _ctx); /*(1008, 1009, 1010)*/
  return kk_std_core_types__tuple3__box(_x20297, _ctx);
}

kk_std_core_types__tuple3_ kk_std_core_map_3(kk_std_core_types__tuple3_ t, kk_function_t f, kk_context_t* _ctx) { /* forall<a,b,e> (t : (a, a, a), f : (a) -> e b) -> e (b, b, b) */ 
  kk_box_t x_17833;
  kk_function_t _x20285 = kk_function_dup(f); /*(5775) -> 5777 5776*/
  kk_box_t _x20284;
  {
    kk_box_t _x = t.fst;
    kk_box_dup(_x);
    _x20284 = _x; /*5775*/
  }
  x_17833 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x20285, (_x20285, _x20284, _ctx)); /*5776*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_17833, _ctx);
    kk_box_t _x20286 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun20287_3(f, t, _ctx), _ctx); /*3926*/
    return kk_std_core_types__tuple3__unbox(_x20286, _ctx);
  }
  {
    kk_box_t x0_17837;
    kk_function_t _x20290 = kk_function_dup(f); /*(5775) -> 5777 5776*/
    kk_box_t _x20289;
    {
      kk_box_t _x0 = t.snd;
      kk_box_dup(_x0);
      _x20289 = _x0; /*5775*/
    }
    x0_17837 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x20290, (_x20290, _x20289, _ctx)); /*5776*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x0_17837, _ctx);
      kk_box_t _x20291 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun20292_3(f, t, x_17833, _ctx), _ctx); /*3926*/
      return kk_std_core_types__tuple3__unbox(_x20291, _ctx);
    }
    {
      kk_box_t x1_17841;
      kk_box_t _x20294;
      {
        kk_box_t _x1 = t.thd;
        kk_box_dup(_x1);
        kk_std_core_types__tuple3__drop(t, _ctx);
        _x20294 = _x1; /*5775*/
      }
      x1_17841 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, _x20294, _ctx)); /*5776*/
      if (kk_yielding(kk_context())) {
        kk_box_drop(x1_17841, _ctx);
        kk_box_t _x20295 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun20296_3(x_17833, x0_17837, _ctx), _ctx); /*3926*/
        return kk_std_core_types__tuple3__unbox(_x20295, _ctx);
      }
      {
        return kk_std_core_types__new_dash__lp__comma__comma__rp_(x_17833, x0_17837, x1_17841, _ctx);
      }
    }
  }
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift17690_map_fun20301__t_4 {
  struct kk_function_s _base;
  kk_box_t _y_17407;
  kk_box_t _y_17408;
  kk_box_t _y_17409;
};
static kk_box_t kk_std_core__mlift17690_map_fun20301_4(kk_function_t _fself, kk_box_t _b_18567, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17690_map_fun20301_4(kk_box_t _y_17407, kk_box_t _y_17408, kk_box_t _y_17409, kk_context_t* _ctx) {
  struct kk_std_core__mlift17690_map_fun20301__t_4* _self = kk_function_alloc_as(struct kk_std_core__mlift17690_map_fun20301__t_4, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17690_map_fun20301_4, kk_context());
  _self->_y_17407 = _y_17407;
  _self->_y_17408 = _y_17408;
  _self->_y_17409 = _y_17409;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift17690_map_fun20301_4(kk_function_t _fself, kk_box_t _b_18567, kk_context_t* _ctx) {
  struct kk_std_core__mlift17690_map_fun20301__t_4* _self = kk_function_as(struct kk_std_core__mlift17690_map_fun20301__t_4*, _fself);
  kk_box_t _y_17407 = _self->_y_17407; /* 6063 */
  kk_box_t _y_17408 = _self->_y_17408; /* 6063 */
  kk_box_t _y_17409 = _self->_y_17409; /* 6063 */
  kk_drop_match(_self, {kk_box_dup(_y_17407);kk_box_dup(_y_17408);kk_box_dup(_y_17409);}, {}, _ctx)
  kk_box_t _y_18569_17410 = _b_18567; /*6063*/;
  kk_std_core_types__tuple4_ _x20302 = kk_std_core_types__new_dash__lp__comma__comma__comma__rp_(kk_reuse_null, _y_17407, _y_17408, _y_17409, _y_18569_17410, _ctx); /*(1013, 1014, 1015, 1016)*/
  return kk_std_core_types__tuple4__box(_x20302, _ctx);
}

kk_std_core_types__tuple4_ kk_std_core__mlift17690_map_4(kk_box_t _y_17407, kk_box_t _y_17408, kk_function_t f, kk_std_core_types__tuple4_ t, kk_box_t _y_17409, kk_context_t* _ctx) { /* forall<a,b,e> (b, b, f : (a) -> e b, t : (a, a, a, a), b) -> e (b, b, b, b) */ 
  kk_box_t x_17848;
  kk_box_t _x20298;
  {
    struct kk_std_core_types__Tuple4_* _con20299 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(t);
    kk_box_t _pat00 = _con20299->fst;
    kk_box_t _pat10 = _con20299->snd;
    kk_box_t _pat2 = _con20299->thd;
    kk_box_t _x = _con20299->field4;
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
    _x20298 = _x; /*6062*/
  }
  x_17848 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, _x20298, _ctx)); /*6063*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_17848, _ctx);
    kk_box_t _x20300 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift17690_map_fun20301_4(_y_17407, _y_17408, _y_17409, _ctx), _ctx); /*3926*/
    return kk_std_core_types__tuple4__unbox(_x20300, _ctx);
  }
  {
    return kk_std_core_types__new_dash__lp__comma__comma__comma__rp_(kk_reuse_null, _y_17407, _y_17408, _y_17409, x_17848, _ctx);
  }
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift17691_map_fun20307__t_4 {
  struct kk_function_s _base;
  kk_box_t _y_17407;
  kk_box_t _y_17408;
  kk_function_t f;
  kk_std_core_types__tuple4_ t;
};
static kk_box_t kk_std_core__mlift17691_map_fun20307_4(kk_function_t _fself, kk_box_t _b_18571, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17691_map_fun20307_4(kk_box_t _y_17407, kk_box_t _y_17408, kk_function_t f, kk_std_core_types__tuple4_ t, kk_context_t* _ctx) {
  struct kk_std_core__mlift17691_map_fun20307__t_4* _self = kk_function_alloc_as(struct kk_std_core__mlift17691_map_fun20307__t_4, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17691_map_fun20307_4, kk_context());
  _self->_y_17407 = _y_17407;
  _self->_y_17408 = _y_17408;
  _self->f = f;
  _self->t = t;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift17691_map_fun20307_4(kk_function_t _fself, kk_box_t _b_18571, kk_context_t* _ctx) {
  struct kk_std_core__mlift17691_map_fun20307__t_4* _self = kk_function_as(struct kk_std_core__mlift17691_map_fun20307__t_4*, _fself);
  kk_box_t _y_17407 = _self->_y_17407; /* 6063 */
  kk_box_t _y_17408 = _self->_y_17408; /* 6063 */
  kk_function_t f = _self->f; /* (6062) -> 6064 6063 */
  kk_std_core_types__tuple4_ t = _self->t; /* (6062, 6062, 6062, 6062) */
  kk_drop_match(_self, {kk_box_dup(_y_17407);kk_box_dup(_y_17408);kk_function_dup(f);kk_std_core_types__tuple4__dup(t);}, {}, _ctx)
  kk_box_t _y_18573_17409 = _b_18571; /*6063*/;
  kk_std_core_types__tuple4_ _x20308 = kk_std_core__mlift17690_map_4(_y_17407, _y_17408, f, t, _y_18573_17409, _ctx); /*(6063, 6063, 6063, 6063)*/
  return kk_std_core_types__tuple4__box(_x20308, _ctx);
}

kk_std_core_types__tuple4_ kk_std_core__mlift17691_map_4(kk_box_t _y_17407, kk_function_t f, kk_std_core_types__tuple4_ t, kk_box_t _y_17408, kk_context_t* _ctx) { /* forall<a,b,e> (b, f : (a) -> e b, t : (a, a, a, a), b) -> e (b, b, b, b) */ 
  kk_box_t x_17855;
  kk_function_t _x20305 = kk_function_dup(f); /*(6062) -> 6064 6063*/
  kk_box_t _x20303;
  {
    struct kk_std_core_types__Tuple4_* _con20304 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(t);
    kk_box_t _x = _con20304->thd;
    kk_box_dup(_x);
    _x20303 = _x; /*6062*/
  }
  x_17855 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x20305, (_x20305, _x20303, _ctx)); /*6063*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_17855, _ctx);
    kk_box_t _x20306 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift17691_map_fun20307_4(_y_17407, _y_17408, f, t, _ctx), _ctx); /*3926*/
    return kk_std_core_types__tuple4__unbox(_x20306, _ctx);
  }
  {
    return kk_std_core__mlift17690_map_4(_y_17407, _y_17408, f, t, x_17855, _ctx);
  }
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift17692_map_fun20313__t_4 {
  struct kk_function_s _base;
  kk_box_t _y_17407;
  kk_function_t f;
  kk_std_core_types__tuple4_ t;
};
static kk_box_t kk_std_core__mlift17692_map_fun20313_4(kk_function_t _fself, kk_box_t _b_18575, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17692_map_fun20313_4(kk_box_t _y_17407, kk_function_t f, kk_std_core_types__tuple4_ t, kk_context_t* _ctx) {
  struct kk_std_core__mlift17692_map_fun20313__t_4* _self = kk_function_alloc_as(struct kk_std_core__mlift17692_map_fun20313__t_4, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17692_map_fun20313_4, kk_context());
  _self->_y_17407 = _y_17407;
  _self->f = f;
  _self->t = t;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift17692_map_fun20313_4(kk_function_t _fself, kk_box_t _b_18575, kk_context_t* _ctx) {
  struct kk_std_core__mlift17692_map_fun20313__t_4* _self = kk_function_as(struct kk_std_core__mlift17692_map_fun20313__t_4*, _fself);
  kk_box_t _y_17407 = _self->_y_17407; /* 6063 */
  kk_function_t f = _self->f; /* (6062) -> 6064 6063 */
  kk_std_core_types__tuple4_ t = _self->t; /* (6062, 6062, 6062, 6062) */
  kk_drop_match(_self, {kk_box_dup(_y_17407);kk_function_dup(f);kk_std_core_types__tuple4__dup(t);}, {}, _ctx)
  kk_box_t _y_18577_17408 = _b_18575; /*6063*/;
  kk_std_core_types__tuple4_ _x20314 = kk_std_core__mlift17691_map_4(_y_17407, f, t, _y_18577_17408, _ctx); /*(6063, 6063, 6063, 6063)*/
  return kk_std_core_types__tuple4__box(_x20314, _ctx);
}

kk_std_core_types__tuple4_ kk_std_core__mlift17692_map_4(kk_function_t f, kk_std_core_types__tuple4_ t, kk_box_t _y_17407, kk_context_t* _ctx) { /* forall<a,b,e> (f : (a) -> e b, t : (a, a, a, a), b) -> e (b, b, b, b) */ 
  kk_box_t x_17858;
  kk_function_t _x20311 = kk_function_dup(f); /*(6062) -> 6064 6063*/
  kk_box_t _x20309;
  {
    struct kk_std_core_types__Tuple4_* _con20310 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(t);
    kk_box_t _x = _con20310->snd;
    kk_box_dup(_x);
    _x20309 = _x; /*6062*/
  }
  x_17858 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x20311, (_x20311, _x20309, _ctx)); /*6063*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_17858, _ctx);
    kk_box_t _x20312 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift17692_map_fun20313_4(_y_17407, f, t, _ctx), _ctx); /*3926*/
    return kk_std_core_types__tuple4__unbox(_x20312, _ctx);
  }
  {
    return kk_std_core__mlift17691_map_4(_y_17407, f, t, x_17858, _ctx);
  }
}


// lift anonymous function
struct kk_std_core_map_fun20319__t_4 {
  struct kk_function_s _base;
  kk_function_t f;
  kk_std_core_types__tuple4_ t;
};
static kk_box_t kk_std_core_map_fun20319_4(kk_function_t _fself, kk_box_t _b_18579, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20319_4(kk_function_t f, kk_std_core_types__tuple4_ t, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20319__t_4* _self = kk_function_alloc_as(struct kk_std_core_map_fun20319__t_4, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun20319_4, kk_context());
  _self->f = f;
  _self->t = t;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun20319_4(kk_function_t _fself, kk_box_t _b_18579, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20319__t_4* _self = kk_function_as(struct kk_std_core_map_fun20319__t_4*, _fself);
  kk_function_t f = _self->f; /* (6062) -> 6064 6063 */
  kk_std_core_types__tuple4_ t = _self->t; /* (6062, 6062, 6062, 6062) */
  kk_drop_match(_self, {kk_function_dup(f);kk_std_core_types__tuple4__dup(t);}, {}, _ctx)
  kk_box_t _y_18590_17407 = _b_18579; /*6063*/;
  kk_std_core_types__tuple4_ _x20320 = kk_std_core__mlift17692_map_4(f, t, _y_18590_17407, _ctx); /*(6063, 6063, 6063, 6063)*/
  return kk_std_core_types__tuple4__box(_x20320, _ctx);
}


// lift anonymous function
struct kk_std_core_map_fun20325__t_4 {
  struct kk_function_s _base;
  kk_function_t f;
  kk_std_core_types__tuple4_ t;
  kk_box_t x_17861;
};
static kk_box_t kk_std_core_map_fun20325_4(kk_function_t _fself, kk_box_t _b_18581, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20325_4(kk_function_t f, kk_std_core_types__tuple4_ t, kk_box_t x_17861, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20325__t_4* _self = kk_function_alloc_as(struct kk_std_core_map_fun20325__t_4, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun20325_4, kk_context());
  _self->f = f;
  _self->t = t;
  _self->x_17861 = x_17861;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun20325_4(kk_function_t _fself, kk_box_t _b_18581, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20325__t_4* _self = kk_function_as(struct kk_std_core_map_fun20325__t_4*, _fself);
  kk_function_t f = _self->f; /* (6062) -> 6064 6063 */
  kk_std_core_types__tuple4_ t = _self->t; /* (6062, 6062, 6062, 6062) */
  kk_box_t x_17861 = _self->x_17861; /* 6063 */
  kk_drop_match(_self, {kk_function_dup(f);kk_std_core_types__tuple4__dup(t);kk_box_dup(x_17861);}, {}, _ctx)
  kk_box_t _y_18591_17408 = _b_18581; /*6063*/;
  kk_std_core_types__tuple4_ _x20326 = kk_std_core__mlift17691_map_4(x_17861, f, t, _y_18591_17408, _ctx); /*(6063, 6063, 6063, 6063)*/
  return kk_std_core_types__tuple4__box(_x20326, _ctx);
}


// lift anonymous function
struct kk_std_core_map_fun20331__t_4 {
  struct kk_function_s _base;
  kk_function_t f;
  kk_std_core_types__tuple4_ t;
  kk_box_t x_17861;
  kk_box_t x0_17865;
};
static kk_box_t kk_std_core_map_fun20331_4(kk_function_t _fself, kk_box_t _b_18583, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20331_4(kk_function_t f, kk_std_core_types__tuple4_ t, kk_box_t x_17861, kk_box_t x0_17865, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20331__t_4* _self = kk_function_alloc_as(struct kk_std_core_map_fun20331__t_4, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun20331_4, kk_context());
  _self->f = f;
  _self->t = t;
  _self->x_17861 = x_17861;
  _self->x0_17865 = x0_17865;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun20331_4(kk_function_t _fself, kk_box_t _b_18583, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20331__t_4* _self = kk_function_as(struct kk_std_core_map_fun20331__t_4*, _fself);
  kk_function_t f = _self->f; /* (6062) -> 6064 6063 */
  kk_std_core_types__tuple4_ t = _self->t; /* (6062, 6062, 6062, 6062) */
  kk_box_t x_17861 = _self->x_17861; /* 6063 */
  kk_box_t x0_17865 = _self->x0_17865; /* 6063 */
  kk_drop_match(_self, {kk_function_dup(f);kk_std_core_types__tuple4__dup(t);kk_box_dup(x_17861);kk_box_dup(x0_17865);}, {}, _ctx)
  kk_box_t _y_18592_17409 = _b_18583; /*6063*/;
  kk_std_core_types__tuple4_ _x20332 = kk_std_core__mlift17690_map_4(x_17861, x0_17865, f, t, _y_18592_17409, _ctx); /*(6063, 6063, 6063, 6063)*/
  return kk_std_core_types__tuple4__box(_x20332, _ctx);
}


// lift anonymous function
struct kk_std_core_map_fun20336__t_4 {
  struct kk_function_s _base;
  kk_box_t x_17861;
  kk_box_t x0_17865;
  kk_box_t x1_17869;
};
static kk_box_t kk_std_core_map_fun20336_4(kk_function_t _fself, kk_box_t _b_18585, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20336_4(kk_box_t x_17861, kk_box_t x0_17865, kk_box_t x1_17869, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20336__t_4* _self = kk_function_alloc_as(struct kk_std_core_map_fun20336__t_4, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun20336_4, kk_context());
  _self->x_17861 = x_17861;
  _self->x0_17865 = x0_17865;
  _self->x1_17869 = x1_17869;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun20336_4(kk_function_t _fself, kk_box_t _b_18585, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20336__t_4* _self = kk_function_as(struct kk_std_core_map_fun20336__t_4*, _fself);
  kk_box_t x_17861 = _self->x_17861; /* 6063 */
  kk_box_t x0_17865 = _self->x0_17865; /* 6063 */
  kk_box_t x1_17869 = _self->x1_17869; /* 6063 */
  kk_drop_match(_self, {kk_box_dup(x_17861);kk_box_dup(x0_17865);kk_box_dup(x1_17869);}, {}, _ctx)
  kk_box_t _y_18593_17410 = _b_18585; /*6063*/;
  kk_std_core_types__tuple4_ _x20337 = kk_std_core_types__new_dash__lp__comma__comma__comma__rp_(kk_reuse_null, x_17861, x0_17865, x1_17869, _y_18593_17410, _ctx); /*(1013, 1014, 1015, 1016)*/
  return kk_std_core_types__tuple4__box(_x20337, _ctx);
}

kk_std_core_types__tuple4_ kk_std_core_map_4(kk_std_core_types__tuple4_ t, kk_function_t f, kk_context_t* _ctx) { /* forall<a,b,e> (t : (a, a, a, a), f : (a) -> e b) -> e (b, b, b, b) */ 
  kk_box_t x_17861;
  kk_function_t _x20317 = kk_function_dup(f); /*(6062) -> 6064 6063*/
  kk_box_t _x20315;
  {
    struct kk_std_core_types__Tuple4_* _con20316 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(t);
    kk_box_t _x = _con20316->fst;
    kk_box_dup(_x);
    _x20315 = _x; /*6062*/
  }
  x_17861 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x20317, (_x20317, _x20315, _ctx)); /*6063*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_17861, _ctx);
    kk_box_t _x20318 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun20319_4(f, t, _ctx), _ctx); /*3926*/
    return kk_std_core_types__tuple4__unbox(_x20318, _ctx);
  }
  {
    kk_box_t x0_17865;
    kk_function_t _x20323 = kk_function_dup(f); /*(6062) -> 6064 6063*/
    kk_box_t _x20321;
    {
      struct kk_std_core_types__Tuple4_* _con20322 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(t);
      kk_box_t _x0 = _con20322->snd;
      kk_box_dup(_x0);
      _x20321 = _x0; /*6062*/
    }
    x0_17865 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x20323, (_x20323, _x20321, _ctx)); /*6063*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x0_17865, _ctx);
      kk_box_t _x20324 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun20325_4(f, t, x_17861, _ctx), _ctx); /*3926*/
      return kk_std_core_types__tuple4__unbox(_x20324, _ctx);
    }
    {
      kk_box_t x1_17869;
      kk_function_t _x20329 = kk_function_dup(f); /*(6062) -> 6064 6063*/
      kk_box_t _x20327;
      {
        struct kk_std_core_types__Tuple4_* _con20328 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(t);
        kk_box_t _x1 = _con20328->thd;
        kk_box_dup(_x1);
        _x20327 = _x1; /*6062*/
      }
      x1_17869 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x20329, (_x20329, _x20327, _ctx)); /*6063*/
      if (kk_yielding(kk_context())) {
        kk_box_drop(x1_17869, _ctx);
        kk_box_t _x20330 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun20331_4(f, t, x_17861, x0_17865, _ctx), _ctx); /*3926*/
        return kk_std_core_types__tuple4__unbox(_x20330, _ctx);
      }
      {
        kk_box_t x2_17873;
        kk_box_t _x20333;
        {
          struct kk_std_core_types__Tuple4_* _con20334 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(t);
          kk_box_t _pat06 = _con20334->fst;
          kk_box_t _pat13 = _con20334->snd;
          kk_box_t _pat22 = _con20334->thd;
          kk_box_t _x2 = _con20334->field4;
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
          _x20333 = _x2; /*6062*/
        }
        x2_17873 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, _x20333, _ctx)); /*6063*/
        if (kk_yielding(kk_context())) {
          kk_box_drop(x2_17873, _ctx);
          kk_box_t _x20335 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun20336_4(x_17861, x0_17865, x1_17869, _ctx), _ctx); /*3926*/
          return kk_std_core_types__tuple4__unbox(_x20335, _ctx);
        }
        {
          return kk_std_core_types__new_dash__lp__comma__comma__comma__rp_(kk_reuse_null, x_17861, x0_17865, x1_17869, x2_17873, _ctx);
        }
      }
    }
  }
}
 
// Apply a function `f` to each character in a string


// lift anonymous function
struct kk_std_core_map_fun20338__t_6 {
  struct kk_function_s _base;
  kk_function_t f;
};
static kk_box_t kk_std_core_map_fun20338_6(kk_function_t _fself, kk_box_t _b_18596, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20338_6(kk_function_t f, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20338__t_6* _self = kk_function_alloc_as(struct kk_std_core_map_fun20338__t_6, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun20338_6, kk_context());
  _self->f = f;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun20338_6(kk_function_t _fself, kk_box_t _b_18596, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20338__t_6* _self = kk_function_as(struct kk_std_core_map_fun20338__t_6*, _fself);
  kk_function_t f = _self->f; /* (char) -> 6531 char */
  kk_drop_match(_self, {kk_function_dup(f);}, {}, _ctx)
  kk_char_t _x20339;
  kk_char_t _x20340 = kk_char_unbox(_b_18596, _ctx); /*char*/
  _x20339 = kk_function_call(kk_char_t, (kk_function_t, kk_char_t, kk_context_t*), f, (f, _x20340, _ctx)); /*char*/
  return kk_char_box(_x20339, _ctx);
}


// lift anonymous function
struct kk_std_core_map_fun20342__t_6 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_map_fun20342_6(kk_function_t _fself, kk_box_t _b_18600, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20342_6(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_map_fun20342_6, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_map_fun20342_6(kk_function_t _fself, kk_box_t _b_18600, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_string_t _x20343;
  kk_std_core__list _x20344 = kk_std_core__list_unbox(_b_18600, _ctx); /*list<char>*/
  _x20343 = kk_std_core_string_2(_x20344, _ctx); /*string*/
  return kk_string_box(_x20343);
}

kk_string_t kk_std_core_map_6(kk_string_t s, kk_function_t f, kk_context_t* _ctx) { /* forall<e> (s : string, f : (char) -> e char) -> e string */ 
  kk_std_core__list _b_18597_18594 = kk_std_core_list_6(s, _ctx); /*list<char>*/;
  kk_std_core__list x_17881 = kk_std_core_map_5(_b_18597_18594, kk_std_core_new_map_fun20338_6(f, _ctx), _ctx); /*list<char>*/;
  if (kk_yielding(kk_context())) {
    kk_std_core__list_drop(x_17881, _ctx);
    kk_box_t _x20341 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun20342_6(_ctx), _ctx); /*3926*/
    return kk_string_unbox(_x20341);
  }
  {
    return kk_std_core_string_2(x_17881, _ctx);
  }
}
 
// Apply a total function `f` to each element in a vector `v`


// lift anonymous function
struct kk_std_core_map_fun20350__t_7 {
  struct kk_function_s _base;
  kk_function_t f;
  kk_vector_t v;
  kk_vector_t w;
};
static kk_unit_t kk_std_core_map_fun20350_7(kk_function_t _fself, kk_ssize_t i, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20350_7(kk_function_t f, kk_vector_t v, kk_vector_t w, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20350__t_7* _self = kk_function_alloc_as(struct kk_std_core_map_fun20350__t_7, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun20350_7, kk_context());
  _self->f = f;
  _self->v = v;
  _self->w = w;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_map_fun20352__t_7 {
  struct kk_function_s _base;
  kk_vector_t w;
  kk_ssize_t i;
};
static kk_box_t kk_std_core_map_fun20352_7(kk_function_t _fself, kk_box_t _b_18603, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20352_7(kk_vector_t w, kk_ssize_t i, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20352__t_7* _self = kk_function_alloc_as(struct kk_std_core_map_fun20352__t_7, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun20352_7, kk_context());
  _self->w = w;
  _self->i = i;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun20352_7(kk_function_t _fself, kk_box_t _b_18603, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20352__t_7* _self = kk_function_as(struct kk_std_core_map_fun20352__t_7*, _fself);
  kk_vector_t w = _self->w; /* vector<6580> */
  kk_ssize_t i = _self->i; /* ssize_t */
  kk_drop_match(_self, {kk_vector_dup(w);;}, {}, _ctx)
  kk_unit_t _x20353 = kk_Unit;
  kk_vector_unsafe_assign(w,i,_b_18603,kk_context());
  return kk_unit_box(_x20353);
}
static kk_unit_t kk_std_core_map_fun20350_7(kk_function_t _fself, kk_ssize_t i, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20350__t_7* _self = kk_function_as(struct kk_std_core_map_fun20350__t_7*, _fself);
  kk_function_t f = _self->f; /* (6579) -> 6581 6580 */
  kk_vector_t v = _self->v; /* vector<6579> */
  kk_vector_t w = _self->w; /* vector<6580> */
  kk_drop_match(_self, {kk_function_dup(f);kk_vector_dup(v);kk_vector_dup(w);}, {}, _ctx)
  kk_box_t x0_18115;
  kk_box_t _brw_19803 = kk_vector_at_borrow(v,i); /*228*/;
  kk_vector_drop(v, _ctx);
  x0_18115 = _brw_19803; /*6579*/
  kk_box_t x1_17889 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, x0_18115, _ctx)); /*6580*/;
  if (kk_yielding(kk_context())) {
    kk_box_drop(x1_17889, _ctx);
    kk_box_t _x20351 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun20352_7(w, i, _ctx), _ctx); /*3926*/
    return kk_unit_unbox(_x20351);
  }
  {
    return kk_vector_unsafe_assign(w,i,x1_17889,kk_context());
  }
}


// lift anonymous function
struct kk_std_core_map_fun20355__t_7 {
  struct kk_function_s _base;
  kk_vector_t w;
};
static kk_box_t kk_std_core_map_fun20355_7(kk_function_t _fself, kk_box_t _b_18607, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun20355_7(kk_vector_t w, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20355__t_7* _self = kk_function_alloc_as(struct kk_std_core_map_fun20355__t_7, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun20355_7, kk_context());
  _self->w = w;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun20355_7(kk_function_t _fself, kk_box_t _b_18607, kk_context_t* _ctx) {
  struct kk_std_core_map_fun20355__t_7* _self = kk_function_as(struct kk_std_core_map_fun20355__t_7*, _fself);
  kk_vector_t w = _self->w; /* vector<6580> */
  kk_drop_match(_self, {kk_vector_dup(w);}, {}, _ctx)
  kk_unit_t wild___18609 = kk_Unit;
  kk_unit_unbox(_b_18607);
  return kk_vector_box(w, _ctx);
}

kk_vector_t kk_std_core_map_7(kk_vector_t v, kk_function_t f, kk_context_t* _ctx) { /* forall<a,b,e> (v : vector<a>, f : (a) -> e b) -> e vector<b> */ 
  kk_vector_t w;
  kk_ssize_t _x20345;
  kk_integer_t _x20346;
  kk_ssize_t _x20347 = kk_vector_len_borrow(v); /*ssize_t*/
  _x20346 = kk_integer_from_ssize_t(_x20347,kk_context()); /*int*/
  _x20345 = kk_std_core_ssize__t(_x20346, _ctx); /*ssize_t*/
  w = kk_vector_alloc_uninit(_x20345,NULL,kk_context()); /*vector<6580>*/
  kk_ssize_t start0_17894 = (KK_IZ(0)); /*ssize_t*/;
  kk_ssize_t end_17895;
  kk_ssize_t _x20348 = kk_vector_len_borrow(v); /*ssize_t*/
  end_17895 = kk_std_core_decr_1(_x20348, _ctx); /*ssize_t*/
  kk_unit_t x_17884 = kk_Unit;
  kk_function_t _x20349;
  kk_vector_dup(w);
  _x20349 = kk_std_core_new_map_fun20350_7(f, v, w, _ctx); /*(i : ssize_t) -> 6581 ()*/
  kk_std_core__lift17190_forz(_x20349, end_17895, start0_17894, _ctx);
  if (kk_yielding(kk_context())) {
    kk_box_t _x20354 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun20355_7(w, _ctx), _ctx); /*3926*/
    return kk_vector_unbox(_x20354, _ctx);
  }
  {
    return w;
  }
}
 
// Right-align a string to width `width`  using `fill`  (default is a space) to fill from the left.

kk_string_t kk_std_core_pad_left(kk_string_t s, kk_integer_t width, kk_std_core_types__optional fill, kk_context_t* _ctx) { /* (s : string, width : int, fill : optional<char>) -> string */ 
  kk_ssize_t w;
  kk_integer_t _x20356 = kk_integer_dup(width); /*int*/
  w = kk_std_core_ssize__t(_x20356, _ctx); /*ssize_t*/
  kk_ssize_t n;
  kk_string_t _x20357 = kk_string_dup(s); /*string*/
  n = kk_string_len(_x20357,kk_context()); /*ssize_t*/
  bool _match_19800 = (w <= n); /*bool*/;
  if (_match_19800) {
    kk_std_core_types__optional_drop(fill, _ctx);
    return s;
  }
  {
    kk_string_t _x20358;
    kk_string_t _x20359;
    kk_char_t _x20360;
    if (kk_std_core_types__is_Optional(fill)) {
      kk_box_t _box_x18610 = fill._cons.Optional.value;
      kk_char_t _fill_7329 = kk_char_unbox(_box_x18610, NULL);
      _x20360 = _fill_7329; /*char*/
    }
    else {
      _x20360 = ' '; /*char*/
    }
    _x20359 = kk_std_core_string(_x20360, _ctx); /*string*/
    kk_ssize_t _x20362 = (w - n); /*ssize_t*/
    _x20358 = kk_std_core_repeatz(_x20359, _x20362, _ctx); /*string*/
    return kk_std_core__lp__plus__plus__1_rp_(_x20358, s, _ctx);
  }
}
 
// Show an `:int` as a hexadecimal value.
// The `width`  parameter specifies how wide the hex value is where `"0"`  is used to align.
// The `use-capitals` parameter (= `True`) determines if captical letters should be used to display the hexadecimal digits.
// The `pre` (=`"0x"`) is an optional prefix for the number (goes between the sign and the number).

kk_string_t kk_std_core_show_hex(kk_integer_t i, kk_std_core_types__optional width, kk_std_core_types__optional use_capitals, kk_std_core_types__optional pre, kk_context_t* _ctx) { /* (i : int, width : optional<int>, use-capitals : optional<bool>, pre : optional<string>) -> string */ 
  kk_string_t _x20363;
  bool _match_19799 = kk_integer_lt_borrow(i,(kk_integer_from_small(0)),kk_context()); /*bool*/;
  if (_match_19799) {
    kk_define_string_literal(, _s20364, 1, "-")
    _x20363 = kk_string_dup(_s20364); /*string*/
  }
  else {
    _x20363 = kk_string_empty(); /*string*/
  }
  kk_string_t _x20366;
  kk_char_t _b_18615_18614 = '0'; /*char*/;
  kk_string_t _x20367;
  if (kk_std_core_types__is_Optional(pre)) {
    kk_box_t _box_x18611 = pre._cons.Optional.value;
    kk_string_t _pre_7437 = kk_string_unbox(_box_x18611);
    _x20367 = _pre_7437; /*string*/
  }
  else {
    kk_define_string_literal(, _s20369, 2, "0x")
    _x20367 = kk_string_dup(_s20369); /*string*/
  }
  kk_string_t _x20370;
  kk_string_t _own_19797;
  kk_integer_t _x20371 = kk_integer_abs(i,kk_context()); /*int*/
  bool _x20372;
  if (kk_std_core_types__is_Optional(use_capitals)) {
    kk_box_t _box_x18612 = use_capitals._cons.Optional.value;
    bool _use_capitals_7433 = kk_bool_unbox(_box_x18612);
    _x20372 = _use_capitals_7433; /*bool*/
  }
  else {
    _x20372 = true; /*bool*/
  }
  _own_19797 = kk_std_core_int_show_hex(_x20371, _x20372, _ctx); /*string*/
  kk_integer_t _brw_19796;
  if (kk_std_core_types__is_Optional(width)) {
    kk_box_t _box_x18613 = width._cons.Optional.value;
    kk_integer_t _width_7429 = kk_integer_unbox(_box_x18613);
    _brw_19796 = _width_7429; /*int*/
  }
  else {
    _brw_19796 = kk_integer_from_small(1); /*int*/
  }
  kk_string_t _brw_19798;
  kk_std_core_types__optional _x20375 = kk_std_core_types__new_Optional(kk_char_box(_b_18615_18614, _ctx), _ctx); /*optional<1035>*/
  _brw_19798 = kk_std_core_pad_left(_own_19797, _brw_19796, _x20375, _ctx); /*string*/
  kk_integer_drop(_brw_19796, _ctx);
  _x20370 = _brw_19798; /*string*/
  _x20366 = kk_std_core__lp__plus__plus__1_rp_(_x20367, _x20370, _ctx); /*string*/
  return kk_std_core__lp__plus__plus__1_rp_(_x20363, _x20366, _ctx);
}
 
// Is the integer positive (stricly greater than zero)

bool kk_std_core_is_pos_2(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> bool */ 
  kk_std_core_types__order x_17015;
  kk_std_core_types__order _brw_19793 = kk_int_as_order(kk_integer_signum_borrow(i),kk_context()); /*order*/;
  kk_integer_drop(i, _ctx);
  x_17015 = _brw_19793; /*order*/
  kk_integer_t _brw_19791;
  if (kk_std_core_types__is_Lt(x_17015)) {
    _brw_19791 = kk_integer_from_small(-1); /*int*/
    goto _match20376;
  }
  if (kk_std_core_types__is_Eq(x_17015)) {
    _brw_19791 = kk_integer_from_small(0); /*int*/
    goto _match20376;
  }
  {
    _brw_19791 = kk_integer_from_small(1); /*int*/
  }
  _match20376: ;
  bool _brw_19792 = kk_integer_eq_borrow(_brw_19791,(kk_integer_from_small(1)),kk_context()); /*bool*/;
  kk_integer_drop(_brw_19791, _ctx);
  return _brw_19792;
}
 
// Show a character as a string

kk_string_t kk_std_core_show_char(kk_char_t c, kk_context_t* _ctx) { /* (c : char) -> string */ 
  bool _match_19765 = (c < (' ')); /*bool*/;
  if (_match_19765) {
    bool _match_19780 = (c == 0x000A); /*bool*/;
    if (_match_19780) {
      kk_define_string_literal(, _s20383, 2, "\\n")
      return kk_string_dup(_s20383);
    }
    {
      bool _match_19781 = (c == 0x000D); /*bool*/;
      if (_match_19781) {
        kk_define_string_literal(, _s20384, 2, "\\r")
        return kk_string_dup(_s20384);
      }
      {
        bool _match_19782 = (c == 0x0009); /*bool*/;
        if (_match_19782) {
          kk_define_string_literal(, _s20385, 2, "\\t")
          return kk_string_dup(_s20385);
        }
        {
          bool _match_19783;
          kk_integer_t _brw_19788 = kk_integer_from_int(c,kk_context()); /*int*/;
          bool _brw_19789 = kk_integer_lte_borrow(_brw_19788,(kk_integer_from_small(255)),kk_context()); /*bool*/;
          kk_integer_drop(_brw_19788, _ctx);
          _match_19783 = _brw_19789; /*bool*/
          if (_match_19783) {
            kk_integer_t _arg_7906 = kk_integer_from_int(c,kk_context()); /*int*/;
            kk_integer_t _b_18628_18616 = kk_integer_from_small(2); /*int*/;
            kk_string_t _b_18629_18617 = kk_string_empty(); /*string*/;
            kk_string_t _x20387;
            kk_define_string_literal(, _s20388, 2, "\\x")
            _x20387 = kk_string_dup(_s20388); /*string*/
            kk_string_t _x20389;
            kk_std_core_types__optional _x20390 = kk_std_core_types__new_Optional(kk_integer_box(_b_18628_18616), _ctx); /*optional<1035>*/
            kk_std_core_types__optional _x20391 = kk_std_core_types__new_Optional(kk_string_box(_b_18629_18617), _ctx); /*optional<1035>*/
            _x20389 = kk_std_core_show_hex(_arg_7906, _x20390, kk_std_core_types__new_None(_ctx), _x20391, _ctx); /*string*/
            return kk_std_core__lp__plus__plus__1_rp_(_x20387, _x20389, _ctx);
          }
          {
            bool _match_19784;
            kk_integer_t _brw_19786 = kk_integer_from_int(c,kk_context()); /*int*/;
            kk_integer_t _brw_19785 = kk_integer_from_int(65535, _ctx); /*int*/;
            bool _brw_19787 = kk_integer_lte_borrow(_brw_19786,_brw_19785,kk_context()); /*bool*/;
            kk_integer_drop(_brw_19786, _ctx);
            kk_integer_drop(_brw_19785, _ctx);
            _match_19784 = _brw_19787; /*bool*/
            if (_match_19784) {
              kk_integer_t _arg_7994 = kk_integer_from_int(c,kk_context()); /*int*/;
              kk_integer_t _b_18630_18618 = kk_integer_from_small(4); /*int*/;
              kk_string_t _b_18631_18619 = kk_string_empty(); /*string*/;
              kk_string_t _x20393;
              kk_define_string_literal(, _s20394, 2, "\\u")
              _x20393 = kk_string_dup(_s20394); /*string*/
              kk_string_t _x20395;
              kk_std_core_types__optional _x20396 = kk_std_core_types__new_Optional(kk_integer_box(_b_18630_18618), _ctx); /*optional<1035>*/
              kk_std_core_types__optional _x20397 = kk_std_core_types__new_Optional(kk_string_box(_b_18631_18619), _ctx); /*optional<1035>*/
              _x20395 = kk_std_core_show_hex(_arg_7994, _x20396, kk_std_core_types__new_None(_ctx), _x20397, _ctx); /*string*/
              return kk_std_core__lp__plus__plus__1_rp_(_x20393, _x20395, _ctx);
            }
            {
              kk_integer_t _arg_8038 = kk_integer_from_int(c,kk_context()); /*int*/;
              kk_integer_t _b_18632_18620 = kk_integer_from_small(6); /*int*/;
              kk_string_t _b_18633_18621 = kk_string_empty(); /*string*/;
              kk_string_t _x20399;
              kk_define_string_literal(, _s20400, 2, "\\U")
              _x20399 = kk_string_dup(_s20400); /*string*/
              kk_string_t _x20401;
              kk_std_core_types__optional _x20402 = kk_std_core_types__new_Optional(kk_integer_box(_b_18632_18620), _ctx); /*optional<1035>*/
              kk_std_core_types__optional _x20403 = kk_std_core_types__new_Optional(kk_string_box(_b_18633_18621), _ctx); /*optional<1035>*/
              _x20401 = kk_std_core_show_hex(_arg_8038, _x20402, kk_std_core_types__new_None(_ctx), _x20403, _ctx); /*string*/
              return kk_std_core__lp__plus__plus__1_rp_(_x20399, _x20401, _ctx);
            }
          }
        }
      }
    }
  }
  {
    bool _match_19766 = (c > ('~')); /*bool*/;
    if (_match_19766) {
      bool _match_19770 = (c == 0x000A); /*bool*/;
      if (_match_19770) {
        kk_define_string_literal(, _s20404, 2, "\\n")
        return kk_string_dup(_s20404);
      }
      {
        bool _match_19771 = (c == 0x000D); /*bool*/;
        if (_match_19771) {
          kk_define_string_literal(, _s20405, 2, "\\r")
          return kk_string_dup(_s20405);
        }
        {
          bool _match_19772 = (c == 0x0009); /*bool*/;
          if (_match_19772) {
            kk_define_string_literal(, _s20406, 2, "\\t")
            return kk_string_dup(_s20406);
          }
          {
            bool _match_19773;
            kk_integer_t _brw_19778 = kk_integer_from_int(c,kk_context()); /*int*/;
            bool _brw_19779 = kk_integer_lte_borrow(_brw_19778,(kk_integer_from_small(255)),kk_context()); /*bool*/;
            kk_integer_drop(_brw_19778, _ctx);
            _match_19773 = _brw_19779; /*bool*/
            if (_match_19773) {
              kk_integer_t _arg_79060 = kk_integer_from_int(c,kk_context()); /*int*/;
              kk_integer_t _b_18634_18622 = kk_integer_from_small(2); /*int*/;
              kk_string_t _b_18635_18623 = kk_string_empty(); /*string*/;
              kk_string_t _x20408;
              kk_define_string_literal(, _s20409, 2, "\\x")
              _x20408 = kk_string_dup(_s20409); /*string*/
              kk_string_t _x20410;
              kk_std_core_types__optional _x20411 = kk_std_core_types__new_Optional(kk_integer_box(_b_18634_18622), _ctx); /*optional<1035>*/
              kk_std_core_types__optional _x20412 = kk_std_core_types__new_Optional(kk_string_box(_b_18635_18623), _ctx); /*optional<1035>*/
              _x20410 = kk_std_core_show_hex(_arg_79060, _x20411, kk_std_core_types__new_None(_ctx), _x20412, _ctx); /*string*/
              return kk_std_core__lp__plus__plus__1_rp_(_x20408, _x20410, _ctx);
            }
            {
              bool _match_19774;
              kk_integer_t _brw_19776 = kk_integer_from_int(c,kk_context()); /*int*/;
              kk_integer_t _brw_19775 = kk_integer_from_int(65535, _ctx); /*int*/;
              bool _brw_19777 = kk_integer_lte_borrow(_brw_19776,_brw_19775,kk_context()); /*bool*/;
              kk_integer_drop(_brw_19776, _ctx);
              kk_integer_drop(_brw_19775, _ctx);
              _match_19774 = _brw_19777; /*bool*/
              if (_match_19774) {
                kk_integer_t _arg_79940 = kk_integer_from_int(c,kk_context()); /*int*/;
                kk_integer_t _b_18636_18624 = kk_integer_from_small(4); /*int*/;
                kk_string_t _b_18637_18625 = kk_string_empty(); /*string*/;
                kk_string_t _x20414;
                kk_define_string_literal(, _s20415, 2, "\\u")
                _x20414 = kk_string_dup(_s20415); /*string*/
                kk_string_t _x20416;
                kk_std_core_types__optional _x20417 = kk_std_core_types__new_Optional(kk_integer_box(_b_18636_18624), _ctx); /*optional<1035>*/
                kk_std_core_types__optional _x20418 = kk_std_core_types__new_Optional(kk_string_box(_b_18637_18625), _ctx); /*optional<1035>*/
                _x20416 = kk_std_core_show_hex(_arg_79940, _x20417, kk_std_core_types__new_None(_ctx), _x20418, _ctx); /*string*/
                return kk_std_core__lp__plus__plus__1_rp_(_x20414, _x20416, _ctx);
              }
              {
                kk_integer_t _arg_80380 = kk_integer_from_int(c,kk_context()); /*int*/;
                kk_integer_t _b_18638_18626 = kk_integer_from_small(6); /*int*/;
                kk_string_t _b_18639_18627 = kk_string_empty(); /*string*/;
                kk_string_t _x20420;
                kk_define_string_literal(, _s20421, 2, "\\U")
                _x20420 = kk_string_dup(_s20421); /*string*/
                kk_string_t _x20422;
                kk_std_core_types__optional _x20423 = kk_std_core_types__new_Optional(kk_integer_box(_b_18638_18626), _ctx); /*optional<1035>*/
                kk_std_core_types__optional _x20424 = kk_std_core_types__new_Optional(kk_string_box(_b_18639_18627), _ctx); /*optional<1035>*/
                _x20422 = kk_std_core_show_hex(_arg_80380, _x20423, kk_std_core_types__new_None(_ctx), _x20424, _ctx); /*string*/
                return kk_std_core__lp__plus__plus__1_rp_(_x20420, _x20422, _ctx);
              }
            }
          }
        }
      }
    }
    {
      bool _match_19767 = (c == ('\'')); /*bool*/;
      if (_match_19767) {
        kk_define_string_literal(, _s20425, 2, "\\\'")
        return kk_string_dup(_s20425);
      }
      {
        bool _match_19768 = (c == ('"')); /*bool*/;
        if (_match_19768) {
          kk_define_string_literal(, _s20426, 2, "\\\"")
          return kk_string_dup(_s20426);
        }
        {
          bool _match_19769 = (c == ('\\')); /*bool*/;
          if (_match_19769) {
            kk_define_string_literal(, _s20427, 2, "\\\\")
            return kk_string_dup(_s20427);
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
  bool _match_19763 = (dabs < (1.0e-15)); /*bool*/;
  if (_match_19763) {
    int32_t _x20431;
    kk_integer_t _x20432;
    if (kk_std_core_types__is_Optional(precision)) {
      kk_box_t _box_x18641 = precision._cons.Optional.value;
      kk_integer_t _precision_8127 = kk_integer_unbox(_box_x18641);
      _x20432 = _precision_8127; /*int*/
    }
    else {
      _x20432 = kk_integer_from_small(-2); /*int*/
    }
    _x20431 = kk_std_core_int32(_x20432, _ctx); /*int32*/
    return kk_std_core_show_expx(d, _x20431, _ctx);
  }
  {
    bool _match_19764 = (dabs > (1.0e21)); /*bool*/;
    if (_match_19764) {
      int32_t _x20434;
      kk_integer_t _x20435;
      if (kk_std_core_types__is_Optional(precision)) {
        kk_box_t _box_x18642 = precision._cons.Optional.value;
        kk_integer_t _precision_81270 = kk_integer_unbox(_box_x18642);
        _x20435 = _precision_81270; /*int*/
      }
      else {
        _x20435 = kk_integer_from_small(-2); /*int*/
      }
      _x20434 = kk_std_core_int32(_x20435, _ctx); /*int32*/
      return kk_std_core_show_expx(d, _x20434, _ctx);
    }
    {
      int32_t _x20437;
      kk_integer_t _x20438;
      if (kk_std_core_types__is_Optional(precision)) {
        kk_box_t _box_x18643 = precision._cons.Optional.value;
        kk_integer_t _precision_81271 = kk_integer_unbox(_box_x18643);
        _x20438 = _precision_81271; /*int*/
      }
      else {
        _x20438 = kk_integer_from_small(-2); /*int*/
      }
      _x20437 = kk_std_core_int32(_x20438, _ctx); /*int32*/
      return kk_std_core_show_fixedx(d, _x20437, _ctx);
    }
  }
}
 
// lifted local: show-list, join-acc

kk_string_t kk_std_core__lift17192_show_list(kk_string_t sep, kk_std_core__list ys, kk_string_t acc, kk_context_t* _ctx) { /* (sep : string, ys : list<string>, acc : string) -> string */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con20440 = kk_std_core__as_Cons(ys);
    kk_box_t _box_x18644 = _con20440->head;
    kk_std_core__list yy = _con20440->tail;
    kk_string_t y = kk_string_unbox(_box_x18644);
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      kk_std_core__list_free(ys, _ctx);
    }
    else {
      kk_string_dup(y);
      kk_std_core__list_dup(yy);
      kk_std_core__list_decref(ys, _ctx);
    }
    kk_string_t acc0_17221;
    kk_string_t _x20442;
    kk_string_t _x20443 = kk_string_dup(sep); /*string*/
    _x20442 = kk_std_core__lp__plus__plus__1_rp_(_x20443, y, _ctx); /*string*/
    acc0_17221 = kk_std_core__lp__plus__plus__1_rp_(acc, _x20442, _ctx); /*string*/
    { // tailcall
      ys = yy;
      acc = acc0_17221;
      goto kk__tailcall;
    }
  }
  {
    kk_string_drop(sep, _ctx);
    return acc;
  }
}
 
// monadic lift

kk_string_t kk_std_core__mlift17695_show_list(kk_std_core__list _y_17414, kk_context_t* _ctx) { /* forall<e> (list<string>) -> e string */ 
  kk_string_t _x20444;
  kk_define_string_literal(, _s20445, 1, "[")
  _x20444 = kk_string_dup(_s20445); /*string*/
  kk_string_t _x20446;
  kk_string_t _x20447;
  if (kk_std_core__is_Nil(_y_17414)) {
    _x20447 = kk_string_empty(); /*string*/
  }
  else {
    struct kk_std_core_Cons* _con20449 = kk_std_core__as_Cons(_y_17414);
    kk_box_t _box_x18645 = _con20449->head;
    kk_std_core__list xx = _con20449->tail;
    kk_string_t x = kk_string_unbox(_box_x18645);
    if (kk_likely(kk_std_core__list_is_unique(_y_17414))) {
      kk_std_core__list_free(_y_17414, _ctx);
    }
    else {
      kk_string_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(_y_17414, _ctx);
    }
    kk_string_t _x20451;
    kk_define_string_literal(, _s20452, 1, ",")
    _x20451 = kk_string_dup(_s20452); /*string*/
    _x20447 = kk_std_core__lift17192_show_list(_x20451, xx, x, _ctx); /*string*/
  }
  kk_string_t _x20453;
  kk_define_string_literal(, _s20454, 1, "]")
  _x20453 = kk_string_dup(_s20454); /*string*/
  _x20446 = kk_std_core__lp__plus__plus__1_rp_(_x20447, _x20453, _ctx); /*string*/
  return kk_std_core__lp__plus__plus__1_rp_(_x20444, _x20446, _ctx);
}
 
// Convert a list to a string


// lift anonymous function
struct kk_std_core_show_list_fun20455__t {
  struct kk_function_s _base;
  kk_function_t show_elem;
};
static kk_box_t kk_std_core_show_list_fun20455(kk_function_t _fself, kk_box_t _b_18648, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_show_list_fun20455(kk_function_t show_elem, kk_context_t* _ctx) {
  struct kk_std_core_show_list_fun20455__t* _self = kk_function_alloc_as(struct kk_std_core_show_list_fun20455__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_show_list_fun20455, kk_context());
  _self->show_elem = show_elem;
  return &_self->_base;
}

static kk_box_t kk_std_core_show_list_fun20455(kk_function_t _fself, kk_box_t _b_18648, kk_context_t* _ctx) {
  struct kk_std_core_show_list_fun20455__t* _self = kk_function_as(struct kk_std_core_show_list_fun20455__t*, _fself);
  kk_function_t show_elem = _self->show_elem; /* (8672) -> 8673 string */
  kk_drop_match(_self, {kk_function_dup(show_elem);}, {}, _ctx)
  kk_string_t _x20456 = kk_function_call(kk_string_t, (kk_function_t, kk_box_t, kk_context_t*), show_elem, (show_elem, _b_18648, _ctx)); /*string*/
  return kk_string_box(_x20456);
}


// lift anonymous function
struct kk_std_core_show_list_fun20458__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_show_list_fun20458(kk_function_t _fself, kk_box_t _b_18653, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_show_list_fun20458(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_show_list_fun20458, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_show_list_fun20458(kk_function_t _fself, kk_box_t _b_18653, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_std_core__list _y_18656_17414 = kk_std_core__list_unbox(_b_18653, _ctx); /*list<string>*/;
  kk_string_t _x20459;
  kk_string_t _x20460;
  kk_define_string_literal(, _s20461, 1, "[")
  _x20460 = kk_string_dup(_s20461); /*string*/
  kk_string_t _x20462;
  kk_string_t _x20463;
  if (kk_std_core__is_Nil(_y_18656_17414)) {
    _x20463 = kk_string_empty(); /*string*/
  }
  else {
    struct kk_std_core_Cons* _con20465 = kk_std_core__as_Cons(_y_18656_17414);
    kk_box_t _box_x18651 = _con20465->head;
    kk_std_core__list xx = _con20465->tail;
    kk_string_t x0 = kk_string_unbox(_box_x18651);
    if (kk_likely(kk_std_core__list_is_unique(_y_18656_17414))) {
      kk_std_core__list_free(_y_18656_17414, _ctx);
    }
    else {
      kk_string_dup(x0);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(_y_18656_17414, _ctx);
    }
    kk_string_t _x20467;
    kk_define_string_literal(, _s20468, 1, ",")
    _x20467 = kk_string_dup(_s20468); /*string*/
    _x20463 = kk_std_core__lift17192_show_list(_x20467, xx, x0, _ctx); /*string*/
  }
  kk_string_t _x20469;
  kk_define_string_literal(, _s20470, 1, "]")
  _x20469 = kk_string_dup(_s20470); /*string*/
  _x20462 = kk_std_core__lp__plus__plus__1_rp_(_x20463, _x20469, _ctx); /*string*/
  _x20459 = kk_std_core__lp__plus__plus__1_rp_(_x20460, _x20462, _ctx); /*string*/
  return kk_string_box(_x20459);
}

kk_string_t kk_std_core_show_list(kk_std_core__list xs, kk_function_t show_elem, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, show-elem : (a) -> e string) -> e string */ 
  kk_std_core__list x_17899 = kk_std_core_map_5(xs, kk_std_core_new_show_list_fun20455(show_elem, _ctx), _ctx); /*list<string>*/;
  if (kk_yielding(kk_context())) {
    kk_std_core__list_drop(x_17899, _ctx);
    kk_box_t _x20457 = kk_std_core_hnd_yield_extend(kk_std_core_new_show_list_fun20458(_ctx), _ctx); /*3926*/
    return kk_string_unbox(_x20457);
  }
  {
    kk_string_t _x20471;
    kk_define_string_literal(, _s20472, 1, "[")
    _x20471 = kk_string_dup(_s20472); /*string*/
    kk_string_t _x20473;
    kk_string_t _x20474;
    if (kk_std_core__is_Nil(x_17899)) {
      _x20474 = kk_string_empty(); /*string*/
    }
    else {
      struct kk_std_core_Cons* _con20476 = kk_std_core__as_Cons(x_17899);
      kk_box_t _box_x18654 = _con20476->head;
      kk_std_core__list xx0 = _con20476->tail;
      kk_string_t x1 = kk_string_unbox(_box_x18654);
      if (kk_likely(kk_std_core__list_is_unique(x_17899))) {
        kk_std_core__list_free(x_17899, _ctx);
      }
      else {
        kk_string_dup(x1);
        kk_std_core__list_dup(xx0);
        kk_std_core__list_decref(x_17899, _ctx);
      }
      kk_string_t _x20478;
      kk_define_string_literal(, _s20479, 1, ",")
      _x20478 = kk_string_dup(_s20479); /*string*/
      _x20474 = kk_std_core__lift17192_show_list(_x20478, xx0, x1, _ctx); /*string*/
    }
    kk_string_t _x20480;
    kk_define_string_literal(, _s20481, 1, "]")
    _x20480 = kk_string_dup(_s20481); /*string*/
    _x20473 = kk_std_core__lp__plus__plus__1_rp_(_x20474, _x20480, _ctx); /*string*/
    return kk_std_core__lp__plus__plus__1_rp_(_x20471, _x20473, _ctx);
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
  bool _match_19760 = (dabs >= (1.0e-5)); /*bool*/;
  if (_match_19760) {
    bool _match_19761 = (dabs < (1.0e21)); /*bool*/;
    if (_match_19761) {
      kk_std_core_types__optional _x20482;
      kk_box_t _x20483;
      kk_integer_t _x20484;
      if (kk_std_core_types__is_Optional(precision)) {
        kk_box_t _box_x18657 = precision._cons.Optional.value;
        kk_integer_t _precision_8682 = kk_integer_unbox(_box_x18657);
        _x20484 = _precision_8682; /*int*/
      }
      else {
        _x20484 = kk_integer_from_small(-17); /*int*/
      }
      _x20483 = kk_integer_box(_x20484); /*1035*/
      _x20482 = kk_std_core_types__new_Optional(_x20483, _ctx); /*optional<1035>*/
      return kk_std_core_show_fixed(d, _x20482, _ctx);
    }
    {
      int32_t _x20486;
      kk_integer_t _x20487;
      if (kk_std_core_types__is_Optional(precision)) {
        kk_box_t _box_x18659 = precision._cons.Optional.value;
        kk_integer_t _precision_86820 = kk_integer_unbox(_box_x18659);
        _x20487 = _precision_86820; /*int*/
      }
      else {
        _x20487 = kk_integer_from_small(-17); /*int*/
      }
      _x20486 = kk_std_core_int32(_x20487, _ctx); /*int32*/
      return kk_std_core_show_expx(d, _x20486, _ctx);
    }
  }
  {
    int32_t _x20489;
    kk_integer_t _x20490;
    if (kk_std_core_types__is_Optional(precision)) {
      kk_box_t _box_x18660 = precision._cons.Optional.value;
      kk_integer_t _precision_86821 = kk_integer_unbox(_box_x18660);
      _x20490 = _precision_86821; /*int*/
    }
    else {
      _x20490 = kk_integer_from_small(-17); /*int*/
    }
    _x20489 = kk_std_core_int32(_x20490, _ctx); /*int32*/
    return kk_std_core_show_expx(d, _x20489, _ctx);
  }
}
 
// lifted local: show.3, join-acc

kk_string_t kk_std_core__lift17193_show_3(kk_std_core__list ys, kk_string_t acc, kk_context_t* _ctx) { /* (ys : list<string>, acc : string) -> string */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con20498 = kk_std_core__as_Cons(ys);
    kk_box_t _box_x18662 = _con20498->head;
    kk_std_core__list yy = _con20498->tail;
    kk_string_t y = kk_string_unbox(_box_x18662);
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      kk_std_core__list_free(ys, _ctx);
    }
    else {
      kk_string_dup(y);
      kk_std_core__list_dup(yy);
      kk_std_core__list_decref(ys, _ctx);
    }
    { // tailcall
      kk_string_t _x20500;
      kk_string_t _x20501;
      kk_string_t _x20502 = kk_string_empty(); /*string*/
      _x20501 = kk_std_core__lp__plus__plus__1_rp_(_x20502, y, _ctx); /*string*/
      _x20500 = kk_std_core__lp__plus__plus__1_rp_(acc, _x20501, _ctx); /*string*/
      ys = yy;
      acc = _x20500;
      goto kk__tailcall;
    }
  }
  {
    return acc;
  }
}
 
// Show a string as a string literal


// lift anonymous function
struct kk_std_core_show_fun20504__t_3 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_show_fun20504_3(kk_function_t _fself, kk_box_t _b_18665, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_show_fun20504_3(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_show_fun20504_3, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_show_fun20504_3(kk_function_t _fself, kk_box_t _b_18665, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_string_t _x20505;
  kk_char_t _x20506 = kk_char_unbox(_b_18665, _ctx); /*char*/
  _x20505 = kk_std_core_show_char(_x20506, _ctx); /*string*/
  return kk_string_box(_x20505);
}

kk_string_t kk_std_core_show_3(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> string */ 
  kk_std_core__list _b_18666_18663 = kk_std_core_list_6(s, _ctx); /*list<char>*/;
  kk_std_core__list xs_17031 = kk_std_core_map_5(_b_18666_18663, kk_std_core_new_show_fun20504_3(_ctx), _ctx); /*list<string>*/;
  kk_string_t _x20507;
  kk_define_string_literal(, _s20508, 1, "\"")
  _x20507 = kk_string_dup(_s20508); /*string*/
  kk_string_t _x20509;
  kk_string_t _x20510;
  if (kk_std_core__is_Nil(xs_17031)) {
    _x20510 = kk_string_empty(); /*string*/
  }
  else {
    struct kk_std_core_Cons* _con20512 = kk_std_core__as_Cons(xs_17031);
    kk_box_t _box_x18668 = _con20512->head;
    kk_std_core__list xx = _con20512->tail;
    kk_string_t x = kk_string_unbox(_box_x18668);
    if (kk_likely(kk_std_core__list_is_unique(xs_17031))) {
      kk_std_core__list_free(xs_17031, _ctx);
    }
    else {
      kk_string_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs_17031, _ctx);
    }
    _x20510 = kk_std_core__lift17193_show_3(xx, x, _ctx); /*string*/
  }
  kk_string_t _x20514;
  kk_define_string_literal(, _s20515, 1, "\"")
  _x20514 = kk_string_dup(_s20515); /*string*/
  _x20509 = kk_std_core__lp__plus__plus__1_rp_(_x20510, _x20514, _ctx); /*string*/
  return kk_std_core__lp__plus__plus__1_rp_(_x20507, _x20509, _ctx);
}
extern kk_string_t kk_std_core_show_fun20520_7(kk_function_t _fself, kk_box_t _b_18671, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_string_t _x20521 = kk_string_unbox(_b_18671); /*string*/
  return kk_std_core_show_3(_x20521, _ctx);
}
extern kk_string_t kk_std_core_show_fun20522_8(kk_function_t _fself, kk_box_t _b_18676, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_integer_t _x20523 = kk_integer_unbox(_b_18676); /*int*/
  return kk_std_core_show(_x20523, _ctx);
}
extern kk_string_t kk_std_core_show_fun20524_9(kk_function_t _fself, kk_box_t _b_18681, kk_context_t* _ctx) {
  kk_unused(_fself);
  bool _x20525 = kk_bool_unbox(_b_18681); /*bool*/
  return kk_std_core_show_4(_x20525, _ctx);
}

kk_unit_t kk_std_core_prints(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> console () */ 
  kk_std_core_types__maybe _match_19759;
  kk_box_t _x20534;
  kk_ref_t _x20535 = kk_ref_dup(kk_std_core_redirect); /*ref<global,maybe<(string) -> console ()>>*/
  _x20534 = kk_ref_get(_x20535,kk_context()); /*1001*/
  _match_19759 = kk_std_core_types__maybe_unbox(_x20534, _ctx); /*maybe<(string) -> console ()>*/
  if (kk_std_core_types__is_Nothing(_match_19759)) {
    kk_std_core_xprints(s, _ctx); return kk_Unit;
  }
  {
    kk_box_t _fun_unbox_x18688 = _match_19759._cons.Just.value;
    kk_box_t _x20536;
    kk_function_t _x20537 = kk_function_unbox(_fun_unbox_x18688); /*(18689) -> console 18690*/
    _x20536 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x20537, (_x20537, kk_string_box(s), _ctx)); /*18690*/
    kk_unit_unbox(_x20536); return kk_Unit;
  }
}


// lift anonymous function
struct kk_std_core__default_exn_fun20542__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core__default_exn_fun20542(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__525__16, kk_std_core__exception x, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_default_exn_fun20542(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core__default_exn_fun20542, _ctx)
  return kk_function_dup(_fself);
}



// lift anonymous function
struct kk_std_core__default_exn_fun20543__t {
  struct kk_function_s _base;
  kk_std_core__exception x;
};
static kk_box_t kk_std_core__default_exn_fun20543(kk_function_t _fself, kk_function_t _b_18703, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_default_exn_fun20543(kk_std_core__exception x, kk_context_t* _ctx) {
  struct kk_std_core__default_exn_fun20543__t* _self = kk_function_alloc_as(struct kk_std_core__default_exn_fun20543__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__default_exn_fun20543, kk_context());
  _self->x = x;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core__default_exn_fun20544__t {
  struct kk_function_s _base;
  kk_function_t _b_18703;
};
static kk_unit_t kk_std_core__default_exn_fun20544(kk_function_t _fself, kk_std_core_hnd__resume_result _b_18704, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_default_exn_fun20544(kk_function_t _b_18703, kk_context_t* _ctx) {
  struct kk_std_core__default_exn_fun20544__t* _self = kk_function_alloc_as(struct kk_std_core__default_exn_fun20544__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__default_exn_fun20544, kk_context());
  _self->_b_18703 = _b_18703;
  return &_self->_base;
}

static kk_unit_t kk_std_core__default_exn_fun20544(kk_function_t _fself, kk_std_core_hnd__resume_result _b_18704, kk_context_t* _ctx) {
  struct kk_std_core__default_exn_fun20544__t* _self = kk_function_as(struct kk_std_core__default_exn_fun20544__t*, _fself);
  kk_function_t _b_18703 = _self->_b_18703; /* (std/core/hnd/resume-result<3990,3993>) -> 3992 3993 */
  kk_drop_match(_self, {kk_function_dup(_b_18703);}, {}, _ctx)
  kk_box_t _x20545 = kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__resume_result, kk_context_t*), _b_18703, (_b_18703, _b_18704, _ctx)); /*3993*/
  return kk_unit_unbox(_x20545);
}
static kk_box_t kk_std_core__default_exn_fun20543(kk_function_t _fself, kk_function_t _b_18703, kk_context_t* _ctx) {
  struct kk_std_core__default_exn_fun20543__t* _self = kk_function_as(struct kk_std_core__default_exn_fun20543__t*, _fself);
  kk_std_core__exception x = _self->x; /* exception */
  kk_drop_match(_self, {kk_std_core__exception_dup(x);}, {}, _ctx)
  kk_function_t ___wildcard__525__45_18721 = kk_std_core__new_default_exn_fun20544(_b_18703, _ctx); /*(std/core/hnd/resume-result<9736,()>) -> <console|9734> ()*/;
  kk_function_drop(___wildcard__525__45_18721, _ctx);
  kk_unit_t __ = kk_Unit;
  kk_string_t _x20546;
  kk_define_string_literal(, _s20547, 20, "uncaught exception: ")
  _x20546 = kk_string_dup(_s20547); /*string*/
  kk_std_core_prints(_x20546, _ctx);
  kk_unit_t _x20548 = kk_Unit;
  kk_string_t _x20549;
  {
    kk_string_t _x = x.message;
    kk_string_dup(_x);
    kk_std_core__exception_drop(x, _ctx);
    _x20549 = _x; /*string*/
  }
  kk_std_core_printsln(_x20549, _ctx);
  return kk_unit_box(_x20548);
}
static kk_box_t kk_std_core__default_exn_fun20542(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__525__16, kk_std_core__exception x, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_std_core_hnd__ev_dropn(___wildcard__525__16, (KK_I32(3)), _ctx);
  return kk_std_core_hnd_yield_to_final(m0, kk_std_core__new_default_exn_fun20543(x, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core__default_exn_fun20552__t {
  struct kk_function_s _base;
  kk_function_t _b_18718_18705;
};
static kk_box_t kk_std_core__default_exn_fun20552(kk_function_t _fself, kk_std_core_hnd__marker _b_18706, kk_std_core_hnd__ev _b_18707, kk_box_t _b_18708, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_default_exn_fun20552(kk_function_t _b_18718_18705, kk_context_t* _ctx) {
  struct kk_std_core__default_exn_fun20552__t* _self = kk_function_alloc_as(struct kk_std_core__default_exn_fun20552__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__default_exn_fun20552, kk_context());
  _self->_b_18718_18705 = _b_18718_18705;
  return &_self->_base;
}

static kk_box_t kk_std_core__default_exn_fun20552(kk_function_t _fself, kk_std_core_hnd__marker _b_18706, kk_std_core_hnd__ev _b_18707, kk_box_t _b_18708, kk_context_t* _ctx) {
  struct kk_std_core__default_exn_fun20552__t* _self = kk_function_as(struct kk_std_core__default_exn_fun20552__t*, _fself);
  kk_function_t _b_18718_18705 = _self->_b_18718_18705; /* (m0 : std/core/hnd/marker<<console|9734>,()>, std/core/hnd/ev<.hnd-exn>, x : exception) -> <console|9734> 9736 */
  kk_drop_match(_self, {kk_function_dup(_b_18718_18705);}, {}, _ctx)
  kk_std_core__exception _x20553 = kk_std_core__exception_unbox(_b_18708, _ctx); /*exception*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_std_core__exception, kk_context_t*), _b_18718_18705, (_b_18718_18705, _b_18706, _b_18707, _x20553, _ctx));
}


// lift anonymous function
struct kk_std_core__default_exn_fun20554__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core__default_exn_fun20554(kk_function_t _fself, kk_box_t _b_18713, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_default_exn_fun20554(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core__default_exn_fun20554, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core__default_exn_fun20554(kk_function_t _fself, kk_box_t _b_18713, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_unit_t _x0_18722 = kk_Unit;
  kk_unit_unbox(_b_18713);
  return kk_unit_box(_x0_18722);
}


// lift anonymous function
struct kk_std_core__default_exn_fun20555__t {
  struct kk_function_s _base;
  kk_function_t action;
};
static kk_box_t kk_std_core__default_exn_fun20555(kk_function_t _fself, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_default_exn_fun20555(kk_function_t action, kk_context_t* _ctx) {
  struct kk_std_core__default_exn_fun20555__t* _self = kk_function_alloc_as(struct kk_std_core__default_exn_fun20555__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__default_exn_fun20555, kk_context());
  _self->action = action;
  return &_self->_base;
}

static kk_box_t kk_std_core__default_exn_fun20555(kk_function_t _fself, kk_context_t* _ctx) {
  struct kk_std_core__default_exn_fun20555__t* _self = kk_function_as(struct kk_std_core__default_exn_fun20555__t*, _fself);
  kk_function_t action = _self->action; /* () -> <exn,console|9734> () */
  kk_drop_match(_self, {kk_function_dup(action);}, {}, _ctx)
  kk_unit_t _x20556 = kk_Unit;
  kk_function_call(kk_unit_t, (kk_function_t, kk_context_t*), action, (action, _ctx));
  return kk_unit_box(_x20556);
}

kk_unit_t kk_std_core__default_exn(kk_function_t action, kk_context_t* _ctx) { /* forall<e> (action : () -> <console,exn|e> ()) -> <console|e> () */ 
  int32_t _b_18714_18709 = (KK_I32(0)); /*int32*/;
  kk_box_t _x20541;
  kk_function_t _b_18718_18705 = kk_std_core__new_default_exn_fun20542(_ctx); /*(m0 : std/core/hnd/marker<<console|9734>,()>, std/core/hnd/ev<.hnd-exn>, x : exception) -> <console|9734> 9736*/;
  kk_std_core__hnd_exn _x20550;
  kk_std_core_hnd__clause1 _x20551 = kk_std_core_hnd__new_Clause1(kk_std_core__new_default_exn_fun20552(_b_18718_18705, _ctx), _ctx); /*std/core/hnd/clause1<51,52,53,54,55>*/
  _x20550 = kk_std_core__new_Hnd_exn(kk_reuse_null, _x20551, _ctx); /*.hnd-exn<11,12>*/
  _x20541 = kk_std_core__handle_exn(_b_18714_18709, _x20550, kk_std_core__new_default_exn_fun20554(_ctx), kk_std_core__new_default_exn_fun20555(action, _ctx), _ctx); /*1969*/
  kk_unit_unbox(_x20541); return kk_Unit;
}
 
// Get (zero-based) element `n`  of a list. Return a `:maybe` type.

kk_std_core_types__maybe kk_std_core__lp__lb__rb__2_rp_(kk_std_core__list xs, kk_integer_t n, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, n : int) -> maybe<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con20557 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con20557->head;
    kk_std_core__list xx = _con20557->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    bool _match_19754 = kk_integer_gt_borrow(n,(kk_integer_from_small(0)),kk_context()); /*bool*/;
    if (_match_19754) {
      kk_box_drop(x, _ctx);
      { // tailcall
        kk_integer_t _x20558 = kk_integer_add_small_const(n, -1, _ctx); /*int*/
        xs = xx;
        n = _x20558;
        goto kk__tailcall;
      }
    }
    {
      kk_std_core__list_drop(xx, _ctx);
      bool _match_19755;
      bool _brw_19756 = kk_integer_eq_borrow(n,(kk_integer_from_small(0)),kk_context()); /*bool*/;
      kk_integer_drop(n, _ctx);
      _match_19755 = _brw_19756; /*bool*/
      if (_match_19755) {
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
    kk_string_t _x20559 = kk_string_dup(s); /*string*/
    kk_ssize_t _x20560 = (start0 + len0); /*ssize_t*/
    kk_ssize_t _x20561;
    kk_ssize_t _x20562 = kk_string_len(s,kk_context()); /*ssize_t*/
    kk_ssize_t _x20563 = (start0 + len0); /*ssize_t*/
    _x20561 = (_x20562 - _x20563); /*ssize_t*/
    return kk_std_core__new_Sslice(_x20559, _x20560, _x20561, _ctx);
  }
}
 
// monadic lift

bool kk_std_core__mlift17696_all(kk_function_t predicate, kk_std_core__list xx, bool _y_17417, kk_context_t* _ctx) { /* forall<a,e> (predicate : (a) -> e bool, xx : list<a>, bool) -> e bool */ 
  if (_y_17417) {
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
struct kk_std_core_all_fun20567__t {
  struct kk_function_s _base;
  kk_function_t predicate0;
  kk_std_core__list xx0;
};
static kk_box_t kk_std_core_all_fun20567(kk_function_t _fself, kk_box_t _b_18724, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_all_fun20567(kk_function_t predicate0, kk_std_core__list xx0, kk_context_t* _ctx) {
  struct kk_std_core_all_fun20567__t* _self = kk_function_alloc_as(struct kk_std_core_all_fun20567__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_all_fun20567, kk_context());
  _self->predicate0 = predicate0;
  _self->xx0 = xx0;
  return &_self->_base;
}

static kk_box_t kk_std_core_all_fun20567(kk_function_t _fself, kk_box_t _b_18724, kk_context_t* _ctx) {
  struct kk_std_core_all_fun20567__t* _self = kk_function_as(struct kk_std_core_all_fun20567__t*, _fself);
  kk_function_t predicate0 = _self->predicate0; /* (9971) -> 9972 bool */
  kk_std_core__list xx0 = _self->xx0; /* list<9971> */
  kk_drop_match(_self, {kk_function_dup(predicate0);kk_std_core__list_dup(xx0);}, {}, _ctx)
  bool _y_18726_174170 = kk_bool_unbox(_b_18724); /*bool*/;
  bool _x20568 = kk_std_core__mlift17696_all(predicate0, xx0, _y_18726_174170, _ctx); /*bool*/
  return kk_bool_box(_x20568);
}

bool kk_std_core_all(kk_std_core__list xs, kk_function_t predicate0, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, predicate : (a) -> e bool) -> e bool */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con20564 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con20564->head;
    kk_std_core__list xx0 = _con20564->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(xs, _ctx);
    }
    bool x0_17907;
    kk_function_t _x20565 = kk_function_dup(predicate0); /*(9971) -> 9972 bool*/
    x0_17907 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x20565, (_x20565, x, _ctx)); /*bool*/
    if (kk_yielding(kk_context())) {
      kk_box_t _x20566 = kk_std_core_hnd_yield_extend(kk_std_core_new_all_fun20567(predicate0, xx0, _ctx), _ctx); /*3926*/
      return kk_bool_unbox(_x20566);
    }
    if (x0_17907) { // tailcall
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

bool kk_std_core__mlift17697_any(kk_function_t predicate, kk_std_core__list xx, bool _y_17421, kk_context_t* _ctx) { /* forall<a,e> (predicate : (a) -> e bool, xx : list<a>, bool) -> e bool */ 
  if (_y_17421) {
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
struct kk_std_core_any_fun20572__t {
  struct kk_function_s _base;
  kk_function_t predicate0;
  kk_std_core__list xx0;
};
static kk_box_t kk_std_core_any_fun20572(kk_function_t _fself, kk_box_t _b_18728, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_any_fun20572(kk_function_t predicate0, kk_std_core__list xx0, kk_context_t* _ctx) {
  struct kk_std_core_any_fun20572__t* _self = kk_function_alloc_as(struct kk_std_core_any_fun20572__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_any_fun20572, kk_context());
  _self->predicate0 = predicate0;
  _self->xx0 = xx0;
  return &_self->_base;
}

static kk_box_t kk_std_core_any_fun20572(kk_function_t _fself, kk_box_t _b_18728, kk_context_t* _ctx) {
  struct kk_std_core_any_fun20572__t* _self = kk_function_as(struct kk_std_core_any_fun20572__t*, _fself);
  kk_function_t predicate0 = _self->predicate0; /* (9999) -> 10000 bool */
  kk_std_core__list xx0 = _self->xx0; /* list<9999> */
  kk_drop_match(_self, {kk_function_dup(predicate0);kk_std_core__list_dup(xx0);}, {}, _ctx)
  bool _y_18730_174210 = kk_bool_unbox(_b_18728); /*bool*/;
  bool _x20573 = kk_std_core__mlift17697_any(predicate0, xx0, _y_18730_174210, _ctx); /*bool*/
  return kk_bool_box(_x20573);
}

bool kk_std_core_any(kk_std_core__list xs, kk_function_t predicate0, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, predicate : (a) -> e bool) -> e bool */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con20569 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con20569->head;
    kk_std_core__list xx0 = _con20569->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(xs, _ctx);
    }
    bool x0_17910;
    kk_function_t _x20570 = kk_function_dup(predicate0); /*(9999) -> 10000 bool*/
    x0_17910 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x20570, (_x20570, x, _ctx)); /*bool*/
    if (kk_yielding(kk_context())) {
      kk_box_t _x20571 = kk_std_core_hnd_yield_extend(kk_std_core_new_any_fun20572(predicate0, xx0, _ctx), _ctx); /*3926*/
      return kk_bool_unbox(_x20571);
    }
    if (x0_17910) {
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
  kk_integer_t _x20574 = kk_integer_dup(index); /*int*/
  idx = kk_std_core_ssize__t(_x20574, _ctx); /*ssize_t*/
  bool _match_19751;
  kk_ssize_t _x20575 = kk_vector_len_borrow(v); /*ssize_t*/
  _match_19751 = (_x20575 <= idx); /*bool*/
  if (_match_19751) {
    return kk_std_core_types__new_Nothing(_ctx);
  }
  {
    kk_box_t _x20576 = kk_vector_at_borrow(v,idx); /*228*/
    return kk_std_core_types__new_Just(_x20576, _ctx);
  }
}
 
// O(`n`). The first `n` (default = `1`) characters in a string.

kk_std_core__sslice kk_std_core_first(kk_string_t s, kk_std_core_types__optional n, kk_context_t* _ctx) { /* (s : string, n : optional<int>) -> sslice */ 
  kk_std_core__sslice slice0 = kk_std_core_first1(s, _ctx); /*sslice*/;
  bool _match_19745;
  kk_integer_t _brw_19748;
  if (kk_std_core_types__is_Optional(n)) {
    kk_box_t _box_x18731 = n._cons.Optional.value;
    kk_integer_t _n_10142 = kk_integer_unbox(_box_x18731);
    kk_integer_dup(_n_10142);
    _brw_19748 = _n_10142; /*int*/
  }
  else {
    _brw_19748 = kk_integer_from_small(1); /*int*/
  }
  bool _brw_19749 = kk_integer_eq_borrow(_brw_19748,(kk_integer_from_small(1)),kk_context()); /*bool*/;
  kk_integer_drop(_brw_19748, _ctx);
  _match_19745 = _brw_19749; /*bool*/
  if (_match_19745) {
    kk_std_core_types__optional_drop(n, _ctx);
    return slice0;
  }
  {
    kk_integer_t _brw_19746;
    kk_integer_t _x20581;
    if (kk_std_core_types__is_Optional(n)) {
      kk_box_t _box_x18732 = n._cons.Optional.value;
      kk_integer_t _n_101420 = kk_integer_unbox(_box_x18732);
      _x20581 = _n_101420; /*int*/
    }
    else {
      _x20581 = kk_integer_from_small(1); /*int*/
    }
    _brw_19746 = kk_integer_add_small_const(_x20581, -1, _ctx); /*int*/
    kk_std_core__sslice _brw_19747 = kk_std_core_extend(slice0, _brw_19746, _ctx); /*sslice*/;
    kk_integer_drop(_brw_19746, _ctx);
    return _brw_19747;
  }
}
 
// Convert the first character of a string to uppercase.

kk_string_t kk_std_core_capitalize(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> string */ 
  kk_std_core__sslice slice2;
  kk_string_t _x20583 = kk_string_dup(s); /*string*/
  slice2 = kk_std_core_first1(_x20583, _ctx); /*sslice*/
  kk_std_core__sslice slice1_17046;
  bool _match_19738;
  kk_integer_t _brw_19743;
  kk_std_core_types__optional _x20584 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
  if (kk_std_core_types__is_Optional(_x20584)) {
    kk_box_t _box_x18733 = _x20584._cons.Optional.value;
    kk_integer_t _n_101420 = kk_integer_unbox(_box_x18733);
    _brw_19743 = _n_101420; /*int*/
  }
  else {
    _brw_19743 = kk_integer_from_small(1); /*int*/
  }
  bool _brw_19744 = kk_integer_eq_borrow(_brw_19743,(kk_integer_from_small(1)),kk_context()); /*bool*/;
  kk_integer_drop(_brw_19743, _ctx);
  _match_19738 = _brw_19744; /*bool*/
  if (_match_19738) {
    slice1_17046 = slice2; /*sslice*/
  }
  else {
    kk_integer_t _brw_19740;
    kk_integer_t _x20586;
    kk_std_core_types__optional _x20587 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
    if (kk_std_core_types__is_Optional(_x20587)) {
      kk_box_t _box_x18734 = _x20587._cons.Optional.value;
      kk_integer_t _n_1014200 = kk_integer_unbox(_box_x18734);
      _x20586 = _n_1014200; /*int*/
    }
    else {
      _x20586 = kk_integer_from_small(1); /*int*/
    }
    _brw_19740 = kk_integer_add_small_const(_x20586, -1, _ctx); /*int*/
    kk_std_core__sslice _brw_19741 = kk_std_core_extend(slice2, _brw_19740, _ctx); /*sslice*/;
    kk_integer_drop(_brw_19740, _ctx);
    slice1_17046 = _brw_19741; /*sslice*/
  }
  kk_std_core__sslice slice0 = kk_std_core_first1(s, _ctx); /*sslice*/;
  kk_string_t _x20589;
  kk_string_t _x20590;
  kk_std_core__sslice _x20591;
  bool _match_19731;
  kk_integer_t _brw_19736;
  kk_std_core_types__optional _x20592 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
  if (kk_std_core_types__is_Optional(_x20592)) {
    kk_box_t _box_x18735 = _x20592._cons.Optional.value;
    kk_integer_t _n_10142 = kk_integer_unbox(_box_x18735);
    _brw_19736 = _n_10142; /*int*/
  }
  else {
    _brw_19736 = kk_integer_from_small(1); /*int*/
  }
  bool _brw_19737 = kk_integer_eq_borrow(_brw_19736,(kk_integer_from_small(1)),kk_context()); /*bool*/;
  kk_integer_drop(_brw_19736, _ctx);
  _match_19731 = _brw_19737; /*bool*/
  if (_match_19731) {
    _x20591 = slice0; /*sslice*/
  }
  else {
    kk_integer_t _brw_19733;
    kk_integer_t _x20594;
    kk_std_core_types__optional _x20595 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
    if (kk_std_core_types__is_Optional(_x20595)) {
      kk_box_t _box_x18736 = _x20595._cons.Optional.value;
      kk_integer_t _n_101421 = kk_integer_unbox(_box_x18736);
      _x20594 = _n_101421; /*int*/
    }
    else {
      _x20594 = kk_integer_from_small(1); /*int*/
    }
    _brw_19733 = kk_integer_add_small_const(_x20594, -1, _ctx); /*int*/
    kk_std_core__sslice _brw_19734 = kk_std_core_extend(slice0, _brw_19733, _ctx); /*sslice*/;
    kk_integer_drop(_brw_19733, _ctx);
    _x20591 = _brw_19734; /*sslice*/
  }
  _x20590 = kk_std_core_string_3(_x20591, _ctx); /*string*/
  _x20589 = kk_std_core_to_upper(_x20590, _ctx); /*string*/
  kk_string_t _x20597;
  kk_std_core__sslice _x20598;
  {
    kk_string_t s1 = slice1_17046.str;
    kk_ssize_t start0 = slice1_17046.start;
    kk_ssize_t len0 = slice1_17046.len;
    kk_string_dup(s1);
    kk_std_core__sslice_drop(slice1_17046, _ctx);
    kk_string_t _x20599 = kk_string_dup(s1); /*string*/
    kk_ssize_t _x20600 = (start0 + len0); /*ssize_t*/
    kk_ssize_t _x20601;
    kk_ssize_t _x20602 = kk_string_len(s1,kk_context()); /*ssize_t*/
    kk_ssize_t _x20603 = (start0 + len0); /*ssize_t*/
    _x20601 = (_x20602 - _x20603); /*ssize_t*/
    _x20598 = kk_std_core__new_Sslice(_x20599, _x20600, _x20601, _ctx); /*sslice*/
  }
  _x20597 = kk_std_core_string_3(_x20598, _ctx); /*string*/
  return kk_std_core__lp__plus__plus__1_rp_(_x20589, _x20597, _ctx);
}
 
// Catch any exception raised in `action` and handle it.
// Use `on-exn` or `on-exit` when appropiate.


// lift anonymous function
struct kk_std_core_try_fun20604__t {
  struct kk_function_s _base;
  kk_function_t hndl;
};
static kk_box_t kk_std_core_try_fun20604(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__525__16, kk_std_core__exception x, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun20604(kk_function_t hndl, kk_context_t* _ctx) {
  struct kk_std_core_try_fun20604__t* _self = kk_function_alloc_as(struct kk_std_core_try_fun20604__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_try_fun20604, kk_context());
  _self->hndl = hndl;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_try_fun20605__t {
  struct kk_function_s _base;
  kk_function_t hndl;
  kk_std_core__exception x;
};
static kk_box_t kk_std_core_try_fun20605(kk_function_t _fself, kk_function_t ___wildcard__525__45, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun20605(kk_function_t hndl, kk_std_core__exception x, kk_context_t* _ctx) {
  struct kk_std_core_try_fun20605__t* _self = kk_function_alloc_as(struct kk_std_core_try_fun20605__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_try_fun20605, kk_context());
  _self->hndl = hndl;
  _self->x = x;
  return &_self->_base;
}

static kk_box_t kk_std_core_try_fun20605(kk_function_t _fself, kk_function_t ___wildcard__525__45, kk_context_t* _ctx) {
  struct kk_std_core_try_fun20605__t* _self = kk_function_as(struct kk_std_core_try_fun20605__t*, _fself);
  kk_function_t hndl = _self->hndl; /* (exception) -> 10309 10308 */
  kk_std_core__exception x = _self->x; /* exception */
  kk_drop_match(_self, {kk_function_dup(hndl);kk_std_core__exception_dup(x);}, {}, _ctx)
  kk_function_drop(___wildcard__525__45, _ctx);
  return kk_function_call(kk_box_t, (kk_function_t, kk_std_core__exception, kk_context_t*), hndl, (hndl, x, _ctx));
}
static kk_box_t kk_std_core_try_fun20604(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__525__16, kk_std_core__exception x, kk_context_t* _ctx) {
  struct kk_std_core_try_fun20604__t* _self = kk_function_as(struct kk_std_core_try_fun20604__t*, _fself);
  kk_function_t hndl = _self->hndl; /* (exception) -> 10309 10308 */
  kk_drop_match(_self, {kk_function_dup(hndl);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(___wildcard__525__16, (KK_I32(3)), _ctx);
  return kk_std_core_hnd_yield_to_final(m0, kk_std_core_new_try_fun20605(hndl, x, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_try_fun20608__t {
  struct kk_function_s _base;
  kk_function_t _b_18741_18737;
};
static kk_box_t kk_std_core_try_fun20608(kk_function_t _fself, kk_std_core_hnd__marker _b_18738, kk_std_core_hnd__ev _b_18739, kk_box_t _b_18740, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun20608(kk_function_t _b_18741_18737, kk_context_t* _ctx) {
  struct kk_std_core_try_fun20608__t* _self = kk_function_alloc_as(struct kk_std_core_try_fun20608__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_try_fun20608, kk_context());
  _self->_b_18741_18737 = _b_18741_18737;
  return &_self->_base;
}

static kk_box_t kk_std_core_try_fun20608(kk_function_t _fself, kk_std_core_hnd__marker _b_18738, kk_std_core_hnd__ev _b_18739, kk_box_t _b_18740, kk_context_t* _ctx) {
  struct kk_std_core_try_fun20608__t* _self = kk_function_as(struct kk_std_core_try_fun20608__t*, _fself);
  kk_function_t _b_18741_18737 = _self->_b_18741_18737; /* (m0 : std/core/hnd/marker<10309,10308>, std/core/hnd/ev<.hnd-exn>, x : exception) -> 10309 10295 */
  kk_drop_match(_self, {kk_function_dup(_b_18741_18737);}, {}, _ctx)
  kk_std_core__exception _x20609 = kk_std_core__exception_unbox(_b_18740, _ctx); /*exception*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_std_core__exception, kk_context_t*), _b_18741_18737, (_b_18741_18737, _b_18738, _b_18739, _x20609, _ctx));
}


// lift anonymous function
struct kk_std_core_try_fun20610__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_try_fun20610(kk_function_t _fself, kk_box_t _x, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun20610(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_try_fun20610, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_try_fun20610(kk_function_t _fself, kk_box_t _x, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _x;
}

kk_box_t kk_std_core_try(kk_function_t action, kk_function_t hndl, kk_context_t* _ctx) { /* forall<a,e> (action : () -> <exn|e> a, hndl : (exception) -> e a) -> e a */ 
  kk_function_t _b_18741_18737 = kk_std_core_new_try_fun20604(hndl, _ctx); /*(m0 : std/core/hnd/marker<10309,10308>, std/core/hnd/ev<.hnd-exn>, x : exception) -> 10309 10295*/;
  kk_std_core__hnd_exn _x20606;
  kk_std_core_hnd__clause1 _x20607 = kk_std_core_hnd__new_Clause1(kk_std_core_new_try_fun20608(_b_18741_18737, _ctx), _ctx); /*std/core/hnd/clause1<51,52,53,54,55>*/
  _x20606 = kk_std_core__new_Hnd_exn(kk_reuse_null, _x20607, _ctx); /*.hnd-exn<11,12>*/
  return kk_std_core__handle_exn((KK_I32(0)), _x20606, kk_std_core_new_try_fun20610(_ctx), action, _ctx);
}
 
// Transform an exception effect to an  `:error` type.


// lift anonymous function
struct kk_std_core_try_fun20612__t_1 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_try_fun20612_1(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__525__16, kk_std_core__exception x, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun20612_1(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_try_fun20612_1, _ctx)
  return kk_function_dup(_fself);
}



// lift anonymous function
struct kk_std_core_try_fun20613__t_1 {
  struct kk_function_s _base;
  kk_std_core__exception x;
};
static kk_box_t kk_std_core_try_fun20613_1(kk_function_t _fself, kk_function_t _b_18744, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun20613_1(kk_std_core__exception x, kk_context_t* _ctx) {
  struct kk_std_core_try_fun20613__t_1* _self = kk_function_alloc_as(struct kk_std_core_try_fun20613__t_1, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_try_fun20613_1, kk_context());
  _self->x = x;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_try_fun20614__t_1 {
  struct kk_function_s _base;
  kk_function_t _b_18744;
};
static kk_std_core__error kk_std_core_try_fun20614_1(kk_function_t _fself, kk_std_core_hnd__resume_result _b_18745, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun20614_1(kk_function_t _b_18744, kk_context_t* _ctx) {
  struct kk_std_core_try_fun20614__t_1* _self = kk_function_alloc_as(struct kk_std_core_try_fun20614__t_1, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_try_fun20614_1, kk_context());
  _self->_b_18744 = _b_18744;
  return &_self->_base;
}

static kk_std_core__error kk_std_core_try_fun20614_1(kk_function_t _fself, kk_std_core_hnd__resume_result _b_18745, kk_context_t* _ctx) {
  struct kk_std_core_try_fun20614__t_1* _self = kk_function_as(struct kk_std_core_try_fun20614__t_1*, _fself);
  kk_function_t _b_18744 = _self->_b_18744; /* (std/core/hnd/resume-result<3990,3993>) -> 3992 3993 */
  kk_drop_match(_self, {kk_function_dup(_b_18744);}, {}, _ctx)
  kk_box_t _x20615 = kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__resume_result, kk_context_t*), _b_18744, (_b_18744, _b_18745, _ctx)); /*3993*/
  return kk_std_core__error_unbox(_x20615, _ctx);
}
static kk_box_t kk_std_core_try_fun20613_1(kk_function_t _fself, kk_function_t _b_18744, kk_context_t* _ctx) {
  struct kk_std_core_try_fun20613__t_1* _self = kk_function_as(struct kk_std_core_try_fun20613__t_1*, _fself);
  kk_std_core__exception x = _self->x; /* exception */
  kk_drop_match(_self, {kk_std_core__exception_dup(x);}, {}, _ctx)
  kk_function_t ___wildcard__525__45_18765 = kk_std_core_new_try_fun20614_1(_b_18744, _ctx); /*(std/core/hnd/resume-result<10295,error<10347>>) -> 10348 error<10347>*/;
  kk_function_drop(___wildcard__525__45_18765, _ctx);
  kk_std_core__error _x20616 = kk_std_core__new_Error(x, _ctx); /*error<30>*/
  return kk_std_core__error_box(_x20616, _ctx);
}
static kk_box_t kk_std_core_try_fun20612_1(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__525__16, kk_std_core__exception x, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_std_core_hnd__ev_dropn(___wildcard__525__16, (KK_I32(3)), _ctx);
  return kk_std_core_hnd_yield_to_final(m0, kk_std_core_new_try_fun20613_1(x, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_try_fun20619__t_1 {
  struct kk_function_s _base;
  kk_function_t _b_18761_18746;
};
static kk_box_t kk_std_core_try_fun20619_1(kk_function_t _fself, kk_std_core_hnd__marker _b_18747, kk_std_core_hnd__ev _b_18748, kk_box_t _b_18749, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun20619_1(kk_function_t _b_18761_18746, kk_context_t* _ctx) {
  struct kk_std_core_try_fun20619__t_1* _self = kk_function_alloc_as(struct kk_std_core_try_fun20619__t_1, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_try_fun20619_1, kk_context());
  _self->_b_18761_18746 = _b_18761_18746;
  return &_self->_base;
}

static kk_box_t kk_std_core_try_fun20619_1(kk_function_t _fself, kk_std_core_hnd__marker _b_18747, kk_std_core_hnd__ev _b_18748, kk_box_t _b_18749, kk_context_t* _ctx) {
  struct kk_std_core_try_fun20619__t_1* _self = kk_function_as(struct kk_std_core_try_fun20619__t_1*, _fself);
  kk_function_t _b_18761_18746 = _self->_b_18761_18746; /* (m0 : std/core/hnd/marker<10348,error<10347>>, std/core/hnd/ev<.hnd-exn>, x : exception) -> 10348 10295 */
  kk_drop_match(_self, {kk_function_dup(_b_18761_18746);}, {}, _ctx)
  kk_std_core__exception _x20620 = kk_std_core__exception_unbox(_b_18749, _ctx); /*exception*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_std_core__exception, kk_context_t*), _b_18761_18746, (_b_18761_18746, _b_18747, _b_18748, _x20620, _ctx));
}


// lift anonymous function
struct kk_std_core_try_fun20621__t_1 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_try_fun20621_1(kk_function_t _fself, kk_box_t _b_18756, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun20621_1(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_try_fun20621_1, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_try_fun20621_1(kk_function_t _fself, kk_box_t _b_18756, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_std_core__error _x_18766 = kk_std_core__error_unbox(_b_18756, _ctx); /*error<10347>*/;
  return kk_std_core__error_box(_x_18766, _ctx);
}


// lift anonymous function
struct kk_std_core_try_fun20622__t_1 {
  struct kk_function_s _base;
  kk_function_t action;
};
static kk_box_t kk_std_core_try_fun20622_1(kk_function_t _fself, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun20622_1(kk_function_t action, kk_context_t* _ctx) {
  struct kk_std_core_try_fun20622__t_1* _self = kk_function_alloc_as(struct kk_std_core_try_fun20622__t_1, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_try_fun20622_1, kk_context());
  _self->action = action;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_try_fun20625__t_1 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_try_fun20625_1(kk_function_t _fself, kk_box_t _b_18751, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun20625_1(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_try_fun20625_1, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_try_fun20625_1(kk_function_t _fself, kk_box_t _b_18751, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_box_t _y_18767_17426 = _b_18751; /*10347*/;
  kk_std_core__error _x20626 = kk_std_core__new_Ok(_y_18767_17426, _ctx); /*error<30>*/
  return kk_std_core__error_box(_x20626, _ctx);
}
static kk_box_t kk_std_core_try_fun20622_1(kk_function_t _fself, kk_context_t* _ctx) {
  struct kk_std_core_try_fun20622__t_1* _self = kk_function_as(struct kk_std_core_try_fun20622__t_1*, _fself);
  kk_function_t action = _self->action; /* () -> <exn|10348> 10347 */
  kk_drop_match(_self, {kk_function_dup(action);}, {}, _ctx)
  kk_box_t x0_17915 = kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), action, (action, _ctx)); /*10347*/;
  kk_std_core__error _x20623;
  if (kk_yielding(kk_context())) {
    kk_box_drop(x0_17915, _ctx);
    kk_box_t _x20624 = kk_std_core_hnd_yield_extend(kk_std_core_new_try_fun20625_1(_ctx), _ctx); /*3926*/
    _x20623 = kk_std_core__error_unbox(_x20624, _ctx); /*error<10347>*/
  }
  else {
    _x20623 = kk_std_core__new_Ok(x0_17915, _ctx); /*error<10347>*/
  }
  return kk_std_core__error_box(_x20623, _ctx);
}

kk_std_core__error kk_std_core_try_1(kk_function_t action, kk_context_t* _ctx) { /* forall<a,e> (action : () -> <exn|e> a) -> e error<a> */ 
  int32_t _b_18757_18752 = (KK_I32(0)); /*int32*/;
  kk_box_t _x20611;
  kk_function_t _b_18761_18746 = kk_std_core_new_try_fun20612_1(_ctx); /*(m0 : std/core/hnd/marker<10348,error<10347>>, std/core/hnd/ev<.hnd-exn>, x : exception) -> 10348 10295*/;
  kk_std_core__hnd_exn _x20617;
  kk_std_core_hnd__clause1 _x20618 = kk_std_core_hnd__new_Clause1(kk_std_core_new_try_fun20619_1(_b_18761_18746, _ctx), _ctx); /*std/core/hnd/clause1<51,52,53,54,55>*/
  _x20617 = kk_std_core__new_Hnd_exn(kk_reuse_null, _x20618, _ctx); /*.hnd-exn<11,12>*/
  _x20611 = kk_std_core__handle_exn(_b_18757_18752, _x20617, kk_std_core_new_try_fun20621_1(_ctx), kk_std_core_new_try_fun20622_1(action, _ctx), _ctx); /*1969*/
  return kk_std_core__error_unbox(_x20611, _ctx);
}

kk_std_core_types__tuple2_ kk_std_core_cdivmod_exp10(kk_integer_t i, kk_integer_t n, kk_context_t* _ctx) { /* (i : int, n : int) -> (int, int) */ 
  bool _match_19729 = kk_integer_lte_borrow(n,(kk_integer_from_small(0)),kk_context()); /*bool*/;
  if (_match_19729) {
    kk_integer_drop(n, _ctx);
    return kk_std_core_types__new_dash__lp__comma__rp_(kk_integer_box(i), kk_integer_box(kk_integer_from_small(0)), _ctx);
  }
  {
    kk_integer_t cq;
    kk_integer_t _x20627 = kk_integer_dup(i); /*int*/
    kk_integer_t _x20628 = kk_integer_dup(n); /*int*/
    cq = kk_std_core_cdiv_exp10(_x20627, _x20628, _ctx); /*int*/
    kk_integer_t y_17054;
    kk_integer_t _x20629 = kk_integer_dup(cq); /*int*/
    y_17054 = kk_std_core_mul_exp10(_x20629, n, _ctx); /*int*/
    kk_integer_t cr = kk_integer_sub(i,y_17054,kk_context()); /*int*/;
    return kk_std_core_types__new_dash__lp__comma__rp_(kk_integer_box(cq), kk_integer_box(cr), _ctx);
  }
}
 
// Concatenate a list of `:maybe` values

kk_std_core__list kk_std_core__ctail_concat_maybe(kk_std_core__list xs, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* forall<a> (xs : list<maybe<a>>, ctail<list<a>>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con20630 = kk_std_core__as_Cons(xs);
    kk_box_t _box_x18776 = _con20630->head;
    kk_std_core__list xx = _con20630->tail;
    kk_std_core_types__maybe x = kk_std_core_types__maybe_unbox(_box_x18776, NULL);
    kk_reuse_t _ru_19924 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core_types__maybe_dup(x);
      kk_box_drop(_box_x18776, _ctx);
      _ru_19924 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_std_core_types__maybe_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    if (kk_std_core_types__is_Just(x)) {
      kk_box_t y = x._cons.Just.value;
      kk_std_core__list _ctail_17274 = kk_std_core__list_hole(); /*list<10487>*/;
      kk_std_core__list _ctail_17275 = kk_std_core__new_Cons(_ru_19924, y, _ctail_17274, _ctx); /*list<10487>*/;
      kk_box_t* _b_18787_18782 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17275)->tail)); /*cfield<list<10487>>*/;
      { // tailcall
        kk_std_core_types__ctail _x20632 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17275, _ctx)),_b_18787_18782); /*ctail<0>*/
        xs = xx;
        _acc = _x20632;
        goto kk__tailcall;
      }
    }
    {
      kk_reuse_drop(_ru_19924,kk_context());
      { // tailcall
        xs = xx;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_box_t _x20633 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20633, _ctx);
  }
}
 
// Concatenate a list of `:maybe` values

kk_std_core__list kk_std_core_concat_maybe(kk_std_core__list xs0, kk_context_t* _ctx) { /* forall<a> (xs : list<maybe<a>>) -> list<a> */ 
  kk_std_core_types__ctail _x20634 = kk_ctail_nil(); /*ctail<0>*/
  return kk_std_core__ctail_concat_maybe(xs0, _x20634, _ctx);
}
 
// monadic lift

kk_std_core_types__maybe kk_std_core__mlift17699_op(kk_function_t action, kk_ssize_t end, kk_ssize_t i, kk_std_core_types__maybe _y_17429, kk_context_t* _ctx) { /* forall<a,e> (action : (ssize_t) -> e maybe<a>, end : ssize_t, i : ssize_t, maybe<a>) -> e maybe<a> */ 
  if (kk_std_core_types__is_Nothing(_y_17429)) {
    kk_ssize_t i0_17224 = kk_std_core_incr_1(i, _ctx); /*ssize_t*/;
    return kk_std_core__lift17194_for_whilez(action, end, i0_17224, _ctx);
  }
  {
    kk_box_t x = _y_17429._cons.Just.value;
    kk_function_drop(action, _ctx);
    return kk_std_core_types__new_Just(x, _ctx);
  }
}
 
// lifted local: for-whilez, rep


// lift anonymous function
struct kk_std_core__lift17194_for_whilez_fun20637__t {
  struct kk_function_s _base;
  kk_function_t action0;
  kk_ssize_t end0;
  kk_ssize_t i0;
};
static kk_box_t kk_std_core__lift17194_for_whilez_fun20637(kk_function_t _fself, kk_box_t _b_18794, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17194_for_whilez_fun20637(kk_function_t action0, kk_ssize_t end0, kk_ssize_t i0, kk_context_t* _ctx) {
  struct kk_std_core__lift17194_for_whilez_fun20637__t* _self = kk_function_alloc_as(struct kk_std_core__lift17194_for_whilez_fun20637__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift17194_for_whilez_fun20637, kk_context());
  _self->action0 = action0;
  _self->end0 = end0;
  _self->i0 = i0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift17194_for_whilez_fun20637(kk_function_t _fself, kk_box_t _b_18794, kk_context_t* _ctx) {
  struct kk_std_core__lift17194_for_whilez_fun20637__t* _self = kk_function_as(struct kk_std_core__lift17194_for_whilez_fun20637__t*, _fself);
  kk_function_t action0 = _self->action0; /* (ssize_t) -> 10542 maybe<10541> */
  kk_ssize_t end0 = _self->end0; /* ssize_t */
  kk_ssize_t i0 = _self->i0; /* ssize_t */
  kk_drop_match(_self, {kk_function_dup(action0);;;}, {}, _ctx)
  kk_std_core_types__maybe _y_18796_174290 = kk_std_core_types__maybe_unbox(_b_18794, _ctx); /*maybe<10541>*/;
  kk_std_core_types__maybe _x20638 = kk_std_core__mlift17699_op(action0, end0, i0, _y_18796_174290, _ctx); /*maybe<10541>*/
  return kk_std_core_types__maybe_box(_x20638, _ctx);
}

kk_std_core_types__maybe kk_std_core__lift17194_for_whilez(kk_function_t action0, kk_ssize_t end0, kk_ssize_t i0, kk_context_t* _ctx) { /* forall<a,e> (action : (ssize_t) -> e maybe<a>, end : ssize_t, i : ssize_t) -> e maybe<a> */ 
  kk__tailcall: ;
  bool _match_19727 = (i0 <= end0); /*bool*/;
  if (_match_19727) {
    kk_std_core_types__maybe x0_17918;
    kk_function_t _x20635 = kk_function_dup(action0); /*(ssize_t) -> 10542 maybe<10541>*/
    x0_17918 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_ssize_t, kk_context_t*), _x20635, (_x20635, i0, _ctx)); /*maybe<10541>*/
    if (kk_yielding(kk_context())) {
      kk_std_core_types__maybe_drop(x0_17918, _ctx);
      kk_box_t _x20636 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift17194_for_whilez_fun20637(action0, end0, i0, _ctx), _ctx); /*3926*/
      return kk_std_core_types__maybe_unbox(_x20636, _ctx);
    }
    if (kk_std_core_types__is_Nothing(x0_17918)) {
      kk_ssize_t i0_172240 = kk_std_core_incr_1(i0, _ctx); /*ssize_t*/;
      { // tailcall
        i0 = i0_172240;
        goto kk__tailcall;
      }
    }
    {
      kk_box_t x1 = x0_17918._cons.Just.value;
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

kk_std_core_types__maybe kk_std_core__mlift17700_foreach_while(kk_function_t action, kk_std_core__list xx, kk_std_core_types__maybe _y_17434, kk_context_t* _ctx) { /* forall<a,b,e> (action : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e maybe<b> */ 
  if (kk_std_core_types__is_Nothing(_y_17434)) {
    return kk_std_core_foreach_while(xx, action, _ctx);
  }
  {
    kk_std_core__list_drop(xx, _ctx);
    kk_function_drop(action, _ctx);
    return _y_17434;
  }
}
 
// Invoke `action` for each element of a list while `action` return `Nothing`


// lift anonymous function
struct kk_std_core_foreach_while_fun20645__t {
  struct kk_function_s _base;
  kk_function_t action0;
  kk_std_core__list xx0;
};
static kk_box_t kk_std_core_foreach_while_fun20645(kk_function_t _fself, kk_box_t _b_18798, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_while_fun20645(kk_function_t action0, kk_std_core__list xx0, kk_context_t* _ctx) {
  struct kk_std_core_foreach_while_fun20645__t* _self = kk_function_alloc_as(struct kk_std_core_foreach_while_fun20645__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foreach_while_fun20645, kk_context());
  _self->action0 = action0;
  _self->xx0 = xx0;
  return &_self->_base;
}

static kk_box_t kk_std_core_foreach_while_fun20645(kk_function_t _fself, kk_box_t _b_18798, kk_context_t* _ctx) {
  struct kk_std_core_foreach_while_fun20645__t* _self = kk_function_as(struct kk_std_core_foreach_while_fun20645__t*, _fself);
  kk_function_t action0 = _self->action0; /* (10675) -> 10677 maybe<10676> */
  kk_std_core__list xx0 = _self->xx0; /* list<10675> */
  kk_drop_match(_self, {kk_function_dup(action0);kk_std_core__list_dup(xx0);}, {}, _ctx)
  kk_std_core_types__maybe _y_18800_174340 = kk_std_core_types__maybe_unbox(_b_18798, _ctx); /*maybe<10676>*/;
  kk_std_core_types__maybe _x20646 = kk_std_core__mlift17700_foreach_while(action0, xx0, _y_18800_174340, _ctx); /*maybe<10676>*/
  return kk_std_core_types__maybe_box(_x20646, _ctx);
}

kk_std_core_types__maybe kk_std_core_foreach_while(kk_std_core__list xs, kk_function_t action0, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, action : (a) -> e maybe<b>) -> e maybe<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Nil(xs)) {
    kk_function_drop(action0, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  {
    struct kk_std_core_Cons* _con20642 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con20642->head;
    kk_std_core__list xx0 = _con20642->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_std_core_types__maybe x0_17921;
    kk_function_t _x20643 = kk_function_dup(action0); /*(10675) -> 10677 maybe<10676>*/
    x0_17921 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_box_t, kk_context_t*), _x20643, (_x20643, x, _ctx)); /*maybe<10676>*/
    if (kk_yielding(kk_context())) {
      kk_std_core_types__maybe_drop(x0_17921, _ctx);
      kk_box_t _x20644 = kk_std_core_hnd_yield_extend(kk_std_core_new_foreach_while_fun20645(action0, xx0, _ctx), _ctx); /*3926*/
      return kk_std_core_types__maybe_unbox(_x20644, _ctx);
    }
    if (kk_std_core_types__is_Nothing(x0_17921)) { // tailcall
                                                   xs = xx0;
                                                   goto kk__tailcall;
    }
    {
      kk_std_core__list_drop(xx0, _ctx);
      kk_function_drop(action0, _ctx);
      return x0_17921;
    }
  }
}
 
// monadic lift

kk_std_core_types__maybe kk_std_core__mlift17701_foreach_while_1(kk_function_t action, kk_std_core__sslice rest, kk_std_core_types__maybe _y_17438, kk_context_t* _ctx) { /* forall<a,e> (action : (c : char) -> e maybe<a>, rest : sslice, maybe<a>) -> e maybe<a> */ 
  if (kk_std_core_types__is_Nothing(_y_17438)) {
    return kk_std_core_foreach_while_1(rest, action, _ctx);
  }
  {
    kk_std_core__sslice_drop(rest, _ctx);
    kk_function_drop(action, _ctx);
    return _y_17438;
  }
}
 
// Apply a function for each character in a string slice.
// If `action` returns `Just`, the function returns immediately with that result.


// lift anonymous function
struct kk_std_core_foreach_while_fun20652__t_1 {
  struct kk_function_s _base;
  kk_function_t action0;
  kk_std_core__sslice rest0;
};
static kk_box_t kk_std_core_foreach_while_fun20652_1(kk_function_t _fself, kk_box_t _b_18805, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_while_fun20652_1(kk_function_t action0, kk_std_core__sslice rest0, kk_context_t* _ctx) {
  struct kk_std_core_foreach_while_fun20652__t_1* _self = kk_function_alloc_as(struct kk_std_core_foreach_while_fun20652__t_1, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foreach_while_fun20652_1, kk_context());
  _self->action0 = action0;
  _self->rest0 = rest0;
  return &_self->_base;
}

static kk_box_t kk_std_core_foreach_while_fun20652_1(kk_function_t _fself, kk_box_t _b_18805, kk_context_t* _ctx) {
  struct kk_std_core_foreach_while_fun20652__t_1* _self = kk_function_as(struct kk_std_core_foreach_while_fun20652__t_1*, _fself);
  kk_function_t action0 = _self->action0; /* (c : char) -> 10762 maybe<10761> */
  kk_std_core__sslice rest0 = _self->rest0; /* sslice */
  kk_drop_match(_self, {kk_function_dup(action0);kk_std_core__sslice_dup(rest0);}, {}, _ctx)
  kk_std_core_types__maybe _y_18807_174380 = kk_std_core_types__maybe_unbox(_b_18805, _ctx); /*maybe<10761>*/;
  kk_std_core_types__maybe _x20653 = kk_std_core__mlift17701_foreach_while_1(action0, rest0, _y_18807_174380, _ctx); /*maybe<10761>*/
  return kk_std_core_types__maybe_box(_x20653, _ctx);
}

kk_std_core_types__maybe kk_std_core_foreach_while_1(kk_std_core__sslice slice0, kk_function_t action0, kk_context_t* _ctx) { /* forall<a,e> (slice : sslice, action : (c : char) -> e maybe<a>) -> e maybe<a> */ 
  kk__tailcall: ;
  kk_std_core_types__maybe _match_19724 = kk_std_core_next(slice0, _ctx); /*maybe<(char, sslice)>*/;
  if (kk_std_core_types__is_Nothing(_match_19724)) {
    kk_function_drop(action0, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  {
    kk_box_t _box_x18801 = _match_19724._cons.Just.value;
    kk_std_core_types__tuple2_ _pat1 = kk_std_core_types__tuple2__unbox(_box_x18801, NULL);
    kk_box_t _box_x18802 = _pat1.fst;
    kk_box_t _box_x18803 = _pat1.snd;
    kk_char_t c = kk_char_unbox(_box_x18802, NULL);
    kk_std_core__sslice rest0 = kk_std_core__sslice_unbox(_box_x18803, NULL);
    kk_std_core__sslice_dup(rest0);
    kk_std_core_types__maybe_drop(_match_19724, _ctx);
    kk_std_core_types__maybe x_17924;
    kk_function_t _x20650 = kk_function_dup(action0); /*(c : char) -> 10762 maybe<10761>*/
    x_17924 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_char_t, kk_context_t*), _x20650, (_x20650, c, _ctx)); /*maybe<10761>*/
    if (kk_yielding(kk_context())) {
      kk_std_core_types__maybe_drop(x_17924, _ctx);
      kk_box_t _x20651 = kk_std_core_hnd_yield_extend(kk_std_core_new_foreach_while_fun20652_1(action0, rest0, _ctx), _ctx); /*3926*/
      return kk_std_core_types__maybe_unbox(_x20651, _ctx);
    }
    if (kk_std_core_types__is_Nothing(x_17924)) { // tailcall
                                                  slice0 = rest0;
                                                  goto kk__tailcall;
    }
    {
      kk_std_core__sslice_drop(rest0, _ctx);
      kk_function_drop(action0, _ctx);
      return x_17924;
    }
  }
}
 
// Invoke a function `f` for each element in a vector `v`.
// If `f` returns `Just`, the iteration is stopped early and the result is returned.


// lift anonymous function
struct kk_std_core_foreach_while_fun20659__t_3 {
  struct kk_function_s _base;
  kk_function_t f;
  kk_vector_t v;
};
static kk_std_core_types__maybe kk_std_core_foreach_while_fun20659_3(kk_function_t _fself, kk_ssize_t i, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_while_fun20659_3(kk_function_t f, kk_vector_t v, kk_context_t* _ctx) {
  struct kk_std_core_foreach_while_fun20659__t_3* _self = kk_function_alloc_as(struct kk_std_core_foreach_while_fun20659__t_3, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foreach_while_fun20659_3, kk_context());
  _self->f = f;
  _self->v = v;
  return &_self->_base;
}

static kk_std_core_types__maybe kk_std_core_foreach_while_fun20659_3(kk_function_t _fself, kk_ssize_t i, kk_context_t* _ctx) {
  struct kk_std_core_foreach_while_fun20659__t_3* _self = kk_function_as(struct kk_std_core_foreach_while_fun20659__t_3*, _fself);
  kk_function_t f = _self->f; /* (10860) -> 10862 maybe<10861> */
  kk_vector_t v = _self->v; /* vector<10860> */
  kk_drop_match(_self, {kk_function_dup(f);kk_vector_dup(v);}, {}, _ctx)
  kk_box_t _x20660;
  kk_box_t _brw_19723 = kk_vector_at_borrow(v,i); /*228*/;
  kk_vector_drop(v, _ctx);
  _x20660 = _brw_19723; /*228*/
  return kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_box_t, kk_context_t*), f, (f, _x20660, _ctx));
}

kk_std_core_types__maybe kk_std_core_foreach_while_3(kk_vector_t v, kk_function_t f, kk_context_t* _ctx) { /* forall<a,b,e> (v : vector<a>, f : (a) -> e maybe<b>) -> e maybe<b> */ 
  kk_ssize_t start0_17927 = (KK_IZ(0)); /*ssize_t*/;
  kk_ssize_t end_17928;
  kk_ssize_t _x20658 = kk_vector_len_borrow(v); /*ssize_t*/
  end_17928 = kk_std_core_decr_1(_x20658, _ctx); /*ssize_t*/
  return kk_std_core__lift17194_for_whilez(kk_std_core_new_foreach_while_fun20659_3(f, v, _ctx), end_17928, start0_17927, _ctx);
}
 
// monadic lift

kk_unit_t kk_std_core__mlift17702_foreach(kk_function_t action, kk_std_core__list xx, kk_unit_t wild__, kk_context_t* _ctx) { /* forall<a,e> (action : (a) -> e (), xx : list<a>, wild_ : ()) -> e () */ 
  kk_std_core_foreach(xx, action, _ctx); return kk_Unit;
}
 
// Invoke `action` for each element of a list


// lift anonymous function
struct kk_std_core_foreach_fun20664__t {
  struct kk_function_s _base;
  kk_function_t action0;
  kk_std_core__list xx0;
};
static kk_box_t kk_std_core_foreach_fun20664(kk_function_t _fself, kk_box_t _b_18809, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_fun20664(kk_function_t action0, kk_std_core__list xx0, kk_context_t* _ctx) {
  struct kk_std_core_foreach_fun20664__t* _self = kk_function_alloc_as(struct kk_std_core_foreach_fun20664__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foreach_fun20664, kk_context());
  _self->action0 = action0;
  _self->xx0 = xx0;
  return &_self->_base;
}

static kk_box_t kk_std_core_foreach_fun20664(kk_function_t _fself, kk_box_t _b_18809, kk_context_t* _ctx) {
  struct kk_std_core_foreach_fun20664__t* _self = kk_function_as(struct kk_std_core_foreach_fun20664__t*, _fself);
  kk_function_t action0 = _self->action0; /* (11054) -> 11055 () */
  kk_std_core__list xx0 = _self->xx0; /* list<11054> */
  kk_drop_match(_self, {kk_function_dup(action0);kk_std_core__list_dup(xx0);}, {}, _ctx)
  kk_unit_t wild__0_18811 = kk_Unit;
  kk_unit_unbox(_b_18809);
  kk_unit_t _x20665 = kk_Unit;
  kk_std_core__mlift17702_foreach(action0, xx0, wild__0_18811, _ctx);
  return kk_unit_box(_x20665);
}

kk_unit_t kk_std_core_foreach(kk_std_core__list xs, kk_function_t action0, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, action : (a) -> e ()) -> e () */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con20661 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con20661->head;
    kk_std_core__list xx0 = _con20661->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_unit_t x0_17930 = kk_Unit;
    kk_function_t _x20662 = kk_function_dup(action0); /*(11054) -> 11055 ()*/
    kk_function_call(kk_unit_t, (kk_function_t, kk_box_t, kk_context_t*), _x20662, (_x20662, x, _ctx));
    if (kk_yielding(kk_context())) {
      kk_box_t _x20663 = kk_std_core_hnd_yield_extend(kk_std_core_new_foreach_fun20664(action0, xx0, _ctx), _ctx); /*3926*/
      kk_unit_unbox(_x20663); return kk_Unit;
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
 
// Apply a function for each character in a string slice.


// lift anonymous function
struct kk_std_core_foreach_fun20666__t_1 {
  struct kk_function_s _base;
  kk_function_t action;
};
static kk_std_core_types__maybe kk_std_core_foreach_fun20666_1(kk_function_t _fself, kk_char_t c, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_fun20666_1(kk_function_t action, kk_context_t* _ctx) {
  struct kk_std_core_foreach_fun20666__t_1* _self = kk_function_alloc_as(struct kk_std_core_foreach_fun20666__t_1, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foreach_fun20666_1, kk_context());
  _self->action = action;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_foreach_fun20668__t_1 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_foreach_fun20668_1(kk_function_t _fself, kk_box_t _b_18813, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_fun20668_1(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_foreach_fun20668_1, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_foreach_fun20668_1(kk_function_t _fself, kk_box_t _b_18813, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_box_drop(_b_18813, _ctx);
  return kk_std_core_types__maybe_box(kk_std_core_types__new_Nothing(_ctx), _ctx);
}
static kk_std_core_types__maybe kk_std_core_foreach_fun20666_1(kk_function_t _fself, kk_char_t c, kk_context_t* _ctx) {
  struct kk_std_core_foreach_fun20666__t_1* _self = kk_function_as(struct kk_std_core_foreach_fun20666__t_1*, _fself);
  kk_function_t action = _self->action; /* (c : char) -> 11113 () */
  kk_drop_match(_self, {kk_function_dup(action);}, {}, _ctx)
  kk_unit_t x0_17936 = kk_Unit;
  kk_function_call(kk_unit_t, (kk_function_t, kk_char_t, kk_context_t*), action, (action, c, _ctx));
  if (kk_yielding(kk_context())) {
    kk_box_t _x20667 = kk_std_core_hnd_yield_extend(kk_std_core_new_foreach_fun20668_1(_ctx), _ctx); /*3926*/
    return kk_std_core_types__maybe_unbox(_x20667, _ctx);
  }
  {
    return kk_std_core_types__new_Nothing(_ctx);
  }
}


// lift anonymous function
struct kk_std_core_foreach_fun20670__t_1 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_foreach_fun20670_1(kk_function_t _fself, kk_box_t _b_18817, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_fun20670_1(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_foreach_fun20670_1, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_foreach_fun20670_1(kk_function_t _fself, kk_box_t _b_18817, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_std_core_types__maybe wild__00_18819 = kk_std_core_types__maybe_unbox(_b_18817, _ctx); /*maybe<_11100>*/;
  kk_std_core_types__maybe_drop(wild__00_18819, _ctx);
  return kk_unit_box(kk_Unit);
}

kk_unit_t kk_std_core_foreach_1(kk_std_core__sslice slice0, kk_function_t action, kk_context_t* _ctx) { /* forall<e> (slice : sslice, action : (c : char) -> e ()) -> e () */ 
  kk_std_core_types__maybe x_17933 = kk_std_core_foreach_while_1(slice0, kk_std_core_new_foreach_fun20666_1(action, _ctx), _ctx); /*maybe<_11100>*/;
  kk_std_core_types__maybe_drop(x_17933, _ctx);
  if (kk_yielding(kk_context())) {
    kk_box_t _x20669 = kk_std_core_hnd_yield_extend(kk_std_core_new_foreach_fun20670_1(_ctx), _ctx); /*3926*/
    kk_unit_unbox(_x20669); return kk_Unit;
  }
  {
    kk_Unit; return kk_Unit;
  }
}
 
// Invoke a function for each character in a string


// lift anonymous function
struct kk_std_core_foreach_fun20674__t_2 {
  struct kk_function_s _base;
  kk_function_t action;
};
static kk_std_core_types__maybe kk_std_core_foreach_fun20674_2(kk_function_t _fself, kk_char_t c, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_fun20674_2(kk_function_t action, kk_context_t* _ctx) {
  struct kk_std_core_foreach_fun20674__t_2* _self = kk_function_alloc_as(struct kk_std_core_foreach_fun20674__t_2, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foreach_fun20674_2, kk_context());
  _self->action = action;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_foreach_fun20676__t_2 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_foreach_fun20676_2(kk_function_t _fself, kk_box_t _b_18821, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_fun20676_2(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_foreach_fun20676_2, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_foreach_fun20676_2(kk_function_t _fself, kk_box_t _b_18821, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_box_drop(_b_18821, _ctx);
  return kk_std_core_types__maybe_box(kk_std_core_types__new_Nothing(_ctx), _ctx);
}
static kk_std_core_types__maybe kk_std_core_foreach_fun20674_2(kk_function_t _fself, kk_char_t c, kk_context_t* _ctx) {
  struct kk_std_core_foreach_fun20674__t_2* _self = kk_function_as(struct kk_std_core_foreach_fun20674__t_2*, _fself);
  kk_function_t action = _self->action; /* (c : char) -> 11152 () */
  kk_drop_match(_self, {kk_function_dup(action);}, {}, _ctx)
  kk_unit_t x0_17943 = kk_Unit;
  kk_function_call(kk_unit_t, (kk_function_t, kk_char_t, kk_context_t*), action, (action, c, _ctx));
  if (kk_yielding(kk_context())) {
    kk_box_t _x20675 = kk_std_core_hnd_yield_extend(kk_std_core_new_foreach_fun20676_2(_ctx), _ctx); /*3926*/
    return kk_std_core_types__maybe_unbox(_x20675, _ctx);
  }
  {
    return kk_std_core_types__new_Nothing(_ctx);
  }
}


// lift anonymous function
struct kk_std_core_foreach_fun20678__t_2 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_foreach_fun20678_2(kk_function_t _fself, kk_box_t _b_18825, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_fun20678_2(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_foreach_fun20678_2, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_foreach_fun20678_2(kk_function_t _fself, kk_box_t _b_18825, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_std_core_types__maybe wild__00_18827 = kk_std_core_types__maybe_unbox(_b_18825, _ctx); /*maybe<_11100>*/;
  kk_std_core_types__maybe_drop(wild__00_18827, _ctx);
  return kk_unit_box(kk_Unit);
}

kk_unit_t kk_std_core_foreach_2(kk_string_t s, kk_function_t action, kk_context_t* _ctx) { /* forall<e> (s : string, action : (c : char) -> e ()) -> e () */ 
  kk_std_core__sslice slice0_17055;
  kk_string_t _x20671 = kk_string_dup(s); /*string*/
  kk_ssize_t _x20672 = (KK_IZ(0)); /*ssize_t*/
  kk_ssize_t _x20673 = kk_string_len(s,kk_context()); /*ssize_t*/
  slice0_17055 = kk_std_core__new_Sslice(_x20671, _x20672, _x20673, _ctx); /*sslice*/
  kk_std_core_types__maybe x_17940 = kk_std_core_foreach_while_1(slice0_17055, kk_std_core_new_foreach_fun20674_2(action, _ctx), _ctx); /*maybe<_11100>*/;
  kk_std_core_types__maybe_drop(x_17940, _ctx);
  if (kk_yielding(kk_context())) {
    kk_box_t _x20677 = kk_std_core_hnd_yield_extend(kk_std_core_new_foreach_fun20678_2(_ctx), _ctx); /*3926*/
    kk_unit_unbox(_x20677); return kk_Unit;
  }
  {
    kk_Unit; return kk_Unit;
  }
}
 
// Invoke a function `f` for each element in a vector `v`


// lift anonymous function
struct kk_std_core_foreach_fun20680__t_3 {
  struct kk_function_s _base;
  kk_function_t f;
  kk_vector_t v;
};
static kk_unit_t kk_std_core_foreach_fun20680_3(kk_function_t _fself, kk_ssize_t i, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_fun20680_3(kk_function_t f, kk_vector_t v, kk_context_t* _ctx) {
  struct kk_std_core_foreach_fun20680__t_3* _self = kk_function_alloc_as(struct kk_std_core_foreach_fun20680__t_3, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foreach_fun20680_3, kk_context());
  _self->f = f;
  _self->v = v;
  return &_self->_base;
}

static kk_unit_t kk_std_core_foreach_fun20680_3(kk_function_t _fself, kk_ssize_t i, kk_context_t* _ctx) {
  struct kk_std_core_foreach_fun20680__t_3* _self = kk_function_as(struct kk_std_core_foreach_fun20680__t_3*, _fself);
  kk_function_t f = _self->f; /* (11167) -> 11168 () */
  kk_vector_t v = _self->v; /* vector<11167> */
  kk_drop_match(_self, {kk_function_dup(f);kk_vector_dup(v);}, {}, _ctx)
  kk_box_t x_18129;
  kk_box_t _brw_19717 = kk_vector_at_borrow(v,i); /*228*/;
  kk_vector_drop(v, _ctx);
  x_18129 = _brw_19717; /*11167*/
  return kk_function_call(kk_unit_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, x_18129, _ctx));
}

kk_unit_t kk_std_core_foreach_3(kk_vector_t v, kk_function_t f, kk_context_t* _ctx) { /* forall<a,e> (v : vector<a>, f : (a) -> e ()) -> e () */ 
  kk_ssize_t start0_17949 = (KK_IZ(0)); /*ssize_t*/;
  kk_ssize_t end_17950;
  kk_ssize_t _x20679 = kk_vector_len_borrow(v); /*ssize_t*/
  end_17950 = kk_std_core_decr_1(_x20679, _ctx); /*ssize_t*/
  kk_std_core__lift17190_forz(kk_std_core_new_foreach_fun20680_3(f, v, _ctx), end_17950, start0_17949, _ctx); return kk_Unit;
}
 
// O(n). Return the number of characters in a string.

kk_integer_t kk_std_core_count_1(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> int */ 
  return kk_string_count_int(s,kk_context());
}
 
// O(n). Return the number of characters in a string slice

kk_integer_t kk_std_core_count_2(kk_std_core__sslice slice0, kk_context_t* _ctx) { /* (slice : sslice) -> int */ 
  return kk_slice_count(slice0,kk_context());
}
 
// Count the number of times a predicate is true for each character in a string


// lift anonymous function
struct kk_std_core_count_fun20685__t_3 {
  struct kk_function_s _base;
  kk_ref_t loc;
  kk_function_t pred;
};
static kk_std_core_types__maybe kk_std_core_count_fun20685_3(kk_function_t _fself, kk_char_t c, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_count_fun20685_3(kk_ref_t loc, kk_function_t pred, kk_context_t* _ctx) {
  struct kk_std_core_count_fun20685__t_3* _self = kk_function_alloc_as(struct kk_std_core_count_fun20685__t_3, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_count_fun20685_3, kk_context());
  _self->loc = loc;
  _self->pred = pred;
  return &_self->_base;
}

static kk_std_core_types__maybe kk_std_core_count_fun20685_3(kk_function_t _fself, kk_char_t c, kk_context_t* _ctx) {
  struct kk_std_core_count_fun20685__t_3* _self = kk_function_as(struct kk_std_core_count_fun20685__t_3*, _fself);
  kk_ref_t loc = _self->loc; /* local-var<11361,int> */
  kk_function_t pred = _self->pred; /* (char) -> bool */
  kk_drop_match(_self, {kk_ref_dup(loc);kk_function_dup(pred);}, {}, _ctx)
  kk_unit_t __ = kk_Unit;
  bool _match_19715 = kk_function_call(bool, (kk_function_t, kk_char_t, kk_context_t*), pred, (pred, c, _ctx)); /*bool*/;
  if (_match_19715) {
    kk_integer_t x_17060;
    kk_box_t _x20686;
    kk_ref_t _x20687 = kk_ref_dup(loc); /*local-var<11361,int>*/
    _x20686 = kk_ref_get(_x20687,kk_context()); /*1000*/
    x_17060 = kk_integer_unbox(_x20686); /*int*/
    kk_integer_t _b_18835_18833 = kk_integer_add_small_const(x_17060, 1, _ctx); /*int*/;
    kk_unit_t _brw_19716 = kk_Unit;
    kk_ref_set_borrow(loc,(kk_integer_box(_b_18835_18833)),kk_context());
    kk_ref_drop(loc, _ctx);
    _brw_19716;
  }
  else {
    kk_ref_drop(loc, _ctx);
  }
  return kk_std_core_types__new_Nothing(_ctx);
}

kk_integer_t kk_std_core_count_3(kk_string_t s, kk_function_t pred, kk_context_t* _ctx) { /* (s : string, pred : (char) -> bool) -> int */ 
  kk_ref_t loc = kk_ref_alloc((kk_integer_box(kk_integer_from_small(0))),kk_context()); /*local-var<11361,int>*/;
  kk_std_core__sslice slice0_17062;
  kk_string_t _x20681 = kk_string_dup(s); /*string*/
  kk_ssize_t _x20682 = (KK_IZ(0)); /*ssize_t*/
  kk_ssize_t _x20683 = kk_string_len(s,kk_context()); /*ssize_t*/
  slice0_17062 = kk_std_core__new_Sslice(_x20681, _x20682, _x20683, _ctx); /*sslice*/
  kk_std_core_types__maybe __0;
  kk_function_t _x20684;
  kk_ref_dup(loc);
  _x20684 = kk_std_core_new_count_fun20685_3(loc, pred, _ctx); /*(c : char) -> (local<11361>) (forall<a> maybe<a>)*/
  __0 = kk_std_core_foreach_while_1(slice0_17062, _x20684, _ctx); /*maybe<_11100>*/
  kk_std_core_types__maybe_drop(__0, _ctx);
  kk_integer_t res;
  kk_box_t _x20688;
  kk_ref_t _x20689 = kk_ref_dup(loc); /*local-var<11361,int>*/
  _x20688 = kk_ref_get(_x20689,kk_context()); /*1000*/
  res = kk_integer_unbox(_x20688); /*int*/
  kk_box_t _x20690 = kk_std_core_hnd_prompt_local_var(loc, kk_integer_box(res), _ctx); /*10459*/
  return kk_integer_unbox(_x20690);
}
 
// Is the integer negative (stricly smaller than zero)

bool kk_std_core_is_neg_2(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> bool */ 
  kk_std_core_types__order x_17067;
  kk_std_core_types__order _brw_19714 = kk_int_as_order(kk_integer_signum_borrow(i),kk_context()); /*order*/;
  kk_integer_drop(i, _ctx);
  x_17067 = _brw_19714; /*order*/
  kk_integer_t _brw_19712;
  if (kk_std_core_types__is_Lt(x_17067)) {
    _brw_19712 = kk_integer_from_small(-1); /*int*/
    goto _match20694;
  }
  if (kk_std_core_types__is_Eq(x_17067)) {
    _brw_19712 = kk_integer_from_small(0); /*int*/
    goto _match20694;
  }
  {
    _brw_19712 = kk_integer_from_small(1); /*int*/
  }
  _match20694: ;
  bool _brw_19713 = kk_integer_eq_borrow(_brw_19712,(kk_integer_from_small(-1)),kk_context()); /*bool*/;
  kk_integer_drop(_brw_19712, _ctx);
  return _brw_19713;
}

kk_std_core_types__tuple2_ kk_std_core_divmod_exp10(kk_integer_t i, kk_integer_t n, kk_context_t* _ctx) { /* (i : int, n : int) -> (int, int) */ 
  kk_std_core_types__tuple2_ _match_19709;
  kk_integer_t _x20695 = kk_integer_dup(n); /*int*/
  _match_19709 = kk_std_core_cdivmod_exp10(i, _x20695, _ctx); /*(int, int)*/
  {
    kk_box_t _box_x18847 = _match_19709.fst;
    kk_box_t _box_x18848 = _match_19709.snd;
    kk_integer_t cq = kk_integer_unbox(_box_x18847);
    kk_integer_t cr = kk_integer_unbox(_box_x18848);
    kk_integer_dup(cq);
    kk_integer_dup(cr);
    kk_std_core_types__tuple2__drop(_match_19709, _ctx);
    kk_std_core_types__order x_17067 = kk_int_as_order(kk_integer_signum_borrow(cr),kk_context()); /*order*/;
    bool b_17071;
    kk_integer_t _brw_19710;
    if (kk_std_core_types__is_Lt(x_17067)) {
      _brw_19710 = kk_integer_from_small(-1); /*int*/
      goto _match20698;
    }
    if (kk_std_core_types__is_Eq(x_17067)) {
      _brw_19710 = kk_integer_from_small(0); /*int*/
      goto _match20698;
    }
    {
      _brw_19710 = kk_integer_from_small(1); /*int*/
    }
    _match20698: ;
    bool _brw_19711 = kk_integer_eq_borrow(_brw_19710,(kk_integer_from_small(-1)),kk_context()); /*bool*/;
    kk_integer_drop(_brw_19710, _ctx);
    b_17071 = _brw_19711; /*bool*/
    if (b_17071) {
      kk_integer_t y0_17076 = kk_std_core_mul_exp10(kk_integer_from_small(1), n, _ctx); /*int*/;
      kk_integer_t _b_18853_18849 = kk_integer_add_small_const(cq, -1, _ctx); /*int*/;
      kk_integer_t _b_18854_18850 = kk_integer_add(cr,y0_17076,kk_context()); /*int*/;
      return kk_std_core_types__new_dash__lp__comma__rp_(kk_integer_box(_b_18853_18849), kk_integer_box(_b_18854_18850), _ctx);
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
    struct kk_std_core_Cons* _con20699 = kk_std_core__as_Cons(xs);
    kk_box_t _pat0 = _con20699->head;
    kk_std_core__list xx = _con20699->tail;
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
        kk_integer_t _x20700 = kk_integer_add_small_const(n, -1, _ctx); /*int*/
        xs = xx;
        n = _x20700;
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

kk_std_core__list kk_std_core__mlift17707_drop_while(kk_function_t predicate, kk_std_core__list xs, kk_std_core__list xx, bool _y_17454, kk_context_t* _ctx) { /* forall<a,e> (predicate : (a) -> e bool, xs : list<a>, xx : list<a>, bool) -> e list<a> */ 
  if (_y_17454) {
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
struct kk_std_core_drop_while_fun20704__t {
  struct kk_function_s _base;
  kk_function_t predicate0;
  kk_std_core__list xs0;
  kk_std_core__list xx0;
};
static kk_box_t kk_std_core_drop_while_fun20704(kk_function_t _fself, kk_box_t _b_18858, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_drop_while_fun20704(kk_function_t predicate0, kk_std_core__list xs0, kk_std_core__list xx0, kk_context_t* _ctx) {
  struct kk_std_core_drop_while_fun20704__t* _self = kk_function_alloc_as(struct kk_std_core_drop_while_fun20704__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_drop_while_fun20704, kk_context());
  _self->predicate0 = predicate0;
  _self->xs0 = xs0;
  _self->xx0 = xx0;
  return &_self->_base;
}

static kk_box_t kk_std_core_drop_while_fun20704(kk_function_t _fself, kk_box_t _b_18858, kk_context_t* _ctx) {
  struct kk_std_core_drop_while_fun20704__t* _self = kk_function_as(struct kk_std_core_drop_while_fun20704__t*, _fself);
  kk_function_t predicate0 = _self->predicate0; /* (11629) -> 11630 bool */
  kk_std_core__list xs0 = _self->xs0; /* list<11629> */
  kk_std_core__list xx0 = _self->xx0; /* list<11629> */
  kk_drop_match(_self, {kk_function_dup(predicate0);kk_std_core__list_dup(xs0);kk_std_core__list_dup(xx0);}, {}, _ctx)
  bool _y_18860_174540 = kk_bool_unbox(_b_18858); /*bool*/;
  kk_std_core__list _x20705 = kk_std_core__mlift17707_drop_while(predicate0, xs0, xx0, _y_18860_174540, _ctx); /*list<11629>*/
  return kk_std_core__list_box(_x20705, _ctx);
}

kk_std_core__list kk_std_core_drop_while(kk_std_core__list xs0, kk_function_t predicate0, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, predicate : (a) -> e bool) -> e list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con20701 = kk_std_core__as_Cons(xs0);
    kk_box_t x = _con20701->head;
    kk_std_core__list xx0 = _con20701->tail;
    kk_box_dup(x);
    kk_std_core__list_dup(xx0);
    bool x0_17954;
    kk_function_t _x20702 = kk_function_dup(predicate0); /*(11629) -> 11630 bool*/
    x0_17954 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x20702, (_x20702, x, _ctx)); /*bool*/
    if (kk_yielding(kk_context())) {
      kk_box_t _x20703 = kk_std_core_hnd_yield_extend(kk_std_core_new_drop_while_fun20704(predicate0, xs0, xx0, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20703, _ctx);
    }
    if (x0_17954) {
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
  bool _match_19707;
  kk_string_t _x20710 = kk_string_dup(s); /*string*/
  kk_string_t _x20711 = kk_string_dup(post); /*string*/
  _match_19707 = kk_std_core_xends_with(_x20710, _x20711, _ctx); /*bool*/
  if (_match_19707) {
    kk_std_core__sslice _b_18862_18861;
    kk_string_t _x20712 = kk_string_dup(s); /*string*/
    kk_ssize_t _x20713 = (KK_IZ(0)); /*ssize_t*/
    kk_ssize_t _x20714;
    kk_ssize_t _x20715 = kk_string_len(s,kk_context()); /*ssize_t*/
    kk_ssize_t _x20716 = kk_string_len(post,kk_context()); /*ssize_t*/
    _x20714 = (_x20715 - _x20716); /*ssize_t*/
    _b_18862_18861 = kk_std_core__new_Sslice(_x20712, _x20713, _x20714, _ctx); /*sslice*/
    return kk_std_core_types__new_Just(kk_std_core__sslice_box(_b_18862_18861, _ctx), _ctx);
  }
  {
    kk_string_drop(s, _ctx);
    kk_string_drop(post, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
}
 
// Throw an exception with a specified message.

kk_box_t kk_std_core_throw(kk_string_t message0, kk_std_core_types__optional info0, kk_context_t* _ctx) { /* forall<a> (message : string, info : optional<exception-info>) -> exn a */ 
  kk_std_core_hnd__ev ev_17957;
  kk_ssize_t _x20717 = (KK_IZ(0)); /*ssize_t*/
  ev_17957 = kk_evv_at(_x20717,kk_context()); /*std/core/hnd/ev<.hnd-exn>*/
  {
    struct kk_std_core_hnd_Ev* _con20718 = kk_std_core_hnd__as_Ev(ev_17957);
    kk_std_core_hnd__marker m0 = _con20718->marker;
    kk_box_t _box_x18863 = _con20718->hnd;
    kk_std_core__hnd_exn h = kk_std_core__hnd_exn_unbox(_box_x18863, NULL);
    kk_std_core__hnd_exn_dup(h);
    kk_std_core_hnd__clause1 _match_19705;
    kk_std_core_hnd__clause1 _brw_19706 = kk_std_core__select_throw_exn(h, _ctx); /*std/core/hnd/clause1<exception,1938,.hnd-exn,1939,1940>*/;
    kk_std_core__hnd_exn_drop(h, _ctx);
    _match_19705 = _brw_19706; /*std/core/hnd/clause1<exception,1938,.hnd-exn,1939,1940>*/
    {
      kk_function_t _fun_unbox_x18867 = _match_19705.clause;
      kk_box_t _x20720;
      kk_std_core__exception _x20721;
      kk_std_core__exception_info _x20722;
      if (kk_std_core_types__is_Optional(info0)) {
        kk_box_t _box_x18871 = info0._cons.Optional.value;
        kk_std_core__exception_info _info_11715 = kk_std_core__exception_info_unbox(_box_x18871, NULL);
        _x20722 = _info_11715; /*exception-info*/
      }
      else {
        _x20722 = kk_std_core__new_ExnError(_ctx); /*exception-info*/
      }
      _x20721 = kk_std_core__new_Exception(message0, _x20722, _ctx); /*exception*/
      _x20720 = kk_std_core__exception_box(_x20721, _ctx); /*51*/
      return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x18867, (_fun_unbox_x18867, m0, ev_17957, _x20720, _ctx));
    }
  }
}
 
// monadic lift

kk_box_t kk_std_core__mlift17708_error_pattern(kk_string_t definition, kk_string_t location, kk_string_t _c_17459, kk_context_t* _ctx) { /* forall<a> (definition : string, location : string, string) -> a */ 
  kk_string_t message0_17081;
  kk_string_t _x20724 = kk_string_dup(location); /*string*/
  kk_string_t _x20725;
  kk_string_t _x20726;
  kk_define_string_literal(, _s20727, 23, ": pattern match failure")
  _x20726 = kk_string_dup(_s20727); /*string*/
  _x20725 = kk_std_core__lp__plus__plus__1_rp_(_c_17459, _x20726, _ctx); /*string*/
  message0_17081 = kk_std_core__lp__plus__plus__1_rp_(_x20724, _x20725, _ctx); /*string*/
  kk_std_core_hnd__ev ev_17960;
  kk_ssize_t _x20728 = (KK_IZ(0)); /*ssize_t*/
  ev_17960 = kk_evv_at(_x20728,kk_context()); /*std/core/hnd/ev<.hnd-exn>*/
  {
    struct kk_std_core_hnd_Ev* _con20729 = kk_std_core_hnd__as_Ev(ev_17960);
    kk_std_core_hnd__marker m0 = _con20729->marker;
    kk_box_t _box_x18872 = _con20729->hnd;
    kk_std_core__hnd_exn h = kk_std_core__hnd_exn_unbox(_box_x18872, NULL);
    kk_std_core__hnd_exn_dup(h);
    kk_std_core_hnd__clause1 _match_19703;
    kk_std_core_hnd__clause1 _brw_19704 = kk_std_core__select_throw_exn(h, _ctx); /*std/core/hnd/clause1<exception,1938,.hnd-exn,1939,1940>*/;
    kk_std_core__hnd_exn_drop(h, _ctx);
    _match_19703 = _brw_19704; /*std/core/hnd/clause1<exception,1938,.hnd-exn,1939,1940>*/
    {
      kk_function_t _fun_unbox_x18876 = _match_19703.clause;
      kk_box_t _x20731;
      kk_std_core__exception _x20732;
      kk_std_core__exception_info _x20733 = kk_std_core__new_ExnPattern(kk_reuse_null, location, definition, _ctx); /*exception-info*/
      _x20732 = kk_std_core__new_Exception(message0_17081, _x20733, _ctx); /*exception*/
      _x20731 = kk_std_core__exception_box(_x20732, _ctx); /*51*/
      return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x18876, (_fun_unbox_x18876, m0, ev_17960, _x20731, _ctx));
    }
  }
}
 
// Raise a pattern match exception. This is function is used internally by the
// compiler to generate error messages on pattern match failures.


// lift anonymous function
struct kk_std_core_error_pattern_fun20741__t {
  struct kk_function_s _base;
  kk_string_t definition;
  kk_string_t location;
};
static kk_box_t kk_std_core_error_pattern_fun20741(kk_function_t _fself, kk_box_t _b_18881, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_error_pattern_fun20741(kk_string_t definition, kk_string_t location, kk_context_t* _ctx) {
  struct kk_std_core_error_pattern_fun20741__t* _self = kk_function_alloc_as(struct kk_std_core_error_pattern_fun20741__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_error_pattern_fun20741, kk_context());
  _self->definition = definition;
  _self->location = location;
  return &_self->_base;
}

static kk_box_t kk_std_core_error_pattern_fun20741(kk_function_t _fself, kk_box_t _b_18881, kk_context_t* _ctx) {
  struct kk_std_core_error_pattern_fun20741__t* _self = kk_function_as(struct kk_std_core_error_pattern_fun20741__t*, _fself);
  kk_string_t definition = _self->definition; /* string */
  kk_string_t location = _self->location; /* string */
  kk_drop_match(_self, {kk_string_dup(definition);kk_string_dup(location);}, {}, _ctx)
  kk_string_t _x20742 = kk_string_unbox(_b_18881); /*string*/
  return kk_std_core__mlift17708_error_pattern(definition, location, _x20742, _ctx);
}

kk_box_t kk_std_core_error_pattern(kk_string_t location, kk_string_t definition, kk_context_t* _ctx) { /* forall<a> (location : string, definition : string) -> exn a */ 
  kk_string_t x_17963;
  bool _match_19702;
  kk_string_t _x20734 = kk_string_dup(definition); /*string*/
  kk_string_t _x20735 = kk_string_empty(); /*string*/
  _match_19702 = kk_string_is_eq(_x20734,_x20735,kk_context()); /*bool*/
  if (_match_19702) {
    x_17963 = kk_string_empty(); /*string*/
  }
  else {
    kk_string_t _x20738;
    kk_define_string_literal(, _s20739, 2, ": ")
    _x20738 = kk_string_dup(_s20739); /*string*/
    kk_string_t _x20740 = kk_string_dup(definition); /*string*/
    x_17963 = kk_std_core__lp__plus__plus__1_rp_(_x20738, _x20740, _ctx); /*string*/
  }
  if (kk_yielding(kk_context())) {
    kk_string_drop(x_17963, _ctx);
    return kk_std_core_hnd_yield_extend(kk_std_core_new_error_pattern_fun20741(definition, location, _ctx), _ctx);
  }
  {
    kk_string_t message0_17081;
    kk_string_t _x20743 = kk_string_dup(location); /*string*/
    kk_string_t _x20744;
    kk_string_t _x20745;
    kk_define_string_literal(, _s20746, 23, ": pattern match failure")
    _x20745 = kk_string_dup(_s20746); /*string*/
    _x20744 = kk_std_core__lp__plus__plus__1_rp_(x_17963, _x20745, _ctx); /*string*/
    message0_17081 = kk_std_core__lp__plus__plus__1_rp_(_x20743, _x20744, _ctx); /*string*/
    kk_std_core_hnd__ev ev_17966;
    kk_ssize_t _x20747 = (KK_IZ(0)); /*ssize_t*/
    ev_17966 = kk_evv_at(_x20747,kk_context()); /*std/core/hnd/ev<.hnd-exn>*/
    {
      struct kk_std_core_hnd_Ev* _con20748 = kk_std_core_hnd__as_Ev(ev_17966);
      kk_std_core_hnd__marker m0 = _con20748->marker;
      kk_box_t _box_x18882 = _con20748->hnd;
      kk_std_core__hnd_exn h = kk_std_core__hnd_exn_unbox(_box_x18882, NULL);
      kk_std_core__hnd_exn_dup(h);
      kk_std_core_hnd__clause1 _match_19700;
      kk_std_core_hnd__clause1 _brw_19701 = kk_std_core__select_throw_exn(h, _ctx); /*std/core/hnd/clause1<exception,1938,.hnd-exn,1939,1940>*/;
      kk_std_core__hnd_exn_drop(h, _ctx);
      _match_19700 = _brw_19701; /*std/core/hnd/clause1<exception,1938,.hnd-exn,1939,1940>*/
      {
        kk_function_t _fun_unbox_x18886 = _match_19700.clause;
        kk_box_t _x20750;
        kk_std_core__exception _x20751;
        kk_std_core__exception_info _x20752 = kk_std_core__new_ExnPattern(kk_reuse_null, location, definition, _ctx); /*exception-info*/
        _x20751 = kk_std_core__new_Exception(message0_17081, _x20752, _ctx); /*exception*/
        _x20750 = kk_std_core__exception_box(_x20751, _ctx); /*51*/
        return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x18886, (_fun_unbox_x18886, m0, ev_17966, _x20750, _ctx));
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
    kk_std_core_hnd__ev ev_17969;
    kk_ssize_t _x20753 = (KK_IZ(0)); /*ssize_t*/
    ev_17969 = kk_evv_at(_x20753,kk_context()); /*std/core/hnd/ev<.hnd-exn>*/
    {
      struct kk_std_core_hnd_Ev* _con20754 = kk_std_core_hnd__as_Ev(ev_17969);
      kk_std_core_hnd__marker m0 = _con20754->marker;
      kk_box_t _box_x18892 = _con20754->hnd;
      kk_std_core__hnd_exn h = kk_std_core__hnd_exn_unbox(_box_x18892, NULL);
      kk_std_core__hnd_exn_dup(h);
      kk_std_core_hnd__clause1 _match_19697;
      kk_std_core_hnd__clause1 _brw_19698 = kk_std_core__select_throw_exn(h, _ctx); /*std/core/hnd/clause1<exception,1938,.hnd-exn,1939,1940>*/;
      kk_std_core__hnd_exn_drop(h, _ctx);
      _match_19697 = _brw_19698; /*std/core/hnd/clause1<exception,1938,.hnd-exn,1939,1940>*/
      {
        kk_function_t _fun_unbox_x18896 = _match_19697.clause;
        return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x18896, (_fun_unbox_x18896, m0, ev_17969, kk_std_core__exception_box(exn0, _ctx), _ctx));
      }
    }
  }
  {
    kk_box_t x0 = err._cons.Ok.result;
    return x0;
  }
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift17709_op(kk_std_core_types__ctail _acc, kk_function_t pred, kk_box_t x, kk_std_core__list xx, bool _y_17464, kk_context_t* _ctx) { /* forall<a,e> (ctail<list<a>>, pred : (a) -> e bool, x : a, xx : list<a>, bool) -> e list<a> */ 
  if (_y_17464) {
    kk_std_core__list _ctail_17276 = kk_std_core__list_hole(); /*list<11895>*/;
    kk_std_core__list _ctail_17277 = kk_std_core__new_Cons(kk_reuse_null, x, _ctail_17276, _ctx); /*list<11895>*/;
    kk_box_t* _b_18908_18905 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17277)->tail)); /*cfield<list<11895>>*/;
    kk_std_core_types__ctail _x20756 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17277, _ctx)),_b_18908_18905); /*ctail<0>*/
    return kk_std_core__ctail_filter(xx, pred, _x20756, _ctx);
  }
  {
    kk_box_drop(x, _ctx);
    return kk_std_core__ctail_filter(xx, pred, _acc, _ctx);
  }
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift17710_op_fun20757__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t x0;
};
static kk_std_core__list kk_std_core__mlift17710_op_fun20757(kk_function_t _fself, kk_std_core__list _ctail_17279, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17710_op_fun20757(kk_function_t _accm, kk_box_t x0, kk_context_t* _ctx) {
  struct kk_std_core__mlift17710_op_fun20757__t* _self = kk_function_alloc_as(struct kk_std_core__mlift17710_op_fun20757__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17710_op_fun20757, kk_context());
  _self->_accm = _accm;
  _self->x0 = x0;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift17710_op_fun20757(kk_function_t _fself, kk_std_core__list _ctail_17279, kk_context_t* _ctx) {
  struct kk_std_core__mlift17710_op_fun20757__t* _self = kk_function_as(struct kk_std_core__mlift17710_op_fun20757__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<11895>) -> list<11895> */
  kk_box_t x0 = _self->x0; /* 11895 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(x0);}, {}, _ctx)
  kk_std_core__list _x20758 = kk_std_core__new_Cons(kk_reuse_null, x0, _ctail_17279, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x20758, _ctx));
}

kk_std_core__list kk_std_core__mlift17710_op(kk_function_t _accm, kk_function_t pred0, kk_box_t x0, kk_std_core__list xx0, bool _y_17469, kk_context_t* _ctx) { /* forall<a,e> ((list<a>) -> list<a>, pred : (a) -> e bool, x : a, xx : list<a>, bool) -> e list<a> */ 
  if (_y_17469) {
    return kk_std_core__ctailm_filter(xx0, pred0, kk_std_core__new_mlift17710_op_fun20757(_accm, x0, _ctx), _ctx);
  }
  {
    kk_box_drop(x0, _ctx);
    return kk_std_core__ctailm_filter(xx0, pred0, _accm, _ctx);
  }
}
 
// Retain only those elements of a list that satisfy the given predicate `pred`.
// For example: `filter([1,2,3],odd?) == [1,3]`


// lift anonymous function
struct kk_std_core__ctail_filter_fun20763__t {
  struct kk_function_s _base;
  kk_function_t pred1;
  kk_box_t x1;
  kk_std_core__list xx1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_filter_fun20763(kk_function_t _fself, kk_box_t _b_18913, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_filter_fun20763(kk_function_t pred1, kk_box_t x1, kk_std_core__list xx1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_filter_fun20763__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_filter_fun20763__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_filter_fun20763, kk_context());
  _self->pred1 = pred1;
  _self->x1 = x1;
  _self->xx1 = xx1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_filter_fun20763(kk_function_t _fself, kk_box_t _b_18913, kk_context_t* _ctx) {
  struct kk_std_core__ctail_filter_fun20763__t* _self = kk_function_as(struct kk_std_core__ctail_filter_fun20763__t*, _fself);
  kk_function_t pred1 = _self->pred1; /* (11895) -> 11896 bool */
  kk_box_t x1 = _self->x1; /* 11895 */
  kk_std_core__list xx1 = _self->xx1; /* list<11895> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<11895>> */
  kk_drop_match(_self, {kk_function_dup(pred1);kk_box_dup(x1);kk_std_core__list_dup(xx1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  bool _y_18931_174640 = kk_bool_unbox(_b_18913); /*bool*/;
  kk_std_core__list _x20764 = kk_std_core__mlift17709_op(_acc0, pred1, x1, xx1, _y_18931_174640, _ctx); /*list<11895>*/
  return kk_std_core__list_box(_x20764, _ctx);
}

kk_std_core__list kk_std_core__ctail_filter(kk_std_core__list xs, kk_function_t pred1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, pred : (a) -> e bool, ctail<list<a>>) -> e list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con20759 = kk_std_core__as_Cons(xs);
    kk_box_t x1 = _con20759->head;
    kk_std_core__list xx1 = _con20759->tail;
    kk_reuse_t _ru_19929 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      _ru_19929 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_box_dup(x1);
      kk_std_core__list_dup(xx1);
      kk_std_core__list_decref(xs, _ctx);
    }
    bool x2_17972;
    kk_function_t _x20761 = kk_function_dup(pred1); /*(11895) -> 11896 bool*/
    kk_box_t _x20760 = kk_box_dup(x1); /*11895*/
    x2_17972 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x20761, (_x20761, _x20760, _ctx)); /*bool*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_19929,kk_context());
      kk_box_t _x20762 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_filter_fun20763(pred1, x1, xx1, _acc0, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20762, _ctx);
    }
    if (x2_17972) {
      kk_std_core__list _ctail_172760 = kk_std_core__list_hole(); /*list<11895>*/;
      kk_std_core__list _ctail_172770 = kk_std_core__new_Cons(_ru_19929, x1, _ctail_172760, _ctx); /*list<11895>*/;
      kk_box_t* _b_18925_18919 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_172770)->tail)); /*cfield<list<11895>>*/;
      { // tailcall
        kk_std_core_types__ctail _x20765 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_172770, _ctx)),_b_18925_18919); /*ctail<0>*/
        xs = xx1;
        _acc0 = _x20765;
        goto kk__tailcall;
      }
    }
    {
      kk_reuse_drop(_ru_19929,kk_context());
      kk_box_drop(x1, _ctx);
      { // tailcall
        xs = xx1;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_function_drop(pred1, _ctx);
    kk_box_t _x20766 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20766, _ctx);
  }
}
 
// Retain only those elements of a list that satisfy the given predicate `pred`.
// For example: `filter([1,2,3],odd?) == [1,3]`


// lift anonymous function
struct kk_std_core__ctailm_filter_fun20771__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t pred2;
  kk_box_t x3;
  kk_std_core__list xx2;
};
static kk_box_t kk_std_core__ctailm_filter_fun20771(kk_function_t _fself, kk_box_t _b_18933, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_filter_fun20771(kk_function_t _accm0, kk_function_t pred2, kk_box_t x3, kk_std_core__list xx2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_filter_fun20771__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_filter_fun20771__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_filter_fun20771, kk_context());
  _self->_accm0 = _accm0;
  _self->pred2 = pred2;
  _self->x3 = x3;
  _self->xx2 = xx2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_filter_fun20771(kk_function_t _fself, kk_box_t _b_18933, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_filter_fun20771__t* _self = kk_function_as(struct kk_std_core__ctailm_filter_fun20771__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<11895>) -> list<11895> */
  kk_function_t pred2 = _self->pred2; /* (11895) -> 11896 bool */
  kk_box_t x3 = _self->x3; /* 11895 */
  kk_std_core__list xx2 = _self->xx2; /* list<11895> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(pred2);kk_box_dup(x3);kk_std_core__list_dup(xx2);}, {}, _ctx)
  bool _y_18935_174690 = kk_bool_unbox(_b_18933); /*bool*/;
  kk_std_core__list _x20772 = kk_std_core__mlift17710_op(_accm0, pred2, x3, xx2, _y_18935_174690, _ctx); /*list<11895>*/
  return kk_std_core__list_box(_x20772, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_filter_fun20774__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t x3;
};
static kk_std_core__list kk_std_core__ctailm_filter_fun20774(kk_function_t _fself, kk_std_core__list _ctail_172790, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_filter_fun20774(kk_function_t _accm0, kk_box_t x3, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_filter_fun20774__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_filter_fun20774__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_filter_fun20774, kk_context());
  _self->_accm0 = _accm0;
  _self->x3 = x3;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_filter_fun20774(kk_function_t _fself, kk_std_core__list _ctail_172790, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_filter_fun20774__t* _self = kk_function_as(struct kk_std_core__ctailm_filter_fun20774__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<11895>) -> list<11895> */
  kk_box_t x3 = _self->x3; /* 11895 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(x3);}, {}, _ctx)
  kk_std_core__list _x20775 = kk_std_core__new_Cons(kk_reuse_null, x3, _ctail_172790, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x20775, _ctx));
}

kk_std_core__list kk_std_core__ctailm_filter(kk_std_core__list xs0, kk_function_t pred2, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, pred : (a) -> e bool, (list<a>) -> list<a>) -> e list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con20767 = kk_std_core__as_Cons(xs0);
    kk_box_t x3 = _con20767->head;
    kk_std_core__list xx2 = _con20767->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs0))) {
      kk_std_core__list_free(xs0, _ctx);
    }
    else {
      kk_box_dup(x3);
      kk_std_core__list_dup(xx2);
      kk_std_core__list_decref(xs0, _ctx);
    }
    bool x4_17975;
    kk_function_t _x20769 = kk_function_dup(pred2); /*(11895) -> 11896 bool*/
    kk_box_t _x20768 = kk_box_dup(x3); /*11895*/
    x4_17975 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x20769, (_x20769, _x20768, _ctx)); /*bool*/
    if (kk_yielding(kk_context())) {
      kk_box_t _x20770 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_filter_fun20771(_accm0, pred2, x3, xx2, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20770, _ctx);
    }
    if (x4_17975) { // tailcall
                    kk_function_t _x20773 = kk_std_core__new_ctailm_filter_fun20774(_accm0, x3, _ctx); /*(list<11895>) -> list<11895>*/
                    xs0 = xx2;
                    _accm0 = _x20773;
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
struct kk_std_core_filter_fun20777__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core_filter_fun20777(kk_function_t _fself, kk_std_core__list _ctail_17278, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_filter_fun20777(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_filter_fun20777, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core_filter_fun20777(kk_function_t _fself, kk_std_core__list _ctail_17278, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17278;
}

kk_std_core__list kk_std_core_filter(kk_std_core__list xs1, kk_function_t pred3, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, pred : (a) -> e bool) -> e list<a> */ 
  bool _match_19694 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_19694) {
    kk_std_core_types__ctail _x20776 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_filter(xs1, pred3, _x20776, _ctx);
  }
  {
    return kk_std_core__ctailm_filter(xs1, pred3, kk_std_core_new_filter_fun20777(_ctx), _ctx);
  }
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift17711_op(kk_std_core_types__ctail _acc, kk_function_t pred, kk_std_core__list xx, kk_std_core_types__maybe _y_17477, kk_context_t* _ctx) { /* forall<a,b,e> (ctail<list<b>>, pred : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e list<b> */ 
  if (kk_std_core_types__is_Nothing(_y_17477)) {
    return kk_std_core__ctail_filter_map(xx, pred, _acc, _ctx);
  }
  {
    kk_box_t y = _y_17477._cons.Just.value;
    kk_std_core__list _ctail_17280 = kk_std_core__list_hole(); /*list<11941>*/;
    kk_std_core__list _ctail_17281 = kk_std_core__new_Cons(kk_reuse_null, y, _ctail_17280, _ctx); /*list<11941>*/;
    kk_box_t* _b_18944_18941 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17281)->tail)); /*cfield<list<11941>>*/;
    kk_std_core_types__ctail _x20778 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17281, _ctx)),_b_18944_18941); /*ctail<0>*/
    return kk_std_core__ctail_filter_map(xx, pred, _x20778, _ctx);
  }
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift17712_op_fun20779__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t y0;
};
static kk_std_core__list kk_std_core__mlift17712_op_fun20779(kk_function_t _fself, kk_std_core__list _ctail_17283, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17712_op_fun20779(kk_function_t _accm, kk_box_t y0, kk_context_t* _ctx) {
  struct kk_std_core__mlift17712_op_fun20779__t* _self = kk_function_alloc_as(struct kk_std_core__mlift17712_op_fun20779__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17712_op_fun20779, kk_context());
  _self->_accm = _accm;
  _self->y0 = y0;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift17712_op_fun20779(kk_function_t _fself, kk_std_core__list _ctail_17283, kk_context_t* _ctx) {
  struct kk_std_core__mlift17712_op_fun20779__t* _self = kk_function_as(struct kk_std_core__mlift17712_op_fun20779__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<11941>) -> list<11941> */
  kk_box_t y0 = _self->y0; /* 11941 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(y0);}, {}, _ctx)
  kk_std_core__list _x20780 = kk_std_core__new_Cons(kk_reuse_null, y0, _ctail_17283, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x20780, _ctx));
}

kk_std_core__list kk_std_core__mlift17712_op(kk_function_t _accm, kk_function_t pred0, kk_std_core__list xx0, kk_std_core_types__maybe _y_17482, kk_context_t* _ctx) { /* forall<a,b,e> ((list<b>) -> list<b>, pred : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e list<b> */ 
  if (kk_std_core_types__is_Nothing(_y_17482)) {
    return kk_std_core__ctailm_filter_map(xx0, pred0, _accm, _ctx);
  }
  {
    kk_box_t y0 = _y_17482._cons.Just.value;
    return kk_std_core__ctailm_filter_map(xx0, pred0, kk_std_core__new_mlift17712_op_fun20779(_accm, y0, _ctx), _ctx);
  }
}
 
// Retain only those elements of a list that satisfy the given predicate `pred`.
// For example: `filterMap([1,2,3],fn(i) { if i.odd? then Nothing else Just(i*i) }) == [4]`


// lift anonymous function
struct kk_std_core__ctail_filter_map_fun20785__t {
  struct kk_function_s _base;
  kk_function_t pred1;
  kk_std_core__list xx1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_filter_map_fun20785(kk_function_t _fself, kk_box_t _b_18951, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_filter_map_fun20785(kk_function_t pred1, kk_std_core__list xx1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_filter_map_fun20785__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_filter_map_fun20785__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_filter_map_fun20785, kk_context());
  _self->pred1 = pred1;
  _self->xx1 = xx1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_filter_map_fun20785(kk_function_t _fself, kk_box_t _b_18951, kk_context_t* _ctx) {
  struct kk_std_core__ctail_filter_map_fun20785__t* _self = kk_function_as(struct kk_std_core__ctail_filter_map_fun20785__t*, _fself);
  kk_function_t pred1 = _self->pred1; /* (11940) -> 11942 maybe<11941> */
  kk_std_core__list xx1 = _self->xx1; /* list<11940> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<11941>> */
  kk_drop_match(_self, {kk_function_dup(pred1);kk_std_core__list_dup(xx1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_std_core_types__maybe _y_18967_174770 = kk_std_core_types__maybe_unbox(_b_18951, _ctx); /*maybe<11941>*/;
  kk_std_core__list _x20786 = kk_std_core__mlift17711_op(_acc0, pred1, xx1, _y_18967_174770, _ctx); /*list<11941>*/
  return kk_std_core__list_box(_x20786, _ctx);
}

kk_std_core__list kk_std_core__ctail_filter_map(kk_std_core__list xs, kk_function_t pred1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, pred : (a) -> e maybe<b>, ctail<list<b>>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Nil(xs)) {
    kk_function_drop(pred1, _ctx);
    kk_box_t _x20781 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20781, _ctx);
  }
  {
    struct kk_std_core_Cons* _con20782 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con20782->head;
    kk_std_core__list xx1 = _con20782->tail;
    kk_reuse_t _ru_19931 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      _ru_19931 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx1);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_std_core_types__maybe x0_17978;
    kk_function_t _x20783 = kk_function_dup(pred1); /*(11940) -> 11942 maybe<11941>*/
    x0_17978 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_box_t, kk_context_t*), _x20783, (_x20783, x, _ctx)); /*maybe<11941>*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_19931,kk_context());
      kk_std_core_types__maybe_drop(x0_17978, _ctx);
      kk_box_t _x20784 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_filter_map_fun20785(pred1, xx1, _acc0, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20784, _ctx);
    }
    if (kk_std_core_types__is_Nothing(x0_17978)) {
      kk_reuse_drop(_ru_19931,kk_context());
      { // tailcall
        xs = xx1;
        goto kk__tailcall;
      }
    }
    {
      kk_box_t y1 = x0_17978._cons.Just.value;
      kk_std_core__list _ctail_172800 = kk_std_core__list_hole(); /*list<11941>*/;
      kk_std_core__list _ctail_172810 = kk_std_core__new_Cons(_ru_19931, y1, _ctail_172800, _ctx); /*list<11941>*/;
      kk_box_t* _b_18963_18957 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_172810)->tail)); /*cfield<list<11941>>*/;
      { // tailcall
        kk_std_core_types__ctail _x20787 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_172810, _ctx)),_b_18963_18957); /*ctail<0>*/
        xs = xx1;
        _acc0 = _x20787;
        goto kk__tailcall;
      }
    }
  }
}
 
// Retain only those elements of a list that satisfy the given predicate `pred`.
// For example: `filterMap([1,2,3],fn(i) { if i.odd? then Nothing else Just(i*i) }) == [4]`


// lift anonymous function
struct kk_std_core__ctailm_filter_map_fun20791__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t pred2;
  kk_std_core__list xx2;
};
static kk_box_t kk_std_core__ctailm_filter_map_fun20791(kk_function_t _fself, kk_box_t _b_18969, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_filter_map_fun20791(kk_function_t _accm0, kk_function_t pred2, kk_std_core__list xx2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_filter_map_fun20791__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_filter_map_fun20791__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_filter_map_fun20791, kk_context());
  _self->_accm0 = _accm0;
  _self->pred2 = pred2;
  _self->xx2 = xx2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_filter_map_fun20791(kk_function_t _fself, kk_box_t _b_18969, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_filter_map_fun20791__t* _self = kk_function_as(struct kk_std_core__ctailm_filter_map_fun20791__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<11941>) -> list<11941> */
  kk_function_t pred2 = _self->pred2; /* (11940) -> 11942 maybe<11941> */
  kk_std_core__list xx2 = _self->xx2; /* list<11940> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(pred2);kk_std_core__list_dup(xx2);}, {}, _ctx)
  kk_std_core_types__maybe _y_18971_174820 = kk_std_core_types__maybe_unbox(_b_18969, _ctx); /*maybe<11941>*/;
  kk_std_core__list _x20792 = kk_std_core__mlift17712_op(_accm0, pred2, xx2, _y_18971_174820, _ctx); /*list<11941>*/
  return kk_std_core__list_box(_x20792, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_filter_map_fun20794__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t y2;
};
static kk_std_core__list kk_std_core__ctailm_filter_map_fun20794(kk_function_t _fself, kk_std_core__list _ctail_172830, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_filter_map_fun20794(kk_function_t _accm0, kk_box_t y2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_filter_map_fun20794__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_filter_map_fun20794__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_filter_map_fun20794, kk_context());
  _self->_accm0 = _accm0;
  _self->y2 = y2;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_filter_map_fun20794(kk_function_t _fself, kk_std_core__list _ctail_172830, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_filter_map_fun20794__t* _self = kk_function_as(struct kk_std_core__ctailm_filter_map_fun20794__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<11941>) -> list<11941> */
  kk_box_t y2 = _self->y2; /* 11941 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(y2);}, {}, _ctx)
  kk_std_core__list _x20795 = kk_std_core__new_Cons(kk_reuse_null, y2, _ctail_172830, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x20795, _ctx));
}

kk_std_core__list kk_std_core__ctailm_filter_map(kk_std_core__list xs0, kk_function_t pred2, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, pred : (a) -> e maybe<b>, (list<b>) -> list<b>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Nil(xs0)) {
    kk_function_drop(pred2, _ctx);
    return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
  }
  {
    struct kk_std_core_Cons* _con20788 = kk_std_core__as_Cons(xs0);
    kk_box_t x1 = _con20788->head;
    kk_std_core__list xx2 = _con20788->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs0))) {
      kk_std_core__list_free(xs0, _ctx);
    }
    else {
      kk_box_dup(x1);
      kk_std_core__list_dup(xx2);
      kk_std_core__list_decref(xs0, _ctx);
    }
    kk_std_core_types__maybe x2_17981;
    kk_function_t _x20789 = kk_function_dup(pred2); /*(11940) -> 11942 maybe<11941>*/
    x2_17981 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_box_t, kk_context_t*), _x20789, (_x20789, x1, _ctx)); /*maybe<11941>*/
    if (kk_yielding(kk_context())) {
      kk_std_core_types__maybe_drop(x2_17981, _ctx);
      kk_box_t _x20790 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_filter_map_fun20791(_accm0, pred2, xx2, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20790, _ctx);
    }
    if (kk_std_core_types__is_Nothing(x2_17981)) { // tailcall
                                                   xs0 = xx2;
                                                   goto kk__tailcall;
    }
    {
      kk_box_t y2 = x2_17981._cons.Just.value;
      { // tailcall
        kk_function_t _x20793 = kk_std_core__new_ctailm_filter_map_fun20794(_accm0, y2, _ctx); /*(list<11941>) -> list<11941>*/
        xs0 = xx2;
        _accm0 = _x20793;
        goto kk__tailcall;
      }
    }
  }
}
 
// Retain only those elements of a list that satisfy the given predicate `pred`.
// For example: `filterMap([1,2,3],fn(i) { if i.odd? then Nothing else Just(i*i) }) == [4]`


// lift anonymous function
struct kk_std_core_filter_map_fun20797__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core_filter_map_fun20797(kk_function_t _fself, kk_std_core__list _ctail_17282, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_filter_map_fun20797(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_filter_map_fun20797, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core_filter_map_fun20797(kk_function_t _fself, kk_std_core__list _ctail_17282, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17282;
}

kk_std_core__list kk_std_core_filter_map(kk_std_core__list xs1, kk_function_t pred3, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, pred : (a) -> e maybe<b>) -> e list<b> */ 
  bool _match_19691 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_19691) {
    kk_std_core_types__ctail _x20796 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_filter_map(xs1, pred3, _x20796, _ctx);
  }
  {
    return kk_std_core__ctailm_filter_map(xs1, pred3, kk_std_core_new_filter_map_fun20797(_ctx), _ctx);
  }
}

bool kk_std_core_is_zero_1(kk_ssize_t i, kk_context_t* _ctx) { /* (i : ssize_t) -> bool */ 
  return (i == 0);
}
 
// Find the first element satisfying some predicate


// lift anonymous function
struct kk_std_core_find_fun20798__t {
  struct kk_function_s _base;
  kk_function_t pred;
};
static kk_std_core_types__maybe kk_std_core_find_fun20798(kk_function_t _fself, kk_box_t x, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_find_fun20798(kk_function_t pred, kk_context_t* _ctx) {
  struct kk_std_core_find_fun20798__t* _self = kk_function_alloc_as(struct kk_std_core_find_fun20798__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_find_fun20798, kk_context());
  _self->pred = pred;
  return &_self->_base;
}

static kk_std_core_types__maybe kk_std_core_find_fun20798(kk_function_t _fself, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_find_fun20798__t* _self = kk_function_as(struct kk_std_core_find_fun20798__t*, _fself);
  kk_function_t pred = _self->pred; /* (12039) -> bool */
  kk_drop_match(_self, {kk_function_dup(pred);}, {}, _ctx)
  bool _match_19690;
  kk_box_t _x20799 = kk_box_dup(x); /*12039*/
  _match_19690 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), pred, (pred, _x20799, _ctx)); /*bool*/
  if (_match_19690) {
    return kk_std_core_types__new_Just(x, _ctx);
  }
  {
    kk_box_drop(x, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
}

kk_std_core_types__maybe kk_std_core_find(kk_std_core__list xs, kk_function_t pred, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, pred : (a) -> bool) -> maybe<a> */ 
  return kk_std_core_foreach_while(xs, kk_std_core_new_find_fun20798(pred, _ctx), _ctx);
}
 
// O(n). If it occurs, return the position of substring `sub` in `s`, tupled with
// the position just following the substring `sub`.

kk_std_core_types__maybe kk_std_core_find_1(kk_string_t s, kk_string_t sub, kk_context_t* _ctx) { /* (s : string, sub : string) -> maybe<sslice> */ 
  kk_ssize_t i;
  kk_string_t _x20800 = kk_string_dup(s); /*string*/
  kk_string_t _x20801 = kk_string_dup(sub); /*string*/
  i = kk_string_index_of1(_x20800,_x20801,kk_context()); /*ssize_t*/
  bool _match_19689 = kk_std_core_is_zero_1(i, _ctx); /*bool*/;
  if (_match_19689) {
    kk_string_drop(sub, _ctx);
    kk_string_drop(s, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  {
    kk_std_core__sslice _b_18973_18972;
    kk_ssize_t _x20802 = kk_std_core_decr_1(i, _ctx); /*ssize_t*/
    kk_ssize_t _x20803 = kk_string_len(sub,kk_context()); /*ssize_t*/
    _b_18973_18972 = kk_std_core__new_Sslice(s, _x20802, _x20803, _ctx); /*sslice*/
    return kk_std_core_types__new_Just(kk_std_core__sslice_box(_b_18973_18972, _ctx), _ctx);
  }
}
 
// Return the last index of substring `sub` in `s` if it occurs.

kk_std_core_types__maybe kk_std_core_find_last(kk_string_t s, kk_string_t sub, kk_context_t* _ctx) { /* (s : string, sub : string) -> maybe<sslice> */ 
  kk_ssize_t i;
  kk_string_t _x20804 = kk_string_dup(s); /*string*/
  kk_string_t _x20805 = kk_string_dup(sub); /*string*/
  i = kk_string_last_index_of1(_x20804,_x20805,kk_context()); /*ssize_t*/
  bool _match_19688 = kk_std_core_is_zero_1(i, _ctx); /*bool*/;
  if (_match_19688) {
    kk_string_drop(sub, _ctx);
    kk_string_drop(s, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  {
    kk_std_core__sslice _b_18975_18974;
    kk_ssize_t _x20806 = kk_std_core_decr_1(i, _ctx); /*ssize_t*/
    kk_ssize_t _x20807 = kk_string_len(sub,kk_context()); /*ssize_t*/
    _b_18975_18974 = kk_std_core__new_Sslice(s, _x20806, _x20807, _ctx); /*sslice*/
    return kk_std_core_types__new_Just(kk_std_core__sslice_box(_b_18975_18974, _ctx), _ctx);
  }
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift17713_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_std_core__list xx, kk_std_core_types__maybe _y_17490, kk_context_t* _ctx) { /* forall<a,b,e> (ctail<list<b>>, f : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e list<b> */ 
  if (kk_std_core_types__is_Just(_y_17490)) {
    kk_box_t y = _y_17490._cons.Just.value;
    kk_std_core__list _ctail_17284 = kk_std_core__list_hole(); /*list<12245>*/;
    kk_std_core__list _ctail_17285 = kk_std_core__new_Cons(kk_reuse_null, y, _ctail_17284, _ctx); /*list<12245>*/;
    kk_box_t* _b_18984_18981 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17285)->tail)); /*cfield<list<12245>>*/;
    kk_std_core_types__ctail _x20808 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17285, _ctx)),_b_18984_18981); /*ctail<0>*/
    return kk_std_core__ctail_flatmap_maybe(xx, f, _x20808, _ctx);
  }
  {
    return kk_std_core__ctail_flatmap_maybe(xx, f, _acc, _ctx);
  }
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift17714_op_fun20809__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t y0;
};
static kk_std_core__list kk_std_core__mlift17714_op_fun20809(kk_function_t _fself, kk_std_core__list _ctail_17287, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17714_op_fun20809(kk_function_t _accm, kk_box_t y0, kk_context_t* _ctx) {
  struct kk_std_core__mlift17714_op_fun20809__t* _self = kk_function_alloc_as(struct kk_std_core__mlift17714_op_fun20809__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17714_op_fun20809, kk_context());
  _self->_accm = _accm;
  _self->y0 = y0;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift17714_op_fun20809(kk_function_t _fself, kk_std_core__list _ctail_17287, kk_context_t* _ctx) {
  struct kk_std_core__mlift17714_op_fun20809__t* _self = kk_function_as(struct kk_std_core__mlift17714_op_fun20809__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<12245>) -> list<12245> */
  kk_box_t y0 = _self->y0; /* 12245 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(y0);}, {}, _ctx)
  kk_std_core__list _x20810 = kk_std_core__new_Cons(kk_reuse_null, y0, _ctail_17287, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x20810, _ctx));
}

kk_std_core__list kk_std_core__mlift17714_op(kk_function_t _accm, kk_function_t f0, kk_std_core__list xx0, kk_std_core_types__maybe _y_17495, kk_context_t* _ctx) { /* forall<a,b,e> ((list<b>) -> list<b>, f : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e list<b> */ 
  if (kk_std_core_types__is_Just(_y_17495)) {
    kk_box_t y0 = _y_17495._cons.Just.value;
    return kk_std_core__ctailm_flatmap_maybe(xx0, f0, kk_std_core__new_mlift17714_op_fun20809(_accm, y0, _ctx), _ctx);
  }
  {
    return kk_std_core__ctailm_flatmap_maybe(xx0, f0, _accm, _ctx);
  }
}
 
// Concatenate the `Just` result elements from applying a function to all elements.


// lift anonymous function
struct kk_std_core__ctail_flatmap_maybe_fun20814__t {
  struct kk_function_s _base;
  kk_function_t f1;
  kk_std_core__list xx1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_flatmap_maybe_fun20814(kk_function_t _fself, kk_box_t _b_18989, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_flatmap_maybe_fun20814(kk_function_t f1, kk_std_core__list xx1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_flatmap_maybe_fun20814__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_flatmap_maybe_fun20814__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_flatmap_maybe_fun20814, kk_context());
  _self->f1 = f1;
  _self->xx1 = xx1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_flatmap_maybe_fun20814(kk_function_t _fself, kk_box_t _b_18989, kk_context_t* _ctx) {
  struct kk_std_core__ctail_flatmap_maybe_fun20814__t* _self = kk_function_as(struct kk_std_core__ctail_flatmap_maybe_fun20814__t*, _fself);
  kk_function_t f1 = _self->f1; /* (12244) -> 12246 maybe<12245> */
  kk_std_core__list xx1 = _self->xx1; /* list<12244> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<12245>> */
  kk_drop_match(_self, {kk_function_dup(f1);kk_std_core__list_dup(xx1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_std_core_types__maybe _y_19007_174900 = kk_std_core_types__maybe_unbox(_b_18989, _ctx); /*maybe<12245>*/;
  kk_std_core__list _x20815 = kk_std_core__mlift17713_op(_acc0, f1, xx1, _y_19007_174900, _ctx); /*list<12245>*/
  return kk_std_core__list_box(_x20815, _ctx);
}

kk_std_core__list kk_std_core__ctail_flatmap_maybe(kk_std_core__list xs, kk_function_t f1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, f : (a) -> e maybe<b>, ctail<list<b>>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con20811 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con20811->head;
    kk_std_core__list xx1 = _con20811->tail;
    kk_reuse_t _ru_19933 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      _ru_19933 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx1);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_std_core_types__maybe x0_17984;
    kk_function_t _x20812 = kk_function_dup(f1); /*(12244) -> 12246 maybe<12245>*/
    x0_17984 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_box_t, kk_context_t*), _x20812, (_x20812, x, _ctx)); /*maybe<12245>*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_19933,kk_context());
      kk_std_core_types__maybe_drop(x0_17984, _ctx);
      kk_box_t _x20813 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_flatmap_maybe_fun20814(f1, xx1, _acc0, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20813, _ctx);
    }
    if (kk_std_core_types__is_Just(x0_17984)) {
      kk_box_t y1 = x0_17984._cons.Just.value;
      kk_std_core__list _ctail_172840 = kk_std_core__list_hole(); /*list<12245>*/;
      kk_std_core__list _ctail_172850 = kk_std_core__new_Cons(_ru_19933, y1, _ctail_172840, _ctx); /*list<12245>*/;
      kk_box_t* _b_19001_18995 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_172850)->tail)); /*cfield<list<12245>>*/;
      { // tailcall
        kk_std_core_types__ctail _x20816 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_172850, _ctx)),_b_19001_18995); /*ctail<0>*/
        xs = xx1;
        _acc0 = _x20816;
        goto kk__tailcall;
      }
    }
    {
      kk_reuse_drop(_ru_19933,kk_context());
      { // tailcall
        xs = xx1;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_function_drop(f1, _ctx);
    kk_box_t _x20817 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20817, _ctx);
  }
}
 
// Concatenate the `Just` result elements from applying a function to all elements.


// lift anonymous function
struct kk_std_core__ctailm_flatmap_maybe_fun20821__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t f2;
  kk_std_core__list xx2;
};
static kk_box_t kk_std_core__ctailm_flatmap_maybe_fun20821(kk_function_t _fself, kk_box_t _b_19009, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_flatmap_maybe_fun20821(kk_function_t _accm0, kk_function_t f2, kk_std_core__list xx2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_flatmap_maybe_fun20821__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_flatmap_maybe_fun20821__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_flatmap_maybe_fun20821, kk_context());
  _self->_accm0 = _accm0;
  _self->f2 = f2;
  _self->xx2 = xx2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_flatmap_maybe_fun20821(kk_function_t _fself, kk_box_t _b_19009, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_flatmap_maybe_fun20821__t* _self = kk_function_as(struct kk_std_core__ctailm_flatmap_maybe_fun20821__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<12245>) -> list<12245> */
  kk_function_t f2 = _self->f2; /* (12244) -> 12246 maybe<12245> */
  kk_std_core__list xx2 = _self->xx2; /* list<12244> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(f2);kk_std_core__list_dup(xx2);}, {}, _ctx)
  kk_std_core_types__maybe _y_19011_174950 = kk_std_core_types__maybe_unbox(_b_19009, _ctx); /*maybe<12245>*/;
  kk_std_core__list _x20822 = kk_std_core__mlift17714_op(_accm0, f2, xx2, _y_19011_174950, _ctx); /*list<12245>*/
  return kk_std_core__list_box(_x20822, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_flatmap_maybe_fun20824__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t y2;
};
static kk_std_core__list kk_std_core__ctailm_flatmap_maybe_fun20824(kk_function_t _fself, kk_std_core__list _ctail_172870, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_flatmap_maybe_fun20824(kk_function_t _accm0, kk_box_t y2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_flatmap_maybe_fun20824__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_flatmap_maybe_fun20824__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_flatmap_maybe_fun20824, kk_context());
  _self->_accm0 = _accm0;
  _self->y2 = y2;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_flatmap_maybe_fun20824(kk_function_t _fself, kk_std_core__list _ctail_172870, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_flatmap_maybe_fun20824__t* _self = kk_function_as(struct kk_std_core__ctailm_flatmap_maybe_fun20824__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<12245>) -> list<12245> */
  kk_box_t y2 = _self->y2; /* 12245 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(y2);}, {}, _ctx)
  kk_std_core__list _x20825 = kk_std_core__new_Cons(kk_reuse_null, y2, _ctail_172870, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x20825, _ctx));
}

kk_std_core__list kk_std_core__ctailm_flatmap_maybe(kk_std_core__list xs0, kk_function_t f2, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, f : (a) -> e maybe<b>, (list<b>) -> list<b>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con20818 = kk_std_core__as_Cons(xs0);
    kk_box_t x1 = _con20818->head;
    kk_std_core__list xx2 = _con20818->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs0))) {
      kk_std_core__list_free(xs0, _ctx);
    }
    else {
      kk_box_dup(x1);
      kk_std_core__list_dup(xx2);
      kk_std_core__list_decref(xs0, _ctx);
    }
    kk_std_core_types__maybe x2_17987;
    kk_function_t _x20819 = kk_function_dup(f2); /*(12244) -> 12246 maybe<12245>*/
    x2_17987 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_box_t, kk_context_t*), _x20819, (_x20819, x1, _ctx)); /*maybe<12245>*/
    if (kk_yielding(kk_context())) {
      kk_std_core_types__maybe_drop(x2_17987, _ctx);
      kk_box_t _x20820 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_flatmap_maybe_fun20821(_accm0, f2, xx2, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20820, _ctx);
    }
    if (kk_std_core_types__is_Just(x2_17987)) {
      kk_box_t y2 = x2_17987._cons.Just.value;
      { // tailcall
        kk_function_t _x20823 = kk_std_core__new_ctailm_flatmap_maybe_fun20824(_accm0, y2, _ctx); /*(list<12245>) -> list<12245>*/
        xs0 = xx2;
        _accm0 = _x20823;
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
struct kk_std_core_flatmap_maybe_fun20827__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core_flatmap_maybe_fun20827(kk_function_t _fself, kk_std_core__list _ctail_17286, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_flatmap_maybe_fun20827(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_flatmap_maybe_fun20827, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core_flatmap_maybe_fun20827(kk_function_t _fself, kk_std_core__list _ctail_17286, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17286;
}

kk_std_core__list kk_std_core_flatmap_maybe(kk_std_core__list xs1, kk_function_t f3, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, f : (a) -> e maybe<b>) -> e list<b> */ 
  bool _match_19685 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_19685) {
    kk_std_core_types__ctail _x20826 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_flatmap_maybe(xs1, f3, _x20826, _ctx);
  }
  {
    return kk_std_core__ctailm_flatmap_maybe(xs1, f3, kk_std_core_new_flatmap_maybe_fun20827(_ctx), _ctx);
  }
}
 
// monadic lift

kk_box_t kk_std_core__mlift17715_fold_int(kk_integer_t end, kk_function_t f, kk_integer_t start0, kk_box_t x, kk_context_t* _ctx) { /* forall<a,e> (end : int, f : (int, a) -> e a, start0 : int, x : a) -> e a */ 
  kk_integer_t _x20828 = kk_integer_add_small_const(start0, 1, _ctx); /*int*/
  return kk_std_core_fold_int(_x20828, end, x, f, _ctx);
}
 
// fold over the integers between [`start`,`end`] (inclusive).


// lift anonymous function
struct kk_std_core_fold_int_fun20831__t {
  struct kk_function_s _base;
  kk_integer_t end0;
  kk_function_t f0;
  kk_integer_t start00;
};
static kk_box_t kk_std_core_fold_int_fun20831(kk_function_t _fself, kk_box_t x1, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_fold_int_fun20831(kk_integer_t end0, kk_function_t f0, kk_integer_t start00, kk_context_t* _ctx) {
  struct kk_std_core_fold_int_fun20831__t* _self = kk_function_alloc_as(struct kk_std_core_fold_int_fun20831__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_fold_int_fun20831, kk_context());
  _self->end0 = end0;
  _self->f0 = f0;
  _self->start00 = start00;
  return &_self->_base;
}

static kk_box_t kk_std_core_fold_int_fun20831(kk_function_t _fself, kk_box_t x1, kk_context_t* _ctx) {
  struct kk_std_core_fold_int_fun20831__t* _self = kk_function_as(struct kk_std_core_fold_int_fun20831__t*, _fself);
  kk_integer_t end0 = _self->end0; /* int */
  kk_function_t f0 = _self->f0; /* (int, 12305) -> 12306 12305 */
  kk_integer_t start00 = _self->start00; /* int */
  kk_drop_match(_self, {kk_integer_dup(end0);kk_function_dup(f0);kk_integer_dup(start00);}, {}, _ctx)
  return kk_std_core__mlift17715_fold_int(end0, f0, start00, x1, _ctx);
}

kk_box_t kk_std_core_fold_int(kk_integer_t start00, kk_integer_t end0, kk_box_t init0, kk_function_t f0, kk_context_t* _ctx) { /* forall<a,e> (start : int, end : int, init : a, f : (int, a) -> e a) -> e a */ 
  kk__tailcall: ;
  bool _match_19683 = kk_integer_gte_borrow(start00,end0,kk_context()); /*bool*/;
  if (_match_19683) {
    kk_integer_drop(start00, _ctx);
    kk_function_drop(f0, _ctx);
    kk_integer_drop(end0, _ctx);
    return init0;
  }
  {
    kk_box_t x0_17990;
    kk_function_t _x20830 = kk_function_dup(f0); /*(int, 12305) -> 12306 12305*/
    kk_integer_t _x20829 = kk_integer_dup(start00); /*int*/
    x0_17990 = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_box_t, kk_context_t*), _x20830, (_x20830, _x20829, init0, _ctx)); /*12305*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x0_17990, _ctx);
      return kk_std_core_hnd_yield_extend(kk_std_core_new_fold_int_fun20831(end0, f0, start00, _ctx), _ctx);
    }
    { // tailcall
      kk_integer_t _x20832 = kk_integer_add_small_const(start00, 1, _ctx); /*int*/
      start00 = _x20832;
      init0 = x0_17990;
      goto kk__tailcall;
    }
  }
}
 
// monadic lift

kk_box_t kk_std_core__mlift17716_foldl(kk_function_t f, kk_std_core__list xx, kk_box_t _y_17507, kk_context_t* _ctx) { /* forall<a,e,b> (f : (a, b) -> e a, xx : list<b>, a) -> e a */ 
  return kk_std_core_foldl(xx, _y_17507, f, _ctx);
}
 
// Fold a list from the left, i.e. `foldl([1,2],0,(+)) == (0+1)+2`
// Since `foldl` is tail recursive, it is preferred over `foldr` when using an associative function `f`


// lift anonymous function
struct kk_std_core_foldl_fun20836__t {
  struct kk_function_s _base;
  kk_function_t f0;
  kk_std_core__list xx0;
};
static kk_box_t kk_std_core_foldl_fun20836(kk_function_t _fself, kk_box_t _y_175070, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foldl_fun20836(kk_function_t f0, kk_std_core__list xx0, kk_context_t* _ctx) {
  struct kk_std_core_foldl_fun20836__t* _self = kk_function_alloc_as(struct kk_std_core_foldl_fun20836__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foldl_fun20836, kk_context());
  _self->f0 = f0;
  _self->xx0 = xx0;
  return &_self->_base;
}

static kk_box_t kk_std_core_foldl_fun20836(kk_function_t _fself, kk_box_t _y_175070, kk_context_t* _ctx) {
  struct kk_std_core_foldl_fun20836__t* _self = kk_function_as(struct kk_std_core_foldl_fun20836__t*, _fself);
  kk_function_t f0 = _self->f0; /* (12365, 12370) -> 12367 12365 */
  kk_std_core__list xx0 = _self->xx0; /* list<12370> */
  kk_drop_match(_self, {kk_function_dup(f0);kk_std_core__list_dup(xx0);}, {}, _ctx)
  return kk_std_core__mlift17716_foldl(f0, xx0, _y_175070, _ctx);
}

kk_box_t kk_std_core_foldl(kk_std_core__list xs, kk_box_t z, kk_function_t f0, kk_context_t* _ctx) { /* forall<a,b,e> (list<a>, b, (b, a) -> e b) -> e b */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con20834 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con20834->head;
    kk_std_core__list xx0 = _con20834->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_box_t x0_17993;
    kk_function_t _x20835 = kk_function_dup(f0); /*(12365, 12370) -> 12367 12365*/
    x0_17993 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_context_t*), _x20835, (_x20835, z, x, _ctx)); /*12365*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x0_17993, _ctx);
      return kk_std_core_hnd_yield_extend(kk_std_core_new_foldl_fun20836(f0, xx0, _ctx), _ctx);
    }
    { // tailcall
      xs = xx0;
      z = x0_17993;
      goto kk__tailcall;
    }
  }
  {
    kk_function_drop(f0, _ctx);
    return z;
  }
}


// lift anonymous function
struct kk_std_core_foldl1_fun20839__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_foldl1_fun20839(kk_function_t _fself, kk_box_t _b_19016, kk_box_t _b_19017, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foldl1_fun20839(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_foldl1_fun20839, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_foldl1_fun20839(kk_function_t _fself, kk_box_t _b_19016, kk_box_t _b_19017, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_string_t _x20840 = kk_string_unbox(_b_19016); /*string*/
  kk_std_core_types__optional _x20841 = kk_std_core_types__optional_unbox(_b_19017, _ctx); /*optional<exception-info>*/
  return kk_std_core_throw(_x20840, _x20841, _ctx);
}

kk_box_t kk_std_core_foldl1(kk_std_core__list xs, kk_function_t f, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, f : (a, a) -> <exn|e> a) -> <exn|e> a */ 
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con20837 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con20837->head;
    kk_std_core__list xx = _con20837->tail;
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
    kk_ssize_t _b_19018_19012;
    kk_std_core_hnd__htag _x20838 = kk_std_core_hnd__htag_dup(kk_std_core__tag_exn); /*std/core/hnd/htag<.hnd-exn>*/
    _b_19018_19012 = kk_std_core_hnd__evv_index(_x20838, _ctx); /*std/core/hnd/ev-index*/
    kk_box_t _x20842;
    kk_string_t _x20843;
    kk_define_string_literal(, _s20844, 33, "unexpected Nil in std/core/foldl1")
    _x20843 = kk_string_dup(_s20844); /*string*/
    _x20842 = kk_string_box(_x20843); /*5615*/
    return kk_std_core_hnd__open_at2(_b_19018_19012, kk_std_core_new_foldl1_fun20839(_ctx), _x20842, kk_std_core_types__optional_box(kk_std_core_types__new_None(_ctx), _ctx), _ctx);
  }
}
 
// lifted local: reverse, reverse-acc

kk_std_core__list kk_std_core__lift17195_reverse(kk_std_core__list acc, kk_std_core__list ys, kk_context_t* _ctx) { /* forall<a> (acc : list<a>, ys : list<a>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con20845 = kk_std_core__as_Cons(ys);
    kk_box_t x = _con20845->head;
    kk_std_core__list xx = _con20845->tail;
    kk_reuse_t _ru_19937 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      _ru_19937 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(ys, _ctx);
    }
    { // tailcall
      kk_std_core__list _x20846 = kk_std_core__new_Cons(_ru_19937, x, acc, _ctx); /*list<61>*/
      acc = _x20846;
      ys = xx;
      goto kk__tailcall;
    }
  }
  {
    return acc;
  }
}
extern kk_box_t kk_std_core_foldr_fun20848(kk_function_t _fself, kk_box_t x, kk_box_t y, kk_context_t* _ctx) {
  struct kk_std_core_foldr_fun20848__t* _self = kk_function_as(struct kk_std_core_foldr_fun20848__t*, _fself);
  kk_function_t f = _self->f; /* (12443, 12439) -> 12441 12439 */
  kk_drop_match(_self, {kk_function_dup(f);}, {}, _ctx)
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_context_t*), f, (f, y, x, _ctx));
}


// lift anonymous function
struct kk_std_core_foldr1_fun20851__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_foldr1_fun20851(kk_function_t _fself, kk_box_t _b_19026, kk_box_t _b_19027, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foldr1_fun20851(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_foldr1_fun20851, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_foldr1_fun20851(kk_function_t _fself, kk_box_t _b_19026, kk_box_t _b_19027, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_string_t _x20852 = kk_string_unbox(_b_19026); /*string*/
  kk_std_core_types__optional _x20853 = kk_std_core_types__optional_unbox(_b_19027, _ctx); /*optional<exception-info>*/
  return kk_std_core_throw(_x20852, _x20853, _ctx);
}

kk_box_t kk_std_core_foldr1(kk_std_core__list xs, kk_function_t f, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, f : (a, a) -> <exn|e> a) -> <exn|e> a */ 
  kk_std_core__list xs0_17091 = kk_std_core__lift17195_reverse(kk_std_core__new_Nil(_ctx), xs, _ctx); /*list<12477>*/;
  if (kk_std_core__is_Cons(xs0_17091)) {
    struct kk_std_core_Cons* _con20849 = kk_std_core__as_Cons(xs0_17091);
    kk_box_t x = _con20849->head;
    kk_std_core__list xx = _con20849->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs0_17091))) {
      kk_std_core__list_free(xs0_17091, _ctx);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs0_17091, _ctx);
    }
    return kk_std_core_foldl(xx, x, f, _ctx);
  }
  {
    kk_function_drop(f, _ctx);
    kk_ssize_t _b_19028_19022;
    kk_std_core_hnd__htag _x20850 = kk_std_core_hnd__htag_dup(kk_std_core__tag_exn); /*std/core/hnd/htag<.hnd-exn>*/
    _b_19028_19022 = kk_std_core_hnd__evv_index(_x20850, _ctx); /*std/core/hnd/ev-index*/
    kk_box_t _x20854;
    kk_string_t _x20855;
    kk_define_string_literal(, _s20856, 33, "unexpected Nil in std/core/foldl1")
    _x20855 = kk_string_dup(_s20856); /*string*/
    _x20854 = kk_string_box(_x20855); /*5615*/
    return kk_std_core_hnd__open_at2(_b_19028_19022, kk_std_core_new_foldr1_fun20851(_ctx), _x20854, kk_std_core_types__optional_box(kk_std_core_types__new_None(_ctx), _ctx), _ctx);
  }
}
 
// monadic lift

kk_unit_t kk_std_core__mlift17717_op(kk_function_t action, kk_integer_t end, kk_integer_t i, kk_unit_t wild__, kk_context_t* _ctx) { /* forall<e> (action : (int) -> e (), end : int, i : int, wild_ : ()) -> e () */ 
  kk_integer_t i0_17225 = kk_integer_add_small_const(i, 1, _ctx); /*int*/;
  kk_std_core__lift17196_for(action, end, i0_17225, _ctx); return kk_Unit;
}
 
// lifted local: for, rep


// lift anonymous function
struct kk_std_core__lift17196_for_fun20860__t {
  struct kk_function_s _base;
  kk_function_t action0;
  kk_integer_t end0;
  kk_integer_t i0;
};
static kk_box_t kk_std_core__lift17196_for_fun20860(kk_function_t _fself, kk_box_t _b_19033, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17196_for_fun20860(kk_function_t action0, kk_integer_t end0, kk_integer_t i0, kk_context_t* _ctx) {
  struct kk_std_core__lift17196_for_fun20860__t* _self = kk_function_alloc_as(struct kk_std_core__lift17196_for_fun20860__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift17196_for_fun20860, kk_context());
  _self->action0 = action0;
  _self->end0 = end0;
  _self->i0 = i0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift17196_for_fun20860(kk_function_t _fself, kk_box_t _b_19033, kk_context_t* _ctx) {
  struct kk_std_core__lift17196_for_fun20860__t* _self = kk_function_as(struct kk_std_core__lift17196_for_fun20860__t*, _fself);
  kk_function_t action0 = _self->action0; /* (int) -> 12514 () */
  kk_integer_t end0 = _self->end0; /* int */
  kk_integer_t i0 = _self->i0; /* int */
  kk_drop_match(_self, {kk_function_dup(action0);kk_integer_dup(end0);kk_integer_dup(i0);}, {}, _ctx)
  kk_unit_t wild__0_19035 = kk_Unit;
  kk_unit_unbox(_b_19033);
  kk_unit_t _x20861 = kk_Unit;
  kk_std_core__mlift17717_op(action0, end0, i0, wild__0_19035, _ctx);
  return kk_unit_box(_x20861);
}

kk_unit_t kk_std_core__lift17196_for(kk_function_t action0, kk_integer_t end0, kk_integer_t i0, kk_context_t* _ctx) { /* forall<e> (action : (int) -> e (), end : int, i : int) -> e () */ 
  kk__tailcall: ;
  bool _match_19680 = kk_integer_lte_borrow(i0,end0,kk_context()); /*bool*/;
  if (_match_19680) {
    kk_unit_t x_17998 = kk_Unit;
    kk_function_t _x20858 = kk_function_dup(action0); /*(int) -> 12514 ()*/
    kk_integer_t _x20857 = kk_integer_dup(i0); /*int*/
    kk_function_call(kk_unit_t, (kk_function_t, kk_integer_t, kk_context_t*), _x20858, (_x20858, _x20857, _ctx));
    if (kk_yielding(kk_context())) {
      kk_box_t _x20859 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift17196_for_fun20860(action0, end0, i0, _ctx), _ctx); /*3926*/
      kk_unit_unbox(_x20859); return kk_Unit;
    }
    {
      kk_integer_t i0_172250 = kk_integer_add_small_const(i0, 1, _ctx); /*int*/;
      { // tailcall
        i0 = i0_172250;
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

kk_std_core_types__maybe kk_std_core__mlift17718_op(kk_function_t action, kk_integer_t end, kk_integer_t i, kk_std_core_types__maybe _y_17522, kk_context_t* _ctx) { /* forall<a,e> (action : (int) -> e maybe<a>, end : int, i : int, maybe<a>) -> e maybe<a> */ 
  if (kk_std_core_types__is_Nothing(_y_17522)) {
    kk_integer_t i0_17226 = kk_integer_add_small_const(i, 1, _ctx); /*int*/;
    return kk_std_core__lift17197_for_while(action, end, i0_17226, _ctx);
  }
  {
    kk_box_t x0 = _y_17522._cons.Just.value;
    kk_integer_drop(i, _ctx);
    kk_integer_drop(end, _ctx);
    kk_function_drop(action, _ctx);
    return kk_std_core_types__new_Just(x0, _ctx);
  }
}
 
// lifted local: for-while, rep


// lift anonymous function
struct kk_std_core__lift17197_for_while_fun20866__t {
  struct kk_function_s _base;
  kk_function_t action0;
  kk_integer_t end0;
  kk_integer_t i0;
};
static kk_box_t kk_std_core__lift17197_for_while_fun20866(kk_function_t _fself, kk_box_t _b_19037, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17197_for_while_fun20866(kk_function_t action0, kk_integer_t end0, kk_integer_t i0, kk_context_t* _ctx) {
  struct kk_std_core__lift17197_for_while_fun20866__t* _self = kk_function_alloc_as(struct kk_std_core__lift17197_for_while_fun20866__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift17197_for_while_fun20866, kk_context());
  _self->action0 = action0;
  _self->end0 = end0;
  _self->i0 = i0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift17197_for_while_fun20866(kk_function_t _fself, kk_box_t _b_19037, kk_context_t* _ctx) {
  struct kk_std_core__lift17197_for_while_fun20866__t* _self = kk_function_as(struct kk_std_core__lift17197_for_while_fun20866__t*, _fself);
  kk_function_t action0 = _self->action0; /* (int) -> 12565 maybe<12564> */
  kk_integer_t end0 = _self->end0; /* int */
  kk_integer_t i0 = _self->i0; /* int */
  kk_drop_match(_self, {kk_function_dup(action0);kk_integer_dup(end0);kk_integer_dup(i0);}, {}, _ctx)
  kk_std_core_types__maybe _y_19039_175220 = kk_std_core_types__maybe_unbox(_b_19037, _ctx); /*maybe<12564>*/;
  kk_std_core_types__maybe _x20867 = kk_std_core__mlift17718_op(action0, end0, i0, _y_19039_175220, _ctx); /*maybe<12564>*/
  return kk_std_core_types__maybe_box(_x20867, _ctx);
}

kk_std_core_types__maybe kk_std_core__lift17197_for_while(kk_function_t action0, kk_integer_t end0, kk_integer_t i0, kk_context_t* _ctx) { /* forall<a,e> (action : (int) -> e maybe<a>, end : int, i : int) -> e maybe<a> */ 
  kk__tailcall: ;
  bool _match_19678 = kk_integer_lte_borrow(i0,end0,kk_context()); /*bool*/;
  if (_match_19678) {
    kk_std_core_types__maybe x_18001;
    kk_function_t _x20864 = kk_function_dup(action0); /*(int) -> 12565 maybe<12564>*/
    kk_integer_t _x20863 = kk_integer_dup(i0); /*int*/
    x_18001 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_integer_t, kk_context_t*), _x20864, (_x20864, _x20863, _ctx)); /*maybe<12564>*/
    if (kk_yielding(kk_context())) {
      kk_std_core_types__maybe_drop(x_18001, _ctx);
      kk_box_t _x20865 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift17197_for_while_fun20866(action0, end0, i0, _ctx), _ctx); /*3926*/
      return kk_std_core_types__maybe_unbox(_x20865, _ctx);
    }
    if (kk_std_core_types__is_Nothing(x_18001)) {
      kk_integer_t i0_172260 = kk_integer_add_small_const(i0, 1, _ctx); /*int*/;
      { // tailcall
        i0 = i0_172260;
        goto kk__tailcall;
      }
    }
    {
      kk_box_t x00 = x_18001._cons.Just.value;
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


// lift anonymous function
struct kk_std_core__mlift17720_foreach_indexed_fun20870__t {
  struct kk_function_s _base;
  kk_ref_t i;
};
static kk_unit_t kk_std_core__mlift17720_foreach_indexed_fun20870(kk_function_t _fself, kk_integer_t _y_17530, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17720_foreach_indexed_fun20870(kk_ref_t i, kk_context_t* _ctx) {
  struct kk_std_core__mlift17720_foreach_indexed_fun20870__t* _self = kk_function_alloc_as(struct kk_std_core__mlift17720_foreach_indexed_fun20870__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17720_foreach_indexed_fun20870, kk_context());
  _self->i = i;
  return &_self->_base;
}

static kk_unit_t kk_std_core__mlift17720_foreach_indexed_fun20870(kk_function_t _fself, kk_integer_t _y_17530, kk_context_t* _ctx) {
  struct kk_std_core__mlift17720_foreach_indexed_fun20870__t* _self = kk_function_as(struct kk_std_core__mlift17720_foreach_indexed_fun20870__t*, _fself);
  kk_ref_t i = _self->i; /* local-var<12669,int> */
  kk_drop_match(_self, {kk_ref_dup(i);}, {}, _ctx)
  kk_integer_t _b_19049_19047 = kk_integer_add_small_const(_y_17530, 1, _ctx); /*int*/;
  kk_unit_t _brw_19676 = kk_Unit;
  kk_ref_set_borrow(i,(kk_integer_box(_b_19049_19047)),kk_context());
  kk_ref_drop(i, _ctx);
  return _brw_19676;
}


// lift anonymous function
struct kk_std_core__mlift17720_foreach_indexed_fun20872__t {
  struct kk_function_s _base;
  kk_function_t next0_18005;
};
static kk_box_t kk_std_core__mlift17720_foreach_indexed_fun20872(kk_function_t _fself, kk_box_t _b_19051, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17720_foreach_indexed_fun20872(kk_function_t next0_18005, kk_context_t* _ctx) {
  struct kk_std_core__mlift17720_foreach_indexed_fun20872__t* _self = kk_function_alloc_as(struct kk_std_core__mlift17720_foreach_indexed_fun20872__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17720_foreach_indexed_fun20872, kk_context());
  _self->next0_18005 = next0_18005;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift17720_foreach_indexed_fun20872(kk_function_t _fself, kk_box_t _b_19051, kk_context_t* _ctx) {
  struct kk_std_core__mlift17720_foreach_indexed_fun20872__t* _self = kk_function_as(struct kk_std_core__mlift17720_foreach_indexed_fun20872__t*, _fself);
  kk_function_t next0_18005 = _self->next0_18005; /* (int) -> <local<12669>|12676> () */
  kk_drop_match(_self, {kk_function_dup(next0_18005);}, {}, _ctx)
  kk_unit_t _x20873 = kk_Unit;
  kk_integer_t _x20874 = kk_integer_unbox(_b_19051); /*int*/
  kk_function_call(kk_unit_t, (kk_function_t, kk_integer_t, kk_context_t*), next0_18005, (next0_18005, _x20874, _ctx));
  return kk_unit_box(_x20873);
}

kk_unit_t kk_std_core__mlift17720_foreach_indexed(kk_ref_t i, kk_unit_t wild__, kk_context_t* _ctx) { /* forall<h,e> (i : local-var<h,int>, wild_ : ()) -> <local<h>|e> () */ 
  kk_integer_t x_18004;
  kk_box_t _x20868;
  kk_ref_t _x20869 = kk_ref_dup(i); /*local-var<12669,int>*/
  _x20868 = kk_ref_get(_x20869,kk_context()); /*1000*/
  x_18004 = kk_integer_unbox(_x20868); /*int*/
  kk_function_t next0_18005 = kk_std_core__new_mlift17720_foreach_indexed_fun20870(i, _ctx); /*(int) -> <local<12669>|12676> ()*/;
  if (kk_yielding(kk_context())) {
    kk_integer_drop(x_18004, _ctx);
    kk_box_t _x20871 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift17720_foreach_indexed_fun20872(next0_18005, _ctx), _ctx); /*3926*/
    kk_unit_unbox(_x20871); return kk_Unit;
  }
  {
    kk_function_call(kk_unit_t, (kk_function_t, kk_integer_t, kk_context_t*), next0_18005, (next0_18005, x_18004, _ctx)); return kk_Unit;
  }
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift17721_foreach_indexed_fun20876__t {
  struct kk_function_s _base;
  kk_ref_t i;
};
static kk_box_t kk_std_core__mlift17721_foreach_indexed_fun20876(kk_function_t _fself, kk_box_t _b_19054, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17721_foreach_indexed_fun20876(kk_ref_t i, kk_context_t* _ctx) {
  struct kk_std_core__mlift17721_foreach_indexed_fun20876__t* _self = kk_function_alloc_as(struct kk_std_core__mlift17721_foreach_indexed_fun20876__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17721_foreach_indexed_fun20876, kk_context());
  _self->i = i;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift17721_foreach_indexed_fun20876(kk_function_t _fself, kk_box_t _b_19054, kk_context_t* _ctx) {
  struct kk_std_core__mlift17721_foreach_indexed_fun20876__t* _self = kk_function_as(struct kk_std_core__mlift17721_foreach_indexed_fun20876__t*, _fself);
  kk_ref_t i = _self->i; /* local-var<12669,int> */
  kk_drop_match(_self, {kk_ref_dup(i);}, {}, _ctx)
  kk_unit_t wild___19056 = kk_Unit;
  kk_unit_unbox(_b_19054);
  kk_unit_t _x20877 = kk_Unit;
  kk_std_core__mlift17720_foreach_indexed(i, wild___19056, _ctx);
  return kk_unit_box(_x20877);
}

kk_unit_t kk_std_core__mlift17721_foreach_indexed(kk_function_t action, kk_ref_t i, kk_box_t x, kk_integer_t j, kk_context_t* _ctx) { /* forall<h,a,e> (action : (int, a) -> e (), i : local-var<h,int>, x : a, j : int) -> <local<h>|e> () */ 
  kk_unit_t x0_18008 = kk_Unit;
  kk_function_call(kk_unit_t, (kk_function_t, kk_integer_t, kk_box_t, kk_context_t*), action, (action, j, x, _ctx));
  if (kk_yielding(kk_context())) {
    kk_box_t _x20875 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift17721_foreach_indexed_fun20876(i, _ctx), _ctx); /*3926*/
    kk_unit_unbox(_x20875); return kk_Unit;
  }
  {
    kk_std_core__mlift17720_foreach_indexed(i, x0_18008, _ctx); return kk_Unit;
  }
}
 
// Invoke `action` for each element of a list, passing also the position of the element.


// lift anonymous function
struct kk_std_core_foreach_indexed_fun20879__t {
  struct kk_function_s _base;
  kk_function_t action;
  kk_ref_t loc;
};
static kk_unit_t kk_std_core_foreach_indexed_fun20879(kk_function_t _fself, kk_box_t x, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_indexed_fun20879(kk_function_t action, kk_ref_t loc, kk_context_t* _ctx) {
  struct kk_std_core_foreach_indexed_fun20879__t* _self = kk_function_alloc_as(struct kk_std_core_foreach_indexed_fun20879__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foreach_indexed_fun20879, kk_context());
  _self->action = action;
  _self->loc = loc;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_foreach_indexed_fun20883__t {
  struct kk_function_s _base;
  kk_function_t action;
  kk_ref_t loc;
  kk_box_t x;
};
static kk_box_t kk_std_core_foreach_indexed_fun20883(kk_function_t _fself, kk_box_t _b_19062, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_indexed_fun20883(kk_function_t action, kk_ref_t loc, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_foreach_indexed_fun20883__t* _self = kk_function_alloc_as(struct kk_std_core_foreach_indexed_fun20883__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foreach_indexed_fun20883, kk_context());
  _self->action = action;
  _self->loc = loc;
  _self->x = x;
  return &_self->_base;
}

static kk_box_t kk_std_core_foreach_indexed_fun20883(kk_function_t _fself, kk_box_t _b_19062, kk_context_t* _ctx) {
  struct kk_std_core_foreach_indexed_fun20883__t* _self = kk_function_as(struct kk_std_core_foreach_indexed_fun20883__t*, _fself);
  kk_function_t action = _self->action; /* (int, 12675) -> 12676 () */
  kk_ref_t loc = _self->loc; /* local-var<12669,int> */
  kk_box_t x = _self->x; /* 12675 */
  kk_drop_match(_self, {kk_function_dup(action);kk_ref_dup(loc);kk_box_dup(x);}, {}, _ctx)
  kk_unit_t _x20884 = kk_Unit;
  kk_integer_t _x20885 = kk_integer_unbox(_b_19062); /*int*/
  kk_std_core__mlift17721_foreach_indexed(action, loc, x, _x20885, _ctx);
  return kk_unit_box(_x20884);
}
static kk_unit_t kk_std_core_foreach_indexed_fun20879(kk_function_t _fself, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_foreach_indexed_fun20879__t* _self = kk_function_as(struct kk_std_core_foreach_indexed_fun20879__t*, _fself);
  kk_function_t action = _self->action; /* (int, 12675) -> 12676 () */
  kk_ref_t loc = _self->loc; /* local-var<12669,int> */
  kk_drop_match(_self, {kk_function_dup(action);kk_ref_dup(loc);}, {}, _ctx)
  kk_integer_t x0_18013;
  kk_box_t _x20880;
  kk_ref_t _x20881 = kk_ref_dup(loc); /*local-var<12669,int>*/
  _x20880 = kk_ref_get(_x20881,kk_context()); /*1000*/
  x0_18013 = kk_integer_unbox(_x20880); /*int*/
  if (kk_yielding(kk_context())) {
    kk_integer_drop(x0_18013, _ctx);
    kk_box_t _x20882 = kk_std_core_hnd_yield_extend(kk_std_core_new_foreach_indexed_fun20883(action, loc, x, _ctx), _ctx); /*3926*/
    return kk_unit_unbox(_x20882);
  }
  {
    return kk_std_core__mlift17721_foreach_indexed(action, loc, x, x0_18013, _ctx);
  }
}

kk_unit_t kk_std_core_foreach_indexed(kk_std_core__list xs, kk_function_t action, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, action : (int, a) -> e ()) -> e () */ 
  kk_ref_t loc = kk_ref_alloc((kk_integer_box(kk_integer_from_small(0))),kk_context()); /*local-var<12669,int>*/;
  kk_unit_t res = kk_Unit;
  kk_function_t _x20878;
  kk_ref_dup(loc);
  _x20878 = kk_std_core_new_foreach_indexed_fun20879(action, loc, _ctx); /*(x : 12675) -> <local<12669>|12676> ()*/
  kk_std_core_foreach(xs, _x20878, _ctx);
  kk_box_t _x20886 = kk_std_core_hnd_prompt_local_var(loc, kk_unit_box(res), _ctx); /*10459*/
  kk_unit_unbox(_x20886); return kk_Unit;
}
 
// Invoke a function `f` for each element in a vector `v`


// lift anonymous function
struct kk_std_core_foreach_indexed_fun20888__t_1 {
  struct kk_function_s _base;
  kk_function_t f;
  kk_vector_t v;
};
static kk_unit_t kk_std_core_foreach_indexed_fun20888_1(kk_function_t _fself, kk_ssize_t i, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_indexed_fun20888_1(kk_function_t f, kk_vector_t v, kk_context_t* _ctx) {
  struct kk_std_core_foreach_indexed_fun20888__t_1* _self = kk_function_alloc_as(struct kk_std_core_foreach_indexed_fun20888__t_1, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foreach_indexed_fun20888_1, kk_context());
  _self->f = f;
  _self->v = v;
  return &_self->_base;
}

static kk_unit_t kk_std_core_foreach_indexed_fun20888_1(kk_function_t _fself, kk_ssize_t i, kk_context_t* _ctx) {
  struct kk_std_core_foreach_indexed_fun20888__t_1* _self = kk_function_as(struct kk_std_core_foreach_indexed_fun20888__t_1*, _fself);
  kk_function_t f = _self->f; /* (int, 12706) -> 12707 () */
  kk_vector_t v = _self->v; /* vector<12706> */
  kk_drop_match(_self, {kk_function_dup(f);kk_vector_dup(v);}, {}, _ctx)
  kk_box_t x_18143;
  kk_box_t _brw_19672 = kk_vector_at_borrow(v,i); /*228*/;
  kk_vector_drop(v, _ctx);
  x_18143 = _brw_19672; /*12706*/
  kk_integer_t _x20889 = kk_integer_from_ssize_t(i,kk_context()); /*int*/
  return kk_function_call(kk_unit_t, (kk_function_t, kk_integer_t, kk_box_t, kk_context_t*), f, (f, _x20889, x_18143, _ctx));
}

kk_unit_t kk_std_core_foreach_indexed_1(kk_vector_t v, kk_function_t f, kk_context_t* _ctx) { /* forall<a,e> (v : vector<a>, f : (int, a) -> e ()) -> e () */ 
  kk_ssize_t start0_18017 = (KK_IZ(0)); /*ssize_t*/;
  kk_ssize_t end_18018;
  kk_ssize_t _x20887 = kk_vector_len_borrow(v); /*ssize_t*/
  end_18018 = kk_std_core_decr_1(_x20887, _ctx); /*ssize_t*/
  kk_std_core__lift17190_forz(kk_std_core_new_foreach_indexed_fun20888_1(f, v, _ctx), end_18018, start0_18017, _ctx); return kk_Unit;
}
 
// Return the head of list if the list is not empty.

kk_std_core_types__maybe kk_std_core_head_1(kk_std_core__list xs, kk_context_t* _ctx) { /* forall<a> (xs : list<a>) -> maybe<a> */ 
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con20898 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con20898->head;
    kk_std_core__list _pat0 = _con20898->tail;
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
    struct kk_std_core_Cons* _con20899 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con20899->head;
    kk_std_core__list _pat0 = _con20899->tail;
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
  kk_std_core__sslice _x20900;
  bool _match_19665;
  kk_integer_t _brw_19670;
  kk_std_core_types__optional _x20901 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
  if (kk_std_core_types__is_Optional(_x20901)) {
    kk_box_t _box_x19069 = _x20901._cons.Optional.value;
    kk_integer_t _n_10142 = kk_integer_unbox(_box_x19069);
    _brw_19670 = _n_10142; /*int*/
  }
  else {
    _brw_19670 = kk_integer_from_small(1); /*int*/
  }
  bool _brw_19671 = kk_integer_eq_borrow(_brw_19670,(kk_integer_from_small(1)),kk_context()); /*bool*/;
  kk_integer_drop(_brw_19670, _ctx);
  _match_19665 = _brw_19671; /*bool*/
  if (_match_19665) {
    _x20900 = slice0; /*sslice*/
  }
  else {
    kk_integer_t _brw_19667;
    kk_integer_t _x20903;
    kk_std_core_types__optional _x20904 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
    if (kk_std_core_types__is_Optional(_x20904)) {
      kk_box_t _box_x19070 = _x20904._cons.Optional.value;
      kk_integer_t _n_101420 = kk_integer_unbox(_box_x19070);
      _x20903 = _n_101420; /*int*/
    }
    else {
      _x20903 = kk_integer_from_small(1); /*int*/
    }
    _brw_19667 = kk_integer_add_small_const(_x20903, -1, _ctx); /*int*/
    kk_std_core__sslice _brw_19668 = kk_std_core_extend(slice0, _brw_19667, _ctx); /*sslice*/;
    kk_integer_drop(_brw_19667, _ctx);
    _x20900 = _brw_19668; /*sslice*/
  }
  return kk_std_core_string_3(_x20900, _ctx);
}
 
// Return the first character of a string (or `Nothing` for the empty string).


// lift anonymous function
struct kk_std_core_head_char_fun20910__t {
  struct kk_function_s _base;
};
static kk_std_core_types__maybe kk_std_core_head_char_fun20910(kk_function_t _fself, kk_char_t _b_19071, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_head_char_fun20910(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_head_char_fun20910, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core_types__maybe kk_std_core_head_char_fun20910(kk_function_t _fself, kk_char_t _b_19071, kk_context_t* _ctx) {
  kk_unused(_fself);
  return kk_std_core_types__new_Just(kk_char_box(_b_19071, _ctx), _ctx);
}

kk_std_core_types__maybe kk_std_core_head_char(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> maybe<char> */ 
  kk_std_core__sslice _x20906;
  kk_string_t _x20907 = kk_string_dup(s); /*string*/
  kk_ssize_t _x20908 = (KK_IZ(0)); /*ssize_t*/
  kk_ssize_t _x20909 = kk_string_len(s,kk_context()); /*ssize_t*/
  _x20906 = kk_std_core__new_Sslice(_x20907, _x20908, _x20909, _ctx); /*sslice*/
  return kk_std_core_foreach_while_1(_x20906, kk_std_core_new_head_char_fun20910(_ctx), _ctx);
}

kk_integer_t kk_std_core_index_of_acc(kk_std_core__list xs, kk_function_t pred, kk_integer_t idx, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, pred : (a) -> bool, idx : int) -> int */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con20911 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con20911->head;
    kk_std_core__list xx = _con20911->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    bool _match_19664;
    kk_function_t _x20912 = kk_function_dup(pred); /*(13082) -> bool*/
    _match_19664 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x20912, (_x20912, x, _ctx)); /*bool*/
    if (_match_19664) {
      kk_std_core__list_drop(xx, _ctx);
      kk_function_drop(pred, _ctx);
      return idx;
    }
    { // tailcall
      kk_integer_t _x20913 = kk_integer_add_small_const(idx, 1, _ctx); /*int*/
      xs = xx;
      idx = _x20913;
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
    struct kk_std_core_Cons* _con20914 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con20914->head;
    kk_std_core__list xx = _con20914->tail;
    if (kk_std_core__is_Cons(xx)) {
      struct kk_std_core_Cons* _con20915 = kk_std_core__as_Cons(xx);
      kk_reuse_t _ru_19942 = kk_reuse_null; /*reuse*/;
      if (kk_likely(kk_std_core__list_is_unique(xs))) {
        _ru_19942 = (kk_std_core__list_reuse(xs));
      }
      else {
        kk_box_dup(x);
        kk_std_core__list_dup(xx);
        kk_std_core__list_decref(xs, _ctx);
      }
      kk_std_core__list _ctail_17288 = kk_std_core__list_hole(); /*list<13119>*/;
      kk_std_core__list _ctail_17289 = kk_std_core__new_Cons(_ru_19942, x, _ctail_17288, _ctx); /*list<13119>*/;
      kk_box_t* _b_19082_19077 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17289)->tail)); /*cfield<list<13119>>*/;
      { // tailcall
        kk_std_core_types__ctail _x20916 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17289, _ctx)),_b_19082_19077); /*ctail<0>*/
        xs = xx;
        _acc = _x20916;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_std_core__list_drop(xs, _ctx);
    kk_box_t _x20917 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20917, _ctx);
  }
}
 
// Return the list without its last element.
// Return an empty list for an empty list.

kk_std_core__list kk_std_core_init(kk_std_core__list xs0, kk_context_t* _ctx) { /* forall<a> (xs : list<a>) -> list<a> */ 
  kk_std_core_types__ctail _x20918 = kk_ctail_nil(); /*ctail<0>*/
  return kk_std_core__ctail_init(xs0, _x20918, _ctx);
}
 
// An invalid slice

kk_std_core__sslice kk_std_core_invalid;
 
// Is the character an ASCII letter is-

bool kk_std_core_is_alpha(kk_char_t c, kk_context_t* _ctx) { /* (c : char) -> bool */ 
  bool _match_19658 = (c >= ('a')); /*bool*/;
  if (_match_19658) {
    bool _match_19660 = (c <= ('z')); /*bool*/;
    if (_match_19660) {
      return true;
    }
    {
      bool _match_19661 = (c >= ('A')); /*bool*/;
      if (_match_19661) {
        return (c <= ('Z'));
      }
      {
        return false;
      }
    }
  }
  {
    bool _match_19659 = (c >= ('A')); /*bool*/;
    if (_match_19659) {
      return (c <= ('Z'));
    }
    {
      return false;
    }
  }
}
 
// Is the character an ASCII hexa-decimal digit ?

bool kk_std_core_is_hex_digit(kk_char_t c, kk_context_t* _ctx) { /* (c : char) -> bool */ 
  bool _match_19645 = (c >= ('0')); /*bool*/;
  if (_match_19645) {
    bool _match_19650 = (c <= ('9')); /*bool*/;
    if (_match_19650) {
      return true;
    }
    {
      bool _match_19651 = (c >= ('a')); /*bool*/;
      if (_match_19651) {
        bool _match_19653 = (c <= ('f')); /*bool*/;
        if (_match_19653) {
          return true;
        }
        {
          bool _match_19654 = (c >= ('A')); /*bool*/;
          if (_match_19654) {
            return (c <= ('F'));
          }
          {
            return false;
          }
        }
      }
      {
        bool _match_19652 = (c >= ('A')); /*bool*/;
        if (_match_19652) {
          return (c <= ('F'));
        }
        {
          return false;
        }
      }
    }
  }
  {
    bool _match_19646 = (c >= ('a')); /*bool*/;
    if (_match_19646) {
      bool _match_19648 = (c <= ('f')); /*bool*/;
      if (_match_19648) {
        return true;
      }
      {
        bool _match_19649 = (c >= ('A')); /*bool*/;
        if (_match_19649) {
          return (c <= ('F'));
        }
        {
          return false;
        }
      }
    }
    {
      bool _match_19647 = (c >= ('A')); /*bool*/;
      if (_match_19647) {
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
  bool _match_19642 = (c == (' ')); /*bool*/;
  if (_match_19642) {
    return true;
  }
  {
    bool _match_19643 = (c == 0x0009); /*bool*/;
    if (_match_19643) {
      return true;
    }
    {
      bool _match_19644 = (c == 0x000A); /*bool*/;
      if (_match_19644) {
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
    kk_string_t _x20929;
    if (kk_std_core__is_Nil(xs)) {
      _x20929 = kk_string_empty(); /*string*/
    }
    else {
      struct kk_std_core_Cons* _con20931 = kk_std_core__as_Cons(xs);
      kk_box_t _box_x19088 = _con20931->head;
      kk_std_core__list xx = _con20931->tail;
      kk_string_t x = kk_string_unbox(_box_x19088);
      if (kk_likely(kk_std_core__list_is_unique(xs))) {
        kk_std_core__list_free(xs, _ctx);
      }
      else {
        kk_string_dup(x);
        kk_std_core__list_dup(xx);
        kk_std_core__list_decref(xs, _ctx);
      }
      kk_string_t _x20933 = kk_string_dup(end); /*string*/
      _x20929 = kk_std_core__lift17188_joinsep(_x20933, xx, x, _ctx); /*string*/
    }
    return kk_std_core__lp__plus__plus__1_rp_(_x20929, end, _ctx);
  }
}
 
// Return the last element of a list (or `Nothing` for the empty list)

kk_std_core_types__maybe kk_std_core_last(kk_std_core__list xs, kk_context_t* _ctx) { /* forall<a> (xs : list<a>) -> maybe<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con20934 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con20934->head;
    kk_std_core__list _pat0 = _con20934->tail;
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
    struct kk_std_core_Cons* _con20935 = kk_std_core__as_Cons(xs);
    kk_box_t _pat2 = _con20935->head;
    kk_std_core__list xx = _con20935->tail;
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
    struct kk_std_core_Cons* _con20936 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con20936->head;
    kk_std_core__list _pat0 = _con20936->tail;
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
    struct kk_std_core_Cons* _con20937 = kk_std_core__as_Cons(xs);
    kk_box_t _pat2 = _con20937->head;
    kk_std_core__list xx = _con20937->tail;
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
  bool _match_19634;
  kk_integer_t _brw_19640;
  if (kk_std_core_types__is_Optional(n)) {
    kk_box_t _box_x19089 = n._cons.Optional.value;
    kk_integer_t _n_13713 = kk_integer_unbox(_box_x19089);
    kk_integer_dup(_n_13713);
    _brw_19640 = _n_13713; /*int*/
  }
  else {
    _brw_19640 = kk_integer_from_small(1); /*int*/
  }
  bool _brw_19641 = kk_integer_eq_borrow(_brw_19640,(kk_integer_from_small(1)),kk_context()); /*bool*/;
  kk_integer_drop(_brw_19640, _ctx);
  _match_19634 = _brw_19641; /*bool*/
  if (_match_19634) {
    kk_std_core_types__optional_drop(n, _ctx);
    return slice0;
  }
  {
    kk_std_core__sslice _own_19636;
    kk_integer_t _brw_19637;
    kk_integer_t _x20939;
    if (kk_std_core_types__is_Optional(n)) {
      kk_box_t _box_x19090 = n._cons.Optional.value;
      kk_integer_t _n_137130 = kk_integer_unbox(_box_x19090);
      kk_integer_dup(_n_137130);
      _x20939 = _n_137130; /*int*/
    }
    else {
      _x20939 = kk_integer_from_small(1); /*int*/
    }
    _brw_19637 = kk_integer_sub((kk_integer_from_small(1)),_x20939,kk_context()); /*int*/
    kk_std_core__sslice _brw_19638 = kk_std_core_advance(slice0, _brw_19637, _ctx); /*sslice*/;
    kk_integer_drop(_brw_19637, _ctx);
    _own_19636 = _brw_19638; /*sslice*/
    kk_integer_t _brw_19635;
    kk_integer_t _x20941;
    if (kk_std_core_types__is_Optional(n)) {
      kk_box_t _box_x19091 = n._cons.Optional.value;
      kk_integer_t _n_137131 = kk_integer_unbox(_box_x19091);
      _x20941 = _n_137131; /*int*/
    }
    else {
      _x20941 = kk_integer_from_small(1); /*int*/
    }
    _brw_19635 = kk_integer_add_small_const(_x20941, -1, _ctx); /*int*/
    kk_std_core__sslice _brw_19639 = kk_std_core_extend(_own_19636, _brw_19635, _ctx); /*sslice*/;
    kk_integer_drop(_brw_19635, _ctx);
    return _brw_19639;
  }
}
 
// Take the first `n` elements of a list (or fewer if the list is shorter than `n`)

kk_std_core__list kk_std_core__ctail_take(kk_std_core__list xs, kk_integer_t n, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, n : int, ctail<list<a>>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con20943 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con20943->head;
    kk_std_core__list xx = _con20943->tail;
    if (kk_integer_gt_borrow(n,(kk_integer_from_small(0)),kk_context())) {
      kk_reuse_t _ru_19948 = kk_reuse_null; /*reuse*/;
      if (kk_likely(kk_std_core__list_is_unique(xs))) {
        _ru_19948 = (kk_std_core__list_reuse(xs));
      }
      else {
        kk_box_dup(x);
        kk_std_core__list_dup(xx);
        kk_std_core__list_decref(xs, _ctx);
      }
      kk_std_core__list _ctail_17290 = kk_std_core__list_hole(); /*list<13837>*/;
      kk_std_core__list _ctail_17291 = kk_std_core__new_Cons(_ru_19948, x, _ctail_17290, _ctx); /*list<13837>*/;
      kk_box_t* _b_19102_19097 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17291)->tail)); /*cfield<list<13837>>*/;
      { // tailcall
        kk_integer_t _x20944 = kk_integer_add_small_const(n, -1, _ctx); /*int*/
        kk_std_core_types__ctail _x20945 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17291, _ctx)),_b_19102_19097); /*ctail<0>*/
        xs = xx;
        n = _x20944;
        _acc = _x20945;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_std_core__list_drop(xs, _ctx);
    kk_integer_drop(n, _ctx);
    kk_box_t _x20946 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20946, _ctx);
  }
}
 
// Take the first `n` elements of a list (or fewer if the list is shorter than `n`)

kk_std_core__list kk_std_core_take(kk_std_core__list xs0, kk_integer_t n0, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, n : int) -> list<a> */ 
  kk_std_core_types__ctail _x20947 = kk_ctail_nil(); /*ctail<0>*/
  return kk_std_core__ctail_take(xs0, n0, _x20947, _ctx);
}
 
// split a list at position `n`

kk_std_core_types__tuple2_ kk_std_core_split(kk_std_core__list xs, kk_integer_t n, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, n : int) -> (list<a>, list<a>) */ 
  kk_std_core__list _b_19110_19108;
  kk_std_core__list _x20948 = kk_std_core__list_dup(xs); /*list<13857>*/
  kk_integer_t _x20949 = kk_integer_dup(n); /*int*/
  _b_19110_19108 = kk_std_core_take(_x20948, _x20949, _ctx); /*list<13857>*/
  kk_std_core__list _b_19111_19109 = kk_std_core_drop(xs, n, _ctx); /*list<13857>*/;
  return kk_std_core_types__new_dash__lp__comma__rp_(kk_std_core__list_box(_b_19110_19108, _ctx), kk_std_core__list_box(_b_19111_19109, _ctx), _ctx);
}
 
// Lookup the first element satisfying some predicate


// lift anonymous function
struct kk_std_core_lookup_fun20954__t {
  struct kk_function_s _base;
  kk_function_t pred;
};
static kk_std_core_types__maybe kk_std_core_lookup_fun20954(kk_function_t _fself, kk_box_t _b_19114, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_lookup_fun20954(kk_function_t pred, kk_context_t* _ctx) {
  struct kk_std_core_lookup_fun20954__t* _self = kk_function_alloc_as(struct kk_std_core_lookup_fun20954__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_lookup_fun20954, kk_context());
  _self->pred = pred;
  return &_self->_base;
}

static kk_std_core_types__maybe kk_std_core_lookup_fun20954(kk_function_t _fself, kk_box_t _b_19114, kk_context_t* _ctx) {
  struct kk_std_core_lookup_fun20954__t* _self = kk_function_as(struct kk_std_core_lookup_fun20954__t*, _fself);
  kk_function_t pred = _self->pred; /* (14189) -> bool */
  kk_drop_match(_self, {kk_function_dup(pred);}, {}, _ctx)
  bool _match_19631;
  kk_box_t _x20955;
  kk_std_core_types__tuple2_ _match_19633;
  kk_box_t _x20956 = kk_box_dup(_b_19114); /*10675*/
  _match_19633 = kk_std_core_types__tuple2__unbox(_x20956, _ctx); /*(14189, 14190)*/
  {
    kk_box_t _x = _match_19633.fst;
    kk_box_dup(_x);
    kk_std_core_types__tuple2__drop(_match_19633, _ctx);
    _x20955 = _x; /*14189*/
  }
  _match_19631 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), pred, (pred, _x20955, _ctx)); /*bool*/
  if (_match_19631) {
    kk_box_t _x20957;
    kk_std_core_types__tuple2_ _match_19632 = kk_std_core_types__tuple2__unbox(_b_19114, _ctx); /*(14189, 14190)*/;
    {
      kk_box_t _x0 = _match_19632.snd;
      kk_box_dup(_x0);
      kk_std_core_types__tuple2__drop(_match_19632, _ctx);
      _x20957 = _x0; /*14190*/
    }
    return kk_std_core_types__new_Just(_x20957, _ctx);
  }
  {
    kk_box_drop(_b_19114, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
}

kk_std_core_types__maybe kk_std_core_lookup(kk_std_core__list xs, kk_function_t pred, kk_context_t* _ctx) { /* forall<a,b> (xs : list<(a, b)>, pred : (a) -> bool) -> maybe<b> */ 
  return kk_std_core_foreach_while(xs, kk_std_core_new_lookup_fun20954(pred, _ctx), _ctx);
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift17722_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_integer_t i0_17228, kk_std_core__list yy, kk_box_t _ctail_17292, kk_context_t* _ctx) { /* forall<a,b,e> (ctail<list<b>>, f : (idx : int, value : a) -> e b, i0.17228 : int, yy : list<a>, b) -> e list<b> */ 
  kk_std_core__list _ctail_17293 = kk_std_core__list_hole(); /*list<14245>*/;
  kk_std_core__list _ctail_17294 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17292, _ctail_17293, _ctx); /*list<14245>*/;
  kk_box_t* _b_19126_19123 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17294)->tail)); /*cfield<list<14245>>*/;
  kk_std_core_types__ctail _x20958 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17294, _ctx)),_b_19126_19123); /*ctail<0>*/
  return kk_std_core__ctail_lift17198_map_indexed(f, yy, i0_17228, _x20958, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift17723_op_fun20959__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t _ctail_17297;
};
static kk_std_core__list kk_std_core__mlift17723_op_fun20959(kk_function_t _fself, kk_std_core__list _ctail_17296, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17723_op_fun20959(kk_function_t _accm, kk_box_t _ctail_17297, kk_context_t* _ctx) {
  struct kk_std_core__mlift17723_op_fun20959__t* _self = kk_function_alloc_as(struct kk_std_core__mlift17723_op_fun20959__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17723_op_fun20959, kk_context());
  _self->_accm = _accm;
  _self->_ctail_17297 = _ctail_17297;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift17723_op_fun20959(kk_function_t _fself, kk_std_core__list _ctail_17296, kk_context_t* _ctx) {
  struct kk_std_core__mlift17723_op_fun20959__t* _self = kk_function_as(struct kk_std_core__mlift17723_op_fun20959__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<14245>) -> list<14245> */
  kk_box_t _ctail_17297 = _self->_ctail_17297; /* 14245 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(_ctail_17297);}, {}, _ctx)
  kk_std_core__list _x20960 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17297, _ctail_17296, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x20960, _ctx));
}

kk_std_core__list kk_std_core__mlift17723_op(kk_function_t _accm, kk_function_t f0, kk_integer_t i0_172280, kk_std_core__list yy0, kk_box_t _ctail_17297, kk_context_t* _ctx) { /* forall<a,b,e> ((list<b>) -> list<b>, f : (idx : int, value : a) -> e b, i0.17228 : int, yy : list<a>, b) -> e list<b> */ 
  return kk_std_core__ctailm_lift17198_map_indexed(f0, yy0, i0_172280, kk_std_core__new_mlift17723_op_fun20959(_accm, _ctail_17297, _ctx), _ctx);
}
 
// lifted local: map-indexed, map-idx


// lift anonymous function
struct kk_std_core__ctail_lift17198_map_indexed_fun20965__t {
  struct kk_function_s _base;
  kk_function_t f1;
  kk_integer_t i0_172281;
  kk_std_core__list yy1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_lift17198_map_indexed_fun20965(kk_function_t _fself, kk_box_t _b_19131, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_lift17198_map_indexed_fun20965(kk_function_t f1, kk_integer_t i0_172281, kk_std_core__list yy1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_lift17198_map_indexed_fun20965__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_lift17198_map_indexed_fun20965__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_lift17198_map_indexed_fun20965, kk_context());
  _self->f1 = f1;
  _self->i0_172281 = i0_172281;
  _self->yy1 = yy1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_lift17198_map_indexed_fun20965(kk_function_t _fself, kk_box_t _b_19131, kk_context_t* _ctx) {
  struct kk_std_core__ctail_lift17198_map_indexed_fun20965__t* _self = kk_function_as(struct kk_std_core__ctail_lift17198_map_indexed_fun20965__t*, _fself);
  kk_function_t f1 = _self->f1; /* (idx : int, value : 14244) -> 14246 14245 */
  kk_integer_t i0_172281 = _self->i0_172281; /* int */
  kk_std_core__list yy1 = _self->yy1; /* list<14244> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<14245>> */
  kk_drop_match(_self, {kk_function_dup(f1);kk_integer_dup(i0_172281);kk_std_core__list_dup(yy1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_box_t _ctail_19149_172920 = _b_19131; /*14245*/;
  kk_std_core__list _x20966 = kk_std_core__mlift17722_op(_acc0, f1, i0_172281, yy1, _ctail_19149_172920, _ctx); /*list<14245>*/
  return kk_std_core__list_box(_x20966, _ctx);
}

kk_std_core__list kk_std_core__ctail_lift17198_map_indexed(kk_function_t f1, kk_std_core__list ys, kk_integer_t i, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,b,e> (f : (idx : int, value : a) -> e b, ys : list<a>, i : int, ctail<list<b>>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con20961 = kk_std_core__as_Cons(ys);
    kk_box_t y = _con20961->head;
    kk_std_core__list yy1 = _con20961->tail;
    kk_reuse_t _ru_19949 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      _ru_19949 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(y);
      kk_std_core__list_dup(yy1);
      kk_std_core__list_decref(ys, _ctx);
    }
    kk_integer_t i0_172281;
    kk_integer_t _x20962 = kk_integer_dup(i); /*int*/
    i0_172281 = kk_integer_add_small_const(_x20962, 1, _ctx); /*int*/
    kk_box_t x_18022;
    kk_function_t _x20963 = kk_function_dup(f1); /*(idx : int, value : 14244) -> 14246 14245*/
    x_18022 = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_box_t, kk_context_t*), _x20963, (_x20963, i, y, _ctx)); /*14245*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_19949,kk_context());
      kk_box_drop(x_18022, _ctx);
      kk_box_t _x20964 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_lift17198_map_indexed_fun20965(f1, i0_172281, yy1, _acc0, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20964, _ctx);
    }
    {
      kk_std_core__list _ctail_172930 = kk_std_core__list_hole(); /*list<14245>*/;
      kk_std_core__list _ctail_172940 = kk_std_core__new_Cons(_ru_19949, x_18022, _ctail_172930, _ctx); /*list<14245>*/;
      kk_box_t* _b_19143_19137 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_172940)->tail)); /*cfield<list<14245>>*/;
      { // tailcall
        kk_std_core_types__ctail _x20967 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_172940, _ctx)),_b_19143_19137); /*ctail<0>*/
        ys = yy1;
        i = i0_172281;
        _acc0 = _x20967;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_integer_drop(i, _ctx);
    kk_function_drop(f1, _ctx);
    kk_box_t _x20968 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20968, _ctx);
  }
}
 
// lifted local: map-indexed, map-idx


// lift anonymous function
struct kk_std_core__ctailm_lift17198_map_indexed_fun20973__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t f2;
  kk_integer_t i0_172282;
  kk_std_core__list yy2;
};
static kk_box_t kk_std_core__ctailm_lift17198_map_indexed_fun20973(kk_function_t _fself, kk_box_t _b_19151, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_lift17198_map_indexed_fun20973(kk_function_t _accm0, kk_function_t f2, kk_integer_t i0_172282, kk_std_core__list yy2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17198_map_indexed_fun20973__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_lift17198_map_indexed_fun20973__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_lift17198_map_indexed_fun20973, kk_context());
  _self->_accm0 = _accm0;
  _self->f2 = f2;
  _self->i0_172282 = i0_172282;
  _self->yy2 = yy2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_lift17198_map_indexed_fun20973(kk_function_t _fself, kk_box_t _b_19151, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17198_map_indexed_fun20973__t* _self = kk_function_as(struct kk_std_core__ctailm_lift17198_map_indexed_fun20973__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<14245>) -> list<14245> */
  kk_function_t f2 = _self->f2; /* (idx : int, value : 14244) -> 14246 14245 */
  kk_integer_t i0_172282 = _self->i0_172282; /* int */
  kk_std_core__list yy2 = _self->yy2; /* list<14244> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(f2);kk_integer_dup(i0_172282);kk_std_core__list_dup(yy2);}, {}, _ctx)
  kk_box_t _ctail_19153_172970 = _b_19151; /*14245*/;
  kk_std_core__list _x20974 = kk_std_core__mlift17723_op(_accm0, f2, i0_172282, yy2, _ctail_19153_172970, _ctx); /*list<14245>*/
  return kk_std_core__list_box(_x20974, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_lift17198_map_indexed_fun20976__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t x0_18025;
};
static kk_std_core__list kk_std_core__ctailm_lift17198_map_indexed_fun20976(kk_function_t _fself, kk_std_core__list _ctail_172960, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_lift17198_map_indexed_fun20976(kk_function_t _accm0, kk_box_t x0_18025, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17198_map_indexed_fun20976__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_lift17198_map_indexed_fun20976__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_lift17198_map_indexed_fun20976, kk_context());
  _self->_accm0 = _accm0;
  _self->x0_18025 = x0_18025;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_lift17198_map_indexed_fun20976(kk_function_t _fself, kk_std_core__list _ctail_172960, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17198_map_indexed_fun20976__t* _self = kk_function_as(struct kk_std_core__ctailm_lift17198_map_indexed_fun20976__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<14245>) -> list<14245> */
  kk_box_t x0_18025 = _self->x0_18025; /* 14245 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(x0_18025);}, {}, _ctx)
  kk_std_core__list _x20977 = kk_std_core__new_Cons(kk_reuse_null, x0_18025, _ctail_172960, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x20977, _ctx));
}

kk_std_core__list kk_std_core__ctailm_lift17198_map_indexed(kk_function_t f2, kk_std_core__list ys0, kk_integer_t i0, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,b,e> (f : (idx : int, value : a) -> e b, ys : list<a>, i : int, (list<b>) -> list<b>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys0)) {
    struct kk_std_core_Cons* _con20969 = kk_std_core__as_Cons(ys0);
    kk_box_t y0 = _con20969->head;
    kk_std_core__list yy2 = _con20969->tail;
    if (kk_likely(kk_std_core__list_is_unique(ys0))) {
      kk_std_core__list_free(ys0, _ctx);
    }
    else {
      kk_box_dup(y0);
      kk_std_core__list_dup(yy2);
      kk_std_core__list_decref(ys0, _ctx);
    }
    kk_integer_t i0_172282;
    kk_integer_t _x20970 = kk_integer_dup(i0); /*int*/
    i0_172282 = kk_integer_add_small_const(_x20970, 1, _ctx); /*int*/
    kk_box_t x0_18025;
    kk_function_t _x20971 = kk_function_dup(f2); /*(idx : int, value : 14244) -> 14246 14245*/
    x0_18025 = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_box_t, kk_context_t*), _x20971, (_x20971, i0, y0, _ctx)); /*14245*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x0_18025, _ctx);
      kk_box_t _x20972 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_lift17198_map_indexed_fun20973(_accm0, f2, i0_172282, yy2, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20972, _ctx);
    }
    { // tailcall
      kk_function_t _x20975 = kk_std_core__new_ctailm_lift17198_map_indexed_fun20976(_accm0, x0_18025, _ctx); /*(list<14245>) -> list<14245>*/
      ys0 = yy2;
      i0 = i0_172282;
      _accm0 = _x20975;
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
struct kk_std_core__lift17198_map_indexed_fun20979__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core__lift17198_map_indexed_fun20979(kk_function_t _fself, kk_std_core__list _ctail_17295, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17198_map_indexed_fun20979(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core__lift17198_map_indexed_fun20979, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core__lift17198_map_indexed_fun20979(kk_function_t _fself, kk_std_core__list _ctail_17295, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17295;
}

kk_std_core__list kk_std_core__lift17198_map_indexed(kk_function_t f3, kk_std_core__list ys1, kk_integer_t i1, kk_context_t* _ctx) { /* forall<a,b,e> (f : (idx : int, value : a) -> e b, ys : list<a>, i : int) -> e list<b> */ 
  bool _match_19628 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_19628) {
    kk_std_core_types__ctail _x20978 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_lift17198_map_indexed(f3, ys1, i1, _x20978, _ctx);
  }
  {
    return kk_std_core__ctailm_lift17198_map_indexed(f3, ys1, i1, kk_std_core__new_lift17198_map_indexed_fun20979(_ctx), _ctx);
  }
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift17724_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_integer_t i0_17230, kk_std_core__list yy, kk_box_t _ctail_17298, kk_context_t* _ctx) { /* forall<a,b,e> (ctail<list<b>>, f : (idx : int, value : a, rest : list<a>) -> e b, i0.17230 : int, yy : list<a>, b) -> e list<b> */ 
  kk_std_core__list _ctail_17299 = kk_std_core__list_hole(); /*list<14302>*/;
  kk_std_core__list _ctail_17300 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17298, _ctail_17299, _ctx); /*list<14302>*/;
  kk_box_t* _b_19162_19159 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17300)->tail)); /*cfield<list<14302>>*/;
  kk_std_core_types__ctail _x20980 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17300, _ctx)),_b_19162_19159); /*ctail<0>*/
  return kk_std_core__ctail_lift17199_map_indexed_peek(f, yy, i0_17230, _x20980, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift17725_op_fun20981__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t _ctail_17303;
};
static kk_std_core__list kk_std_core__mlift17725_op_fun20981(kk_function_t _fself, kk_std_core__list _ctail_17302, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17725_op_fun20981(kk_function_t _accm, kk_box_t _ctail_17303, kk_context_t* _ctx) {
  struct kk_std_core__mlift17725_op_fun20981__t* _self = kk_function_alloc_as(struct kk_std_core__mlift17725_op_fun20981__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17725_op_fun20981, kk_context());
  _self->_accm = _accm;
  _self->_ctail_17303 = _ctail_17303;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift17725_op_fun20981(kk_function_t _fself, kk_std_core__list _ctail_17302, kk_context_t* _ctx) {
  struct kk_std_core__mlift17725_op_fun20981__t* _self = kk_function_as(struct kk_std_core__mlift17725_op_fun20981__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<14302>) -> list<14302> */
  kk_box_t _ctail_17303 = _self->_ctail_17303; /* 14302 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(_ctail_17303);}, {}, _ctx)
  kk_std_core__list _x20982 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17303, _ctail_17302, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x20982, _ctx));
}

kk_std_core__list kk_std_core__mlift17725_op(kk_function_t _accm, kk_function_t f0, kk_integer_t i0_172300, kk_std_core__list yy0, kk_box_t _ctail_17303, kk_context_t* _ctx) { /* forall<a,b,e> ((list<b>) -> list<b>, f : (idx : int, value : a, rest : list<a>) -> e b, i0.17230 : int, yy : list<a>, b) -> e list<b> */ 
  return kk_std_core__ctailm_lift17199_map_indexed_peek(f0, yy0, i0_172300, kk_std_core__new_mlift17725_op_fun20981(_accm, _ctail_17303, _ctx), _ctx);
}
 
// lifted local: map-indexed-peek, mapidx


// lift anonymous function
struct kk_std_core__ctail_lift17199_map_indexed_peek_fun20988__t {
  struct kk_function_s _base;
  kk_function_t f1;
  kk_integer_t i0_172301;
  kk_std_core__list yy1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_lift17199_map_indexed_peek_fun20988(kk_function_t _fself, kk_box_t _b_19167, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_lift17199_map_indexed_peek_fun20988(kk_function_t f1, kk_integer_t i0_172301, kk_std_core__list yy1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_lift17199_map_indexed_peek_fun20988__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_lift17199_map_indexed_peek_fun20988__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_lift17199_map_indexed_peek_fun20988, kk_context());
  _self->f1 = f1;
  _self->i0_172301 = i0_172301;
  _self->yy1 = yy1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_lift17199_map_indexed_peek_fun20988(kk_function_t _fself, kk_box_t _b_19167, kk_context_t* _ctx) {
  struct kk_std_core__ctail_lift17199_map_indexed_peek_fun20988__t* _self = kk_function_as(struct kk_std_core__ctail_lift17199_map_indexed_peek_fun20988__t*, _fself);
  kk_function_t f1 = _self->f1; /* (idx : int, value : 14301, rest : list<14301>) -> 14303 14302 */
  kk_integer_t i0_172301 = _self->i0_172301; /* int */
  kk_std_core__list yy1 = _self->yy1; /* list<14301> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<14302>> */
  kk_drop_match(_self, {kk_function_dup(f1);kk_integer_dup(i0_172301);kk_std_core__list_dup(yy1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_box_t _ctail_19185_172980 = _b_19167; /*14302*/;
  kk_std_core__list _x20989 = kk_std_core__mlift17724_op(_acc0, f1, i0_172301, yy1, _ctail_19185_172980, _ctx); /*list<14302>*/
  return kk_std_core__list_box(_x20989, _ctx);
}

kk_std_core__list kk_std_core__ctail_lift17199_map_indexed_peek(kk_function_t f1, kk_std_core__list ys, kk_integer_t i, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,b,e> (f : (idx : int, value : a, rest : list<a>) -> e b, ys : list<a>, i : int, ctail<list<b>>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con20983 = kk_std_core__as_Cons(ys);
    kk_box_t y = _con20983->head;
    kk_std_core__list yy1 = _con20983->tail;
    kk_reuse_t _ru_19951 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      _ru_19951 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(y);
      kk_std_core__list_dup(yy1);
      kk_std_core__list_decref(ys, _ctx);
    }
    kk_integer_t i0_172301;
    kk_integer_t _x20984 = kk_integer_dup(i); /*int*/
    i0_172301 = kk_integer_add_small_const(_x20984, 1, _ctx); /*int*/
    kk_box_t x_18028;
    kk_function_t _x20986 = kk_function_dup(f1); /*(idx : int, value : 14301, rest : list<14301>) -> 14303 14302*/
    kk_std_core__list _x20985 = kk_std_core__list_dup(yy1); /*list<14301>*/
    x_18028 = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_box_t, kk_std_core__list, kk_context_t*), _x20986, (_x20986, i, y, _x20985, _ctx)); /*14302*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_19951,kk_context());
      kk_box_drop(x_18028, _ctx);
      kk_box_t _x20987 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_lift17199_map_indexed_peek_fun20988(f1, i0_172301, yy1, _acc0, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20987, _ctx);
    }
    {
      kk_std_core__list _ctail_172990 = kk_std_core__list_hole(); /*list<14302>*/;
      kk_std_core__list _ctail_173000 = kk_std_core__new_Cons(_ru_19951, x_18028, _ctail_172990, _ctx); /*list<14302>*/;
      kk_box_t* _b_19179_19173 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_173000)->tail)); /*cfield<list<14302>>*/;
      { // tailcall
        kk_std_core_types__ctail _x20990 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_173000, _ctx)),_b_19179_19173); /*ctail<0>*/
        ys = yy1;
        i = i0_172301;
        _acc0 = _x20990;
        goto kk__tailcall;
      }
    }
  }
  {
    kk_integer_drop(i, _ctx);
    kk_function_drop(f1, _ctx);
    kk_box_t _x20991 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x20991, _ctx);
  }
}
 
// lifted local: map-indexed-peek, mapidx


// lift anonymous function
struct kk_std_core__ctailm_lift17199_map_indexed_peek_fun20997__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t f2;
  kk_integer_t i0_172302;
  kk_std_core__list yy2;
};
static kk_box_t kk_std_core__ctailm_lift17199_map_indexed_peek_fun20997(kk_function_t _fself, kk_box_t _b_19187, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_lift17199_map_indexed_peek_fun20997(kk_function_t _accm0, kk_function_t f2, kk_integer_t i0_172302, kk_std_core__list yy2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17199_map_indexed_peek_fun20997__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_lift17199_map_indexed_peek_fun20997__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_lift17199_map_indexed_peek_fun20997, kk_context());
  _self->_accm0 = _accm0;
  _self->f2 = f2;
  _self->i0_172302 = i0_172302;
  _self->yy2 = yy2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_lift17199_map_indexed_peek_fun20997(kk_function_t _fself, kk_box_t _b_19187, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17199_map_indexed_peek_fun20997__t* _self = kk_function_as(struct kk_std_core__ctailm_lift17199_map_indexed_peek_fun20997__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<14302>) -> list<14302> */
  kk_function_t f2 = _self->f2; /* (idx : int, value : 14301, rest : list<14301>) -> 14303 14302 */
  kk_integer_t i0_172302 = _self->i0_172302; /* int */
  kk_std_core__list yy2 = _self->yy2; /* list<14301> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(f2);kk_integer_dup(i0_172302);kk_std_core__list_dup(yy2);}, {}, _ctx)
  kk_box_t _ctail_19189_173030 = _b_19187; /*14302*/;
  kk_std_core__list _x20998 = kk_std_core__mlift17725_op(_accm0, f2, i0_172302, yy2, _ctail_19189_173030, _ctx); /*list<14302>*/
  return kk_std_core__list_box(_x20998, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_lift17199_map_indexed_peek_fun21000__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t x0_18031;
};
static kk_std_core__list kk_std_core__ctailm_lift17199_map_indexed_peek_fun21000(kk_function_t _fself, kk_std_core__list _ctail_173020, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_lift17199_map_indexed_peek_fun21000(kk_function_t _accm0, kk_box_t x0_18031, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17199_map_indexed_peek_fun21000__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_lift17199_map_indexed_peek_fun21000__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_lift17199_map_indexed_peek_fun21000, kk_context());
  _self->_accm0 = _accm0;
  _self->x0_18031 = x0_18031;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_lift17199_map_indexed_peek_fun21000(kk_function_t _fself, kk_std_core__list _ctail_173020, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17199_map_indexed_peek_fun21000__t* _self = kk_function_as(struct kk_std_core__ctailm_lift17199_map_indexed_peek_fun21000__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<14302>) -> list<14302> */
  kk_box_t x0_18031 = _self->x0_18031; /* 14302 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(x0_18031);}, {}, _ctx)
  kk_std_core__list _x21001 = kk_std_core__new_Cons(kk_reuse_null, x0_18031, _ctail_173020, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x21001, _ctx));
}

kk_std_core__list kk_std_core__ctailm_lift17199_map_indexed_peek(kk_function_t f2, kk_std_core__list ys0, kk_integer_t i0, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,b,e> (f : (idx : int, value : a, rest : list<a>) -> e b, ys : list<a>, i : int, (list<b>) -> list<b>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys0)) {
    struct kk_std_core_Cons* _con20992 = kk_std_core__as_Cons(ys0);
    kk_box_t y0 = _con20992->head;
    kk_std_core__list yy2 = _con20992->tail;
    if (kk_likely(kk_std_core__list_is_unique(ys0))) {
      kk_std_core__list_free(ys0, _ctx);
    }
    else {
      kk_box_dup(y0);
      kk_std_core__list_dup(yy2);
      kk_std_core__list_decref(ys0, _ctx);
    }
    kk_integer_t i0_172302;
    kk_integer_t _x20993 = kk_integer_dup(i0); /*int*/
    i0_172302 = kk_integer_add_small_const(_x20993, 1, _ctx); /*int*/
    kk_box_t x0_18031;
    kk_function_t _x20995 = kk_function_dup(f2); /*(idx : int, value : 14301, rest : list<14301>) -> 14303 14302*/
    kk_std_core__list _x20994 = kk_std_core__list_dup(yy2); /*list<14301>*/
    x0_18031 = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_box_t, kk_std_core__list, kk_context_t*), _x20995, (_x20995, i0, y0, _x20994, _ctx)); /*14302*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x0_18031, _ctx);
      kk_box_t _x20996 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_lift17199_map_indexed_peek_fun20997(_accm0, f2, i0_172302, yy2, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x20996, _ctx);
    }
    { // tailcall
      kk_function_t _x20999 = kk_std_core__new_ctailm_lift17199_map_indexed_peek_fun21000(_accm0, x0_18031, _ctx); /*(list<14302>) -> list<14302>*/
      ys0 = yy2;
      i0 = i0_172302;
      _accm0 = _x20999;
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
struct kk_std_core__lift17199_map_indexed_peek_fun21003__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core__lift17199_map_indexed_peek_fun21003(kk_function_t _fself, kk_std_core__list _ctail_17301, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17199_map_indexed_peek_fun21003(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core__lift17199_map_indexed_peek_fun21003, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core__lift17199_map_indexed_peek_fun21003(kk_function_t _fself, kk_std_core__list _ctail_17301, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17301;
}

kk_std_core__list kk_std_core__lift17199_map_indexed_peek(kk_function_t f3, kk_std_core__list ys1, kk_integer_t i1, kk_context_t* _ctx) { /* forall<a,b,e> (f : (idx : int, value : a, rest : list<a>) -> e b, ys : list<a>, i : int) -> e list<b> */ 
  bool _match_19625 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_19625) {
    kk_std_core_types__ctail _x21002 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_lift17199_map_indexed_peek(f3, ys1, i1, _x21002, _ctx);
  }
  {
    return kk_std_core__ctailm_lift17199_map_indexed_peek(f3, ys1, i1, kk_std_core__new_lift17199_map_indexed_peek_fun21003(_ctx), _ctx);
  }
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift17726_op(kk_std_core_types__ctail _acc, kk_function_t action, kk_std_core__list xx, kk_std_core_types__maybe _y_17557, kk_context_t* _ctx) { /* forall<a,b,e> (ctail<list<b>>, action : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e list<b> */ 
  if (kk_std_core_types__is_Just(_y_17557)) {
    kk_box_t y = _y_17557._cons.Just.value;
    kk_std_core__list _ctail_17304 = kk_std_core__list_hole(); /*list<14344>*/;
    kk_std_core__list _ctail_17305 = kk_std_core__new_Cons(kk_reuse_null, y, _ctail_17304, _ctx); /*list<14344>*/;
    kk_box_t* _b_19200_19195 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17305)->tail)); /*cfield<list<14344>>*/;
    kk_std_core_types__ctail _x21004 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17305, _ctx)),_b_19200_19195); /*ctail<0>*/
    return kk_std_core__ctail_map_while(xx, action, _x21004, _ctx);
  }
  {
    kk_std_core__list_drop(xx, _ctx);
    kk_function_drop(action, _ctx);
    kk_box_t _x21005 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21005, _ctx);
  }
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift17727_op_fun21006__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t y0;
};
static kk_std_core__list kk_std_core__mlift17727_op_fun21006(kk_function_t _fself, kk_std_core__list _ctail_17307, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17727_op_fun21006(kk_function_t _accm, kk_box_t y0, kk_context_t* _ctx) {
  struct kk_std_core__mlift17727_op_fun21006__t* _self = kk_function_alloc_as(struct kk_std_core__mlift17727_op_fun21006__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17727_op_fun21006, kk_context());
  _self->_accm = _accm;
  _self->y0 = y0;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift17727_op_fun21006(kk_function_t _fself, kk_std_core__list _ctail_17307, kk_context_t* _ctx) {
  struct kk_std_core__mlift17727_op_fun21006__t* _self = kk_function_as(struct kk_std_core__mlift17727_op_fun21006__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<14344>) -> list<14344> */
  kk_box_t y0 = _self->y0; /* 14344 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(y0);}, {}, _ctx)
  kk_std_core__list _x21007 = kk_std_core__new_Cons(kk_reuse_null, y0, _ctail_17307, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x21007, _ctx));
}

kk_std_core__list kk_std_core__mlift17727_op(kk_function_t _accm, kk_function_t action0, kk_std_core__list xx0, kk_std_core_types__maybe _y_17561, kk_context_t* _ctx) { /* forall<a,b,e> ((list<b>) -> list<b>, action : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e list<b> */ 
  if (kk_std_core_types__is_Just(_y_17561)) {
    kk_box_t y0 = _y_17561._cons.Just.value;
    return kk_std_core__ctailm_map_while(xx0, action0, kk_std_core__new_mlift17727_op_fun21006(_accm, y0, _ctx), _ctx);
  }
  {
    kk_std_core__list_drop(xx0, _ctx);
    kk_function_drop(action0, _ctx);
    return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, kk_std_core__new_Nil(_ctx), _ctx));
  }
}
 
// Invoke `action` on each element of a list while `action` returns `Just`


// lift anonymous function
struct kk_std_core__ctail_map_while_fun21012__t {
  struct kk_function_s _base;
  kk_function_t action1;
  kk_std_core__list xx1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_map_while_fun21012(kk_function_t _fself, kk_box_t _b_19209, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_map_while_fun21012(kk_function_t action1, kk_std_core__list xx1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_map_while_fun21012__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_map_while_fun21012__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_map_while_fun21012, kk_context());
  _self->action1 = action1;
  _self->xx1 = xx1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_map_while_fun21012(kk_function_t _fself, kk_box_t _b_19209, kk_context_t* _ctx) {
  struct kk_std_core__ctail_map_while_fun21012__t* _self = kk_function_as(struct kk_std_core__ctail_map_while_fun21012__t*, _fself);
  kk_function_t action1 = _self->action1; /* (14343) -> 14345 maybe<14344> */
  kk_std_core__list xx1 = _self->xx1; /* list<14343> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<14344>> */
  kk_drop_match(_self, {kk_function_dup(action1);kk_std_core__list_dup(xx1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_std_core_types__maybe _y_19229_175570 = kk_std_core_types__maybe_unbox(_b_19209, _ctx); /*maybe<14344>*/;
  kk_std_core__list _x21013 = kk_std_core__mlift17726_op(_acc0, action1, xx1, _y_19229_175570, _ctx); /*list<14344>*/
  return kk_std_core__list_box(_x21013, _ctx);
}

kk_std_core__list kk_std_core__ctail_map_while(kk_std_core__list xs, kk_function_t action1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, action : (a) -> e maybe<b>, ctail<list<b>>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Nil(xs)) {
    kk_function_drop(action1, _ctx);
    kk_box_t _x21008 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21008, _ctx);
  }
  {
    struct kk_std_core_Cons* _con21009 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con21009->head;
    kk_std_core__list xx1 = _con21009->tail;
    kk_reuse_t _ru_19953 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      _ru_19953 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx1);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_std_core_types__maybe x0_18034;
    kk_function_t _x21010 = kk_function_dup(action1); /*(14343) -> 14345 maybe<14344>*/
    x0_18034 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_box_t, kk_context_t*), _x21010, (_x21010, x, _ctx)); /*maybe<14344>*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_19953,kk_context());
      kk_std_core_types__maybe_drop(x0_18034, _ctx);
      kk_box_t _x21011 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_map_while_fun21012(action1, xx1, _acc0, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x21011, _ctx);
    }
    if (kk_std_core_types__is_Just(x0_18034)) {
      kk_box_t y1 = x0_18034._cons.Just.value;
      kk_std_core__list _ctail_173040 = kk_std_core__list_hole(); /*list<14344>*/;
      kk_std_core__list _ctail_173050 = kk_std_core__new_Cons(_ru_19953, y1, _ctail_173040, _ctx); /*list<14344>*/;
      kk_box_t* _b_19223_19215 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_173050)->tail)); /*cfield<list<14344>>*/;
      { // tailcall
        kk_std_core_types__ctail _x21014 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_173050, _ctx)),_b_19223_19215); /*ctail<0>*/
        xs = xx1;
        _acc0 = _x21014;
        goto kk__tailcall;
      }
    }
    {
      kk_reuse_drop(_ru_19953,kk_context());
      kk_std_core__list_drop(xx1, _ctx);
      kk_function_drop(action1, _ctx);
      kk_box_t _x21015 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
      return kk_std_core__list_unbox(_x21015, _ctx);
    }
  }
}
 
// Invoke `action` on each element of a list while `action` returns `Just`


// lift anonymous function
struct kk_std_core__ctailm_map_while_fun21019__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t action2;
  kk_std_core__list xx2;
};
static kk_box_t kk_std_core__ctailm_map_while_fun21019(kk_function_t _fself, kk_box_t _b_19231, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_map_while_fun21019(kk_function_t _accm0, kk_function_t action2, kk_std_core__list xx2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_map_while_fun21019__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_map_while_fun21019__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_map_while_fun21019, kk_context());
  _self->_accm0 = _accm0;
  _self->action2 = action2;
  _self->xx2 = xx2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_map_while_fun21019(kk_function_t _fself, kk_box_t _b_19231, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_map_while_fun21019__t* _self = kk_function_as(struct kk_std_core__ctailm_map_while_fun21019__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<14344>) -> list<14344> */
  kk_function_t action2 = _self->action2; /* (14343) -> 14345 maybe<14344> */
  kk_std_core__list xx2 = _self->xx2; /* list<14343> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(action2);kk_std_core__list_dup(xx2);}, {}, _ctx)
  kk_std_core_types__maybe _y_19233_175610 = kk_std_core_types__maybe_unbox(_b_19231, _ctx); /*maybe<14344>*/;
  kk_std_core__list _x21020 = kk_std_core__mlift17727_op(_accm0, action2, xx2, _y_19233_175610, _ctx); /*list<14344>*/
  return kk_std_core__list_box(_x21020, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_map_while_fun21022__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t y2;
};
static kk_std_core__list kk_std_core__ctailm_map_while_fun21022(kk_function_t _fself, kk_std_core__list _ctail_173070, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_map_while_fun21022(kk_function_t _accm0, kk_box_t y2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_map_while_fun21022__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_map_while_fun21022__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_map_while_fun21022, kk_context());
  _self->_accm0 = _accm0;
  _self->y2 = y2;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_map_while_fun21022(kk_function_t _fself, kk_std_core__list _ctail_173070, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_map_while_fun21022__t* _self = kk_function_as(struct kk_std_core__ctailm_map_while_fun21022__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<14344>) -> list<14344> */
  kk_box_t y2 = _self->y2; /* 14344 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(y2);}, {}, _ctx)
  kk_std_core__list _x21023 = kk_std_core__new_Cons(kk_reuse_null, y2, _ctail_173070, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x21023, _ctx));
}

kk_std_core__list kk_std_core__ctailm_map_while(kk_std_core__list xs0, kk_function_t action2, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, action : (a) -> e maybe<b>, (list<b>) -> list<b>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Nil(xs0)) {
    kk_function_drop(action2, _ctx);
    return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
  }
  {
    struct kk_std_core_Cons* _con21016 = kk_std_core__as_Cons(xs0);
    kk_box_t x1 = _con21016->head;
    kk_std_core__list xx2 = _con21016->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs0))) {
      kk_std_core__list_free(xs0, _ctx);
    }
    else {
      kk_box_dup(x1);
      kk_std_core__list_dup(xx2);
      kk_std_core__list_decref(xs0, _ctx);
    }
    kk_std_core_types__maybe x2_18037;
    kk_function_t _x21017 = kk_function_dup(action2); /*(14343) -> 14345 maybe<14344>*/
    x2_18037 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_box_t, kk_context_t*), _x21017, (_x21017, x1, _ctx)); /*maybe<14344>*/
    if (kk_yielding(kk_context())) {
      kk_std_core_types__maybe_drop(x2_18037, _ctx);
      kk_box_t _x21018 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_map_while_fun21019(_accm0, action2, xx2, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x21018, _ctx);
    }
    if (kk_std_core_types__is_Just(x2_18037)) {
      kk_box_t y2 = x2_18037._cons.Just.value;
      { // tailcall
        kk_function_t _x21021 = kk_std_core__new_ctailm_map_while_fun21022(_accm0, y2, _ctx); /*(list<14344>) -> list<14344>*/
        xs0 = xx2;
        _accm0 = _x21021;
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
struct kk_std_core_map_while_fun21025__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core_map_while_fun21025(kk_function_t _fself, kk_std_core__list _ctail_17306, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_while_fun21025(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_map_while_fun21025, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core_map_while_fun21025(kk_function_t _fself, kk_std_core__list _ctail_17306, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17306;
}

kk_std_core__list kk_std_core_map_while(kk_std_core__list xs1, kk_function_t action3, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, action : (a) -> e maybe<b>) -> e list<b> */ 
  bool _match_19622 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_19622) {
    kk_std_core_types__ctail _x21024 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_map_while(xs1, action3, _x21024, _ctx);
  }
  {
    return kk_std_core__ctailm_map_while(xs1, action3, kk_std_core_new_map_while_fun21025(_ctx), _ctx);
  }
}
 
// Returns the largest element of a list of integers (or `default` (=`0`) for the empty list)


// lift anonymous function
struct kk_std_core_maximum_fun21030__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_maximum_fun21030(kk_function_t _fself, kk_box_t _b_19239, kk_box_t _b_19240, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_maximum_fun21030(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_maximum_fun21030, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_maximum_fun21030(kk_function_t _fself, kk_box_t _b_19239, kk_box_t _b_19240, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_integer_t _x21031;
  kk_integer_t _x21032 = kk_integer_unbox(_b_19239); /*int*/
  kk_integer_t _x21033 = kk_integer_unbox(_b_19240); /*int*/
  _x21031 = kk_std_core_max(_x21032, _x21033, _ctx); /*int*/
  return kk_integer_box(_x21031);
}

kk_integer_t kk_std_core_maximum(kk_std_core__list xs, kk_std_core_types__optional default0, kk_context_t* _ctx) { /* (xs : list<int>, default : optional<int>) -> int */ 
  if (kk_std_core__is_Nil(xs)) {
    if (kk_std_core_types__is_Optional(default0)) {
      kk_box_t _box_x19234 = default0._cons.Optional.value;
      kk_integer_t _default_14393 = kk_integer_unbox(_box_x19234);
      return _default_14393;
    }
    {
      return kk_integer_from_small(0);
    }
  }
  {
    struct kk_std_core_Cons* _con21027 = kk_std_core__as_Cons(xs);
    kk_box_t _box_x19235 = _con21027->head;
    kk_std_core__list xx = _con21027->tail;
    kk_integer_t x = kk_integer_unbox(_box_x19235);
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_integer_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_std_core_types__optional_drop(default0, _ctx);
    kk_box_t _x21029 = kk_std_core_foldl(xx, kk_integer_box(x), kk_std_core_new_maximum_fun21030(_ctx), _ctx); /*12365*/
    return kk_integer_unbox(_x21029);
  }
}
 
// Returns the largest element of a list of floats (or `0` for the empty list)


// lift anonymous function
struct kk_std_core_maximum_fun21037__t_1 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_maximum_fun21037_1(kk_function_t _fself, kk_box_t _b_19248, kk_box_t _b_19249, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_maximum_fun21037_1(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_maximum_fun21037_1, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_maximum_fun21037_1(kk_function_t _fself, kk_box_t _b_19248, kk_box_t _b_19249, kk_context_t* _ctx) {
  kk_unused(_fself);
  double _x21038;
  double _x21039 = kk_double_unbox(_b_19248, _ctx); /*float64*/
  double _x21040 = kk_double_unbox(_b_19249, _ctx); /*float64*/
  _x21038 = kk_std_core_max_1(_x21039, _x21040, _ctx); /*float64*/
  return kk_double_box(_x21038, _ctx);
}

double kk_std_core_maximum_1(kk_std_core__list xs, kk_context_t* _ctx) { /* (xs : list<float64>) -> float64 */ 
  if (kk_std_core__is_Nil(xs)) {
    return 0.0;
  }
  {
    struct kk_std_core_Cons* _con21034 = kk_std_core__as_Cons(xs);
    kk_box_t _box_x19244 = _con21034->head;
    kk_std_core__list xx = _con21034->tail;
    double x = kk_double_unbox(_box_x19244, NULL);
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_box_drop(_box_x19244, _ctx);
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_box_t _x21036 = kk_std_core_foldl(xx, kk_double_box(x, _ctx), kk_std_core_new_maximum_fun21037_1(_ctx), _ctx); /*12365*/
    return kk_double_unbox(_x21036, _ctx);
  }
}
 
// Convert a list to a `:maybe` type, using `Nothing` for an empty list, and otherwise `Just` on the head element.
// Note: this is just `head`.

kk_std_core_types__maybe kk_std_core_maybe_3(kk_std_core__list xs, kk_context_t* _ctx) { /* forall<a> (xs : list<a>) -> maybe<a> */ 
  if (kk_std_core__is_Nil(xs)) {
    return kk_std_core_types__new_Nothing(_ctx);
  }
  {
    struct kk_std_core_Cons* _con21041 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con21041->head;
    kk_std_core__list _pat1 = _con21041->tail;
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
  bool _match_19618;
  kk_string_t _x21042 = kk_string_dup(s); /*string*/
  kk_string_t _x21043 = kk_string_empty(); /*string*/
  _match_19618 = kk_string_is_eq(_x21042,_x21043,kk_context()); /*bool*/
  if (_match_19618) {
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
 
// Returns the smallest element of a list of integers (or `default` (=`0`) for the empty list)


// lift anonymous function
struct kk_std_core_minimum_fun21049__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_minimum_fun21049(kk_function_t _fself, kk_box_t _b_19264, kk_box_t _b_19265, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_minimum_fun21049(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_minimum_fun21049, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_minimum_fun21049(kk_function_t _fself, kk_box_t _b_19264, kk_box_t _b_19265, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_integer_t _x21050;
  kk_integer_t _x21051 = kk_integer_unbox(_b_19264); /*int*/
  kk_integer_t _x21052 = kk_integer_unbox(_b_19265); /*int*/
  _x21050 = kk_std_core_min(_x21051, _x21052, _ctx); /*int*/
  return kk_integer_box(_x21050);
}

kk_integer_t kk_std_core_minimum(kk_std_core__list xs, kk_std_core_types__optional default0, kk_context_t* _ctx) { /* (xs : list<int>, default : optional<int>) -> int */ 
  if (kk_std_core__is_Nil(xs)) {
    if (kk_std_core_types__is_Optional(default0)) {
      kk_box_t _box_x19259 = default0._cons.Optional.value;
      kk_integer_t _default_14872 = kk_integer_unbox(_box_x19259);
      return _default_14872;
    }
    {
      return kk_integer_from_small(0);
    }
  }
  {
    struct kk_std_core_Cons* _con21046 = kk_std_core__as_Cons(xs);
    kk_box_t _box_x19260 = _con21046->head;
    kk_std_core__list xx = _con21046->tail;
    kk_integer_t x = kk_integer_unbox(_box_x19260);
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_integer_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_std_core_types__optional_drop(default0, _ctx);
    kk_box_t _x21048 = kk_std_core_foldl(xx, kk_integer_box(x), kk_std_core_new_minimum_fun21049(_ctx), _ctx); /*12365*/
    return kk_integer_unbox(_x21048);
  }
}
 
// Returns the smallest element of a list of floats (or `0` for the empty list)


// lift anonymous function
struct kk_std_core_minimum_fun21056__t_1 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_minimum_fun21056_1(kk_function_t _fself, kk_box_t _b_19273, kk_box_t _b_19274, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_minimum_fun21056_1(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_minimum_fun21056_1, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_minimum_fun21056_1(kk_function_t _fself, kk_box_t _b_19273, kk_box_t _b_19274, kk_context_t* _ctx) {
  kk_unused(_fself);
  double _x21057;
  double _x21058 = kk_double_unbox(_b_19273, _ctx); /*float64*/
  double _x21059 = kk_double_unbox(_b_19274, _ctx); /*float64*/
  _x21057 = kk_std_core_min_1(_x21058, _x21059, _ctx); /*float64*/
  return kk_double_box(_x21057, _ctx);
}

double kk_std_core_minimum_1(kk_std_core__list xs, kk_context_t* _ctx) { /* (xs : list<float64>) -> float64 */ 
  if (kk_std_core__is_Nil(xs)) {
    return 0.0;
  }
  {
    struct kk_std_core_Cons* _con21053 = kk_std_core__as_Cons(xs);
    kk_box_t _box_x19269 = _con21053->head;
    kk_std_core__list xx = _con21053->tail;
    double x = kk_double_unbox(_box_x19269, NULL);
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_box_drop(_box_x19269, _ctx);
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_box_t _x21055 = kk_std_core_foldl(xx, kk_double_box(x, _ctx), kk_std_core_new_minimum_fun21056_1(_ctx), _ctx); /*12365*/
    return kk_double_unbox(_x21055, _ctx);
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
  kk_std_core_types__maybe _x21060;
  bool _match_19615 = kk_integer_eq_borrow(i,(kk_integer_from_small(0)),kk_context()); /*bool*/;
  if (_match_19615) {
    kk_integer_drop(i, _ctx);
    _x21060 = kk_std_core_types__new_Nothing(_ctx); /*forall<a> maybe<a>*/
  }
  else {
    _x21060 = kk_std_core_types__new_Just(kk_integer_box(i), _ctx); /*forall<a> maybe<a>*/
  }
  return kk_std_core_null(_x21060, _ctx);
}
 
// Cast an empty string a null

kk_std_core__null kk_std_core_null_2(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> null<string> */ 
  kk_std_core_types__maybe _x21061;
  bool _match_19614;
  kk_string_t _x21062 = kk_string_dup(s); /*string*/
  kk_string_t _x21063 = kk_string_empty(); /*string*/
  _match_19614 = kk_string_is_eq(_x21062,_x21063,kk_context()); /*bool*/
  if (_match_19614) {
    kk_string_drop(s, _ctx);
    _x21061 = kk_std_core_types__new_Nothing(_ctx); /*forall<a> maybe<a>*/
  }
  else {
    _x21061 = kk_std_core_types__new_Just(kk_string_box(s), _ctx); /*forall<a> maybe<a>*/
  }
  return kk_std_core_null(_x21061, _ctx);
}
 
// Left-align a string to width `width`  using `fill`  (default is a space) to fill on the right.

kk_string_t kk_std_core_pad_right(kk_string_t s, kk_integer_t width, kk_std_core_types__optional fill, kk_context_t* _ctx) { /* (s : string, width : int, fill : optional<char>) -> string */ 
  kk_ssize_t w;
  kk_integer_t _x21066 = kk_integer_dup(width); /*int*/
  w = kk_std_core_ssize__t(_x21066, _ctx); /*ssize_t*/
  kk_ssize_t n;
  kk_string_t _x21067 = kk_string_dup(s); /*string*/
  n = kk_string_len(_x21067,kk_context()); /*ssize_t*/
  bool _match_19613 = (w <= n); /*bool*/;
  if (_match_19613) {
    kk_std_core_types__optional_drop(fill, _ctx);
    return s;
  }
  {
    kk_string_t _x21068;
    kk_string_t _x21069;
    kk_char_t _x21070;
    if (kk_std_core_types__is_Optional(fill)) {
      kk_box_t _box_x19288 = fill._cons.Optional.value;
      kk_char_t _fill_15274 = kk_char_unbox(_box_x19288, NULL);
      _x21070 = _fill_15274; /*char*/
    }
    else {
      _x21070 = ' '; /*char*/
    }
    _x21069 = kk_std_core_string(_x21070, _ctx); /*string*/
    kk_ssize_t _x21072 = (w - n); /*ssize_t*/
    _x21068 = kk_std_core_repeatz(_x21069, _x21072, _ctx); /*string*/
    return kk_std_core__lp__plus__plus__1_rp_(s, _x21068, _ctx);
  }
}
 
// Is `pre`  a prefix of `s`? If so, returns a slice
// of `s` following `pre` up to the end of `s`.

kk_std_core_types__maybe kk_std_core_starts_with(kk_string_t s, kk_string_t pre, kk_context_t* _ctx) { /* (s : string, pre : string) -> maybe<sslice> */ 
  bool _match_19612;
  kk_string_t _x21073 = kk_string_dup(s); /*string*/
  kk_string_t _x21074 = kk_string_dup(pre); /*string*/
  _match_19612 = kk_string_starts_with(_x21073,_x21074,kk_context()); /*bool*/
  if (_match_19612) {
    kk_std_core__sslice _b_19290_19289;
    kk_string_t _x21075 = kk_string_dup(s); /*string*/
    kk_ssize_t _x21076;
    kk_string_t _x21077 = kk_string_dup(pre); /*string*/
    _x21076 = kk_string_len(_x21077,kk_context()); /*ssize_t*/
    kk_ssize_t _x21078;
    kk_ssize_t _x21079 = kk_string_len(s,kk_context()); /*ssize_t*/
    kk_ssize_t _x21080 = kk_string_len(pre,kk_context()); /*ssize_t*/
    _x21078 = (_x21079 - _x21080); /*ssize_t*/
    _b_19290_19289 = kk_std_core__new_Sslice(_x21075, _x21076, _x21078, _ctx); /*sslice*/
    return kk_std_core_types__new_Just(kk_std_core__sslice_box(_b_19290_19289, _ctx), _ctx);
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
  bool _match_19610;
  kk_string_t _x21081 = kk_string_dup(sub); /*string*/
  kk_string_t _x21082 = kk_string_empty(); /*string*/
  _match_19610 = kk_string_is_eq(_x21081,_x21082,kk_context()); /*bool*/
  if (_match_19610) {
    kk_string_drop(sub, _ctx);
    return s;
  }
  {
    kk_std_core_types__maybe _match_19611;
    kk_string_t _x21084 = kk_string_dup(s); /*string*/
    kk_string_t _x21085 = kk_string_dup(sub); /*string*/
    _match_19611 = kk_std_core_starts_with(_x21084, _x21085, _ctx); /*maybe<sslice>*/
    if (kk_std_core_types__is_Just(_match_19611)) {
      kk_box_t _box_x19291 = _match_19611._cons.Just.value;
      kk_std_core__sslice slice0 = kk_std_core__sslice_unbox(_box_x19291, NULL);
      kk_string_drop(s, _ctx);
      kk_std_core__sslice_dup(slice0);
      kk_std_core_types__maybe_drop(_match_19611, _ctx);
      { // tailcall
        kk_string_t _x21087 = kk_std_core_string_3(slice0, _ctx); /*string*/
        s = _x21087;
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
  bool _match_19608;
  kk_string_t _x21088 = kk_string_dup(sub); /*string*/
  kk_string_t _x21089 = kk_string_empty(); /*string*/
  _match_19608 = kk_string_is_eq(_x21088,_x21089,kk_context()); /*bool*/
  if (_match_19608) {
    kk_string_drop(sub, _ctx);
    return s;
  }
  {
    kk_std_core_types__maybe _match_19609;
    kk_string_t _x21091 = kk_string_dup(s); /*string*/
    kk_string_t _x21092 = kk_string_dup(sub); /*string*/
    _match_19609 = kk_std_core_ends_with(_x21091, _x21092, _ctx); /*maybe<sslice>*/
    if (kk_std_core_types__is_Just(_match_19609)) {
      kk_box_t _box_x19292 = _match_19609._cons.Just.value;
      kk_std_core__sslice slice0 = kk_std_core__sslice_unbox(_box_x19292, NULL);
      kk_string_drop(s, _ctx);
      kk_std_core__sslice_dup(slice0);
      kk_std_core_types__maybe_drop(_match_19609, _ctx);
      { // tailcall
        kk_string_t _x21094 = kk_std_core_string_3(slice0, _ctx); /*string*/
        s = _x21094;
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
  bool _match_19607;
  kk_string_t _x21100 = kk_string_dup(s); /*string*/
  kk_string_t _x21101 = kk_string_empty(); /*string*/
  _match_19607 = kk_string_is_eq(_x21100,_x21101,kk_context()); /*bool*/
  if (_match_19607) {
    kk_string_drop(s, _ctx);
    kk_std_core_types__optional_drop(hex, _ctx);
    if (kk_std_core_types__is_Optional(default0)) {
      kk_box_t _box_x19294 = default0._cons.Optional.value;
      kk_integer_t _default_15556 = kk_integer_unbox(_box_x19294);
      return _default_15556;
    }
    {
      return kk_integer_from_small(0);
    }
  }
  {
    kk_std_core_types__maybe m_17140;
    kk_string_t _x21104;
    kk_string_t _x21105 = kk_string_trim_left(s,kk_context()); /*string*/
    _x21104 = kk_string_trim_right(_x21105,kk_context()); /*string*/
    bool _x21106;
    if (kk_std_core_types__is_Optional(hex)) {
      kk_box_t _box_x19295 = hex._cons.Optional.value;
      bool _hex_15560 = kk_bool_unbox(_box_x19295);
      _x21106 = _hex_15560; /*bool*/
    }
    else {
      _x21106 = false; /*bool*/
    }
    m_17140 = kk_std_core_xparse_int(_x21104, _x21106, _ctx); /*maybe<int>*/
    if (kk_std_core_types__is_Nothing(m_17140)) {
      if (kk_std_core_types__is_Optional(default0)) {
        kk_box_t _box_x19296 = default0._cons.Optional.value;
        kk_integer_t _default_155560 = kk_integer_unbox(_box_x19296);
        return _default_155560;
      }
      {
        return kk_integer_from_small(0);
      }
    }
    {
      kk_box_t _box_x19297 = m_17140._cons.Just.value;
      kk_integer_t x = kk_integer_unbox(_box_x19297);
      kk_std_core_types__optional_drop(default0, _ctx);
      return x;
    }
  }
}
 
// monadic lift

kk_std_core_types__tuple2_ kk_std_core__mlift17728_partition_acc(kk_std_core__list acc1, kk_std_core__list acc2, kk_function_t pred, kk_box_t x, kk_std_core__list xx, bool _y_17570, kk_context_t* _ctx) { /* forall<a,e> (acc1 : list<a>, acc2 : list<a>, pred : (a) -> e bool, x : a, xx : list<a>, bool) -> e (list<a>, list<a>) */ 
  if (_y_17570) {
    kk_std_core__list _x21110 = kk_std_core__new_Cons(kk_reuse_null, x, acc1, _ctx); /*list<61>*/
    return kk_std_core_partition_acc(xx, pred, _x21110, acc2, _ctx);
  }
  {
    kk_std_core__list _x21111 = kk_std_core__new_Cons(kk_reuse_null, x, acc2, _ctx); /*list<61>*/
    return kk_std_core_partition_acc(xx, pred, acc1, _x21111, _ctx);
  }
}


// lift anonymous function
struct kk_std_core_partition_acc_fun21116__t {
  struct kk_function_s _base;
  kk_std_core__list acc10;
  kk_std_core__list acc20;
  kk_function_t pred0;
  kk_box_t x0;
  kk_std_core__list xx0;
};
static kk_box_t kk_std_core_partition_acc_fun21116(kk_function_t _fself, kk_box_t _b_19301, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_partition_acc_fun21116(kk_std_core__list acc10, kk_std_core__list acc20, kk_function_t pred0, kk_box_t x0, kk_std_core__list xx0, kk_context_t* _ctx) {
  struct kk_std_core_partition_acc_fun21116__t* _self = kk_function_alloc_as(struct kk_std_core_partition_acc_fun21116__t, 6, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_partition_acc_fun21116, kk_context());
  _self->acc10 = acc10;
  _self->acc20 = acc20;
  _self->pred0 = pred0;
  _self->x0 = x0;
  _self->xx0 = xx0;
  return &_self->_base;
}

static kk_box_t kk_std_core_partition_acc_fun21116(kk_function_t _fself, kk_box_t _b_19301, kk_context_t* _ctx) {
  struct kk_std_core_partition_acc_fun21116__t* _self = kk_function_as(struct kk_std_core_partition_acc_fun21116__t*, _fself);
  kk_std_core__list acc10 = _self->acc10; /* list<15678> */
  kk_std_core__list acc20 = _self->acc20; /* list<15678> */
  kk_function_t pred0 = _self->pred0; /* (15678) -> 15679 bool */
  kk_box_t x0 = _self->x0; /* 15678 */
  kk_std_core__list xx0 = _self->xx0; /* list<15678> */
  kk_drop_match(_self, {kk_std_core__list_dup(acc10);kk_std_core__list_dup(acc20);kk_function_dup(pred0);kk_box_dup(x0);kk_std_core__list_dup(xx0);}, {}, _ctx)
  bool _y_19305_175700 = kk_bool_unbox(_b_19301); /*bool*/;
  kk_std_core_types__tuple2_ _x21117 = kk_std_core__mlift17728_partition_acc(acc10, acc20, pred0, x0, xx0, _y_19305_175700, _ctx); /*(list<15678>, list<15678>)*/
  return kk_std_core_types__tuple2__box(_x21117, _ctx);
}

kk_std_core_types__tuple2_ kk_std_core_partition_acc(kk_std_core__list xs, kk_function_t pred0, kk_std_core__list acc10, kk_std_core__list acc20, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, pred : (a) -> e bool, acc1 : list<a>, acc2 : list<a>) -> e (list<a>, list<a>) */ 
  kk__tailcall: ;
  if (kk_std_core__is_Nil(xs)) {
    kk_function_drop(pred0, _ctx);
    kk_std_core__list _b_19302_19298 = kk_std_core__lift17195_reverse(kk_std_core__new_Nil(_ctx), acc10, _ctx); /*list<15678>*/;
    kk_std_core__list _b_19303_19299 = kk_std_core__lift17195_reverse(kk_std_core__new_Nil(_ctx), acc20, _ctx); /*list<15678>*/;
    return kk_std_core_types__new_dash__lp__comma__rp_(kk_std_core__list_box(_b_19302_19298, _ctx), kk_std_core__list_box(_b_19303_19299, _ctx), _ctx);
  }
  {
    struct kk_std_core_Cons* _con21112 = kk_std_core__as_Cons(xs);
    kk_box_t x0 = _con21112->head;
    kk_std_core__list xx0 = _con21112->tail;
    kk_reuse_t _ru_19960 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      _ru_19960 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_box_dup(x0);
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(xs, _ctx);
    }
    bool x1_18042;
    kk_function_t _x21114 = kk_function_dup(pred0); /*(15678) -> 15679 bool*/
    kk_box_t _x21113 = kk_box_dup(x0); /*15678*/
    x1_18042 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x21114, (_x21114, _x21113, _ctx)); /*bool*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_19960,kk_context());
      kk_box_t _x21115 = kk_std_core_hnd_yield_extend(kk_std_core_new_partition_acc_fun21116(acc10, acc20, pred0, x0, xx0, _ctx), _ctx); /*3926*/
      return kk_std_core_types__tuple2__unbox(_x21115, _ctx);
    }
    if (x1_18042) { // tailcall
                    kk_std_core__list _x21118 = kk_std_core__new_Cons(_ru_19960, x0, acc10, _ctx); /*list<61>*/
                    xs = xx0;
                    acc10 = _x21118;
                    goto kk__tailcall;
    }
    { // tailcall
      kk_std_core__list _x21119 = kk_std_core__new_Cons(_ru_19960, x0, acc20, _ctx); /*list<61>*/
      xs = xx0;
      acc20 = _x21119;
      goto kk__tailcall;
    }
  }
}
 
// redirect `print` and `println` calls to a specified function.


// lift anonymous function
struct kk_std_core_print_redirect_fun21122__t {
  struct kk_function_s _base;
  kk_function_t print0;
};
static kk_box_t kk_std_core_print_redirect_fun21122(kk_function_t _fself, kk_box_t _b_19309, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_print_redirect_fun21122(kk_function_t print0, kk_context_t* _ctx) {
  struct kk_std_core_print_redirect_fun21122__t* _self = kk_function_alloc_as(struct kk_std_core_print_redirect_fun21122__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_print_redirect_fun21122, kk_context());
  _self->print0 = print0;
  return &_self->_base;
}

static kk_box_t kk_std_core_print_redirect_fun21122(kk_function_t _fself, kk_box_t _b_19309, kk_context_t* _ctx) {
  struct kk_std_core_print_redirect_fun21122__t* _self = kk_function_as(struct kk_std_core_print_redirect_fun21122__t*, _fself);
  kk_function_t print0 = _self->print0; /* (msg : string) -> console () */
  kk_drop_match(_self, {kk_function_dup(print0);}, {}, _ctx)
  kk_unit_t _x21123 = kk_Unit;
  kk_string_t _x21124 = kk_string_unbox(_b_19309); /*string*/
  kk_function_call(kk_unit_t, (kk_function_t, kk_string_t, kk_context_t*), print0, (print0, _x21124, _ctx));
  return kk_unit_box(_x21123);
}

kk_unit_t kk_std_core_print_redirect(kk_function_t print0, kk_context_t* _ctx) { /* (print : (msg : string) -> console ()) -> io () */ 
  kk_box_t _x21120;
  kk_std_core_types__maybe _x21121 = kk_std_core_types__new_Just(kk_function_box(kk_std_core_new_print_redirect_fun21122(print0, _ctx)), _ctx); /*maybe<1034>*/
  _x21120 = kk_std_core_types__maybe_box(_x21121, _ctx); /*1000*/
  kk_ref_set_borrow(kk_std_core_redirect,_x21120,kk_context()); return kk_Unit;
}
extern bool kk_std_core_remove_fun21125(kk_function_t _fself, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_remove_fun21125__t* _self = kk_function_as(struct kk_std_core_remove_fun21125__t*, _fself);
  kk_function_t pred = _self->pred; /* (15747) -> bool */
  kk_drop_match(_self, {kk_function_dup(pred);}, {}, _ctx)
  bool b_17147 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), pred, (pred, x, _ctx)); /*bool*/;
  if (b_17147) {
    return false;
  }
  {
    return true;
  }
}
extern kk_unit_t kk_std_core_repeat_fun21128_1(kk_function_t _fself, kk_integer_t i, kk_context_t* _ctx) {
  struct kk_std_core_repeat_fun21128__t_1* _self = kk_function_as(struct kk_std_core_repeat_fun21128__t_1*, _fself);
  kk_function_t action = _self->action; /* () -> 15766 () */
  kk_drop_match(_self, {kk_function_dup(action);}, {}, _ctx)
  kk_integer_drop(i, _ctx);
  return kk_function_call(kk_unit_t, (kk_function_t, kk_context_t*), action, (action, _ctx));
}
 
// Create a list of `n`  repeated elementes `x`

kk_std_core__list kk_std_core__ctail_replicate(kk_box_t x, kk_integer_t n, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* forall<a> (x : a, n : int, ctail<list<a>>) -> list<a> */ 
  kk__tailcall: ;
  bool _match_19605 = kk_integer_gt_borrow(n,(kk_integer_from_small(0)),kk_context()); /*bool*/;
  if (_match_19605) {
    kk_std_core__list _ctail_17308 = kk_std_core__list_hole(); /*list<15810>*/;
    kk_std_core__list _ctail_17309;
    kk_box_t _x21130 = kk_box_dup(x); /*15810*/
    _ctail_17309 = kk_std_core__new_Cons(kk_reuse_null, _x21130, _ctail_17308, _ctx); /*list<15810>*/
    kk_box_t* _b_19325_19320 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17309)->tail)); /*cfield<list<15810>>*/;
    { // tailcall
      kk_integer_t _x21131 = kk_integer_add_small_const(n, -1, _ctx); /*int*/
      kk_std_core_types__ctail _x21132 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17309, _ctx)),_b_19325_19320); /*ctail<0>*/
      n = _x21131;
      _acc = _x21132;
      goto kk__tailcall;
    }
  }
  {
    kk_box_drop(x, _ctx);
    kk_integer_drop(n, _ctx);
    kk_box_t _x21133 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21133, _ctx);
  }
}
 
// Create a list of `n`  repeated elementes `x`

kk_std_core__list kk_std_core_replicate(kk_box_t x0, kk_integer_t n0, kk_context_t* _ctx) { /* forall<a> (x : a, n : int) -> list<a> */ 
  kk_std_core_types__ctail _x21134 = kk_ctail_nil(); /*ctail<0>*/
  return kk_std_core__ctail_replicate(x0, n0, _x21134, _ctx);
}
 
// lifted local: reverse-join, reverse-acc

kk_std_core__list kk_std_core__lift17200_reverse_join(kk_std_core__list acc, kk_std_core__list ys, kk_context_t* _ctx) { /* forall<a> (acc : list<a>, ys : list<a>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con21135 = kk_std_core__as_Cons(ys);
    kk_box_t x = _con21135->head;
    kk_std_core__list xx = _con21135->tail;
    kk_reuse_t _ru_19961 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      _ru_19961 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(ys, _ctx);
    }
    { // tailcall
      kk_std_core__list _x21136 = kk_std_core__new_Cons(_ru_19961, x, acc, _ctx); /*list<61>*/
      acc = _x21136;
      ys = xx;
      goto kk__tailcall;
    }
  }
  {
    return acc;
  }
}
 
// lifted local: reverse-join, join-acc

kk_string_t kk_std_core__lift17201_reverse_join(kk_std_core__list ys0, kk_string_t acc0, kk_context_t* _ctx) { /* (ys0 : list<string>, acc0 : string) -> string */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys0)) {
    struct kk_std_core_Cons* _con21137 = kk_std_core__as_Cons(ys0);
    kk_box_t _box_x19331 = _con21137->head;
    kk_std_core__list yy = _con21137->tail;
    kk_string_t y = kk_string_unbox(_box_x19331);
    if (kk_likely(kk_std_core__list_is_unique(ys0))) {
      kk_std_core__list_free(ys0, _ctx);
    }
    else {
      kk_string_dup(y);
      kk_std_core__list_dup(yy);
      kk_std_core__list_decref(ys0, _ctx);
    }
    { // tailcall
      kk_string_t _x21139;
      kk_string_t _x21140;
      kk_string_t _x21141 = kk_string_empty(); /*string*/
      _x21140 = kk_std_core__lp__plus__plus__1_rp_(_x21141, y, _ctx); /*string*/
      _x21139 = kk_std_core__lp__plus__plus__1_rp_(acc0, _x21140, _ctx); /*string*/
      ys0 = yy;
      acc0 = _x21139;
      goto kk__tailcall;
    }
  }
  {
    return acc0;
  }
}
 
// Concatenate all strings in a list in reverse order

kk_string_t kk_std_core_reverse_join(kk_std_core__list xs, kk_context_t* _ctx) { /* (xs : list<string>) -> string */ 
  kk_std_core__list xs0_17151 = kk_std_core__lift17200_reverse_join(kk_std_core__new_Nil(_ctx), xs, _ctx); /*list<string>*/;
  if (kk_std_core__is_Nil(xs0_17151)) {
    return kk_string_empty();
  }
  {
    struct kk_std_core_Cons* _con21144 = kk_std_core__as_Cons(xs0_17151);
    kk_box_t _box_x19332 = _con21144->head;
    kk_std_core__list xx0 = _con21144->tail;
    kk_string_t x0 = kk_string_unbox(_box_x19332);
    if (kk_likely(kk_std_core__list_is_unique(xs0_17151))) {
      kk_std_core__list_free(xs0_17151, _ctx);
    }
    else {
      kk_string_dup(x0);
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(xs0_17151, _ctx);
    }
    return kk_std_core__lift17201_reverse_join(xx0, x0, _ctx);
  }
}

kk_string_t kk_std_core_show_tuple(kk_std_core_types__tuple2_ x, kk_function_t showfst, kk_function_t showsnd, kk_context_t* _ctx) { /* forall<a,b> (x : (a, b), showfst : (a) -> string, showsnd : (b) -> string) -> string */ 
  kk_string_t _x21146;
  kk_define_string_literal(, _s21147, 1, "(")
  _x21146 = kk_string_dup(_s21147); /*string*/
  kk_string_t _x21148;
  kk_string_t _x21149;
  kk_box_t _x21150;
  {
    kk_box_t _x = x.fst;
    kk_box_dup(_x);
    _x21150 = _x; /*16129*/
  }
  _x21149 = kk_function_call(kk_string_t, (kk_function_t, kk_box_t, kk_context_t*), showfst, (showfst, _x21150, _ctx)); /*string*/
  kk_string_t _x21151;
  kk_string_t _x21152;
  kk_define_string_literal(, _s21153, 1, ",")
  _x21152 = kk_string_dup(_s21153); /*string*/
  kk_string_t _x21154;
  kk_string_t _x21155;
  kk_box_t _x21156;
  {
    kk_box_t _x0 = x.snd;
    kk_box_dup(_x0);
    kk_std_core_types__tuple2__drop(x, _ctx);
    _x21156 = _x0; /*16130*/
  }
  _x21155 = kk_function_call(kk_string_t, (kk_function_t, kk_box_t, kk_context_t*), showsnd, (showsnd, _x21156, _ctx)); /*string*/
  kk_string_t _x21157;
  kk_define_string_literal(, _s21158, 1, ")")
  _x21157 = kk_string_dup(_s21158); /*string*/
  _x21154 = kk_std_core__lp__plus__plus__1_rp_(_x21155, _x21157, _ctx); /*string*/
  _x21151 = kk_std_core__lp__plus__plus__1_rp_(_x21152, _x21154, _ctx); /*string*/
  _x21148 = kk_std_core__lp__plus__plus__1_rp_(_x21149, _x21151, _ctx); /*string*/
  return kk_std_core__lp__plus__plus__1_rp_(_x21146, _x21148, _ctx);
}
 
// monadic lift

kk_std_core_types__tuple2_ kk_std_core__mlift17729_op(kk_std_core__list acc, kk_function_t predicate, kk_box_t y, kk_std_core__list ys, kk_std_core__list yy, bool _y_17578, kk_context_t* _ctx) { /* forall<a,e> (acc : list<a>, predicate : (a) -> e bool, y : a, ys : list<a>, yy : list<a>, bool) -> e (list<a>, list<a>) */ 
  if (_y_17578) {
    kk_std_core__list_drop(ys, _ctx);
    kk_std_core__list _x21159 = kk_std_core__new_Cons(kk_reuse_null, y, acc, _ctx); /*list<61>*/
    return kk_std_core__lift17202_span(predicate, yy, _x21159, _ctx);
  }
  {
    kk_std_core__list_drop(yy, _ctx);
    kk_box_drop(y, _ctx);
    kk_function_drop(predicate, _ctx);
    kk_std_core__list _b_19335_19333 = kk_std_core__lift17195_reverse(kk_std_core__new_Nil(_ctx), acc, _ctx); /*list<16188>*/;
    return kk_std_core_types__new_dash__lp__comma__rp_(kk_std_core__list_box(_b_19335_19333, _ctx), kk_std_core__list_box(ys, _ctx), _ctx);
  }
}
 
// lifted local: span, span-acc
// todo: implement TRMC with multiple results to avoid the reverse


// lift anonymous function
struct kk_std_core__lift17202_span_fun21164__t {
  struct kk_function_s _base;
  kk_std_core__list acc0;
  kk_function_t predicate0;
  kk_box_t y0;
  kk_std_core__list ys0;
  kk_std_core__list yy0;
};
static kk_box_t kk_std_core__lift17202_span_fun21164(kk_function_t _fself, kk_box_t _b_19338, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17202_span_fun21164(kk_std_core__list acc0, kk_function_t predicate0, kk_box_t y0, kk_std_core__list ys0, kk_std_core__list yy0, kk_context_t* _ctx) {
  struct kk_std_core__lift17202_span_fun21164__t* _self = kk_function_alloc_as(struct kk_std_core__lift17202_span_fun21164__t, 6, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift17202_span_fun21164, kk_context());
  _self->acc0 = acc0;
  _self->predicate0 = predicate0;
  _self->y0 = y0;
  _self->ys0 = ys0;
  _self->yy0 = yy0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift17202_span_fun21164(kk_function_t _fself, kk_box_t _b_19338, kk_context_t* _ctx) {
  struct kk_std_core__lift17202_span_fun21164__t* _self = kk_function_as(struct kk_std_core__lift17202_span_fun21164__t*, _fself);
  kk_std_core__list acc0 = _self->acc0; /* list<16188> */
  kk_function_t predicate0 = _self->predicate0; /* (16188) -> 16189 bool */
  kk_box_t y0 = _self->y0; /* 16188 */
  kk_std_core__list ys0 = _self->ys0; /* list<16188> */
  kk_std_core__list yy0 = _self->yy0; /* list<16188> */
  kk_drop_match(_self, {kk_std_core__list_dup(acc0);kk_function_dup(predicate0);kk_box_dup(y0);kk_std_core__list_dup(ys0);kk_std_core__list_dup(yy0);}, {}, _ctx)
  bool _y_19348_175780 = kk_bool_unbox(_b_19338); /*bool*/;
  kk_std_core_types__tuple2_ _x21165 = kk_std_core__mlift17729_op(acc0, predicate0, y0, ys0, yy0, _y_19348_175780, _ctx); /*(list<16188>, list<16188>)*/
  return kk_std_core_types__tuple2__box(_x21165, _ctx);
}

kk_std_core_types__tuple2_ kk_std_core__lift17202_span(kk_function_t predicate0, kk_std_core__list ys0, kk_std_core__list acc0, kk_context_t* _ctx) { /* forall<a,e> (predicate : (a) -> e bool, ys : list<a>, acc : list<a>) -> e (list<a>, list<a>) */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys0)) {
    struct kk_std_core_Cons* _con21160 = kk_std_core__as_Cons(ys0);
    kk_box_t y0 = _con21160->head;
    kk_std_core__list yy0 = _con21160->tail;
    kk_box_dup(y0);
    kk_std_core__list_dup(yy0);
    bool x_18049;
    kk_function_t _x21162 = kk_function_dup(predicate0); /*(16188) -> 16189 bool*/
    kk_box_t _x21161 = kk_box_dup(y0); /*16188*/
    x_18049 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x21162, (_x21162, _x21161, _ctx)); /*bool*/
    if (kk_yielding(kk_context())) {
      kk_box_t _x21163 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift17202_span_fun21164(acc0, predicate0, y0, ys0, yy0, _ctx), _ctx); /*3926*/
      return kk_std_core_types__tuple2__unbox(_x21163, _ctx);
    }
    if (x_18049) {
      kk_reuse_t _ru_19964 = kk_reuse_null; /*reuse*/;
      if (kk_likely(kk_std_core__list_is_unique(ys0))) {
        kk_std_core__list_drop(yy0, _ctx);
        kk_box_drop(y0, _ctx);
        _ru_19964 = (kk_std_core__list_reuse(ys0));
      }
      else {
        kk_std_core__list_decref(ys0, _ctx);
      }
      { // tailcall
        kk_std_core__list _x21166 = kk_std_core__new_Cons(_ru_19964, y0, acc0, _ctx); /*list<61>*/
        ys0 = yy0;
        acc0 = _x21166;
        goto kk__tailcall;
      }
    }
    {
      kk_std_core__list_drop(yy0, _ctx);
      kk_box_drop(y0, _ctx);
      kk_function_drop(predicate0, _ctx);
      kk_std_core__list _b_19344_19339 = kk_std_core__lift17195_reverse(kk_std_core__new_Nil(_ctx), acc0, _ctx); /*list<16188>*/;
      return kk_std_core_types__new_dash__lp__comma__rp_(kk_std_core__list_box(_b_19344_19339, _ctx), kk_std_core__list_box(ys0, _ctx), _ctx);
    }
  }
  {
    kk_function_drop(predicate0, _ctx);
    kk_std_core__list _b_19346_19341 = kk_std_core__lift17195_reverse(kk_std_core__new_Nil(_ctx), acc0, _ctx); /*list<16188>*/;
    return kk_std_core_types__new_dash__lp__comma__rp_(kk_std_core__list_box(_b_19346_19341, _ctx), kk_std_core__list_box(ys0, _ctx), _ctx);
  }
}
 
// Return the sum of a list of integers


// lift anonymous function
struct kk_std_core_sum_fun21168__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_sum_fun21168(kk_function_t _fself, kk_box_t _b_19352, kk_box_t _b_19353, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_sum_fun21168(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_sum_fun21168, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_sum_fun21168(kk_function_t _fself, kk_box_t _b_19352, kk_box_t _b_19353, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_integer_t _x21169;
  kk_integer_t _x21170 = kk_integer_unbox(_b_19352); /*int*/
  kk_integer_t _x21171 = kk_integer_unbox(_b_19353); /*int*/
  _x21169 = kk_std_core__lp__plus__4_rp_(_x21170, _x21171, _ctx); /*int*/
  return kk_integer_box(_x21169);
}

kk_integer_t kk_std_core_sum(kk_std_core__list xs, kk_context_t* _ctx) { /* (xs : list<int>) -> int */ 
  kk_box_t _x21167 = kk_std_core_foldl(xs, kk_integer_box(kk_integer_from_small(0)), kk_std_core_new_sum_fun21168(_ctx), _ctx); /*12365*/
  return kk_integer_unbox(_x21167);
}
 
// Return the tail of list. Returns the empty list if `xs` is empty.

kk_std_core__list kk_std_core_tail_1(kk_std_core__list xs, kk_context_t* _ctx) { /* forall<a> (xs : list<a>) -> list<a> */ 
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21172 = kk_std_core__as_Cons(xs);
    kk_box_t _pat0 = _con21172->head;
    kk_std_core__list xx = _con21172->tail;
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
  kk_std_core__sslice slice0_17155;
  bool _match_19597;
  kk_integer_t _brw_19602;
  kk_std_core_types__optional _x21173 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
  if (kk_std_core_types__is_Optional(_x21173)) {
    kk_box_t _box_x19357 = _x21173._cons.Optional.value;
    kk_integer_t _n_10142 = kk_integer_unbox(_box_x19357);
    _brw_19602 = _n_10142; /*int*/
  }
  else {
    _brw_19602 = kk_integer_from_small(1); /*int*/
  }
  bool _brw_19603 = kk_integer_eq_borrow(_brw_19602,(kk_integer_from_small(1)),kk_context()); /*bool*/;
  kk_integer_drop(_brw_19602, _ctx);
  _match_19597 = _brw_19603; /*bool*/
  if (_match_19597) {
    slice0_17155 = slice1; /*sslice*/
  }
  else {
    kk_integer_t _brw_19599;
    kk_integer_t _x21175;
    kk_std_core_types__optional _x21176 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
    if (kk_std_core_types__is_Optional(_x21176)) {
      kk_box_t _box_x19358 = _x21176._cons.Optional.value;
      kk_integer_t _n_101420 = kk_integer_unbox(_box_x19358);
      _x21175 = _n_101420; /*int*/
    }
    else {
      _x21175 = kk_integer_from_small(1); /*int*/
    }
    _brw_19599 = kk_integer_add_small_const(_x21175, -1, _ctx); /*int*/
    kk_std_core__sslice _brw_19600 = kk_std_core_extend(slice1, _brw_19599, _ctx); /*sslice*/;
    kk_integer_drop(_brw_19599, _ctx);
    slice0_17155 = _brw_19600; /*sslice*/
  }
  kk_std_core__sslice _x21178;
  {
    kk_string_t s0 = slice0_17155.str;
    kk_ssize_t start0 = slice0_17155.start;
    kk_ssize_t len0 = slice0_17155.len;
    kk_string_dup(s0);
    kk_std_core__sslice_drop(slice0_17155, _ctx);
    kk_string_t _x21179 = kk_string_dup(s0); /*string*/
    kk_ssize_t _x21180 = (start0 + len0); /*ssize_t*/
    kk_ssize_t _x21181;
    kk_ssize_t _x21182 = kk_string_len(s0,kk_context()); /*ssize_t*/
    kk_ssize_t _x21183 = (start0 + len0); /*ssize_t*/
    _x21181 = (_x21182 - _x21183); /*ssize_t*/
    _x21178 = kk_std_core__new_Sslice(_x21179, _x21180, _x21181, _ctx); /*sslice*/
  }
  return kk_std_core_string_3(_x21178, _ctx);
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift17730_op(kk_std_core_types__ctail _acc, kk_function_t predicate, kk_box_t x, kk_std_core__list xx, bool _y_17583, kk_context_t* _ctx) { /* forall<a,e> (ctail<list<a>>, predicate : (a) -> e bool, x : a, xx : list<a>, bool) -> e list<a> */ 
  if (_y_17583) {
    kk_std_core__list _ctail_17310 = kk_std_core__list_hole(); /*list<16299>*/;
    kk_std_core__list _ctail_17311 = kk_std_core__new_Cons(kk_reuse_null, x, _ctail_17310, _ctx); /*list<16299>*/;
    kk_box_t* _b_19369_19364 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17311)->tail)); /*cfield<list<16299>>*/;
    kk_std_core_types__ctail _x21184 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17311, _ctx)),_b_19369_19364); /*ctail<0>*/
    return kk_std_core__ctail_take_while(xx, predicate, _x21184, _ctx);
  }
  {
    kk_std_core__list_drop(xx, _ctx);
    kk_box_drop(x, _ctx);
    kk_function_drop(predicate, _ctx);
    kk_box_t _x21185 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21185, _ctx);
  }
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift17731_op_fun21186__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t x0;
};
static kk_std_core__list kk_std_core__mlift17731_op_fun21186(kk_function_t _fself, kk_std_core__list _ctail_17313, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17731_op_fun21186(kk_function_t _accm, kk_box_t x0, kk_context_t* _ctx) {
  struct kk_std_core__mlift17731_op_fun21186__t* _self = kk_function_alloc_as(struct kk_std_core__mlift17731_op_fun21186__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17731_op_fun21186, kk_context());
  _self->_accm = _accm;
  _self->x0 = x0;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift17731_op_fun21186(kk_function_t _fself, kk_std_core__list _ctail_17313, kk_context_t* _ctx) {
  struct kk_std_core__mlift17731_op_fun21186__t* _self = kk_function_as(struct kk_std_core__mlift17731_op_fun21186__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<16299>) -> list<16299> */
  kk_box_t x0 = _self->x0; /* 16299 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(x0);}, {}, _ctx)
  kk_std_core__list _x21187 = kk_std_core__new_Cons(kk_reuse_null, x0, _ctail_17313, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x21187, _ctx));
}

kk_std_core__list kk_std_core__mlift17731_op(kk_function_t _accm, kk_function_t predicate0, kk_box_t x0, kk_std_core__list xx0, bool _y_17587, kk_context_t* _ctx) { /* forall<a,e> ((list<a>) -> list<a>, predicate : (a) -> e bool, x : a, xx : list<a>, bool) -> e list<a> */ 
  if (_y_17587) {
    return kk_std_core__ctailm_take_while(xx0, predicate0, kk_std_core__new_mlift17731_op_fun21186(_accm, x0, _ctx), _ctx);
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
struct kk_std_core__ctail_take_while_fun21192__t {
  struct kk_function_s _base;
  kk_function_t predicate1;
  kk_box_t x1;
  kk_std_core__list xx1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_take_while_fun21192(kk_function_t _fself, kk_box_t _b_19376, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_take_while_fun21192(kk_function_t predicate1, kk_box_t x1, kk_std_core__list xx1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_take_while_fun21192__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_take_while_fun21192__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_take_while_fun21192, kk_context());
  _self->predicate1 = predicate1;
  _self->x1 = x1;
  _self->xx1 = xx1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_take_while_fun21192(kk_function_t _fself, kk_box_t _b_19376, kk_context_t* _ctx) {
  struct kk_std_core__ctail_take_while_fun21192__t* _self = kk_function_as(struct kk_std_core__ctail_take_while_fun21192__t*, _fself);
  kk_function_t predicate1 = _self->predicate1; /* (16299) -> 16300 bool */
  kk_box_t x1 = _self->x1; /* 16299 */
  kk_std_core__list xx1 = _self->xx1; /* list<16299> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<16299>> */
  kk_drop_match(_self, {kk_function_dup(predicate1);kk_box_dup(x1);kk_std_core__list_dup(xx1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  bool _y_19398_175830 = kk_bool_unbox(_b_19376); /*bool*/;
  kk_std_core__list _x21193 = kk_std_core__mlift17730_op(_acc0, predicate1, x1, xx1, _y_19398_175830, _ctx); /*list<16299>*/
  return kk_std_core__list_box(_x21193, _ctx);
}

kk_std_core__list kk_std_core__ctail_take_while(kk_std_core__list xs, kk_function_t predicate1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, predicate : (a) -> e bool, ctail<list<a>>) -> e list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21188 = kk_std_core__as_Cons(xs);
    kk_box_t x1 = _con21188->head;
    kk_std_core__list xx1 = _con21188->tail;
    kk_reuse_t _ru_19966 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      _ru_19966 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_box_dup(x1);
      kk_std_core__list_dup(xx1);
      kk_std_core__list_decref(xs, _ctx);
    }
    bool x2_18054;
    kk_function_t _x21190 = kk_function_dup(predicate1); /*(16299) -> 16300 bool*/
    kk_box_t _x21189 = kk_box_dup(x1); /*16299*/
    x2_18054 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x21190, (_x21190, _x21189, _ctx)); /*bool*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_19966,kk_context());
      kk_box_t _x21191 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_take_while_fun21192(predicate1, x1, xx1, _acc0, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x21191, _ctx);
    }
    if (x2_18054) {
      kk_std_core__list _ctail_173100 = kk_std_core__list_hole(); /*list<16299>*/;
      kk_std_core__list _ctail_173110 = kk_std_core__new_Cons(_ru_19966, x1, _ctail_173100, _ctx); /*list<16299>*/;
      kk_box_t* _b_19390_19382 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_173110)->tail)); /*cfield<list<16299>>*/;
      { // tailcall
        kk_std_core_types__ctail _x21194 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_173110, _ctx)),_b_19390_19382); /*ctail<0>*/
        xs = xx1;
        _acc0 = _x21194;
        goto kk__tailcall;
      }
    }
    {
      kk_reuse_drop(_ru_19966,kk_context());
      kk_std_core__list_drop(xx1, _ctx);
      kk_box_drop(x1, _ctx);
      kk_function_drop(predicate1, _ctx);
      kk_box_t _x21195 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
      return kk_std_core__list_unbox(_x21195, _ctx);
    }
  }
  {
    kk_function_drop(predicate1, _ctx);
    kk_box_t _x21196 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21196, _ctx);
  }
}
 
// Keep only those initial elements that satisfy `predicate`


// lift anonymous function
struct kk_std_core__ctailm_take_while_fun21201__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t predicate2;
  kk_box_t x3;
  kk_std_core__list xx2;
};
static kk_box_t kk_std_core__ctailm_take_while_fun21201(kk_function_t _fself, kk_box_t _b_19400, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_take_while_fun21201(kk_function_t _accm0, kk_function_t predicate2, kk_box_t x3, kk_std_core__list xx2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_take_while_fun21201__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_take_while_fun21201__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_take_while_fun21201, kk_context());
  _self->_accm0 = _accm0;
  _self->predicate2 = predicate2;
  _self->x3 = x3;
  _self->xx2 = xx2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_take_while_fun21201(kk_function_t _fself, kk_box_t _b_19400, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_take_while_fun21201__t* _self = kk_function_as(struct kk_std_core__ctailm_take_while_fun21201__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<16299>) -> list<16299> */
  kk_function_t predicate2 = _self->predicate2; /* (16299) -> 16300 bool */
  kk_box_t x3 = _self->x3; /* 16299 */
  kk_std_core__list xx2 = _self->xx2; /* list<16299> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(predicate2);kk_box_dup(x3);kk_std_core__list_dup(xx2);}, {}, _ctx)
  bool _y_19402_175870 = kk_bool_unbox(_b_19400); /*bool*/;
  kk_std_core__list _x21202 = kk_std_core__mlift17731_op(_accm0, predicate2, x3, xx2, _y_19402_175870, _ctx); /*list<16299>*/
  return kk_std_core__list_box(_x21202, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_take_while_fun21204__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t x3;
};
static kk_std_core__list kk_std_core__ctailm_take_while_fun21204(kk_function_t _fself, kk_std_core__list _ctail_173130, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_take_while_fun21204(kk_function_t _accm0, kk_box_t x3, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_take_while_fun21204__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_take_while_fun21204__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_take_while_fun21204, kk_context());
  _self->_accm0 = _accm0;
  _self->x3 = x3;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_take_while_fun21204(kk_function_t _fself, kk_std_core__list _ctail_173130, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_take_while_fun21204__t* _self = kk_function_as(struct kk_std_core__ctailm_take_while_fun21204__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<16299>) -> list<16299> */
  kk_box_t x3 = _self->x3; /* 16299 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(x3);}, {}, _ctx)
  kk_std_core__list _x21205 = kk_std_core__new_Cons(kk_reuse_null, x3, _ctail_173130, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x21205, _ctx));
}

kk_std_core__list kk_std_core__ctailm_take_while(kk_std_core__list xs0, kk_function_t predicate2, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, predicate : (a) -> e bool, (list<a>) -> list<a>) -> e list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con21197 = kk_std_core__as_Cons(xs0);
    kk_box_t x3 = _con21197->head;
    kk_std_core__list xx2 = _con21197->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs0))) {
      kk_std_core__list_free(xs0, _ctx);
    }
    else {
      kk_box_dup(x3);
      kk_std_core__list_dup(xx2);
      kk_std_core__list_decref(xs0, _ctx);
    }
    bool x4_18057;
    kk_function_t _x21199 = kk_function_dup(predicate2); /*(16299) -> 16300 bool*/
    kk_box_t _x21198 = kk_box_dup(x3); /*16299*/
    x4_18057 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x21199, (_x21199, _x21198, _ctx)); /*bool*/
    if (kk_yielding(kk_context())) {
      kk_box_t _x21200 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_take_while_fun21201(_accm0, predicate2, x3, xx2, _ctx), _ctx); /*3926*/
      return kk_std_core__list_unbox(_x21200, _ctx);
    }
    if (x4_18057) { // tailcall
                    kk_function_t _x21203 = kk_std_core__new_ctailm_take_while_fun21204(_accm0, x3, _ctx); /*(list<16299>) -> list<16299>*/
                    xs0 = xx2;
                    _accm0 = _x21203;
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
struct kk_std_core_take_while_fun21207__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core_take_while_fun21207(kk_function_t _fself, kk_std_core__list _ctail_17312, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_take_while_fun21207(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_take_while_fun21207, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core_take_while_fun21207(kk_function_t _fself, kk_std_core__list _ctail_17312, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17312;
}

kk_std_core__list kk_std_core_take_while(kk_std_core__list xs1, kk_function_t predicate3, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, predicate : (a) -> e bool) -> e list<a> */ 
  bool _match_19594 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_19594) {
    kk_std_core_types__ctail _x21206 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_take_while(xs1, predicate3, _x21206, _ctx);
  }
  {
    return kk_std_core__ctailm_take_while(xs1, predicate3, kk_std_core_new_take_while_fun21207(_ctx), _ctx);
  }
}
 
// Trace a message used for debug purposes.
// The behaviour is system dependent. On a browser and node it uses
// `console.log`  by default.
// Disabled if `notrace` is called.


// lift anonymous function
struct kk_std_core_trace_fun21211__t {
  struct kk_function_s _base;
  kk_string_t message0;
};
static kk_box_t kk_std_core_trace_fun21211(kk_function_t _fself, kk_box_t _b_19406, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_trace_fun21211(kk_string_t message0, kk_context_t* _ctx) {
  struct kk_std_core_trace_fun21211__t* _self = kk_function_alloc_as(struct kk_std_core_trace_fun21211__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_trace_fun21211, kk_context());
  _self->message0 = message0;
  return &_self->_base;
}

static kk_box_t kk_std_core_trace_fun21211(kk_function_t _fself, kk_box_t _b_19406, kk_context_t* _ctx) {
  struct kk_std_core_trace_fun21211__t* _self = kk_function_as(struct kk_std_core_trace_fun21211__t*, _fself);
  kk_string_t message0 = _self->message0; /* string */
  kk_drop_match(_self, {kk_string_dup(message0);}, {}, _ctx)
  kk_unit_t _x21212 = kk_Unit;
  bool _y_19408_17594 = kk_bool_unbox(_b_19406); /*bool*/;
  if (_y_19408_17594) {
    kk_std_core_xtrace(message0, _ctx);
  }
  else {
    kk_string_drop(message0, _ctx);
  }
  return kk_unit_box(_x21212);
}

kk_unit_t kk_std_core_trace(kk_string_t message0, kk_context_t* _ctx) { /* (message : string) -> () */ 
  bool x_18060;
  kk_box_t _x21208;
  kk_ref_t _x21209 = kk_ref_dup(kk_std_core_trace_enabled); /*ref<global,bool>*/
  _x21208 = kk_ref_get(_x21209,kk_context()); /*1001*/
  x_18060 = kk_bool_unbox(_x21208); /*bool*/
  if (kk_yielding(kk_context())) {
    kk_box_t _x21210 = kk_std_core_hnd_yield_extend(kk_std_core_new_trace_fun21211(message0, _ctx), _ctx); /*3926*/
    kk_unit_unbox(_x21210); return kk_Unit;
  }
  if (x_18060) {
    kk_std_core_xtrace(message0, _ctx); return kk_Unit;
  }
  {
    kk_string_drop(message0, _ctx);
    kk_Unit; return kk_Unit;
  }
}


// lift anonymous function
struct kk_std_core_trace_any_fun21216__t {
  struct kk_function_s _base;
  kk_string_t message0;
  kk_box_t x;
};
static kk_box_t kk_std_core_trace_any_fun21216(kk_function_t _fself, kk_box_t _b_19413, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_trace_any_fun21216(kk_string_t message0, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_trace_any_fun21216__t* _self = kk_function_alloc_as(struct kk_std_core_trace_any_fun21216__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_trace_any_fun21216, kk_context());
  _self->message0 = message0;
  _self->x = x;
  return &_self->_base;
}

static kk_box_t kk_std_core_trace_any_fun21216(kk_function_t _fself, kk_box_t _b_19413, kk_context_t* _ctx) {
  struct kk_std_core_trace_any_fun21216__t* _self = kk_function_as(struct kk_std_core_trace_any_fun21216__t*, _fself);
  kk_string_t message0 = _self->message0; /* string */
  kk_box_t x = _self->x; /* 16408 */
  kk_drop_match(_self, {kk_string_dup(message0);kk_box_dup(x);}, {}, _ctx)
  kk_unit_t _x21217 = kk_Unit;
  bool _y_19415_17596 = kk_bool_unbox(_b_19413); /*bool*/;
  if (_y_19415_17596) {
    kk_std_core_xtrace_any(message0, x, _ctx);
  }
  else {
    kk_box_drop(x, _ctx);
    kk_string_drop(message0, _ctx);
  }
  return kk_unit_box(_x21217);
}

kk_unit_t kk_std_core_trace_any(kk_string_t message0, kk_box_t x, kk_context_t* _ctx) { /* forall<a> (message : string, x : a) -> () */ 
  bool x0_18064;
  kk_box_t _x21213;
  kk_ref_t _x21214 = kk_ref_dup(kk_std_core_trace_enabled); /*ref<global,bool>*/
  _x21213 = kk_ref_get(_x21214,kk_context()); /*1001*/
  x0_18064 = kk_bool_unbox(_x21213); /*bool*/
  if (kk_yielding(kk_context())) {
    kk_box_t _x21215 = kk_std_core_hnd_yield_extend(kk_std_core_new_trace_any_fun21216(message0, x, _ctx), _ctx); /*3926*/
    kk_unit_unbox(_x21215); return kk_Unit;
  }
  if (x0_18064) {
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
  kk_std_core__sslice _x21218;
  kk_std_core__sslice _own_19583;
  bool _match_19584;
  kk_integer_t _brw_19589;
  kk_std_core_types__optional _x21219 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
  if (kk_std_core_types__is_Optional(_x21219)) {
    kk_box_t _box_x19417 = _x21219._cons.Optional.value;
    kk_integer_t _n_10142 = kk_integer_unbox(_box_x19417);
    _brw_19589 = _n_10142; /*int*/
  }
  else {
    _brw_19589 = kk_integer_from_small(1); /*int*/
  }
  bool _brw_19590 = kk_integer_eq_borrow(_brw_19589,(kk_integer_from_small(1)),kk_context()); /*bool*/;
  kk_integer_drop(_brw_19589, _ctx);
  _match_19584 = _brw_19590; /*bool*/
  if (_match_19584) {
    _own_19583 = slice0; /*sslice*/
  }
  else {
    kk_integer_t _brw_19586;
    kk_integer_t _x21221;
    kk_std_core_types__optional _x21222 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
    if (kk_std_core_types__is_Optional(_x21222)) {
      kk_box_t _box_x19418 = _x21222._cons.Optional.value;
      kk_integer_t _n_101420 = kk_integer_unbox(_box_x19418);
      _x21221 = _n_101420; /*int*/
    }
    else {
      _x21221 = kk_integer_from_small(1); /*int*/
    }
    _brw_19586 = kk_integer_add_small_const(_x21221, -1, _ctx); /*int*/
    kk_std_core__sslice _brw_19587 = kk_std_core_extend(slice0, _brw_19586, _ctx); /*sslice*/;
    kk_integer_drop(_brw_19586, _ctx);
    _own_19583 = _brw_19587; /*sslice*/
  }
  kk_integer_t _brw_19582 = kk_integer_add_small_const(count, -1, _ctx); /*int*/;
  kk_std_core__sslice _brw_19591 = kk_std_core_extend(_own_19583, _brw_19582, _ctx); /*sslice*/;
  kk_integer_drop(_brw_19582, _ctx);
  _x21218 = _brw_19591; /*sslice*/
  return kk_std_core_string_3(_x21218, _ctx);
}
 
// Return a default value when an exception is raised


// lift anonymous function
struct kk_std_core_try_default_fun21224__t {
  struct kk_function_s _base;
  kk_box_t value;
};
static kk_box_t kk_std_core_try_default_fun21224(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__525__16, kk_std_core__exception x, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_default_fun21224(kk_box_t value, kk_context_t* _ctx) {
  struct kk_std_core_try_default_fun21224__t* _self = kk_function_alloc_as(struct kk_std_core_try_default_fun21224__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_try_default_fun21224, kk_context());
  _self->value = value;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_try_default_fun21225__t {
  struct kk_function_s _base;
  kk_box_t value;
};
static kk_box_t kk_std_core_try_default_fun21225(kk_function_t _fself, kk_function_t ___wildcard__525__45, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_default_fun21225(kk_box_t value, kk_context_t* _ctx) {
  struct kk_std_core_try_default_fun21225__t* _self = kk_function_alloc_as(struct kk_std_core_try_default_fun21225__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_try_default_fun21225, kk_context());
  _self->value = value;
  return &_self->_base;
}

static kk_box_t kk_std_core_try_default_fun21225(kk_function_t _fself, kk_function_t ___wildcard__525__45, kk_context_t* _ctx) {
  struct kk_std_core_try_default_fun21225__t* _self = kk_function_as(struct kk_std_core_try_default_fun21225__t*, _fself);
  kk_box_t value = _self->value; /* 16484 */
  kk_drop_match(_self, {kk_box_dup(value);}, {}, _ctx)
  kk_function_drop(___wildcard__525__45, _ctx);
  return value;
}
static kk_box_t kk_std_core_try_default_fun21224(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__525__16, kk_std_core__exception x, kk_context_t* _ctx) {
  struct kk_std_core_try_default_fun21224__t* _self = kk_function_as(struct kk_std_core_try_default_fun21224__t*, _fself);
  kk_box_t value = _self->value; /* 16484 */
  kk_drop_match(_self, {kk_box_dup(value);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(___wildcard__525__16, (KK_I32(3)), _ctx);
  kk_std_core__exception_drop(x, _ctx);
  return kk_std_core_hnd_yield_to_final(m0, kk_std_core_new_try_default_fun21225(value, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_try_default_fun21228__t {
  struct kk_function_s _base;
  kk_function_t _b_19423_19419;
};
static kk_box_t kk_std_core_try_default_fun21228(kk_function_t _fself, kk_std_core_hnd__marker _b_19420, kk_std_core_hnd__ev _b_19421, kk_box_t _b_19422, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_default_fun21228(kk_function_t _b_19423_19419, kk_context_t* _ctx) {
  struct kk_std_core_try_default_fun21228__t* _self = kk_function_alloc_as(struct kk_std_core_try_default_fun21228__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_try_default_fun21228, kk_context());
  _self->_b_19423_19419 = _b_19423_19419;
  return &_self->_base;
}

static kk_box_t kk_std_core_try_default_fun21228(kk_function_t _fself, kk_std_core_hnd__marker _b_19420, kk_std_core_hnd__ev _b_19421, kk_box_t _b_19422, kk_context_t* _ctx) {
  struct kk_std_core_try_default_fun21228__t* _self = kk_function_as(struct kk_std_core_try_default_fun21228__t*, _fself);
  kk_function_t _b_19423_19419 = _self->_b_19423_19419; /* (m0 : std/core/hnd/marker<16485,16484>, std/core/hnd/ev<.hnd-exn>, x : exception) -> 16485 10295 */
  kk_drop_match(_self, {kk_function_dup(_b_19423_19419);}, {}, _ctx)
  kk_std_core__exception _x21229 = kk_std_core__exception_unbox(_b_19422, _ctx); /*exception*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_std_core__exception, kk_context_t*), _b_19423_19419, (_b_19423_19419, _b_19420, _b_19421, _x21229, _ctx));
}


// lift anonymous function
struct kk_std_core_try_default_fun21230__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_try_default_fun21230(kk_function_t _fself, kk_box_t _x, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_default_fun21230(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_try_default_fun21230, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_try_default_fun21230(kk_function_t _fself, kk_box_t _x, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _x;
}

kk_box_t kk_std_core_try_default(kk_box_t value, kk_function_t action, kk_context_t* _ctx) { /* forall<a,e> (value : a, action : () -> <exn|e> a) -> e a */ 
  kk_function_t _b_19423_19419 = kk_std_core_new_try_default_fun21224(value, _ctx); /*(m0 : std/core/hnd/marker<16485,16484>, std/core/hnd/ev<.hnd-exn>, x : exception) -> 16485 10295*/;
  kk_std_core__hnd_exn _x21226;
  kk_std_core_hnd__clause1 _x21227 = kk_std_core_hnd__new_Clause1(kk_std_core_new_try_default_fun21228(_b_19423_19419, _ctx), _ctx); /*std/core/hnd/clause1<51,52,53,54,55>*/
  _x21226 = kk_std_core__new_Hnd_exn(kk_reuse_null, _x21227, _ctx); /*.hnd-exn<11,12>*/
  return kk_std_core__handle_exn((KK_I32(0)), _x21226, kk_std_core_new_try_default_fun21230(_ctx), action, _ctx);
}
 
// Returns a unique integer (modulo 32-bits).


// lift anonymous function
struct kk_std_core_unique_fun21234__t {
  struct kk_function_s _base;
};
static kk_integer_t kk_std_core_unique_fun21234(kk_function_t _fself, kk_integer_t u, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_unique_fun21234(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_unique_fun21234, _ctx)
  return kk_function_dup(_fself);
}

static kk_integer_t kk_std_core_unique_fun21234(kk_function_t _fself, kk_integer_t u, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_integer_t _b_19433_19431;
  kk_integer_t _x21235 = kk_integer_dup(u); /*int*/
  _b_19433_19431 = kk_integer_add_small_const(_x21235, 1, _ctx); /*int*/
  kk_unit_t __ = kk_Unit;
  kk_ref_set_borrow(kk_std_core_unique_count,(kk_integer_box(_b_19433_19431)),kk_context());
  return u;
}


// lift anonymous function
struct kk_std_core_unique_fun21237__t {
  struct kk_function_s _base;
  kk_function_t next0_18071;
};
static kk_box_t kk_std_core_unique_fun21237(kk_function_t _fself, kk_box_t _b_19435, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_unique_fun21237(kk_function_t next0_18071, kk_context_t* _ctx) {
  struct kk_std_core_unique_fun21237__t* _self = kk_function_alloc_as(struct kk_std_core_unique_fun21237__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_unique_fun21237, kk_context());
  _self->next0_18071 = next0_18071;
  return &_self->_base;
}

static kk_box_t kk_std_core_unique_fun21237(kk_function_t _fself, kk_box_t _b_19435, kk_context_t* _ctx) {
  struct kk_std_core_unique_fun21237__t* _self = kk_function_as(struct kk_std_core_unique_fun21237__t*, _fself);
  kk_function_t next0_18071 = _self->next0_18071; /* (int) -> <read<global>,write<global>|_16545> int */
  kk_drop_match(_self, {kk_function_dup(next0_18071);}, {}, _ctx)
  kk_integer_t _x21238;
  kk_integer_t _x21239 = kk_integer_unbox(_b_19435); /*int*/
  _x21238 = kk_function_call(kk_integer_t, (kk_function_t, kk_integer_t, kk_context_t*), next0_18071, (next0_18071, _x21239, _ctx)); /*int*/
  return kk_integer_box(_x21238);
}

kk_integer_t kk_std_core_unique(kk_context_t* _ctx) { /* () -> ndet int */ 
  kk_integer_t x_18070;
  kk_box_t _x21232;
  kk_ref_t _x21233 = kk_ref_dup(kk_std_core_unique_count); /*ref<global,int>*/
  _x21232 = kk_ref_get(_x21233,kk_context()); /*1001*/
  x_18070 = kk_integer_unbox(_x21232); /*int*/
  kk_function_t next0_18071 = kk_std_core_new_unique_fun21234(_ctx); /*(int) -> <read<global>,write<global>|_16545> int*/;
  if (kk_yielding(kk_context())) {
    kk_integer_drop(x_18070, _ctx);
    kk_box_t _x21236 = kk_std_core_hnd_yield_extend(kk_std_core_new_unique_fun21237(next0_18071, _ctx), _ctx); /*3926*/
    return kk_integer_unbox(_x21236);
  }
  {
    return kk_function_call(kk_integer_t, (kk_function_t, kk_integer_t, kk_context_t*), next0_18071, (next0_18071, x_18070, _ctx));
  }
}
 
// Get the value of the `Just` constructor or raise an exception

kk_box_t kk_std_core_unjust(kk_std_core_types__maybe m, kk_context_t* _ctx) { /* forall<a> (m : maybe<a>) -> exn a */ 
  if (kk_std_core_types__is_Just(m)) {
    kk_box_t x = m._cons.Just.value;
    return x;
  }
  {
    kk_std_core_hnd__ev ev_18073;
    kk_ssize_t _x21240 = (KK_IZ(0)); /*ssize_t*/
    ev_18073 = kk_evv_at(_x21240,kk_context()); /*std/core/hnd/ev<.hnd-exn>*/
    {
      struct kk_std_core_hnd_Ev* _con21241 = kk_std_core_hnd__as_Ev(ev_18073);
      kk_std_core_hnd__marker m0 = _con21241->marker;
      kk_box_t _box_x19437 = _con21241->hnd;
      kk_std_core__hnd_exn h = kk_std_core__hnd_exn_unbox(_box_x19437, NULL);
      kk_std_core__hnd_exn_dup(h);
      kk_std_core_hnd__clause1 _match_19578;
      kk_std_core_hnd__clause1 _brw_19580 = kk_std_core__select_throw_exn(h, _ctx); /*std/core/hnd/clause1<exception,1938,.hnd-exn,1939,1940>*/;
      kk_std_core__hnd_exn_drop(h, _ctx);
      _match_19578 = _brw_19580; /*std/core/hnd/clause1<exception,1938,.hnd-exn,1939,1940>*/
      {
        kk_function_t _fun_unbox_x19441 = _match_19578.clause;
        kk_box_t _x21243;
        kk_std_core__exception _x21244;
        kk_string_t _x21245;
        kk_define_string_literal(, _s21246, 37, "unexpected Nothing in std/core/unjust")
        _x21245 = kk_string_dup(_s21246); /*string*/
        kk_std_core__exception_info _x21247;
        kk_std_core_types__optional _x21248 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
        if (kk_std_core_types__is_Optional(_x21248)) {
          kk_box_t _box_x19445 = _x21248._cons.Optional.value;
          kk_std_core__exception_info _info_11715 = kk_std_core__exception_info_unbox(_box_x19445, NULL);
          _x21247 = _info_11715; /*exception-info*/
        }
        else {
          _x21247 = kk_std_core__new_ExnError(_ctx); /*exception-info*/
        }
        _x21244 = kk_std_core__new_Exception(_x21245, _x21247, _ctx); /*exception*/
        _x21243 = kk_std_core__exception_box(_x21244, _ctx); /*51*/
        return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x19441, (_fun_unbox_x19441, m0, ev_18073, _x21243, _ctx));
      }
    }
  }
}
 
// lifted local: unlines, join-acc

kk_string_t kk_std_core__lift17203_unlines(kk_std_core__list ys, kk_string_t acc, kk_context_t* _ctx) { /* (ys : list<string>, acc : string) -> string */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con21250 = kk_std_core__as_Cons(ys);
    kk_box_t _box_x19446 = _con21250->head;
    kk_std_core__list yy = _con21250->tail;
    kk_string_t y = kk_string_unbox(_box_x19446);
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      kk_std_core__list_free(ys, _ctx);
    }
    else {
      kk_string_dup(y);
      kk_std_core__list_dup(yy);
      kk_std_core__list_decref(ys, _ctx);
    }
    { // tailcall
      kk_string_t _x21252;
      kk_string_t _x21253;
      kk_string_t _x21254;
      kk_define_string_literal(, _s21255, 1, "\n")
      _x21254 = kk_string_dup(_s21255); /*string*/
      _x21253 = kk_std_core__lp__plus__plus__1_rp_(_x21254, y, _ctx); /*string*/
      _x21252 = kk_std_core__lp__plus__plus__1_rp_(acc, _x21253, _ctx); /*string*/
      ys = yy;
      acc = _x21252;
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
    struct kk_std_core_Cons* _con21257 = kk_std_core__as_Cons(xs);
    kk_box_t _box_x19447 = _con21257->head;
    kk_std_core__list xx = _con21257->tail;
    kk_string_t x = kk_string_unbox(_box_x19447);
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_string_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    return kk_std_core__lift17203_unlines(xx, x, _ctx);
  }
}
 
// lifted local: .lift17204-unzip, unzip, reverse-acc

kk_std_core__list kk_std_core__lift17205_unzip(kk_std_core__list acc, kk_std_core__list ys0, kk_context_t* _ctx) { /* forall<a> (acc : list<a>, ys0 : list<a>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys0)) {
    struct kk_std_core_Cons* _con21259 = kk_std_core__as_Cons(ys0);
    kk_box_t x0 = _con21259->head;
    kk_std_core__list xx0 = _con21259->tail;
    kk_reuse_t _ru_19970 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(ys0))) {
      _ru_19970 = (kk_std_core__list_reuse(ys0));
    }
    else {
      kk_box_dup(x0);
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(ys0, _ctx);
    }
    { // tailcall
      kk_std_core__list _x21260 = kk_std_core__new_Cons(_ru_19970, x0, acc, _ctx); /*list<61>*/
      acc = _x21260;
      ys0 = xx0;
      goto kk__tailcall;
    }
  }
  {
    return acc;
  }
}
 
// lifted local: .lift17204-unzip, unzip, reverse-acc0

kk_std_core__list kk_std_core__lift17206_unzip(kk_std_core__list acc0, kk_std_core__list ys1, kk_context_t* _ctx) { /* forall<a> (acc0 : list<a>, ys1 : list<a>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys1)) {
    struct kk_std_core_Cons* _con21261 = kk_std_core__as_Cons(ys1);
    kk_box_t x1 = _con21261->head;
    kk_std_core__list xx1 = _con21261->tail;
    kk_reuse_t _ru_19971 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(ys1))) {
      _ru_19971 = (kk_std_core__list_reuse(ys1));
    }
    else {
      kk_box_dup(x1);
      kk_std_core__list_dup(xx1);
      kk_std_core__list_decref(ys1, _ctx);
    }
    { // tailcall
      kk_std_core__list _x21262 = kk_std_core__new_Cons(_ru_19971, x1, acc0, _ctx); /*list<61>*/
      acc0 = _x21262;
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

kk_std_core_types__tuple2_ kk_std_core__lift17204_unzip(kk_std_core__list ys, kk_std_core__list acc1, kk_std_core__list acc2, kk_context_t* _ctx) { /* forall<a,b> (ys : list<(a, b)>, acc1 : list<a>, acc2 : list<b>) -> (list<a>, list<b>) */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con21263 = kk_std_core__as_Cons(ys);
    kk_box_t _box_x19448 = _con21263->head;
    kk_std_core__list xx = _con21263->tail;
    kk_std_core_types__tuple2_ _pat0 = kk_std_core_types__tuple2__unbox(_box_x19448, NULL);
    kk_box_t x = _pat0.fst;
    kk_box_t y = _pat0.snd;
    kk_reuse_t _ru_19972 = kk_reuse_null; /*reuse*/;
    if (kk_likely(kk_std_core__list_is_unique(ys))) {
      kk_box_dup(x);
      kk_box_dup(y);
      kk_box_drop(_box_x19448, _ctx);
      _ru_19972 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_box_dup(y);
      kk_std_core__list_decref(ys, _ctx);
    }
    { // tailcall
      kk_std_core__list _x21265 = kk_std_core__new_Cons(_ru_19972, x, acc1, _ctx); /*list<61>*/
      kk_std_core__list _x21266 = kk_std_core__new_Cons(kk_reuse_null, y, acc2, _ctx); /*list<61>*/
      ys = xx;
      acc1 = _x21265;
      acc2 = _x21266;
      goto kk__tailcall;
    }
  }
  {
    kk_std_core__list _b_19451_19449 = kk_std_core__lift17205_unzip(kk_std_core__new_Nil(_ctx), acc1, _ctx); /*list<16600>*/;
    kk_std_core__list _b_19452_19450 = kk_std_core__lift17206_unzip(kk_std_core__new_Nil(_ctx), acc2, _ctx); /*list<16601>*/;
    return kk_std_core_types__new_dash__lp__comma__rp_(kk_std_core__list_box(_b_19451_19449, _ctx), kk_std_core__list_box(_b_19452_19450, _ctx), _ctx);
  }
}
 
// Convert a string to a vector of characters.

kk_vector_t kk_std_core_vector_1(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> vector<char> */ 
  return kk_string_to_chars(s,kk_context());
}
extern kk_box_t kk_std_core_vector_fun21269_2(kk_function_t _fself, kk_ssize_t ___wildcard__1854__30, kk_context_t* _ctx) {
  struct kk_std_core_vector_fun21269__t_2* _self = kk_function_as(struct kk_std_core_vector_fun21269__t_2*, _fself);
  kk_box_t default0 = _self->default0; /* 16677 */
  kk_drop_match(_self, {kk_box_dup(default0);}, {}, _ctx)
  return default0;
}
extern kk_box_t kk_std_core_vector_init_fun21272(kk_function_t _fself, kk_ssize_t i, kk_context_t* _ctx) {
  struct kk_std_core_vector_init_fun21272__t* _self = kk_function_as(struct kk_std_core_vector_init_fun21272__t*, _fself);
  kk_function_t f = _self->f; /* (int) -> 16747 */
  kk_drop_match(_self, {kk_function_dup(f);}, {}, _ctx)
  kk_integer_t _x21273 = kk_integer_from_ssize_t(i,kk_context()); /*int*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_context_t*), f, (f, _x21273, _ctx));
}
 
// monadic lift

kk_unit_t kk_std_core__mlift17735_while(kk_function_t action, kk_function_t predicate, kk_unit_t wild__, kk_context_t* _ctx) { /* forall<e> (action : () -> <div|e> (), predicate : () -> <div|e> bool, wild_ : ()) -> <div|e> () */ 
  kk_std_core_while(predicate, action, _ctx); return kk_Unit;
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift17736_while_fun21276__t {
  struct kk_function_s _base;
  kk_function_t action0;
  kk_function_t predicate0;
};
static kk_box_t kk_std_core__mlift17736_while_fun21276(kk_function_t _fself, kk_box_t _b_19454, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17736_while_fun21276(kk_function_t action0, kk_function_t predicate0, kk_context_t* _ctx) {
  struct kk_std_core__mlift17736_while_fun21276__t* _self = kk_function_alloc_as(struct kk_std_core__mlift17736_while_fun21276__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17736_while_fun21276, kk_context());
  _self->action0 = action0;
  _self->predicate0 = predicate0;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift17736_while_fun21276(kk_function_t _fself, kk_box_t _b_19454, kk_context_t* _ctx) {
  struct kk_std_core__mlift17736_while_fun21276__t* _self = kk_function_as(struct kk_std_core__mlift17736_while_fun21276__t*, _fself);
  kk_function_t action0 = _self->action0; /* () -> <div|16768> () */
  kk_function_t predicate0 = _self->predicate0; /* () -> <div|16768> bool */
  kk_drop_match(_self, {kk_function_dup(action0);kk_function_dup(predicate0);}, {}, _ctx)
  kk_unit_t wild__0_19456 = kk_Unit;
  kk_unit_unbox(_b_19454);
  kk_unit_t _x21277 = kk_Unit;
  kk_std_core__mlift17735_while(action0, predicate0, wild__0_19456, _ctx);
  return kk_unit_box(_x21277);
}

kk_unit_t kk_std_core__mlift17736_while(kk_function_t action0, kk_function_t predicate0, bool _y_17602, kk_context_t* _ctx) { /* forall<e> (action : () -> <div|e> (), predicate : () -> <div|e> bool, bool) -> <div|e> () */ 
  if (_y_17602) {
    kk_unit_t x_18076 = kk_Unit;
    kk_function_t _x21274 = kk_function_dup(action0); /*() -> <div|16768> ()*/
    kk_function_call(kk_unit_t, (kk_function_t, kk_context_t*), _x21274, (_x21274, _ctx));
    if (kk_yielding(kk_context())) {
      kk_box_t _x21275 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift17736_while_fun21276(action0, predicate0, _ctx), _ctx); /*3926*/
      kk_unit_unbox(_x21275); return kk_Unit;
    }
    {
      kk_std_core__mlift17735_while(action0, predicate0, x_18076, _ctx); return kk_Unit;
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
struct kk_std_core_while_fun21280__t {
  struct kk_function_s _base;
  kk_function_t action1;
  kk_function_t predicate1;
};
static kk_box_t kk_std_core_while_fun21280(kk_function_t _fself, kk_box_t _b_19458, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_while_fun21280(kk_function_t action1, kk_function_t predicate1, kk_context_t* _ctx) {
  struct kk_std_core_while_fun21280__t* _self = kk_function_alloc_as(struct kk_std_core_while_fun21280__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_while_fun21280, kk_context());
  _self->action1 = action1;
  _self->predicate1 = predicate1;
  return &_self->_base;
}

static kk_box_t kk_std_core_while_fun21280(kk_function_t _fself, kk_box_t _b_19458, kk_context_t* _ctx) {
  struct kk_std_core_while_fun21280__t* _self = kk_function_as(struct kk_std_core_while_fun21280__t*, _fself);
  kk_function_t action1 = _self->action1; /* () -> <div|16768> () */
  kk_function_t predicate1 = _self->predicate1; /* () -> <div|16768> bool */
  kk_drop_match(_self, {kk_function_dup(action1);kk_function_dup(predicate1);}, {}, _ctx)
  bool _y_19463_176020 = kk_bool_unbox(_b_19458); /*bool*/;
  kk_unit_t _x21281 = kk_Unit;
  kk_std_core__mlift17736_while(action1, predicate1, _y_19463_176020, _ctx);
  return kk_unit_box(_x21281);
}


// lift anonymous function
struct kk_std_core_while_fun21284__t {
  struct kk_function_s _base;
  kk_function_t action1;
  kk_function_t predicate1;
};
static kk_box_t kk_std_core_while_fun21284(kk_function_t _fself, kk_box_t _b_19460, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_while_fun21284(kk_function_t action1, kk_function_t predicate1, kk_context_t* _ctx) {
  struct kk_std_core_while_fun21284__t* _self = kk_function_alloc_as(struct kk_std_core_while_fun21284__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_while_fun21284, kk_context());
  _self->action1 = action1;
  _self->predicate1 = predicate1;
  return &_self->_base;
}

static kk_box_t kk_std_core_while_fun21284(kk_function_t _fself, kk_box_t _b_19460, kk_context_t* _ctx) {
  struct kk_std_core_while_fun21284__t* _self = kk_function_as(struct kk_std_core_while_fun21284__t*, _fself);
  kk_function_t action1 = _self->action1; /* () -> <div|16768> () */
  kk_function_t predicate1 = _self->predicate1; /* () -> <div|16768> bool */
  kk_drop_match(_self, {kk_function_dup(action1);kk_function_dup(predicate1);}, {}, _ctx)
  kk_unit_t wild__1_19464 = kk_Unit;
  kk_unit_unbox(_b_19460);
  kk_unit_t _x21285 = kk_Unit;
  kk_std_core__mlift17735_while(action1, predicate1, wild__1_19464, _ctx);
  return kk_unit_box(_x21285);
}

kk_unit_t kk_std_core_while(kk_function_t predicate1, kk_function_t action1, kk_context_t* _ctx) { /* forall<e> (predicate : () -> <div|e> bool, action : () -> <div|e> ()) -> <div|e> () */ 
  kk__tailcall: ;
  bool x0_18078;
  kk_function_t _x21278 = kk_function_dup(predicate1); /*() -> <div|16768> bool*/
  x0_18078 = kk_function_call(bool, (kk_function_t, kk_context_t*), _x21278, (_x21278, _ctx)); /*bool*/
  if (kk_yielding(kk_context())) {
    kk_box_t _x21279 = kk_std_core_hnd_yield_extend(kk_std_core_new_while_fun21280(action1, predicate1, _ctx), _ctx); /*3926*/
    kk_unit_unbox(_x21279); return kk_Unit;
  }
  if (x0_18078) {
    kk_unit_t x1_18081 = kk_Unit;
    kk_function_t _x21282 = kk_function_dup(action1); /*() -> <div|16768> ()*/
    kk_function_call(kk_unit_t, (kk_function_t, kk_context_t*), _x21282, (_x21282, _ctx));
    if (kk_yielding(kk_context())) {
      kk_box_t _x21283 = kk_std_core_hnd_yield_extend(kk_std_core_new_while_fun21284(action1, predicate1, _ctx), _ctx); /*3926*/
      kk_unit_unbox(_x21283); return kk_Unit;
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
    struct kk_std_core_Cons* _con21286 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con21286->head;
    kk_std_core__list xx = _con21286->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    if (kk_std_core__is_Cons(ys)) {
      struct kk_std_core_Cons* _con21287 = kk_std_core__as_Cons(ys);
      kk_box_t y = _con21287->head;
      kk_std_core__list yy = _con21287->tail;
      kk_reuse_t _ru_19974 = kk_reuse_null; /*reuse*/;
      if (kk_likely(kk_std_core__list_is_unique(ys))) {
        _ru_19974 = (kk_std_core__list_reuse(ys));
      }
      else {
        kk_box_dup(y);
        kk_std_core__list_dup(yy);
        kk_std_core__list_decref(ys, _ctx);
      }
      kk_std_core__list _ctail_17314 = kk_std_core__list_hole(); /*list<(16811, 16812)>*/;
      kk_std_core__list _ctail_17315;
      kk_box_t _x21288;
      kk_std_core_types__tuple2_ _x21289 = kk_std_core_types__new_dash__lp__comma__rp_(x, y, _ctx); /*(1004, 1005)*/
      _x21288 = kk_std_core_types__tuple2__box(_x21289, _ctx); /*61*/
      _ctail_17315 = kk_std_core__new_Cons(_ru_19974, _x21288, _ctail_17314, _ctx); /*list<(16811, 16812)>*/
      kk_box_t* _b_19481_19474 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17315)->tail)); /*cfield<list<(16811, 16812)>>*/;
      { // tailcall
        kk_std_core_types__ctail _x21290 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17315, _ctx)),_b_19481_19474); /*ctail<0>*/
        xs = xx;
        ys = yy;
        _acc = _x21290;
        goto kk__tailcall;
      }
    }
    {
      kk_std_core__list_drop(xx, _ctx);
      kk_box_drop(x, _ctx);
      kk_box_t _x21291 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
      return kk_std_core__list_unbox(_x21291, _ctx);
    }
  }
  {
    kk_std_core__list_drop(ys, _ctx);
    kk_box_t _x21292 = kk_ctail_resolve(_acc,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21292, _ctx);
  }
}
 
// Zip two lists together by pairing the corresponding elements.
// The returned list is only as long as the smallest input list.

kk_std_core__list kk_std_core_zip(kk_std_core__list xs0, kk_std_core__list ys0, kk_context_t* _ctx) { /* forall<a,b> (xs : list<a>, ys : list<b>) -> list<(a, b)> */ 
  kk_std_core_types__ctail _x21293 = kk_ctail_nil(); /*ctail<0>*/
  return kk_std_core__ctail_zip(xs0, ys0, _x21293, _ctx);
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift17737_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_std_core__list xx, kk_std_core__list yy, kk_box_t _ctail_17316, kk_context_t* _ctx) { /* forall<a,b,c,e> (ctail<list<c>>, f : (a, b) -> e c, xx : list<a>, yy : list<b>, c) -> e list<c> */ 
  kk_std_core__list _ctail_17317 = kk_std_core__list_hole(); /*list<16859>*/;
  kk_std_core__list _ctail_17318 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17316, _ctail_17317, _ctx); /*list<16859>*/;
  kk_box_t* _b_19497_19494 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17318)->tail)); /*cfield<list<16859>>*/;
  kk_std_core_types__ctail _x21294 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17318, _ctx)),_b_19497_19494); /*ctail<0>*/
  return kk_std_core__ctail_zipwith(xx, yy, f, _x21294, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift17738_op_fun21295__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t _ctail_17321;
};
static kk_std_core__list kk_std_core__mlift17738_op_fun21295(kk_function_t _fself, kk_std_core__list _ctail_17320, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17738_op_fun21295(kk_function_t _accm, kk_box_t _ctail_17321, kk_context_t* _ctx) {
  struct kk_std_core__mlift17738_op_fun21295__t* _self = kk_function_alloc_as(struct kk_std_core__mlift17738_op_fun21295__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17738_op_fun21295, kk_context());
  _self->_accm = _accm;
  _self->_ctail_17321 = _ctail_17321;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift17738_op_fun21295(kk_function_t _fself, kk_std_core__list _ctail_17320, kk_context_t* _ctx) {
  struct kk_std_core__mlift17738_op_fun21295__t* _self = kk_function_as(struct kk_std_core__mlift17738_op_fun21295__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<16859>) -> list<16859> */
  kk_box_t _ctail_17321 = _self->_ctail_17321; /* 16859 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(_ctail_17321);}, {}, _ctx)
  kk_std_core__list _x21296 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17321, _ctail_17320, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x21296, _ctx));
}

kk_std_core__list kk_std_core__mlift17738_op(kk_function_t _accm, kk_function_t f0, kk_std_core__list xx0, kk_std_core__list yy0, kk_box_t _ctail_17321, kk_context_t* _ctx) { /* forall<a,b,c,e> ((list<c>) -> list<c>, f : (a, b) -> e c, xx : list<a>, yy : list<b>, c) -> e list<c> */ 
  return kk_std_core__ctailm_zipwith(xx0, yy0, f0, kk_std_core__new_mlift17738_op_fun21295(_accm, _ctail_17321, _ctx), _ctx);
}
 
// Zip two lists together by apply a function `f` to all corresponding elements.
// The returned list is only as long as the smallest input list.


// lift anonymous function
struct kk_std_core__ctail_zipwith_fun21301__t {
  struct kk_function_s _base;
  kk_function_t f1;
  kk_std_core__list xx1;
  kk_std_core__list yy1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_zipwith_fun21301(kk_function_t _fself, kk_box_t _b_19502, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_zipwith_fun21301(kk_function_t f1, kk_std_core__list xx1, kk_std_core__list yy1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_zipwith_fun21301__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_zipwith_fun21301__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_zipwith_fun21301, kk_context());
  _self->f1 = f1;
  _self->xx1 = xx1;
  _self->yy1 = yy1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_zipwith_fun21301(kk_function_t _fself, kk_box_t _b_19502, kk_context_t* _ctx) {
  struct kk_std_core__ctail_zipwith_fun21301__t* _self = kk_function_as(struct kk_std_core__ctail_zipwith_fun21301__t*, _fself);
  kk_function_t f1 = _self->f1; /* (16857, 16858) -> 16860 16859 */
  kk_std_core__list xx1 = _self->xx1; /* list<16857> */
  kk_std_core__list yy1 = _self->yy1; /* list<16858> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<16859>> */
  kk_drop_match(_self, {kk_function_dup(f1);kk_std_core__list_dup(xx1);kk_std_core__list_dup(yy1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_box_t _ctail_19524_173160 = _b_19502; /*16859*/;
  kk_std_core__list _x21302 = kk_std_core__mlift17737_op(_acc0, f1, xx1, yy1, _ctail_19524_173160, _ctx); /*list<16859>*/
  return kk_std_core__list_box(_x21302, _ctx);
}

kk_std_core__list kk_std_core__ctail_zipwith(kk_std_core__list xs, kk_std_core__list ys, kk_function_t f1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,b,c,e> (xs : list<a>, ys : list<b>, f : (a, b) -> e c, ctail<list<c>>) -> e list<c> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21297 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con21297->head;
    kk_std_core__list xx1 = _con21297->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx1);
      kk_std_core__list_decref(xs, _ctx);
    }
    if (kk_std_core__is_Cons(ys)) {
      struct kk_std_core_Cons* _con21298 = kk_std_core__as_Cons(ys);
      kk_box_t y = _con21298->head;
      kk_std_core__list yy1 = _con21298->tail;
      kk_reuse_t _ru_19976 = kk_reuse_null; /*reuse*/;
      if (kk_likely(kk_std_core__list_is_unique(ys))) {
        _ru_19976 = (kk_std_core__list_reuse(ys));
      }
      else {
        kk_box_dup(y);
        kk_std_core__list_dup(yy1);
        kk_std_core__list_decref(ys, _ctx);
      }
      kk_box_t x0_18084;
      kk_function_t _x21299 = kk_function_dup(f1); /*(16857, 16858) -> 16860 16859*/
      x0_18084 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_context_t*), _x21299, (_x21299, x, y, _ctx)); /*16859*/
      if (kk_yielding(kk_context())) {
        kk_reuse_drop(_ru_19976,kk_context());
        kk_box_drop(x0_18084, _ctx);
        kk_box_t _x21300 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_zipwith_fun21301(f1, xx1, yy1, _acc0, _ctx), _ctx); /*3926*/
        return kk_std_core__list_unbox(_x21300, _ctx);
      }
      {
        kk_std_core__list _ctail_173170 = kk_std_core__list_hole(); /*list<16859>*/;
        kk_std_core__list _ctail_173180 = kk_std_core__new_Cons(_ru_19976, x0_18084, _ctail_173170, _ctx); /*list<16859>*/;
        kk_box_t* _b_19516_19508 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_173180)->tail)); /*cfield<list<16859>>*/;
        { // tailcall
          kk_std_core_types__ctail _x21303 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_173180, _ctx)),_b_19516_19508); /*ctail<0>*/
          xs = xx1;
          ys = yy1;
          _acc0 = _x21303;
          goto kk__tailcall;
        }
      }
    }
    {
      kk_std_core__list_drop(xx1, _ctx);
      kk_box_drop(x, _ctx);
      kk_function_drop(f1, _ctx);
      kk_box_t _x21304 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
      return kk_std_core__list_unbox(_x21304, _ctx);
    }
  }
  {
    kk_std_core__list_drop(ys, _ctx);
    kk_function_drop(f1, _ctx);
    kk_box_t _x21305 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21305, _ctx);
  }
}
 
// Zip two lists together by apply a function `f` to all corresponding elements.
// The returned list is only as long as the smallest input list.


// lift anonymous function
struct kk_std_core__ctailm_zipwith_fun21310__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t f2;
  kk_std_core__list xx2;
  kk_std_core__list yy2;
};
static kk_box_t kk_std_core__ctailm_zipwith_fun21310(kk_function_t _fself, kk_box_t _b_19526, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_zipwith_fun21310(kk_function_t _accm0, kk_function_t f2, kk_std_core__list xx2, kk_std_core__list yy2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_zipwith_fun21310__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_zipwith_fun21310__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_zipwith_fun21310, kk_context());
  _self->_accm0 = _accm0;
  _self->f2 = f2;
  _self->xx2 = xx2;
  _self->yy2 = yy2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_zipwith_fun21310(kk_function_t _fself, kk_box_t _b_19526, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_zipwith_fun21310__t* _self = kk_function_as(struct kk_std_core__ctailm_zipwith_fun21310__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<16859>) -> list<16859> */
  kk_function_t f2 = _self->f2; /* (16857, 16858) -> 16860 16859 */
  kk_std_core__list xx2 = _self->xx2; /* list<16857> */
  kk_std_core__list yy2 = _self->yy2; /* list<16858> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(f2);kk_std_core__list_dup(xx2);kk_std_core__list_dup(yy2);}, {}, _ctx)
  kk_box_t _ctail_19528_173210 = _b_19526; /*16859*/;
  kk_std_core__list _x21311 = kk_std_core__mlift17738_op(_accm0, f2, xx2, yy2, _ctail_19528_173210, _ctx); /*list<16859>*/
  return kk_std_core__list_box(_x21311, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_zipwith_fun21313__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t x2_18087;
};
static kk_std_core__list kk_std_core__ctailm_zipwith_fun21313(kk_function_t _fself, kk_std_core__list _ctail_173200, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_zipwith_fun21313(kk_function_t _accm0, kk_box_t x2_18087, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_zipwith_fun21313__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_zipwith_fun21313__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_zipwith_fun21313, kk_context());
  _self->_accm0 = _accm0;
  _self->x2_18087 = x2_18087;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_zipwith_fun21313(kk_function_t _fself, kk_std_core__list _ctail_173200, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_zipwith_fun21313__t* _self = kk_function_as(struct kk_std_core__ctailm_zipwith_fun21313__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<16859>) -> list<16859> */
  kk_box_t x2_18087 = _self->x2_18087; /* 16859 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(x2_18087);}, {}, _ctx)
  kk_std_core__list _x21314 = kk_std_core__new_Cons(kk_reuse_null, x2_18087, _ctail_173200, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x21314, _ctx));
}

kk_std_core__list kk_std_core__ctailm_zipwith(kk_std_core__list xs0, kk_std_core__list ys0, kk_function_t f2, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,b,c,e> (xs : list<a>, ys : list<b>, f : (a, b) -> e c, (list<c>) -> list<c>) -> e list<c> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con21306 = kk_std_core__as_Cons(xs0);
    kk_box_t x1 = _con21306->head;
    kk_std_core__list xx2 = _con21306->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs0))) {
      kk_std_core__list_free(xs0, _ctx);
    }
    else {
      kk_box_dup(x1);
      kk_std_core__list_dup(xx2);
      kk_std_core__list_decref(xs0, _ctx);
    }
    if (kk_std_core__is_Cons(ys0)) {
      struct kk_std_core_Cons* _con21307 = kk_std_core__as_Cons(ys0);
      kk_box_t y0 = _con21307->head;
      kk_std_core__list yy2 = _con21307->tail;
      if (kk_likely(kk_std_core__list_is_unique(ys0))) {
        kk_std_core__list_free(ys0, _ctx);
      }
      else {
        kk_box_dup(y0);
        kk_std_core__list_dup(yy2);
        kk_std_core__list_decref(ys0, _ctx);
      }
      kk_box_t x2_18087;
      kk_function_t _x21308 = kk_function_dup(f2); /*(16857, 16858) -> 16860 16859*/
      x2_18087 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_context_t*), _x21308, (_x21308, x1, y0, _ctx)); /*16859*/
      if (kk_yielding(kk_context())) {
        kk_box_drop(x2_18087, _ctx);
        kk_box_t _x21309 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_zipwith_fun21310(_accm0, f2, xx2, yy2, _ctx), _ctx); /*3926*/
        return kk_std_core__list_unbox(_x21309, _ctx);
      }
      { // tailcall
        kk_function_t _x21312 = kk_std_core__new_ctailm_zipwith_fun21313(_accm0, x2_18087, _ctx); /*(list<16859>) -> list<16859>*/
        xs0 = xx2;
        ys0 = yy2;
        _accm0 = _x21312;
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
struct kk_std_core_zipwith_fun21316__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core_zipwith_fun21316(kk_function_t _fself, kk_std_core__list _ctail_17319, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_zipwith_fun21316(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_zipwith_fun21316, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core_zipwith_fun21316(kk_function_t _fself, kk_std_core__list _ctail_17319, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17319;
}

kk_std_core__list kk_std_core_zipwith(kk_std_core__list xs1, kk_std_core__list ys1, kk_function_t f3, kk_context_t* _ctx) { /* forall<a,b,c,e> (xs : list<a>, ys : list<b>, f : (a, b) -> e c) -> e list<c> */ 
  bool _match_19572 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_19572) {
    kk_std_core_types__ctail _x21315 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_zipwith(xs1, ys1, f3, _x21315, _ctx);
  }
  {
    return kk_std_core__ctailm_zipwith(xs1, ys1, f3, kk_std_core_new_zipwith_fun21316(_ctx), _ctx);
  }
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift17739_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_integer_t i0_17233, kk_std_core__list xx, kk_std_core__list yy, kk_box_t _ctail_17322, kk_context_t* _ctx) { /* forall<a,b,c,e> (ctail<list<c>>, f : (int, a, b) -> e c, i0.17233 : int, xx : list<a>, yy : list<b>, c) -> e list<c> */ 
  kk_std_core__list _ctail_17323 = kk_std_core__list_hole(); /*list<16933>*/;
  kk_std_core__list _ctail_17324 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17322, _ctail_17323, _ctx); /*list<16933>*/;
  kk_box_t* _b_19537_19534 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_17324)->tail)); /*cfield<list<16933>>*/;
  kk_std_core_types__ctail _x21317 = kk_ctail_link(_acc,(kk_std_core__list_box(_ctail_17324, _ctx)),_b_19537_19534); /*ctail<0>*/
  return kk_std_core__ctail_lift17207_zipwith_indexed(f, i0_17233, xx, yy, _x21317, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift17740_op_fun21318__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t _ctail_17327;
};
static kk_std_core__list kk_std_core__mlift17740_op_fun21318(kk_function_t _fself, kk_std_core__list _ctail_17326, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift17740_op_fun21318(kk_function_t _accm, kk_box_t _ctail_17327, kk_context_t* _ctx) {
  struct kk_std_core__mlift17740_op_fun21318__t* _self = kk_function_alloc_as(struct kk_std_core__mlift17740_op_fun21318__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift17740_op_fun21318, kk_context());
  _self->_accm = _accm;
  _self->_ctail_17327 = _ctail_17327;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift17740_op_fun21318(kk_function_t _fself, kk_std_core__list _ctail_17326, kk_context_t* _ctx) {
  struct kk_std_core__mlift17740_op_fun21318__t* _self = kk_function_as(struct kk_std_core__mlift17740_op_fun21318__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<16933>) -> list<16933> */
  kk_box_t _ctail_17327 = _self->_ctail_17327; /* 16933 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(_ctail_17327);}, {}, _ctx)
  kk_std_core__list _x21319 = kk_std_core__new_Cons(kk_reuse_null, _ctail_17327, _ctail_17326, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x21319, _ctx));
}

kk_std_core__list kk_std_core__mlift17740_op(kk_function_t _accm, kk_function_t f0, kk_integer_t i0_172330, kk_std_core__list xx0, kk_std_core__list yy0, kk_box_t _ctail_17327, kk_context_t* _ctx) { /* forall<a,b,c,e> ((list<c>) -> list<c>, f : (int, a, b) -> e c, i0.17233 : int, xx : list<a>, yy : list<b>, c) -> e list<c> */ 
  return kk_std_core__ctailm_lift17207_zipwith_indexed(f0, i0_172330, xx0, yy0, kk_std_core__new_mlift17740_op_fun21318(_accm, _ctail_17327, _ctx), _ctx);
}
 
// lifted local: zipwith-indexed, zipwith-iter


// lift anonymous function
struct kk_std_core__ctail_lift17207_zipwith_indexed_fun21325__t {
  struct kk_function_s _base;
  kk_function_t f1;
  kk_integer_t i0_172331;
  kk_std_core__list xx1;
  kk_std_core__list yy1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_lift17207_zipwith_indexed_fun21325(kk_function_t _fself, kk_box_t _b_19542, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_lift17207_zipwith_indexed_fun21325(kk_function_t f1, kk_integer_t i0_172331, kk_std_core__list xx1, kk_std_core__list yy1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_lift17207_zipwith_indexed_fun21325__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_lift17207_zipwith_indexed_fun21325__t, 6, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_lift17207_zipwith_indexed_fun21325, kk_context());
  _self->f1 = f1;
  _self->i0_172331 = i0_172331;
  _self->xx1 = xx1;
  _self->yy1 = yy1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_lift17207_zipwith_indexed_fun21325(kk_function_t _fself, kk_box_t _b_19542, kk_context_t* _ctx) {
  struct kk_std_core__ctail_lift17207_zipwith_indexed_fun21325__t* _self = kk_function_as(struct kk_std_core__ctail_lift17207_zipwith_indexed_fun21325__t*, _fself);
  kk_function_t f1 = _self->f1; /* (int, 16931, 16932) -> 16934 16933 */
  kk_integer_t i0_172331 = _self->i0_172331; /* int */
  kk_std_core__list xx1 = _self->xx1; /* list<16931> */
  kk_std_core__list yy1 = _self->yy1; /* list<16932> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<16933>> */
  kk_drop_match(_self, {kk_function_dup(f1);kk_integer_dup(i0_172331);kk_std_core__list_dup(xx1);kk_std_core__list_dup(yy1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_box_t _ctail_19564_173220 = _b_19542; /*16933*/;
  kk_std_core__list _x21326 = kk_std_core__mlift17739_op(_acc0, f1, i0_172331, xx1, yy1, _ctail_19564_173220, _ctx); /*list<16933>*/
  return kk_std_core__list_box(_x21326, _ctx);
}

kk_std_core__list kk_std_core__ctail_lift17207_zipwith_indexed(kk_function_t f1, kk_integer_t i, kk_std_core__list xs, kk_std_core__list ys, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,b,c,e> (f : (int, a, b) -> e c, i : int, xs : list<a>, ys : list<b>, ctail<list<c>>) -> e list<c> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con21320 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con21320->head;
    kk_std_core__list xx1 = _con21320->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs))) {
      kk_std_core__list_free(xs, _ctx);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx1);
      kk_std_core__list_decref(xs, _ctx);
    }
    if (kk_std_core__is_Cons(ys)) {
      struct kk_std_core_Cons* _con21321 = kk_std_core__as_Cons(ys);
      kk_box_t y = _con21321->head;
      kk_std_core__list yy1 = _con21321->tail;
      kk_reuse_t _ru_19980 = kk_reuse_null; /*reuse*/;
      if (kk_likely(kk_std_core__list_is_unique(ys))) {
        _ru_19980 = (kk_std_core__list_reuse(ys));
      }
      else {
        kk_box_dup(y);
        kk_std_core__list_dup(yy1);
        kk_std_core__list_decref(ys, _ctx);
      }
      kk_integer_t i0_172331;
      kk_integer_t _x21322 = kk_integer_dup(i); /*int*/
      i0_172331 = kk_integer_add_small_const(_x21322, 1, _ctx); /*int*/
      kk_box_t x0_18090;
      kk_function_t _x21323 = kk_function_dup(f1); /*(int, 16931, 16932) -> 16934 16933*/
      x0_18090 = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_box_t, kk_box_t, kk_context_t*), _x21323, (_x21323, i, x, y, _ctx)); /*16933*/
      if (kk_yielding(kk_context())) {
        kk_reuse_drop(_ru_19980,kk_context());
        kk_box_drop(x0_18090, _ctx);
        kk_box_t _x21324 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_lift17207_zipwith_indexed_fun21325(f1, i0_172331, xx1, yy1, _acc0, _ctx), _ctx); /*3926*/
        return kk_std_core__list_unbox(_x21324, _ctx);
      }
      {
        kk_std_core__list _ctail_173230 = kk_std_core__list_hole(); /*list<16933>*/;
        kk_std_core__list _ctail_173240 = kk_std_core__new_Cons(_ru_19980, x0_18090, _ctail_173230, _ctx); /*list<16933>*/;
        kk_box_t* _b_19556_19548 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_173240)->tail)); /*cfield<list<16933>>*/;
        { // tailcall
          kk_std_core_types__ctail _x21327 = kk_ctail_link(_acc0,(kk_std_core__list_box(_ctail_173240, _ctx)),_b_19556_19548); /*ctail<0>*/
          i = i0_172331;
          xs = xx1;
          ys = yy1;
          _acc0 = _x21327;
          goto kk__tailcall;
        }
      }
    }
    {
      kk_std_core__list_drop(xx1, _ctx);
      kk_box_drop(x, _ctx);
      kk_integer_drop(i, _ctx);
      kk_function_drop(f1, _ctx);
      kk_box_t _x21328 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
      return kk_std_core__list_unbox(_x21328, _ctx);
    }
  }
  {
    kk_std_core__list_drop(ys, _ctx);
    kk_integer_drop(i, _ctx);
    kk_function_drop(f1, _ctx);
    kk_box_t _x21329 = kk_ctail_resolve(_acc0,(kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx))); /*-1*/
    return kk_std_core__list_unbox(_x21329, _ctx);
  }
}
 
// lifted local: zipwith-indexed, zipwith-iter


// lift anonymous function
struct kk_std_core__ctailm_lift17207_zipwith_indexed_fun21335__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t f2;
  kk_integer_t i0_172332;
  kk_std_core__list xx2;
  kk_std_core__list yy2;
};
static kk_box_t kk_std_core__ctailm_lift17207_zipwith_indexed_fun21335(kk_function_t _fself, kk_box_t _b_19566, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_lift17207_zipwith_indexed_fun21335(kk_function_t _accm0, kk_function_t f2, kk_integer_t i0_172332, kk_std_core__list xx2, kk_std_core__list yy2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17207_zipwith_indexed_fun21335__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_lift17207_zipwith_indexed_fun21335__t, 6, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_lift17207_zipwith_indexed_fun21335, kk_context());
  _self->_accm0 = _accm0;
  _self->f2 = f2;
  _self->i0_172332 = i0_172332;
  _self->xx2 = xx2;
  _self->yy2 = yy2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_lift17207_zipwith_indexed_fun21335(kk_function_t _fself, kk_box_t _b_19566, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17207_zipwith_indexed_fun21335__t* _self = kk_function_as(struct kk_std_core__ctailm_lift17207_zipwith_indexed_fun21335__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<16933>) -> list<16933> */
  kk_function_t f2 = _self->f2; /* (int, 16931, 16932) -> 16934 16933 */
  kk_integer_t i0_172332 = _self->i0_172332; /* int */
  kk_std_core__list xx2 = _self->xx2; /* list<16931> */
  kk_std_core__list yy2 = _self->yy2; /* list<16932> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(f2);kk_integer_dup(i0_172332);kk_std_core__list_dup(xx2);kk_std_core__list_dup(yy2);}, {}, _ctx)
  kk_box_t _ctail_19568_173270 = _b_19566; /*16933*/;
  kk_std_core__list _x21336 = kk_std_core__mlift17740_op(_accm0, f2, i0_172332, xx2, yy2, _ctail_19568_173270, _ctx); /*list<16933>*/
  return kk_std_core__list_box(_x21336, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_lift17207_zipwith_indexed_fun21338__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t x2_18093;
};
static kk_std_core__list kk_std_core__ctailm_lift17207_zipwith_indexed_fun21338(kk_function_t _fself, kk_std_core__list _ctail_173260, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_lift17207_zipwith_indexed_fun21338(kk_function_t _accm0, kk_box_t x2_18093, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17207_zipwith_indexed_fun21338__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_lift17207_zipwith_indexed_fun21338__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_lift17207_zipwith_indexed_fun21338, kk_context());
  _self->_accm0 = _accm0;
  _self->x2_18093 = x2_18093;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_lift17207_zipwith_indexed_fun21338(kk_function_t _fself, kk_std_core__list _ctail_173260, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift17207_zipwith_indexed_fun21338__t* _self = kk_function_as(struct kk_std_core__ctailm_lift17207_zipwith_indexed_fun21338__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<16933>) -> list<16933> */
  kk_box_t x2_18093 = _self->x2_18093; /* 16933 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(x2_18093);}, {}, _ctx)
  kk_std_core__list _x21339 = kk_std_core__new_Cons(kk_reuse_null, x2_18093, _ctail_173260, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x21339, _ctx));
}

kk_std_core__list kk_std_core__ctailm_lift17207_zipwith_indexed(kk_function_t f2, kk_integer_t i0, kk_std_core__list xs0, kk_std_core__list ys0, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,b,c,e> (f : (int, a, b) -> e c, i : int, xs : list<a>, ys : list<b>, (list<c>) -> list<c>) -> e list<c> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con21330 = kk_std_core__as_Cons(xs0);
    kk_box_t x1 = _con21330->head;
    kk_std_core__list xx2 = _con21330->tail;
    if (kk_likely(kk_std_core__list_is_unique(xs0))) {
      kk_std_core__list_free(xs0, _ctx);
    }
    else {
      kk_box_dup(x1);
      kk_std_core__list_dup(xx2);
      kk_std_core__list_decref(xs0, _ctx);
    }
    if (kk_std_core__is_Cons(ys0)) {
      struct kk_std_core_Cons* _con21331 = kk_std_core__as_Cons(ys0);
      kk_box_t y0 = _con21331->head;
      kk_std_core__list yy2 = _con21331->tail;
      if (kk_likely(kk_std_core__list_is_unique(ys0))) {
        kk_std_core__list_free(ys0, _ctx);
      }
      else {
        kk_box_dup(y0);
        kk_std_core__list_dup(yy2);
        kk_std_core__list_decref(ys0, _ctx);
      }
      kk_integer_t i0_172332;
      kk_integer_t _x21332 = kk_integer_dup(i0); /*int*/
      i0_172332 = kk_integer_add_small_const(_x21332, 1, _ctx); /*int*/
      kk_box_t x2_18093;
      kk_function_t _x21333 = kk_function_dup(f2); /*(int, 16931, 16932) -> 16934 16933*/
      x2_18093 = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_box_t, kk_box_t, kk_context_t*), _x21333, (_x21333, i0, x1, y0, _ctx)); /*16933*/
      if (kk_yielding(kk_context())) {
        kk_box_drop(x2_18093, _ctx);
        kk_box_t _x21334 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_lift17207_zipwith_indexed_fun21335(_accm0, f2, i0_172332, xx2, yy2, _ctx), _ctx); /*3926*/
        return kk_std_core__list_unbox(_x21334, _ctx);
      }
      { // tailcall
        kk_function_t _x21337 = kk_std_core__new_ctailm_lift17207_zipwith_indexed_fun21338(_accm0, x2_18093, _ctx); /*(list<16933>) -> list<16933>*/
        i0 = i0_172332;
        xs0 = xx2;
        ys0 = yy2;
        _accm0 = _x21337;
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
struct kk_std_core__lift17207_zipwith_indexed_fun21341__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core__lift17207_zipwith_indexed_fun21341(kk_function_t _fself, kk_std_core__list _ctail_17325, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift17207_zipwith_indexed_fun21341(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core__lift17207_zipwith_indexed_fun21341, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core__lift17207_zipwith_indexed_fun21341(kk_function_t _fself, kk_std_core__list _ctail_17325, kk_context_t* _ctx) {
  kk_unused(_fself);
  return _ctail_17325;
}

kk_std_core__list kk_std_core__lift17207_zipwith_indexed(kk_function_t f3, kk_integer_t i1, kk_std_core__list xs1, kk_std_core__list ys1, kk_context_t* _ctx) { /* forall<a,b,c,e> (f : (int, a, b) -> e c, i : int, xs : list<a>, ys : list<b>) -> e list<c> */ 
  bool _match_19569 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_19569) {
    kk_std_core_types__ctail _x21340 = kk_ctail_nil(); /*ctail<0>*/
    return kk_std_core__ctail_lift17207_zipwith_indexed(f3, i1, xs1, ys1, _x21340, _ctx);
  }
  {
    return kk_std_core__ctailm_lift17207_zipwith_indexed(f3, i1, xs1, ys1, kk_std_core__new_lift17207_zipwith_indexed_fun21341(_ctx), _ctx);
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
    kk_string_t _x20011;
    kk_define_string_literal(, _s20012, 8, "exn.core")
    _x20011 = kk_string_dup(_s20012); /*string*/
    kk_std_core__tag_exn = kk_std_core_hnd__new_Htag(_x20011, _ctx); /*std/core/hnd/htag<.hnd-exn>*/
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
    kk_string_t _x20706 = kk_string_empty(); /*string*/
    kk_ssize_t _x20708 = (KK_IZ(0)); /*ssize_t*/
    kk_ssize_t _x20709 = (KK_IZ(0)); /*ssize_t*/
    kk_std_core_empty = kk_std_core__new_Sslice(_x20706, _x20708, _x20709, _ctx); /*sslice*/
  }
  {
    kk_string_t _x20919 = kk_string_empty(); /*string*/
    kk_ssize_t _x20921 = (KK_IZ(-1)); /*ssize_t*/
    kk_ssize_t _x20922 = (KK_IZ(0)); /*ssize_t*/
    kk_std_core_invalid = kk_std_core__new_Sslice(_x20919, _x20921, _x20922, _ctx); /*sslice*/
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
