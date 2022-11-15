// Koka generated module: "std/core/hnd", koka version: 2.4.0, platform: 64-bit
#include "std_core_hnd.h"
/*---------------------------------------------------------------------------
  Copyright 2020-2021, Microsoft Research, Daan Leijen.

  This is free software; you can redistribute it and/or modify it under the
  terms of the Apache License, Version 2.0. A copy of the License can be
  found in the LICENSE file at the root of this distribution.
---------------------------------------------------------------------------*/
/*
typedef datatype_t kk_std_core_hnd__ev;
struct kk_std_core_hnd_Ev {
  kk_std_core_hnd__htag htag;
  kk_box_t hnd;
  int32_t cfc;  // control flow context
  kk_std_core_hnd__evv hevv;
  kk_std_core_hnd__marker marker;
};
*/

static kk_evv_vector_t kk_evv_vector_alloc(kk_ssize_t length, int32_t cfc, kk_context_t* ctx) {
  kk_assert_internal(length>=0);
  kk_evv_vector_t v = (kk_evv_vector_t)kk_block_alloc(kk_ssizeof(struct kk_evv_vector_s) + (length-1)*kk_ssizeof(void*), length + 1 /* cfc */, KK_TAG_EVV_VECTOR, ctx);
  v->cfc = kk_integer_from_int32(cfc,ctx);
  return v;
}

static kk_std_core_hnd__ev* kk_evv_vector_buf(kk_evv_vector_t vec, kk_ssize_t* len) {
  if (len != NULL) { *len = kk_block_scan_fsize(&vec->_block) - 1; }
  return &vec->vec[0];
}

static kk_std_core_hnd__ev* kk_evv_as_vec(kk_evv_t evv, kk_ssize_t* len, kk_std_core_hnd__ev* single) {
  if (kk_evv_is_vector(evv)) {
    kk_evv_vector_t vec = kk_evv_as_vector(evv);
    *len = kk_block_scan_fsize(&vec->_block) - 1;
    return &vec->vec[0];
  }
  else {
    // single evidence
    *single = kk_evv_as_ev(evv);
    *len = 1;
    return single;
  }
}

kk_std_core_hnd__ev kk_ev_none(kk_context_t* ctx) {
  static kk_std_core_hnd__ev ev_none_singleton;
  if (ev_none_singleton==NULL) {
    ev_none_singleton = kk_std_core_hnd__new_Ev(
      kk_reuse_null,
      kk_std_core_hnd__new_Htag(kk_string_empty(),ctx), // tag ""
      kk_std_core_hnd__new_Marker(0,ctx),               // marker 0
      kk_box_null,                                      // no handler
      -1,                                               // bot
      kk_evv_empty(ctx),
      ctx
    );
  }
  return kk_std_core_hnd__ev_dup(ev_none_singleton);
}


kk_ssize_t kk_evv_index( struct kk_std_core_hnd_Htag htag, kk_context_t* ctx ) {
  // todo: drop htag?
  kk_ssize_t len;
  kk_std_core_hnd__ev single;
  kk_std_core_hnd__ev* vec = kk_evv_as_vec(ctx->evv,&len,&single);
  for(kk_ssize_t i = 0; i < len; i++) {
    struct kk_std_core_hnd_Ev* ev = kk_std_core_hnd__as_Ev(vec[i]);
    if (kk_string_cmp_borrow(htag.tagname,ev->htag.tagname) <= 0) return i; // break on insertion point
  }
  //string_t evvs = kk_evv_show(dup_datatype_as(kk_evv_t,ctx->evv),ctx);
  //fatal_error(EFAULT,"cannot find tag '%s' in: %s", string_cbuf_borrow(htag.htag), string_cbuf_borrow(evvs));
  //drop_string_t(evvs,ctx);
  return len;
}

kk_std_core_hnd__ev kk_evv_lookup( struct kk_std_core_hnd_Htag htag, kk_context_t* ctx ) {
  // todo: drop htag
  kk_ssize_t idx = kk_evv_index(htag,ctx);
  return kk_evv_at(idx,ctx);
}

static inline int32_t kk_cfc_lub(int32_t cfc1, int32_t cfc2) {
  if (cfc1 < 0) return cfc2;
  else if (cfc1+cfc2 == 1) return 2;
  else if (cfc1>cfc2) return cfc1;
  else return cfc2;
}

static inline struct kk_std_core_hnd_Ev* kk_evv_as_Ev( kk_evv_t evv ) {
  return kk_std_core_hnd__as_Ev(kk_evv_as_ev(evv));
}


static int32_t kk_evv_cfc_of_borrow(kk_evv_t evv, kk_context_t* ctx) {
  if (kk_evv_is_vector(evv)) {
    kk_evv_vector_t vec = kk_evv_as_vector(evv);
    return kk_integer_clamp32_borrow(vec->cfc,ctx);
  }
  else {
    struct kk_std_core_hnd_Ev* ev = kk_evv_as_Ev(evv);
    return ev->cfc;
  }
}

int32_t kk_evv_cfc(kk_context_t* ctx) {
  return kk_evv_cfc_of_borrow(ctx->evv,ctx);
}

static void kk_evv_update_cfc_borrow(kk_evv_t evv, int32_t cfc, kk_context_t* ctx) {
  kk_assert_internal(!kk_evv_is_empty(evv)); // should never happen (as named handlers are always in some context)
  if (kk_evv_is_vector(evv)) {
    kk_evv_vector_t vec = kk_evv_as_vector(evv);
    vec->cfc = kk_integer_from_int32(kk_cfc_lub(kk_integer_clamp32_borrow(vec->cfc,ctx),cfc), ctx);
  }
  else {
    struct kk_std_core_hnd_Ev* ev = kk_evv_as_Ev(evv);
    ev->cfc = kk_cfc_lub(ev->cfc,cfc);
  }
}

kk_evv_t kk_evv_insert(kk_evv_t evvd, kk_std_core_hnd__ev evd, kk_context_t* ctx) {
  struct kk_std_core_hnd_Ev* ev = kk_std_core_hnd__as_Ev(evd);
  // update ev with parent evidence vector (either at init, or due to non-scoped resumptions)
  int32_t marker = ev->marker.m;
  if (marker==0) { kk_std_core_hnd__ev_drop(evd,ctx); return evvd; } // ev-none
  kk_evv_drop(ev->hevv,ctx);
  ev->hevv = kk_evv_dup(evvd);
  if (marker<0) { // negative marker is used for named evidence; this means this evidence should not be inserted into the evidence vector
    kk_evv_update_cfc_borrow(evvd,ev->cfc,ctx); // update cfc in-place for named evidence
    kk_std_core_hnd__ev_drop(evd,ctx);
    return evvd;
  }
  // for regular handler evidence, insert ev
  kk_ssize_t n;
  kk_std_core_hnd__ev single;
  kk_std_core_hnd__ev* const evv1 = kk_evv_as_vec(evvd, &n, &single);
  if (n == 0) {
    // use ev directly as the evidence vector
    kk_evv_drop(evvd, ctx);
    return &evd->_block;
  }
  else {
    // create evidence vector
    const int32_t cfc = kk_cfc_lub(kk_evv_cfc_of_borrow(evvd, ctx), ev->cfc);
    ev->cfc = cfc; // update in place
    kk_evv_vector_t vec2 = kk_evv_vector_alloc(n+1, cfc, ctx);
    kk_std_core_hnd__ev* const evv2 = kk_evv_vector_buf(vec2, NULL);
    kk_ssize_t i;
    for (i = 0; i < n; i++) {
      struct kk_std_core_hnd_Ev* ev1 = kk_std_core_hnd__as_Ev(evv1[i]);
      if (kk_string_cmp_borrow(ev->htag.tagname, ev1->htag.tagname) <= 0) break;
      evv2[i] = kk_std_core_hnd__ev_dup(&ev1->_base);
    }
    evv2[i] = evd;
    for (; i < n; i++) {
      evv2[i+1] = kk_std_core_hnd__ev_dup(evv1[i]);
    }
    kk_evv_drop(evvd, ctx);  // assigned to evidence already
    return &vec2->_block;
  }
}

kk_evv_t kk_evv_delete(kk_evv_t evvd, kk_ssize_t index, bool behind, kk_context_t* ctx) {
  kk_ssize_t n;
  kk_std_core_hnd__ev single;
  const kk_std_core_hnd__ev* evv1 = kk_evv_as_vec(evvd, &n, &single);
  if (n <= 1) {
    kk_evv_drop(evvd,ctx);
    return kk_evv_total(ctx);
  }
  if (behind) index++;
  kk_assert_internal(index < n);
  // todo: copy without dupping (and later dropping) when possible
  const int32_t cfc1 = kk_evv_cfc_of_borrow(evvd,ctx);
  kk_evv_vector_t const vec2 = kk_evv_vector_alloc(n-1,cfc1,ctx);
  kk_std_core_hnd__ev* const evv2 = kk_evv_vector_buf(vec2,NULL);
  kk_ssize_t i;
  for(i = 0; i < index; i++) {
    evv2[i] = kk_std_core_hnd__ev_dup(evv1[i]);
  }
  for(; i < n-1; i++) {
    evv2[i] = kk_std_core_hnd__ev_dup(evv1[i+1]);
  }
  struct kk_std_core_hnd_Ev* ev = kk_std_core_hnd__as_Ev(evv1[index]);
  if (ev->cfc >= cfc1) {
    int32_t cfc = kk_std_core_hnd__as_Ev(evv2[0])->cfc;
    for(i = 1; i < n-1; i++) {
      cfc = kk_cfc_lub(cfc,kk_std_core_hnd__as_Ev(evv2[i])->cfc);
    }
    vec2->cfc = kk_integer_from_int32(cfc,ctx);
  }
  kk_evv_drop(evvd,ctx);
  return &vec2->_block;
}

kk_evv_t kk_evv_create(kk_evv_t evv1, kk_vector_t indices, kk_context_t* ctx) {
  kk_ssize_t len;
  kk_box_t* elems = kk_vector_buf_borrow(indices,&len); // borrows
  kk_evv_vector_t evv2 = kk_evv_vector_alloc(len,kk_evv_cfc_of_borrow(evv1,ctx),ctx);
  kk_std_core_hnd__ev* buf2 = kk_evv_vector_buf(evv2,NULL);
  kk_assert_internal(kk_evv_is_vector(evv1));
  kk_ssize_t len1;
  kk_std_core_hnd__ev single;
  kk_std_core_hnd__ev* buf1 = kk_evv_as_vec(evv1,&len1,&single);
  for(kk_ssize_t i = 0; i < len; i++) {
    kk_ssize_t idx = kk_ssize_unbox(elems[i],ctx);
    kk_assert_internal(idx < len1);
    buf2[i] = kk_std_core_hnd__ev_dup( buf1[idx] );
  }
  kk_vector_drop(indices,ctx);
  kk_evv_drop(evv1,ctx);
  return &evv2->_block;
}

kk_evv_t kk_evv_swap_create( kk_vector_t indices, kk_context_t* ctx ) {
  kk_ssize_t len;
  kk_box_t* vec = kk_vector_buf_borrow(indices,&len);
  if (len==0) {
    kk_vector_drop(indices,ctx);
    return kk_evv_swap_create0(ctx);
  }
  if (len==1) {
    kk_ssize_t i = kk_ssize_unbox(vec[0],ctx);
    kk_vector_drop(indices,ctx);
    return kk_evv_swap_create1(i,ctx);
  }
  return kk_evv_swap( kk_evv_create(kk_evv_dup(ctx->evv),indices,ctx), ctx );
}


kk_string_t kk_evv_show(kk_evv_t evv, kk_context_t* ctx) {
  return kk_string_alloc_dup_valid_utf8("(not yet implemented: kk_evv_show)",ctx);
}


/*-----------------------------------------------------------------------
  Compose continuations
-----------------------------------------------------------------------*/

struct kcompose_fun_s {
  struct kk_function_s _base;
  kk_box_t      count;
  kk_function_t conts[1];
};

// kleisli composition of continuations
static kk_box_t kcompose( kk_function_t fself, kk_box_t x, kk_context_t* ctx) {
  struct kcompose_fun_s* self = kk_function_as(struct kcompose_fun_s*,fself);
  kk_intx_t count = kk_intf_unbox(self->count);
  kk_function_t* conts = &self->conts[0];
  // call each continuation in order
  for(kk_intx_t i = 0; i < count; i++) {
    // todo: take uniqueness of fself into account to avoid dup_function
    kk_function_t f = kk_function_dup(conts[i]);
    x = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, x, ctx));
    if (kk_yielding(ctx)) {
      // if yielding, `yield_next` all continuations that still need to be done
      while(++i < count) {
        // todo: if fself is unique, we could copy without dup?
        kk_yield_extend(kk_function_dup(conts[i]),ctx);
      }
      kk_function_drop(fself,ctx);
      kk_box_drop(x,ctx);     // still drop even though we yield as it may release a boxed value type?
      return kk_box_any(ctx); // return yielding
    }
  }
  kk_function_drop(fself,ctx);
  return x;
}

static kk_function_t new_kcompose( kk_function_t* conts, kk_ssize_t count, kk_context_t* ctx ) {
  if (count==0) return kk_function_id(ctx);
  if (count==1) return conts[0];
  struct kcompose_fun_s* f = kk_block_as(struct kcompose_fun_s*,
                               kk_block_alloc(kk_ssizeof(struct kcompose_fun_s) - kk_ssizeof(kk_function_t) + (count*kk_ssizeof(kk_function_t)),
                                 2 + count /* scan size */, KK_TAG_FUNCTION, ctx));
  f->_base.fun = kk_cfun_ptr_box(&kcompose,ctx);
  f->count = kk_intf_box(count);
  kk_memcpy(f->conts, conts, count * kk_ssizeof(kk_function_t));
  return (&f->_base);
}

/*-----------------------------------------------------------------------
  Yield extension
-----------------------------------------------------------------------*/

kk_box_t kk_yield_extend( kk_function_t next, kk_context_t* ctx ) {
  kk_yield_t* yield = &ctx->yield;
  kk_assert_internal(kk_yielding(ctx));  // cannot extend if not yielding
  if (kk_unlikely(kk_yielding_final(ctx))) {
    // todo: can we optimize this so `next` is never allocated in the first place?
    kk_function_drop(next,ctx); // ignore extension if never resuming
  }
  else {
    if (kk_unlikely(yield->conts_count >= KK_YIELD_CONT_MAX)) {
      // alloc a function to compose all continuations in the array
      kk_function_t comp = new_kcompose( yield->conts, yield->conts_count, ctx );
      yield->conts[0] = comp;
      yield->conts_count = 1;
    }
    yield->conts[yield->conts_count++] = next;
  }
  return kk_box_any(ctx);
}

// cont_apply: \x -> f(cont,x)
struct cont_apply_fun_s {
  struct kk_function_s _base;
  kk_function_t f;
  kk_function_t cont;
};

static kk_box_t cont_apply( kk_function_t fself, kk_box_t x, kk_context_t* ctx ) {
  struct cont_apply_fun_s* self = kk_function_as(struct cont_apply_fun_s*, fself);
  kk_function_t f = self->f;
  kk_function_t cont = self->cont;
  kk_drop_match(self,{kk_function_dup(f);kk_function_dup(cont);},{},ctx);
  return kk_function_call( kk_box_t, (kk_function_t, kk_function_t, kk_box_t, kk_context_t* ctx), f, (f, cont, x, ctx));
}

static kk_function_t kk_new_cont_apply( kk_function_t f, kk_function_t cont, kk_context_t* ctx ) {
  struct cont_apply_fun_s* self = kk_function_alloc_as(struct cont_apply_fun_s, 3, ctx);
  self->_base.fun = kk_cfun_ptr_box(&cont_apply,ctx);
  self->f = f;
  self->cont = cont;
  return (&self->_base);
}

// Unlike `yield_extend`, `yield_cont` gets access to the current continuation. This is used in `yield_prompt`.
kk_box_t kk_yield_cont( kk_function_t f, kk_context_t* ctx ) {
  kk_yield_t* yield = &ctx->yield;
  kk_assert_internal(kk_yielding(ctx)); // cannot extend if not yielding
  if (kk_unlikely(kk_yielding_final(ctx))) {
    kk_function_drop(f,ctx); // ignore extension if never resuming
  }
  else {
    kk_function_t cont = new_kcompose(yield->conts, yield->conts_count, ctx);
    yield->conts_count = 1;
    yield->conts[0] = kk_new_cont_apply(f, cont, ctx);
  }
  return kk_box_any(ctx);
}

kk_function_t kk_yield_to( struct kk_std_core_hnd_Marker m, kk_function_t clause, kk_context_t* ctx ) {
  kk_yield_t* yield = &ctx->yield;
  kk_assert_internal(!kk_yielding(ctx)); // already yielding
  ctx->yielding = KK_YIELD_NORMAL;
  yield->marker = m.m;
  yield->clause = clause;
  yield->conts_count = 0;
  return kk_basetype_unbox_as(kk_function_t,kk_box_any(ctx));
}

kk_box_t kk_yield_final( struct kk_std_core_hnd_Marker m, kk_function_t clause, kk_context_t* ctx ) {
  kk_yield_to(m,clause,ctx);
  ctx->yielding = KK_YIELD_FINAL;
  return kk_box_any(ctx);
}

kk_box_t kk_fatal_resume_final(kk_context_t* ctx) {
  kk_fatal_error(EFAULT,"trying to resume a finalized resumption");
  return kk_box_any(ctx);
}

static kk_box_t _fatal_resume_final(kk_function_t self, kk_context_t* ctx) {
  kk_function_drop(self,ctx);
  return kk_fatal_resume_final(ctx);
}
static kk_function_t fun_fatal_resume_final(kk_context_t* ctx) {
  kk_define_static_function(f,_fatal_resume_final,ctx);
  return kk_function_dup(f);
}


struct kk_std_core_hnd_yld_s kk_yield_prompt( struct kk_std_core_hnd_Marker m, kk_context_t* ctx ) {
  kk_yield_t* yield = &ctx->yield;
  if (ctx->yielding == KK_YIELD_NONE) {
    return kk_std_core_hnd__new_Pure(ctx);
  }
  else if (yield->marker != m.m) {
    return (ctx->yielding == KK_YIELD_FINAL ? kk_std_core_hnd__new_YieldingFinal(ctx) : kk_std_core_hnd__new_Yielding(ctx));
  }
  else {
    kk_function_t cont = (ctx->yielding == KK_YIELD_FINAL ? fun_fatal_resume_final(ctx) : new_kcompose(yield->conts, yield->conts_count, ctx));
    kk_function_t clause = yield->clause;
    ctx->yielding = KK_YIELD_NONE;
    #ifndef NDEBUG
    kk_memset(yield,0,kk_ssizeof(kk_yield_t));
    #endif
    return kk_std_core_hnd__new_Yield(clause, cont, ctx);
  }
}

kk_unit_t  kk_evv_guard(kk_evv_t evv, kk_context_t* ctx) {
  bool eq = (ctx->evv == evv);
  kk_evv_drop(evv,ctx);
  if (!eq) {
    // todo: improve error message with diagnostics
    kk_fatal_error(EFAULT,"trying to resume outside the (handler) scope of the original handler");
  }
  return kk_Unit;
}

typedef struct yield_info_s {
  struct kk_std_core_hnd__yield_info_s _base;
  kk_function_t clause;
  kk_function_t conts[KK_YIELD_CONT_MAX];
  kk_ssize_t    conts_count;
  int32_t       marker;
  int8_t        yielding;
}* yield_info_t;

kk_std_core_hnd__yield_info kk_yield_capture(kk_context_t* ctx) {
  kk_assert_internal(kk_yielding(ctx));
  yield_info_t yld = kk_block_alloc_as(struct yield_info_s, 1 + KK_YIELD_CONT_MAX, (kk_tag_t)1, ctx);
  yld->clause = ctx->yield.clause;
  kk_ssize_t i = 0;
  for( ; i < ctx->yield.conts_count; i++) {
    yld->conts[i] = ctx->yield.conts[i];
  }
  for( ; i < KK_YIELD_CONT_MAX; i++) {
    yld->conts[i] = kk_function_null(ctx);
  }
  yld->conts_count = ctx->yield.conts_count;
  yld->marker = ctx->yield.marker;
  yld->yielding = ctx->yielding;
  ctx->yielding = 0;
  ctx->yield.conts_count = 0;
  return kk_datatype_from_base(&yld->_base);
}

kk_box_t kk_yield_reyield( kk_std_core_hnd__yield_info yldinfo, kk_context_t* ctx) {
  kk_assert_internal(!kk_yielding(ctx));
  yield_info_t yld = kk_datatype_as_assert(yield_info_t, yldinfo, (kk_tag_t)1);
  ctx->yield.clause = kk_function_dup(yld->clause);
  ctx->yield.marker = yld->marker;
  ctx->yield.conts_count = yld->conts_count;
  ctx->yielding = yld->yielding;
  for(kk_ssize_t i = 0; i < yld->conts_count; i++) {
    ctx->yield.conts[i] = kk_function_dup(yld->conts[i]);
  }
  kk_constructor_drop(yld,ctx);
  return kk_box_any(ctx);
}


kk_std_core_hnd__ev kk_std_core_hnd__copy_2(kk_std_core_hnd__ev _this, kk_std_core_types__optional htag0, kk_std_core_hnd__marker marker, kk_box_t hnd, kk_std_core_types__optional cfc0, kk_evv_t hevv, kk_context_t* _ctx) { /* forall<a,e,b> (ev<a>, htag : optional<htag<a>>, marker : marker<e,b>, hnd : a<e,b>, cfc : optional<cfc>, hevv : evv<e>) -> ev<a> */ 
  kk_std_core_hnd__htag _x11194;
  if (kk_std_core_types__is_Optional(htag0)) {
    kk_box_t _box_x10888 = htag0._cons.Optional.value;
    kk_std_core_hnd__htag _htag_2119 = kk_std_core_hnd__htag_unbox(_box_x10888, NULL);
    kk_std_core_hnd__htag_dup(_htag_2119);
    kk_std_core_types__optional_drop(htag0, _ctx);
    _x11194 = _htag_2119; /*std/core/hnd/htag<2145>*/
  }
  else {
    struct kk_std_core_hnd_Ev* _con11196 = kk_std_core_hnd__as_Ev(_this);
    kk_std_core_hnd__htag _x = _con11196->htag;
    kk_std_core_hnd__htag_dup(_x);
    _x11194 = _x; /*std/core/hnd/htag<2145>*/
  }
  int32_t _x11197;
  if (kk_std_core_types__is_Optional(cfc0)) {
    kk_box_t _box_x10889 = cfc0._cons.Optional.value;
    int32_t _cfc_2126 = kk_int32_unbox(_box_x10889, NULL);
    kk_std_core_hnd__ev_dropn(_this, (KK_I32(3)), _ctx);
    _x11197 = _cfc_2126; /*std/core/hnd/cfc*/
  }
  else {
    struct kk_std_core_hnd_Ev* _con11199 = kk_std_core_hnd__as_Ev(_this);
    kk_std_core_hnd__htag _pat01 = _con11199->htag;
    kk_box_t _pat21 = _con11199->hnd;
    int32_t _x0 = _con11199->cfc;
    kk_evv_t _pat30 = _con11199->hevv;
    if (kk_likely(kk_std_core_hnd__ev_is_unique(_this))) {
      kk_evv_drop(_pat30, _ctx);
      kk_box_drop(_pat21, _ctx);
      kk_std_core_hnd__htag_drop(_pat01, _ctx);
      kk_std_core_hnd__ev_free(_this, _ctx);
    }
    else {
      kk_std_core_hnd__ev_decref(_this, _ctx);
    }
    _x11197 = _x0; /*std/core/hnd/cfc*/
  }
  return kk_std_core_hnd__new_Ev(kk_reuse_null, _x11194, marker, hnd, _x11197, hevv, _ctx);
}


// lift anonymous function
struct kk_std_core_hnd__copy_fun11201__t_3 {
  struct kk_function_s _base;
  kk_box_t _fun_unbox_x10895;
};
static kk_box_t kk_std_core_hnd__copy_fun11201_3(kk_function_t _fself, kk_std_core_hnd__marker _b_10899, kk_std_core_hnd__ev _b_10900, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_copy_fun11201_3(kk_box_t _fun_unbox_x10895, kk_context_t* _ctx) {
  struct kk_std_core_hnd__copy_fun11201__t_3* _self = kk_function_alloc_as(struct kk_std_core_hnd__copy_fun11201__t_3, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__copy_fun11201_3, kk_context());
  _self->_fun_unbox_x10895 = _fun_unbox_x10895;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__copy_fun11201_3(kk_function_t _fself, kk_std_core_hnd__marker _b_10899, kk_std_core_hnd__ev _b_10900, kk_context_t* _ctx) {
  struct kk_std_core_hnd__copy_fun11201__t_3* _self = kk_function_as(struct kk_std_core_hnd__copy_fun11201__t_3*, _fself);
  kk_box_t _fun_unbox_x10895 = _self->_fun_unbox_x10895; /* 1033 */
  kk_drop_match(_self, {kk_box_dup(_fun_unbox_x10895);}, {}, _ctx)
  kk_function_t _x11202 = kk_function_unbox(_fun_unbox_x10895); /*(10896, 10897) -> 2238 10898*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_context_t*), _x11202, (_x11202, kk_std_core_hnd__marker_box(_b_10899, _ctx), kk_std_core_hnd__ev_box(_b_10900, _ctx), _ctx));
}

kk_std_core_hnd__clause0 kk_std_core_hnd__copy_3(kk_std_core_hnd__clause0 _this, kk_std_core_types__optional clause0, kk_context_t* _ctx) { /* forall<a,b,e,c> (clause0<a,b,e,c>, clause : optional<(marker<e,c>, ev<b>) -> e a>) -> clause0<a,b,e,c> */ 
  kk_function_t _x11200;
  if (kk_std_core_types__is_Optional(clause0)) {
    kk_box_t _fun_unbox_x10895 = clause0._cons.Optional.value;
    kk_std_core_hnd__clause0_drop(_this, _ctx);
    _x11200 = kk_std_core_hnd__new_copy_fun11201_3(_fun_unbox_x10895, _ctx); /*(std/core/hnd/marker<2238,2239>, std/core/hnd/ev<2237>) -> 2238 10898*/
  }
  else {
    kk_function_t _x = _this.clause;
    _x11200 = _x; /*(std/core/hnd/marker<2238,2239>, std/core/hnd/ev<2237>) -> 2238 10898*/
  }
  return kk_std_core_hnd__new_Clause0(_x11200, _ctx);
}


// lift anonymous function
struct kk_std_core_hnd__copy_fun11204__t_4 {
  struct kk_function_s _base;
  kk_box_t _fun_unbox_x10908;
};
static kk_box_t kk_std_core_hnd__copy_fun11204_4(kk_function_t _fself, kk_std_core_hnd__marker _b_10913, kk_std_core_hnd__ev _b_10914, kk_box_t _b_10915, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_copy_fun11204_4(kk_box_t _fun_unbox_x10908, kk_context_t* _ctx) {
  struct kk_std_core_hnd__copy_fun11204__t_4* _self = kk_function_alloc_as(struct kk_std_core_hnd__copy_fun11204__t_4, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__copy_fun11204_4, kk_context());
  _self->_fun_unbox_x10908 = _fun_unbox_x10908;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__copy_fun11204_4(kk_function_t _fself, kk_std_core_hnd__marker _b_10913, kk_std_core_hnd__ev _b_10914, kk_box_t _b_10915, kk_context_t* _ctx) {
  struct kk_std_core_hnd__copy_fun11204__t_4* _self = kk_function_as(struct kk_std_core_hnd__copy_fun11204__t_4*, _fself);
  kk_box_t _fun_unbox_x10908 = _self->_fun_unbox_x10908; /* 1033 */
  kk_drop_match(_self, {kk_box_dup(_fun_unbox_x10908);}, {}, _ctx)
  kk_function_t _x11205 = kk_function_unbox(_fun_unbox_x10908); /*(10909, 10910, 10911) -> 2444 10912*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_context_t*), _x11205, (_x11205, kk_std_core_hnd__marker_box(_b_10913, _ctx), kk_std_core_hnd__ev_box(_b_10914, _ctx), _b_10915, _ctx));
}

kk_std_core_hnd__clause1 kk_std_core_hnd__copy_4(kk_std_core_hnd__clause1 _this, kk_std_core_types__optional clause0, kk_context_t* _ctx) { /* forall<a,b,c,e,d> (clause1<a,b,c,e,d>, clause : optional<(marker<e,d>, ev<c>, a) -> e b>) -> clause1<a,b,c,e,d> */ 
  kk_function_t _x11203;
  if (kk_std_core_types__is_Optional(clause0)) {
    kk_box_t _fun_unbox_x10908 = clause0._cons.Optional.value;
    kk_std_core_hnd__clause1_drop(_this, _ctx);
    _x11203 = kk_std_core_hnd__new_copy_fun11204_4(_fun_unbox_x10908, _ctx); /*(std/core/hnd/marker<2444,2445>, std/core/hnd/ev<2443>, 2441) -> 2444 10912*/
  }
  else {
    kk_function_t _x = _this.clause;
    _x11203 = _x; /*(std/core/hnd/marker<2444,2445>, std/core/hnd/ev<2443>, 2441) -> 2444 10912*/
  }
  return kk_std_core_hnd__new_Clause1(_x11203, _ctx);
}


// lift anonymous function
struct kk_std_core_hnd__copy_fun11207__t_5 {
  struct kk_function_s _base;
  kk_box_t _fun_unbox_x10925;
};
static kk_box_t kk_std_core_hnd__copy_fun11207_5(kk_function_t _fself, kk_std_core_hnd__marker _b_10931, kk_std_core_hnd__ev _b_10932, kk_box_t _b_10933, kk_box_t _b_10934, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_copy_fun11207_5(kk_box_t _fun_unbox_x10925, kk_context_t* _ctx) {
  struct kk_std_core_hnd__copy_fun11207__t_5* _self = kk_function_alloc_as(struct kk_std_core_hnd__copy_fun11207__t_5, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__copy_fun11207_5, kk_context());
  _self->_fun_unbox_x10925 = _fun_unbox_x10925;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__copy_fun11207_5(kk_function_t _fself, kk_std_core_hnd__marker _b_10931, kk_std_core_hnd__ev _b_10932, kk_box_t _b_10933, kk_box_t _b_10934, kk_context_t* _ctx) {
  struct kk_std_core_hnd__copy_fun11207__t_5* _self = kk_function_as(struct kk_std_core_hnd__copy_fun11207__t_5*, _fself);
  kk_box_t _fun_unbox_x10925 = _self->_fun_unbox_x10925; /* 1033 */
  kk_drop_match(_self, {kk_box_dup(_fun_unbox_x10925);}, {}, _ctx)
  kk_function_t _x11208 = kk_function_unbox(_fun_unbox_x10925); /*(10926, 10927, 10928, 10929) -> 2745 10930*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_box_t, kk_context_t*), _x11208, (_x11208, kk_std_core_hnd__marker_box(_b_10931, _ctx), kk_std_core_hnd__ev_box(_b_10932, _ctx), _b_10933, _b_10934, _ctx));
}

kk_std_core_hnd__clause2 kk_std_core_hnd__copy_5(kk_std_core_hnd__clause2 _this, kk_std_core_types__optional clause0, kk_context_t* _ctx) { /* forall<a,b,c,d,e,a1> (clause2<a,b,c,d,e,a1>, clause : optional<(marker<e,a1>, ev<d>, a, b) -> e c>) -> clause2<a,b,c,d,e,a1> */ 
  kk_function_t _x11206;
  if (kk_std_core_types__is_Optional(clause0)) {
    kk_box_t _fun_unbox_x10925 = clause0._cons.Optional.value;
    kk_std_core_hnd__clause2_drop(_this, _ctx);
    _x11206 = kk_std_core_hnd__new_copy_fun11207_5(_fun_unbox_x10925, _ctx); /*(std/core/hnd/marker<2745,2746>, std/core/hnd/ev<2744>, 2741, 2742) -> 2745 10930*/
  }
  else {
    kk_function_t _x = _this.clause;
    _x11206 = _x; /*(std/core/hnd/marker<2745,2746>, std/core/hnd/ev<2744>, 2741, 2742) -> 2745 10930*/
  }
  return kk_std_core_hnd__new_Clause2(_x11206, _ctx);
}


// lift anonymous function
struct kk_std_core_hnd__copy_fun11210__t_6 {
  struct kk_function_s _base;
  kk_box_t _fun_unbox_x10938;
};
static kk_box_t kk_std_core_hnd__copy_fun11210_6(kk_function_t _fself, kk_std_core_hnd__resume_result _b_10941, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_copy_fun11210_6(kk_box_t _fun_unbox_x10938, kk_context_t* _ctx) {
  struct kk_std_core_hnd__copy_fun11210__t_6* _self = kk_function_alloc_as(struct kk_std_core_hnd__copy_fun11210__t_6, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__copy_fun11210_6, kk_context());
  _self->_fun_unbox_x10938 = _fun_unbox_x10938;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__copy_fun11210_6(kk_function_t _fself, kk_std_core_hnd__resume_result _b_10941, kk_context_t* _ctx) {
  struct kk_std_core_hnd__copy_fun11210__t_6* _self = kk_function_as(struct kk_std_core_hnd__copy_fun11210__t_6*, _fself);
  kk_box_t _fun_unbox_x10938 = _self->_fun_unbox_x10938; /* 1033 */
  kk_drop_match(_self, {kk_box_dup(_fun_unbox_x10938);}, {}, _ctx)
  kk_function_t _x11211 = kk_function_unbox(_fun_unbox_x10938); /*(10939) -> 2959 10940*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x11211, (_x11211, kk_std_core_hnd__resume_result_box(_b_10941, _ctx), _ctx));
}

kk_std_core_hnd__resume_context kk_std_core_hnd__copy_6(kk_std_core_hnd__resume_context _this, kk_std_core_types__optional k0, kk_context_t* _ctx) { /* forall<a,e,e1,b> (resume-context<a,e,e1,b>, k : optional<(resume-result<a,b>) -> e b>) -> resume-context<a,e,e1,b> */ 
  kk_function_t _x11209;
  if (kk_std_core_types__is_Optional(k0)) {
    kk_box_t _fun_unbox_x10938 = k0._cons.Optional.value;
    kk_std_core_hnd__resume_context_drop(_this, _ctx);
    _x11209 = kk_std_core_hnd__new_copy_fun11210_6(_fun_unbox_x10938, _ctx); /*(std/core/hnd/resume-result<2958,2961>) -> 2959 10940*/
  }
  else {
    kk_function_t _x = _this.k;
    _x11209 = _x; /*(std/core/hnd/resume-result<2958,2961>) -> 2959 10940*/
  }
  return kk_std_core_hnd__new_Resume_context(_x11209, _ctx);
}
 
// (dynamically) find evidence insertion/deletion index in the evidence vector

kk_ssize_t kk_std_core_hnd__evv_index(kk_std_core_hnd__htag htag0, kk_context_t* _ctx) { /* forall<e,a> (htag : htag<a>) -> e ev-index */ 
  return kk_evv_index(htag0,kk_context());
}

bool kk_std_core_hnd__evv_is_affine(kk_context_t* _ctx) { /* () -> bool */ 
  return kk_evv_cfc(kk_context())<=2;
}

kk_std_core_hnd__ev kk_std_core_hnd__evv_lookup(kk_std_core_hnd__htag htag0, kk_context_t* _ctx) { /* forall<a> (htag : htag<a>) -> ev<a> */ 
  return kk_evv_lookup(htag0,kk_context());
}

kk_evv_t kk_std_core_hnd_evv_get(kk_context_t* _ctx) { /* forall<e> () -> e evv<e> */ 
  return kk_evv_get(kk_context());
}

kk_evv_t kk_std_core_hnd_evv_insert(kk_evv_t evv, kk_std_core_hnd__ev ev, kk_context_t* _ctx) { /* forall<e,e1,a> (evv : evv<e>, ev : ev<a>) -> e evv<e1> */ 
  return kk_evv_insert(evv,ev,kk_context());
}

int32_t kk_std_core_hnd_fresh_marker_int(kk_context_t* _ctx) { /* () -> int32 */ 
  return kk_marker_unique(kk_context());
}

bool kk_std_core_hnd_evv_eq(kk_evv_t evv0, kk_evv_t evv1, kk_context_t* _ctx) { /* forall<e> (evv0 : evv<e>, evv1 : evv<e>) -> bool */ 
  return kk_evv_eq(evv0,evv1,kk_context());
}

kk_unit_t kk_std_core_hnd_guard(kk_evv_t w, kk_context_t* _ctx) { /* forall<e> (w : evv<e>) -> e () */ 
  kk_evv_guard(w,kk_context()); return kk_Unit;
}

kk_box_t kk_std_core_hnd_yield_extend(kk_function_t next, kk_context_t* _ctx) { /* forall<a,b,e> (next : (a) -> e b) -> e b */ 
  return kk_yield_extend(next,kk_context());
}

kk_box_t kk_std_core_hnd_yield_cont(kk_function_t f, kk_context_t* _ctx) { /* forall<a,e,b> (f : forall<c> ((c) -> e a, c) -> e b) -> e b */ 
  return kk_yield_cont(f,kk_context());
}

kk_std_core_hnd__yld kk_std_core_hnd_yield_prompt(kk_std_core_hnd__marker m0, kk_context_t* _ctx) { /* forall<a,e,b> (m : marker<e,b>) -> yld<e,a,b> */ 
  return kk_yield_prompt(m0,kk_context());
}

kk_box_t kk_std_core_hnd_yield_to_final(kk_std_core_hnd__marker m0, kk_function_t clause0, kk_context_t* _ctx) { /* forall<a,e,e1,b> (m : marker<e1,b>, clause : ((resume-result<a,b>) -> e1 b) -> e1 b) -> e a */ 
  return kk_yield_final(m0,clause0,kk_context());
}

kk_evv_t kk_std_core_hnd_evv_swap_delete(kk_ssize_t i, bool behind, kk_context_t* _ctx) { /* forall<e,e1> (i : ev-index, behind : bool) -> e1 evv<e> */ 
  return kk_evv_swap_delete(i,behind,kk_context());
}

int32_t kk_std_core_hnd_fresh_marker_named_int(kk_context_t* _ctx) { /* () -> int32 */ 
  return -kk_marker_unique(kk_context());
}

kk_evv_t kk_std_core_hnd_evv_swap_create(kk_vector_t indices, kk_context_t* _ctx) { /* forall<e> (indices : vector<ev-index>) -> e evv<e> */ 
  return kk_evv_swap_create(indices,kk_context());
}

kk_function_t kk_std_core_hnd_yield_to_prim(kk_std_core_hnd__marker m0, kk_function_t clause0, kk_context_t* _ctx) { /* forall<a,e,e1,b> (m : marker<e1,b>, clause : ((resume-result<a,b>) -> e1 b) -> e1 b) -> e (() -> a) */ 
  return kk_yield_to(m0,clause0,kk_context());
}
extern kk_box_t kk_std_core_hnd_clause_tail_noyield0_fun11216(kk_function_t _fself, kk_std_core_hnd__marker ___wildcard__565__14, kk_std_core_hnd__ev ___wildcard__565__17, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail_noyield0_fun11216__t* _self = kk_function_as(struct kk_std_core_hnd_clause_tail_noyield0_fun11216__t*, _fself);
  kk_function_t op = _self->op; /* () -> 4172 4174 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(___wildcard__565__17, (KK_I32(3)), _ctx);
  return kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), op, (op, _ctx));
}
extern kk_box_t kk_std_core_hnd_clause_tail_noyield1_fun11217(kk_function_t _fself, kk_std_core_hnd__marker ___wildcard__522__14, kk_std_core_hnd__ev ___wildcard__522__17, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail_noyield1_fun11217__t* _self = kk_function_as(struct kk_std_core_hnd_clause_tail_noyield1_fun11217__t*, _fself);
  kk_function_t op = _self->op; /* (4205) -> 4203 4206 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(___wildcard__522__17, (KK_I32(3)), _ctx);
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), op, (op, x, _ctx));
}
extern kk_box_t kk_std_core_hnd_clause_tail_noyield2_fun11218(kk_function_t _fself, kk_std_core_hnd__marker ___wildcard__607__14, kk_std_core_hnd__ev ___wildcard__607__17, kk_box_t x1, kk_box_t x2, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail_noyield2_fun11218__t* _self = kk_function_as(struct kk_std_core_hnd_clause_tail_noyield2_fun11218__t*, _fself);
  kk_function_t op = _self->op; /* (4242, 4243) -> 4240 4244 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(___wildcard__607__17, (KK_I32(3)), _ctx);
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_context_t*), op, (op, x1, x2, _ctx));
}

kk_evv_t kk_std_core_hnd_evv_swap_with(kk_std_core_hnd__ev ev, kk_context_t* _ctx) { /* forall<a,e> (ev : ev<a>) -> evv<e> */ 
  kk_evv_t _x11219;
  {
    struct kk_std_core_hnd_Ev* _con11220 = kk_std_core_hnd__as_Ev(ev);
    kk_std_core_hnd__htag _pat0 = _con11220->htag;
    kk_box_t _pat2 = _con11220->hnd;
    kk_evv_t w = _con11220->hevv;
    if (kk_likely(kk_std_core_hnd__ev_is_unique(ev))) {
      kk_box_drop(_pat2, _ctx);
      kk_std_core_hnd__htag_drop(_pat0, _ctx);
      kk_std_core_hnd__ev_free(ev, _ctx);
    }
    else {
      kk_evv_dup(w);
      kk_std_core_hnd__ev_decref(ev, _ctx);
    }
    _x11219 = w; /*std/core/hnd/evv<4249>*/
  }
  return kk_evv_swap(_x11219,kk_context());
}
extern kk_box_t kk_std_core_hnd_clause_value_fun11221(kk_function_t _fself, kk_std_core_hnd__marker ___wildcard__568__14, kk_std_core_hnd__ev ___wildcard__568__17, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_value_fun11221__t* _self = kk_function_as(struct kk_std_core_hnd_clause_value_fun11221__t*, _fself);
  kk_box_t v = _self->v; /* 4292 */
  kk_drop_match(_self, {kk_box_dup(v);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(___wildcard__568__17, (KK_I32(3)), _ctx);
  return v;
}

kk_string_t kk_std_core_hnd_evv_show(kk_evv_t evv, kk_context_t* _ctx) { /* forall<e> (evv : evv<e>) -> string */ 
  return kk_evv_show(evv,kk_context());
}

kk_box_t kk_std_core_hnd_unsafe_reyield(kk_std_core_hnd__yield_info yld, kk_context_t* _ctx) { /* forall<a,e> (yld : yield-info) -> e a */ 
  return kk_yield_reyield(yld,kk_context());
}

kk_std_core_hnd__yield_info kk_std_core_hnd_yield_capture(kk_context_t* _ctx) { /* forall<e> () -> e yield-info */ 
  return kk_yield_capture(kk_context());
}

kk_box_t kk_std_core_hnd_resume_final(kk_context_t* _ctx) { /* forall<a> () -> a */ 
  return kk_fatal_resume_final(kk_context());
}


// lift anonymous function
struct kk_std_core_hnd_prompt_fun11229__t {
  struct kk_function_s _base;
  kk_std_core_hnd__ev ev;
  kk_function_t ret;
  kk_evv_t w0;
  kk_evv_t w1;
  kk_std_core_hnd__marker m0;
};
static kk_box_t kk_std_core_hnd_prompt_fun11229(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_prompt_fun11229(kk_std_core_hnd__ev ev, kk_function_t ret, kk_evv_t w0, kk_evv_t w1, kk_std_core_hnd__marker m0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_prompt_fun11229__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_prompt_fun11229__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_prompt_fun11229, kk_context());
  _self->ev = ev;
  _self->ret = ret;
  _self->w0 = w0;
  _self->w1 = w1;
  _self->m0 = m0;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_prompt_fun11229(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd_prompt_fun11229__t* _self = kk_function_as(struct kk_std_core_hnd_prompt_fun11229__t*, _fself);
  kk_std_core_hnd__ev ev = _self->ev; /* std/core/hnd/ev<4764> */
  kk_function_t ret = _self->ret; /* (4762) -> 4763 4765 */
  kk_evv_t w0 = _self->w0; /* std/core/hnd/evv<4763> */
  kk_evv_t w1 = _self->w1; /* std/core/hnd/evv<4763> */
  kk_std_core_hnd__marker m0 = _self->m0; /* std/core/hnd/marker<4763,4765> */
  kk_drop_match(_self, {kk_std_core_hnd__ev_dup(ev);kk_function_dup(ret);kk_evv_dup(w0);kk_evv_dup(w1);kk_std_core_hnd__marker_dup(m0);}, {}, _ctx)
  kk_evv_t w0_sq_ = kk_std_core_hnd_evv_get(_ctx); /*std/core/hnd/evv<4763>*/;
  kk_evv_t w1_sq_;
  bool _match_11178;
  kk_evv_t _x11230 = kk_evv_dup(w0_sq_); /*std/core/hnd/evv<4763>*/
  _match_11178 = kk_std_core_hnd_evv_eq(w0, _x11230, _ctx); /*bool*/
  if (_match_11178) {
    w1_sq_ = w1; /*std/core/hnd/evv<4763>*/
  }
  else {
    kk_evv_drop(w1, _ctx);
    kk_evv_t _x11231 = kk_evv_dup(w0_sq_); /*std/core/hnd/evv<4763>*/
    kk_std_core_hnd__ev _x11232 = kk_std_core_hnd__ev_dup(ev); /*std/core/hnd/ev<4764>*/
    w1_sq_ = kk_std_core_hnd_evv_insert(_x11231, _x11232, _ctx); /*std/core/hnd/evv<4763>*/
  }
  kk_unit_t __1 = kk_Unit;
  kk_evv_t _x11233 = kk_evv_dup(w1_sq_); /*std/core/hnd/evv<4763>*/
  kk_evv_set(_x11233,kk_context());
  kk_box_t _x11234 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), cont, (cont, res, _ctx)); /*4762*/
  return kk_std_core_hnd_prompt(w0_sq_, w1_sq_, ev, m0, ret, _x11234, _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_prompt_fun11235__t {
  struct kk_function_s _base;
  kk_function_t cont0;
  kk_std_core_hnd__ev ev;
  kk_function_t ret;
  kk_evv_t w0;
  kk_evv_t w1;
  kk_std_core_hnd__marker m0;
};
static kk_box_t kk_std_core_hnd_prompt_fun11235(kk_function_t _fself, kk_std_core_hnd__resume_result r, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_prompt_fun11235(kk_function_t cont0, kk_std_core_hnd__ev ev, kk_function_t ret, kk_evv_t w0, kk_evv_t w1, kk_std_core_hnd__marker m0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_prompt_fun11235__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_prompt_fun11235__t, 6, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_prompt_fun11235, kk_context());
  _self->cont0 = cont0;
  _self->ev = ev;
  _self->ret = ret;
  _self->w0 = w0;
  _self->w1 = w1;
  _self->m0 = m0;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_prompt_fun11241__t {
  struct kk_function_s _base;
  kk_box_t x;
};
static kk_box_t kk_std_core_hnd_prompt_fun11241(kk_function_t _fself, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_prompt_fun11241(kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_prompt_fun11241__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_prompt_fun11241__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_prompt_fun11241, kk_context());
  _self->x = x;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_prompt_fun11241(kk_function_t _fself, kk_context_t* _ctx) {
  struct kk_std_core_hnd_prompt_fun11241__t* _self = kk_function_as(struct kk_std_core_hnd_prompt_fun11241__t*, _fself);
  kk_box_t x = _self->x; /* 4628 */
  kk_drop_match(_self, {kk_box_dup(x);}, {}, _ctx)
  return x;
}


// lift anonymous function
struct kk_std_core_hnd_prompt_fun11242__t {
  struct kk_function_s _base;
  kk_box_t x0;
};
static kk_box_t kk_std_core_hnd_prompt_fun11242(kk_function_t _fself, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_prompt_fun11242(kk_box_t x0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_prompt_fun11242__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_prompt_fun11242__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_prompt_fun11242, kk_context());
  _self->x0 = x0;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_prompt_fun11242(kk_function_t _fself, kk_context_t* _ctx) {
  struct kk_std_core_hnd_prompt_fun11242__t* _self = kk_function_as(struct kk_std_core_hnd_prompt_fun11242__t*, _fself);
  kk_box_t x0 = _self->x0; /* 4628 */
  kk_drop_match(_self, {kk_box_dup(x0);}, {}, _ctx)
  return x0;
}


// lift anonymous function
struct kk_std_core_hnd_prompt_fun11248__t {
  struct kk_function_s _base;
  kk_box_t x10;
  kk_std_core_hnd__marker m0;
};
static kk_box_t kk_std_core_hnd_prompt_fun11248(kk_function_t _fself, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_prompt_fun11248(kk_box_t x10, kk_std_core_hnd__marker m0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_prompt_fun11248__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_prompt_fun11248__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_prompt_fun11248, kk_context());
  _self->x10 = x10;
  _self->m0 = m0;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_prompt_fun11249__t {
  struct kk_function_s _base;
  kk_box_t x10;
};
static kk_box_t kk_std_core_hnd_prompt_fun11249(kk_function_t _fself, kk_function_t ___wildcard__302__84, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_prompt_fun11249(kk_box_t x10, kk_context_t* _ctx) {
  struct kk_std_core_hnd_prompt_fun11249__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_prompt_fun11249__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_prompt_fun11249, kk_context());
  _self->x10 = x10;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_prompt_fun11249(kk_function_t _fself, kk_function_t ___wildcard__302__84, kk_context_t* _ctx) {
  struct kk_std_core_hnd_prompt_fun11249__t* _self = kk_function_as(struct kk_std_core_hnd_prompt_fun11249__t*, _fself);
  kk_box_t x10 = _self->x10; /* 4765 */
  kk_drop_match(_self, {kk_box_dup(x10);}, {}, _ctx)
  kk_function_drop(___wildcard__302__84, _ctx);
  return x10;
}
static kk_box_t kk_std_core_hnd_prompt_fun11248(kk_function_t _fself, kk_context_t* _ctx) {
  struct kk_std_core_hnd_prompt_fun11248__t* _self = kk_function_as(struct kk_std_core_hnd_prompt_fun11248__t*, _fself);
  kk_box_t x10 = _self->x10; /* 4765 */
  kk_std_core_hnd__marker m0 = _self->m0; /* std/core/hnd/marker<4763,4765> */
  kk_drop_match(_self, {kk_box_dup(x10);kk_std_core_hnd__marker_dup(m0);}, {}, _ctx)
  return kk_std_core_hnd_yield_to_final(m0, kk_std_core_hnd_new_prompt_fun11249(x10, _ctx), _ctx);
}
static kk_box_t kk_std_core_hnd_prompt_fun11235(kk_function_t _fself, kk_std_core_hnd__resume_result r, kk_context_t* _ctx) {
  struct kk_std_core_hnd_prompt_fun11235__t* _self = kk_function_as(struct kk_std_core_hnd_prompt_fun11235__t*, _fself);
  kk_function_t cont0 = _self->cont0; /* (() -> 4628) -> 4763 4762 */
  kk_std_core_hnd__ev ev = _self->ev; /* std/core/hnd/ev<4764> */
  kk_function_t ret = _self->ret; /* (4762) -> 4763 4765 */
  kk_evv_t w0 = _self->w0; /* std/core/hnd/evv<4763> */
  kk_evv_t w1 = _self->w1; /* std/core/hnd/evv<4763> */
  kk_std_core_hnd__marker m0 = _self->m0; /* std/core/hnd/marker<4763,4765> */
  kk_drop_match(_self, {kk_function_dup(cont0);kk_std_core_hnd__ev_dup(ev);kk_function_dup(ret);kk_evv_dup(w0);kk_evv_dup(w1);kk_std_core_hnd__marker_dup(m0);}, {}, _ctx)
  if (kk_std_core_hnd__is_Deep(r)) {
    kk_box_t x = r._cons.Deep.result;
    kk_evv_t w00_sq_ = kk_std_core_hnd_evv_get(_ctx); /*std/core/hnd/evv<4763>*/;
    kk_evv_t w10_sq_;
    bool _match_11177;
    kk_evv_t _x11236 = kk_evv_dup(w00_sq_); /*std/core/hnd/evv<4763>*/
    _match_11177 = kk_std_core_hnd_evv_eq(w0, _x11236, _ctx); /*bool*/
    if (_match_11177) {
      w10_sq_ = w1; /*std/core/hnd/evv<4763>*/
    }
    else {
      kk_evv_drop(w1, _ctx);
      kk_evv_t _x11237 = kk_evv_dup(w00_sq_); /*std/core/hnd/evv<4763>*/
      kk_std_core_hnd__ev _x11238 = kk_std_core_hnd__ev_dup(ev); /*std/core/hnd/ev<4764>*/
      w10_sq_ = kk_std_core_hnd_evv_insert(_x11237, _x11238, _ctx); /*std/core/hnd/evv<4763>*/
    }
    kk_unit_t __2 = kk_Unit;
    kk_evv_t _x11239 = kk_evv_dup(w10_sq_); /*std/core/hnd/evv<4763>*/
    kk_evv_set(_x11239,kk_context());
    kk_box_t _x11240 = kk_function_call(kk_box_t, (kk_function_t, kk_function_t, kk_context_t*), cont0, (cont0, kk_std_core_hnd_new_prompt_fun11241(x, _ctx), _ctx)); /*4762*/
    return kk_std_core_hnd_prompt(w00_sq_, w10_sq_, ev, m0, ret, _x11240, _ctx);
  }
  if (kk_std_core_hnd__is_Shallow(r)) {
    kk_box_t x0 = r._cons.Shallow.result;
    kk_evv_drop(w1, _ctx);
    kk_evv_drop(w0, _ctx);
    kk_std_core_hnd__ev_dropn(ev, (KK_I32(3)), _ctx);
    kk_box_t x1_10832 = kk_function_call(kk_box_t, (kk_function_t, kk_function_t, kk_context_t*), cont0, (cont0, kk_std_core_hnd_new_prompt_fun11242(x0, _ctx), _ctx)); /*4762*/;
    if (kk_yielding(kk_context())) {
      kk_box_drop(x1_10832, _ctx);
      return kk_std_core_hnd_yield_extend(ret, _ctx);
    }
    {
      return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), ret, (ret, x1_10832, _ctx));
    }
  }
  {
    kk_box_t x10 = r._cons.Finalize.result;
    kk_evv_t w01_sq_ = kk_std_core_hnd_evv_get(_ctx); /*std/core/hnd/evv<4763>*/;
    kk_evv_t w11_sq_;
    bool _match_11175;
    kk_evv_t _x11243 = kk_evv_dup(w01_sq_); /*std/core/hnd/evv<4763>*/
    _match_11175 = kk_std_core_hnd_evv_eq(w0, _x11243, _ctx); /*bool*/
    if (_match_11175) {
      w11_sq_ = w1; /*std/core/hnd/evv<4763>*/
    }
    else {
      kk_evv_drop(w1, _ctx);
      kk_evv_t _x11244 = kk_evv_dup(w01_sq_); /*std/core/hnd/evv<4763>*/
      kk_std_core_hnd__ev _x11245 = kk_std_core_hnd__ev_dup(ev); /*std/core/hnd/ev<4764>*/
      w11_sq_ = kk_std_core_hnd_evv_insert(_x11244, _x11245, _ctx); /*std/core/hnd/evv<4763>*/
    }
    kk_unit_t __3 = kk_Unit;
    kk_evv_t _x11246 = kk_evv_dup(w11_sq_); /*std/core/hnd/evv<4763>*/
    kk_evv_set(_x11246,kk_context());
    kk_box_t _x11247 = kk_function_call(kk_box_t, (kk_function_t, kk_function_t, kk_context_t*), cont0, (cont0, kk_std_core_hnd_new_prompt_fun11248(x10, m0, _ctx), _ctx)); /*4762*/
    return kk_std_core_hnd_prompt(w01_sq_, w11_sq_, ev, m0, ret, _x11247, _ctx);
  }
}

kk_box_t kk_std_core_hnd_prompt(kk_evv_t w0, kk_evv_t w1, kk_std_core_hnd__ev ev, kk_std_core_hnd__marker m0, kk_function_t ret, kk_box_t result, kk_context_t* _ctx) { /* forall<a,e,b,c> (w0 : evv<e>, w1 : evv<e>, ev : ev<b>, m : marker<e,c>, ret : (a) -> e c, result : a) -> e c */ 
  kk_unit_t __ = kk_Unit;
  kk_evv_t _x11227 = kk_evv_dup(w1); /*std/core/hnd/evv<4763>*/
  kk_std_core_hnd_guard(_x11227, _ctx);
  kk_unit_t __0 = kk_Unit;
  kk_evv_t _x11228 = kk_evv_dup(w0); /*std/core/hnd/evv<4763>*/
  kk_evv_set(_x11228,kk_context());
  kk_std_core_hnd__yld _match_11174 = kk_std_core_hnd_yield_prompt(m0, _ctx); /*std/core/hnd/yld<3968,3967,3969>*/;
  if (kk_std_core_hnd__is_Pure(_match_11174)) {
    kk_evv_drop(w1, _ctx);
    kk_evv_drop(w0, _ctx);
    kk_std_core_hnd__ev_dropn(ev, (KK_I32(3)), _ctx);
    return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), ret, (ret, result, _ctx));
  }
  if (kk_std_core_hnd__is_YieldingFinal(_match_11174)) {
    kk_evv_drop(w1, _ctx);
    kk_evv_drop(w0, _ctx);
    kk_function_drop(ret, _ctx);
    kk_box_drop(result, _ctx);
    kk_std_core_hnd__ev_dropn(ev, (KK_I32(3)), _ctx);
    return kk_box_any(kk_context());
  }
  if (kk_std_core_hnd__is_Yielding(_match_11174)) {
    kk_box_drop(result, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_prompt_fun11229(ev, ret, w0, w1, m0, _ctx), _ctx);
  }
  {
    kk_function_t clause0 = _match_11174._cons.Yield.clause;
    kk_function_t cont0 = _match_11174._cons.Yield.cont;
    kk_box_drop(result, _ctx);
    return kk_function_call(kk_box_t, (kk_function_t, kk_function_t, kk_context_t*), clause0, (clause0, kk_std_core_hnd_new_prompt_fun11235(cont0, ev, ret, w0, w1, m0, _ctx), _ctx));
  }
}

kk_box_t kk_std_core_hnd__hhandle(kk_std_core_hnd__htag tag, int32_t cfc0, kk_box_t h, kk_function_t ret, kk_function_t action, kk_context_t* _ctx) { /* forall<a,e,e1,b,c> (tag : htag<b>, cfc : cfc, h : b<e,c>, ret : (a) -> e c, action : () -> e1 a) -> e c */ 
  kk_evv_t w0 = kk_std_core_hnd_evv_get(_ctx); /*std/core/hnd/evv<4861>*/;
  kk_std_core_hnd__marker m0;
  int32_t _x11250 = kk_std_core_hnd_fresh_marker_int(_ctx); /*int32*/
  m0 = kk_std_core_hnd__new_Marker(_x11250, _ctx); /*std/core/hnd/marker<4861,4864>*/
  kk_std_core_hnd__ev ev;
  kk_evv_t _x11251 = kk_evv_dup(w0); /*std/core/hnd/evv<4861>*/
  ev = kk_std_core_hnd__new_Ev(kk_reuse_null, tag, m0, h, cfc0, _x11251, _ctx); /*std/core/hnd/ev<4863>*/
  kk_evv_t w1;
  kk_evv_t _x11252 = kk_evv_dup(w0); /*std/core/hnd/evv<4861>*/
  kk_std_core_hnd__ev _x11253 = kk_std_core_hnd__ev_dup(ev); /*std/core/hnd/ev<4863>*/
  w1 = kk_std_core_hnd_evv_insert(_x11252, _x11253, _ctx); /*std/core/hnd/evv<4861>*/
  kk_unit_t __ = kk_Unit;
  kk_evv_t _x11254 = kk_evv_dup(w1); /*std/core/hnd/evv<4861>*/
  kk_evv_set(_x11254,kk_context());
  kk_box_t _x11255 = kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), action, (action, _ctx)); /*4860*/
  return kk_std_core_hnd_prompt(w0, w1, ev, m0, ret, _x11255, _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_mask_at1_fun11256__t {
  struct kk_function_s _base;
  kk_ssize_t i;
  bool behind;
};
static kk_box_t kk_std_core_hnd_mask_at1_fun11256(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_mask_at1_fun11256(kk_ssize_t i, bool behind, kk_context_t* _ctx) {
  struct kk_std_core_hnd_mask_at1_fun11256__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_mask_at1_fun11256__t, 1, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_mask_at1_fun11256, kk_context());
  _self->i = i;
  _self->behind = behind;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_mask_at1_fun11256(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd_mask_at1_fun11256__t* _self = kk_function_as(struct kk_std_core_hnd_mask_at1_fun11256__t*, _fself);
  kk_ssize_t i = _self->i; /* std/core/hnd/ev-index */
  bool behind = _self->behind; /* bool */
  kk_drop_match(_self, {;;}, {}, _ctx)
  return kk_std_core_hnd_mask_at1(i, behind, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd_mask_at1(kk_ssize_t i, bool behind, kk_function_t action, kk_box_t x, kk_context_t* _ctx) { /* forall<a,b,e,e1> (i : ev-index, behind : bool, action : (a) -> e b, x : a) -> e1 b */ 
  kk_evv_t w0 = kk_std_core_hnd_evv_swap_delete(i, behind, _ctx); /*std/core/hnd/evv<_4869>*/;
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), action, (action, x, _ctx)); /*4975*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w0,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_mask_at1_fun11256(i, behind, _ctx), _ctx);
  }
  {
    return y;
  }
}


// lift anonymous function
struct kk_std_core_hnd__mask_at_fun11257__t {
  struct kk_function_s _base;
  kk_ssize_t i;
  bool behind;
};
static kk_box_t kk_std_core_hnd__mask_at_fun11257(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_mask_at_fun11257(kk_ssize_t i, bool behind, kk_context_t* _ctx) {
  struct kk_std_core_hnd__mask_at_fun11257__t* _self = kk_function_alloc_as(struct kk_std_core_hnd__mask_at_fun11257__t, 1, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__mask_at_fun11257, kk_context());
  _self->i = i;
  _self->behind = behind;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__mask_at_fun11257(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd__mask_at_fun11257__t* _self = kk_function_as(struct kk_std_core_hnd__mask_at_fun11257__t*, _fself);
  kk_ssize_t i = _self->i; /* std/core/hnd/ev-index */
  bool behind = _self->behind; /* bool */
  kk_drop_match(_self, {;;}, {}, _ctx)
  return kk_std_core_hnd_mask_at1(i, behind, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd__mask_at(kk_ssize_t i, bool behind, kk_function_t action, kk_context_t* _ctx) { /* forall<a,e,e1> (i : ev-index, behind : bool, action : () -> e a) -> e1 a */ 
  kk_evv_t w0 = kk_std_core_hnd_evv_swap_delete(i, behind, _ctx); /*std/core/hnd/evv<_4981>*/;
  kk_box_t x = kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), action, (action, _ctx)); /*5070*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w0,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(x, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd__new_mask_at_fun11257(i, behind, _ctx), _ctx);
  }
  {
    return x;
  }
}

kk_box_t kk_std_core_hnd__named_handle(kk_std_core_hnd__htag tag, int32_t cfc0, kk_box_t h, kk_function_t ret, kk_function_t action, kk_context_t* _ctx) { /* forall<a,e,e1,b,c> (tag : htag<b>, cfc : cfc, h : b<e,c>, ret : (a) -> e c, action : (ev<b>) -> e1 a) -> e c */ 
  kk_std_core_hnd__marker m0;
  int32_t _x11259 = kk_std_core_hnd_fresh_marker_named_int(_ctx); /*int32*/
  m0 = kk_std_core_hnd__new_Marker(_x11259, _ctx); /*std/core/hnd/marker<5185,5188>*/
  kk_evv_t w0 = kk_std_core_hnd_evv_get(_ctx); /*std/core/hnd/evv<5185>*/;
  kk_std_core_hnd__ev ev;
  kk_evv_t _x11260 = kk_evv_dup(w0); /*std/core/hnd/evv<5185>*/
  ev = kk_std_core_hnd__new_Ev(kk_reuse_null, tag, m0, h, cfc0, _x11260, _ctx); /*std/core/hnd/ev<5187>*/
  kk_evv_t _x11261 = kk_evv_dup(w0); /*std/core/hnd/evv<5185>*/
  kk_std_core_hnd__ev _x11262 = kk_std_core_hnd__ev_dup(ev); /*std/core/hnd/ev<5187>*/
  kk_box_t _x11263 = kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__ev, kk_context_t*), action, (action, ev, _ctx)); /*5184*/
  return kk_std_core_hnd_prompt(_x11261, w0, _x11262, m0, ret, _x11263, _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_open_at1_fun11264__t {
  struct kk_function_s _base;
  kk_ssize_t i;
};
static kk_box_t kk_std_core_hnd_open_at1_fun11264(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_open_at1_fun11264(kk_ssize_t i, kk_context_t* _ctx) {
  struct kk_std_core_hnd_open_at1_fun11264__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_open_at1_fun11264__t, 1, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_open_at1_fun11264, kk_context());
  _self->i = i;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_open_at1_fun11264(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd_open_at1_fun11264__t* _self = kk_function_as(struct kk_std_core_hnd_open_at1_fun11264__t*, _fself);
  kk_ssize_t i = _self->i; /* std/core/hnd/ev-index */
  kk_drop_match(_self, {;}, {}, _ctx)
  return kk_std_core_hnd_open_at1(i, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd_open_at1(kk_ssize_t i, kk_function_t f, kk_box_t x, kk_context_t* _ctx) { /* forall<a,b,e,e1> (i : ev-index, f : (a) -> e b, x : a) -> e1 b */ 
  kk_evv_t w = kk_evv_swap_create1(i,kk_context()); /*std/core/hnd/evv<5298>*/;
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, x, _ctx)); /*5296*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_open_at1_fun11264(i, _ctx), _ctx);
  }
  {
    return y;
  }
}


// lift anonymous function
struct kk_std_core_hnd__open_at0_fun11265__t {
  struct kk_function_s _base;
  kk_ssize_t i;
};
static kk_box_t kk_std_core_hnd__open_at0_fun11265(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_open_at0_fun11265(kk_ssize_t i, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open_at0_fun11265__t* _self = kk_function_alloc_as(struct kk_std_core_hnd__open_at0_fun11265__t, 1, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__open_at0_fun11265, kk_context());
  _self->i = i;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__open_at0_fun11265(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open_at0_fun11265__t* _self = kk_function_as(struct kk_std_core_hnd__open_at0_fun11265__t*, _fself);
  kk_ssize_t i = _self->i; /* std/core/hnd/ev-index */
  kk_drop_match(_self, {;}, {}, _ctx)
  return kk_std_core_hnd_open_at1(i, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd__open_at0(kk_ssize_t i, kk_function_t f, kk_context_t* _ctx) { /* forall<a,e,e1> (i : ev-index, f : () -> e a) -> e1 a */ 
  kk_evv_t w = kk_evv_swap_create1(i,kk_context()); /*std/core/hnd/evv<5390>*/;
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), f, (f, _ctx)); /*5388*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd__new_open_at0_fun11265(i, _ctx), _ctx);
  }
  {
    return y;
  }
}


// lift anonymous function
struct kk_std_core_hnd__open_at1_fun11266__t {
  struct kk_function_s _base;
  kk_ssize_t i;
};
static kk_box_t kk_std_core_hnd__open_at1_fun11266(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_open_at1_fun11266(kk_ssize_t i, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open_at1_fun11266__t* _self = kk_function_alloc_as(struct kk_std_core_hnd__open_at1_fun11266__t, 1, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__open_at1_fun11266, kk_context());
  _self->i = i;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__open_at1_fun11266(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open_at1_fun11266__t* _self = kk_function_as(struct kk_std_core_hnd__open_at1_fun11266__t*, _fself);
  kk_ssize_t i = _self->i; /* std/core/hnd/ev-index */
  kk_drop_match(_self, {;}, {}, _ctx)
  return kk_std_core_hnd_open_at1(i, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd__open_at1(kk_ssize_t i, kk_function_t f, kk_box_t x, kk_context_t* _ctx) { /* forall<a,b,e,e1> (i : ev-index, f : (a) -> e b, x : a) -> e1 b */ 
  kk_evv_t w = kk_evv_swap_create1(i,kk_context()); /*std/core/hnd/evv<5497>*/;
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, x, _ctx)); /*5495*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd__new_open_at1_fun11266(i, _ctx), _ctx);
  }
  {
    return y;
  }
}


// lift anonymous function
struct kk_std_core_hnd__open_at2_fun11267__t {
  struct kk_function_s _base;
  kk_ssize_t i;
};
static kk_box_t kk_std_core_hnd__open_at2_fun11267(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_open_at2_fun11267(kk_ssize_t i, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open_at2_fun11267__t* _self = kk_function_alloc_as(struct kk_std_core_hnd__open_at2_fun11267__t, 1, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__open_at2_fun11267, kk_context());
  _self->i = i;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__open_at2_fun11267(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open_at2_fun11267__t* _self = kk_function_as(struct kk_std_core_hnd__open_at2_fun11267__t*, _fself);
  kk_ssize_t i = _self->i; /* std/core/hnd/ev-index */
  kk_drop_match(_self, {;}, {}, _ctx)
  return kk_std_core_hnd_open_at1(i, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd__open_at2(kk_ssize_t i, kk_function_t f, kk_box_t x1, kk_box_t x2, kk_context_t* _ctx) { /* forall<a,b,c,e,e1> (i : ev-index, f : (a, b) -> e c, x1 : a, x2 : b) -> e1 c */ 
  kk_evv_t w = kk_evv_swap_create1(i,kk_context()); /*std/core/hnd/evv<5619>*/;
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_context_t*), f, (f, x1, x2, _ctx)); /*5617*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd__new_open_at2_fun11267(i, _ctx), _ctx);
  }
  {
    return y;
  }
}


// lift anonymous function
struct kk_std_core_hnd__open_at3_fun11268__t {
  struct kk_function_s _base;
  kk_ssize_t i;
};
static kk_box_t kk_std_core_hnd__open_at3_fun11268(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_open_at3_fun11268(kk_ssize_t i, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open_at3_fun11268__t* _self = kk_function_alloc_as(struct kk_std_core_hnd__open_at3_fun11268__t, 1, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__open_at3_fun11268, kk_context());
  _self->i = i;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__open_at3_fun11268(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open_at3_fun11268__t* _self = kk_function_as(struct kk_std_core_hnd__open_at3_fun11268__t*, _fself);
  kk_ssize_t i = _self->i; /* std/core/hnd/ev-index */
  kk_drop_match(_self, {;}, {}, _ctx)
  return kk_std_core_hnd_open_at1(i, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd__open_at3(kk_ssize_t i, kk_function_t f, kk_box_t x1, kk_box_t x2, kk_box_t x3, kk_context_t* _ctx) { /* forall<a,b,c,d,e,e1> (i : ev-index, f : (a, b, c) -> e d, x1 : a, x2 : b, x3 : c) -> e1 d */ 
  kk_evv_t w = kk_evv_swap_create1(i,kk_context()); /*std/core/hnd/evv<5693>*/;
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_context_t*), f, (f, x1, x2, x3, _ctx)); /*5691*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd__new_open_at3_fun11268(i, _ctx), _ctx);
  }
  {
    return y;
  }
}


// lift anonymous function
struct kk_std_core_hnd__open_at4_fun11269__t {
  struct kk_function_s _base;
  kk_ssize_t i;
};
static kk_box_t kk_std_core_hnd__open_at4_fun11269(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_open_at4_fun11269(kk_ssize_t i, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open_at4_fun11269__t* _self = kk_function_alloc_as(struct kk_std_core_hnd__open_at4_fun11269__t, 1, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__open_at4_fun11269, kk_context());
  _self->i = i;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__open_at4_fun11269(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open_at4_fun11269__t* _self = kk_function_as(struct kk_std_core_hnd__open_at4_fun11269__t*, _fself);
  kk_ssize_t i = _self->i; /* std/core/hnd/ev-index */
  kk_drop_match(_self, {;}, {}, _ctx)
  return kk_std_core_hnd_open_at1(i, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd__open_at4(kk_ssize_t i, kk_function_t f, kk_box_t x1, kk_box_t x2, kk_box_t x3, kk_box_t x4, kk_context_t* _ctx) { /* forall<a,b,c,d,a1,e,e1> (i : ev-index, f : (a, b, c, d) -> e a1, x1 : a, x2 : b, x3 : c, x4 : d) -> e1 a1 */ 
  kk_evv_t w = kk_evv_swap_create1(i,kk_context()); /*std/core/hnd/evv<5773>*/;
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_box_t, kk_context_t*), f, (f, x1, x2, x3, x4, _ctx)); /*5771*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd__new_open_at4_fun11269(i, _ctx), _ctx);
  }
  {
    return y;
  }
}


// lift anonymous function
struct kk_std_core_hnd_open1_fun11271__t {
  struct kk_function_s _base;
  kk_vector_t indices;
};
static kk_box_t kk_std_core_hnd_open1_fun11271(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_open1_fun11271(kk_vector_t indices, kk_context_t* _ctx) {
  struct kk_std_core_hnd_open1_fun11271__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_open1_fun11271__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_open1_fun11271, kk_context());
  _self->indices = indices;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_open1_fun11271(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd_open1_fun11271__t* _self = kk_function_as(struct kk_std_core_hnd_open1_fun11271__t*, _fself);
  kk_vector_t indices = _self->indices; /* vector<std/core/hnd/ev-index> */
  kk_drop_match(_self, {kk_vector_dup(indices);}, {}, _ctx)
  return kk_std_core_hnd_open1(indices, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd_open1(kk_vector_t indices, kk_function_t f, kk_box_t x, kk_context_t* _ctx) { /* forall<a,b,e,e1> (indices : vector<ev-index>, f : (a) -> e b, x : a) -> e1 b */ 
  kk_evv_t w;
  kk_vector_t _x11270 = kk_vector_dup(indices); /*vector<std/core/hnd/ev-index>*/
  w = kk_std_core_hnd_evv_swap_create(_x11270, _ctx); /*std/core/hnd/evv<5883>*/
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, x, _ctx)); /*5881*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_open1_fun11271(indices, _ctx), _ctx);
  }
  {
    kk_vector_drop(indices, _ctx);
    return y;
  }
}


// lift anonymous function
struct kk_std_core_hnd__open0_fun11273__t {
  struct kk_function_s _base;
  kk_vector_t indices;
};
static kk_box_t kk_std_core_hnd__open0_fun11273(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_open0_fun11273(kk_vector_t indices, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open0_fun11273__t* _self = kk_function_alloc_as(struct kk_std_core_hnd__open0_fun11273__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__open0_fun11273, kk_context());
  _self->indices = indices;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__open0_fun11273(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open0_fun11273__t* _self = kk_function_as(struct kk_std_core_hnd__open0_fun11273__t*, _fself);
  kk_vector_t indices = _self->indices; /* vector<std/core/hnd/ev-index> */
  kk_drop_match(_self, {kk_vector_dup(indices);}, {}, _ctx)
  return kk_std_core_hnd_open1(indices, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd__open0(kk_vector_t indices, kk_function_t f, kk_context_t* _ctx) { /* forall<a,e,e1> (indices : vector<ev-index>, f : () -> e a) -> e1 a */ 
  kk_evv_t w;
  kk_vector_t _x11272 = kk_vector_dup(indices); /*vector<std/core/hnd/ev-index>*/
  w = kk_std_core_hnd_evv_swap_create(_x11272, _ctx); /*std/core/hnd/evv<5975>*/
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), f, (f, _ctx)); /*5973*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd__new_open0_fun11273(indices, _ctx), _ctx);
  }
  {
    kk_vector_drop(indices, _ctx);
    return y;
  }
}


// lift anonymous function
struct kk_std_core_hnd__open1_fun11275__t {
  struct kk_function_s _base;
  kk_vector_t indices;
};
static kk_box_t kk_std_core_hnd__open1_fun11275(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_open1_fun11275(kk_vector_t indices, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open1_fun11275__t* _self = kk_function_alloc_as(struct kk_std_core_hnd__open1_fun11275__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__open1_fun11275, kk_context());
  _self->indices = indices;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__open1_fun11275(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open1_fun11275__t* _self = kk_function_as(struct kk_std_core_hnd__open1_fun11275__t*, _fself);
  kk_vector_t indices = _self->indices; /* vector<std/core/hnd/ev-index> */
  kk_drop_match(_self, {kk_vector_dup(indices);}, {}, _ctx)
  return kk_std_core_hnd_open1(indices, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd__open1(kk_vector_t indices, kk_function_t f, kk_box_t x, kk_context_t* _ctx) { /* forall<a,b,e,e1> (indices : vector<ev-index>, f : (a) -> e b, x : a) -> e1 b */ 
  kk_evv_t w;
  kk_vector_t _x11274 = kk_vector_dup(indices); /*vector<std/core/hnd/ev-index>*/
  w = kk_std_core_hnd_evv_swap_create(_x11274, _ctx); /*std/core/hnd/evv<6082>*/
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, x, _ctx)); /*6080*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd__new_open1_fun11275(indices, _ctx), _ctx);
  }
  {
    kk_vector_drop(indices, _ctx);
    return y;
  }
}


// lift anonymous function
struct kk_std_core_hnd__open2_fun11277__t {
  struct kk_function_s _base;
  kk_vector_t indices;
};
static kk_box_t kk_std_core_hnd__open2_fun11277(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_open2_fun11277(kk_vector_t indices, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open2_fun11277__t* _self = kk_function_alloc_as(struct kk_std_core_hnd__open2_fun11277__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__open2_fun11277, kk_context());
  _self->indices = indices;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__open2_fun11277(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open2_fun11277__t* _self = kk_function_as(struct kk_std_core_hnd__open2_fun11277__t*, _fself);
  kk_vector_t indices = _self->indices; /* vector<std/core/hnd/ev-index> */
  kk_drop_match(_self, {kk_vector_dup(indices);}, {}, _ctx)
  return kk_std_core_hnd_open1(indices, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd__open2(kk_vector_t indices, kk_function_t f, kk_box_t x1, kk_box_t x2, kk_context_t* _ctx) { /* forall<a,b,c,e,e1> (indices : vector<ev-index>, f : (a, b) -> e c, x1 : a, x2 : b) -> e1 c */ 
  kk_evv_t w;
  kk_vector_t _x11276 = kk_vector_dup(indices); /*vector<std/core/hnd/ev-index>*/
  w = kk_std_core_hnd_evv_swap_create(_x11276, _ctx); /*std/core/hnd/evv<6204>*/
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_context_t*), f, (f, x1, x2, _ctx)); /*6202*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd__new_open2_fun11277(indices, _ctx), _ctx);
  }
  {
    kk_vector_drop(indices, _ctx);
    return y;
  }
}


// lift anonymous function
struct kk_std_core_hnd__open3_fun11279__t {
  struct kk_function_s _base;
  kk_vector_t indices;
};
static kk_box_t kk_std_core_hnd__open3_fun11279(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_open3_fun11279(kk_vector_t indices, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open3_fun11279__t* _self = kk_function_alloc_as(struct kk_std_core_hnd__open3_fun11279__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__open3_fun11279, kk_context());
  _self->indices = indices;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__open3_fun11279(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open3_fun11279__t* _self = kk_function_as(struct kk_std_core_hnd__open3_fun11279__t*, _fself);
  kk_vector_t indices = _self->indices; /* vector<std/core/hnd/ev-index> */
  kk_drop_match(_self, {kk_vector_dup(indices);}, {}, _ctx)
  return kk_std_core_hnd_open1(indices, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd__open3(kk_vector_t indices, kk_function_t f, kk_box_t x1, kk_box_t x2, kk_box_t x3, kk_context_t* _ctx) { /* forall<a,b,c,d,e,e1> (indices : vector<ev-index>, f : (a, b, c) -> e d, x1 : a, x2 : b, x3 : c) -> e1 d */ 
  kk_evv_t w;
  kk_vector_t _x11278 = kk_vector_dup(indices); /*vector<std/core/hnd/ev-index>*/
  w = kk_std_core_hnd_evv_swap_create(_x11278, _ctx); /*std/core/hnd/evv<6278>*/
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_context_t*), f, (f, x1, x2, x3, _ctx)); /*6276*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd__new_open3_fun11279(indices, _ctx), _ctx);
  }
  {
    kk_vector_drop(indices, _ctx);
    return y;
  }
}


// lift anonymous function
struct kk_std_core_hnd__open4_fun11281__t {
  struct kk_function_s _base;
  kk_vector_t indices;
};
static kk_box_t kk_std_core_hnd__open4_fun11281(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_open4_fun11281(kk_vector_t indices, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open4_fun11281__t* _self = kk_function_alloc_as(struct kk_std_core_hnd__open4_fun11281__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__open4_fun11281, kk_context());
  _self->indices = indices;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__open4_fun11281(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open4_fun11281__t* _self = kk_function_as(struct kk_std_core_hnd__open4_fun11281__t*, _fself);
  kk_vector_t indices = _self->indices; /* vector<std/core/hnd/ev-index> */
  kk_drop_match(_self, {kk_vector_dup(indices);}, {}, _ctx)
  return kk_std_core_hnd_open1(indices, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd__open4(kk_vector_t indices, kk_function_t f, kk_box_t x1, kk_box_t x2, kk_box_t x3, kk_box_t x4, kk_context_t* _ctx) { /* forall<a,b,c,d,a1,e,e1> (indices : vector<ev-index>, f : (a, b, c, d) -> e a1, x1 : a, x2 : b, x3 : c, x4 : d) -> e1 a1 */ 
  kk_evv_t w;
  kk_vector_t _x11280 = kk_vector_dup(indices); /*vector<std/core/hnd/ev-index>*/
  w = kk_std_core_hnd_evv_swap_create(_x11280, _ctx); /*std/core/hnd/evv<6358>*/
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_box_t, kk_context_t*), f, (f, x1, x2, x3, x4, _ctx)); /*6356*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd__new_open4_fun11281(indices, _ctx), _ctx);
  }
  {
    kk_vector_drop(indices, _ctx);
    return y;
  }
}


// lift anonymous function
struct kk_std_core_hnd_yield_to_fun11288__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_hnd_yield_to_fun11288(kk_function_t _fself, kk_box_t _b_10957, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_yield_to_fun11288(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_hnd_yield_to_fun11288, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_hnd_yield_to_fun11288(kk_function_t _fself, kk_box_t _b_10957, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_function_t _x11289 = kk_function_unbox(_b_10957); /*() -> 6490 10958*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), _x11289, (_x11289, _ctx));
}

kk_box_t kk_std_core_hnd_yield_to(kk_std_core_hnd__marker m0, kk_function_t clause0, kk_context_t* _ctx) { /* forall<a,e,b> (m : marker<e,b>, clause : ((resume-result<a,b>) -> e b) -> e b) -> e a */ 
  kk_function_t g = kk_std_core_hnd_yield_to_prim(m0, clause0, _ctx); /*() -> 6489*/;
  kk_function_drop(g, _ctx);
  return kk_std_core_hnd_yield_extend(kk_std_core_hnd_new_yield_to_fun11288(_ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_control_raw0_fun11290__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_control_raw0_fun11290(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__550__16, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control_raw0_fun11290(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw0_fun11290__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control_raw0_fun11290__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control_raw0_fun11290, kk_context());
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_control_raw0_fun11291__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_control_raw0_fun11291(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control_raw0_fun11291(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw0_fun11291__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control_raw0_fun11291__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control_raw0_fun11291, kk_context());
  _self->op = op;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_control_raw0_fun11291(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw0_fun11291__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control_raw0_fun11291__t*, _fself);
  kk_function_t op = _self->op; /* (std/core/hnd/resume-context<6530,6531,6532,6534>) -> 6531 6534 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__resume_context _x11292 = kk_std_core_hnd__new_Resume_context(k0, _ctx); /*std/core/hnd/resume-context<89,90,91,92>*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__resume_context, kk_context_t*), op, (op, _x11292, _ctx));
}
static kk_box_t kk_std_core_hnd_clause_control_raw0_fun11290(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__550__16, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw0_fun11290__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control_raw0_fun11290__t*, _fself);
  kk_function_t op = _self->op; /* (std/core/hnd/resume-context<6530,6531,6532,6534>) -> 6531 6534 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(___wildcard__550__16, (KK_I32(3)), _ctx);
  return kk_std_core_hnd_yield_to(m0, kk_std_core_hnd_new_clause_control_raw0_fun11291(op, _ctx), _ctx);
}

kk_std_core_hnd__clause0 kk_std_core_hnd_clause_control_raw0(kk_function_t op, kk_context_t* _ctx) { /* forall<a,e,e1,b,c> (op : (resume-context<a,e,e1,c>) -> e c) -> clause0<a,b,e,c> */ 
  return kk_std_core_hnd__new_Clause0(kk_std_core_hnd_new_clause_control_raw0_fun11290(op, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_control_raw1_fun11293__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_control_raw1_fun11293(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__487__16, kk_box_t x, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control_raw1_fun11293(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw1_fun11293__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control_raw1_fun11293__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control_raw1_fun11293, kk_context());
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_control_raw1_fun11294__t {
  struct kk_function_s _base;
  kk_function_t op;
  kk_box_t x;
};
static kk_box_t kk_std_core_hnd_clause_control_raw1_fun11294(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control_raw1_fun11294(kk_function_t op, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw1_fun11294__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control_raw1_fun11294__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control_raw1_fun11294, kk_context());
  _self->op = op;
  _self->x = x;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_control_raw1_fun11294(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw1_fun11294__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control_raw1_fun11294__t*, _fself);
  kk_function_t op = _self->op; /* (x : 6578, r : std/core/hnd/resume-context<6579,6580,6581,6583>) -> 6580 6583 */
  kk_box_t x = _self->x; /* 6578 */
  kk_drop_match(_self, {kk_function_dup(op);kk_box_dup(x);}, {}, _ctx)
  kk_std_core_hnd__resume_context _x11295 = kk_std_core_hnd__new_Resume_context(k0, _ctx); /*std/core/hnd/resume-context<89,90,91,92>*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_std_core_hnd__resume_context, kk_context_t*), op, (op, x, _x11295, _ctx));
}
static kk_box_t kk_std_core_hnd_clause_control_raw1_fun11293(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__487__16, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw1_fun11293__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control_raw1_fun11293__t*, _fself);
  kk_function_t op = _self->op; /* (x : 6578, r : std/core/hnd/resume-context<6579,6580,6581,6583>) -> 6580 6583 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(___wildcard__487__16, (KK_I32(3)), _ctx);
  return kk_std_core_hnd_yield_to(m0, kk_std_core_hnd_new_clause_control_raw1_fun11294(op, x, _ctx), _ctx);
}

kk_std_core_hnd__clause1 kk_std_core_hnd_clause_control_raw1(kk_function_t op, kk_context_t* _ctx) { /* forall<a,b,e,e1,c,d> (op : (x : a, r : resume-context<b,e,e1,d>) -> e d) -> clause1<a,b,c,e,d> */ 
  return kk_std_core_hnd__new_Clause1(kk_std_core_hnd_new_clause_control_raw1_fun11293(op, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_control_raw2_fun11296__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_control_raw2_fun11296(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__601__16, kk_box_t x1, kk_box_t x2, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control_raw2_fun11296(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw2_fun11296__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control_raw2_fun11296__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control_raw2_fun11296, kk_context());
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_control_raw2_fun11297__t {
  struct kk_function_s _base;
  kk_function_t op;
  kk_box_t x1;
  kk_box_t x2;
};
static kk_box_t kk_std_core_hnd_clause_control_raw2_fun11297(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control_raw2_fun11297(kk_function_t op, kk_box_t x1, kk_box_t x2, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw2_fun11297__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control_raw2_fun11297__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control_raw2_fun11297, kk_context());
  _self->op = op;
  _self->x1 = x1;
  _self->x2 = x2;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_control_raw2_fun11297(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw2_fun11297__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control_raw2_fun11297__t*, _fself);
  kk_function_t op = _self->op; /* (x1 : 6632, x2 : 6633, r : std/core/hnd/resume-context<6634,6635,6636,6638>) -> 6635 6638 */
  kk_box_t x1 = _self->x1; /* 6632 */
  kk_box_t x2 = _self->x2; /* 6633 */
  kk_drop_match(_self, {kk_function_dup(op);kk_box_dup(x1);kk_box_dup(x2);}, {}, _ctx)
  kk_std_core_hnd__resume_context _x11298 = kk_std_core_hnd__new_Resume_context(k0, _ctx); /*std/core/hnd/resume-context<89,90,91,92>*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_std_core_hnd__resume_context, kk_context_t*), op, (op, x1, x2, _x11298, _ctx));
}
static kk_box_t kk_std_core_hnd_clause_control_raw2_fun11296(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__601__16, kk_box_t x1, kk_box_t x2, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw2_fun11296__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control_raw2_fun11296__t*, _fself);
  kk_function_t op = _self->op; /* (x1 : 6632, x2 : 6633, r : std/core/hnd/resume-context<6634,6635,6636,6638>) -> 6635 6638 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(___wildcard__601__16, (KK_I32(3)), _ctx);
  return kk_std_core_hnd_yield_to(m0, kk_std_core_hnd_new_clause_control_raw2_fun11297(op, x1, x2, _ctx), _ctx);
}

kk_std_core_hnd__clause2 kk_std_core_hnd_clause_control_raw2(kk_function_t op, kk_context_t* _ctx) { /* forall<a,b,c,e,e1,d,a1> (op : (x1 : a, x2 : b, r : resume-context<c,e,e1,a1>) -> e a1) -> clause2<a,b,c,d,e,a1> */ 
  return kk_std_core_hnd__new_Clause2(kk_std_core_hnd_new_clause_control_raw2_fun11296(op, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_control_raw3_fun11299__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_control_raw3_fun11299(kk_function_t _fself, kk_std_core_hnd__marker _b_10962, kk_std_core_hnd__ev _b_10963, kk_box_t _b_10964, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control_raw3_fun11299(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw3_fun11299__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control_raw3_fun11299__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control_raw3_fun11299, kk_context());
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_control_raw3_fun11300__t {
  struct kk_function_s _base;
  kk_box_t _b_10964;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_control_raw3_fun11300(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control_raw3_fun11300(kk_box_t _b_10964, kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw3_fun11300__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control_raw3_fun11300__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control_raw3_fun11300, kk_context());
  _self->_b_10964 = _b_10964;
  _self->op = op;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_control_raw3_fun11300(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw3_fun11300__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control_raw3_fun11300__t*, _fself);
  kk_box_t _b_10964 = _self->_b_10964; /* 51 */
  kk_function_t op = _self->op; /* (x1 : 6873, x2 : 6874, x3 : 6875, r : std/core/hnd/resume-context<6876,6877,6878,6880>) -> 6877 6880 */
  kk_drop_match(_self, {kk_box_dup(_b_10964);kk_function_dup(op);}, {}, _ctx)
  kk_box_t _x11301;
  kk_std_core_types__tuple3_ _brw_11156;
  kk_box_t _x11302 = kk_box_dup(_b_10964); /*51*/
  _brw_11156 = kk_std_core_types__tuple3__unbox(_x11302, _ctx); /*(6873, 6874, 6875)*/
  kk_box_t _brw_11157 = kk_std_core_types_fst_1(_brw_11156, _ctx); /*1000*/;
  kk_std_core_types__tuple3__drop(_brw_11156, _ctx);
  _x11301 = _brw_11157; /*1000*/
  kk_box_t _x11303;
  kk_std_core_types__tuple3_ _brw_11154;
  kk_box_t _x11304 = kk_box_dup(_b_10964); /*51*/
  _brw_11154 = kk_std_core_types__tuple3__unbox(_x11304, _ctx); /*(6873, 6874, 6875)*/
  kk_box_t _brw_11155 = kk_std_core_types_snd_1(_brw_11154, _ctx); /*1001*/;
  kk_std_core_types__tuple3__drop(_brw_11154, _ctx);
  _x11303 = _brw_11155; /*1001*/
  kk_box_t _x11305;
  kk_std_core_types__tuple3_ _brw_11152 = kk_std_core_types__tuple3__unbox(_b_10964, _ctx); /*(6873, 6874, 6875)*/;
  kk_box_t _brw_11153 = kk_std_core_types_thd(_brw_11152, _ctx); /*1002*/;
  kk_std_core_types__tuple3__drop(_brw_11152, _ctx);
  _x11305 = _brw_11153; /*1002*/
  kk_std_core_hnd__resume_context _x11306 = kk_std_core_hnd__new_Resume_context(k0, _ctx); /*std/core/hnd/resume-context<89,90,91,92>*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_std_core_hnd__resume_context, kk_context_t*), op, (op, _x11301, _x11303, _x11305, _x11306, _ctx));
}
static kk_box_t kk_std_core_hnd_clause_control_raw3_fun11299(kk_function_t _fself, kk_std_core_hnd__marker _b_10962, kk_std_core_hnd__ev _b_10963, kk_box_t _b_10964, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw3_fun11299__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control_raw3_fun11299__t*, _fself);
  kk_function_t op = _self->op; /* (x1 : 6873, x2 : 6874, x3 : 6875, r : std/core/hnd/resume-context<6876,6877,6878,6880>) -> 6877 6880 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(_b_10963, (KK_I32(3)), _ctx);
  return kk_std_core_hnd_yield_to(_b_10962, kk_std_core_hnd_new_clause_control_raw3_fun11300(_b_10964, op, _ctx), _ctx);
}

kk_std_core_hnd__clause1 kk_std_core_hnd_clause_control_raw3(kk_function_t op, kk_context_t* _ctx) { /* forall<a,b,c,d,e,e1,a1,b1> (op : (x1 : a, x2 : b, x3 : c, r : resume-context<d,e,e1,b1>) -> e b1) -> clause1<(a, b, c),d,a1,e,b1> */ 
  return kk_std_core_hnd__new_Clause1(kk_std_core_hnd_new_clause_control_raw3_fun11299(op, _ctx), _ctx);
}

kk_box_t kk_std_core_hnd_protect_check(kk_ref_t resumed, kk_function_t k0, kk_box_t res, kk_context_t* _ctx) { /* forall<a,e,b> (resumed : ref<global,bool>, k : (resume-result<a,b>) -> e b, res : b) -> e b */ 
  bool did_resume;
  kk_box_t _x11309 = kk_ref_get(resumed,kk_context()); /*1001*/
  did_resume = kk_bool_unbox(_x11309); /*bool*/
  if (did_resume) {
    kk_function_drop(k0, _ctx);
    return res;
  }
  {
    kk_std_core_hnd__resume_result _x11310 = kk_std_core_hnd__new_Finalize(res, _ctx); /*std/core/hnd/resume-result<80,81>*/
    return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__resume_result, kk_context_t*), k0, (k0, _x11310, _ctx));
  }
}


// lift anonymous function
struct kk_std_core_hnd_protect_fun11312__t {
  struct kk_function_s _base;
  kk_function_t k0;
  kk_ref_t resumed;
};
static kk_box_t kk_std_core_hnd_protect_fun11312(kk_function_t _fself, kk_box_t ret, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_protect_fun11312(kk_function_t k0, kk_ref_t resumed, kk_context_t* _ctx) {
  struct kk_std_core_hnd_protect_fun11312__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_protect_fun11312__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_protect_fun11312, kk_context());
  _self->k0 = k0;
  _self->resumed = resumed;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_protect_fun11312(kk_function_t _fself, kk_box_t ret, kk_context_t* _ctx) {
  struct kk_std_core_hnd_protect_fun11312__t* _self = kk_function_as(struct kk_std_core_hnd_protect_fun11312__t*, _fself);
  kk_function_t k0 = _self->k0; /* (std/core/hnd/resume-result<7088,7090>) -> 7089 7090 */
  kk_ref_t resumed = _self->resumed; /* ref<global,bool> */
  kk_drop_match(_self, {kk_function_dup(k0);kk_ref_dup(resumed);}, {}, _ctx)
  kk_unit_t __ = kk_Unit;
  kk_unit_t _brw_11150 = kk_Unit;
  kk_ref_set_borrow(resumed,(kk_bool_box(true)),kk_context());
  kk_ref_drop(resumed, _ctx);
  _brw_11150;
  kk_std_core_hnd__resume_result _x11313 = kk_std_core_hnd__new_Deep(ret, _ctx); /*std/core/hnd/resume-result<80,81>*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__resume_result, kk_context_t*), k0, (k0, _x11313, _ctx));
}


// lift anonymous function
struct kk_std_core_hnd_protect_fun11314__t {
  struct kk_function_s _base;
  kk_function_t k0;
  kk_ref_t resumed;
};
static kk_box_t kk_std_core_hnd_protect_fun11314(kk_function_t _fself, kk_box_t xres, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_protect_fun11314(kk_function_t k0, kk_ref_t resumed, kk_context_t* _ctx) {
  struct kk_std_core_hnd_protect_fun11314__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_protect_fun11314__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_protect_fun11314, kk_context());
  _self->k0 = k0;
  _self->resumed = resumed;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_protect_fun11314(kk_function_t _fself, kk_box_t xres, kk_context_t* _ctx) {
  struct kk_std_core_hnd_protect_fun11314__t* _self = kk_function_as(struct kk_std_core_hnd_protect_fun11314__t*, _fself);
  kk_function_t k0 = _self->k0; /* (std/core/hnd/resume-result<7088,7090>) -> 7089 7090 */
  kk_ref_t resumed = _self->resumed; /* ref<global,bool> */
  kk_drop_match(_self, {kk_function_dup(k0);kk_ref_dup(resumed);}, {}, _ctx)
  return kk_std_core_hnd_protect_check(resumed, k0, xres, _ctx);
}

kk_box_t kk_std_core_hnd_protect(kk_box_t x, kk_function_t clause0, kk_function_t k0, kk_context_t* _ctx) { /* forall<a,b,e,c> (x : a, clause : (x : a, k : (b) -> e c) -> e c, k : (resume-result<b,c>) -> e c) -> e c */ 
  kk_ref_t resumed = kk_ref_alloc((kk_bool_box(false)),kk_context()); /*ref<global,bool>*/;
  kk_box_t res;
  kk_function_t _x11311;
  kk_function_dup(k0);
  kk_ref_dup(resumed);
  _x11311 = kk_std_core_hnd_new_protect_fun11312(k0, resumed, _ctx); /*(ret : 7088) -> 7089 7090*/
  res = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_function_t, kk_context_t*), clause0, (clause0, x, _x11311, _ctx)); /*7090*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(res, _ctx);
    return kk_std_core_hnd_yield_extend(kk_std_core_hnd_new_protect_fun11314(k0, resumed, _ctx), _ctx);
  }
  {
    return kk_std_core_hnd_protect_check(resumed, k0, res, _ctx);
  }
}


// lift anonymous function
struct kk_std_core_hnd_protect_fun11316__t_1 {
  struct kk_function_s _base;
  kk_function_t k0;
  kk_ref_t resumed;
};
static kk_box_t kk_std_core_hnd_protect_fun11316_1(kk_function_t _fself, kk_box_t ret, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_protect_fun11316_1(kk_function_t k0, kk_ref_t resumed, kk_context_t* _ctx) {
  struct kk_std_core_hnd_protect_fun11316__t_1* _self = kk_function_alloc_as(struct kk_std_core_hnd_protect_fun11316__t_1, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_protect_fun11316_1, kk_context());
  _self->k0 = k0;
  _self->resumed = resumed;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_protect_fun11316_1(kk_function_t _fself, kk_box_t ret, kk_context_t* _ctx) {
  struct kk_std_core_hnd_protect_fun11316__t_1* _self = kk_function_as(struct kk_std_core_hnd_protect_fun11316__t_1*, _fself);
  kk_function_t k0 = _self->k0; /* (std/core/hnd/resume-result<7181,7183>) -> 7182 7183 */
  kk_ref_t resumed = _self->resumed; /* ref<global,bool> */
  kk_drop_match(_self, {kk_function_dup(k0);kk_ref_dup(resumed);}, {}, _ctx)
  kk_unit_t __ = kk_Unit;
  kk_unit_t _brw_11148 = kk_Unit;
  kk_ref_set_borrow(resumed,(kk_bool_box(true)),kk_context());
  kk_ref_drop(resumed, _ctx);
  _brw_11148;
  kk_std_core_hnd__resume_result _x11317 = kk_std_core_hnd__new_Deep(ret, _ctx); /*std/core/hnd/resume-result<80,81>*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__resume_result, kk_context_t*), k0, (k0, _x11317, _ctx));
}


// lift anonymous function
struct kk_std_core_hnd_protect_fun11318__t_1 {
  struct kk_function_s _base;
  kk_function_t k0;
  kk_ref_t resumed;
};
static kk_box_t kk_std_core_hnd_protect_fun11318_1(kk_function_t _fself, kk_box_t xres, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_protect_fun11318_1(kk_function_t k0, kk_ref_t resumed, kk_context_t* _ctx) {
  struct kk_std_core_hnd_protect_fun11318__t_1* _self = kk_function_alloc_as(struct kk_std_core_hnd_protect_fun11318__t_1, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_protect_fun11318_1, kk_context());
  _self->k0 = k0;
  _self->resumed = resumed;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_protect_fun11318_1(kk_function_t _fself, kk_box_t xres, kk_context_t* _ctx) {
  struct kk_std_core_hnd_protect_fun11318__t_1* _self = kk_function_as(struct kk_std_core_hnd_protect_fun11318__t_1*, _fself);
  kk_function_t k0 = _self->k0; /* (std/core/hnd/resume-result<7181,7183>) -> 7182 7183 */
  kk_ref_t resumed = _self->resumed; /* ref<global,bool> */
  kk_drop_match(_self, {kk_function_dup(k0);kk_ref_dup(resumed);}, {}, _ctx)
  return kk_std_core_hnd_protect_check(resumed, k0, xres, _ctx);
}

kk_box_t kk_std_core_hnd_protect_1(kk_box_t x1, kk_box_t x2, kk_function_t clause0, kk_function_t k0, kk_context_t* _ctx) { /* forall<a,b,c,e,d> (x1 : a, x2 : b, clause : (x : a, x : b, k : (c) -> e d) -> e d, k : (resume-result<c,d>) -> e d) -> e d */ 
  kk_ref_t resumed = kk_ref_alloc((kk_bool_box(false)),kk_context()); /*ref<global,bool>*/;
  kk_box_t res;
  kk_function_t _x11315;
  kk_function_dup(k0);
  kk_ref_dup(resumed);
  _x11315 = kk_std_core_hnd_new_protect_fun11316_1(k0, resumed, _ctx); /*(ret : 7181) -> 7182 7183*/
  res = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_function_t, kk_context_t*), clause0, (clause0, x1, x2, _x11315, _ctx)); /*7183*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(res, _ctx);
    return kk_std_core_hnd_yield_extend(kk_std_core_hnd_new_protect_fun11318_1(k0, resumed, _ctx), _ctx);
  }
  {
    return kk_std_core_hnd_protect_check(resumed, k0, res, _ctx);
  }
}


// lift anonymous function
struct kk_std_core_hnd_clause_control0_fun11319__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_control0_fun11319(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__558__16, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control0_fun11319(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control0_fun11319__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control0_fun11319__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control0_fun11319, kk_context());
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_control0_fun11320__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_control0_fun11320(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control0_fun11320(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control0_fun11320__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control0_fun11320__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control0_fun11320, kk_context());
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_control0_fun11321__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_control0_fun11321(kk_function_t _fself, kk_box_t _b_10986, kk_function_t _b_10987, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control0_fun11321(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control0_fun11321__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control0_fun11321__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control0_fun11321, kk_context());
  _self->op = op;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_control0_fun11321(kk_function_t _fself, kk_box_t _b_10986, kk_function_t _b_10987, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control0_fun11321__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control0_fun11321__t*, _fself);
  kk_function_t op = _self->op; /* ((7284) -> 7285 7287) -> 7285 7287 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_box_drop(_b_10986, _ctx);
  return kk_function_call(kk_box_t, (kk_function_t, kk_function_t, kk_context_t*), op, (op, _b_10987, _ctx));
}
static kk_box_t kk_std_core_hnd_clause_control0_fun11320(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control0_fun11320__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control0_fun11320__t*, _fself);
  kk_function_t op = _self->op; /* ((7284) -> 7285 7287) -> 7285 7287 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  return kk_std_core_hnd_protect(kk_unit_box(kk_Unit), kk_std_core_hnd_new_clause_control0_fun11321(op, _ctx), k0, _ctx);
}
static kk_box_t kk_std_core_hnd_clause_control0_fun11319(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__558__16, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control0_fun11319__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control0_fun11319__t*, _fself);
  kk_function_t op = _self->op; /* ((7284) -> 7285 7287) -> 7285 7287 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(___wildcard__558__16, (KK_I32(3)), _ctx);
  return kk_std_core_hnd_yield_to(m0, kk_std_core_hnd_new_clause_control0_fun11320(op, _ctx), _ctx);
}

kk_std_core_hnd__clause0 kk_std_core_hnd_clause_control0(kk_function_t op, kk_context_t* _ctx) { /* forall<a,e,b,c> (op : ((a) -> e c) -> e c) -> clause0<a,b,e,c> */ 
  return kk_std_core_hnd__new_Clause0(kk_std_core_hnd_new_clause_control0_fun11319(op, _ctx), _ctx);
}
extern kk_box_t kk_std_core_hnd_clause_control1_fun11323(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control1_fun11323__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control1_fun11323__t*, _fself);
  kk_function_t clause0 = _self->clause0; /* (x : 7357, k : (7358) -> 7359 7361) -> 7359 7361 */
  kk_box_t x = _self->x; /* 7357 */
  kk_drop_match(_self, {kk_function_dup(clause0);kk_box_dup(x);}, {}, _ctx)
  return kk_std_core_hnd_protect(x, clause0, k0, _ctx);
}
extern kk_box_t kk_std_core_hnd_clause_control1_fun11322(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__516__16, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control1_fun11322__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control1_fun11322__t*, _fself);
  kk_function_t clause0 = _self->clause0; /* (x : 7357, k : (7358) -> 7359 7361) -> 7359 7361 */
  kk_drop_match(_self, {kk_function_dup(clause0);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(___wildcard__516__16, (KK_I32(3)), _ctx);
  return kk_std_core_hnd_yield_to(m0, kk_std_core_hnd_new_clause_control1_fun11323(clause0, x, _ctx), _ctx);
}
extern kk_box_t kk_std_core_hnd_clause_control2_fun11325(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control2_fun11325__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control2_fun11325__t*, _fself);
  kk_function_t clause0 = _self->clause0; /* (x1 : 7440, x2 : 7441, k : (7442) -> 7443 7445) -> 7443 7445 */
  kk_box_t x1 = _self->x1; /* 7440 */
  kk_box_t x2 = _self->x2; /* 7441 */
  kk_drop_match(_self, {kk_function_dup(clause0);kk_box_dup(x1);kk_box_dup(x2);}, {}, _ctx)
  return kk_std_core_hnd_protect_1(x1, x2, clause0, k0, _ctx);
}
extern kk_box_t kk_std_core_hnd_clause_control2_fun11324(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__597__16, kk_box_t x1, kk_box_t x2, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control2_fun11324__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control2_fun11324__t*, _fself);
  kk_function_t clause0 = _self->clause0; /* (x1 : 7440, x2 : 7441, k : (7442) -> 7443 7445) -> 7443 7445 */
  kk_drop_match(_self, {kk_function_dup(clause0);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(___wildcard__597__16, (KK_I32(3)), _ctx);
  return kk_std_core_hnd_yield_to(m0, kk_std_core_hnd_new_clause_control2_fun11325(clause0, x1, x2, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_control3_fun11326__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_control3_fun11326(kk_function_t _fself, kk_std_core_hnd__marker _b_10999, kk_std_core_hnd__ev _b_11000, kk_box_t _b_11001, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control3_fun11326(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control3_fun11326__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control3_fun11326__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control3_fun11326, kk_context());
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_control3_fun11327__t {
  struct kk_function_s _base;
  kk_box_t _b_11001;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_control3_fun11327(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control3_fun11327(kk_box_t _b_11001, kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control3_fun11327__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control3_fun11327__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control3_fun11327, kk_context());
  _self->_b_11001 = _b_11001;
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_control3_fun11328__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_control3_fun11328(kk_function_t _fself, kk_box_t _b_10996, kk_function_t _b_10997, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control3_fun11328(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control3_fun11328__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control3_fun11328__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control3_fun11328, kk_context());
  _self->op = op;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_control3_fun11328(kk_function_t _fself, kk_box_t _b_10996, kk_function_t _b_10997, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control3_fun11328__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control3_fun11328__t*, _fself);
  kk_function_t op = _self->op; /* (x1 : 7676, x2 : 7677, x3 : 7678, k : (7679) -> 7680 7682) -> 7680 7682 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_box_t _x11329;
  kk_std_core_types__tuple3_ _brw_11145;
  kk_box_t _x11330 = kk_box_dup(_b_10996); /*7087*/
  _brw_11145 = kk_std_core_types__tuple3__unbox(_x11330, _ctx); /*(7676, 7677, 7678)*/
  kk_box_t _brw_11146 = kk_std_core_types_fst_1(_brw_11145, _ctx); /*1000*/;
  kk_std_core_types__tuple3__drop(_brw_11145, _ctx);
  _x11329 = _brw_11146; /*1000*/
  kk_box_t _x11331;
  kk_std_core_types__tuple3_ _brw_11143;
  kk_box_t _x11332 = kk_box_dup(_b_10996); /*7087*/
  _brw_11143 = kk_std_core_types__tuple3__unbox(_x11332, _ctx); /*(7676, 7677, 7678)*/
  kk_box_t _brw_11144 = kk_std_core_types_snd_1(_brw_11143, _ctx); /*1001*/;
  kk_std_core_types__tuple3__drop(_brw_11143, _ctx);
  _x11331 = _brw_11144; /*1001*/
  kk_box_t _x11333;
  kk_std_core_types__tuple3_ _brw_11141 = kk_std_core_types__tuple3__unbox(_b_10996, _ctx); /*(7676, 7677, 7678)*/;
  kk_box_t _brw_11142 = kk_std_core_types_thd(_brw_11141, _ctx); /*1002*/;
  kk_std_core_types__tuple3__drop(_brw_11141, _ctx);
  _x11333 = _brw_11142; /*1002*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_function_t, kk_context_t*), op, (op, _x11329, _x11331, _x11333, _b_10997, _ctx));
}
static kk_box_t kk_std_core_hnd_clause_control3_fun11327(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control3_fun11327__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control3_fun11327__t*, _fself);
  kk_box_t _b_11001 = _self->_b_11001; /* 51 */
  kk_function_t op = _self->op; /* (x1 : 7676, x2 : 7677, x3 : 7678, k : (7679) -> 7680 7682) -> 7680 7682 */
  kk_drop_match(_self, {kk_box_dup(_b_11001);kk_function_dup(op);}, {}, _ctx)
  return kk_std_core_hnd_protect(_b_11001, kk_std_core_hnd_new_clause_control3_fun11328(op, _ctx), k0, _ctx);
}
static kk_box_t kk_std_core_hnd_clause_control3_fun11326(kk_function_t _fself, kk_std_core_hnd__marker _b_10999, kk_std_core_hnd__ev _b_11000, kk_box_t _b_11001, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control3_fun11326__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control3_fun11326__t*, _fself);
  kk_function_t op = _self->op; /* (x1 : 7676, x2 : 7677, x3 : 7678, k : (7679) -> 7680 7682) -> 7680 7682 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(_b_11000, (KK_I32(3)), _ctx);
  return kk_std_core_hnd_yield_to(_b_10999, kk_std_core_hnd_new_clause_control3_fun11327(_b_11001, op, _ctx), _ctx);
}

kk_std_core_hnd__clause1 kk_std_core_hnd_clause_control3(kk_function_t op, kk_context_t* _ctx) { /* forall<a,b,c,d,e,a1,b1> (op : (x1 : a, x2 : b, x3 : c, k : (d) -> e b1) -> e b1) -> clause1<(a, b, c),d,a1,e,b1> */ 
  return kk_std_core_hnd__new_Clause1(kk_std_core_hnd_new_clause_control3_fun11326(op, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_control4_fun11334__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_control4_fun11334(kk_function_t _fself, kk_std_core_hnd__marker _b_11017, kk_std_core_hnd__ev _b_11018, kk_box_t _b_11019, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control4_fun11334(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control4_fun11334__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control4_fun11334__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control4_fun11334, kk_context());
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_control4_fun11335__t {
  struct kk_function_s _base;
  kk_box_t _b_11019;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_control4_fun11335(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control4_fun11335(kk_box_t _b_11019, kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control4_fun11335__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control4_fun11335__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control4_fun11335, kk_context());
  _self->_b_11019 = _b_11019;
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_control4_fun11336__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_control4_fun11336(kk_function_t _fself, kk_box_t _b_11014, kk_function_t _b_11015, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control4_fun11336(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control4_fun11336__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control4_fun11336__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control4_fun11336, kk_context());
  _self->op = op;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_control4_fun11336(kk_function_t _fself, kk_box_t _b_11014, kk_function_t _b_11015, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control4_fun11336__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control4_fun11336__t*, _fself);
  kk_function_t op = _self->op; /* (x1 : 7983, x2 : 7984, x3 : 7985, x4 : 7986, k : (7987) -> 7988 7990) -> 7988 7990 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_types__tuple4_ x0_11027 = kk_std_core_types__tuple4__unbox(_b_11014, _ctx); /*(7983, 7984, 7985, 7986)*/;
  kk_box_t _x11337 = kk_std_core_types_fst_2(x0_11027, _ctx); /*1000*/
  kk_box_t _x11338 = kk_std_core_types_snd_2(x0_11027, _ctx); /*1001*/
  kk_box_t _x11339 = kk_std_core_types_thd_1(x0_11027, _ctx); /*1002*/
  kk_box_t _x11340;
  kk_box_t _brw_11140 = kk_std_core_types_field4(x0_11027, _ctx); /*1003*/;
  kk_std_core_types__tuple4__dropn(x0_11027, (KK_I32(4)), _ctx);
  _x11340 = _brw_11140; /*1003*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_box_t, kk_function_t, kk_context_t*), op, (op, _x11337, _x11338, _x11339, _x11340, _b_11015, _ctx));
}
static kk_box_t kk_std_core_hnd_clause_control4_fun11335(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control4_fun11335__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control4_fun11335__t*, _fself);
  kk_box_t _b_11019 = _self->_b_11019; /* 51 */
  kk_function_t op = _self->op; /* (x1 : 7983, x2 : 7984, x3 : 7985, x4 : 7986, k : (7987) -> 7988 7990) -> 7988 7990 */
  kk_drop_match(_self, {kk_box_dup(_b_11019);kk_function_dup(op);}, {}, _ctx)
  return kk_std_core_hnd_protect(_b_11019, kk_std_core_hnd_new_clause_control4_fun11336(op, _ctx), k0, _ctx);
}
static kk_box_t kk_std_core_hnd_clause_control4_fun11334(kk_function_t _fself, kk_std_core_hnd__marker _b_11017, kk_std_core_hnd__ev _b_11018, kk_box_t _b_11019, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control4_fun11334__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control4_fun11334__t*, _fself);
  kk_function_t op = _self->op; /* (x1 : 7983, x2 : 7984, x3 : 7985, x4 : 7986, k : (7987) -> 7988 7990) -> 7988 7990 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(_b_11018, (KK_I32(3)), _ctx);
  return kk_std_core_hnd_yield_to(_b_11017, kk_std_core_hnd_new_clause_control4_fun11335(_b_11019, op, _ctx), _ctx);
}

kk_std_core_hnd__clause1 kk_std_core_hnd_clause_control4(kk_function_t op, kk_context_t* _ctx) { /* forall<a,b,c,d,a1,e,b1,c1> (op : (x1 : a, x2 : b, x3 : c, x4 : d, k : (a1) -> e c1) -> e c1) -> clause1<(a, b, c, d),a1,b1,e,c1> */ 
  return kk_std_core_hnd__new_Clause1(kk_std_core_hnd_new_clause_control4_fun11334(op, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_never0_fun11341__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_never0_fun11341(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__571__16, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_never0_fun11341(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never0_fun11341__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_never0_fun11341__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_never0_fun11341, kk_context());
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_never0_fun11342__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_never0_fun11342(kk_function_t _fself, kk_function_t ___wildcard__571__43, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_never0_fun11342(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never0_fun11342__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_never0_fun11342__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_never0_fun11342, kk_context());
  _self->op = op;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_never0_fun11342(kk_function_t _fself, kk_function_t ___wildcard__571__43, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never0_fun11342__t* _self = kk_function_as(struct kk_std_core_hnd_clause_never0_fun11342__t*, _fself);
  kk_function_t op = _self->op; /* () -> 8021 8023 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_function_drop(___wildcard__571__43, _ctx);
  return kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), op, (op, _ctx));
}
static kk_box_t kk_std_core_hnd_clause_never0_fun11341(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__571__16, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never0_fun11341__t* _self = kk_function_as(struct kk_std_core_hnd_clause_never0_fun11341__t*, _fself);
  kk_function_t op = _self->op; /* () -> 8021 8023 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(___wildcard__571__16, (KK_I32(3)), _ctx);
  return kk_std_core_hnd_yield_to_final(m0, kk_std_core_hnd_new_clause_never0_fun11342(op, _ctx), _ctx);
}

kk_std_core_hnd__clause0 kk_std_core_hnd_clause_never0(kk_function_t op, kk_context_t* _ctx) { /* forall<a,e,b,c> (op : () -> e c) -> clause0<a,b,e,c> */ 
  return kk_std_core_hnd__new_Clause0(kk_std_core_hnd_new_clause_never0_fun11341(op, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_never1_fun11343__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_never1_fun11343(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__525__16, kk_box_t x, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_never1_fun11343(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never1_fun11343__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_never1_fun11343__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_never1_fun11343, kk_context());
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_never1_fun11344__t {
  struct kk_function_s _base;
  kk_function_t op;
  kk_box_t x;
};
static kk_box_t kk_std_core_hnd_clause_never1_fun11344(kk_function_t _fself, kk_function_t ___wildcard__525__45, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_never1_fun11344(kk_function_t op, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never1_fun11344__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_never1_fun11344__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_never1_fun11344, kk_context());
  _self->op = op;
  _self->x = x;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_never1_fun11344(kk_function_t _fself, kk_function_t ___wildcard__525__45, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never1_fun11344__t* _self = kk_function_as(struct kk_std_core_hnd_clause_never1_fun11344__t*, _fself);
  kk_function_t op = _self->op; /* (8058) -> 8060 8062 */
  kk_box_t x = _self->x; /* 8058 */
  kk_drop_match(_self, {kk_function_dup(op);kk_box_dup(x);}, {}, _ctx)
  kk_function_drop(___wildcard__525__45, _ctx);
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), op, (op, x, _ctx));
}
static kk_box_t kk_std_core_hnd_clause_never1_fun11343(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__525__16, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never1_fun11343__t* _self = kk_function_as(struct kk_std_core_hnd_clause_never1_fun11343__t*, _fself);
  kk_function_t op = _self->op; /* (8058) -> 8060 8062 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(___wildcard__525__16, (KK_I32(3)), _ctx);
  return kk_std_core_hnd_yield_to_final(m0, kk_std_core_hnd_new_clause_never1_fun11344(op, x, _ctx), _ctx);
}

kk_std_core_hnd__clause1 kk_std_core_hnd_clause_never1(kk_function_t op, kk_context_t* _ctx) { /* forall<a,b,e,c,d> (op : (a) -> e d) -> clause1<a,b,c,e,d> */ 
  return kk_std_core_hnd__new_Clause1(kk_std_core_hnd_new_clause_never1_fun11343(op, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_never2_fun11345__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_never2_fun11345(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__615__16, kk_box_t x1, kk_box_t x2, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_never2_fun11345(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never2_fun11345__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_never2_fun11345__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_never2_fun11345, kk_context());
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_never2_fun11346__t {
  struct kk_function_s _base;
  kk_function_t op;
  kk_box_t x1;
  kk_box_t x2;
};
static kk_box_t kk_std_core_hnd_clause_never2_fun11346(kk_function_t _fself, kk_function_t ___wildcard__615__49, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_never2_fun11346(kk_function_t op, kk_box_t x1, kk_box_t x2, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never2_fun11346__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_never2_fun11346__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_never2_fun11346, kk_context());
  _self->op = op;
  _self->x1 = x1;
  _self->x2 = x2;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_never2_fun11346(kk_function_t _fself, kk_function_t ___wildcard__615__49, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never2_fun11346__t* _self = kk_function_as(struct kk_std_core_hnd_clause_never2_fun11346__t*, _fself);
  kk_function_t op = _self->op; /* (8102, 8103) -> 8105 8107 */
  kk_box_t x1 = _self->x1; /* 8102 */
  kk_box_t x2 = _self->x2; /* 8103 */
  kk_drop_match(_self, {kk_function_dup(op);kk_box_dup(x1);kk_box_dup(x2);}, {}, _ctx)
  kk_function_drop(___wildcard__615__49, _ctx);
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_context_t*), op, (op, x1, x2, _ctx));
}
static kk_box_t kk_std_core_hnd_clause_never2_fun11345(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__615__16, kk_box_t x1, kk_box_t x2, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never2_fun11345__t* _self = kk_function_as(struct kk_std_core_hnd_clause_never2_fun11345__t*, _fself);
  kk_function_t op = _self->op; /* (8102, 8103) -> 8105 8107 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(___wildcard__615__16, (KK_I32(3)), _ctx);
  return kk_std_core_hnd_yield_to_final(m0, kk_std_core_hnd_new_clause_never2_fun11346(op, x1, x2, _ctx), _ctx);
}

kk_std_core_hnd__clause2 kk_std_core_hnd_clause_never2(kk_function_t op, kk_context_t* _ctx) { /* forall<a,b,c,e,d,a1> (op : (a, b) -> e a1) -> clause2<a,b,c,d,e,a1> */ 
  return kk_std_core_hnd__new_Clause2(kk_std_core_hnd_new_clause_never2_fun11345(op, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_never3_fun11347__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_never3_fun11347(kk_function_t _fself, kk_std_core_hnd__marker _b_11030, kk_std_core_hnd__ev _b_11031, kk_box_t _b_11032, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_never3_fun11347(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never3_fun11347__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_never3_fun11347__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_never3_fun11347, kk_context());
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_never3_fun11348__t {
  struct kk_function_s _base;
  kk_box_t _b_11032;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_never3_fun11348(kk_function_t _fself, kk_function_t ___wildcard__525__45, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_never3_fun11348(kk_box_t _b_11032, kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never3_fun11348__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_never3_fun11348__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_never3_fun11348, kk_context());
  _self->_b_11032 = _b_11032;
  _self->op = op;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_never3_fun11348(kk_function_t _fself, kk_function_t ___wildcard__525__45, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never3_fun11348__t* _self = kk_function_as(struct kk_std_core_hnd_clause_never3_fun11348__t*, _fself);
  kk_box_t _b_11032 = _self->_b_11032; /* 51 */
  kk_function_t op = _self->op; /* (8337, 8338, 8339) -> 8341 8343 */
  kk_drop_match(_self, {kk_box_dup(_b_11032);kk_function_dup(op);}, {}, _ctx)
  kk_function_drop(___wildcard__525__45, _ctx);
  kk_box_t _x11349;
  kk_std_core_types__tuple3_ _brw_11138;
  kk_box_t _x11350 = kk_box_dup(_b_11032); /*51*/
  _brw_11138 = kk_std_core_types__tuple3__unbox(_x11350, _ctx); /*(8337, 8338, 8339)*/
  kk_box_t _brw_11139 = kk_std_core_types_fst_1(_brw_11138, _ctx); /*1000*/;
  kk_std_core_types__tuple3__drop(_brw_11138, _ctx);
  _x11349 = _brw_11139; /*1000*/
  kk_box_t _x11351;
  kk_std_core_types__tuple3_ _brw_11136;
  kk_box_t _x11352 = kk_box_dup(_b_11032); /*51*/
  _brw_11136 = kk_std_core_types__tuple3__unbox(_x11352, _ctx); /*(8337, 8338, 8339)*/
  kk_box_t _brw_11137 = kk_std_core_types_snd_1(_brw_11136, _ctx); /*1001*/;
  kk_std_core_types__tuple3__drop(_brw_11136, _ctx);
  _x11351 = _brw_11137; /*1001*/
  kk_box_t _x11353;
  kk_std_core_types__tuple3_ _brw_11134 = kk_std_core_types__tuple3__unbox(_b_11032, _ctx); /*(8337, 8338, 8339)*/;
  kk_box_t _brw_11135 = kk_std_core_types_thd(_brw_11134, _ctx); /*1002*/;
  kk_std_core_types__tuple3__drop(_brw_11134, _ctx);
  _x11353 = _brw_11135; /*1002*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_context_t*), op, (op, _x11349, _x11351, _x11353, _ctx));
}
static kk_box_t kk_std_core_hnd_clause_never3_fun11347(kk_function_t _fself, kk_std_core_hnd__marker _b_11030, kk_std_core_hnd__ev _b_11031, kk_box_t _b_11032, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never3_fun11347__t* _self = kk_function_as(struct kk_std_core_hnd_clause_never3_fun11347__t*, _fself);
  kk_function_t op = _self->op; /* (8337, 8338, 8339) -> 8341 8343 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(_b_11031, (KK_I32(3)), _ctx);
  return kk_std_core_hnd_yield_to_final(_b_11030, kk_std_core_hnd_new_clause_never3_fun11348(_b_11032, op, _ctx), _ctx);
}

kk_std_core_hnd__clause1 kk_std_core_hnd_clause_never3(kk_function_t op, kk_context_t* _ctx) { /* forall<a,b,c,d,e,a1,b1> (op : (a, b, c) -> e b1) -> clause1<(a, b, c),d,a1,e,b1> */ 
  return kk_std_core_hnd__new_Clause1(kk_std_core_hnd_new_clause_never3_fun11347(op, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_never4_fun11354__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_never4_fun11354(kk_function_t _fself, kk_std_core_hnd__marker _b_11038, kk_std_core_hnd__ev _b_11039, kk_box_t _b_11040, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_never4_fun11354(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never4_fun11354__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_never4_fun11354__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_never4_fun11354, kk_context());
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_never4_fun11355__t {
  struct kk_function_s _base;
  kk_function_t op;
  kk_std_core_types__tuple4_ x_11044;
};
static kk_box_t kk_std_core_hnd_clause_never4_fun11355(kk_function_t _fself, kk_function_t ___wildcard__525__45, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_never4_fun11355(kk_function_t op, kk_std_core_types__tuple4_ x_11044, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never4_fun11355__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_never4_fun11355__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_never4_fun11355, kk_context());
  _self->op = op;
  _self->x_11044 = x_11044;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_never4_fun11355(kk_function_t _fself, kk_function_t ___wildcard__525__45, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never4_fun11355__t* _self = kk_function_as(struct kk_std_core_hnd_clause_never4_fun11355__t*, _fself);
  kk_function_t op = _self->op; /* (8643, 8644, 8645, 8646) -> 8648 8650 */
  kk_std_core_types__tuple4_ x_11044 = _self->x_11044; /* (8643, 8644, 8645, 8646) */
  kk_drop_match(_self, {kk_function_dup(op);kk_std_core_types__tuple4__dup(x_11044);}, {}, _ctx)
  kk_function_drop(___wildcard__525__45, _ctx);
  kk_box_t _x11356 = kk_std_core_types_fst_2(x_11044, _ctx); /*1000*/
  kk_box_t _x11357 = kk_std_core_types_snd_2(x_11044, _ctx); /*1001*/
  kk_box_t _x11358 = kk_std_core_types_thd_1(x_11044, _ctx); /*1002*/
  kk_box_t _x11359;
  kk_box_t _brw_11133 = kk_std_core_types_field4(x_11044, _ctx); /*1003*/;
  kk_std_core_types__tuple4__dropn(x_11044, (KK_I32(4)), _ctx);
  _x11359 = _brw_11133; /*1003*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_box_t, kk_context_t*), op, (op, _x11356, _x11357, _x11358, _x11359, _ctx));
}
static kk_box_t kk_std_core_hnd_clause_never4_fun11354(kk_function_t _fself, kk_std_core_hnd__marker _b_11038, kk_std_core_hnd__ev _b_11039, kk_box_t _b_11040, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never4_fun11354__t* _self = kk_function_as(struct kk_std_core_hnd_clause_never4_fun11354__t*, _fself);
  kk_function_t op = _self->op; /* (8643, 8644, 8645, 8646) -> 8648 8650 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(_b_11039, (KK_I32(3)), _ctx);
  kk_std_core_types__tuple4_ x_11044 = kk_std_core_types__tuple4__unbox(_b_11040, _ctx); /*(8643, 8644, 8645, 8646)*/;
  return kk_std_core_hnd_yield_to_final(_b_11038, kk_std_core_hnd_new_clause_never4_fun11355(op, x_11044, _ctx), _ctx);
}

kk_std_core_hnd__clause1 kk_std_core_hnd_clause_never4(kk_function_t op, kk_context_t* _ctx) { /* forall<a,b,c,d,a1,e,b1,c1> (op : (a, b, c, d) -> e c1) -> clause1<(a, b, c, d),a1,b1,e,c1> */ 
  return kk_std_core_hnd__new_Clause1(kk_std_core_hnd_new_clause_never4_fun11354(op, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_tail_noyield3_fun11360__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_tail_noyield3_fun11360(kk_function_t _fself, kk_std_core_hnd__marker _b_11046, kk_std_core_hnd__ev _b_11047, kk_box_t _b_11048, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_tail_noyield3_fun11360(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail_noyield3_fun11360__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_tail_noyield3_fun11360__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_tail_noyield3_fun11360, kk_context());
  _self->op = op;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_tail_noyield3_fun11360(kk_function_t _fself, kk_std_core_hnd__marker _b_11046, kk_std_core_hnd__ev _b_11047, kk_box_t _b_11048, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail_noyield3_fun11360__t* _self = kk_function_as(struct kk_std_core_hnd_clause_tail_noyield3_fun11360__t*, _fself);
  kk_function_t op = _self->op; /* (8880, 8881, 8882) -> 8884 8883 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(_b_11047, (KK_I32(3)), _ctx);
  kk_box_t _x11361;
  kk_std_core_types__tuple3_ _brw_11131;
  kk_box_t _x11362 = kk_box_dup(_b_11048); /*51*/
  _brw_11131 = kk_std_core_types__tuple3__unbox(_x11362, _ctx); /*(8880, 8881, 8882)*/
  kk_box_t _brw_11132 = kk_std_core_types_fst_1(_brw_11131, _ctx); /*1000*/;
  kk_std_core_types__tuple3__drop(_brw_11131, _ctx);
  _x11361 = _brw_11132; /*1000*/
  kk_box_t _x11363;
  kk_std_core_types__tuple3_ _brw_11129;
  kk_box_t _x11364 = kk_box_dup(_b_11048); /*51*/
  _brw_11129 = kk_std_core_types__tuple3__unbox(_x11364, _ctx); /*(8880, 8881, 8882)*/
  kk_box_t _brw_11130 = kk_std_core_types_snd_1(_brw_11129, _ctx); /*1001*/;
  kk_std_core_types__tuple3__drop(_brw_11129, _ctx);
  _x11363 = _brw_11130; /*1001*/
  kk_box_t _x11365;
  kk_std_core_types__tuple3_ _brw_11127 = kk_std_core_types__tuple3__unbox(_b_11048, _ctx); /*(8880, 8881, 8882)*/;
  kk_box_t _brw_11128 = kk_std_core_types_thd(_brw_11127, _ctx); /*1002*/;
  kk_std_core_types__tuple3__drop(_brw_11127, _ctx);
  _x11365 = _brw_11128; /*1002*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_context_t*), op, (op, _x11361, _x11363, _x11365, _ctx));
}

kk_std_core_hnd__clause1 kk_std_core_hnd_clause_tail_noyield3(kk_function_t op, kk_context_t* _ctx) { /* forall<a,b,c,d,e,a1,b1> (op : (a, b, c) -> e d) -> clause1<(a, b, c),d,a1,e,b1> */ 
  return kk_std_core_hnd__new_Clause1(kk_std_core_hnd_new_clause_tail_noyield3_fun11360(op, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_tail_noyield4_fun11366__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_tail_noyield4_fun11366(kk_function_t _fself, kk_std_core_hnd__marker _b_11054, kk_std_core_hnd__ev _b_11055, kk_box_t _b_11056, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_tail_noyield4_fun11366(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail_noyield4_fun11366__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_tail_noyield4_fun11366__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_tail_noyield4_fun11366, kk_context());
  _self->op = op;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_tail_noyield4_fun11366(kk_function_t _fself, kk_std_core_hnd__marker _b_11054, kk_std_core_hnd__ev _b_11055, kk_box_t _b_11056, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail_noyield4_fun11366__t* _self = kk_function_as(struct kk_std_core_hnd_clause_tail_noyield4_fun11366__t*, _fself);
  kk_function_t op = _self->op; /* (9186, 9187, 9188, 9189) -> 9191 9190 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(_b_11055, (KK_I32(3)), _ctx);
  kk_std_core_types__tuple4_ x_11060 = kk_std_core_types__tuple4__unbox(_b_11056, _ctx); /*(9186, 9187, 9188, 9189)*/;
  kk_box_t _x11367 = kk_std_core_types_fst_2(x_11060, _ctx); /*1000*/
  kk_box_t _x11368 = kk_std_core_types_snd_2(x_11060, _ctx); /*1001*/
  kk_box_t _x11369 = kk_std_core_types_thd_1(x_11060, _ctx); /*1002*/
  kk_box_t _x11370;
  kk_box_t _brw_11126 = kk_std_core_types_field4(x_11060, _ctx); /*1003*/;
  kk_std_core_types__tuple4__dropn(x_11060, (KK_I32(4)), _ctx);
  _x11370 = _brw_11126; /*1003*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_box_t, kk_context_t*), op, (op, _x11367, _x11368, _x11369, _x11370, _ctx));
}

kk_std_core_hnd__clause1 kk_std_core_hnd_clause_tail_noyield4(kk_function_t op, kk_context_t* _ctx) { /* forall<a,b,c,d,a1,e,b1,c1> (op : (a, b, c, d) -> e a1) -> clause1<(a, b, c, d),a1,b1,e,c1> */ 
  return kk_std_core_hnd__new_Clause1(kk_std_core_hnd_new_clause_tail_noyield4_fun11366(op, _ctx), _ctx);
}
 
// extra under1x to make under1 inlineable


// lift anonymous function
struct kk_std_core_hnd_under1x_fun11372__t {
  struct kk_function_s _base;
  kk_std_core_hnd__ev ev;
};
static kk_box_t kk_std_core_hnd_under1x_fun11372(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_under1x_fun11372(kk_std_core_hnd__ev ev, kk_context_t* _ctx) {
  struct kk_std_core_hnd_under1x_fun11372__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_under1x_fun11372__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_under1x_fun11372, kk_context());
  _self->ev = ev;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_under1x_fun11372(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd_under1x_fun11372__t* _self = kk_function_as(struct kk_std_core_hnd_under1x_fun11372__t*, _fself);
  kk_std_core_hnd__ev ev = _self->ev; /* std/core/hnd/ev<9254> */
  kk_drop_match(_self, {kk_std_core_hnd__ev_dup(ev);}, {}, _ctx)
  return kk_std_core_hnd_under1x(ev, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd_under1x(kk_std_core_hnd__ev ev, kk_function_t op, kk_box_t x, kk_context_t* _ctx) { /* forall<a,b,e,c> (ev : ev<c>, op : (a) -> e b, x : a) -> e b */ 
  kk_evv_t w0;
  kk_std_core_hnd__ev _x11371 = kk_std_core_hnd__ev_dup(ev); /*std/core/hnd/ev<9254>*/
  w0 = kk_std_core_hnd_evv_swap_with(_x11371, _ctx); /*std/core/hnd/evv<_9199>*/
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), op, (op, x, _ctx)); /*9252*/;
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    kk_evv_drop(w0, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_under1x_fun11372(ev, _ctx), _ctx);
  }
  {
    kk_std_core_hnd__ev_dropn(ev, (KK_I32(3)), _ctx);
    kk_unit_t __0 = kk_Unit;
    kk_evv_set(w0,kk_context());
    return y;
  }
}
extern kk_box_t kk_std_core_hnd_under1_fun11374(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd_under1_fun11374__t* _self = kk_function_as(struct kk_std_core_hnd_under1_fun11374__t*, _fself);
  kk_std_core_hnd__ev ev = _self->ev; /* std/core/hnd/ev<9315> */
  kk_drop_match(_self, {kk_std_core_hnd__ev_dup(ev);}, {}, _ctx)
  return kk_std_core_hnd_under1x(ev, cont, res, _ctx);
}
extern kk_box_t kk_std_core_hnd_under0_fun11378(kk_function_t _fself, kk_function_t cont0, kk_box_t res0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_under0_fun11378__t* _self = kk_function_as(struct kk_std_core_hnd_under0_fun11378__t*, _fself);
  kk_std_core_hnd__ev ev = _self->ev; /* std/core/hnd/ev<9368> */
  kk_drop_match(_self, {kk_std_core_hnd__ev_dup(ev);}, {}, _ctx)
  return kk_std_core_hnd_under1x(ev, cont0, res0, _ctx);
}
extern kk_box_t kk_std_core_hnd_under0_fun11376(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd_under0_fun11376__t* _self = kk_function_as(struct kk_std_core_hnd_under0_fun11376__t*, _fself);
  kk_std_core_hnd__ev ev = _self->ev; /* std/core/hnd/ev<9368> */
  kk_drop_match(_self, {kk_std_core_hnd__ev_dup(ev);}, {}, _ctx)
  kk_evv_t w00;
  kk_std_core_hnd__ev _x11377 = kk_std_core_hnd__ev_dup(ev); /*std/core/hnd/ev<9368>*/
  w00 = kk_std_core_hnd_evv_swap_with(_x11377, _ctx); /*std/core/hnd/evv<_9260>*/
  kk_box_t y0 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), cont, (cont, res, _ctx)); /*9366*/;
  if (kk_yielding(kk_context())) {
    kk_box_drop(y0, _ctx);
    kk_evv_drop(w00, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_under0_fun11378(ev, _ctx), _ctx);
  }
  {
    kk_std_core_hnd__ev_dropn(ev, (KK_I32(3)), _ctx);
    kk_unit_t __0 = kk_Unit;
    kk_evv_set(w00,kk_context());
    return y0;
  }
}


// lift anonymous function
struct kk_std_core_hnd_clause_tail0_fun11379__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_tail0_fun11379(kk_function_t _fself, kk_std_core_hnd__marker ___wildcard__562__14, kk_std_core_hnd__ev ev, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_tail0_fun11379(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail0_fun11379__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_tail0_fun11379__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_tail0_fun11379, kk_context());
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_tail0_fun11381__t {
  struct kk_function_s _base;
  kk_std_core_hnd__ev ev;
};
static kk_box_t kk_std_core_hnd_clause_tail0_fun11381(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_tail0_fun11381(kk_std_core_hnd__ev ev, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail0_fun11381__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_tail0_fun11381__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_tail0_fun11381, kk_context());
  _self->ev = ev;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_tail0_fun11383__t {
  struct kk_function_s _base;
  kk_std_core_hnd__ev ev;
};
static kk_box_t kk_std_core_hnd_clause_tail0_fun11383(kk_function_t _fself, kk_function_t cont0, kk_box_t res0, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_tail0_fun11383(kk_std_core_hnd__ev ev, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail0_fun11383__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_tail0_fun11383__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_tail0_fun11383, kk_context());
  _self->ev = ev;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_tail0_fun11383(kk_function_t _fself, kk_function_t cont0, kk_box_t res0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail0_fun11383__t* _self = kk_function_as(struct kk_std_core_hnd_clause_tail0_fun11383__t*, _fself);
  kk_std_core_hnd__ev ev = _self->ev; /* std/core/hnd/ev<9399> */
  kk_drop_match(_self, {kk_std_core_hnd__ev_dup(ev);}, {}, _ctx)
  return kk_std_core_hnd_under1x(ev, cont0, res0, _ctx);
}
static kk_box_t kk_std_core_hnd_clause_tail0_fun11381(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail0_fun11381__t* _self = kk_function_as(struct kk_std_core_hnd_clause_tail0_fun11381__t*, _fself);
  kk_std_core_hnd__ev ev = _self->ev; /* std/core/hnd/ev<9399> */
  kk_drop_match(_self, {kk_std_core_hnd__ev_dup(ev);}, {}, _ctx)
  kk_evv_t w00;
  kk_std_core_hnd__ev _x11382 = kk_std_core_hnd__ev_dup(ev); /*std/core/hnd/ev<9399>*/
  w00 = kk_std_core_hnd_evv_swap_with(_x11382, _ctx); /*std/core/hnd/evv<_9260>*/
  kk_box_t y0 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), cont, (cont, res, _ctx)); /*9398*/;
  if (kk_yielding(kk_context())) {
    kk_box_drop(y0, _ctx);
    kk_evv_drop(w00, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_clause_tail0_fun11383(ev, _ctx), _ctx);
  }
  {
    kk_std_core_hnd__ev_dropn(ev, (KK_I32(3)), _ctx);
    kk_unit_t __0 = kk_Unit;
    kk_evv_set(w00,kk_context());
    return y0;
  }
}
static kk_box_t kk_std_core_hnd_clause_tail0_fun11379(kk_function_t _fself, kk_std_core_hnd__marker ___wildcard__562__14, kk_std_core_hnd__ev ev, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail0_fun11379__t* _self = kk_function_as(struct kk_std_core_hnd_clause_tail0_fun11379__t*, _fself);
  kk_function_t op = _self->op; /* () -> 9396 9398 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_evv_t w0;
  kk_std_core_hnd__ev _x11380 = kk_std_core_hnd__ev_dup(ev); /*std/core/hnd/ev<9399>*/
  w0 = kk_std_core_hnd_evv_swap_with(_x11380, _ctx); /*std/core/hnd/evv<_9320>*/
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), op, (op, _ctx)); /*9398*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w0,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_clause_tail0_fun11381(ev, _ctx), _ctx);
  }
  {
    kk_std_core_hnd__ev_dropn(ev, (KK_I32(3)), _ctx);
    return y;
  }
}

kk_std_core_hnd__clause0 kk_std_core_hnd_clause_tail0(kk_function_t op, kk_context_t* _ctx) { /* forall<e,a,b,c> (op : () -> e b) -> clause0<b,c,e,a> */ 
  return kk_std_core_hnd__new_Clause0(kk_std_core_hnd_new_clause_tail0_fun11379(op, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_tail1_fun11384__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_tail1_fun11384(kk_function_t _fself, kk_std_core_hnd__marker ___wildcard__519__14, kk_std_core_hnd__ev ev, kk_box_t x, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_tail1_fun11384(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail1_fun11384__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_tail1_fun11384__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_tail1_fun11384, kk_context());
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_tail1_fun11386__t {
  struct kk_function_s _base;
  kk_std_core_hnd__ev ev;
};
static kk_box_t kk_std_core_hnd_clause_tail1_fun11386(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_tail1_fun11386(kk_std_core_hnd__ev ev, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail1_fun11386__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_tail1_fun11386__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_tail1_fun11386, kk_context());
  _self->ev = ev;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_tail1_fun11386(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail1_fun11386__t* _self = kk_function_as(struct kk_std_core_hnd_clause_tail1_fun11386__t*, _fself);
  kk_std_core_hnd__ev ev = _self->ev; /* std/core/hnd/ev<9437> */
  kk_drop_match(_self, {kk_std_core_hnd__ev_dup(ev);}, {}, _ctx)
  return kk_std_core_hnd_under1x(ev, cont, res, _ctx);
}
static kk_box_t kk_std_core_hnd_clause_tail1_fun11384(kk_function_t _fself, kk_std_core_hnd__marker ___wildcard__519__14, kk_std_core_hnd__ev ev, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail1_fun11384__t* _self = kk_function_as(struct kk_std_core_hnd_clause_tail1_fun11384__t*, _fself);
  kk_function_t op = _self->op; /* (9435) -> 9433 9436 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_evv_t w0;
  kk_std_core_hnd__ev _x11385 = kk_std_core_hnd__ev_dup(ev); /*std/core/hnd/ev<9437>*/
  w0 = kk_std_core_hnd_evv_swap_with(_x11385, _ctx); /*std/core/hnd/evv<_9260>*/
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), op, (op, x, _ctx)); /*9436*/;
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    kk_evv_drop(w0, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_clause_tail1_fun11386(ev, _ctx), _ctx);
  }
  {
    kk_std_core_hnd__ev_dropn(ev, (KK_I32(3)), _ctx);
    kk_unit_t __0 = kk_Unit;
    kk_evv_set(w0,kk_context());
    return y;
  }
}

kk_std_core_hnd__clause1 kk_std_core_hnd_clause_tail1(kk_function_t op, kk_context_t* _ctx) { /* forall<e,a,b,c,d> (op : (b) -> e c) -> clause1<b,c,d,e,a> */ 
  return kk_std_core_hnd__new_Clause1(kk_std_core_hnd_new_clause_tail1_fun11384(op, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_under2_fun11388__t {
  struct kk_function_s _base;
  kk_std_core_hnd__ev ev;
};
static kk_box_t kk_std_core_hnd_under2_fun11388(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_under2_fun11388(kk_std_core_hnd__ev ev, kk_context_t* _ctx) {
  struct kk_std_core_hnd_under2_fun11388__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_under2_fun11388__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_under2_fun11388, kk_context());
  _self->ev = ev;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_under2_fun11390__t {
  struct kk_function_s _base;
  kk_std_core_hnd__ev ev;
};
static kk_box_t kk_std_core_hnd_under2_fun11390(kk_function_t _fself, kk_function_t cont0, kk_box_t res0, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_under2_fun11390(kk_std_core_hnd__ev ev, kk_context_t* _ctx) {
  struct kk_std_core_hnd_under2_fun11390__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_under2_fun11390__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_under2_fun11390, kk_context());
  _self->ev = ev;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_under2_fun11390(kk_function_t _fself, kk_function_t cont0, kk_box_t res0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_under2_fun11390__t* _self = kk_function_as(struct kk_std_core_hnd_under2_fun11390__t*, _fself);
  kk_std_core_hnd__ev ev = _self->ev; /* std/core/hnd/ev<9500> */
  kk_drop_match(_self, {kk_std_core_hnd__ev_dup(ev);}, {}, _ctx)
  return kk_std_core_hnd_under1x(ev, cont0, res0, _ctx);
}
static kk_box_t kk_std_core_hnd_under2_fun11388(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd_under2_fun11388__t* _self = kk_function_as(struct kk_std_core_hnd_under2_fun11388__t*, _fself);
  kk_std_core_hnd__ev ev = _self->ev; /* std/core/hnd/ev<9500> */
  kk_drop_match(_self, {kk_std_core_hnd__ev_dup(ev);}, {}, _ctx)
  kk_evv_t w00;
  kk_std_core_hnd__ev _x11389 = kk_std_core_hnd__ev_dup(ev); /*std/core/hnd/ev<9500>*/
  w00 = kk_std_core_hnd_evv_swap_with(_x11389, _ctx); /*std/core/hnd/evv<_9260>*/
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), cont, (cont, res, _ctx)); /*9498*/;
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    kk_evv_drop(w00, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_under2_fun11390(ev, _ctx), _ctx);
  }
  {
    kk_std_core_hnd__ev_dropn(ev, (KK_I32(3)), _ctx);
    kk_unit_t __0 = kk_Unit;
    kk_evv_set(w00,kk_context());
    return y;
  }
}

kk_box_t kk_std_core_hnd_under2(kk_std_core_hnd__ev ev, kk_function_t op, kk_box_t x1, kk_box_t x2, kk_context_t* _ctx) { /* forall<a,b,c,e,d> (ev : ev<d>, op : (a, b) -> e c, x1 : a, x2 : b) -> e c */ 
  kk_evv_t w0;
  kk_std_core_hnd__ev _x11387 = kk_std_core_hnd__ev_dup(ev); /*std/core/hnd/ev<9500>*/
  w0 = kk_std_core_hnd_evv_swap_with(_x11387, _ctx); /*std/core/hnd/evv<_9444>*/
  kk_box_t z = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_context_t*), op, (op, x1, x2, _ctx)); /*9498*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w0,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(z, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_under2_fun11388(ev, _ctx), _ctx);
  }
  {
    kk_std_core_hnd__ev_dropn(ev, (KK_I32(3)), _ctx);
    return z;
  }
}
extern kk_box_t kk_std_core_hnd_clause_tail2_fun11391(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ev, kk_box_t x1, kk_box_t x2, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail2_fun11391__t* _self = kk_function_as(struct kk_std_core_hnd_clause_tail2_fun11391__t*, _fself);
  kk_function_t op = _self->op; /* (9542, 9543) -> 9540 9544 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  return kk_std_core_hnd_under2(ev, op, x1, x2, _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_tail3_fun11392__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_tail3_fun11392(kk_function_t _fself, kk_std_core_hnd__marker _b_11062, kk_std_core_hnd__ev _b_11063, kk_box_t _b_11064, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_tail3_fun11392(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail3_fun11392__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_tail3_fun11392__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_tail3_fun11392, kk_context());
  _self->op = op;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_tail3_fun11392(kk_function_t _fself, kk_std_core_hnd__marker _b_11062, kk_std_core_hnd__ev _b_11063, kk_box_t _b_11064, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail3_fun11392__t* _self = kk_function_as(struct kk_std_core_hnd_clause_tail3_fun11392__t*, _fself);
  kk_function_t op = _self->op; /* (9775, 9776, 9777) -> 9779 9778 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(_b_11063, (KK_I32(3)), _ctx);
  kk_box_t _x11393;
  kk_std_core_types__tuple3_ _brw_11115;
  kk_box_t _x11394 = kk_box_dup(_b_11064); /*51*/
  _brw_11115 = kk_std_core_types__tuple3__unbox(_x11394, _ctx); /*(9775, 9776, 9777)*/
  kk_box_t _brw_11116 = kk_std_core_types_fst_1(_brw_11115, _ctx); /*1000*/;
  kk_std_core_types__tuple3__drop(_brw_11115, _ctx);
  _x11393 = _brw_11116; /*1000*/
  kk_box_t _x11395;
  kk_std_core_types__tuple3_ _brw_11113;
  kk_box_t _x11396 = kk_box_dup(_b_11064); /*51*/
  _brw_11113 = kk_std_core_types__tuple3__unbox(_x11396, _ctx); /*(9775, 9776, 9777)*/
  kk_box_t _brw_11114 = kk_std_core_types_snd_1(_brw_11113, _ctx); /*1001*/;
  kk_std_core_types__tuple3__drop(_brw_11113, _ctx);
  _x11395 = _brw_11114; /*1001*/
  kk_box_t _x11397;
  kk_std_core_types__tuple3_ _brw_11111 = kk_std_core_types__tuple3__unbox(_b_11064, _ctx); /*(9775, 9776, 9777)*/;
  kk_box_t _brw_11112 = kk_std_core_types_thd(_brw_11111, _ctx); /*1002*/;
  kk_std_core_types__tuple3__drop(_brw_11111, _ctx);
  _x11397 = _brw_11112; /*1002*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_context_t*), op, (op, _x11393, _x11395, _x11397, _ctx));
}

kk_std_core_hnd__clause1 kk_std_core_hnd_clause_tail3(kk_function_t op, kk_context_t* _ctx) { /* forall<a,b,c,d,e,a1,b1> (op : (a, b, c) -> e d) -> clause1<(a, b, c),d,a1,e,b1> */ 
  return kk_std_core_hnd__new_Clause1(kk_std_core_hnd_new_clause_tail3_fun11392(op, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_tail4_fun11398__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_tail4_fun11398(kk_function_t _fself, kk_std_core_hnd__marker _b_11070, kk_std_core_hnd__ev _b_11071, kk_box_t _b_11072, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_tail4_fun11398(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail4_fun11398__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_tail4_fun11398__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_tail4_fun11398, kk_context());
  _self->op = op;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_tail4_fun11398(kk_function_t _fself, kk_std_core_hnd__marker _b_11070, kk_std_core_hnd__ev _b_11071, kk_box_t _b_11072, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail4_fun11398__t* _self = kk_function_as(struct kk_std_core_hnd_clause_tail4_fun11398__t*, _fself);
  kk_function_t op = _self->op; /* (10081, 10082, 10083, 10084) -> 10086 10085 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(_b_11071, (KK_I32(3)), _ctx);
  kk_std_core_types__tuple4_ x_11076 = kk_std_core_types__tuple4__unbox(_b_11072, _ctx); /*(10081, 10082, 10083, 10084)*/;
  kk_box_t _x11399 = kk_std_core_types_fst_2(x_11076, _ctx); /*1000*/
  kk_box_t _x11400 = kk_std_core_types_snd_2(x_11076, _ctx); /*1001*/
  kk_box_t _x11401 = kk_std_core_types_thd_1(x_11076, _ctx); /*1002*/
  kk_box_t _x11402;
  kk_box_t _brw_11110 = kk_std_core_types_field4(x_11076, _ctx); /*1003*/;
  kk_std_core_types__tuple4__dropn(x_11076, (KK_I32(4)), _ctx);
  _x11402 = _brw_11110; /*1003*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_box_t, kk_context_t*), op, (op, _x11399, _x11400, _x11401, _x11402, _ctx));
}

kk_std_core_hnd__clause1 kk_std_core_hnd_clause_tail4(kk_function_t op, kk_context_t* _ctx) { /* forall<a,b,c,d,a1,e,b1,c1> (op : (a, b, c, d) -> e a1) -> clause1<(a, b, c, d),a1,b1,e,c1> */ 
  return kk_std_core_hnd__new_Clause1(kk_std_core_hnd_new_clause_tail4_fun11398(op, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_finally_prompt_fun11403__t {
  struct kk_function_s _base;
  kk_function_t fin;
};
static kk_box_t kk_std_core_hnd_finally_prompt_fun11403(kk_function_t _fself, kk_function_t cont, kk_box_t x, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_finally_prompt_fun11403(kk_function_t fin, kk_context_t* _ctx) {
  struct kk_std_core_hnd_finally_prompt_fun11403__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_finally_prompt_fun11403__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_finally_prompt_fun11403, kk_context());
  _self->fin = fin;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_finally_prompt_fun11403(kk_function_t _fself, kk_function_t cont, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_finally_prompt_fun11403__t* _self = kk_function_as(struct kk_std_core_hnd_finally_prompt_fun11403__t*, _fself);
  kk_function_t fin = _self->fin; /* () -> 10178 () */
  kk_drop_match(_self, {kk_function_dup(fin);}, {}, _ctx)
  kk_box_t _x11404 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), cont, (cont, x, _ctx)); /*10177*/
  return kk_std_core_hnd_finally_prompt(fin, _x11404, _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_finally_prompt_fun11405__t {
  struct kk_function_s _base;
  kk_std_core_hnd__yield_info yld;
};
static kk_box_t kk_std_core_hnd_finally_prompt_fun11405(kk_function_t _fself, kk_box_t ___wildcard__380__43, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_finally_prompt_fun11405(kk_std_core_hnd__yield_info yld, kk_context_t* _ctx) {
  struct kk_std_core_hnd_finally_prompt_fun11405__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_finally_prompt_fun11405__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_finally_prompt_fun11405, kk_context());
  _self->yld = yld;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_finally_prompt_fun11405(kk_function_t _fself, kk_box_t ___wildcard__380__43, kk_context_t* _ctx) {
  struct kk_std_core_hnd_finally_prompt_fun11405__t* _self = kk_function_as(struct kk_std_core_hnd_finally_prompt_fun11405__t*, _fself);
  kk_std_core_hnd__yield_info yld = _self->yld; /* std/core/hnd/yield-info */
  kk_drop_match(_self, {kk_std_core_hnd__yield_info_dup(yld);}, {}, _ctx)
  kk_box_drop(___wildcard__380__43, _ctx);
  return kk_std_core_hnd_unsafe_reyield(yld, _ctx);
}

kk_box_t kk_std_core_hnd_finally_prompt(kk_function_t fin, kk_box_t res, kk_context_t* _ctx) { /* forall<a,e> (fin : () -> e (), res : a) -> e a */ 
  if (kk_yielding(kk_context())) {
    kk_box_drop(res, _ctx);
    bool _match_11108 = kk_yielding_non_final(kk_context()); /*bool*/;
    if (_match_11108) {
      return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_finally_prompt_fun11403(fin, _ctx), _ctx);
    }
    {
      kk_std_core_hnd__yield_info yld = kk_std_core_hnd_yield_capture(_ctx); /*std/core/hnd/yield-info*/;
      kk_unit_t __0 = kk_Unit;
      kk_function_call(kk_unit_t, (kk_function_t, kk_context_t*), fin, (fin, _ctx));
      if (kk_yielding(kk_context())) {
        return kk_std_core_hnd_yield_extend(kk_std_core_hnd_new_finally_prompt_fun11405(yld, _ctx), _ctx);
      }
      {
        return kk_std_core_hnd_unsafe_reyield(yld, _ctx);
      }
    }
  }
  {
    kk_unit_t __ = kk_Unit;
    kk_function_call(kk_unit_t, (kk_function_t, kk_context_t*), fin, (fin, _ctx));
    return res;
  }
}


// lift anonymous function
struct kk_std_core_hnd_initially_prompt_fun11407__t {
  struct kk_function_s _base;
  kk_ref_t count;
  kk_function_t init;
};
static kk_box_t kk_std_core_hnd_initially_prompt_fun11407(kk_function_t _fself, kk_function_t cont, kk_box_t x, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_initially_prompt_fun11407(kk_ref_t count, kk_function_t init, kk_context_t* _ctx) {
  struct kk_std_core_hnd_initially_prompt_fun11407__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_initially_prompt_fun11407__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_initially_prompt_fun11407, kk_context());
  _self->count = count;
  _self->init = init;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_initially_prompt_fun11413__t {
  struct kk_function_s _base;
  kk_function_t cont;
  kk_function_t init;
  kk_box_t x;
};
static kk_box_t kk_std_core_hnd_initially_prompt_fun11413(kk_function_t _fself, kk_box_t ___wildcard__420__47, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_initially_prompt_fun11413(kk_function_t cont, kk_function_t init, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_initially_prompt_fun11413__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_initially_prompt_fun11413__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_initially_prompt_fun11413, kk_context());
  _self->cont = cont;
  _self->init = init;
  _self->x = x;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_initially_prompt_fun11413(kk_function_t _fself, kk_box_t ___wildcard__420__47, kk_context_t* _ctx) {
  struct kk_std_core_hnd_initially_prompt_fun11413__t* _self = kk_function_as(struct kk_std_core_hnd_initially_prompt_fun11413__t*, _fself);
  kk_function_t cont = _self->cont; /* (10322) -> 10332 10331 */
  kk_function_t init = _self->init; /* (int) -> 10332 () */
  kk_box_t x = _self->x; /* 10322 */
  kk_drop_match(_self, {kk_function_dup(cont);kk_function_dup(init);kk_box_dup(x);}, {}, _ctx)
  kk_box_drop(___wildcard__420__47, _ctx);
  kk_box_t _x11414 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), cont, (cont, x, _ctx)); /*10331*/
  return kk_std_core_hnd_initially_prompt(init, _x11414, _ctx);
}
static kk_box_t kk_std_core_hnd_initially_prompt_fun11407(kk_function_t _fself, kk_function_t cont, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_initially_prompt_fun11407__t* _self = kk_function_as(struct kk_std_core_hnd_initially_prompt_fun11407__t*, _fself);
  kk_ref_t count = _self->count; /* ref<global,int> */
  kk_function_t init = _self->init; /* (int) -> 10332 () */
  kk_drop_match(_self, {kk_ref_dup(count);kk_function_dup(init);}, {}, _ctx)
  kk_integer_t cnt;
  kk_box_t _x11408;
  kk_ref_t _x11409 = kk_ref_dup(count); /*ref<global,int>*/
  _x11408 = kk_ref_get(_x11409,kk_context()); /*1001*/
  cnt = kk_integer_unbox(_x11408); /*int*/
  kk_integer_t _b_11084_11082;
  kk_integer_t _x11410 = kk_integer_dup(cnt); /*int*/
  _b_11084_11082 = kk_integer_add(_x11410,(kk_integer_from_small(1)),kk_context()); /*int*/
  kk_unit_t __ = kk_Unit;
  kk_unit_t _brw_11107 = kk_Unit;
  kk_ref_set_borrow(count,(kk_integer_box(_b_11084_11082)),kk_context());
  kk_ref_drop(count, _ctx);
  _brw_11107;
  bool _match_11105 = kk_integer_eq_borrow(cnt,(kk_integer_from_small(0)),kk_context()); /*bool*/;
  if (_match_11105) {
    kk_integer_drop(cnt, _ctx);
    kk_box_t _x11411 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), cont, (cont, x, _ctx)); /*10331*/
    return kk_std_core_hnd_initially_prompt(init, _x11411, _ctx);
  }
  {
    kk_unit_t r = kk_Unit;
    kk_function_t _x11412 = kk_function_dup(init); /*(int) -> 10332 ()*/
    kk_function_call(kk_unit_t, (kk_function_t, kk_integer_t, kk_context_t*), _x11412, (_x11412, cnt, _ctx));
    if (kk_yielding(kk_context())) {
      return kk_std_core_hnd_yield_extend(kk_std_core_hnd_new_initially_prompt_fun11413(cont, init, x, _ctx), _ctx);
    }
    {
      kk_box_t _x11415 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), cont, (cont, x, _ctx)); /*10331*/
      return kk_std_core_hnd_initially_prompt(init, _x11415, _ctx);
    }
  }
}

kk_box_t kk_std_core_hnd_initially_prompt(kk_function_t init, kk_box_t res, kk_context_t* _ctx) { /* forall<a,e> (init : (int) -> e (), res : a) -> e a */ 
  bool _match_11104 = kk_yielding_non_final(kk_context()); /*bool*/;
  if (_match_11104) {
    kk_box_drop(res, _ctx);
    kk_ref_t count = kk_ref_alloc((kk_integer_box(kk_integer_from_small(0))),kk_context()); /*ref<global,int>*/;
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_initially_prompt_fun11407(count, init, _ctx), _ctx);
  }
  {
    kk_function_drop(init, _ctx);
    return res;
  }
}


// lift anonymous function
struct kk_std_core_hnd_initially_fun11417__t {
  struct kk_function_s _base;
  kk_function_t action;
  kk_function_t init;
};
static kk_box_t kk_std_core_hnd_initially_fun11417(kk_function_t _fself, kk_box_t _b_11086, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_initially_fun11417(kk_function_t action, kk_function_t init, kk_context_t* _ctx) {
  struct kk_std_core_hnd_initially_fun11417__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_initially_fun11417__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_initially_fun11417, kk_context());
  _self->action = action;
  _self->init = init;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_initially_fun11417(kk_function_t _fself, kk_box_t _b_11086, kk_context_t* _ctx) {
  struct kk_std_core_hnd_initially_fun11417__t* _self = kk_function_as(struct kk_std_core_hnd_initially_fun11417__t*, _fself);
  kk_function_t action = _self->action; /* () -> 10368 10367 */
  kk_function_t init = _self->init; /* (int) -> 10368 () */
  kk_drop_match(_self, {kk_function_dup(action);kk_function_dup(init);}, {}, _ctx)
  kk_box_drop(_b_11086, _ctx);
  kk_box_t _x11418 = kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), action, (action, _ctx)); /*10367*/
  return kk_std_core_hnd_initially_prompt(init, _x11418, _ctx);
}

kk_box_t kk_std_core_hnd_initially(kk_function_t init, kk_function_t action, kk_context_t* _ctx) { /* forall<a,e> (init : (int) -> e (), action : () -> e a) -> e a */ 
  kk_unit_t __ = kk_Unit;
  kk_function_t _x11416 = kk_function_dup(init); /*(int) -> 10368 ()*/
  kk_function_call(kk_unit_t, (kk_function_t, kk_integer_t, kk_context_t*), _x11416, (_x11416, kk_integer_from_small(0), _ctx));
  if (kk_yielding(kk_context())) {
    return kk_std_core_hnd_yield_extend(kk_std_core_hnd_new_initially_fun11417(action, init, _ctx), _ctx);
  }
  {
    kk_box_t _x11419 = kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), action, (action, _ctx)); /*10367*/
    return kk_std_core_hnd_initially_prompt(init, _x11419, _ctx);
  }
}


// lift anonymous function
struct kk_std_core_hnd_prompt_local_var_fun11421__t {
  struct kk_function_s _base;
  kk_ref_t loc;
  kk_box_t v;
};
static kk_box_t kk_std_core_hnd_prompt_local_var_fun11421(kk_function_t _fself, kk_function_t cont, kk_box_t x, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_prompt_local_var_fun11421(kk_ref_t loc, kk_box_t v, kk_context_t* _ctx) {
  struct kk_std_core_hnd_prompt_local_var_fun11421__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_prompt_local_var_fun11421__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_prompt_local_var_fun11421, kk_context());
  _self->loc = loc;
  _self->v = v;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_prompt_local_var_fun11421(kk_function_t _fself, kk_function_t cont, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_prompt_local_var_fun11421__t* _self = kk_function_as(struct kk_std_core_hnd_prompt_local_var_fun11421__t*, _fself);
  kk_ref_t loc = _self->loc; /* local-var<10461,10458> */
  kk_box_t v = _self->v; /* 10458 */
  kk_drop_match(_self, {kk_ref_dup(loc);kk_box_dup(v);}, {}, _ctx)
  kk_unit_t __0 = kk_Unit;
  kk_ref_set_borrow(loc,v,kk_context());
  kk_box_t _x11422 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), cont, (cont, x, _ctx)); /*10459*/
  return kk_std_core_hnd_prompt_local_var(loc, _x11422, _ctx);
}

kk_box_t kk_std_core_hnd_prompt_local_var(kk_ref_t loc, kk_box_t res, kk_context_t* _ctx) { /* forall<a,b,h> (loc : local-var<h,a>, res : b) -> <div,local<h>> b */ 
  if (kk_yielding(kk_context())) {
    kk_box_drop(res, _ctx);
    kk_box_t v;
    kk_ref_t _x11420 = kk_ref_dup(loc); /*local-var<10461,10458>*/
    v = kk_ref_get(_x11420,kk_context()); /*10458*/
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_prompt_local_var_fun11421(loc, v, _ctx), _ctx);
  }
  {
    kk_ref_drop(loc, _ctx);
    return res;
  }
}


// lift anonymous function
struct kk_std_core_hnd_try_finalize_prompt_fun11425__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_hnd_try_finalize_prompt_fun11425(kk_function_t _fself, kk_function_t _b_11090, kk_box_t _b_11091, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_try_finalize_prompt_fun11425(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_hnd_try_finalize_prompt_fun11425, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_hnd_try_finalize_prompt_fun11425(kk_function_t _fself, kk_function_t _b_11090, kk_box_t _b_11091, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_function_t cont_11095 = _b_11090; /*(10584) -> 10635 10634*/;
  kk_box_t x_11096 = _b_11091; /*10584*/;
  kk_std_core_types__either _x11426;
  kk_box_t _x11427 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), cont_11095, (cont_11095, x_11096, _ctx)); /*10634*/
  _x11426 = kk_std_core_hnd_try_finalize_prompt(_x11427, _ctx); /*either<std/core/hnd/yield-info,1926>*/
  return kk_std_core_types__either_box(_x11426, _ctx);
}

kk_std_core_types__either kk_std_core_hnd_try_finalize_prompt(kk_box_t res, kk_context_t* _ctx) { /* forall<a,e> (res : a) -> e either<yield-info,a> */ 
  bool _match_11102 = kk_yielding_non_final(kk_context()); /*bool*/;
  if (_match_11102) {
    kk_box_drop(res, _ctx);
    kk_box_t _x11424 = kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_try_finalize_prompt_fun11425(_ctx), _ctx); /*3949*/
    return kk_std_core_types__either_unbox(_x11424, _ctx);
  }
  if (kk_yielding(kk_context())) {
    kk_box_drop(res, _ctx);
    kk_std_core_hnd__yield_info _b_11094_11092 = kk_std_core_hnd_yield_capture(_ctx); /*std/core/hnd/yield-info*/;
    return kk_std_core_types__new_Left(kk_std_core_hnd__yield_info_box(_b_11094_11092, _ctx), _ctx);
  }
  {
    return kk_std_core_types__new_Right(res, _ctx);
  }
}


// lift anonymous function
struct kk_std_core_hnd_under3_fun11429__t {
  struct kk_function_s _base;
  kk_std_core_hnd__ev ev;
};
static kk_box_t kk_std_core_hnd_under3_fun11429(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_under3_fun11429(kk_std_core_hnd__ev ev, kk_context_t* _ctx) {
  struct kk_std_core_hnd_under3_fun11429__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_under3_fun11429__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_under3_fun11429, kk_context());
  _self->ev = ev;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_under3_fun11431__t {
  struct kk_function_s _base;
  kk_std_core_hnd__ev ev;
};
static kk_box_t kk_std_core_hnd_under3_fun11431(kk_function_t _fself, kk_function_t cont0, kk_box_t res0, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_under3_fun11431(kk_std_core_hnd__ev ev, kk_context_t* _ctx) {
  struct kk_std_core_hnd_under3_fun11431__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_under3_fun11431__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_under3_fun11431, kk_context());
  _self->ev = ev;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_under3_fun11431(kk_function_t _fself, kk_function_t cont0, kk_box_t res0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_under3_fun11431__t* _self = kk_function_as(struct kk_std_core_hnd_under3_fun11431__t*, _fself);
  kk_std_core_hnd__ev ev = _self->ev; /* std/core/hnd/ev<10703> */
  kk_drop_match(_self, {kk_std_core_hnd__ev_dup(ev);}, {}, _ctx)
  return kk_std_core_hnd_under1x(ev, cont0, res0, _ctx);
}
static kk_box_t kk_std_core_hnd_under3_fun11429(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd_under3_fun11429__t* _self = kk_function_as(struct kk_std_core_hnd_under3_fun11429__t*, _fself);
  kk_std_core_hnd__ev ev = _self->ev; /* std/core/hnd/ev<10703> */
  kk_drop_match(_self, {kk_std_core_hnd__ev_dup(ev);}, {}, _ctx)
  kk_evv_t w00;
  kk_std_core_hnd__ev _x11430 = kk_std_core_hnd__ev_dup(ev); /*std/core/hnd/ev<10703>*/
  w00 = kk_std_core_hnd_evv_swap_with(_x11430, _ctx); /*std/core/hnd/evv<_9260>*/
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), cont, (cont, res, _ctx)); /*10701*/;
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    kk_evv_drop(w00, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_under3_fun11431(ev, _ctx), _ctx);
  }
  {
    kk_std_core_hnd__ev_dropn(ev, (KK_I32(3)), _ctx);
    kk_unit_t __0 = kk_Unit;
    kk_evv_set(w00,kk_context());
    return y;
  }
}

kk_box_t kk_std_core_hnd_under3(kk_std_core_hnd__ev ev, kk_function_t op, kk_box_t x1, kk_box_t x2, kk_box_t x3, kk_context_t* _ctx) { /* forall<a,b,c,d,e,a1> (ev : ev<a1>, op : (a, b, c) -> e d, x1 : a, x2 : b, x3 : c) -> e d */ 
  kk_evv_t w0;
  kk_std_core_hnd__ev _x11428 = kk_std_core_hnd__ev_dup(ev); /*std/core/hnd/ev<10703>*/
  w0 = kk_std_core_hnd_evv_swap_with(_x11428, _ctx); /*std/core/hnd/evv<_10643>*/
  kk_box_t z = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_context_t*), op, (op, x1, x2, x3, _ctx)); /*10701*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w0,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(z, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_under3_fun11429(ev, _ctx), _ctx);
  }
  {
    kk_std_core_hnd__ev_dropn(ev, (KK_I32(3)), _ctx);
    return z;
  }
}


// lift anonymous function
struct kk_std_core_hnd_under4_fun11433__t {
  struct kk_function_s _base;
  kk_std_core_hnd__ev ev;
};
static kk_box_t kk_std_core_hnd_under4_fun11433(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_under4_fun11433(kk_std_core_hnd__ev ev, kk_context_t* _ctx) {
  struct kk_std_core_hnd_under4_fun11433__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_under4_fun11433__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_under4_fun11433, kk_context());
  _self->ev = ev;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_under4_fun11435__t {
  struct kk_function_s _base;
  kk_std_core_hnd__ev ev;
};
static kk_box_t kk_std_core_hnd_under4_fun11435(kk_function_t _fself, kk_function_t cont0, kk_box_t res0, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_under4_fun11435(kk_std_core_hnd__ev ev, kk_context_t* _ctx) {
  struct kk_std_core_hnd_under4_fun11435__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_under4_fun11435__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_under4_fun11435, kk_context());
  _self->ev = ev;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_under4_fun11435(kk_function_t _fself, kk_function_t cont0, kk_box_t res0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_under4_fun11435__t* _self = kk_function_as(struct kk_std_core_hnd_under4_fun11435__t*, _fself);
  kk_std_core_hnd__ev ev = _self->ev; /* std/core/hnd/ev<10776> */
  kk_drop_match(_self, {kk_std_core_hnd__ev_dup(ev);}, {}, _ctx)
  return kk_std_core_hnd_under1x(ev, cont0, res0, _ctx);
}
static kk_box_t kk_std_core_hnd_under4_fun11433(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd_under4_fun11433__t* _self = kk_function_as(struct kk_std_core_hnd_under4_fun11433__t*, _fself);
  kk_std_core_hnd__ev ev = _self->ev; /* std/core/hnd/ev<10776> */
  kk_drop_match(_self, {kk_std_core_hnd__ev_dup(ev);}, {}, _ctx)
  kk_evv_t w00;
  kk_std_core_hnd__ev _x11434 = kk_std_core_hnd__ev_dup(ev); /*std/core/hnd/ev<10776>*/
  w00 = kk_std_core_hnd_evv_swap_with(_x11434, _ctx); /*std/core/hnd/evv<_9260>*/
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), cont, (cont, res, _ctx)); /*10774*/;
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    kk_evv_drop(w00, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_under4_fun11435(ev, _ctx), _ctx);
  }
  {
    kk_std_core_hnd__ev_dropn(ev, (KK_I32(3)), _ctx);
    kk_unit_t __0 = kk_Unit;
    kk_evv_set(w00,kk_context());
    return y;
  }
}

kk_box_t kk_std_core_hnd_under4(kk_std_core_hnd__ev ev, kk_function_t op, kk_box_t x1, kk_box_t x2, kk_box_t x3, kk_box_t x4, kk_context_t* _ctx) { /* forall<a,b,c,d,a1,e,b1> (ev : ev<b1>, op : (a, b, c, d) -> e a1, x1 : a, x2 : b, x3 : c, x4 : d) -> e a1 */ 
  kk_evv_t w0;
  kk_std_core_hnd__ev _x11432 = kk_std_core_hnd__ev_dup(ev); /*std/core/hnd/ev<10776>*/
  w0 = kk_std_core_hnd_evv_swap_with(_x11432, _ctx); /*std/core/hnd/evv<_10712>*/
  kk_box_t z = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_box_t, kk_context_t*), op, (op, x1, x2, x3, x4, _ctx)); /*10774*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w0,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(z, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_under4_fun11433(ev, _ctx), _ctx);
  }
  {
    kk_std_core_hnd__ev_dropn(ev, (KK_I32(3)), _ctx);
    return z;
  }
}

// initialization
void kk_std_core_hnd__init(kk_context_t* _ctx){
  static bool _kk_initialized = false;
  if (_kk_initialized) return;
  _kk_initialized = true;
  kk_std_core_types__init(_ctx);
  #if defined(KK_CUSTOM_INIT)
    KK_CUSTOM_INIT (_ctx);
  #endif
}

// termination
void kk_std_core_hnd__done(kk_context_t* _ctx){
  static bool _kk_done = false;
  if (_kk_done) return;
  _kk_done = true;
  #if defined(KK_CUSTOM_DONE)
    KK_CUSTOM_DONE (_ctx);
  #endif
  kk_std_core_types__done(_ctx);
}
