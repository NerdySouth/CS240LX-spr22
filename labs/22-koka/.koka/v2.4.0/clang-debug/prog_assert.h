#pragma once
#ifndef kk_prog_assert_H
#define kk_prog_assert_H
// Koka generated module: "prog/assert", koka version: 2.4.0, platform: 64-bit
#include <kklib.h>
#include "std_core_types.h"
#include "std_core_hnd.h"
#include "std_core.h"

// type declarations

// type prog/assert/.hnd-assert
struct kk_prog_assert__hnd_assert_s {
  kk_block_t _block;
};
typedef struct kk_prog_assert__hnd_assert_s* kk_prog_assert__hnd_assert;
struct kk_prog_assert__Hnd_assert {
  struct kk_prog_assert__hnd_assert_s _base;
  kk_std_core_hnd__clause1 ctl_assert;
};
static inline kk_prog_assert__hnd_assert kk_prog_assert__base_Hnd_assert(struct kk_prog_assert__Hnd_assert* _x) {
  return &_x->_base;
}
static inline kk_prog_assert__hnd_assert kk_prog_assert__new_Hnd_assert(kk_reuse_t _at, kk_std_core_hnd__clause1 ctl_assert, kk_context_t* _ctx) {
  struct kk_prog_assert__Hnd_assert* _con = kk_block_alloc_at_as(struct kk_prog_assert__Hnd_assert, _at, 1 /* scan count */, (kk_tag_t)(1), _ctx);
  _con->ctl_assert = ctl_assert;
  return kk_prog_assert__base_Hnd_assert(_con);
}
static inline struct kk_prog_assert__Hnd_assert* kk_prog_assert__as_Hnd_assert(kk_prog_assert__hnd_assert x) {
  return kk_basetype_as_assert(struct kk_prog_assert__Hnd_assert*, x, (kk_tag_t)(1) /* _tag */);
}
static inline bool kk_prog_assert__is_Hnd_assert(kk_prog_assert__hnd_assert x) {
  return (true);
}
static inline kk_prog_assert__hnd_assert kk_prog_assert__hnd_assert_dup(kk_prog_assert__hnd_assert _x) {
  return kk_basetype_dup_as(kk_prog_assert__hnd_assert, _x);
}
static inline void kk_prog_assert__hnd_assert_drop(kk_prog_assert__hnd_assert _x, kk_context_t* _ctx) {
  kk_basetype_drop(_x, _ctx);
}
static inline kk_prog_assert__hnd_assert kk_prog_assert__hnd_assert_hole() {
  return (kk_prog_assert__hnd_assert)(1);
}
static inline bool kk_prog_assert__hnd_assert_is_unique(kk_prog_assert__hnd_assert _x) {
  return kk_basetype_is_unique(_x);
}
static inline void kk_prog_assert__hnd_assert_free(kk_prog_assert__hnd_assert _x, kk_context_t* _ctx) {
  kk_basetype_free(_x, _ctx);
}
static inline void kk_prog_assert__hnd_assert_decref(kk_prog_assert__hnd_assert _x, kk_context_t* _ctx) {
  kk_basetype_decref(_x, _ctx);
}
static inline kk_reuse_t kk_prog_assert__hnd_assert_dropn_reuse(kk_prog_assert__hnd_assert _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  return kk_basetype_dropn_reuse(_x, _scan_fsize, _ctx);
}
static inline void kk_prog_assert__hnd_assert_dropn(kk_prog_assert__hnd_assert _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  kk_basetype_dropn(_x, _scan_fsize, _ctx);
}
static inline kk_reuse_t kk_prog_assert__hnd_assert_reuse(kk_prog_assert__hnd_assert _x) {
  return kk_basetype_reuse(_x);
}
static inline kk_box_t kk_prog_assert__hnd_assert_box(kk_prog_assert__hnd_assert _x, kk_context_t* _ctx) {
  return kk_basetype_box(_x);
}
static inline kk_prog_assert__hnd_assert kk_prog_assert__hnd_assert_unbox(kk_box_t _x, kk_context_t* _ctx) {
  return kk_basetype_unbox_as(kk_prog_assert__hnd_assert, _x);
}

// type prog/assert/assert
struct kk_prog_assert__assert_s {
  kk_block_t _block;
};
typedef struct kk_prog_assert__assert_s* kk_prog_assert__assert;
struct kk_prog_assert_Assert {
  struct kk_prog_assert__assert_s _base;
  kk_prog_assert__hnd_assert _field1;
};
static inline kk_prog_assert__assert kk_prog_assert__base_Assert(struct kk_prog_assert_Assert* _x) {
  return &_x->_base;
}
static inline kk_prog_assert__assert kk_prog_assert__new_Assert(kk_reuse_t _at, kk_prog_assert__hnd_assert _field1, kk_context_t* _ctx) {
  struct kk_prog_assert_Assert* _con = kk_block_alloc_at_as(struct kk_prog_assert_Assert, _at, 1 /* scan count */, (kk_tag_t)(1), _ctx);
  _con->_field1 = _field1;
  return kk_prog_assert__base_Assert(_con);
}
static inline struct kk_prog_assert_Assert* kk_prog_assert__as_Assert(kk_prog_assert__assert x) {
  return kk_basetype_as_assert(struct kk_prog_assert_Assert*, x, (kk_tag_t)(1) /* _tag */);
}
static inline bool kk_prog_assert__is_Assert(kk_prog_assert__assert x) {
  return (true);
}
static inline kk_prog_assert__assert kk_prog_assert__assert_dup(kk_prog_assert__assert _x) {
  return kk_basetype_dup_as(kk_prog_assert__assert, _x);
}
static inline void kk_prog_assert__assert_drop(kk_prog_assert__assert _x, kk_context_t* _ctx) {
  kk_basetype_drop(_x, _ctx);
}
static inline kk_prog_assert__assert kk_prog_assert__assert_hole() {
  return (kk_prog_assert__assert)(1);
}
static inline bool kk_prog_assert__assert_is_unique(kk_prog_assert__assert _x) {
  return kk_basetype_is_unique(_x);
}
static inline void kk_prog_assert__assert_free(kk_prog_assert__assert _x, kk_context_t* _ctx) {
  kk_basetype_free(_x, _ctx);
}
static inline void kk_prog_assert__assert_decref(kk_prog_assert__assert _x, kk_context_t* _ctx) {
  kk_basetype_decref(_x, _ctx);
}
static inline kk_reuse_t kk_prog_assert__assert_dropn_reuse(kk_prog_assert__assert _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  return kk_basetype_dropn_reuse(_x, _scan_fsize, _ctx);
}
static inline void kk_prog_assert__assert_dropn(kk_prog_assert__assert _x, kk_ssize_t _scan_fsize, kk_context_t* _ctx) {
  kk_basetype_dropn(_x, _scan_fsize, _ctx);
}
static inline kk_reuse_t kk_prog_assert__assert_reuse(kk_prog_assert__assert _x) {
  return kk_basetype_reuse(_x);
}
static inline kk_box_t kk_prog_assert__assert_box(kk_prog_assert__assert _x, kk_context_t* _ctx) {
  return kk_basetype_box(_x);
}
static inline kk_prog_assert__assert kk_prog_assert__assert_unbox(kk_box_t _x, kk_context_t* _ctx) {
  return kk_basetype_unbox_as(kk_prog_assert__assert, _x);
}

// value declarations

extern kk_std_core_hnd__htag kk_prog_assert__tag_assert;

kk_box_t kk_prog_assert__handle_assert(int32_t cfc, kk_prog_assert__hnd_assert hnd, kk_function_t ret, kk_function_t action, kk_context_t* _ctx); /* forall<a,e,b> (cfc : int32, hnd : .hnd-assert<e,b>, ret : (res : a) -> e b, action : () -> <assert|e> a) -> e b */ 
 
// select `assert` operation out of the `:assert` effect handler

static inline kk_std_core_hnd__clause1 kk_prog_assert__select_assert(kk_prog_assert__hnd_assert hnd, kk_context_t* _ctx) { /* forall<e,a> (hnd : .hnd-assert<e,a>) -> std/core/hnd/clause1<bool,(),.hnd-assert,e,a> */ 
  {
    struct kk_prog_assert__Hnd_assert* _con558 = kk_prog_assert__as_Hnd_assert(hnd);
    kk_std_core_hnd__clause1 ctl_assert = _con558->ctl_assert;
    return kk_std_core_hnd__clause1_dup(ctl_assert);
  }
}
 
// call `assert` operation of the `:assert` effect

static inline kk_unit_t kk_prog_assert_assert(bool value, kk_context_t* _ctx) { /* (value : bool) -> assert () */ 
  kk_std_core_hnd__ev ev_425;
  kk_ssize_t _x559 = (KK_IZ(0)); /*ssize_t*/
  ev_425 = kk_evv_at(_x559,kk_context()); /*std/core/hnd/ev<prog/assert/.hnd-assert>*/
  kk_box_t _x560;
  {
    struct kk_std_core_hnd_Ev* _con561 = kk_std_core_hnd__as_Ev(ev_425);
    kk_std_core_hnd__marker m0 = _con561->marker;
    kk_box_t _box_x457 = _con561->hnd;
    kk_prog_assert__hnd_assert h = kk_prog_assert__hnd_assert_unbox(_box_x457, NULL);
    kk_prog_assert__hnd_assert_dup(h);
    kk_std_core_hnd__clause1 _match_553;
    kk_std_core_hnd__clause1 _brw_554 = kk_prog_assert__select_assert(h, _ctx); /*std/core/hnd/clause1<bool,(),prog/assert/.hnd-assert,93,94>*/;
    kk_prog_assert__hnd_assert_drop(h, _ctx);
    _match_553 = _brw_554; /*std/core/hnd/clause1<bool,(),prog/assert/.hnd-assert,93,94>*/
    {
      kk_function_t _fun_unbox_x461 = _match_553.clause;
      _x560 = kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x461, (_fun_unbox_x461, m0, ev_425, kk_bool_box(value), _ctx)); /*1010*/
    }
  }
  kk_unit_unbox(_x560); return kk_Unit;
}

kk_unit_t kk_prog_assert__mlift421_main(kk_unit_t wild__4, kk_context_t* _ctx); /* (wild_4 : ()) -> assert () */ 

kk_unit_t kk_prog_assert__mlift422_main(kk_unit_t wild__3, kk_context_t* _ctx); /* (wild_3 : ()) -> assert () */ 

kk_unit_t kk_prog_assert__mlift423_main(kk_unit_t wild__2, kk_context_t* _ctx); /* (wild_2 : ()) -> assert () */ 

kk_unit_t kk_prog_assert_main(kk_context_t* _ctx); /* () -> console () */ 

void kk_prog_assert__init(kk_context_t* _ctx);


void kk_prog_assert__done(kk_context_t* _ctx);

#endif // header
