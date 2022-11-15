#pragma once
#ifndef kk_std_core_H
#define kk_std_core_H
// Koka generated module: "std/core", koka version: 2.4.0, platform: 64-bit
#include <kklib.h>
#include "std_core_types.h"
#include "std_core_hnd.h"
/*---------------------------------------------------------------------------
  Copyright 2020-2021, Microsoft Research, Daan Leijen.

  This is free software; you can redistribute it and/or modify it under the
  terms of the Apache License, Version 2.0. A copy of the License can be
  found in the LICENSE file at the root of this distribution.
---------------------------------------------------------------------------*/

kk_box_t kk_std_core_error_pattern(kk_string_t location, kk_string_t definition, kk_context_t* _ctx);

static inline kk_std_core_types__order kk_int_as_order(int i,kk_context_t* ctx) {
  return (i==0 ? kk_std_core_types__new_Eq(ctx) : (i > 0 ? kk_std_core_types__new_Gt(ctx) : kk_std_core_types__new_Lt(ctx)));
}

static inline kk_std_core_types__maybe kk_integer_xparse( kk_string_t s, bool hex, kk_context_t* ctx ) {
  kk_integer_t i;
  bool ok = (hex ? kk_integer_hex_parse(kk_string_cbuf_borrow(s,NULL),&i,ctx) : kk_integer_parse(kk_string_cbuf_borrow(s,NULL),&i,ctx) );
  kk_string_drop(s,ctx);
  return (ok ? kk_std_core_types__new_Just(kk_integer_box(i),ctx) : kk_std_core_types__new_Nothing(ctx));
}

struct kk_std_core_Sslice;

kk_datatype_t kk_string_to_list(kk_string_t s, kk_context_t* ctx);
kk_string_t   kk_string_from_list(kk_datatype_t cs, kk_context_t* ctx);

kk_datatype_t  kk_vector_to_list(kk_vector_t v, kk_datatype_t tail, kk_context_t* ctx);
kk_vector_t    kk_list_to_vector(kk_datatype_t xs, kk_context_t* ctx);

static inline kk_integer_t  kk_string_count_int(kk_string_t s, kk_context_t* ctx) {
  return kk_integer_from_ssize_t( kk_string_count(s,ctx), ctx );
}

static inline kk_integer_t kk_string_cmp_int(kk_string_t s1, kk_string_t s2, kk_context_t* ctx) {
  return kk_integer_from_small( kk_string_cmp(s1,s2,ctx) );
}

kk_string_t  kk_string_join(kk_vector_t v, kk_context_t* ctx);
kk_string_t  kk_string_join_with(kk_vector_t v, kk_string_t sep, kk_context_t* ctx);
kk_string_t  kk_string_replace_all(kk_string_t str, kk_string_t pattern, kk_string_t repl, kk_context_t* ctx);
static inline kk_integer_t kk_string_count_pattern(kk_string_t str, kk_string_t pattern, kk_context_t* ctx) {
  kk_integer_t count = kk_integer_from_ssize_t( kk_string_count_pattern_borrow(str,pattern), ctx );
  kk_string_drop(str,ctx);
  kk_string_drop(pattern,ctx);
  return count;
}

kk_integer_t kk_slice_count( struct kk_std_core_Sslice sslice, kk_context_t* ctx );
kk_string_t  kk_slice_to_string( struct kk_std_core_Sslice sslice, kk_context_t* ctx );
struct kk_std_core_Sslice kk_slice_first( kk_string_t str, kk_context_t* ctx );
struct kk_std_core_Sslice kk_slice_last( kk_string_t str, kk_context_t* ctx );

struct kk_std_core_Sslice kk_slice_common_prefix_borrow( kk_string_t str1, kk_string_t str2, kk_integer_t upto, kk_context_t* ctx );
struct kk_std_core_Sslice kk_slice_advance_borrow( struct kk_std_core_Sslice slice, kk_integer_t count, kk_context_t* ctx );
struct kk_std_core_Sslice kk_slice_extend_borrow( struct kk_std_core_Sslice slice, kk_integer_t count, kk_context_t* ctx );
kk_std_core_types__maybe kk_slice_next( struct kk_std_core_Sslice slice, kk_context_t* ctx );


static inline kk_unit_t kk_vector_unsafe_assign( kk_vector_t v, kk_ssize_t i, kk_box_t x, kk_context_t* ctx  ) {
  kk_ssize_t len;
  kk_box_t* p = kk_vector_buf_borrow(v,&len);
  kk_assert(i < len);
  p[i] = x;
  kk_vector_drop(v,ctx); // TODO: use borrowing
  return kk_Unit;
}

kk_vector_t kk_vector_init( kk_ssize_t n, kk_function_t init, kk_context_t* ctx);

static inline kk_box_t kk_vector_at_int_borrow( kk_vector_t v, kk_integer_t n, kk_context_t* ctx) {
  // TODO: check bounds
  kk_box_t b = kk_vector_at_borrow(v,kk_integer_clamp_ssize_t_borrow(n,ctx));
  return b;
}

static inline double kk_double_abs(double d) {
  return (isfinite(d) && d < 0.0 ? -d : d);
}

static inline kk_std_core_types__tuple2_ kk_integer_div_mod_tuple(kk_integer_t x, kk_integer_t y, kk_context_t* ctx) {
  kk_integer_t mod;
  kk_integer_t div = kk_integer_div_mod(x,y,&mod,ctx);
  return kk_std_core_types__new_dash__lp__comma__rp_(kk_integer_box(div),kk_integer_box(mod),ctx);
}

kk_box_t kk_main_console( kk_function_t action, kk_context_t* ctx );
kk_unit_t kk_assert_fail( kk_string_t msg, kk_context_t* ctx );

struct kk_std_core_error_s;
struct kk_std_core_error_s kk_error_ok( kk_box_t result, kk_context_t* ctx );
struct kk_std_core_error_s kk_error_from_errno( int err, kk_context_t* ctx );




// type declarations

// type std/core/exception-info
struct kk_std_core__exception_info_s {
  kk_block_t _block;
  kk_string_t _tag;
};
typedef struct kk_std_core__exception_info_s* kk_std_core__exception_info;
struct kk_std_core_ExnError {
  struct kk_std_core__exception_info_s _base;
};
struct kk_std_core_ExnAssert {
  struct kk_std_core__exception_info_s _base;
};
struct kk_std_core_ExnTodo {
  struct kk_std_core__exception_info_s _base;
};
struct kk_std_core_ExnRange {
  struct kk_std_core__exception_info_s _base;
};
struct kk_std_core_ExnPattern {
  struct kk_std_core__exception_info_s _base;
  kk_string_t location;
  kk_string_t definition;
};
struct kk_std_core_ExnSystem {
  struct kk_std_core__exception_info_s _base;
  kk_integer_t kkloc_errno;
};
struct kk_std_core_ExnInternal {
  struct kk_std_core__exception_info_s _base;
  kk_string_t name;
};
static inline kk_std_core__exception_info kk_std_core__base_ExnError(struct kk_std_core_ExnError* _x) {
  return &_x->_base;
}
extern kk_string_t kk_std_core__tag_ExnError;
static inline kk_std_core__exception_info kk_std_core__new_ExnError(kk_context_t* _ctx) {
  struct kk_std_core_ExnError* _con = kk_block_alloc_at_as(struct kk_std_core_ExnError, kk_reuse_null, 1 /* scan count */, KK_TAG_OPEN, _ctx);
  _con->_base._tag = kk_string_dup(kk_std_core__tag_ExnError);
  return kk_std_core__base_ExnError(_con);
}
static inline struct kk_std_core_ExnError* kk_std_core__as_ExnError(kk_std_core__exception_info x) {
  return kk_basetype_as_assert(struct kk_std_core_ExnError*, x, KK_TAG_OPEN);
}
static inline kk_std_core__exception_info kk_std_core__base_ExnAssert(struct kk_std_core_ExnAssert* _x) {
  return &_x->_base;
}
extern kk_string_t kk_std_core__tag_ExnAssert;
static inline kk_std_core__exception_info kk_std_core__new_ExnAssert(kk_context_t* _ctx) {
  struct kk_std_core_ExnAssert* _con = kk_block_alloc_at_as(struct kk_std_core_ExnAssert, kk_reuse_null, 1 /* scan count */, KK_TAG_OPEN, _ctx);
  _con->_base._tag = kk_string_dup(kk_std_core__tag_ExnAssert);
  return kk_std_core__base_ExnAssert(_con);
}
static inline struct kk_std_core_ExnAssert* kk_std_core__as_ExnAssert(kk_std_core__exception_info x) {
  return kk_basetype_as_assert(struct kk_std_core_ExnAssert*, x, KK_TAG_OPEN);
}
static inline kk_std_core__exception_info kk_std_core__base_ExnTodo(struct kk_std_core_ExnTodo* _x) {
  return &_x->_base;
}
extern kk_string_t kk_std_core__tag_ExnTodo;
static inline kk_std_core__exception_info kk_std_core__new_ExnTodo(kk_context_t* _ctx) {
  struct kk_std_core_ExnTodo* _con = kk_block_alloc_at_as(struct kk_std_core_ExnTodo, kk_reuse_null, 1 /* scan count */, KK_TAG_OPEN, _ctx);
  _con->_base._tag = kk_string_dup(kk_std_core__tag_ExnTodo);
  return kk_std_core__base_ExnTodo(_con);
}
static inline struct kk_std_core_ExnTodo* kk_std_core__as_ExnTodo(kk_std_core__exception_info x) {
  return kk_basetype_as_assert(struct kk_std_core_ExnTodo*, x, KK_TAG_OPEN);
}
static inline kk_std_core__exception_info kk_std_core__base_ExnRange(struct kk_std_core_ExnRange* _x) {
  return &_x->_base;
}
extern kk_string_t kk_std_core__tag_ExnRange;
static inline kk_std_core__exception_info kk_std_core__new_ExnRange(kk_context_t* _ctx) {
  struct kk_std_core_ExnRange* _con = kk_block_alloc_at_as(struct kk_std_core_ExnRange, kk_reuse_null, 1 /* scan count */, KK_TAG_OPEN, _ctx);
  _con->_base._tag = kk_string_dup(kk_std_core__tag_ExnRange);
  return kk_std_core__base_ExnRange(_con);
}
static inline struct kk_std_core_ExnRange* kk_std_core__as_ExnRange(kk_std_core__exception_info x) {
  return kk_basetype_as_assert(struct kk_std_core_ExnRange*, x, KK_TAG_OPEN);
}
static inline kk_std_core__exception_info kk_std_core__base_ExnSystem(struct kk_std_core_ExnSystem* _x) {
  return &_x->_base;
}
extern kk_string_t kk_std_core__tag_ExnSystem;
static inline kk_std_core__exception_info kk_std_core__new_ExnSystem(kk_reuse_t _at, kk_integer_t kkloc_errno, kk_context_t* _ctx) {
  struct kk_std_core_ExnSystem* _con = kk_block_alloc_at_as(struct kk_std_core_ExnSystem, _at, 2 /* scan count */, KK_TAG_OPEN, _ctx);
  _con->_base._tag = kk_string_dup(kk_std_core__tag_ExnSystem);
  _con->kkloc_errno = kkloc_errno;
  return kk_std_core__base_ExnSystem(_con);
}
static inline struct kk_std_core_ExnSystem* kk_std_core__as_ExnSystem(kk_std_core__exception_info x) {
  return kk_basetype_as_assert(struct kk_std_core_ExnSystem*, x, KK_TAG_OPEN);
}
static inline kk_std_core__exception_info kk_std_core__base_ExnInternal(struct kk_std_core_ExnInternal* _x) {
  return &_x->_base;
}
extern kk_string_t kk_std_core__tag_ExnInternal;
static inline kk_std_core__exception_info kk_std_core__new_ExnInternal(kk_reuse_t _at, kk_string_t name, kk_context_t* _ctx) {
  struct kk_std_core_ExnInternal* _con = kk_block_alloc_at_as(struct kk_std_core_ExnInternal, _at, 2 /* scan count */, KK_TAG_OPEN, _ctx);
  _con->_base._tag = kk_string_dup(kk_std_core__tag_ExnInternal);
  _con->name = name;
  return kk_std_core__base_ExnInternal(_con);
}
static inline struct kk_std_core_ExnInternal* kk_std_core__as_ExnInternal(kk_std_core__exception_info x) {
  return kk_basetype_as_assert(struct kk_std_core_ExnInternal*, x, KK_TAG_OPEN);
}
static inline kk_std_core__exception_info kk_std_core__base_ExnPattern(struct kk_std_core_ExnPattern* _x) {
  return &_x->_base;
}
extern kk_string_t kk_std_core__tag_ExnPattern;
static inline kk_std_core__exception_info kk_std_core__new_ExnPattern(kk_reuse_t _at, kk_string_t location, kk_string_t definition, kk_context_t* _ctx) {
  struct kk_std_core_ExnPattern* _con = kk_block_alloc_at_as(struct kk_std_core_ExnPattern, _at, 3 /* scan count */, KK_TAG_OPEN, _ctx);
  _con->_base._tag = kk_string_dup(kk_std_core__tag_ExnPattern);
  _con->location = location;
  _con->definition = definition;
  return kk_std_core__base_ExnPattern(_con);
}
static inline struct kk_std_core_ExnPattern* kk_std_core__as_ExnPattern(kk_std_core__exception_info x) {
  return kk_basetype_as_assert(struct kk_std_core_ExnPattern*, x, KK_TAG_OPEN);
}
static inline bool kk_std_core__is_ExnError(kk_std_core__exception_info x) {
  return (kk_string_ptr_eq_borrow(x->_tag, kk_std_core__tag_ExnError));
}
static inline bool kk_std_core__is_ExnAssert(kk_std_core__exception_info x) {
  return (kk_string_ptr_eq_borrow(x->_tag, kk_std_core__tag_ExnAssert));
}
static inline bool kk_std_core__is_ExnTodo(kk_std_core__exception_info x) {
  return (kk_string_ptr_eq_borrow(x->_tag, kk_std_core__tag_ExnTodo));
}
static inline bool kk_std_core__is_ExnRange(kk_std_core__exception_info x) {
  return (kk_string_ptr_eq_borrow(x->_tag, kk_std_core__tag_ExnRange));
}
static inline bool kk_std_core__is_ExnSystem(kk_std_core__exception_info x) {
  return (kk_string_ptr_eq_borrow(x->_tag, kk_std_core__tag_ExnSystem));
}
static inline bool kk_std_core__is_ExnInternal(kk_std_core__exception_info x) {
  return (kk_string_ptr_eq_borrow(x->_tag, kk_std_core__tag_ExnInternal));
}
static inline bool kk_std_core__is_ExnPattern(kk_std_core__exception_info x) {
  return (kk_string_ptr_eq_borrow(x->_tag, kk_std_core__tag_ExnPattern));
}
static inline kk_std_core__exception_info kk_std_core__exception_info_dup(kk_std_core__exception_info _x) {
  return kk_basetype_dup_as(kk_std_core__exception_info, _x);
}
static inline void kk_std_core__exception_info_drop(kk_std_core__exception_info _x, kk_context_t* _ctx) {
  kk_basetype_drop(_x, _ctx);
}
static inline kk_std_core__exception_info kk_std_core__exception_info_hole() {
  return (kk_std_core__exception_info)(1);
}
static inline bool kk_std_core__exception_info_is_unique(kk_std_core__exception_info _x) {
  return kk_basetype_is_unique(_x);
}
static inline void kk_std_core__exception_info_free(kk_std_core__exception_info _x, kk_context_t* _ctx) {
  kk_basetype_free(_x, _ctx);
}
static inline void kk_std_core__exception_info_decref(kk_std_core__exception_info _x, kk_context_t* _ctx) {
  kk_basetype_decref(_x, _ctx);
}
static inline kk_reuse_t kk_std_core__exception_info_dropn_reuse(kk_std_core__exception_info _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  return kk_basetype_dropn_reuse(_x, _scan_fsize, _ctx);
}
static inline void kk_std_core__exception_info_dropn(kk_std_core__exception_info _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  kk_basetype_dropn(_x, _scan_fsize, _ctx);
}
static inline kk_reuse_t kk_std_core__exception_info_reuse(kk_std_core__exception_info _x) {
  return kk_basetype_reuse(_x);
}
static inline kk_box_t kk_std_core__exception_info_box(kk_std_core__exception_info _x, kk_context_t* _ctx) {
  return kk_basetype_box(_x);
}
static inline kk_std_core__exception_info kk_std_core__exception_info_unbox(kk_box_t _x, kk_context_t* _ctx) {
  return kk_basetype_unbox_as(kk_std_core__exception_info, _x);
}

// value type std/core/exception
struct kk_std_core_Exception {
  kk_string_t message;
  kk_std_core__exception_info info;
};
typedef struct kk_std_core_Exception kk_std_core__exception;
static inline kk_std_core__exception kk_std_core__new_Exception(kk_string_t message, kk_std_core__exception_info info, kk_context_t* _ctx) {
  kk_std_core__exception _con;
  _con.message = message;
  _con.info = info;
  return _con;
}
static inline bool kk_std_core__is_Exception(kk_std_core__exception x) {
  return (true);
}
static inline kk_std_core__exception kk_std_core__exception_dup(kk_std_core__exception _x) {
  kk_string_dup(_x.message);
  kk_std_core__exception_info_dup(_x.info);
  return _x;
}
static inline void kk_std_core__exception_drop(kk_std_core__exception _x, kk_context_t* _ctx) {
  kk_string_drop(_x.message, _ctx);
  kk_std_core__exception_info_drop(_x.info, _ctx);
}
static inline kk_box_t kk_std_core__exception_box(kk_std_core__exception _x, kk_context_t* _ctx) {
  kk_box_t _box;
  kk_valuetype_box(kk_std_core__exception, _box, _x, 2 /* scan count */, _ctx);
  return _box;
}
static inline kk_std_core__exception kk_std_core__exception_unbox(kk_box_t _x, kk_context_t* _ctx) {
  kk_boxed_value_t _p;
  kk_std_core__exception _unbox;
  kk_valuetype_unbox_(kk_std_core__exception, _p, _unbox, _x, _ctx);
  if (_ctx!=NULL && _p!=NULL) {
    if (kk_basetype_is_unique(_p)) { kk_basetype_free(_p,_ctx); } else {
      kk_std_core__exception_dup(_unbox);
      kk_basetype_decref(_p, _ctx);
    }
  }
  return _unbox;
}

// type std/core/.hnd-exn
struct kk_std_core__hnd_exn_s {
  kk_block_t _block;
};
typedef struct kk_std_core__hnd_exn_s* kk_std_core__hnd_exn;
struct kk_std_core__Hnd_exn {
  struct kk_std_core__hnd_exn_s _base;
  kk_std_core_hnd__clause1 ctl_throw_exn;
};
static inline kk_std_core__hnd_exn kk_std_core__base_Hnd_exn(struct kk_std_core__Hnd_exn* _x) {
  return &_x->_base;
}
static inline kk_std_core__hnd_exn kk_std_core__new_Hnd_exn(kk_reuse_t _at, kk_std_core_hnd__clause1 ctl_throw_exn, kk_context_t* _ctx) {
  struct kk_std_core__Hnd_exn* _con = kk_block_alloc_at_as(struct kk_std_core__Hnd_exn, _at, 1 /* scan count */, (kk_tag_t)(1), _ctx);
  _con->ctl_throw_exn = ctl_throw_exn;
  return kk_std_core__base_Hnd_exn(_con);
}
static inline struct kk_std_core__Hnd_exn* kk_std_core__as_Hnd_exn(kk_std_core__hnd_exn x) {
  return kk_basetype_as_assert(struct kk_std_core__Hnd_exn*, x, (kk_tag_t)(1) /* _tag */);
}
static inline bool kk_std_core__is_Hnd_exn(kk_std_core__hnd_exn x) {
  return (true);
}
static inline kk_std_core__hnd_exn kk_std_core__hnd_exn_dup(kk_std_core__hnd_exn _x) {
  return kk_basetype_dup_as(kk_std_core__hnd_exn, _x);
}
static inline void kk_std_core__hnd_exn_drop(kk_std_core__hnd_exn _x, kk_context_t* _ctx) {
  kk_basetype_drop(_x, _ctx);
}
static inline kk_std_core__hnd_exn kk_std_core__hnd_exn_hole() {
  return (kk_std_core__hnd_exn)(1);
}
static inline bool kk_std_core__hnd_exn_is_unique(kk_std_core__hnd_exn _x) {
  return kk_basetype_is_unique(_x);
}
static inline void kk_std_core__hnd_exn_free(kk_std_core__hnd_exn _x, kk_context_t* _ctx) {
  kk_basetype_free(_x, _ctx);
}
static inline void kk_std_core__hnd_exn_decref(kk_std_core__hnd_exn _x, kk_context_t* _ctx) {
  kk_basetype_decref(_x, _ctx);
}
static inline kk_reuse_t kk_std_core__hnd_exn_dropn_reuse(kk_std_core__hnd_exn _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  return kk_basetype_dropn_reuse(_x, _scan_fsize, _ctx);
}
static inline void kk_std_core__hnd_exn_dropn(kk_std_core__hnd_exn _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  kk_basetype_dropn(_x, _scan_fsize, _ctx);
}
static inline kk_reuse_t kk_std_core__hnd_exn_reuse(kk_std_core__hnd_exn _x) {
  return kk_basetype_reuse(_x);
}
static inline kk_box_t kk_std_core__hnd_exn_box(kk_std_core__hnd_exn _x, kk_context_t* _ctx) {
  return kk_basetype_box(_x);
}
static inline kk_std_core__hnd_exn kk_std_core__hnd_exn_unbox(kk_box_t _x, kk_context_t* _ctx) {
  return kk_basetype_unbox_as(kk_std_core__hnd_exn, _x);
}

// type std/core/blocking
struct kk_std_core__blocking_s {
  kk_block_t _block;
};
typedef kk_datatype_t kk_std_core__blocking;
static inline kk_std_core__blocking kk_std_core__blocking_dup(kk_std_core__blocking _x) {
  return kk_datatype_dup(_x);
}
static inline void kk_std_core__blocking_drop(kk_std_core__blocking _x, kk_context_t* _ctx) {
  kk_datatype_drop(_x, _ctx);
}
static inline kk_std_core__blocking kk_std_core__blocking_hole() {
  return kk_datatype_from_tag((kk_tag_t)0);
}
static inline bool kk_std_core__blocking_is_unique(kk_std_core__blocking _x) {
  return kk_datatype_is_unique(_x);
}
static inline void kk_std_core__blocking_free(kk_std_core__blocking _x, kk_context_t* _ctx) {
  kk_datatype_free(_x, _ctx);
}
static inline void kk_std_core__blocking_decref(kk_std_core__blocking _x, kk_context_t* _ctx) {
  kk_datatype_decref(_x, _ctx);
}
static inline kk_reuse_t kk_std_core__blocking_dropn_reuse(kk_std_core__blocking _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  return kk_datatype_dropn_reuse(_x, _scan_fsize, _ctx);
}
static inline void kk_std_core__blocking_dropn(kk_std_core__blocking _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  kk_datatype_dropn(_x, _scan_fsize, _ctx);
}
static inline kk_reuse_t kk_std_core__blocking_reuse(kk_std_core__blocking _x) {
  return kk_datatype_reuse(_x);
}
static inline kk_box_t kk_std_core__blocking_box(kk_std_core__blocking _x, kk_context_t* _ctx) {
  return kk_datatype_box(_x);
}
static inline kk_std_core__blocking kk_std_core__blocking_unbox(kk_box_t _x, kk_context_t* _ctx) {
  return kk_datatype_unbox(_x);
}

// type std/core/console
struct kk_std_core__console_s {
  kk_block_t _block;
};
typedef kk_datatype_t kk_std_core__console;
static inline kk_std_core__console kk_std_core__console_dup(kk_std_core__console _x) {
  return kk_datatype_dup(_x);
}
static inline void kk_std_core__console_drop(kk_std_core__console _x, kk_context_t* _ctx) {
  kk_datatype_drop(_x, _ctx);
}
static inline kk_std_core__console kk_std_core__console_hole() {
  return kk_datatype_from_tag((kk_tag_t)0);
}
static inline bool kk_std_core__console_is_unique(kk_std_core__console _x) {
  return kk_datatype_is_unique(_x);
}
static inline void kk_std_core__console_free(kk_std_core__console _x, kk_context_t* _ctx) {
  kk_datatype_free(_x, _ctx);
}
static inline void kk_std_core__console_decref(kk_std_core__console _x, kk_context_t* _ctx) {
  kk_datatype_decref(_x, _ctx);
}
static inline kk_reuse_t kk_std_core__console_dropn_reuse(kk_std_core__console _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  return kk_datatype_dropn_reuse(_x, _scan_fsize, _ctx);
}
static inline void kk_std_core__console_dropn(kk_std_core__console _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  kk_datatype_dropn(_x, _scan_fsize, _ctx);
}
static inline kk_reuse_t kk_std_core__console_reuse(kk_std_core__console _x) {
  return kk_datatype_reuse(_x);
}
static inline kk_box_t kk_std_core__console_box(kk_std_core__console _x, kk_context_t* _ctx) {
  return kk_datatype_box(_x);
}
static inline kk_std_core__console kk_std_core__console_unbox(kk_box_t _x, kk_context_t* _ctx) {
  return kk_datatype_unbox(_x);
}

// value type std/core/delayed
struct kk_std_core_Delay {
  kk_ref_t dref;
};
typedef struct kk_std_core_Delay kk_std_core__delayed;
static inline kk_std_core__delayed kk_std_core__new_Delay(kk_ref_t dref, kk_context_t* _ctx) {
  kk_std_core__delayed _con = { dref };
  return _con;
}
static inline bool kk_std_core__is_Delay(kk_std_core__delayed x) {
  return (true);
}
static inline kk_std_core__delayed kk_std_core__delayed_dup(kk_std_core__delayed _x) {
  kk_ref_dup(_x.dref);
  return _x;
}
static inline void kk_std_core__delayed_drop(kk_std_core__delayed _x, kk_context_t* _ctx) {
  kk_ref_drop(_x.dref, _ctx);
}
static inline kk_box_t kk_std_core__delayed_box(kk_std_core__delayed _x, kk_context_t* _ctx) {
  return kk_ref_box(_x.dref, _ctx);
}
static inline kk_std_core__delayed kk_std_core__delayed_unbox(kk_box_t _x, kk_context_t* _ctx) {
  return kk_std_core__new_Delay(kk_ref_unbox(_x, _ctx), _ctx);
}

// value type std/core/error
struct kk_std_core_Error {
  kk_std_core__exception exception;
};
struct kk_std_core_Ok {
  kk_box_t result;
};
struct kk_std_core_error_s {
  kk_value_tag_t _tag;
  union {
    struct kk_std_core_Error Error;
    struct kk_std_core_Ok Ok;
    kk_box_t _fields[2];
  } _cons;
};
typedef struct kk_std_core_error_s kk_std_core__error;
static inline kk_std_core__error kk_std_core__new_Error(kk_std_core__exception exception, kk_context_t* _ctx) {
  kk_std_core__error _con;
  _con._tag = kk_value_tag(1);
  _con._cons.Error.exception = exception;
  return _con;
}
static inline kk_std_core__error kk_std_core__new_Ok(kk_box_t result, kk_context_t* _ctx) {
  kk_std_core__error _con;
  _con._tag = kk_value_tag(2);
  _con._cons.Ok.result = result;
  _con._cons._fields[1] = kk_box_null;
  return _con;
}
static inline bool kk_std_core__is_Error(kk_std_core__error x) {
  return (kk_value_tag_eq(x._tag, kk_value_tag(1)));
}
static inline bool kk_std_core__is_Ok(kk_std_core__error x) {
  return (kk_value_tag_eq(x._tag, kk_value_tag(2)));
}
static inline kk_ssize_t kk_std_core__error_scan_count(kk_std_core__error _x) {
  if (kk_std_core__is_Error(_x)) return 3;
  else return 2;
}
static inline kk_std_core__error kk_std_core__error_dup(kk_std_core__error _x) {
  if (kk_std_core__is_Error(_x)) {
    kk_std_core__exception_dup(_x._cons.Error.exception);
  }
  else {
    kk_box_dup(_x._cons.Ok.result);
  }
  return _x;
}
static inline void kk_std_core__error_drop(kk_std_core__error _x, kk_context_t* _ctx) {
  if (kk_std_core__is_Error(_x)) {
    kk_std_core__exception_drop(_x._cons.Error.exception, _ctx);
  }
  else {
    kk_box_drop(_x._cons.Ok.result, _ctx);
  }
}
static inline kk_box_t kk_std_core__error_box(kk_std_core__error _x, kk_context_t* _ctx) {
  kk_box_t _box;
  kk_valuetype_box(kk_std_core__error, _box, _x, kk_std_core__error_scan_count(_x), _ctx);
  return _box;
}
static inline kk_std_core__error kk_std_core__error_unbox(kk_box_t _x, kk_context_t* _ctx) {
  kk_boxed_value_t _p;
  kk_std_core__error _unbox;
  kk_valuetype_unbox_(kk_std_core__error, _p, _unbox, _x, _ctx);
  if (_ctx!=NULL && _p!=NULL) {
    if (kk_basetype_is_unique(_p)) { kk_basetype_free(_p,_ctx); } else {
      kk_std_core__error_dup(_unbox);
      kk_basetype_decref(_p, _ctx);
    }
  }
  return _unbox;
}

// type std/core/exn
struct kk_std_core__exn_s {
  kk_block_t _block;
};
typedef struct kk_std_core__exn_s* kk_std_core__exn;
struct kk_std_core_Exn {
  struct kk_std_core__exn_s _base;
  kk_std_core__hnd_exn _field1;
};
static inline kk_std_core__exn kk_std_core__base_Exn(struct kk_std_core_Exn* _x) {
  return &_x->_base;
}
static inline kk_std_core__exn kk_std_core__new_Exn(kk_reuse_t _at, kk_std_core__hnd_exn _field1, kk_context_t* _ctx) {
  struct kk_std_core_Exn* _con = kk_block_alloc_at_as(struct kk_std_core_Exn, _at, 1 /* scan count */, (kk_tag_t)(1), _ctx);
  _con->_field1 = _field1;
  return kk_std_core__base_Exn(_con);
}
static inline struct kk_std_core_Exn* kk_std_core__as_Exn(kk_std_core__exn x) {
  return kk_basetype_as_assert(struct kk_std_core_Exn*, x, (kk_tag_t)(1) /* _tag */);
}
static inline bool kk_std_core__is_Exn(kk_std_core__exn x) {
  return (true);
}
static inline kk_std_core__exn kk_std_core__exn_dup(kk_std_core__exn _x) {
  return kk_basetype_dup_as(kk_std_core__exn, _x);
}
static inline void kk_std_core__exn_drop(kk_std_core__exn _x, kk_context_t* _ctx) {
  kk_basetype_drop(_x, _ctx);
}
static inline kk_std_core__exn kk_std_core__exn_hole() {
  return (kk_std_core__exn)(1);
}
static inline bool kk_std_core__exn_is_unique(kk_std_core__exn _x) {
  return kk_basetype_is_unique(_x);
}
static inline void kk_std_core__exn_free(kk_std_core__exn _x, kk_context_t* _ctx) {
  kk_basetype_free(_x, _ctx);
}
static inline void kk_std_core__exn_decref(kk_std_core__exn _x, kk_context_t* _ctx) {
  kk_basetype_decref(_x, _ctx);
}
static inline kk_reuse_t kk_std_core__exn_dropn_reuse(kk_std_core__exn _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  return kk_basetype_dropn_reuse(_x, _scan_fsize, _ctx);
}
static inline void kk_std_core__exn_dropn(kk_std_core__exn _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  kk_basetype_dropn(_x, _scan_fsize, _ctx);
}
static inline kk_reuse_t kk_std_core__exn_reuse(kk_std_core__exn _x) {
  return kk_basetype_reuse(_x);
}
static inline kk_box_t kk_std_core__exn_box(kk_std_core__exn _x, kk_context_t* _ctx) {
  return kk_basetype_box(_x);
}
static inline kk_std_core__exn kk_std_core__exn_unbox(kk_box_t _x, kk_context_t* _ctx) {
  return kk_basetype_unbox_as(kk_std_core__exn, _x);
}

// type std/core/fsys
struct kk_std_core__fsys_s {
  kk_block_t _block;
};
typedef kk_datatype_t kk_std_core__fsys;
static inline kk_std_core__fsys kk_std_core__fsys_dup(kk_std_core__fsys _x) {
  return kk_datatype_dup(_x);
}
static inline void kk_std_core__fsys_drop(kk_std_core__fsys _x, kk_context_t* _ctx) {
  kk_datatype_drop(_x, _ctx);
}
static inline kk_std_core__fsys kk_std_core__fsys_hole() {
  return kk_datatype_from_tag((kk_tag_t)0);
}
static inline bool kk_std_core__fsys_is_unique(kk_std_core__fsys _x) {
  return kk_datatype_is_unique(_x);
}
static inline void kk_std_core__fsys_free(kk_std_core__fsys _x, kk_context_t* _ctx) {
  kk_datatype_free(_x, _ctx);
}
static inline void kk_std_core__fsys_decref(kk_std_core__fsys _x, kk_context_t* _ctx) {
  kk_datatype_decref(_x, _ctx);
}
static inline kk_reuse_t kk_std_core__fsys_dropn_reuse(kk_std_core__fsys _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  return kk_datatype_dropn_reuse(_x, _scan_fsize, _ctx);
}
static inline void kk_std_core__fsys_dropn(kk_std_core__fsys _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  kk_datatype_dropn(_x, _scan_fsize, _ctx);
}
static inline kk_reuse_t kk_std_core__fsys_reuse(kk_std_core__fsys _x) {
  return kk_datatype_reuse(_x);
}
static inline kk_box_t kk_std_core__fsys_box(kk_std_core__fsys _x, kk_context_t* _ctx) {
  return kk_datatype_box(_x);
}
static inline kk_std_core__fsys kk_std_core__fsys_unbox(kk_box_t _x, kk_context_t* _ctx) {
  return kk_datatype_unbox(_x);
}

// type std/core/global-scope
struct kk_std_core__global_scope_s {
  kk_block_t _block;
};
typedef kk_datatype_t kk_std_core__global_scope;
static inline kk_std_core__global_scope kk_std_core__global_scope_dup(kk_std_core__global_scope _x) {
  return kk_datatype_dup(_x);
}
static inline void kk_std_core__global_scope_drop(kk_std_core__global_scope _x, kk_context_t* _ctx) {
  kk_datatype_drop(_x, _ctx);
}
static inline kk_std_core__global_scope kk_std_core__global_scope_hole() {
  return kk_datatype_from_tag((kk_tag_t)0);
}
static inline bool kk_std_core__global_scope_is_unique(kk_std_core__global_scope _x) {
  return kk_datatype_is_unique(_x);
}
static inline void kk_std_core__global_scope_free(kk_std_core__global_scope _x, kk_context_t* _ctx) {
  kk_datatype_free(_x, _ctx);
}
static inline void kk_std_core__global_scope_decref(kk_std_core__global_scope _x, kk_context_t* _ctx) {
  kk_datatype_decref(_x, _ctx);
}
static inline kk_reuse_t kk_std_core__global_scope_dropn_reuse(kk_std_core__global_scope _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  return kk_datatype_dropn_reuse(_x, _scan_fsize, _ctx);
}
static inline void kk_std_core__global_scope_dropn(kk_std_core__global_scope _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  kk_datatype_dropn(_x, _scan_fsize, _ctx);
}
static inline kk_reuse_t kk_std_core__global_scope_reuse(kk_std_core__global_scope _x) {
  return kk_datatype_reuse(_x);
}
static inline kk_box_t kk_std_core__global_scope_box(kk_std_core__global_scope _x, kk_context_t* _ctx) {
  return kk_datatype_box(_x);
}
static inline kk_std_core__global_scope kk_std_core__global_scope_unbox(kk_box_t _x, kk_context_t* _ctx) {
  return kk_datatype_unbox(_x);
}

// type std/core/net
struct kk_std_core__net_s {
  kk_block_t _block;
};
typedef kk_datatype_t kk_std_core__net;
static inline kk_std_core__net kk_std_core__net_dup(kk_std_core__net _x) {
  return kk_datatype_dup(_x);
}
static inline void kk_std_core__net_drop(kk_std_core__net _x, kk_context_t* _ctx) {
  kk_datatype_drop(_x, _ctx);
}
static inline kk_std_core__net kk_std_core__net_hole() {
  return kk_datatype_from_tag((kk_tag_t)0);
}
static inline bool kk_std_core__net_is_unique(kk_std_core__net _x) {
  return kk_datatype_is_unique(_x);
}
static inline void kk_std_core__net_free(kk_std_core__net _x, kk_context_t* _ctx) {
  kk_datatype_free(_x, _ctx);
}
static inline void kk_std_core__net_decref(kk_std_core__net _x, kk_context_t* _ctx) {
  kk_datatype_decref(_x, _ctx);
}
static inline kk_reuse_t kk_std_core__net_dropn_reuse(kk_std_core__net _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  return kk_datatype_dropn_reuse(_x, _scan_fsize, _ctx);
}
static inline void kk_std_core__net_dropn(kk_std_core__net _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  kk_datatype_dropn(_x, _scan_fsize, _ctx);
}
static inline kk_reuse_t kk_std_core__net_reuse(kk_std_core__net _x) {
  return kk_datatype_reuse(_x);
}
static inline kk_box_t kk_std_core__net_box(kk_std_core__net _x, kk_context_t* _ctx) {
  return kk_datatype_box(_x);
}
static inline kk_std_core__net kk_std_core__net_unbox(kk_box_t _x, kk_context_t* _ctx) {
  return kk_datatype_unbox(_x);
}

// type std/core/ui
struct kk_std_core__ui_s {
  kk_block_t _block;
};
typedef kk_datatype_t kk_std_core__ui;
static inline kk_std_core__ui kk_std_core__ui_dup(kk_std_core__ui _x) {
  return kk_datatype_dup(_x);
}
static inline void kk_std_core__ui_drop(kk_std_core__ui _x, kk_context_t* _ctx) {
  kk_datatype_drop(_x, _ctx);
}
static inline kk_std_core__ui kk_std_core__ui_hole() {
  return kk_datatype_from_tag((kk_tag_t)0);
}
static inline bool kk_std_core__ui_is_unique(kk_std_core__ui _x) {
  return kk_datatype_is_unique(_x);
}
static inline void kk_std_core__ui_free(kk_std_core__ui _x, kk_context_t* _ctx) {
  kk_datatype_free(_x, _ctx);
}
static inline void kk_std_core__ui_decref(kk_std_core__ui _x, kk_context_t* _ctx) {
  kk_datatype_decref(_x, _ctx);
}
static inline kk_reuse_t kk_std_core__ui_dropn_reuse(kk_std_core__ui _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  return kk_datatype_dropn_reuse(_x, _scan_fsize, _ctx);
}
static inline void kk_std_core__ui_dropn(kk_std_core__ui _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  kk_datatype_dropn(_x, _scan_fsize, _ctx);
}
static inline kk_reuse_t kk_std_core__ui_reuse(kk_std_core__ui _x) {
  return kk_datatype_reuse(_x);
}
static inline kk_box_t kk_std_core__ui_box(kk_std_core__ui _x, kk_context_t* _ctx) {
  return kk_datatype_box(_x);
}
static inline kk_std_core__ui kk_std_core__ui_unbox(kk_box_t _x, kk_context_t* _ctx) {
  return kk_datatype_unbox(_x);
}

// type std/core/list
struct kk_std_core__list_s {
  kk_block_t _block;
};
typedef kk_datatype_t kk_std_core__list;
struct kk_std_core_Cons {
  struct kk_std_core__list_s _base;
  kk_box_t head;
  kk_std_core__list tail;
};
static inline kk_std_core__list kk_std_core__new_Nil(kk_context_t* _ctx) {
  return kk_datatype_from_tag((kk_tag_t)(1));
}
static inline kk_std_core__list kk_std_core__base_Cons(struct kk_std_core_Cons* _x) {
  return kk_datatype_from_base(&_x->_base);
}
static inline kk_std_core__list kk_std_core__new_Cons(kk_reuse_t _at, kk_box_t head, kk_std_core__list tail, kk_context_t* _ctx) {
  struct kk_std_core_Cons* _con = kk_block_alloc_at_as(struct kk_std_core_Cons, _at, 2 /* scan count */, (kk_tag_t)(2), _ctx);
  _con->head = head;
  _con->tail = tail;
  return kk_std_core__base_Cons(_con);
}
static inline struct kk_std_core_Cons* kk_std_core__as_Cons(kk_std_core__list x) {
  return kk_datatype_as_assert(struct kk_std_core_Cons*, x, (kk_tag_t)(2) /* _tag */);
}
static inline bool kk_std_core__is_Nil(kk_std_core__list x) {
  return (kk_datatype_has_singleton_tag(x, (kk_tag_t)(1)));
}
static inline bool kk_std_core__is_Cons(kk_std_core__list x) {
  return (!kk_std_core__is_Nil(x));
}
static inline kk_std_core__list kk_std_core__list_dup(kk_std_core__list _x) {
  return kk_datatype_dup(_x);
}
static inline void kk_std_core__list_drop(kk_std_core__list _x, kk_context_t* _ctx) {
  kk_datatype_drop(_x, _ctx);
}
static inline kk_std_core__list kk_std_core__list_hole() {
  return kk_datatype_from_tag((kk_tag_t)0);
}
static inline bool kk_std_core__list_is_unique(kk_std_core__list _x) {
  return kk_datatype_is_unique(_x);
}
static inline void kk_std_core__list_free(kk_std_core__list _x, kk_context_t* _ctx) {
  kk_datatype_free(_x, _ctx);
}
static inline void kk_std_core__list_decref(kk_std_core__list _x, kk_context_t* _ctx) {
  kk_datatype_decref(_x, _ctx);
}
static inline kk_reuse_t kk_std_core__list_dropn_reuse(kk_std_core__list _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  return kk_datatype_dropn_reuse(_x, _scan_fsize, _ctx);
}
static inline void kk_std_core__list_dropn(kk_std_core__list _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  kk_datatype_dropn(_x, _scan_fsize, _ctx);
}
static inline kk_reuse_t kk_std_core__list_reuse(kk_std_core__list _x) {
  return kk_datatype_reuse(_x);
}
static inline kk_box_t kk_std_core__list_box(kk_std_core__list _x, kk_context_t* _ctx) {
  return kk_datatype_box(_x);
}
static inline kk_std_core__list kk_std_core__list_unbox(kk_box_t _x, kk_context_t* _ctx) {
  return kk_datatype_unbox(_x);
}

// type std/core/nmd
struct kk_std_core__nmd_s {
  kk_block_t _block;
};
typedef kk_datatype_t kk_std_core__nmd;
static inline kk_std_core__nmd kk_std_core__nmd_dup(kk_std_core__nmd _x) {
  return kk_datatype_dup(_x);
}
static inline void kk_std_core__nmd_drop(kk_std_core__nmd _x, kk_context_t* _ctx) {
  kk_datatype_drop(_x, _ctx);
}
static inline kk_std_core__nmd kk_std_core__nmd_hole() {
  return kk_datatype_from_tag((kk_tag_t)0);
}
static inline bool kk_std_core__nmd_is_unique(kk_std_core__nmd _x) {
  return kk_datatype_is_unique(_x);
}
static inline void kk_std_core__nmd_free(kk_std_core__nmd _x, kk_context_t* _ctx) {
  kk_datatype_free(_x, _ctx);
}
static inline void kk_std_core__nmd_decref(kk_std_core__nmd _x, kk_context_t* _ctx) {
  kk_datatype_decref(_x, _ctx);
}
static inline kk_reuse_t kk_std_core__nmd_dropn_reuse(kk_std_core__nmd _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  return kk_datatype_dropn_reuse(_x, _scan_fsize, _ctx);
}
static inline void kk_std_core__nmd_dropn(kk_std_core__nmd _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  kk_datatype_dropn(_x, _scan_fsize, _ctx);
}
static inline kk_reuse_t kk_std_core__nmd_reuse(kk_std_core__nmd _x) {
  return kk_datatype_reuse(_x);
}
static inline kk_box_t kk_std_core__nmd_box(kk_std_core__nmd _x, kk_context_t* _ctx) {
  return kk_datatype_box(_x);
}
static inline kk_std_core__nmd kk_std_core__nmd_unbox(kk_box_t _x, kk_context_t* _ctx) {
  return kk_datatype_unbox(_x);
}

// type std/core/null
struct kk_std_core__null_s {
  kk_block_t _block;
};
typedef kk_datatype_t kk_std_core__null;
static inline kk_std_core__null kk_std_core__null_dup(kk_std_core__null _x) {
  return kk_datatype_dup(_x);
}
static inline void kk_std_core__null_drop(kk_std_core__null _x, kk_context_t* _ctx) {
  kk_datatype_drop(_x, _ctx);
}
static inline kk_std_core__null kk_std_core__null_hole() {
  return kk_datatype_from_tag((kk_tag_t)0);
}
static inline bool kk_std_core__null_is_unique(kk_std_core__null _x) {
  return kk_datatype_is_unique(_x);
}
static inline void kk_std_core__null_free(kk_std_core__null _x, kk_context_t* _ctx) {
  kk_datatype_free(_x, _ctx);
}
static inline void kk_std_core__null_decref(kk_std_core__null _x, kk_context_t* _ctx) {
  kk_datatype_decref(_x, _ctx);
}
static inline kk_reuse_t kk_std_core__null_dropn_reuse(kk_std_core__null _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  return kk_datatype_dropn_reuse(_x, _scan_fsize, _ctx);
}
static inline void kk_std_core__null_dropn(kk_std_core__null _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  kk_datatype_dropn(_x, _scan_fsize, _ctx);
}
static inline kk_reuse_t kk_std_core__null_reuse(kk_std_core__null _x) {
  return kk_datatype_reuse(_x);
}
static inline kk_box_t kk_std_core__null_box(kk_std_core__null _x, kk_context_t* _ctx) {
  return kk_datatype_box(_x);
}
static inline kk_std_core__null kk_std_core__null_unbox(kk_box_t _x, kk_context_t* _ctx) {
  return kk_datatype_unbox(_x);
}

// type std/core/scope
struct kk_std_core__scope_s {
  kk_block_t _block;
};
typedef kk_datatype_t kk_std_core__scope;
static inline kk_std_core__scope kk_std_core__scope_dup(kk_std_core__scope _x) {
  return kk_datatype_dup(_x);
}
static inline void kk_std_core__scope_drop(kk_std_core__scope _x, kk_context_t* _ctx) {
  kk_datatype_drop(_x, _ctx);
}
static inline kk_std_core__scope kk_std_core__scope_hole() {
  return kk_datatype_from_tag((kk_tag_t)0);
}
static inline bool kk_std_core__scope_is_unique(kk_std_core__scope _x) {
  return kk_datatype_is_unique(_x);
}
static inline void kk_std_core__scope_free(kk_std_core__scope _x, kk_context_t* _ctx) {
  kk_datatype_free(_x, _ctx);
}
static inline void kk_std_core__scope_decref(kk_std_core__scope _x, kk_context_t* _ctx) {
  kk_datatype_decref(_x, _ctx);
}
static inline kk_reuse_t kk_std_core__scope_dropn_reuse(kk_std_core__scope _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  return kk_datatype_dropn_reuse(_x, _scan_fsize, _ctx);
}
static inline void kk_std_core__scope_dropn(kk_std_core__scope _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  kk_datatype_dropn(_x, _scan_fsize, _ctx);
}
static inline kk_reuse_t kk_std_core__scope_reuse(kk_std_core__scope _x) {
  return kk_datatype_reuse(_x);
}
static inline kk_box_t kk_std_core__scope_box(kk_std_core__scope _x, kk_context_t* _ctx) {
  return kk_datatype_box(_x);
}
static inline kk_std_core__scope kk_std_core__scope_unbox(kk_box_t _x, kk_context_t* _ctx) {
  return kk_datatype_unbox(_x);
}

// value type std/core/sslice
struct kk_std_core_Sslice {
  kk_string_t str;
  kk_ssize_t start;
  kk_ssize_t len;
};
typedef struct kk_std_core_Sslice kk_std_core__sslice;
static inline kk_std_core__sslice kk_std_core__new_Sslice(kk_string_t str, kk_ssize_t start, kk_ssize_t len, kk_context_t* _ctx) {
  kk_std_core__sslice _con;
  _con.str = str;
  _con.start = start;
  _con.len = len;
  return _con;
}
static inline bool kk_std_core__is_Sslice(kk_std_core__sslice x) {
  return (true);
}
static inline kk_std_core__sslice kk_std_core__sslice_dup(kk_std_core__sslice _x) {
  kk_string_dup(_x.str);
  return _x;
}
static inline void kk_std_core__sslice_drop(kk_std_core__sslice _x, kk_context_t* _ctx) {
  kk_string_drop(_x.str, _ctx);
}
static inline kk_box_t kk_std_core__sslice_box(kk_std_core__sslice _x, kk_context_t* _ctx) {
  kk_box_t _box;
  kk_valuetype_box(kk_std_core__sslice, _box, _x, 1 /* scan count */, _ctx);
  return _box;
}
static inline kk_std_core__sslice kk_std_core__sslice_unbox(kk_box_t _x, kk_context_t* _ctx) {
  kk_boxed_value_t _p;
  kk_std_core__sslice _unbox;
  kk_valuetype_unbox_(kk_std_core__sslice, _p, _unbox, _x, _ctx);
  if (_ctx!=NULL && _p!=NULL) {
    if (kk_basetype_is_unique(_p)) { kk_basetype_free(_p,_ctx); } else {
      kk_std_core__sslice_dup(_unbox);
      kk_basetype_decref(_p, _ctx);
    }
  }
  return _unbox;
}

// type std/core/stream
struct kk_std_core__stream_s {
  kk_block_t _block;
};
typedef struct kk_std_core__stream_s* kk_std_core__stream;
struct kk_std_core_Next {
  struct kk_std_core__stream_s _base;
  kk_box_t head;
  kk_std_core__stream tail;
};
static inline kk_std_core__stream kk_std_core__base_Next(struct kk_std_core_Next* _x) {
  return &_x->_base;
}
static inline kk_std_core__stream kk_std_core__new_Next(kk_reuse_t _at, kk_box_t head, kk_std_core__stream tail, kk_context_t* _ctx) {
  struct kk_std_core_Next* _con = kk_block_alloc_at_as(struct kk_std_core_Next, _at, 2 /* scan count */, (kk_tag_t)(1), _ctx);
  _con->head = head;
  _con->tail = tail;
  return kk_std_core__base_Next(_con);
}
static inline struct kk_std_core_Next* kk_std_core__as_Next(kk_std_core__stream x) {
  return kk_basetype_as_assert(struct kk_std_core_Next*, x, (kk_tag_t)(1) /* _tag */);
}
static inline bool kk_std_core__is_Next(kk_std_core__stream x) {
  return (true);
}
static inline kk_std_core__stream kk_std_core__stream_dup(kk_std_core__stream _x) {
  return kk_basetype_dup_as(kk_std_core__stream, _x);
}
static inline void kk_std_core__stream_drop(kk_std_core__stream _x, kk_context_t* _ctx) {
  kk_basetype_drop(_x, _ctx);
}
static inline kk_std_core__stream kk_std_core__stream_hole() {
  return (kk_std_core__stream)(1);
}
static inline bool kk_std_core__stream_is_unique(kk_std_core__stream _x) {
  return kk_basetype_is_unique(_x);
}
static inline void kk_std_core__stream_free(kk_std_core__stream _x, kk_context_t* _ctx) {
  kk_basetype_free(_x, _ctx);
}
static inline void kk_std_core__stream_decref(kk_std_core__stream _x, kk_context_t* _ctx) {
  kk_basetype_decref(_x, _ctx);
}
static inline kk_reuse_t kk_std_core__stream_dropn_reuse(kk_std_core__stream _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  return kk_basetype_dropn_reuse(_x, _scan_fsize, _ctx);
}
static inline void kk_std_core__stream_dropn(kk_std_core__stream _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  kk_basetype_dropn(_x, _scan_fsize, _ctx);
}
static inline kk_reuse_t kk_std_core__stream_reuse(kk_std_core__stream _x) {
  return kk_basetype_reuse(_x);
}
static inline kk_box_t kk_std_core__stream_box(kk_std_core__stream _x, kk_context_t* _ctx) {
  return kk_basetype_box(_x);
}
static inline kk_std_core__stream kk_std_core__stream_unbox(kk_box_t _x, kk_context_t* _ctx) {
  return kk_basetype_unbox_as(kk_std_core__stream, _x);
}

// value declarations
 
// Automatically generated. Tests for the `ExnError` constructor of the `:exception-info` type.

static inline bool kk_std_core_is_exnError(kk_std_core__exception_info kkloc_exception_info, kk_context_t* _ctx) { /* (exception-info : exception-info) -> bool */ 
  if (kk_std_core__is_ExnError(kkloc_exception_info)) {
    struct kk_std_core_ExnError* _con19983 = kk_std_core__as_ExnError(kkloc_exception_info);
    return true;
  }
  {
    return false;
  }
}
 
// Automatically generated. Tests for the `ExnAssert` constructor of the `:exception-info` type.

static inline bool kk_std_core_is_exnAssert(kk_std_core__exception_info kkloc_exception_info, kk_context_t* _ctx) { /* (exception-info : exception-info) -> bool */ 
  if (kk_std_core__is_ExnAssert(kkloc_exception_info)) {
    struct kk_std_core_ExnAssert* _con19984 = kk_std_core__as_ExnAssert(kkloc_exception_info);
    return true;
  }
  {
    return false;
  }
}
 
// Automatically generated. Tests for the `ExnTodo` constructor of the `:exception-info` type.

static inline bool kk_std_core_is_exnTodo(kk_std_core__exception_info kkloc_exception_info, kk_context_t* _ctx) { /* (exception-info : exception-info) -> bool */ 
  if (kk_std_core__is_ExnTodo(kkloc_exception_info)) {
    struct kk_std_core_ExnTodo* _con19985 = kk_std_core__as_ExnTodo(kkloc_exception_info);
    return true;
  }
  {
    return false;
  }
}
 
// Automatically generated. Tests for the `ExnRange` constructor of the `:exception-info` type.

static inline bool kk_std_core_is_exnRange(kk_std_core__exception_info kkloc_exception_info, kk_context_t* _ctx) { /* (exception-info : exception-info) -> bool */ 
  if (kk_std_core__is_ExnRange(kkloc_exception_info)) {
    struct kk_std_core_ExnRange* _con19986 = kk_std_core__as_ExnRange(kkloc_exception_info);
    return true;
  }
  {
    return false;
  }
}
 
// Automatically generated. Tests for the `ExnPattern` constructor of the `:exception-info` type.

static inline bool kk_std_core_is_exnPattern(kk_std_core__exception_info kkloc_exception_info, kk_context_t* _ctx) { /* (exception-info : exception-info) -> bool */ 
  if (kk_std_core__is_ExnPattern(kkloc_exception_info)) {
    struct kk_std_core_ExnPattern* _con19987 = kk_std_core__as_ExnPattern(kkloc_exception_info);
    return true;
  }
  {
    return false;
  }
}
 
// Automatically generated. Tests for the `ExnSystem` constructor of the `:exception-info` type.

static inline bool kk_std_core_is_exnSystem(kk_std_core__exception_info kkloc_exception_info, kk_context_t* _ctx) { /* (exception-info : exception-info) -> bool */ 
  if (kk_std_core__is_ExnSystem(kkloc_exception_info)) {
    struct kk_std_core_ExnSystem* _con19988 = kk_std_core__as_ExnSystem(kkloc_exception_info);
    return true;
  }
  {
    return false;
  }
}
 
// Automatically generated. Tests for the `ExnInternal` constructor of the `:exception-info` type.

static inline bool kk_std_core_is_exnInternal(kk_std_core__exception_info kkloc_exception_info, kk_context_t* _ctx) { /* (exception-info : exception-info) -> bool */ 
  if (kk_std_core__is_ExnInternal(kkloc_exception_info)) {
    struct kk_std_core_ExnInternal* _con19989 = kk_std_core__as_ExnInternal(kkloc_exception_info);
    return true;
  }
  {
    return false;
  }
}

extern kk_string_t kk_std_core__tag_ExnError;

extern kk_string_t kk_std_core__tag_ExnAssert;

extern kk_string_t kk_std_core__tag_ExnTodo;

extern kk_string_t kk_std_core__tag_ExnRange;

extern kk_string_t kk_std_core__tag_ExnPattern;

extern kk_string_t kk_std_core__tag_ExnSystem;

extern kk_string_t kk_std_core__tag_ExnInternal;
 
// Automatically generated. Retrieves the `message` constructor field of the `:exception` type.

static inline kk_string_t kk_std_core_message(kk_std_core__exception exception, kk_context_t* _ctx) { /* (exception : exception) -> string */ 
  {
    kk_string_t _x = exception.message;
    return kk_string_dup(_x);
  }
}
 
// Automatically generated. Retrieves the `info` constructor field of the `:exception` type.

static inline kk_std_core__exception_info kk_std_core_info(kk_std_core__exception exception, kk_context_t* _ctx) { /* (exception : exception) -> exception-info */ 
  {
    kk_std_core__exception_info _x = exception.info;
    return kk_std_core__exception_info_dup(_x);
  }
}

kk_std_core__exception kk_std_core__copy(kk_std_core__exception _this, kk_std_core_types__optional message0, kk_std_core_types__optional info0, kk_context_t* _ctx); /* (exception, message : optional<string>, info : optional<exception-info>) -> exception */ 
 
// Automatically generated. Retrieves the `dref` constructor field of the `:delayed` type.

static inline kk_ref_t kk_std_core_dref(kk_std_core__delayed delayed, kk_context_t* _ctx) { /* forall<e,a> (delayed : delayed<e,a>) -> ref<global,either<() -> e a,a>> */ 
  {
    kk_ref_t _x = delayed.dref;
    return kk_ref_dup(_x);
  }
}

static inline kk_std_core__delayed kk_std_core__copy_1(kk_std_core__delayed _this, kk_std_core_types__optional dref0, kk_context_t* _ctx) { /* forall<e,a> (delayed<e,a>, dref : optional<ref<global,either<() -> e a,a>>>) -> delayed<e,a> */ 
  kk_ref_t _x19994;
  if (kk_std_core_types__is_Optional(dref0)) {
    kk_box_t _box_x18181 = dref0._cons.Optional.value;
    kk_ref_t _dref_1727 = kk_ref_unbox(_box_x18181, NULL);
    kk_std_core__delayed_drop(_this, _ctx);
    _x19994 = _dref_1727; /*ref<global,either<() -> 1739 1740,1740>>*/
  }
  else {
    kk_ref_t _x = _this.dref;
    _x19994 = _x; /*ref<global,either<() -> 1739 1740,1740>>*/
  }
  return kk_std_core__new_Delay(_x19994, _ctx);
}
 
// Automatically generated. Tests for the `Error` constructor of the `:error` type.

static inline bool kk_std_core_is_error(kk_std_core__error error, kk_context_t* _ctx) { /* forall<a> (error : error<a>) -> bool */ 
  if (kk_std_core__is_Error(error)) {
    kk_std_core__exception _pat0 = error._cons.Error.exception;
    return true;
  }
  {
    return false;
  }
}
 
// Automatically generated. Tests for the `Ok` constructor of the `:error` type.

static inline bool kk_std_core_is_ok(kk_std_core__error error, kk_context_t* _ctx) { /* forall<a> (error : error<a>) -> bool */ 
  if (kk_std_core__is_Ok(error)) {
    return true;
  }
  {
    return false;
  }
}
 
// Automatically generated. Tests for the `Nil` constructor of the `:list` type.

static inline bool kk_std_core_is_nil(kk_std_core__list list0, kk_context_t* _ctx) { /* forall<a> (list : list<a>) -> bool */ 
  if (kk_std_core__is_Nil(list0)) {
    return true;
  }
  {
    return false;
  }
}
 
// Automatically generated. Tests for the `Cons` constructor of the `:list` type.

static inline bool kk_std_core_is_cons(kk_std_core__list list0, kk_context_t* _ctx) { /* forall<a> (list : list<a>) -> bool */ 
  if (kk_std_core__is_Cons(list0)) {
    struct kk_std_core_Cons* _con19996 = kk_std_core__as_Cons(list0);
    return true;
  }
  {
    return false;
  }
}
 
// Automatically generated. Retrieves the `str` constructor field of the `:sslice` type.

static inline kk_string_t kk_std_core_str(kk_std_core__sslice sslice, kk_context_t* _ctx) { /* (sslice : sslice) -> string */ 
  {
    kk_string_t _x = sslice.str;
    return kk_string_dup(_x);
  }
}
 
// Automatically generated. Retrieves the `start` constructor field of the `:sslice` type.

static inline kk_ssize_t kk_std_core_start(kk_std_core__sslice sslice, kk_context_t* _ctx) { /* (sslice : sslice) -> ssize_t */ 
  {
    kk_ssize_t _x = sslice.start;
    return _x;
  }
}
 
// Automatically generated. Retrieves the `len` constructor field of the `:sslice` type.

static inline kk_ssize_t kk_std_core_len(kk_std_core__sslice sslice, kk_context_t* _ctx) { /* (sslice : sslice) -> ssize_t */ 
  {
    kk_ssize_t _x = sslice.len;
    return _x;
  }
}

kk_std_core__sslice kk_std_core__copy_2(kk_std_core__sslice _this, kk_std_core_types__optional str0, kk_std_core_types__optional start0, kk_std_core_types__optional len0, kk_context_t* _ctx); /* (sslice, str : optional<string>, start : optional<ssize_t>, len : optional<ssize_t>) -> sslice */ 
 
// Automatically generated. Retrieves the `head` constructor field of the `:stream` type.

static inline kk_box_t kk_std_core_head(kk_std_core__stream stream, kk_context_t* _ctx) { /* forall<a> (stream : stream<a>) -> a */ 
  {
    struct kk_std_core_Next* _con20003 = kk_std_core__as_Next(stream);
    kk_box_t _x = _con20003->head;
    return kk_box_dup(_x);
  }
}
 
// Automatically generated. Retrieves the `tail` constructor field of the `:stream` type.

static inline kk_std_core__stream kk_std_core_tail(kk_std_core__stream stream, kk_context_t* _ctx) { /* forall<a> (stream : stream<a>) -> stream<a> */ 
  {
    struct kk_std_core_Next* _con20004 = kk_std_core__as_Next(stream);
    kk_std_core__stream _x = _con20004->tail;
    return kk_std_core__stream_dup(_x);
  }
}

kk_std_core__stream kk_std_core__copy_3(kk_std_core__stream _this, kk_std_core_types__optional head0, kk_std_core_types__optional tail0, kk_context_t* _ctx); /* forall<a> (stream<a>, head : optional<a>, tail : optional<stream<a>>) -> stream<a> */ 
 
// select `throw-exn` operation out of the `:exn` effect handler

static inline kk_std_core_hnd__clause1 kk_std_core__select_throw_exn(kk_std_core__hnd_exn hnd, kk_context_t* _ctx) { /* forall<a,e,b> (hnd : .hnd-exn<e,b>) -> std/core/hnd/clause1<exception,a,.hnd-exn,e,b> */ 
  {
    struct kk_std_core__Hnd_exn* _con20010 = kk_std_core__as_Hnd_exn(hnd);
    kk_std_core_hnd__clause1 ctl_throw_exn = _con20010->ctl_throw_exn;
    return kk_std_core_hnd__clause1_dup(ctl_throw_exn);
  }
}

extern kk_std_core_hnd__htag kk_std_core__tag_exn;

kk_box_t kk_std_core__handle_exn(int32_t cfc, kk_std_core__hnd_exn hnd, kk_function_t ret, kk_function_t action, kk_context_t* _ctx); /* forall<a,e,b> (cfc : int32, hnd : .hnd-exn<e,b>, ret : (res : a) -> e b, action : () -> <exn|e> a) -> e b */ 
 
// Internal export for the regex module

static inline kk_std_core__sslice kk_std_core__new_sslice(kk_string_t str0, kk_ssize_t start0, kk_ssize_t len0, kk_context_t* _ctx) { /* (str : string, start : ssize_t, len : ssize_t) -> sslice */ 
  return kk_std_core__new_Sslice(str0, start0, len0, _ctx);
}

kk_std_core__null kk_std_core__null_any(kk_box_t x, kk_context_t* _ctx); /* forall<a> (x : a) -> null<a> */ 

static inline kk_integer_t kk_std_core_int_8(bool b, kk_context_t* _ctx) { /* (b : bool) -> int */ 
  if (b) {
    return kk_integer_from_small(1);
  }
  {
    return kk_integer_from_small(0);
  }
}

static inline kk_integer_t kk_std_core_int_9(kk_std_core_types__order x, kk_context_t* _ctx) { /* (x : order) -> int */ 
  if (kk_std_core_types__is_Lt(x)) {
    return kk_integer_from_small(-1);
  }
  if (kk_std_core_types__is_Eq(x)) {
    return kk_integer_from_small(0);
  }
  {
    return kk_integer_from_small(1);
  }
}

extern kk_ref_t kk_std_core_redirect;

kk_unit_t kk_std_core_xprintsln(kk_string_t s, kk_context_t* _ctx); /* (s : string) -> console () */ 

kk_integer_t kk_std_core_string_compare(kk_string_t x, kk_string_t y, kk_context_t* _ctx); /* (x : string, y : string) -> int */ 

int32_t kk_std_core_int32(kk_integer_t i, kk_context_t* _ctx); /* (i : int) -> int32 */ 

kk_ssize_t kk_std_core_ssize__t(kk_integer_t i, kk_context_t* _ctx); /* (i : int) -> ssize_t */ 

kk_string_t kk_std_core_string(kk_char_t c, kk_context_t* _ctx); /* (c : char) -> string */ 

kk_string_t kk_std_core_string_1(kk_vector_t _arg1, kk_context_t* _ctx); /* (vector<char>) -> string */ 

kk_string_t kk_std_core_string_2(kk_std_core__list cs, kk_context_t* _ctx); /* (cs : list<char>) -> total string */ 

kk_string_t kk_std_core_string_3(kk_std_core__sslice slice0, kk_context_t* _ctx); /* (slice : sslice) -> string */ 
 
// Convert a `:maybe` string to a string using the empty sting for `Nothing`

static inline kk_string_t kk_std_core_string_4(kk_std_core_types__maybe ms, kk_context_t* _ctx) { /* (ms : maybe<string>) -> string */ 
  if (kk_std_core_types__is_Nothing(ms)) {
    return kk_string_empty();
  }
  {
    kk_box_t _box_x18198 = ms._cons.Just.value;
    kk_string_t s = kk_string_unbox(_box_x18198);
    return s;
  }
}

kk_std_core__list kk_std_core_vlist(kk_vector_t v, kk_std_core_types__optional tail0, kk_context_t* _ctx); /* forall<a> (v : vector<a>, tail : optional<list<a>>) -> list<a> */ 

kk_string_t kk_std_core_int_show_hex(kk_integer_t i, bool use_capitals, kk_context_t* _ctx); /* (i : int, use-capitals : bool) -> string */ 

kk_string_t kk_std_core_repeatz(kk_string_t s, kk_ssize_t n, kk_context_t* _ctx); /* (s : string, n : ssize_t) -> string */ 

kk_string_t kk_std_core_show_expx(double d, int32_t prec, kk_context_t* _ctx); /* (d : float64, prec : int32) -> string */ 

kk_string_t kk_std_core_show_fixedx(double d, int32_t prec, kk_context_t* _ctx); /* (d : float64, prec : int32) -> string */ 

kk_unit_t kk_std_core_xprints(kk_string_t s, kk_context_t* _ctx); /* (s : string) -> console () */ 

kk_integer_t kk_std_core_pow(kk_integer_t i, kk_integer_t exp, kk_context_t* _ctx); /* (i : int, exp : int) -> int */ 

kk_std_core__sslice kk_std_core_advance(kk_std_core__sslice slice0, kk_integer_t count, kk_context_t* _ctx); /* (slice : sslice, count : int) -> sslice */ 
 
// Apply a function `f` to a specified argument `x`.

static inline kk_box_t kk_std_core_apply(kk_function_t f, kk_box_t x, kk_context_t* _ctx) { /* forall<a,b,e> (f : (a) -> e b, x : a) -> e b */ 
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, x, _ctx));
}

kk_unit_t kk_std_core_unsafe_assert_fail(kk_string_t msg, kk_context_t* _ctx); /* (msg : string) -> () */ 

kk_std_core__sslice kk_std_core_extend(kk_std_core__sslice slice0, kk_integer_t count, kk_context_t* _ctx); /* (slice : sslice, count : int) -> sslice */ 

kk_std_core__sslice kk_std_core_first1(kk_string_t s, kk_context_t* _ctx); /* (s : string) -> sslice */ 

kk_string_t kk_std_core_to_upper(kk_string_t s, kk_context_t* _ctx); /* (s : string) -> string */ 

kk_integer_t kk_std_core_cdiv_exp10(kk_integer_t i, kk_integer_t n, kk_context_t* _ctx); /* (i : int, n : int) -> int */ 

kk_integer_t kk_std_core_mul_exp10(kk_integer_t i, kk_integer_t n, kk_context_t* _ctx); /* (i : int, n : int) -> int */ 

kk_std_core__sslice kk_std_core_common_prefix(kk_string_t s, kk_string_t t, kk_std_core_types__optional upto, kk_context_t* _ctx); /* (s : string, t : string, upto : optional<int>) -> sslice */ 

kk_std_core__list kk_std_core__ctail_lift17183_concat(kk_std_core__list ys, kk_std_core__list zss, kk_std_core_types__ctail _acc, kk_context_t* _ctx); /* forall<a> (ys : list<a>, zss : list<list<a>>, ctail<list<a>>) -> list<a> */ 

kk_std_core__list kk_std_core__lift17183_concat(kk_std_core__list ys0, kk_std_core__list zss0, kk_context_t* _ctx); /* forall<a> (ys : list<a>, zss : list<list<a>>) -> list<a> */ 
 
// Concatenate all lists in a list (e.g. flatten the list). (tail-recursive)

static inline kk_std_core__list kk_std_core_concat(kk_std_core__list xss, kk_context_t* _ctx) { /* forall<a> (xss : list<list<a>>) -> list<a> */ 
  return kk_std_core__lift17183_concat(kk_std_core__new_Nil(_ctx), xss, _ctx);
}
 
// The `const` funs returns its first argument and ignores the second.

static inline kk_box_t kk_std_core_const(kk_box_t x, kk_box_t y, kk_context_t* _ctx) { /* forall<a,b> (x : a, y : b) -> a */ 
  kk_box_drop(y, _ctx);
  return x;
}
 
// Return a 'constant' function that ignores its argument and always returns the same result


// lift anonymous function
struct kk_std_core_const_fun20025__t_1 {
  struct kk_function_s _base;
  kk_box_t default0;
};
extern kk_box_t kk_std_core_const_fun20025_1(kk_function_t _fself, kk_box_t ___wildcard__110__6, kk_context_t* _ctx);
static inline kk_function_t kk_std_core_new_const_fun20025_1(kk_box_t default0, kk_context_t* _ctx) {
  struct kk_std_core_const_fun20025__t_1* _self = kk_function_alloc_as(struct kk_std_core_const_fun20025__t_1, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_const_fun20025_1, kk_context());
  _self->default0 = default0;
  return &_self->_base;
}


static inline kk_function_t kk_std_core_const_1(kk_box_t default0, kk_context_t* _ctx) { /* forall<a,b> (default : a) -> total ((x : b) -> a) */ 
  return kk_std_core_new_const_fun20025_1(default0, _ctx);
}

kk_std_core_types__maybe kk_std_core_next(kk_std_core__sslice slice0, kk_context_t* _ctx); /* (slice : sslice) -> maybe<(char, sslice)> */ 

kk_integer_t kk_std_core_count_digits(kk_integer_t i, kk_context_t* _ctx); /* (i : int) -> int */ 
 
// Convert a `:maybe<a>` value to `:a`, using the `nothing` parameter for `Nothing`.

static inline kk_box_t kk_std_core_default(kk_std_core_types__maybe m, kk_box_t nothing, kk_context_t* _ctx) { /* forall<a> (m : maybe<a>, nothing : a) -> a */ 
  if (kk_std_core_types__is_Nothing(m)) {
    return nothing;
  }
  {
    kk_box_t x = m._cons.Just.value;
    kk_box_drop(nothing, _ctx);
    return x;
  }
}
 
// Use default value `def` in case of an error.

static inline kk_box_t kk_std_core_default_1(kk_std_core__error t, kk_box_t def, kk_context_t* _ctx) { /* forall<a> (t : error<a>, def : a) -> a */ 
  if (kk_std_core__is_Error(t)) {
    kk_std_core__exception _pat0 = t._cons.Error.exception;
    kk_std_core__error_drop(t, _ctx);
    return def;
  }
  {
    kk_box_t x = t._cons.Ok.result;
    kk_box_drop(def, _ctx);
    return x;
  }
}

kk_std_core_types__either kk_std_core_either(kk_std_core__error t, kk_context_t* _ctx); /* forall<a> (t : error<a>) -> either<exception,a> */ 

bool kk_std_core_xends_with(kk_string_t s, kk_string_t post, kk_context_t* _ctx); /* (s : string, post : string) -> bool */ 

kk_std_core__list kk_std_core__mlift17667_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_std_core__list zz, kk_std_core__list ys1_17210, kk_context_t* _ctx); /* forall<a,b,e> (ctail<list<b>>, f : (a) -> e list<b>, zz : list<a>, ys1.17210 : list<b>) -> e list<b> */ 

kk_std_core__list kk_std_core__mlift17668_op(kk_function_t _accm, kk_function_t f0, kk_std_core__list zz0, kk_std_core__list ys1_172100, kk_context_t* _ctx); /* forall<a,b,e> ((list<b>) -> list<b>, f : (a) -> e list<b>, zz : list<a>, ys1.17210 : list<b>) -> e list<b> */ 

kk_std_core__list kk_std_core__ctail_lift17184_flatmap(kk_function_t f1, kk_std_core__list ys, kk_std_core__list zs, kk_std_core_types__ctail _acc0, kk_context_t* _ctx); /* forall<a,b,e> (f : (a) -> e list<b>, ys : list<b>, zs : list<a>, ctail<list<b>>) -> e list<b> */ 

kk_std_core__list kk_std_core__ctailm_lift17184_flatmap(kk_function_t f2, kk_std_core__list ys0, kk_std_core__list zs0, kk_function_t _accm0, kk_context_t* _ctx); /* forall<a,b,e> (f : (a) -> e list<b>, ys : list<b>, zs : list<a>, (list<b>) -> list<b>) -> e list<b> */ 

kk_std_core__list kk_std_core__lift17184_flatmap(kk_function_t f3, kk_std_core__list ys1, kk_std_core__list zs1, kk_context_t* _ctx); /* forall<a,b,e> (f : (a) -> e list<b>, ys : list<b>, zs : list<a>) -> e list<b> */ 
 
// Concatenate the result lists from applying a function to all elements.

static inline kk_std_core__list kk_std_core_flatmap(kk_std_core__list xs, kk_function_t f, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, f : (a) -> e list<b>) -> e list<b> */ 
  return kk_std_core__lift17184_flatmap(f, kk_std_core__new_Nil(_ctx), xs, _ctx);
}

kk_std_core__list kk_std_core__lift17185_reverse_append(kk_std_core__list acc, kk_std_core__list ys, kk_context_t* _ctx); /* forall<a> (acc : list<a>, ys : list<a>) -> list<a> */ 
 
// Efficiently reverse a list `xs` and append it to `tl`:
// `reverse-append(xs,tl) == reserve(xs) ++ tl

static inline kk_std_core__list kk_std_core_reverse_append(kk_std_core__list xs, kk_std_core__list tl, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, tl : list<a>) -> list<a> */ 
  return kk_std_core__lift17185_reverse_append(tl, xs, _ctx);
}
 
// monadic lift

static inline kk_box_t kk_std_core__mlift17669_force(kk_ref_t r, kk_box_t x0, kk_context_t* _ctx) { /* forall<a,e> (r : ref<global,either<() -> e a,a>>, x0 : a) -> <st<global>,div|e> a */ 
  kk_unit_t __ = kk_Unit;
  kk_unit_t _brw_19886 = kk_Unit;
  kk_box_t _x20047;
  kk_std_core_types__either _x20048;
  kk_box_t _x20049 = kk_box_dup(x0); /*2592*/
  _x20048 = kk_std_core_types__new_Right(_x20049, _ctx); /*either<1024,1025>*/
  _x20047 = kk_std_core_types__either_box(_x20048, _ctx); /*1000*/
  kk_ref_set_borrow(r,_x20047,kk_context());
  kk_ref_drop(r, _ctx);
  _brw_19886;
  return x0;
}

kk_box_t kk_std_core__mlift17670_force(kk_ref_t r, kk_function_t _y_17346, kk_context_t* _ctx); /* forall<a,e> (r : ref<global,either<() -> e a,a>>, () -> <st<global>,div|e> a) -> <alloc<global>,div,read<global>,write<global>|e> a */ 

kk_box_t kk_std_core__mlift17671_force(kk_ref_t r, kk_std_core_types__either _y_17344, kk_context_t* _ctx); /* forall<a,e> (r : ref<global,either<() -> e a,a>>, either<() -> e a,a>) -> <read<global>,div,alloc<global>,write<global>|e> a */ 

kk_box_t kk_std_core_force(kk_std_core__delayed delayed, kk_context_t* _ctx); /* forall<a,e> (delayed : delayed<e,a>) -> e a */ 

kk_string_t kk_std_core_gshow(kk_box_t _arg1, kk_context_t* _ctx); /* forall<a> (a) -> string */ 

kk_string_t kk_std_core_host(kk_context_t* _ctx); /* () -> ndet string */ 

int64_t kk_std_core_int64(kk_integer_t i, kk_context_t* _ctx); /* (i : int) -> int64 */ 
 
// The `ignore` function ignores its argument.

static inline kk_unit_t kk_std_core_ignore(kk_box_t x, kk_context_t* _ctx) { /* forall<a> (x : a) -> () */ 
  kk_box_drop(x, _ctx);
  kk_Unit; return kk_Unit;
}

int16_t kk_std_core_int16(kk_integer_t i, kk_context_t* _ctx); /* (i : int) -> int16 */ 

int8_t kk_std_core_int8(kk_integer_t i, kk_context_t* _ctx); /* (i : int) -> int8 */ 

kk_std_core__list kk_std_core__ctail_lift17186_intersperse(kk_box_t sep, kk_std_core__list ys, kk_std_core_types__ctail _acc, kk_context_t* _ctx); /* forall<a> (sep : a, ys : list<a>, ctail<list<a>>) -> list<a> */ 

kk_std_core__list kk_std_core__lift17186_intersperse(kk_box_t sep0, kk_std_core__list ys0, kk_context_t* _ctx); /* forall<a> (sep : a, ys : list<a>) -> list<a> */ 

kk_std_core__list kk_std_core_intersperse(kk_std_core__list xs, kk_box_t sep, kk_context_t* _ctx); /* forall<a> (xs : list<a>, sep : a) -> list<a> */ 

intptr_t kk_std_core_intptr__t(kk_integer_t i, kk_context_t* _ctx); /* (i : int) -> intptr_t */ 
 
// Is this an even integer?

static inline bool kk_std_core_is_even(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> bool */ 
  bool b_16946 = kk_integer_is_odd(i,kk_context()); /*bool*/;
  if (b_16946) {
    return false;
  }
  {
    return true;
  }
}

kk_integer_t kk_std_core_is_exp10(kk_integer_t i, kk_context_t* _ctx); /* (i : int) -> int */ 

kk_std_core__sslice kk_std_core_last1(kk_string_t s, kk_context_t* _ctx); /* (s : string) -> sslice */ 

kk_box_t kk_std_core_main_console(kk_function_t main, kk_context_t* _ctx); /* forall<a,e> (main : () -> e a) -> e a */ 

kk_std_core__list kk_std_core__mlift17672_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_std_core__list yy, kk_box_t _ctail_17244, kk_context_t* _ctx); /* forall<a,b,e> (ctail<list<b>>, f : (value : a, rest : list<a>) -> e b, yy : list<a>, b) -> e list<b> */ 

kk_std_core__list kk_std_core__mlift17673_op(kk_function_t _accm, kk_function_t f0, kk_std_core__list yy0, kk_box_t _ctail_17249, kk_context_t* _ctx); /* forall<a,b,e> ((list<b>) -> list<b>, f : (value : a, rest : list<a>) -> e b, yy : list<a>, b) -> e list<b> */ 

kk_std_core__list kk_std_core__ctail_lift17187_map_peek(kk_function_t f1, kk_std_core__list ys, kk_std_core_types__ctail _acc0, kk_context_t* _ctx); /* forall<a,b,e> (f : (value : a, rest : list<a>) -> e b, ys : list<a>, ctail<list<b>>) -> e list<b> */ 

kk_std_core__list kk_std_core__ctailm_lift17187_map_peek(kk_function_t f2, kk_std_core__list ys0, kk_function_t _accm0, kk_context_t* _ctx); /* forall<a,b,e> (f : (value : a, rest : list<a>) -> e b, ys : list<a>, (list<b>) -> list<b>) -> e list<b> */ 

kk_std_core__list kk_std_core__lift17187_map_peek(kk_function_t f3, kk_std_core__list ys1, kk_context_t* _ctx); /* forall<a,b,e> (f : (value : a, rest : list<a>) -> e b, ys : list<a>) -> e list<b> */ 
 
// Apply a function `f`  to each element of the input list in sequence where `f` takes
// both the current element and the tail list as arguments.

static inline kk_std_core__list kk_std_core_map_peek(kk_std_core__list xs, kk_function_t f, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, f : (value : a, rest : list<a>) -> e b) -> e list<b> */ 
  return kk_std_core__lift17187_map_peek(f, xs, _ctx);
}

static inline kk_integer_t kk_std_core_mbint(kk_std_core_types__maybe m, kk_context_t* _ctx) { /* (m : maybe<int>) -> int */ 
  if (kk_std_core_types__is_Nothing(m)) {
    return kk_integer_from_small(0);
  }
  {
    kk_box_t _box_x18319 = m._cons.Just.value;
    kk_integer_t i = kk_integer_unbox(_box_x18319);
    return i;
  }
}

static inline kk_integer_t kk_std_core_negate(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> int */ 
  return kk_integer_neg(i,kk_context());
}

extern kk_ref_t kk_std_core_trace_enabled;

kk_function_t kk_std_core_o(kk_function_t f, kk_function_t g, kk_context_t* _ctx); /* forall<a,b,c,e> (f : (a) -> e b, g : (c) -> e a) -> ((x : c) -> e b) */ 
 
// Set a `hndler` that is always called when the `action` finishes (either normally or with an exception).

static inline kk_box_t kk_std_core_on_exit(kk_function_t hndler, kk_function_t action, kk_context_t* _ctx) { /* forall<a,e> (hndler : () -> e (), action : () -> e a) -> e a */ 
  kk_box_t _x20096 = kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), action, (action, _ctx)); /*2834*/
  return kk_std_core_hnd_finally_prompt(hndler, _x20096, _ctx);
}

kk_box_t kk_std_core__mlift17674_once(kk_function_t calc, kk_ref_t r, kk_std_core_types__maybe _y_17363, kk_context_t* _ctx); /* forall<_h,_e,a> (calc : () -> a, r : ref<_h,maybe<a>>, maybe<a>) -> <read<_h>,write<_h>,div|_e> a */ 

kk_function_t kk_std_core_once(kk_function_t calc, kk_context_t* _ctx); /* forall<a> (calc : () -> a) -> (() -> a) */ 

kk_std_core_types__maybe kk_std_core_xparse_int(kk_string_t s, bool hex, kk_context_t* _ctx); /* (s : string, hex : bool) -> maybe<int> */ 

kk_box_t kk_std_core_phantom(kk_context_t* _ctx); /* forall<a> () -> a */ 
 
// Returns a singleton list.

static inline kk_std_core__list kk_std_core_single(kk_box_t x, kk_context_t* _ctx) { /* forall<a> (x : a) -> list<a> */ 
  return kk_std_core__new_Cons(kk_reuse_null, x, kk_std_core__new_Nil(_ctx), _ctx);
}

kk_string_t kk_std_core_to_lower(kk_string_t s, kk_context_t* _ctx); /* (s : string) -> string */ 

kk_unit_t kk_std_core_xtrace(kk_string_t message0, kk_context_t* _ctx); /* (message : string) -> () */ 

kk_unit_t kk_std_core_xtrace_any(kk_string_t message0, kk_box_t x, kk_context_t* _ctx); /* forall<a> (message : string, x : a) -> () */ 

int8_t kk_std_core_uint8(kk_integer_t i, kk_context_t* _ctx); /* (i : int) -> int8 */ 

extern kk_ref_t kk_std_core_unique_count;
 
// _Unsafe_. This function removes the exception effect (`:exn`) from the effect of an action

static inline kk_box_t kk_std_core_unsafe_no_exn(kk_function_t action, kk_context_t* _ctx) { /* forall<a,e> (action : () -> <exn|e> a) -> e a */ 
  return kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), action, (action, _ctx));
}

kk_vector_t kk_std_core_unvlist(kk_std_core__list xs, kk_context_t* _ctx); /* forall<a> (xs : list<a>) -> vector<a> */ 

kk_vector_t kk_std_core_vector_initz(kk_ssize_t n, kk_function_t f, kk_context_t* _ctx); /* forall<a> (n : ssize_t, f : (ssize_t) -> a) -> vector<a> */ 

bool kk_std_core__lp__excl__eq__4_rp_(kk_std_core_types__order x, kk_std_core_types__order y, kk_context_t* _ctx); /* (x : order, y : order) -> bool */ 

static inline bool kk_std_core__lp__excl__eq__5_rp_(bool x, bool y, kk_context_t* _ctx) { /* (x : bool, y : bool) -> bool */ 
  if (x) {
    if (y) {
      return false;
    }
    {
      return true;
    }
  }
  {
    return y;
  }
}
 
// Add two integers.

static inline kk_integer_t kk_std_core__lp__plus__4_rp_(kk_integer_t x, kk_integer_t y, kk_context_t* _ctx) { /* (x : int, y : int) -> int */ 
  return kk_integer_add(x,y,kk_context());
}
 
// Add two character code points

static inline kk_char_t kk_std_core__lp__plus__3_rp_(kk_char_t c, kk_char_t d, kk_context_t* _ctx) { /* (c : char, d : char) -> total char */ 
  kk_integer_t x_16954 = kk_integer_from_int(c,kk_context()); /*int*/;
  kk_integer_t y_16955 = kk_integer_from_int(d,kk_context()); /*int*/;
  kk_integer_t _x20111 = kk_integer_add(x_16954,y_16955,kk_context()); /*int*/
  return kk_integer_clamp32(_x20111,kk_context());
}

kk_std_core__list kk_std_core__ctail_append(kk_std_core__list xs, kk_std_core__list ys, kk_std_core_types__ctail _acc, kk_context_t* _ctx); /* forall<a> (xs : list<a>, ys : list<a>, ctail<list<a>>) -> list<a> */ 

kk_std_core__list kk_std_core_append(kk_std_core__list xs0, kk_std_core__list ys0, kk_context_t* _ctx); /* forall<a> (xs : list<a>, ys : list<a>) -> list<a> */ 
 
// Append two lists.

static inline kk_std_core__list kk_std_core__lp__plus__plus__rp_(kk_std_core__list xs, kk_std_core__list ys, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, ys : list<a>) -> list<a> */ 
  return kk_std_core_append(xs, ys, _ctx);
}

kk_string_t kk_std_core__lp__plus__plus__1_rp_(kk_string_t x, kk_string_t y, kk_context_t* _ctx); /* (x : string, y : string) -> string */ 
 
// Substract two integers.

static inline kk_integer_t kk_std_core__lp__dash__4_rp_(kk_integer_t x, kk_integer_t y, kk_context_t* _ctx) { /* (x : int, y : int) -> int */ 
  return kk_integer_sub(x,y,kk_context());
}
 
// Substract two character codePoints

static inline kk_char_t kk_std_core__lp__dash__3_rp_(kk_char_t c, kk_char_t d, kk_context_t* _ctx) { /* (c : char, d : char) -> total char */ 
  kk_integer_t x_16956 = kk_integer_from_int(c,kk_context()); /*int*/;
  kk_integer_t y_16957 = kk_integer_from_int(d,kk_context()); /*int*/;
  kk_integer_t _x20116 = kk_integer_sub(x_16956,y_16957,kk_context()); /*int*/
  return kk_integer_clamp32(_x20116,kk_context());
}

kk_unit_t kk_std_core_printsln(kk_string_t s, kk_context_t* _ctx); /* (s : string) -> console () */ 

bool kk_std_core__lp__eq__eq__4_rp_(kk_std_core_types__order x, kk_std_core_types__order y, kk_context_t* _ctx); /* (x : order, y : order) -> bool */ 

static inline bool kk_std_core__lp__eq__eq__5_rp_(bool x, bool y, kk_context_t* _ctx) { /* (x : bool, y : bool) -> bool */ 
  if (x) {
    return y;
  }
  if (y) {
    return false;
  }
  {
    return true;
  }
}

bool kk_std_core__lp__lt__5_rp_(kk_std_core_types__order x, kk_std_core_types__order y, kk_context_t* _ctx); /* (x : order, y : order) -> bool */ 

static inline bool kk_std_core__lp__lt__6_rp_(bool x, bool y, kk_context_t* _ctx) { /* (x : bool, y : bool) -> bool */ 
  if (x) {
    return false;
  }
  {
    return y;
  }
}

kk_std_core_types__order kk_std_core_order(kk_integer_t i, kk_context_t* _ctx); /* (i : int) -> order */ 

kk_std_core_types__order kk_std_core_compare_3(kk_string_t x, kk_string_t y, kk_context_t* _ctx); /* (x : string, y : string) -> order */ 

static inline bool kk_std_core__lp__lt__7_rp_(kk_string_t x, kk_string_t y, kk_context_t* _ctx) { /* (x : string, y : string) -> bool */ 
  kk_std_core_types__order x0_16965 = kk_std_core_compare_3(x, y, _ctx); /*order*/;
  kk_integer_t _brw_19857;
  if (kk_std_core_types__is_Lt(x0_16965)) {
    _brw_19857 = kk_integer_from_small(-1); /*int*/
    goto _match20127;
  }
  if (kk_std_core_types__is_Eq(x0_16965)) {
    _brw_19857 = kk_integer_from_small(0); /*int*/
    goto _match20127;
  }
  {
    _brw_19857 = kk_integer_from_small(1); /*int*/
  }
  _match20127: ;
  bool _brw_19858 = kk_integer_eq_borrow(_brw_19857,(kk_integer_from_small(-1)),kk_context()); /*bool*/;
  kk_integer_drop(_brw_19857, _ctx);
  return _brw_19858;
}

bool kk_std_core__lp__gt__3_rp_(kk_std_core_types__order x, kk_std_core_types__order y, kk_context_t* _ctx); /* (x : order, y : order) -> bool */ 

static inline bool kk_std_core__lp__gt__4_rp_(bool x, bool y, kk_context_t* _ctx) { /* (x : bool, y : bool) -> bool */ 
  if (x) {
    if (y) {
      return false;
    }
    {
      return true;
    }
  }
  {
    return false;
  }
}

static inline bool kk_std_core__lp__gt__5_rp_(kk_string_t x, kk_string_t y, kk_context_t* _ctx) { /* (x : string, y : string) -> bool */ 
  kk_std_core_types__order x0_16972 = kk_std_core_compare_3(x, y, _ctx); /*order*/;
  kk_integer_t _brw_19852;
  if (kk_std_core_types__is_Lt(x0_16972)) {
    _brw_19852 = kk_integer_from_small(-1); /*int*/
    goto _match20130;
  }
  if (kk_std_core_types__is_Eq(x0_16972)) {
    _brw_19852 = kk_integer_from_small(0); /*int*/
    goto _match20130;
  }
  {
    _brw_19852 = kk_integer_from_small(1); /*int*/
  }
  _match20130: ;
  bool _brw_19853 = kk_integer_eq_borrow(_brw_19852,(kk_integer_from_small(1)),kk_context()); /*bool*/;
  kk_integer_drop(_brw_19852, _ctx);
  return _brw_19853;
}

static inline kk_std_core_types__order kk_std_core_compare_1(kk_char_t x, kk_char_t y, kk_context_t* _ctx) { /* (x : char, y : char) -> order */ 
  bool _match_19850 = (x < y); /*bool*/;
  if (_match_19850) {
    return kk_std_core_types__new_Lt(_ctx);
  }
  {
    bool _match_19851 = (x > y); /*bool*/;
    if (_match_19851) {
      return kk_std_core_types__new_Gt(_ctx);
    }
    {
      return kk_std_core_types__new_Eq(_ctx);
    }
  }
}

kk_std_core_types__order kk_std_core_compare_2(bool x, bool y, kk_context_t* _ctx); /* (x : bool, y : bool) -> order */ 

bool kk_std_core__lp__gt__eq__4_rp_(kk_std_core_types__order x, kk_std_core_types__order y, kk_context_t* _ctx); /* (x : order, y : order) -> bool */ 

static inline bool kk_std_core__lp__gt__eq__5_rp_(bool x, bool y, kk_context_t* _ctx) { /* (x : bool, y : bool) -> bool */ 
  if (x) {
    return true;
  }
  if (y) {
    return false;
  }
  {
    return true;
  }
}

static inline bool kk_std_core__lp__gt__eq__6_rp_(kk_string_t x, kk_string_t y, kk_context_t* _ctx) { /* (x : string, y : string) -> bool */ 
  kk_std_core_types__order x0_16988 = kk_std_core_compare_3(x, y, _ctx); /*order*/;
  kk_integer_t _brw_19845;
  if (kk_std_core_types__is_Lt(x0_16988)) {
    _brw_19845 = kk_integer_from_small(-1); /*int*/
    goto _match20133;
  }
  if (kk_std_core_types__is_Eq(x0_16988)) {
    _brw_19845 = kk_integer_from_small(0); /*int*/
    goto _match20133;
  }
  {
    _brw_19845 = kk_integer_from_small(1); /*int*/
  }
  _match20133: ;
  bool _brw_19846 = kk_integer_gt_borrow(_brw_19845,(kk_integer_from_small(-1)),kk_context()); /*bool*/;
  kk_integer_drop(_brw_19845, _ctx);
  return _brw_19846;
}

kk_string_t kk_std_core__lift17188_joinsep(kk_string_t sep, kk_std_core__list ys, kk_string_t acc, kk_context_t* _ctx); /* (sep : string, ys : list<string>, acc : string) -> string */ 

kk_string_t kk_std_core_joinsep(kk_std_core__list xs, kk_string_t sep, kk_context_t* _ctx); /* (xs : list<string>, sep : string) -> string */ 

kk_string_t kk_std_core__lift17189_join_2(kk_std_core__list ys, kk_string_t acc, kk_context_t* _ctx); /* (ys : list<string>, acc : string) -> string */ 

kk_string_t kk_std_core_join_2(kk_std_core__list xs, kk_context_t* _ctx); /* (xs : list<string>) -> string */ 

kk_string_t kk_std_core_join_3(kk_std_core__list xs, kk_string_t sep, kk_context_t* _ctx); /* (xs : list<string>, sep : string) -> string */ 

bool kk_std_core__lp__lt__eq__5_rp_(kk_std_core_types__order x, kk_std_core_types__order y, kk_context_t* _ctx); /* (x : order, y : order) -> bool */ 

static inline bool kk_std_core__lp__lt__eq__6_rp_(bool x, bool y, kk_context_t* _ctx) { /* (x : bool, y : bool) -> bool */ 
  if (x) {
    if (y) {
      return true;
    }
    {
      return false;
    }
  }
  {
    return true;
  }
}

static inline bool kk_std_core__lp__lt__eq__7_rp_(kk_string_t x, kk_string_t y, kk_context_t* _ctx) { /* (x : string, y : string) -> bool */ 
  kk_std_core_types__order x0_17000 = kk_std_core_compare_3(x, y, _ctx); /*order*/;
  kk_integer_t _brw_19840;
  if (kk_std_core_types__is_Lt(x0_17000)) {
    _brw_19840 = kk_integer_from_small(-1); /*int*/
    goto _match20155;
  }
  if (kk_std_core_types__is_Eq(x0_17000)) {
    _brw_19840 = kk_integer_from_small(0); /*int*/
    goto _match20155;
  }
  {
    _brw_19840 = kk_integer_from_small(1); /*int*/
  }
  _match20155: ;
  bool _brw_19841 = kk_integer_lt_borrow(_brw_19840,(kk_integer_from_small(1)),kk_context()); /*bool*/;
  kk_integer_drop(_brw_19840, _ctx);
  return _brw_19841;
}

static inline kk_integer_t kk_std_core_inc(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> int */ 
  return kk_integer_add_small_const(i, 1, _ctx);
}

static inline int32_t kk_std_core_decr(int32_t i, kk_context_t* _ctx) { /* (i : int32) -> int32 */ 
  return (i - ((KK_I32(1))));
}

kk_ssize_t kk_std_core_decr_1(kk_ssize_t i, kk_context_t* _ctx); /* (i : ssize_t) -> ssize_t */ 

static inline int32_t kk_std_core_incr(int32_t i, kk_context_t* _ctx) { /* (i : int32) -> int32 */ 
  return (i + ((KK_I32(1))));
}

kk_ssize_t kk_std_core_incr_1(kk_ssize_t i, kk_context_t* _ctx); /* (i : ssize_t) -> ssize_t */ 

kk_unit_t kk_std_core__mlift17675_op(kk_function_t action, kk_ssize_t end, kk_ssize_t i, kk_unit_t wild__, kk_context_t* _ctx); /* forall<e> (action : (ssize_t) -> e (), end : ssize_t, i : ssize_t, wild_ : ()) -> e () */ 

kk_unit_t kk_std_core__lift17190_forz(kk_function_t action0, kk_ssize_t end0, kk_ssize_t i0, kk_context_t* _ctx); /* forall<e> (action : (ssize_t) -> e (), end : ssize_t, i : ssize_t) -> e () */ 
 
// Executes `action`  for each integer between `start`  upto `end`  (including both `start`  and `end` ).
// If `start > end`  the function returns without any call to `action` .

static inline kk_unit_t kk_std_core_forz(kk_ssize_t start0, kk_ssize_t end, kk_function_t action, kk_context_t* _ctx) { /* forall<e> (start : ssize_t, end : ssize_t, action : (ssize_t) -> e ()) -> e () */ 
  kk_std_core__lift17190_forz(action, end, start0, _ctx); return kk_Unit;
}

kk_unit_t kk_std_core_foreach_indexedz(kk_vector_t v, kk_function_t f, kk_context_t* _ctx); /* forall<a,e> (v : vector<a>, f : (ssize_t, a) -> e ()) -> e () */ 

kk_integer_t kk_std_core__lift17191_length_1(kk_std_core__list ys, kk_integer_t acc, kk_context_t* _ctx); /* forall<a> (ys : list<a>, acc : int) -> int */ 
 
// Returns the length of a list.

static inline kk_integer_t kk_std_core_length_1(kk_std_core__list xs, kk_context_t* _ctx) { /* forall<a> (xs : list<a>) -> int */ 
  return kk_std_core__lift17191_length_1(xs, kk_integer_from_small(0), _ctx);
}
 
// Return the length of a vector.

static inline kk_integer_t kk_std_core_length_2(kk_vector_t v, kk_context_t* _ctx) { /* forall<a> (v : vector<a>) -> int */ 
  kk_ssize_t _x20165 = kk_vector_len_borrow(v); /*ssize_t*/
  return kk_integer_from_ssize_t(_x20165,kk_context());
}

kk_std_core__list kk_std_core__ctail_list(kk_integer_t lo, kk_integer_t hi, kk_std_core_types__ctail _acc, kk_context_t* _ctx); /* (lo : int, hi : int, ctail<list<int>>) -> total list<int> */ 

kk_std_core__list kk_std_core_list(kk_integer_t lo0, kk_integer_t hi0, kk_context_t* _ctx); /* (lo : int, hi : int) -> total list<int> */ 

kk_std_core__list kk_std_core__ctail_list_1(kk_integer_t lo, kk_integer_t hi, kk_integer_t stride, kk_std_core_types__ctail _acc, kk_context_t* _ctx); /* (lo : int, hi : int, stride : int, ctail<list<int>>) -> total list<int> */ 

kk_std_core__list kk_std_core_list_1(kk_integer_t lo0, kk_integer_t hi0, kk_integer_t stride0, kk_context_t* _ctx); /* (lo : int, hi : int, stride : int) -> total list<int> */ 

kk_std_core__list kk_std_core__mlift17676_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_integer_t hi, kk_integer_t lo, kk_box_t _ctail_17256, kk_context_t* _ctx); /* forall<a,e> (ctail<list<a>>, f : (int) -> e a, hi : int, lo : int, a) -> e list<a> */ 

kk_std_core__list kk_std_core__mlift17677_op(kk_function_t _accm, kk_function_t f0, kk_integer_t hi0, kk_integer_t lo0, kk_box_t _ctail_17261, kk_context_t* _ctx); /* forall<a,e> ((list<a>) -> list<a>, f : (int) -> e a, hi : int, lo : int, a) -> e list<a> */ 

kk_std_core__list kk_std_core__ctail_list_2(kk_integer_t lo1, kk_integer_t hi1, kk_function_t f1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx); /* forall<a,e> (lo : int, hi : int, f : (int) -> e a, ctail<list<a>>) -> e list<a> */ 

kk_std_core__list kk_std_core__ctailm_list_2(kk_integer_t lo2, kk_integer_t hi2, kk_function_t f2, kk_function_t _accm0, kk_context_t* _ctx); /* forall<a,e> (lo : int, hi : int, f : (int) -> e a, (list<a>) -> list<a>) -> e list<a> */ 

kk_std_core__list kk_std_core_list_2(kk_integer_t lo3, kk_integer_t hi3, kk_function_t f3, kk_context_t* _ctx); /* forall<a,e> (lo : int, hi : int, f : (int) -> e a) -> e list<a> */ 

kk_std_core__list kk_std_core__mlift17678_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_integer_t hi, kk_integer_t lo, kk_integer_t stride, kk_box_t _ctail_17262, kk_context_t* _ctx); /* forall<a,e> (ctail<list<a>>, f : (int) -> e a, hi : int, lo : int, stride : int, a) -> e list<a> */ 

kk_std_core__list kk_std_core__mlift17679_op(kk_function_t _accm, kk_function_t f0, kk_integer_t hi0, kk_integer_t lo0, kk_integer_t stride0, kk_box_t _ctail_17267, kk_context_t* _ctx); /* forall<a,e> ((list<a>) -> list<a>, f : (int) -> e a, hi : int, lo : int, stride : int, a) -> e list<a> */ 

kk_std_core__list kk_std_core__ctail_list_3(kk_integer_t lo1, kk_integer_t hi1, kk_integer_t stride1, kk_function_t f1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx); /* forall<a,e> (lo : int, hi : int, stride : int, f : (int) -> e a, ctail<list<a>>) -> e list<a> */ 

kk_std_core__list kk_std_core__ctailm_list_3(kk_integer_t lo2, kk_integer_t hi2, kk_integer_t stride2, kk_function_t f2, kk_function_t _accm0, kk_context_t* _ctx); /* forall<a,e> (lo : int, hi : int, stride : int, f : (int) -> e a, (list<a>) -> list<a>) -> e list<a> */ 

kk_std_core__list kk_std_core_list_3(kk_integer_t lo3, kk_integer_t hi3, kk_integer_t stride3, kk_function_t f3, kk_context_t* _ctx); /* forall<a,e> (lo : int, hi : int, stride : int, f : (int) -> e a) -> e list<a> */ 

kk_std_core__list kk_std_core__mlift17680_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_std_core__list xx, kk_box_t _ctail_17268, kk_context_t* _ctx); /* forall<a,b,e> (ctail<list<b>>, f : (a) -> e b, xx : list<a>, b) -> e list<b> */ 

kk_std_core__list kk_std_core__mlift17681_op(kk_function_t _accm, kk_function_t f0, kk_std_core__list xx0, kk_box_t _ctail_17273, kk_context_t* _ctx); /* forall<a,b,e> ((list<b>) -> list<b>, f : (a) -> e b, xx : list<a>, b) -> e list<b> */ 

kk_std_core__list kk_std_core__ctail_map_5(kk_std_core__list xs, kk_function_t f1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx); /* forall<a,b,e> (xs : list<a>, f : (a) -> e b, ctail<list<b>>) -> e list<b> */ 

kk_std_core__list kk_std_core__ctailm_map_5(kk_std_core__list xs0, kk_function_t f2, kk_function_t _accm0, kk_context_t* _ctx); /* forall<a,b,e> (xs : list<a>, f : (a) -> e b, (list<b>) -> list<b>) -> e list<b> */ 

kk_std_core__list kk_std_core_map_5(kk_std_core__list xs1, kk_function_t f3, kk_context_t* _ctx); /* forall<a,b,e> (xs : list<a>, f : (a) -> e b) -> e list<b> */ 

kk_std_core__list kk_std_core_list_4(kk_char_t lo, kk_char_t hi, kk_context_t* _ctx); /* (lo : char, hi : char) -> total list<char> */ 
 
// Convert a `:maybe` type to a list type.

static inline kk_std_core__list kk_std_core_list_5(kk_std_core_types__maybe m, kk_context_t* _ctx) { /* forall<a> (m : maybe<a>) -> list<a> */ 
  if (kk_std_core_types__is_Nothing(m)) {
    return kk_std_core__new_Nil(_ctx);
  }
  {
    kk_box_t x = m._cons.Just.value;
    return kk_std_core__new_Cons(kk_reuse_null, x, kk_std_core__new_Nil(_ctx), _ctx);
  }
}

kk_std_core__list kk_std_core_list_6(kk_string_t s, kk_context_t* _ctx); /* (s : string) -> total list<char> */ 
 
// Convert a vector to a list.

static inline kk_std_core__list kk_std_core_list_7(kk_vector_t v, kk_context_t* _ctx) { /* forall<a> (v : vector<a>) -> list<a> */ 
  return kk_std_core_vlist(v, kk_std_core_types__new_None(_ctx), _ctx);
}
 
// monadic lift

static inline kk_std_core_types__maybe kk_std_core__mlift17682_map(kk_box_t _y_17398, kk_context_t* _ctx) { /* forall<a,e> (a) -> e maybe<a> */ 
  return kk_std_core_types__new_Just(_y_17398, _ctx);
}

kk_std_core_types__maybe kk_std_core_map(kk_std_core_types__maybe m, kk_function_t f, kk_context_t* _ctx); /* forall<a,b,e> (m : maybe<a>, f : (a) -> e b) -> e maybe<b> */ 
 
// monadic lift

static inline kk_std_core_types__either kk_std_core__mlift17683_map_1(kk_box_t _y_17400, kk_context_t* _ctx) { /* forall<a,b,e> (b) -> e either<a,b> */ 
  return kk_std_core_types__new_Right(_y_17400, _ctx);
}

kk_std_core_types__either kk_std_core_map_1(kk_std_core_types__either e, kk_function_t f, kk_context_t* _ctx); /* forall<a,b,c,e> (e : either<a,b>, f : (b) -> e c) -> e either<a,c> */ 
 
// monadic lift

static inline kk_std_core_types__tuple2_ kk_std_core__mlift17684_map_2(kk_box_t _y_17402, kk_box_t _y_17403, kk_context_t* _ctx) { /* forall<a,e> (a, a) -> e (a, a) */ 
  return kk_std_core_types__new_dash__lp__comma__rp_(_y_17402, _y_17403, _ctx);
}

kk_std_core_types__tuple2_ kk_std_core__mlift17685_map_2(kk_function_t f, kk_std_core_types__tuple2_ t, kk_box_t _y_17402, kk_context_t* _ctx); /* forall<a,b,e> (f : (a) -> e b, t : (a, a), b) -> e (b, b) */ 

kk_std_core_types__tuple2_ kk_std_core_map_2(kk_std_core_types__tuple2_ t, kk_function_t f, kk_context_t* _ctx); /* forall<a,b,e> (t : (a, a), f : (a) -> e b) -> e (b, b) */ 
 
// monadic lift

static inline kk_std_core_types__tuple3_ kk_std_core__mlift17686_map_3(kk_box_t _y_17404, kk_box_t _y_17405, kk_box_t _y_17406, kk_context_t* _ctx) { /* forall<a,e> (a, a, a) -> e (a, a, a) */ 
  return kk_std_core_types__new_dash__lp__comma__comma__rp_(_y_17404, _y_17405, _y_17406, _ctx);
}

kk_std_core_types__tuple3_ kk_std_core__mlift17687_map_3(kk_box_t _y_17404, kk_function_t f, kk_std_core_types__tuple3_ t, kk_box_t _y_17405, kk_context_t* _ctx); /* forall<a,b,e> (b, f : (a) -> e b, t : (a, a, a), b) -> e (b, b, b) */ 

kk_std_core_types__tuple3_ kk_std_core__mlift17688_map_3(kk_function_t f, kk_std_core_types__tuple3_ t, kk_box_t _y_17404, kk_context_t* _ctx); /* forall<a,b,e> (f : (a) -> e b, t : (a, a, a), b) -> e (b, b, b) */ 

kk_std_core_types__tuple3_ kk_std_core_map_3(kk_std_core_types__tuple3_ t, kk_function_t f, kk_context_t* _ctx); /* forall<a,b,e> (t : (a, a, a), f : (a) -> e b) -> e (b, b, b) */ 
 
// monadic lift

static inline kk_std_core_types__tuple4_ kk_std_core__mlift17689_map_4(kk_box_t _y_17407, kk_box_t _y_17408, kk_box_t _y_17409, kk_box_t _y_17410, kk_context_t* _ctx) { /* forall<a,e> (a, a, a, a) -> e (a, a, a, a) */ 
  return kk_std_core_types__new_dash__lp__comma__comma__comma__rp_(kk_reuse_null, _y_17407, _y_17408, _y_17409, _y_17410, _ctx);
}

kk_std_core_types__tuple4_ kk_std_core__mlift17690_map_4(kk_box_t _y_17407, kk_box_t _y_17408, kk_function_t f, kk_std_core_types__tuple4_ t, kk_box_t _y_17409, kk_context_t* _ctx); /* forall<a,b,e> (b, b, f : (a) -> e b, t : (a, a, a, a), b) -> e (b, b, b, b) */ 

kk_std_core_types__tuple4_ kk_std_core__mlift17691_map_4(kk_box_t _y_17407, kk_function_t f, kk_std_core_types__tuple4_ t, kk_box_t _y_17408, kk_context_t* _ctx); /* forall<a,b,e> (b, f : (a) -> e b, t : (a, a, a, a), b) -> e (b, b, b, b) */ 

kk_std_core_types__tuple4_ kk_std_core__mlift17692_map_4(kk_function_t f, kk_std_core_types__tuple4_ t, kk_box_t _y_17407, kk_context_t* _ctx); /* forall<a,b,e> (f : (a) -> e b, t : (a, a, a, a), b) -> e (b, b, b, b) */ 

kk_std_core_types__tuple4_ kk_std_core_map_4(kk_std_core_types__tuple4_ t, kk_function_t f, kk_context_t* _ctx); /* forall<a,b,e> (t : (a, a, a, a), f : (a) -> e b) -> e (b, b, b, b) */ 

kk_string_t kk_std_core_map_6(kk_string_t s, kk_function_t f, kk_context_t* _ctx); /* forall<e> (s : string, f : (char) -> e char) -> e string */ 
 
// monadic lift

static inline kk_unit_t kk_std_core__mlift17693_map_7(kk_ssize_t i, kk_vector_t w, kk_box_t _y_17412, kk_context_t* _ctx) { /* forall<a,e> (i : ssize_t, w : vector<a>, a) -> e () */ 
  kk_vector_unsafe_assign(w,i,_y_17412,kk_context()); return kk_Unit;
}
 
// monadic lift

static inline kk_vector_t kk_std_core__mlift17694_map_7(kk_vector_t w, kk_unit_t wild__, kk_context_t* _ctx) { /* forall<a,e> (w : vector<a>, wild_ : ()) -> e vector<a> */ 
  return w;
}

kk_vector_t kk_std_core_map_7(kk_vector_t v, kk_function_t f, kk_context_t* _ctx); /* forall<a,b,e> (v : vector<a>, f : (a) -> e b) -> e vector<b> */ 

kk_string_t kk_std_core_pad_left(kk_string_t s, kk_integer_t width, kk_std_core_types__optional fill, kk_context_t* _ctx); /* (s : string, width : int, fill : optional<char>) -> string */ 

kk_string_t kk_std_core_show_hex(kk_integer_t i, kk_std_core_types__optional width, kk_std_core_types__optional use_capitals, kk_std_core_types__optional pre, kk_context_t* _ctx); /* (i : int, width : optional<int>, use-capitals : optional<bool>, pre : optional<string>) -> string */ 

static inline kk_std_core_types__order kk_std_core_sign_1(double d, kk_context_t* _ctx) { /* (d : float64) -> order */ 
  bool _match_19794 = (d < (0.0)); /*bool*/;
  if (_match_19794) {
    return kk_std_core_types__new_Lt(_ctx);
  }
  {
    bool _match_19795 = (d > (0.0)); /*bool*/;
    if (_match_19795) {
      return kk_std_core_types__new_Gt(_ctx);
    }
    {
      return kk_std_core_types__new_Eq(_ctx);
    }
  }
}

bool kk_std_core_is_pos_2(kk_integer_t i, kk_context_t* _ctx); /* (i : int) -> bool */ 
 
// Is the value positive?

static inline bool kk_std_core_is_pos_3(double d, kk_context_t* _ctx) { /* (d : float64) -> bool */ 
  return (d > (0.0));
}
 
// Is the list empty?

static inline bool kk_std_core_is_empty(kk_std_core__list xs, kk_context_t* _ctx) { /* forall<a> (xs : list<a>) -> bool */ 
  if (kk_std_core__is_Nil(xs)) {
    return true;
  }
  {
    kk_std_core__list_drop(xs, _ctx);
    return false;
  }
}
 
// Is a slice empty?

static inline bool kk_std_core_is_empty_1(kk_std_core__sslice slice0, kk_context_t* _ctx) { /* (slice : sslice) -> bool */ 
  bool b_17019;
  kk_ssize_t _x20377;
  {
    kk_ssize_t _x = slice0.len;
    kk_std_core__sslice_drop(slice0, _ctx);
    _x20377 = _x; /*ssize_t*/
  }
  b_17019 = (_x20377 > 0); /*bool*/
  if (b_17019) {
    return false;
  }
  {
    return true;
  }
}
 
// Is a string empty?

static inline bool kk_std_core_is_empty_2(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> bool */ 
  kk_string_t _x20378 = kk_string_empty(); /*string*/
  return kk_string_is_eq(s,_x20378,kk_context());
}

static inline kk_std_core_types__maybe kk_std_core__lp__bar__bar__rp_(kk_std_core_types__maybe m1, kk_std_core_types__maybe m2, kk_context_t* _ctx) { /* forall<a> (m1 : maybe<a>, m2 : maybe<a>) -> maybe<a> */ 
  if (kk_std_core_types__is_Nothing(m1)) {
    return m2;
  }
  {
    kk_std_core_types__maybe_drop(m2, _ctx);
    return m1;
  }
}
 
// Choose a non-empty string

static inline kk_string_t kk_std_core__lp__bar__bar__1_rp_(kk_string_t x, kk_string_t y, kk_context_t* _ctx) { /* (x : string, y : string) -> string */ 
  bool _match_19790;
  kk_string_t _x20380 = kk_string_dup(x); /*string*/
  kk_string_t _x20381 = kk_string_empty(); /*string*/
  _match_19790 = kk_string_is_eq(_x20380,_x20381,kk_context()); /*bool*/
  if (_match_19790) {
    kk_string_drop(x, _ctx);
    return y;
  }
  {
    kk_string_drop(y, _ctx);
    return x;
  }
}

kk_string_t kk_std_core_show_char(kk_char_t c, kk_context_t* _ctx); /* (c : char) -> string */ 
 
// Show a `:float64` in exponential (scientific) notation.
// The optional `precision` (= `-17`) specifies the precision.
// If `>=0` it specifies the number of digits behind the dot (up to `17` max).
// If negative, then at most the absolute value of `precision` digits behind the dot are used.

static inline kk_string_t kk_std_core_show_exp(double d, kk_std_core_types__optional precision, kk_context_t* _ctx) { /* (d : float64, precision : optional<int>) -> string */ 
  int32_t _x20428;
  kk_integer_t _x20429;
  if (kk_std_core_types__is_Optional(precision)) {
    kk_box_t _box_x18640 = precision._cons.Optional.value;
    kk_integer_t _precision_8116 = kk_integer_unbox(_box_x18640);
    _x20429 = _precision_8116; /*int*/
  }
  else {
    _x20429 = kk_integer_from_small(-17); /*int*/
  }
  _x20428 = kk_std_core_int32(_x20429, _ctx); /*int32*/
  return kk_std_core_show_expx(d, _x20428, _ctx);
}

kk_string_t kk_std_core_show_fixed(double d, kk_std_core_types__optional precision, kk_context_t* _ctx); /* (d : float64, precision : optional<int>) -> string */ 

kk_string_t kk_std_core__lift17192_show_list(kk_string_t sep, kk_std_core__list ys, kk_string_t acc, kk_context_t* _ctx); /* (sep : string, ys : list<string>, acc : string) -> string */ 

kk_string_t kk_std_core__mlift17695_show_list(kk_std_core__list _y_17414, kk_context_t* _ctx); /* forall<e> (list<string>) -> e string */ 

kk_string_t kk_std_core_show_list(kk_std_core__list xs, kk_function_t show_elem, kk_context_t* _ctx); /* forall<a,e> (xs : list<a>, show-elem : (a) -> e string) -> e string */ 

kk_string_t kk_std_core_show(kk_integer_t i, kk_context_t* _ctx); /* (i : int) -> string */ 

kk_string_t kk_std_core_show_1(double d, kk_std_core_types__optional precision, kk_context_t* _ctx); /* (d : float64, precision : optional<int>) -> string */ 
 
// Show the exception message

static inline kk_string_t kk_std_core_show_10(kk_std_core__exception exn0, kk_context_t* _ctx) { /* (exn : exception) -> string */ 
  {
    kk_string_t _x = exn0.message;
    kk_string_dup(_x);
    kk_std_core__exception_drop(exn0, _ctx);
    return _x;
  }
}
 
// Show a `:char` as a character literal

static inline kk_string_t kk_std_core_show_2(kk_char_t c, kk_context_t* _ctx) { /* (c : char) -> string */ 
  kk_string_t _x20492;
  kk_define_string_literal(, _s20493, 1, "\'")
  _x20492 = kk_string_dup(_s20493); /*string*/
  kk_string_t _x20494;
  kk_string_t _x20495 = kk_std_core_show_char(c, _ctx); /*string*/
  kk_string_t _x20496;
  kk_define_string_literal(, _s20497, 1, "\'")
  _x20496 = kk_string_dup(_s20497); /*string*/
  _x20494 = kk_std_core__lp__plus__plus__1_rp_(_x20495, _x20496, _ctx); /*string*/
  return kk_std_core__lp__plus__plus__1_rp_(_x20492, _x20494, _ctx);
}

kk_string_t kk_std_core__lift17193_show_3(kk_std_core__list ys, kk_string_t acc, kk_context_t* _ctx); /* (ys : list<string>, acc : string) -> string */ 

kk_string_t kk_std_core_show_3(kk_string_t s, kk_context_t* _ctx); /* (s : string) -> string */ 
 
// Convert a `:bool` to a string

static inline kk_string_t kk_std_core_show_4(bool b, kk_context_t* _ctx) { /* (b : bool) -> string */ 
  if (b) {
    kk_define_string_literal(, _s20516, 4, "True")
    return kk_string_dup(_s20516);
  }
  {
    kk_define_string_literal(, _s20517, 5, "False")
    return kk_string_dup(_s20517);
  }
}
 
// Convert a unit value `()` to a string

static inline kk_string_t kk_std_core_show_5(kk_unit_t u, kk_context_t* _ctx) { /* (u : ()) -> string */ 
  kk_define_string_literal(, _s20518, 2, "()")
  return kk_string_dup(_s20518);
}
 
// Show an `:sslice` as a string literal

static inline kk_string_t kk_std_core_show_6(kk_std_core__sslice s, kk_context_t* _ctx) { /* (s : sslice) -> string */ 
  kk_string_t _x20519 = kk_std_core_string_3(s, _ctx); /*string*/
  return kk_std_core_show_3(_x20519, _ctx);
}


// lift anonymous function
struct kk_std_core_show_fun20520__t_7 {
  struct kk_function_s _base;
};
extern kk_string_t kk_std_core_show_fun20520_7(kk_function_t _fself, kk_box_t _b_18671, kk_context_t* _ctx);
static inline kk_function_t kk_std_core_new_show_fun20520_7(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_show_fun20520_7, _ctx)
  return kk_function_dup(_fself);
}


static inline kk_string_t kk_std_core_show_7(kk_std_core__list xs, kk_context_t* _ctx) { /* (xs : list<string>) -> string */ 
  return kk_std_core_show_list(xs, kk_std_core_new_show_fun20520_7(_ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_show_fun20522__t_8 {
  struct kk_function_s _base;
};
extern kk_string_t kk_std_core_show_fun20522_8(kk_function_t _fself, kk_box_t _b_18676, kk_context_t* _ctx);
static inline kk_function_t kk_std_core_new_show_fun20522_8(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_show_fun20522_8, _ctx)
  return kk_function_dup(_fself);
}


static inline kk_string_t kk_std_core_show_8(kk_std_core__list xs, kk_context_t* _ctx) { /* (xs : list<int>) -> string */ 
  return kk_std_core_show_list(xs, kk_std_core_new_show_fun20522_8(_ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_show_fun20524__t_9 {
  struct kk_function_s _base;
};
extern kk_string_t kk_std_core_show_fun20524_9(kk_function_t _fself, kk_box_t _b_18681, kk_context_t* _ctx);
static inline kk_function_t kk_std_core_new_show_fun20524_9(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_show_fun20524_9, _ctx)
  return kk_function_dup(_fself);
}


static inline kk_string_t kk_std_core_show_9(kk_std_core__list xs, kk_context_t* _ctx) { /* (xs : list<bool>) -> string */ 
  return kk_std_core_show_list(xs, kk_std_core_new_show_fun20524_9(_ctx), _ctx);
}
 
// Print a string to the console, including a final newline character.

static inline kk_unit_t kk_std_core_println(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> console () */ 
  kk_std_core_printsln(s, _ctx); return kk_Unit;
}
 
// Print an integer to the console, including a final newline character.

static inline kk_unit_t kk_std_core_println_1(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> console () */ 
  kk_string_t _x20526 = kk_std_core_show(i, _ctx); /*string*/
  kk_std_core_printsln(_x20526, _ctx); return kk_Unit;
}
 
// Print a float64 to the console, including a final newline character.

static inline kk_unit_t kk_std_core_println_2(double d, kk_context_t* _ctx) { /* (d : float64) -> console () */ 
  kk_string_t _x20527 = kk_std_core_show_1(d, kk_std_core_types__new_None(_ctx), _ctx); /*string*/
  kk_std_core_printsln(_x20527, _ctx); return kk_Unit;
}
 
// Print a boolean to the console, including a final newline character

static inline kk_unit_t kk_std_core_println_3(bool b, kk_context_t* _ctx) { /* (b : bool) -> console () */ 
  kk_string_t _x20528;
  if (b) {
    kk_define_string_literal(, _s20529, 4, "True")
    _x20528 = kk_string_dup(_s20529); /*string*/
  }
  else {
    kk_define_string_literal(, _s20530, 5, "False")
    _x20528 = kk_string_dup(_s20530); /*string*/
  }
  kk_std_core_printsln(_x20528, _ctx); return kk_Unit;
}
 
// Print a character to the console, including a final newline character.

static inline kk_unit_t kk_std_core_println_4(kk_char_t c, kk_context_t* _ctx) { /* (c : char) -> console () */ 
  kk_string_t _x20531 = kk_std_core_string(c, _ctx); /*string*/
  kk_std_core_printsln(_x20531, _ctx); return kk_Unit;
}
 
// Print a unit value to the console, including a final newline character

static inline kk_unit_t kk_std_core_println_5(kk_unit_t u, kk_context_t* _ctx) { /* (u : ()) -> console () */ 
  kk_string_t _x20532;
  kk_define_string_literal(, _s20533, 2, "()")
  _x20532 = kk_string_dup(_s20533); /*string*/
  kk_std_core_printsln(_x20532, _ctx); return kk_Unit;
}

kk_unit_t kk_std_core_prints(kk_string_t s, kk_context_t* _ctx); /* (s : string) -> console () */ 
 
// call `throw-exn` operation of the `:exn` effect

static inline kk_box_t kk_std_core_throw_exn(kk_std_core__exception exn0, kk_context_t* _ctx) { /* forall<a> (exn : exception) -> exn a */ 
  kk_std_core_hnd__ev ev_17903;
  kk_ssize_t _x20538 = (KK_IZ(0)); /*ssize_t*/
  ev_17903 = kk_evv_at(_x20538,kk_context()); /*std/core/hnd/ev<.hnd-exn>*/
  {
    struct kk_std_core_hnd_Ev* _con20539 = kk_std_core_hnd__as_Ev(ev_17903);
    kk_std_core_hnd__marker m0 = _con20539->marker;
    kk_box_t _box_x18693 = _con20539->hnd;
    kk_std_core__hnd_exn h = kk_std_core__hnd_exn_unbox(_box_x18693, NULL);
    kk_std_core__hnd_exn_dup(h);
    kk_std_core_hnd__clause1 _match_19757;
    kk_std_core_hnd__clause1 _brw_19758 = kk_std_core__select_throw_exn(h, _ctx); /*std/core/hnd/clause1<exception,1938,.hnd-exn,1939,1940>*/;
    kk_std_core__hnd_exn_drop(h, _ctx);
    _match_19757 = _brw_19758; /*std/core/hnd/clause1<exception,1938,.hnd-exn,1939,1940>*/
    {
      kk_function_t _fun_unbox_x18697 = _match_19757.clause;
      return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x18697, (_fun_unbox_x18697, m0, ev_17903, kk_std_core__exception_box(exn0, _ctx), _ctx));
    }
  }
}

kk_unit_t kk_std_core__default_exn(kk_function_t action, kk_context_t* _ctx); /* forall<e> (action : () -> <console,exn|e> ()) -> <console|e> () */ 

kk_std_core_types__maybe kk_std_core__lp__lb__rb__2_rp_(kk_std_core__list xs, kk_integer_t n, kk_context_t* _ctx); /* forall<a> (xs : list<a>, n : int) -> maybe<a> */ 
 
// Raise an integer `i` to the power of `exp`.

static inline kk_integer_t kk_std_core__lp__hat__1_rp_(kk_integer_t i, kk_integer_t exp, kk_context_t* _ctx) { /* (i : int, exp : int) -> int */ 
  return kk_std_core_pow(i, exp, _ctx);
}

kk_std_core__sslice kk_std_core_after(kk_std_core__sslice slice0, kk_context_t* _ctx); /* (slice : sslice) -> sslice */ 

bool kk_std_core__mlift17696_all(kk_function_t predicate, kk_std_core__list xx, bool _y_17417, kk_context_t* _ctx); /* forall<a,e> (predicate : (a) -> e bool, xx : list<a>, bool) -> e bool */ 

bool kk_std_core_all(kk_std_core__list xs, kk_function_t predicate0, kk_context_t* _ctx); /* forall<a,e> (xs : list<a>, predicate : (a) -> e bool) -> e bool */ 

bool kk_std_core__mlift17697_any(kk_function_t predicate, kk_std_core__list xx, bool _y_17421, kk_context_t* _ctx); /* forall<a,e> (predicate : (a) -> e bool, xx : list<a>, bool) -> e bool */ 

bool kk_std_core_any(kk_std_core__list xs, kk_function_t predicate0, kk_context_t* _ctx); /* forall<a,e> (xs : list<a>, predicate : (a) -> e bool) -> e bool */ 

static inline kk_unit_t kk_std_core_assert(kk_string_t message0, bool condition, kk_context_t* _ctx) { /* (message : string, condition : bool) -> () */ 
  if (condition) {
    kk_string_drop(message0, _ctx);
    kk_Unit; return kk_Unit;
  }
  {
    kk_std_core_unsafe_assert_fail(message0, _ctx); return kk_Unit;
  }
}

kk_std_core_types__maybe kk_std_core_at(kk_vector_t v, kk_integer_t index, kk_context_t* _ctx); /* forall<a> (v : vector<a>, index : int) -> maybe<a> */ 
 
// O(1). Return the string slice from the start of a string up to the
// start of `slice` argument.

static inline kk_std_core__sslice kk_std_core_before(kk_std_core__sslice slice0, kk_context_t* _ctx) { /* (slice : sslice) -> sslice */ 
  {
    kk_string_t s = slice0.str;
    kk_ssize_t start0 = slice0.start;
    kk_string_dup(s);
    kk_std_core__sslice_drop(slice0, _ctx);
    kk_ssize_t _x20577 = (KK_IZ(0)); /*ssize_t*/
    return kk_std_core__new_Sslice(s, _x20577, start0, _ctx);
  }
}
 
// Convert an int to a boolean, using `False` for 0 and `True` otherwise.

static inline bool kk_std_core_bool(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> bool */ 
  bool _brw_19750 = kk_integer_neq_borrow(i,(kk_integer_from_small(0)),kk_context()); /*bool*/;
  kk_integer_drop(i, _ctx);
  return _brw_19750;
}
 
// Convert a `:maybe` type to a boolean using `False` for `Nothing` and `True` for `Just`.

static inline bool kk_std_core_bool_1(kk_std_core_types__maybe m, kk_context_t* _ctx) { /* forall<a> (m : maybe<a>) -> bool */ 
  if (kk_std_core_types__is_Nothing(m)) {
    return false;
  }
  {
    kk_std_core_types__maybe_drop(m, _ctx);
    return true;
  }
}
 
// Convert a string to a boolean, using `False` for the empty string and `True` otherwise.

static inline bool kk_std_core_bool_2(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> bool */ 
  kk_string_t _x20578 = kk_string_empty(); /*string*/
  return kk_string_is_neq(s,_x20578,kk_context());
}

kk_std_core__sslice kk_std_core_first(kk_string_t s, kk_std_core_types__optional n, kk_context_t* _ctx); /* (s : string, n : optional<int>) -> sslice */ 

kk_string_t kk_std_core_capitalize(kk_string_t s, kk_context_t* _ctx); /* (s : string) -> string */ 

kk_box_t kk_std_core_try(kk_function_t action, kk_function_t hndl, kk_context_t* _ctx); /* forall<a,e> (action : () -> <exn|e> a, hndl : (exception) -> e a) -> e a */ 
 
// monadic lift

static inline kk_std_core__error kk_std_core__mlift17698_try_1(kk_box_t _y_17426, kk_context_t* _ctx) { /* forall<a,e> (a) -> <exn|e> error<a> */ 
  return kk_std_core__new_Ok(_y_17426, _ctx);
}

kk_std_core__error kk_std_core_try_1(kk_function_t action, kk_context_t* _ctx); /* forall<a,e> (action : () -> <exn|e> a) -> e error<a> */ 
 
// _Deprecated_; use `try` instead. Catch an exception raised by `throw` and handle it.
// Use `on-exn` or `on-exit` when appropiate.

static inline kk_box_t kk_std_core_catch(kk_function_t action, kk_function_t hndl, kk_context_t* _ctx) { /* forall<a,e> (action : () -> <exn|e> a, hndl : (exception) -> e a) -> e a */ 
  return kk_std_core_try(action, hndl, _ctx);
}

kk_std_core_types__tuple2_ kk_std_core_cdivmod_exp10(kk_integer_t i, kk_integer_t n, kk_context_t* _ctx); /* (i : int, n : int) -> (int, int) */ 

kk_std_core__list kk_std_core__ctail_concat_maybe(kk_std_core__list xs, kk_std_core_types__ctail _acc, kk_context_t* _ctx); /* forall<a> (xs : list<maybe<a>>, ctail<list<a>>) -> list<a> */ 

kk_std_core__list kk_std_core_concat_maybe(kk_std_core__list xs0, kk_context_t* _ctx); /* forall<a> (xs : list<maybe<a>>) -> list<a> */ 

kk_std_core_types__maybe kk_std_core__mlift17699_op(kk_function_t action, kk_ssize_t end, kk_ssize_t i, kk_std_core_types__maybe _y_17429, kk_context_t* _ctx); /* forall<a,e> (action : (ssize_t) -> e maybe<a>, end : ssize_t, i : ssize_t, maybe<a>) -> e maybe<a> */ 

kk_std_core_types__maybe kk_std_core__lift17194_for_whilez(kk_function_t action0, kk_ssize_t end0, kk_ssize_t i0, kk_context_t* _ctx); /* forall<a,e> (action : (ssize_t) -> e maybe<a>, end : ssize_t, i : ssize_t) -> e maybe<a> */ 
 
// Executes `action`  for each integer between `start`  upto `end`  (including both `start`  and `end` ).
// If `start > end`  the function returns without any call to `action` .
// If `action` returns `Just`, the iteration is stopped and the result returned

static inline kk_std_core_types__maybe kk_std_core_for_whilez(kk_ssize_t start0, kk_ssize_t end, kk_function_t action, kk_context_t* _ctx) { /* forall<a,e> (start : ssize_t, end : ssize_t, action : (ssize_t) -> e maybe<a>) -> e maybe<a> */ 
  return kk_std_core__lift17194_for_whilez(action, end, start0, _ctx);
}
 
// O(1). The entire string as a slice

static inline kk_std_core__sslice kk_std_core_slice(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> sslice */ 
  kk_string_t _x20639 = kk_string_dup(s); /*string*/
  kk_ssize_t _x20640 = (KK_IZ(0)); /*ssize_t*/
  kk_ssize_t _x20641 = kk_string_len(s,kk_context()); /*ssize_t*/
  return kk_std_core__new_Sslice(_x20639, _x20640, _x20641, _ctx);
}

kk_std_core_types__maybe kk_std_core__mlift17700_foreach_while(kk_function_t action, kk_std_core__list xx, kk_std_core_types__maybe _y_17434, kk_context_t* _ctx); /* forall<a,b,e> (action : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e maybe<b> */ 

kk_std_core_types__maybe kk_std_core_foreach_while(kk_std_core__list xs, kk_function_t action0, kk_context_t* _ctx); /* forall<a,b,e> (xs : list<a>, action : (a) -> e maybe<b>) -> e maybe<b> */ 

kk_std_core_types__maybe kk_std_core__mlift17701_foreach_while_1(kk_function_t action, kk_std_core__sslice rest, kk_std_core_types__maybe _y_17438, kk_context_t* _ctx); /* forall<a,e> (action : (c : char) -> e maybe<a>, rest : sslice, maybe<a>) -> e maybe<a> */ 

kk_std_core_types__maybe kk_std_core_foreach_while_1(kk_std_core__sslice slice0, kk_function_t action0, kk_context_t* _ctx); /* forall<a,e> (slice : sslice, action : (c : char) -> e maybe<a>) -> e maybe<a> */ 
 
// Invoke a function for each character in a string.
// If `action` returns `Just`, the function returns immediately with that result.

static inline kk_std_core_types__maybe kk_std_core_foreach_while_2(kk_string_t s, kk_function_t action, kk_context_t* _ctx) { /* forall<a,e> (s : string, action : (c : char) -> e maybe<a>) -> e maybe<a> */ 
  kk_std_core__sslice _x20654;
  kk_string_t _x20655 = kk_string_dup(s); /*string*/
  kk_ssize_t _x20656 = (KK_IZ(0)); /*ssize_t*/
  kk_ssize_t _x20657 = kk_string_len(s,kk_context()); /*ssize_t*/
  _x20654 = kk_std_core__new_Sslice(_x20655, _x20656, _x20657, _ctx); /*sslice*/
  return kk_std_core_foreach_while_1(_x20654, action, _ctx);
}

kk_std_core_types__maybe kk_std_core_foreach_while_3(kk_vector_t v, kk_function_t f, kk_context_t* _ctx); /* forall<a,b,e> (v : vector<a>, f : (a) -> e maybe<b>) -> e maybe<b> */ 

kk_unit_t kk_std_core__mlift17702_foreach(kk_function_t action, kk_std_core__list xx, kk_unit_t wild__, kk_context_t* _ctx); /* forall<a,e> (action : (a) -> e (), xx : list<a>, wild_ : ()) -> e () */ 

kk_unit_t kk_std_core_foreach(kk_std_core__list xs, kk_function_t action0, kk_context_t* _ctx); /* forall<a,e> (xs : list<a>, action : (a) -> e ()) -> e () */ 
 
// monadic lift

static inline kk_std_core_types__maybe kk_std_core__mlift17703_foreach_1(kk_unit_t wild__, kk_context_t* _ctx) { /* forall<_a,e> (wild_ : ()) -> e maybe<_a> */ 
  return kk_std_core_types__new_Nothing(_ctx);
}
 
// monadic lift

static inline kk_unit_t kk_std_core__mlift17704_foreach_1(kk_std_core_types__maybe wild__0, kk_context_t* _ctx) { /* forall<_a,e> (wild_0 : maybe<_a>) -> e () */ 
  kk_std_core_types__maybe_drop(wild__0, _ctx);
  kk_Unit; return kk_Unit;
}

kk_unit_t kk_std_core_foreach_1(kk_std_core__sslice slice0, kk_function_t action, kk_context_t* _ctx); /* forall<e> (slice : sslice, action : (c : char) -> e ()) -> e () */ 
 
// monadic lift

static inline kk_std_core_types__maybe kk_std_core__mlift17705_foreach_2(kk_unit_t wild__, kk_context_t* _ctx) { /* forall<_a,e> (wild_ : ()) -> e maybe<_a> */ 
  return kk_std_core_types__new_Nothing(_ctx);
}
 
// monadic lift

static inline kk_unit_t kk_std_core__mlift17706_foreach_2(kk_std_core_types__maybe wild__0, kk_context_t* _ctx) { /* forall<_a,e> (wild_0 : maybe<_a>) -> e () */ 
  kk_std_core_types__maybe_drop(wild__0, _ctx);
  kk_Unit; return kk_Unit;
}

kk_unit_t kk_std_core_foreach_2(kk_string_t s, kk_function_t action, kk_context_t* _ctx); /* forall<e> (s : string, action : (c : char) -> e ()) -> e () */ 

kk_unit_t kk_std_core_foreach_3(kk_vector_t v, kk_function_t f, kk_context_t* _ctx); /* forall<a,e> (v : vector<a>, f : (a) -> e ()) -> e () */ 

kk_integer_t kk_std_core_count_1(kk_string_t s, kk_context_t* _ctx); /* (s : string) -> int */ 

kk_integer_t kk_std_core_count_2(kk_std_core__sslice slice0, kk_context_t* _ctx); /* (slice : sslice) -> int */ 

kk_integer_t kk_std_core_count_3(kk_string_t s, kk_function_t pred, kk_context_t* _ctx); /* (s : string, pred : (char) -> bool) -> int */ 

static inline kk_integer_t kk_std_core_dec(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> int */ 
  return kk_integer_add_small_const(i, -1, _ctx);
}
 
// Create a new `:delayed` value.

static inline kk_std_core__delayed kk_std_core_delay(kk_function_t action, kk_context_t* _ctx) { /* forall<a,e> (action : () -> e a) -> delayed<e,a> */ 
  kk_ref_t _x20691;
  kk_box_t _x20692;
  kk_std_core_types__either _x20693 = kk_std_core_types__new_Left(kk_function_box(action), _ctx); /*either<1024,1025>*/
  _x20692 = kk_std_core_types__either_box(_x20693, _ctx); /*1000*/
  _x20691 = kk_ref_alloc(_x20692,kk_context()); /*ref<1001,1000>*/
  return kk_std_core__new_Delay(_x20691, _ctx);
}
 
// Calculate `10^exp`

static inline kk_integer_t kk_std_core_exp10(kk_integer_t exp, kk_context_t* _ctx) { /* (exp : int) -> int */ 
  return kk_std_core_mul_exp10(kk_integer_from_small(1), exp, _ctx);
}

bool kk_std_core_is_neg_2(kk_integer_t i, kk_context_t* _ctx); /* (i : int) -> bool */ 
 
// Is the value negative?

static inline bool kk_std_core_is_neg_3(double d, kk_context_t* _ctx) { /* (d : float64) -> bool */ 
  return (d < (0.0));
}

kk_std_core_types__tuple2_ kk_std_core_divmod_exp10(kk_integer_t i, kk_integer_t n, kk_context_t* _ctx); /* (i : int, n : int) -> (int, int) */ 

kk_std_core__list kk_std_core_drop(kk_std_core__list xs, kk_integer_t n, kk_context_t* _ctx); /* forall<a> (xs : list<a>, n : int) -> list<a> */ 

kk_std_core__list kk_std_core__mlift17707_drop_while(kk_function_t predicate, kk_std_core__list xs, kk_std_core__list xx, bool _y_17454, kk_context_t* _ctx); /* forall<a,e> (predicate : (a) -> e bool, xs : list<a>, xx : list<a>, bool) -> e list<a> */ 

kk_std_core__list kk_std_core_drop_while(kk_std_core__list xs0, kk_function_t predicate0, kk_context_t* _ctx); /* forall<a,e> (xs : list<a>, predicate : (a) -> e bool) -> e list<a> */ 

extern kk_std_core__sslice kk_std_core_empty;

kk_std_core_types__maybe kk_std_core_ends_with(kk_string_t s, kk_string_t post, kk_context_t* _ctx); /* (s : string, post : string) -> maybe<sslice> */ 

kk_box_t kk_std_core_throw(kk_string_t message0, kk_std_core_types__optional info0, kk_context_t* _ctx); /* forall<a> (message : string, info : optional<exception-info>) -> exn a */ 

kk_box_t kk_std_core__mlift17708_error_pattern(kk_string_t definition, kk_string_t location, kk_string_t _c_17459, kk_context_t* _ctx); /* forall<a> (definition : string, location : string, string) -> a */ 

kk_box_t kk_std_core_error_pattern(kk_string_t location, kk_string_t definition, kk_context_t* _ctx); /* forall<a> (location : string, definition : string) -> exn a */ 

kk_box_t kk_std_core_untry(kk_std_core__error err, kk_context_t* _ctx); /* forall<a> (err : error<a>) -> exn a */ 
 
// Transform an `:error` type back to an `exn` effect.

static inline kk_box_t kk_std_core_exn(kk_std_core__error err, kk_context_t* _ctx) { /* forall<a> (err : error<a>) -> exn a */ 
  return kk_std_core_untry(err, _ctx);
}
 
// Calculate `2^exp`.

static inline kk_integer_t kk_std_core_exp2(kk_integer_t exp, kk_context_t* _ctx) { /* (exp : int) -> int */ 
  return kk_std_core_pow(kk_integer_from_small(2), exp, _ctx);
}

kk_std_core__list kk_std_core__mlift17709_op(kk_std_core_types__ctail _acc, kk_function_t pred, kk_box_t x, kk_std_core__list xx, bool _y_17464, kk_context_t* _ctx); /* forall<a,e> (ctail<list<a>>, pred : (a) -> e bool, x : a, xx : list<a>, bool) -> e list<a> */ 

kk_std_core__list kk_std_core__mlift17710_op(kk_function_t _accm, kk_function_t pred0, kk_box_t x0, kk_std_core__list xx0, bool _y_17469, kk_context_t* _ctx); /* forall<a,e> ((list<a>) -> list<a>, pred : (a) -> e bool, x : a, xx : list<a>, bool) -> e list<a> */ 

kk_std_core__list kk_std_core__ctail_filter(kk_std_core__list xs, kk_function_t pred1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx); /* forall<a,e> (xs : list<a>, pred : (a) -> e bool, ctail<list<a>>) -> e list<a> */ 

kk_std_core__list kk_std_core__ctailm_filter(kk_std_core__list xs0, kk_function_t pred2, kk_function_t _accm0, kk_context_t* _ctx); /* forall<a,e> (xs : list<a>, pred : (a) -> e bool, (list<a>) -> list<a>) -> e list<a> */ 

kk_std_core__list kk_std_core_filter(kk_std_core__list xs1, kk_function_t pred3, kk_context_t* _ctx); /* forall<a,e> (xs : list<a>, pred : (a) -> e bool) -> e list<a> */ 

kk_std_core__list kk_std_core__mlift17711_op(kk_std_core_types__ctail _acc, kk_function_t pred, kk_std_core__list xx, kk_std_core_types__maybe _y_17477, kk_context_t* _ctx); /* forall<a,b,e> (ctail<list<b>>, pred : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e list<b> */ 

kk_std_core__list kk_std_core__mlift17712_op(kk_function_t _accm, kk_function_t pred0, kk_std_core__list xx0, kk_std_core_types__maybe _y_17482, kk_context_t* _ctx); /* forall<a,b,e> ((list<b>) -> list<b>, pred : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e list<b> */ 

kk_std_core__list kk_std_core__ctail_filter_map(kk_std_core__list xs, kk_function_t pred1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx); /* forall<a,b,e> (xs : list<a>, pred : (a) -> e maybe<b>, ctail<list<b>>) -> e list<b> */ 

kk_std_core__list kk_std_core__ctailm_filter_map(kk_std_core__list xs0, kk_function_t pred2, kk_function_t _accm0, kk_context_t* _ctx); /* forall<a,b,e> (xs : list<a>, pred : (a) -> e maybe<b>, (list<b>) -> list<b>) -> e list<b> */ 

kk_std_core__list kk_std_core_filter_map(kk_std_core__list xs1, kk_function_t pred3, kk_context_t* _ctx); /* forall<a,b,e> (xs : list<a>, pred : (a) -> e maybe<b>) -> e list<b> */ 

bool kk_std_core_is_zero_1(kk_ssize_t i, kk_context_t* _ctx); /* (i : ssize_t) -> bool */ 
 
// Is the value zero?

static inline bool kk_std_core_is_zero_2(double d, kk_context_t* _ctx) { /* (d : float64) -> bool */ 
  return (d == (0.0));
}

kk_std_core_types__maybe kk_std_core_find(kk_std_core__list xs, kk_function_t pred, kk_context_t* _ctx); /* forall<a> (xs : list<a>, pred : (a) -> bool) -> maybe<a> */ 

kk_std_core_types__maybe kk_std_core_find_1(kk_string_t s, kk_string_t sub, kk_context_t* _ctx); /* (s : string, sub : string) -> maybe<sslice> */ 

kk_std_core_types__maybe kk_std_core_find_last(kk_string_t s, kk_string_t sub, kk_context_t* _ctx); /* (s : string, sub : string) -> maybe<sslice> */ 
 
// Find the first element satisfying some predicate and return it.

static inline kk_std_core_types__maybe kk_std_core_find_maybe(kk_std_core__list xs, kk_function_t pred, kk_context_t* _ctx) { /* forall<a,b> (xs : list<a>, pred : (a) -> maybe<b>) -> maybe<b> */ 
  return kk_std_core_foreach_while(xs, pred, _ctx);
}

kk_std_core__list kk_std_core__mlift17713_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_std_core__list xx, kk_std_core_types__maybe _y_17490, kk_context_t* _ctx); /* forall<a,b,e> (ctail<list<b>>, f : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e list<b> */ 

kk_std_core__list kk_std_core__mlift17714_op(kk_function_t _accm, kk_function_t f0, kk_std_core__list xx0, kk_std_core_types__maybe _y_17495, kk_context_t* _ctx); /* forall<a,b,e> ((list<b>) -> list<b>, f : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e list<b> */ 

kk_std_core__list kk_std_core__ctail_flatmap_maybe(kk_std_core__list xs, kk_function_t f1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx); /* forall<a,b,e> (xs : list<a>, f : (a) -> e maybe<b>, ctail<list<b>>) -> e list<b> */ 

kk_std_core__list kk_std_core__ctailm_flatmap_maybe(kk_std_core__list xs0, kk_function_t f2, kk_function_t _accm0, kk_context_t* _ctx); /* forall<a,b,e> (xs : list<a>, f : (a) -> e maybe<b>, (list<b>) -> list<b>) -> e list<b> */ 

kk_std_core__list kk_std_core_flatmap_maybe(kk_std_core__list xs1, kk_function_t f3, kk_context_t* _ctx); /* forall<a,b,e> (xs : list<a>, f : (a) -> e maybe<b>) -> e list<b> */ 

kk_box_t kk_std_core__mlift17715_fold_int(kk_integer_t end, kk_function_t f, kk_integer_t start0, kk_box_t x, kk_context_t* _ctx); /* forall<a,e> (end : int, f : (int, a) -> e a, start0 : int, x : a) -> e a */ 

kk_box_t kk_std_core_fold_int(kk_integer_t start00, kk_integer_t end0, kk_box_t init0, kk_function_t f0, kk_context_t* _ctx); /* forall<a,e> (start : int, end : int, init : a, f : (int, a) -> e a) -> e a */ 
 
// fold over the integers between [0,`upto`)  (not including `upto`).

static inline kk_box_t kk_std_core_fold_int_1(kk_integer_t upto, kk_box_t init0, kk_function_t f, kk_context_t* _ctx) { /* forall<a,e> (upto : int, init : a, f : (int, a) -> e a) -> e a */ 
  kk_integer_t _x20833 = kk_integer_add_small_const(upto, -1, _ctx); /*int*/
  return kk_std_core_fold_int(kk_integer_from_small(0), _x20833, init0, f, _ctx);
}

kk_box_t kk_std_core__mlift17716_foldl(kk_function_t f, kk_std_core__list xx, kk_box_t _y_17507, kk_context_t* _ctx); /* forall<a,e,b> (f : (a, b) -> e a, xx : list<b>, a) -> e a */ 

kk_box_t kk_std_core_foldl(kk_std_core__list xs, kk_box_t z, kk_function_t f0, kk_context_t* _ctx); /* forall<a,b,e> (list<a>, b, (b, a) -> e b) -> e b */ 

kk_box_t kk_std_core_foldl1(kk_std_core__list xs, kk_function_t f, kk_context_t* _ctx); /* forall<a,e> (xs : list<a>, f : (a, a) -> <exn|e> a) -> <exn|e> a */ 

kk_std_core__list kk_std_core__lift17195_reverse(kk_std_core__list acc, kk_std_core__list ys, kk_context_t* _ctx); /* forall<a> (acc : list<a>, ys : list<a>) -> list<a> */ 
 
// Reverse a list.

static inline kk_std_core__list kk_std_core_reverse(kk_std_core__list xs, kk_context_t* _ctx) { /* forall<a> (xs : list<a>) -> list<a> */ 
  return kk_std_core__lift17195_reverse(kk_std_core__new_Nil(_ctx), xs, _ctx);
}
 
// Fold a list from the right, i.e. `foldr([1,2],0,(+)) == 1+(2+0)`
// Note, `foldr` is less efficient than `foldl` as it reverses the list first.


// lift anonymous function
struct kk_std_core_foldr_fun20848__t {
  struct kk_function_s _base;
  kk_function_t f;
};
extern kk_box_t kk_std_core_foldr_fun20848(kk_function_t _fself, kk_box_t x, kk_box_t y, kk_context_t* _ctx);
static inline kk_function_t kk_std_core_new_foldr_fun20848(kk_function_t f, kk_context_t* _ctx) {
  struct kk_std_core_foldr_fun20848__t* _self = kk_function_alloc_as(struct kk_std_core_foldr_fun20848__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foldr_fun20848, kk_context());
  _self->f = f;
  return &_self->_base;
}


static inline kk_box_t kk_std_core_foldr(kk_std_core__list xs, kk_box_t z, kk_function_t f, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, z : b, f : (a, b) -> e b) -> e b */ 
  kk_std_core__list _x20847 = kk_std_core__lift17195_reverse(kk_std_core__new_Nil(_ctx), xs, _ctx); /*list<2485>*/
  return kk_std_core_foldl(_x20847, z, kk_std_core_new_foldr_fun20848(f, _ctx), _ctx);
}

kk_box_t kk_std_core_foldr1(kk_std_core__list xs, kk_function_t f, kk_context_t* _ctx); /* forall<a,e> (xs : list<a>, f : (a, a) -> <exn|e> a) -> <exn|e> a */ 

kk_unit_t kk_std_core__mlift17717_op(kk_function_t action, kk_integer_t end, kk_integer_t i, kk_unit_t wild__, kk_context_t* _ctx); /* forall<e> (action : (int) -> e (), end : int, i : int, wild_ : ()) -> e () */ 

kk_unit_t kk_std_core__lift17196_for(kk_function_t action0, kk_integer_t end0, kk_integer_t i0, kk_context_t* _ctx); /* forall<e> (action : (int) -> e (), end : int, i : int) -> e () */ 
 
// Executes `action`  for each integer between `start`  upto `end`  (including both `start`  and `end` ).
// If `start > end`  the function returns without any call to `action` .

static inline kk_unit_t kk_std_core_for(kk_integer_t start0, kk_integer_t end, kk_function_t action, kk_context_t* _ctx) { /* forall<e> (start : int, end : int, action : (int) -> e ()) -> e () */ 
  kk_integer_t _x20862 = kk_integer_dup(start0); /*int*/
  kk_std_core__lift17196_for(action, end, _x20862, _ctx); return kk_Unit;
}

kk_std_core_types__maybe kk_std_core__mlift17718_op(kk_function_t action, kk_integer_t end, kk_integer_t i, kk_std_core_types__maybe _y_17522, kk_context_t* _ctx); /* forall<a,e> (action : (int) -> e maybe<a>, end : int, i : int, maybe<a>) -> e maybe<a> */ 

kk_std_core_types__maybe kk_std_core__lift17197_for_while(kk_function_t action0, kk_integer_t end0, kk_integer_t i0, kk_context_t* _ctx); /* forall<a,e> (action : (int) -> e maybe<a>, end : int, i : int) -> e maybe<a> */ 
 
// Executes `action`  for each integer between `start`  upto `end`  (including both `start`  and `end` ).
// If `start > end`  the function returns without any call to `action` .
// If `action` returns `Just`, the iteration is stopped and the result returned

static inline kk_std_core_types__maybe kk_std_core_for_while(kk_integer_t start0, kk_integer_t end, kk_function_t action, kk_context_t* _ctx) { /* forall<a,e> (start : int, end : int, action : (int) -> e maybe<a>) -> e maybe<a> */ 
  return kk_std_core__lift17197_for_while(action, end, start0, _ctx);
}
 
// monadic lift

static inline kk_unit_t kk_std_core__mlift17719_foreach_indexed(kk_ref_t i, kk_integer_t _y_17530, kk_context_t* _ctx) { /* forall<h,e> (i : local-var<h,int>, int) -> <local<h>|e> () */ 
  kk_integer_t _b_19043_19041 = kk_integer_add_small_const(_y_17530, 1, _ctx); /*int*/;
  kk_unit_t _brw_19677 = kk_Unit;
  kk_ref_set_borrow(i,(kk_integer_box(_b_19043_19041)),kk_context());
  kk_ref_drop(i, _ctx);
  _brw_19677; return kk_Unit;
}

kk_unit_t kk_std_core__mlift17720_foreach_indexed(kk_ref_t i, kk_unit_t wild__, kk_context_t* _ctx); /* forall<h,e> (i : local-var<h,int>, wild_ : ()) -> <local<h>|e> () */ 

kk_unit_t kk_std_core__mlift17721_foreach_indexed(kk_function_t action, kk_ref_t i, kk_box_t x, kk_integer_t j, kk_context_t* _ctx); /* forall<h,a,e> (action : (int, a) -> e (), i : local-var<h,int>, x : a, j : int) -> <local<h>|e> () */ 

kk_unit_t kk_std_core_foreach_indexed(kk_std_core__list xs, kk_function_t action, kk_context_t* _ctx); /* forall<a,e> (xs : list<a>, action : (int, a) -> e ()) -> e () */ 

kk_unit_t kk_std_core_foreach_indexed_1(kk_vector_t v, kk_function_t f, kk_context_t* _ctx); /* forall<a,e> (v : vector<a>, f : (int, a) -> e ()) -> e () */ 
 
// Print a string to the console.

static inline kk_unit_t kk_std_core_print(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> console () */ 
  kk_std_core_prints(s, _ctx); return kk_Unit;
}
 
// Print an integer to the console.

static inline kk_unit_t kk_std_core_print_1(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> console () */ 
  kk_string_t _x20890 = kk_std_core_show(i, _ctx); /*string*/
  kk_std_core_prints(_x20890, _ctx); return kk_Unit;
}
 
// Print a float64 to the console.

static inline kk_unit_t kk_std_core_print_2(double d, kk_context_t* _ctx) { /* (d : float64) -> console () */ 
  kk_string_t _x20891 = kk_std_core_show_1(d, kk_std_core_types__new_None(_ctx), _ctx); /*string*/
  kk_std_core_prints(_x20891, _ctx); return kk_Unit;
}
 
// Print a boolean to the console

static inline kk_unit_t kk_std_core_print_3(bool b, kk_context_t* _ctx) { /* (b : bool) -> console () */ 
  kk_string_t _x20892;
  if (b) {
    kk_define_string_literal(, _s20893, 4, "True")
    _x20892 = kk_string_dup(_s20893); /*string*/
  }
  else {
    kk_define_string_literal(, _s20894, 5, "False")
    _x20892 = kk_string_dup(_s20894); /*string*/
  }
  kk_std_core_prints(_x20892, _ctx); return kk_Unit;
}
 
// Print a character to the console.

static inline kk_unit_t kk_std_core_print_4(kk_char_t c, kk_context_t* _ctx) { /* (c : char) -> console () */ 
  kk_string_t _x20895 = kk_std_core_string(c, _ctx); /*string*/
  kk_std_core_prints(_x20895, _ctx); return kk_Unit;
}
 
// Print a unit value to the console

static inline kk_unit_t kk_std_core_print_5(kk_unit_t u, kk_context_t* _ctx) { /* (u : ()) -> console () */ 
  kk_string_t _x20896;
  kk_define_string_literal(, _s20897, 2, "()")
  _x20896 = kk_string_dup(_s20897); /*string*/
  kk_std_core_prints(_x20896, _ctx); return kk_Unit;
}
 
// Generic print routine: prints the internal representation as a string to the console,
// including a final newline character.
// Note: this breaks parametricity so it should not be pub

static inline kk_unit_t kk_std_core_gprint(kk_box_t x, kk_context_t* _ctx) { /* forall<a> (x : a) -> console () */ 
  kk_string_t s_17100 = kk_std_core_gshow(x, _ctx); /*string*/;
  kk_std_core_prints(s_17100, _ctx); return kk_Unit;
}
 
// Generic print routine: prints the internal representation as a string to the console, including a final newline character.
// Note: this breaks parametricity so it should not be pub

static inline kk_unit_t kk_std_core_gprintln(kk_box_t x, kk_context_t* _ctx) { /* forall<a> (x : a) -> console () */ 
  kk_string_t s_17101 = kk_std_core_gshow(x, _ctx); /*string*/;
  kk_std_core_printsln(s_17101, _ctx); return kk_Unit;
}

kk_std_core_types__maybe kk_std_core_head_1(kk_std_core__list xs, kk_context_t* _ctx); /* forall<a> (xs : list<a>) -> maybe<a> */ 

kk_box_t kk_std_core_head_2(kk_std_core__list xs, kk_box_t default0, kk_context_t* _ctx); /* forall<a> (xs : list<a>, default : a) -> a */ 

kk_string_t kk_std_core_head_3(kk_string_t s, kk_context_t* _ctx); /* (s : string) -> string */ 

kk_std_core_types__maybe kk_std_core_head_char(kk_string_t s, kk_context_t* _ctx); /* (s : string) -> maybe<char> */ 
 
// Convenient shorthand to `int32`, e.g. `1234.i32`

static inline int32_t kk_std_core_i32(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> int32 */ 
  return kk_std_core_int32(i, _ctx);
}
 
// Convenient shorthand to `int64`, e.g. `1234.i64`

static inline int64_t kk_std_core_i64(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> int64 */ 
  return kk_std_core_int64(i, _ctx);
}

kk_integer_t kk_std_core_index_of_acc(kk_std_core__list xs, kk_function_t pred, kk_integer_t idx, kk_context_t* _ctx); /* forall<a> (xs : list<a>, pred : (a) -> bool, idx : int) -> int */ 
 
// Returns the index of the first element where `pred` holds, or `-1` if no such element exists.

static inline kk_integer_t kk_std_core_index_of(kk_std_core__list xs, kk_function_t pred, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, pred : (a) -> bool) -> int */ 
  return kk_std_core_index_of_acc(xs, pred, kk_integer_from_small(0), _ctx);
}

kk_std_core__list kk_std_core__ctail_init(kk_std_core__list xs, kk_std_core_types__ctail _acc, kk_context_t* _ctx); /* forall<a> (xs : list<a>, ctail<list<a>>) -> list<a> */ 

kk_std_core__list kk_std_core_init(kk_std_core__list xs0, kk_context_t* _ctx); /* forall<a> (xs : list<a>) -> list<a> */ 

extern kk_std_core__sslice kk_std_core_invalid;
 
// Is the character a lower-case ASCII character ?

static inline bool kk_std_core_is_lower(kk_char_t c, kk_context_t* _ctx) { /* (c : char) -> bool */ 
  bool _match_19663 = (c >= ('a')); /*bool*/;
  if (_match_19663) {
    return (c <= ('z'));
  }
  {
    return false;
  }
}
 
// Is the character an upper-case ASCII character ?

static inline bool kk_std_core_is_upper(kk_char_t c, kk_context_t* _ctx) { /* (c : char) -> bool */ 
  bool _match_19662 = (c >= ('A')); /*bool*/;
  if (_match_19662) {
    return (c <= ('Z'));
  }
  {
    return false;
  }
}

bool kk_std_core_is_alpha(kk_char_t c, kk_context_t* _ctx); /* (c : char) -> bool */ 
 
// Is the character an ASCII digit ?

static inline bool kk_std_core_is_digit(kk_char_t c, kk_context_t* _ctx) { /* (c : char) -> bool */ 
  bool _match_19657 = (c >= ('0')); /*bool*/;
  if (_match_19657) {
    return (c <= ('9'));
  }
  {
    return false;
  }
}
 
// Is the character ASCII letter or digit?

static inline bool kk_std_core_is_alpha_num(kk_char_t c, kk_context_t* _ctx) { /* (c : char) -> bool */ 
  bool _match_19655 = kk_std_core_is_alpha(c, _ctx); /*bool*/;
  if (_match_19655) {
    return true;
  }
  {
    bool _match_19656 = (c >= ('0')); /*bool*/;
    if (_match_19656) {
      return (c <= ('9'));
    }
    {
      return false;
    }
  }
}
 
// Is the character an ASCII character, e.g. `c <= '\x7F'`  ?

static inline bool kk_std_core_is_ascii(kk_char_t c, kk_context_t* _ctx) { /* (c : char) -> bool */ 
  return (c <= 0x007F);
}
 
// Is the character an ASCII control character, e.g. `c < ' '`  ?

static inline bool kk_std_core_is_control(kk_char_t c, kk_context_t* _ctx) { /* (c : char) -> bool */ 
  return (c < (' '));
}

bool kk_std_core_is_hex_digit(kk_char_t c, kk_context_t* _ctx); /* (c : char) -> bool */ 
 
// Is a slice not empty?

static inline bool kk_std_core_is_notempty(kk_std_core__sslice slice0, kk_context_t* _ctx) { /* (slice : sslice) -> bool */ 
  kk_ssize_t _x20923;
  {
    kk_ssize_t _x = slice0.len;
    kk_std_core__sslice_drop(slice0, _ctx);
    _x20923 = _x; /*ssize_t*/
  }
  return (_x20923 > 0);
}
 
// Is a string not empty?

static inline bool kk_std_core_is_notempty_1(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> bool */ 
  kk_string_t _x20924 = kk_string_empty(); /*string*/
  return kk_string_is_neq(s,_x20924,kk_context());
}
 
// Is a slice invalid?

static inline bool kk_std_core_is_valid(kk_std_core__sslice slice0, kk_context_t* _ctx) { /* (slice : sslice) -> bool */ 
  kk_ssize_t _x20926;
  {
    kk_ssize_t _x = slice0.start;
    kk_std_core__sslice_drop(slice0, _ctx);
    _x20926 = _x; /*ssize_t*/
  }
  kk_ssize_t _x20927 = (KK_IZ(0)); /*ssize_t*/
  return (_x20926 >= _x20927);
}

bool kk_std_core_is_white(kk_char_t c, kk_context_t* _ctx); /* (c : char) -> bool */ 

kk_string_t kk_std_core_join_end(kk_std_core__list xs, kk_string_t end, kk_context_t* _ctx); /* (xs : list<string>, end : string) -> string */ 

kk_std_core_types__maybe kk_std_core_last(kk_std_core__list xs, kk_context_t* _ctx); /* forall<a> (xs : list<a>) -> maybe<a> */ 

kk_box_t kk_std_core_last_1(kk_std_core__list xs, kk_box_t default0, kk_context_t* _ctx); /* forall<a> (xs : list<a>, default : a) -> a */ 

kk_std_core__sslice kk_std_core_last_2(kk_string_t s, kk_std_core_types__optional n, kk_context_t* _ctx); /* (s : string, n : optional<int>) -> sslice */ 

kk_std_core__list kk_std_core__ctail_take(kk_std_core__list xs, kk_integer_t n, kk_std_core_types__ctail _acc, kk_context_t* _ctx); /* forall<a> (xs : list<a>, n : int, ctail<list<a>>) -> list<a> */ 

kk_std_core__list kk_std_core_take(kk_std_core__list xs0, kk_integer_t n0, kk_context_t* _ctx); /* forall<a> (xs : list<a>, n : int) -> list<a> */ 

kk_std_core_types__tuple2_ kk_std_core_split(kk_std_core__list xs, kk_integer_t n, kk_context_t* _ctx); /* forall<a> (xs : list<a>, n : int) -> (list<a>, list<a>) */ 
 
// Split a string into parts that were delimited by `sep`. The delimeters are not included in the results.
// For example: `split("1,,2",",") == ["1","","2]`

static inline kk_std_core__list kk_std_core_split_1(kk_string_t s, kk_string_t sep, kk_context_t* _ctx) { /* (s : string, sep : string) -> list<string> */ 
  kk_vector_t v_17122 = kk_string_splitv(s,sep,kk_context()); /*vector<string>*/;
  return kk_std_core_vlist(v_17122, kk_std_core_types__new_None(_ctx), _ctx);
}
 
// Split a string into at most `n` parts that were delimited by a string `sep`. The delimeters are not included in the results (except for possibly the final part).
// For example: `split("1,2,3",",",2) == ["1","2,3"]`

static inline kk_std_core__list kk_std_core_split_2(kk_string_t s, kk_string_t sep, kk_integer_t n, kk_context_t* _ctx) { /* (s : string, sep : string, n : int) -> list<string> */ 
  kk_vector_t v_17123;
  kk_ssize_t _x20950;
  kk_integer_t _x20951 = kk_integer_dup(n); /*int*/
  _x20950 = kk_std_core_ssize__t(_x20951, _ctx); /*ssize_t*/
  v_17123 = kk_string_splitv_atmost(s,sep,_x20950,kk_context()); /*vector<string>*/
  return kk_std_core_vlist(v_17123, kk_std_core_types__new_None(_ctx), _ctx);
}
 
// Split a string into a list of lines

static inline kk_std_core__list kk_std_core_lines(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> list<string> */ 
  kk_vector_t v_17126;
  kk_string_t _x20952;
  kk_define_string_literal(, _s20953, 1, "\n")
  _x20952 = kk_string_dup(_s20953); /*string*/
  v_17126 = kk_string_splitv(s,_x20952,kk_context()); /*vector<string>*/
  return kk_std_core_vlist(v_17126, kk_std_core_types__new_None(_ctx), _ctx);
}

kk_std_core_types__maybe kk_std_core_lookup(kk_std_core__list xs, kk_function_t pred, kk_context_t* _ctx); /* forall<a,b> (xs : list<(a, b)>, pred : (a) -> bool) -> maybe<b> */ 

kk_std_core__list kk_std_core__mlift17722_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_integer_t i0_17228, kk_std_core__list yy, kk_box_t _ctail_17292, kk_context_t* _ctx); /* forall<a,b,e> (ctail<list<b>>, f : (idx : int, value : a) -> e b, i0.17228 : int, yy : list<a>, b) -> e list<b> */ 

kk_std_core__list kk_std_core__mlift17723_op(kk_function_t _accm, kk_function_t f0, kk_integer_t i0_172280, kk_std_core__list yy0, kk_box_t _ctail_17297, kk_context_t* _ctx); /* forall<a,b,e> ((list<b>) -> list<b>, f : (idx : int, value : a) -> e b, i0.17228 : int, yy : list<a>, b) -> e list<b> */ 

kk_std_core__list kk_std_core__ctail_lift17198_map_indexed(kk_function_t f1, kk_std_core__list ys, kk_integer_t i, kk_std_core_types__ctail _acc0, kk_context_t* _ctx); /* forall<a,b,e> (f : (idx : int, value : a) -> e b, ys : list<a>, i : int, ctail<list<b>>) -> e list<b> */ 

kk_std_core__list kk_std_core__ctailm_lift17198_map_indexed(kk_function_t f2, kk_std_core__list ys0, kk_integer_t i0, kk_function_t _accm0, kk_context_t* _ctx); /* forall<a,b,e> (f : (idx : int, value : a) -> e b, ys : list<a>, i : int, (list<b>) -> list<b>) -> e list<b> */ 

kk_std_core__list kk_std_core__lift17198_map_indexed(kk_function_t f3, kk_std_core__list ys1, kk_integer_t i1, kk_context_t* _ctx); /* forall<a,b,e> (f : (idx : int, value : a) -> e b, ys : list<a>, i : int) -> e list<b> */ 
 
// Apply a function `f`  to each element of the input list in sequence where takes
// both the index of the current element and the element itself as arguments.

static inline kk_std_core__list kk_std_core_map_indexed(kk_std_core__list xs, kk_function_t f, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, f : (idx : int, value : a) -> e b) -> e list<b> */ 
  return kk_std_core__lift17198_map_indexed(f, xs, kk_integer_from_small(0), _ctx);
}

kk_std_core__list kk_std_core__mlift17724_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_integer_t i0_17230, kk_std_core__list yy, kk_box_t _ctail_17298, kk_context_t* _ctx); /* forall<a,b,e> (ctail<list<b>>, f : (idx : int, value : a, rest : list<a>) -> e b, i0.17230 : int, yy : list<a>, b) -> e list<b> */ 

kk_std_core__list kk_std_core__mlift17725_op(kk_function_t _accm, kk_function_t f0, kk_integer_t i0_172300, kk_std_core__list yy0, kk_box_t _ctail_17303, kk_context_t* _ctx); /* forall<a,b,e> ((list<b>) -> list<b>, f : (idx : int, value : a, rest : list<a>) -> e b, i0.17230 : int, yy : list<a>, b) -> e list<b> */ 

kk_std_core__list kk_std_core__ctail_lift17199_map_indexed_peek(kk_function_t f1, kk_std_core__list ys, kk_integer_t i, kk_std_core_types__ctail _acc0, kk_context_t* _ctx); /* forall<a,b,e> (f : (idx : int, value : a, rest : list<a>) -> e b, ys : list<a>, i : int, ctail<list<b>>) -> e list<b> */ 

kk_std_core__list kk_std_core__ctailm_lift17199_map_indexed_peek(kk_function_t f2, kk_std_core__list ys0, kk_integer_t i0, kk_function_t _accm0, kk_context_t* _ctx); /* forall<a,b,e> (f : (idx : int, value : a, rest : list<a>) -> e b, ys : list<a>, i : int, (list<b>) -> list<b>) -> e list<b> */ 

kk_std_core__list kk_std_core__lift17199_map_indexed_peek(kk_function_t f3, kk_std_core__list ys1, kk_integer_t i1, kk_context_t* _ctx); /* forall<a,b,e> (f : (idx : int, value : a, rest : list<a>) -> e b, ys : list<a>, i : int) -> e list<b> */ 
 
// Apply a function `f`  to each element of the input list in sequence where takes
// both the index of the current element, the element itself, and the tail list as arguments.

static inline kk_std_core__list kk_std_core_map_indexed_peek(kk_std_core__list xs, kk_function_t f, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, f : (idx : int, value : a, rest : list<a>) -> e b) -> e list<b> */ 
  return kk_std_core__lift17199_map_indexed_peek(f, xs, kk_integer_from_small(0), _ctx);
}

kk_std_core__list kk_std_core__mlift17726_op(kk_std_core_types__ctail _acc, kk_function_t action, kk_std_core__list xx, kk_std_core_types__maybe _y_17557, kk_context_t* _ctx); /* forall<a,b,e> (ctail<list<b>>, action : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e list<b> */ 

kk_std_core__list kk_std_core__mlift17727_op(kk_function_t _accm, kk_function_t action0, kk_std_core__list xx0, kk_std_core_types__maybe _y_17561, kk_context_t* _ctx); /* forall<a,b,e> ((list<b>) -> list<b>, action : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e list<b> */ 

kk_std_core__list kk_std_core__ctail_map_while(kk_std_core__list xs, kk_function_t action1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx); /* forall<a,b,e> (xs : list<a>, action : (a) -> e maybe<b>, ctail<list<b>>) -> e list<b> */ 

kk_std_core__list kk_std_core__ctailm_map_while(kk_std_core__list xs0, kk_function_t action2, kk_function_t _accm0, kk_context_t* _ctx); /* forall<a,b,e> (xs : list<a>, action : (a) -> e maybe<b>, (list<b>) -> list<b>) -> e list<b> */ 

kk_std_core__list kk_std_core_map_while(kk_std_core__list xs1, kk_function_t action3, kk_context_t* _ctx); /* forall<a,b,e> (xs : list<a>, action : (a) -> e maybe<b>) -> e list<b> */ 
 
// Return the maximum of two integers

static inline kk_integer_t kk_std_core_max(kk_integer_t i, kk_integer_t j, kk_context_t* _ctx) { /* (i : int, j : int) -> int */ 
  bool _match_19621 = kk_integer_gte_borrow(i,j,kk_context()); /*bool*/;
  if (_match_19621) {
    kk_integer_drop(j, _ctx);
    return i;
  }
  {
    kk_integer_drop(i, _ctx);
    return j;
  }
}
 
// Returns the largest of two floats

static inline double kk_std_core_max_1(double x, double y, kk_context_t* _ctx) { /* (x : float64, y : float64) -> float64 */ 
  bool _match_19620 = (x >= y); /*bool*/;
  if (_match_19620) {
    return x;
  }
  {
    return y;
  }
}

kk_integer_t kk_std_core_maximum(kk_std_core__list xs, kk_std_core_types__optional default0, kk_context_t* _ctx); /* (xs : list<int>, default : optional<int>) -> int */ 

double kk_std_core_maximum_1(kk_std_core__list xs, kk_context_t* _ctx); /* (xs : list<float64>) -> float64 */ 
 
// Match a `:maybe` value and either return a default value on `Nothing` or apply a function to the value on `Just`

static inline kk_box_t kk_std_core_maybe(kk_std_core_types__maybe m, kk_box_t onNothing, kk_function_t onJust, kk_context_t* _ctx) { /* forall<a,b,e> (m : maybe<a>, onNothing : b, onJust : (a) -> e b) -> e b */ 
  if (kk_std_core_types__is_Nothing(m)) {
    kk_function_drop(onJust, _ctx);
    return onNothing;
  }
  {
    kk_box_t x = m._cons.Just.value;
    kk_box_drop(onNothing, _ctx);
    return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), onJust, (onJust, x, _ctx));
  }
}
 
// Convert a `:maybe<a>` value to `:a`, using the `nothing` parameter for `Nothing`.
// This is an alias for `default`.

static inline kk_box_t kk_std_core_maybe_1(kk_std_core_types__maybe m, kk_box_t nothing, kk_context_t* _ctx) { /* forall<a> (m : maybe<a>, nothing : a) -> a */ 
  if (kk_std_core_types__is_Nothing(m)) {
    return nothing;
  }
  {
    kk_box_t x = m._cons.Just.value;
    kk_box_drop(nothing, _ctx);
    return x;
  }
}
 
// Convert a `:either` to a `:maybe` type discarding the value of the `Left` constructor
// and using `Just` for the `Right` constructor.

static inline kk_std_core_types__maybe kk_std_core_maybe_2(kk_std_core_types__either e, kk_context_t* _ctx) { /* forall<a,b> (e : either<a,b>) -> maybe<b> */ 
  if (kk_std_core_types__is_Left(e)) {
    kk_std_core_types__either_drop(e, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  {
    kk_box_t x = e._cons.Right.right;
    return kk_std_core_types__new_Just(x, _ctx);
  }
}

kk_std_core_types__maybe kk_std_core_maybe_3(kk_std_core__list xs, kk_context_t* _ctx); /* forall<a> (xs : list<a>) -> maybe<a> */ 
 
// Transform a boolean to a maybe type, using `Nothing` for `False`

static inline kk_std_core_types__maybe kk_std_core_maybe_4(bool b, kk_context_t* _ctx) { /* (b : bool) -> maybe<()> */ 
  if (b) {
    return kk_std_core_types__new_Just(kk_unit_box(kk_Unit), _ctx);
  }
  {
    return kk_std_core_types__new_Nothing(_ctx);
  }
}
 
// Transform an integer to a maybe type, using `Nothing` for `0`

static inline kk_std_core_types__maybe kk_std_core_maybe_5(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> maybe<int> */ 
  bool _match_19619 = kk_integer_eq_borrow(i,(kk_integer_from_small(0)),kk_context()); /*bool*/;
  if (_match_19619) {
    kk_integer_drop(i, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  {
    return kk_std_core_types__new_Just(kk_integer_box(i), _ctx);
  }
}

kk_std_core_types__maybe kk_std_core_maybe_6(kk_string_t s, kk_context_t* _ctx); /* (s : string) -> maybe<string> */ 
 
// Transform an `:error` type to a `:maybe` value.

static inline kk_std_core_types__maybe kk_std_core_maybe_7(kk_std_core__error t, kk_context_t* _ctx) { /* forall<a> (t : error<a>) -> maybe<a> */ 
  if (kk_std_core__is_Error(t)) {
    kk_std_core__exception _pat0 = t._cons.Error.exception;
    kk_std_core__error_drop(t, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  {
    kk_box_t x = t._cons.Ok.result;
    return kk_std_core_types__new_Just(x, _ctx);
  }
}

kk_std_core_types__maybe kk_std_core_maybe_8(kk_std_core__null n, kk_context_t* _ctx); /* forall<a> (n : null<a>) -> maybe<a> */ 
 
// Return the minimum of two integers

static inline kk_integer_t kk_std_core_min(kk_integer_t i, kk_integer_t j, kk_context_t* _ctx) { /* (i : int, j : int) -> int */ 
  bool _match_19617 = kk_integer_lte_borrow(i,j,kk_context()); /*bool*/;
  if (_match_19617) {
    kk_integer_drop(j, _ctx);
    return i;
  }
  {
    kk_integer_drop(i, _ctx);
    return j;
  }
}
 
// Returns the smallest of two floats

static inline double kk_std_core_min_1(double x, double y, kk_context_t* _ctx) { /* (x : float64, y : float64) -> float64 */ 
  bool _match_19616 = (x <= y); /*bool*/;
  if (_match_19616) {
    return x;
  }
  {
    return y;
  }
}

kk_integer_t kk_std_core_minimum(kk_std_core__list xs, kk_std_core_types__optional default0, kk_context_t* _ctx); /* (xs : list<int>, default : optional<int>) -> int */ 

double kk_std_core_minimum_1(kk_std_core__list xs, kk_context_t* _ctx); /* (xs : list<float64>) -> float64 */ 

kk_unit_t kk_std_core_notrace(kk_context_t* _ctx); /* () -> (st<global>) () */ 

kk_std_core__null kk_std_core_null(kk_std_core_types__maybe x, kk_context_t* _ctx); /* forall<a> (x : maybe<a>) -> null<a> */ 

kk_std_core__null kk_std_core_null_1(kk_integer_t i, kk_context_t* _ctx); /* (i : int) -> null<int> */ 

kk_std_core__null kk_std_core_null_2(kk_string_t s, kk_context_t* _ctx); /* (s : string) -> null<string> */ 
 
// Cast a boolean `False` to null

static inline kk_std_core__null kk_std_core_null_3(bool b, kk_context_t* _ctx) { /* (b : bool) -> null<()> */ 
  kk_std_core_types__maybe _x21065;
  if (b) {
    _x21065 = kk_std_core_types__new_Just(kk_unit_box(kk_Unit), _ctx); /*maybe<1034>*/
  }
  else {
    _x21065 = kk_std_core_types__new_Nothing(_ctx); /*maybe<1034>*/
  }
  return kk_std_core_null(_x21065, _ctx);
}

kk_string_t kk_std_core_pad_right(kk_string_t s, kk_integer_t width, kk_std_core_types__optional fill, kk_context_t* _ctx); /* (s : string, width : int, fill : optional<char>) -> string */ 

kk_std_core_types__maybe kk_std_core_starts_with(kk_string_t s, kk_string_t pre, kk_context_t* _ctx); /* (s : string, pre : string) -> maybe<sslice> */ 

kk_string_t kk_std_core_trim_left_1(kk_string_t s, kk_string_t sub, kk_context_t* _ctx); /* (s : string, sub : string) -> string */ 

kk_string_t kk_std_core_trim_right_1(kk_string_t s, kk_string_t sub, kk_context_t* _ctx); /* (s : string, sub : string) -> string */ 
 
// Trim whitespace on the left and right side of a string

static inline kk_string_t kk_std_core_trim(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> string */ 
  kk_string_t _x21095 = kk_string_trim_left(s,kk_context()); /*string*/
  return kk_string_trim_right(_x21095,kk_context());
}
 
// Parse an integer after trimming whitespace.
// If an illegal digit character is encountered `Nothing` is returned.
// An empty string will result in `Just(0)`.
// A string can start with a `-` sign for negative numbers,
// and with `0x` or `0X` for hexadecimal numbers (in which case the `hex` parameter is ignored).

static inline kk_std_core_types__maybe kk_std_core_parse_int(kk_string_t s, kk_std_core_types__optional hex, kk_context_t* _ctx) { /* (s : string, hex : optional<bool>) -> maybe<int> */ 
  kk_string_t _x21096;
  kk_string_t _x21097 = kk_string_trim_left(s,kk_context()); /*string*/
  _x21096 = kk_string_trim_right(_x21097,kk_context()); /*string*/
  bool _x21098;
  if (kk_std_core_types__is_Optional(hex)) {
    kk_box_t _box_x19293 = hex._cons.Optional.value;
    bool _hex_15544 = kk_bool_unbox(_box_x19293);
    _x21098 = _hex_15544; /*bool*/
  }
  else {
    _x21098 = false; /*bool*/
  }
  return kk_std_core_xparse_int(_x21096, _x21098, _ctx);
}

kk_integer_t kk_std_core_parse_int_default(kk_string_t s, kk_std_core_types__optional default0, kk_std_core_types__optional hex, kk_context_t* _ctx); /* (s : string, default : optional<int>, hex : optional<bool>) -> int */ 

kk_std_core_types__tuple2_ kk_std_core__mlift17728_partition_acc(kk_std_core__list acc1, kk_std_core__list acc2, kk_function_t pred, kk_box_t x, kk_std_core__list xx, bool _y_17570, kk_context_t* _ctx); /* forall<a,e> (acc1 : list<a>, acc2 : list<a>, pred : (a) -> e bool, x : a, xx : list<a>, bool) -> e (list<a>, list<a>) */ 

kk_std_core_types__tuple2_ kk_std_core_partition_acc(kk_std_core__list xs, kk_function_t pred0, kk_std_core__list acc10, kk_std_core__list acc20, kk_context_t* _ctx); /* forall<a,e> (xs : list<a>, pred : (a) -> e bool, acc1 : list<a>, acc2 : list<a>) -> e (list<a>, list<a>) */ 
 
// Partition a list in two lists where the first list contains
// those elements that satisfy the given predicate `pred`.
// For example: `partition([1,2,3],odd?) == ([1,3],[2])`

static inline kk_std_core_types__tuple2_ kk_std_core_partition(kk_std_core__list xs, kk_function_t pred, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, pred : (a) -> e bool) -> e (list<a>, list<a>) */ 
  return kk_std_core_partition_acc(xs, pred, kk_std_core__new_Nil(_ctx), kk_std_core__new_Nil(_ctx), _ctx);
}

kk_unit_t kk_std_core_print_redirect(kk_function_t print0, kk_context_t* _ctx); /* (print : (msg : string) -> console ()) -> io () */ 
 
// Remove those elements of a list that satisfy the given predicate `pred`.
// For example: `remove([1,2,3],odd?) == [2]`


// lift anonymous function
struct kk_std_core_remove_fun21125__t {
  struct kk_function_s _base;
  kk_function_t pred;
};
extern bool kk_std_core_remove_fun21125(kk_function_t _fself, kk_box_t x, kk_context_t* _ctx);
static inline kk_function_t kk_std_core_new_remove_fun21125(kk_function_t pred, kk_context_t* _ctx) {
  struct kk_std_core_remove_fun21125__t* _self = kk_function_alloc_as(struct kk_std_core_remove_fun21125__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_remove_fun21125, kk_context());
  _self->pred = pred;
  return &_self->_base;
}


static inline kk_std_core__list kk_std_core_remove(kk_std_core__list xs, kk_function_t pred, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, pred : (a) -> bool) -> list<a> */ 
  return kk_std_core_filter(xs, kk_std_core_new_remove_fun21125(pred, _ctx), _ctx);
}
 
// Repeat a string `n` times

static inline kk_string_t kk_std_core_repeat(kk_string_t s, kk_integer_t n, kk_context_t* _ctx) { /* (s : string, n : int) -> string */ 
  kk_ssize_t _x21126;
  kk_integer_t _x21127 = kk_integer_dup(n); /*int*/
  _x21126 = kk_std_core_ssize__t(_x21127, _ctx); /*ssize_t*/
  return kk_std_core_repeatz(s, _x21126, _ctx);
}
 
// The `repeat` fun executes `action`  `n`  times.


// lift anonymous function
struct kk_std_core_repeat_fun21128__t_1 {
  struct kk_function_s _base;
  kk_function_t action;
};
extern kk_unit_t kk_std_core_repeat_fun21128_1(kk_function_t _fself, kk_integer_t i, kk_context_t* _ctx);
static inline kk_function_t kk_std_core_new_repeat_fun21128_1(kk_function_t action, kk_context_t* _ctx) {
  struct kk_std_core_repeat_fun21128__t_1* _self = kk_function_alloc_as(struct kk_std_core_repeat_fun21128__t_1, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_repeat_fun21128_1, kk_context());
  _self->action = action;
  return &_self->_base;
}


static inline kk_unit_t kk_std_core_repeat_1(kk_integer_t n, kk_function_t action, kk_context_t* _ctx) { /* forall<e> (n : int, action : () -> e ()) -> e () */ 
  kk_integer_t _x21129 = kk_integer_dup(n); /*int*/
  kk_std_core__lift17196_for(kk_std_core_new_repeat_fun21128_1(action, _ctx), _x21129, kk_integer_from_small(1), _ctx); return kk_Unit;
}

kk_std_core__list kk_std_core__ctail_replicate(kk_box_t x, kk_integer_t n, kk_std_core_types__ctail _acc, kk_context_t* _ctx); /* forall<a> (x : a, n : int, ctail<list<a>>) -> list<a> */ 

kk_std_core__list kk_std_core_replicate(kk_box_t x0, kk_integer_t n0, kk_context_t* _ctx); /* forall<a> (x : a, n : int) -> list<a> */ 

kk_std_core__list kk_std_core__lift17200_reverse_join(kk_std_core__list acc, kk_std_core__list ys, kk_context_t* _ctx); /* forall<a> (acc : list<a>, ys : list<a>) -> list<a> */ 

kk_string_t kk_std_core__lift17201_reverse_join(kk_std_core__list ys0, kk_string_t acc0, kk_context_t* _ctx); /* (ys0 : list<string>, acc0 : string) -> string */ 

kk_string_t kk_std_core_reverse_join(kk_std_core__list xs, kk_context_t* _ctx); /* (xs : list<string>) -> string */ 

kk_string_t kk_std_core_show_tuple(kk_std_core_types__tuple2_ x, kk_function_t showfst, kk_function_t showsnd, kk_context_t* _ctx); /* forall<a,b> (x : (a, b), showfst : (a) -> string, showsnd : (b) -> string) -> string */ 

kk_std_core_types__tuple2_ kk_std_core__mlift17729_op(kk_std_core__list acc, kk_function_t predicate, kk_box_t y, kk_std_core__list ys, kk_std_core__list yy, bool _y_17578, kk_context_t* _ctx); /* forall<a,e> (acc : list<a>, predicate : (a) -> e bool, y : a, ys : list<a>, yy : list<a>, bool) -> e (list<a>, list<a>) */ 

kk_std_core_types__tuple2_ kk_std_core__lift17202_span(kk_function_t predicate0, kk_std_core__list ys0, kk_std_core__list acc0, kk_context_t* _ctx); /* forall<a,e> (predicate : (a) -> e bool, ys : list<a>, acc : list<a>) -> e (list<a>, list<a>) */ 

static inline kk_std_core_types__tuple2_ kk_std_core_span(kk_std_core__list xs, kk_function_t predicate, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, predicate : (a) -> e bool) -> e (list<a>, list<a>) */ 
  return kk_std_core__lift17202_span(predicate, xs, kk_std_core__new_Nil(_ctx), _ctx);
}

kk_integer_t kk_std_core_sum(kk_std_core__list xs, kk_context_t* _ctx); /* (xs : list<int>) -> int */ 

kk_std_core__list kk_std_core_tail_1(kk_std_core__list xs, kk_context_t* _ctx); /* forall<a> (xs : list<a>) -> list<a> */ 

kk_string_t kk_std_core_tail_2(kk_string_t s, kk_context_t* _ctx); /* (s : string) -> string */ 

kk_std_core__list kk_std_core__mlift17730_op(kk_std_core_types__ctail _acc, kk_function_t predicate, kk_box_t x, kk_std_core__list xx, bool _y_17583, kk_context_t* _ctx); /* forall<a,e> (ctail<list<a>>, predicate : (a) -> e bool, x : a, xx : list<a>, bool) -> e list<a> */ 

kk_std_core__list kk_std_core__mlift17731_op(kk_function_t _accm, kk_function_t predicate0, kk_box_t x0, kk_std_core__list xx0, bool _y_17587, kk_context_t* _ctx); /* forall<a,e> ((list<a>) -> list<a>, predicate : (a) -> e bool, x : a, xx : list<a>, bool) -> e list<a> */ 

kk_std_core__list kk_std_core__ctail_take_while(kk_std_core__list xs, kk_function_t predicate1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx); /* forall<a,e> (xs : list<a>, predicate : (a) -> e bool, ctail<list<a>>) -> e list<a> */ 

kk_std_core__list kk_std_core__ctailm_take_while(kk_std_core__list xs0, kk_function_t predicate2, kk_function_t _accm0, kk_context_t* _ctx); /* forall<a,e> (xs : list<a>, predicate : (a) -> e bool, (list<a>) -> list<a>) -> e list<a> */ 

kk_std_core__list kk_std_core_take_while(kk_std_core__list xs1, kk_function_t predicate3, kk_context_t* _ctx); /* forall<a,e> (xs : list<a>, predicate : (a) -> e bool) -> e list<a> */ 
 
// monadic lift

static inline kk_unit_t kk_std_core__mlift17732_trace(kk_string_t message0, bool _y_17594, kk_context_t* _ctx) { /* forall<_e> (message0 : string, bool) -> <read<global>|_e> () */ 
  if (_y_17594) {
    kk_std_core_xtrace(message0, _ctx); return kk_Unit;
  }
  {
    kk_string_drop(message0, _ctx);
    kk_Unit; return kk_Unit;
  }
}

kk_unit_t kk_std_core_trace(kk_string_t message0, kk_context_t* _ctx); /* (message : string) -> () */ 
 
// monadic lift

static inline kk_unit_t kk_std_core__mlift17733_trace_any(kk_string_t message0, kk_box_t x, bool _y_17596, kk_context_t* _ctx) { /* forall<_e,a> (message0 : string, x : a, bool) -> <read<global>|_e> () */ 
  if (_y_17596) {
    kk_std_core_xtrace_any(message0, x, _ctx); return kk_Unit;
  }
  {
    kk_box_drop(x, _ctx);
    kk_string_drop(message0, _ctx);
    kk_Unit; return kk_Unit;
  }
}

kk_unit_t kk_std_core_trace_any(kk_string_t message0, kk_box_t x, kk_context_t* _ctx); /* forall<a> (message : string, x : a) -> () */ 

kk_string_t kk_std_core_truncate(kk_string_t s, kk_integer_t count, kk_context_t* _ctx); /* (s : string, count : int) -> string */ 

kk_box_t kk_std_core_try_default(kk_box_t value, kk_function_t action, kk_context_t* _ctx); /* forall<a,e> (value : a, action : () -> <exn|e> a) -> e a */ 
 
// monadic lift

static inline kk_integer_t kk_std_core__mlift17734_unique(kk_integer_t u, kk_context_t* _ctx) { /* forall<_e> (u : int) -> <read<global>,write<global>|_e> int */ 
  kk_integer_t _b_19427_19425;
  kk_integer_t _x21231 = kk_integer_dup(u); /*int*/
  _b_19427_19425 = kk_integer_add_small_const(_x21231, 1, _ctx); /*int*/
  kk_unit_t __ = kk_Unit;
  kk_ref_set_borrow(kk_std_core_unique_count,(kk_integer_box(_b_19427_19425)),kk_context());
  return u;
}

kk_integer_t kk_std_core_unique(kk_context_t* _ctx); /* () -> ndet int */ 

kk_box_t kk_std_core_unjust(kk_std_core_types__maybe m, kk_context_t* _ctx); /* forall<a> (m : maybe<a>) -> exn a */ 

kk_string_t kk_std_core__lift17203_unlines(kk_std_core__list ys, kk_string_t acc, kk_context_t* _ctx); /* (ys : list<string>, acc : string) -> string */ 

kk_string_t kk_std_core_unlines(kk_std_core__list xs, kk_context_t* _ctx); /* (xs : list<string>) -> string */ 

kk_std_core__list kk_std_core__lift17205_unzip(kk_std_core__list acc, kk_std_core__list ys0, kk_context_t* _ctx); /* forall<a> (acc : list<a>, ys0 : list<a>) -> list<a> */ 

kk_std_core__list kk_std_core__lift17206_unzip(kk_std_core__list acc0, kk_std_core__list ys1, kk_context_t* _ctx); /* forall<a> (acc0 : list<a>, ys1 : list<a>) -> list<a> */ 

kk_std_core_types__tuple2_ kk_std_core__lift17204_unzip(kk_std_core__list ys, kk_std_core__list acc1, kk_std_core__list acc2, kk_context_t* _ctx); /* forall<a,b> (ys : list<(a, b)>, acc1 : list<a>, acc2 : list<b>) -> (list<a>, list<b>) */ 
 
// Unzip a list of pairs into two lists

static inline kk_std_core_types__tuple2_ kk_std_core_unzip(kk_std_core__list xs, kk_context_t* _ctx) { /* forall<a,b> (xs : list<(a, b)>) -> (list<a>, list<b>) */ 
  return kk_std_core__lift17204_unzip(xs, kk_std_core__new_Nil(_ctx), kk_std_core__new_Nil(_ctx), _ctx);
}

kk_vector_t kk_std_core_vector_1(kk_string_t s, kk_context_t* _ctx); /* (s : string) -> vector<char> */ 
 
// Create a new vector of length `n`  with initial elements `default` .


// lift anonymous function
struct kk_std_core_vector_fun21269__t_2 {
  struct kk_function_s _base;
  kk_box_t default0;
};
extern kk_box_t kk_std_core_vector_fun21269_2(kk_function_t _fself, kk_ssize_t ___wildcard__1854__30, kk_context_t* _ctx);
static inline kk_function_t kk_std_core_new_vector_fun21269_2(kk_box_t default0, kk_context_t* _ctx) {
  struct kk_std_core_vector_fun21269__t_2* _self = kk_function_alloc_as(struct kk_std_core_vector_fun21269__t_2, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_vector_fun21269_2, kk_context());
  _self->default0 = default0;
  return &_self->_base;
}


static inline kk_vector_t kk_std_core_vector_2(kk_integer_t n, kk_box_t default0, kk_context_t* _ctx) { /* forall<a> (n : int, default : a) -> vector<a> */ 
  kk_ssize_t _x21267;
  kk_integer_t _x21268 = kk_integer_dup(n); /*int*/
  _x21267 = kk_std_core_ssize__t(_x21268, _ctx); /*ssize_t*/
  return kk_std_core_vector_initz(_x21267, kk_std_core_new_vector_fun21269_2(default0, _ctx), _ctx);
}
 
// Convert a list to a vector.

static inline kk_vector_t kk_std_core_vector_3(kk_std_core__list xs, kk_context_t* _ctx) { /* forall<a> (xs : list<a>) -> vector<a> */ 
  return kk_std_core_unvlist(xs, _ctx);
}
 
// Create a new vector of length `n`  with initial elements given by function `f` .


// lift anonymous function
struct kk_std_core_vector_init_fun21272__t {
  struct kk_function_s _base;
  kk_function_t f;
};
extern kk_box_t kk_std_core_vector_init_fun21272(kk_function_t _fself, kk_ssize_t i, kk_context_t* _ctx);
static inline kk_function_t kk_std_core_new_vector_init_fun21272(kk_function_t f, kk_context_t* _ctx) {
  struct kk_std_core_vector_init_fun21272__t* _self = kk_function_alloc_as(struct kk_std_core_vector_init_fun21272__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_vector_init_fun21272, kk_context());
  _self->f = f;
  return &_self->_base;
}


static inline kk_vector_t kk_std_core_vector_init(kk_integer_t n, kk_function_t f, kk_context_t* _ctx) { /* forall<a> (n : int, f : (int) -> a) -> vector<a> */ 
  kk_ssize_t _x21270;
  kk_integer_t _x21271 = kk_integer_dup(n); /*int*/
  _x21270 = kk_std_core_ssize__t(_x21271, _ctx); /*ssize_t*/
  return kk_std_core_vector_initz(_x21270, kk_std_core_new_vector_init_fun21272(f, _ctx), _ctx);
}

kk_unit_t kk_std_core__mlift17735_while(kk_function_t action, kk_function_t predicate, kk_unit_t wild__, kk_context_t* _ctx); /* forall<e> (action : () -> <div|e> (), predicate : () -> <div|e> bool, wild_ : ()) -> <div|e> () */ 

kk_unit_t kk_std_core__mlift17736_while(kk_function_t action0, kk_function_t predicate0, bool _y_17602, kk_context_t* _ctx); /* forall<e> (action : () -> <div|e> (), predicate : () -> <div|e> bool, bool) -> <div|e> () */ 

kk_unit_t kk_std_core_while(kk_function_t predicate1, kk_function_t action1, kk_context_t* _ctx); /* forall<e> (predicate : () -> <div|e> bool, action : () -> <div|e> ()) -> <div|e> () */ 

kk_std_core__list kk_std_core__ctail_zip(kk_std_core__list xs, kk_std_core__list ys, kk_std_core_types__ctail _acc, kk_context_t* _ctx); /* forall<a,b> (xs : list<a>, ys : list<b>, ctail<list<(a, b)>>) -> list<(a, b)> */ 

kk_std_core__list kk_std_core_zip(kk_std_core__list xs0, kk_std_core__list ys0, kk_context_t* _ctx); /* forall<a,b> (xs : list<a>, ys : list<b>) -> list<(a, b)> */ 

kk_std_core__list kk_std_core__mlift17737_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_std_core__list xx, kk_std_core__list yy, kk_box_t _ctail_17316, kk_context_t* _ctx); /* forall<a,b,c,e> (ctail<list<c>>, f : (a, b) -> e c, xx : list<a>, yy : list<b>, c) -> e list<c> */ 

kk_std_core__list kk_std_core__mlift17738_op(kk_function_t _accm, kk_function_t f0, kk_std_core__list xx0, kk_std_core__list yy0, kk_box_t _ctail_17321, kk_context_t* _ctx); /* forall<a,b,c,e> ((list<c>) -> list<c>, f : (a, b) -> e c, xx : list<a>, yy : list<b>, c) -> e list<c> */ 

kk_std_core__list kk_std_core__ctail_zipwith(kk_std_core__list xs, kk_std_core__list ys, kk_function_t f1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx); /* forall<a,b,c,e> (xs : list<a>, ys : list<b>, f : (a, b) -> e c, ctail<list<c>>) -> e list<c> */ 

kk_std_core__list kk_std_core__ctailm_zipwith(kk_std_core__list xs0, kk_std_core__list ys0, kk_function_t f2, kk_function_t _accm0, kk_context_t* _ctx); /* forall<a,b,c,e> (xs : list<a>, ys : list<b>, f : (a, b) -> e c, (list<c>) -> list<c>) -> e list<c> */ 

kk_std_core__list kk_std_core_zipwith(kk_std_core__list xs1, kk_std_core__list ys1, kk_function_t f3, kk_context_t* _ctx); /* forall<a,b,c,e> (xs : list<a>, ys : list<b>, f : (a, b) -> e c) -> e list<c> */ 

kk_std_core__list kk_std_core__mlift17739_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_integer_t i0_17233, kk_std_core__list xx, kk_std_core__list yy, kk_box_t _ctail_17322, kk_context_t* _ctx); /* forall<a,b,c,e> (ctail<list<c>>, f : (int, a, b) -> e c, i0.17233 : int, xx : list<a>, yy : list<b>, c) -> e list<c> */ 

kk_std_core__list kk_std_core__mlift17740_op(kk_function_t _accm, kk_function_t f0, kk_integer_t i0_172330, kk_std_core__list xx0, kk_std_core__list yy0, kk_box_t _ctail_17327, kk_context_t* _ctx); /* forall<a,b,c,e> ((list<c>) -> list<c>, f : (int, a, b) -> e c, i0.17233 : int, xx : list<a>, yy : list<b>, c) -> e list<c> */ 

kk_std_core__list kk_std_core__ctail_lift17207_zipwith_indexed(kk_function_t f1, kk_integer_t i, kk_std_core__list xs, kk_std_core__list ys, kk_std_core_types__ctail _acc0, kk_context_t* _ctx); /* forall<a,b,c,e> (f : (int, a, b) -> e c, i : int, xs : list<a>, ys : list<b>, ctail<list<c>>) -> e list<c> */ 

kk_std_core__list kk_std_core__ctailm_lift17207_zipwith_indexed(kk_function_t f2, kk_integer_t i0, kk_std_core__list xs0, kk_std_core__list ys0, kk_function_t _accm0, kk_context_t* _ctx); /* forall<a,b,c,e> (f : (int, a, b) -> e c, i : int, xs : list<a>, ys : list<b>, (list<c>) -> list<c>) -> e list<c> */ 

kk_std_core__list kk_std_core__lift17207_zipwith_indexed(kk_function_t f3, kk_integer_t i1, kk_std_core__list xs1, kk_std_core__list ys1, kk_context_t* _ctx); /* forall<a,b,c,e> (f : (int, a, b) -> e c, i : int, xs : list<a>, ys : list<b>) -> e list<c> */ 
 
// Zip two lists together by apply a function `f` to all corresponding elements
// and their index in the list.
// The returned list is only as long as the smallest input list.

static inline kk_std_core__list kk_std_core_zipwith_indexed(kk_std_core__list xs0, kk_std_core__list ys0, kk_function_t f, kk_context_t* _ctx) { /* forall<a,b,c,e> (xs0 : list<a>, ys0 : list<b>, f : (int, a, b) -> e c) -> e list<c> */ 
  return kk_std_core__lift17207_zipwith_indexed(f, kk_integer_from_small(0), xs0, ys0, _ctx);
}

void kk_std_core__init(kk_context_t* _ctx);


void kk_std_core__done(kk_context_t* _ctx);

#endif // header
