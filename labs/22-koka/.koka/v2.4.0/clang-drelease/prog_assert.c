// Koka generated module: "prog/assert", koka version: 2.4.0, platform: 64-bit
#include "prog_assert.h"
 
// runtime tag for the `:assert` effect

kk_std_core_hnd__htag kk_prog_assert__tag_assert;
 
// handler for the `:assert` effect

kk_box_t kk_prog_assert__handle_assert(int32_t cfc, kk_prog_assert__hnd_assert hnd, kk_function_t ret, kk_function_t action, kk_context_t* _ctx) { /* forall<a,e,b> (cfc : int32, hnd : .hnd-assert<e,b>, ret : (res : a) -> e b, action : () -> <assert|e> a) -> e b */ 
  kk_std_core_hnd__htag _x657 = kk_std_core_hnd__htag_dup(kk_prog_assert__tag_assert); /*std/core/hnd/htag<prog/assert/.hnd-assert>*/
  return kk_std_core_hnd__hhandle(_x657, cfc, kk_prog_assert__hnd_assert_box(hnd, _ctx), ret, action, _ctx);
}
 
// monadic lift

kk_unit_t kk_prog_assert__mlift521_main(kk_unit_t wild__4, kk_context_t* _ctx) { /* (wild_4 : ()) -> assert () */ 
  bool value3_410;
  kk_integer_t _brw_651 = kk_integer_add_small_const(kk_integer_from_small(4), 2, _ctx); /*int*/;
  bool _brw_652 = kk_integer_eq_borrow(_brw_651,(kk_integer_from_small(5)),kk_context()); /*bool*/;
  kk_integer_drop(_brw_651, _ctx);
  value3_410 = _brw_652; /*bool*/
  kk_std_core_hnd__ev ev_528;
  kk_ssize_t _x663 = (KK_IZ(0)); /*ssize_t*/
  ev_528 = kk_evv_at(_x663,kk_context()); /*std/core/hnd/ev<prog/assert/.hnd-assert>*/
  kk_box_t _x664;
  {
    struct kk_std_core_hnd_Ev* _con665 = kk_std_core_hnd__as_Ev(ev_528);
    kk_std_core_hnd__marker m0 = _con665->marker;
    kk_box_t _box_x565 = _con665->hnd;
    kk_prog_assert__hnd_assert h = kk_prog_assert__hnd_assert_unbox(_box_x565, NULL);
    kk_prog_assert__hnd_assert_dup(h);
    kk_std_core_hnd__clause1 _match_649;
    kk_std_core_hnd__clause1 _brw_650 = kk_prog_assert__select_assert(h, _ctx); /*std/core/hnd/clause1<bool,(),prog/assert/.hnd-assert,93,94>*/;
    kk_prog_assert__hnd_assert_drop(h, _ctx);
    _match_649 = _brw_650; /*std/core/hnd/clause1<bool,(),prog/assert/.hnd-assert,93,94>*/
    {
      kk_function_t _fun_unbox_x569 = _match_649.clause;
      _x664 = kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x569, (_fun_unbox_x569, m0, ev_528, kk_bool_box(value3_410), _ctx)); /*52*/
    }
  }
  kk_unit_unbox(_x664); return kk_Unit;
}
 
// monadic lift


// lift anonymous function
struct kk_prog_assert__mlift522_main_fun672__t {
  struct kk_function_s _base;
};
static kk_box_t kk_prog_assert__mlift522_main_fun672(kk_function_t _fself, kk_box_t _b_582, kk_context_t* _ctx);
static kk_function_t kk_prog_assert__new_mlift522_main_fun672(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_prog_assert__mlift522_main_fun672, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_prog_assert__mlift522_main_fun672(kk_function_t _fself, kk_box_t _b_582, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_unit_t _x673 = kk_Unit;
  kk_unit_t _x674 = kk_Unit;
  kk_unit_unbox(_b_582);
  kk_prog_assert__mlift521_main(_x674, _ctx);
  return kk_unit_box(_x673);
}

kk_unit_t kk_prog_assert__mlift522_main(kk_unit_t wild__3, kk_context_t* _ctx) { /* (wild_3 : ()) -> assert () */ 
  bool value2_409 = kk_integer_eq_borrow((kk_integer_from_small(3)),(kk_integer_from_small(2)),kk_context()); /*bool*/;
  kk_std_core_hnd__ev ev_533;
  kk_ssize_t _x667 = (KK_IZ(0)); /*ssize_t*/
  ev_533 = kk_evv_at(_x667,kk_context()); /*std/core/hnd/ev<prog/assert/.hnd-assert>*/
  kk_unit_t x_531 = kk_Unit;
  kk_box_t _x668;
  {
    struct kk_std_core_hnd_Ev* _con669 = kk_std_core_hnd__as_Ev(ev_533);
    kk_std_core_hnd__marker m0 = _con669->marker;
    kk_box_t _box_x573 = _con669->hnd;
    kk_prog_assert__hnd_assert h = kk_prog_assert__hnd_assert_unbox(_box_x573, NULL);
    kk_prog_assert__hnd_assert_dup(h);
    kk_std_core_hnd__clause1 _match_647;
    kk_std_core_hnd__clause1 _brw_648 = kk_prog_assert__select_assert(h, _ctx); /*std/core/hnd/clause1<bool,(),prog/assert/.hnd-assert,93,94>*/;
    kk_prog_assert__hnd_assert_drop(h, _ctx);
    _match_647 = _brw_648; /*std/core/hnd/clause1<bool,(),prog/assert/.hnd-assert,93,94>*/
    {
      kk_function_t _fun_unbox_x577 = _match_647.clause;
      _x668 = kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x577, (_fun_unbox_x577, m0, ev_533, kk_bool_box(value2_409), _ctx)); /*52*/
    }
  }
  kk_unit_unbox(_x668);
  if (kk_yielding(kk_context())) {
    kk_box_t _x671 = kk_std_core_hnd_yield_extend(kk_prog_assert__new_mlift522_main_fun672(_ctx), _ctx); /*3926*/
    kk_unit_unbox(_x671); return kk_Unit;
  }
  {
    kk_prog_assert__mlift521_main(x_531, _ctx); return kk_Unit;
  }
}
 
// monadic lift


// lift anonymous function
struct kk_prog_assert__mlift523_main_fun680__t {
  struct kk_function_s _base;
};
static kk_box_t kk_prog_assert__mlift523_main_fun680(kk_function_t _fself, kk_box_t _b_593, kk_context_t* _ctx);
static kk_function_t kk_prog_assert__new_mlift523_main_fun680(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_prog_assert__mlift523_main_fun680, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_prog_assert__mlift523_main_fun680(kk_function_t _fself, kk_box_t _b_593, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_unit_t _x681 = kk_Unit;
  kk_unit_t _x682 = kk_Unit;
  kk_unit_unbox(_b_593);
  kk_prog_assert__mlift522_main(_x682, _ctx);
  return kk_unit_box(_x681);
}

kk_unit_t kk_prog_assert__mlift523_main(kk_unit_t wild__2, kk_context_t* _ctx) { /* (wild_2 : ()) -> assert () */ 
  bool value1_408;
  kk_integer_t _brw_644 = kk_integer_add_small_const(kk_integer_from_small(2), 1, _ctx); /*int*/;
  bool _brw_645 = kk_integer_eq_borrow((kk_integer_from_small(3)),_brw_644,kk_context()); /*bool*/;
  kk_integer_drop(_brw_644, _ctx);
  value1_408 = _brw_645; /*bool*/
  kk_std_core_hnd__ev ev_538;
  kk_ssize_t _x675 = (KK_IZ(0)); /*ssize_t*/
  ev_538 = kk_evv_at(_x675,kk_context()); /*std/core/hnd/ev<prog/assert/.hnd-assert>*/
  kk_unit_t x_536 = kk_Unit;
  kk_box_t _x676;
  {
    struct kk_std_core_hnd_Ev* _con677 = kk_std_core_hnd__as_Ev(ev_538);
    kk_std_core_hnd__marker m0 = _con677->marker;
    kk_box_t _box_x584 = _con677->hnd;
    kk_prog_assert__hnd_assert h = kk_prog_assert__hnd_assert_unbox(_box_x584, NULL);
    kk_prog_assert__hnd_assert_dup(h);
    kk_std_core_hnd__clause1 _match_642;
    kk_std_core_hnd__clause1 _brw_643 = kk_prog_assert__select_assert(h, _ctx); /*std/core/hnd/clause1<bool,(),prog/assert/.hnd-assert,93,94>*/;
    kk_prog_assert__hnd_assert_drop(h, _ctx);
    _match_642 = _brw_643; /*std/core/hnd/clause1<bool,(),prog/assert/.hnd-assert,93,94>*/
    {
      kk_function_t _fun_unbox_x588 = _match_642.clause;
      _x676 = kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x588, (_fun_unbox_x588, m0, ev_538, kk_bool_box(value1_408), _ctx)); /*52*/
    }
  }
  kk_unit_unbox(_x676);
  if (kk_yielding(kk_context())) {
    kk_box_t _x679 = kk_std_core_hnd_yield_extend(kk_prog_assert__new_mlift523_main_fun680(_ctx), _ctx); /*3926*/
    kk_unit_unbox(_x679); return kk_Unit;
  }
  {
    kk_prog_assert__mlift522_main(x_536, _ctx); return kk_Unit;
  }
}


// lift anonymous function
struct kk_prog_assert_main_fun684__t {
  struct kk_function_s _base;
};
static kk_unit_t kk_prog_assert_main_fun684(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__516__16, bool x, kk_context_t* _ctx);
static kk_function_t kk_prog_assert_new_main_fun684(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_prog_assert_main_fun684, _ctx)
  return kk_function_dup(_fself);
}



// lift anonymous function
struct kk_prog_assert_main_fun686__t {
  struct kk_function_s _base;
  bool x;
};
static kk_box_t kk_prog_assert_main_fun686(kk_function_t _fself, kk_function_t _b_604, kk_context_t* _ctx);
static kk_function_t kk_prog_assert_new_main_fun686(bool x, kk_context_t* _ctx) {
  struct kk_prog_assert_main_fun686__t* _self = kk_function_alloc_as(struct kk_prog_assert_main_fun686__t, 1, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_prog_assert_main_fun686, kk_context());
  _self->x = x;
  return &_self->_base;
}



// lift anonymous function
struct kk_prog_assert_main_fun687__t {
  struct kk_function_s _base;
  kk_function_t _b_604;
};
static kk_unit_t kk_prog_assert_main_fun687(kk_function_t _fself, kk_std_core_hnd__resume_result _b_605, kk_context_t* _ctx);
static kk_function_t kk_prog_assert_new_main_fun687(kk_function_t _b_604, kk_context_t* _ctx) {
  struct kk_prog_assert_main_fun687__t* _self = kk_function_alloc_as(struct kk_prog_assert_main_fun687__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_prog_assert_main_fun687, kk_context());
  _self->_b_604 = _b_604;
  return &_self->_base;
}

static kk_unit_t kk_prog_assert_main_fun687(kk_function_t _fself, kk_std_core_hnd__resume_result _b_605, kk_context_t* _ctx) {
  struct kk_prog_assert_main_fun687__t* _self = kk_function_as(struct kk_prog_assert_main_fun687__t*, _fself);
  kk_function_t _b_604 = _self->_b_604; /* (std/core/hnd/resume-result<6489,6491>) -> 6490 6491 */
  kk_drop_match(_self, {kk_function_dup(_b_604);}, {}, _ctx)
  kk_box_t _x688 = kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__resume_result, kk_context_t*), _b_604, (_b_604, _b_605, _ctx)); /*6491*/
  return kk_unit_unbox(_x688);
}


// lift anonymous function
struct kk_prog_assert_main_fun689__t {
  struct kk_function_s _base;
};
static kk_unit_t kk_prog_assert_main_fun689(kk_function_t _fself, bool value, kk_function_t resume, kk_context_t* _ctx);
static kk_function_t kk_prog_assert_new_main_fun689(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_prog_assert_main_fun689, _ctx)
  return kk_function_dup(_fself);
}

static kk_unit_t kk_prog_assert_main_fun689(kk_function_t _fself, bool value, kk_function_t resume, kk_context_t* _ctx) {
  kk_unused(_fself);
  if (value) {
    kk_unit_t __ = kk_Unit;
    kk_string_t _x690;
    kk_define_string_literal(, _s691, 17, "assertion success")
    _x690 = kk_string_dup(_s691); /*string*/
    kk_std_core_printsln(_x690, _ctx);
    kk_unit_t __0 = kk_Unit;
    kk_function_call(kk_unit_t, (kk_function_t, kk_unit_t, kk_context_t*), resume, (resume, kk_Unit, _ctx));
    return kk_Unit;
  }
  {
    kk_function_drop(resume, _ctx);
    kk_unit_t __1 = kk_Unit;
    kk_string_t _x692;
    kk_define_string_literal(, _s693, 17, "assertion failure")
    _x692 = kk_string_dup(_s693); /*string*/
    kk_std_core_printsln(_x692, _ctx);
    return kk_Unit;
  }
}


// lift anonymous function
struct kk_prog_assert_main_fun694__t {
  struct kk_function_s _base;
  kk_function_t _b_633_596;
};
static kk_box_t kk_prog_assert_main_fun694(kk_function_t _fself, kk_box_t _b_598, kk_function_t _b_599, kk_context_t* _ctx);
static kk_function_t kk_prog_assert_new_main_fun694(kk_function_t _b_633_596, kk_context_t* _ctx) {
  struct kk_prog_assert_main_fun694__t* _self = kk_function_alloc_as(struct kk_prog_assert_main_fun694__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_prog_assert_main_fun694, kk_context());
  _self->_b_633_596 = _b_633_596;
  return &_self->_base;
}



// lift anonymous function
struct kk_prog_assert_main_fun697__t {
  struct kk_function_s _base;
  kk_function_t _b_599;
};
static kk_unit_t kk_prog_assert_main_fun697(kk_function_t _fself, kk_unit_t _b_600, kk_context_t* _ctx);
static kk_function_t kk_prog_assert_new_main_fun697(kk_function_t _b_599, kk_context_t* _ctx) {
  struct kk_prog_assert_main_fun697__t* _self = kk_function_alloc_as(struct kk_prog_assert_main_fun697__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_prog_assert_main_fun697, kk_context());
  _self->_b_599 = _b_599;
  return &_self->_base;
}

static kk_unit_t kk_prog_assert_main_fun697(kk_function_t _fself, kk_unit_t _b_600, kk_context_t* _ctx) {
  struct kk_prog_assert_main_fun697__t* _self = kk_function_as(struct kk_prog_assert_main_fun697__t*, _fself);
  kk_function_t _b_599 = _self->_b_599; /* (7088) -> 7089 7090 */
  kk_drop_match(_self, {kk_function_dup(_b_599);}, {}, _ctx)
  kk_box_t _x698 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _b_599, (_b_599, kk_unit_box(_b_600), _ctx)); /*7090*/
  return kk_unit_unbox(_x698);
}
static kk_box_t kk_prog_assert_main_fun694(kk_function_t _fself, kk_box_t _b_598, kk_function_t _b_599, kk_context_t* _ctx) {
  struct kk_prog_assert_main_fun694__t* _self = kk_function_as(struct kk_prog_assert_main_fun694__t*, _fself);
  kk_function_t _b_633_596 = _self->_b_633_596; /* (value : bool, resume : (()) -> console ()) -> console () */
  kk_drop_match(_self, {kk_function_dup(_b_633_596);}, {}, _ctx)
  kk_unit_t _x695 = kk_Unit;
  bool _x696 = kk_bool_unbox(_b_598); /*bool*/
  kk_function_call(kk_unit_t, (kk_function_t, bool, kk_function_t, kk_context_t*), _b_633_596, (_b_633_596, _x696, kk_prog_assert_new_main_fun697(_b_599, _ctx), _ctx));
  return kk_unit_box(_x695);
}


// lift anonymous function
struct kk_prog_assert_main_fun699__t {
  struct kk_function_s _base;
  kk_function_t _b_634_597;
};
static kk_box_t kk_prog_assert_main_fun699(kk_function_t _fself, kk_std_core_hnd__resume_result _b_601, kk_context_t* _ctx);
static kk_function_t kk_prog_assert_new_main_fun699(kk_function_t _b_634_597, kk_context_t* _ctx) {
  struct kk_prog_assert_main_fun699__t* _self = kk_function_alloc_as(struct kk_prog_assert_main_fun699__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_prog_assert_main_fun699, kk_context());
  _self->_b_634_597 = _b_634_597;
  return &_self->_base;
}

static kk_box_t kk_prog_assert_main_fun699(kk_function_t _fself, kk_std_core_hnd__resume_result _b_601, kk_context_t* _ctx) {
  struct kk_prog_assert_main_fun699__t* _self = kk_function_as(struct kk_prog_assert_main_fun699__t*, _fself);
  kk_function_t _b_634_597 = _self->_b_634_597; /* (std/core/hnd/resume-result<(),()>) -> console () */
  kk_drop_match(_self, {kk_function_dup(_b_634_597);}, {}, _ctx)
  kk_unit_t _x700 = kk_Unit;
  kk_function_call(kk_unit_t, (kk_function_t, kk_std_core_hnd__resume_result, kk_context_t*), _b_634_597, (_b_634_597, _b_601, _ctx));
  return kk_unit_box(_x700);
}
static kk_box_t kk_prog_assert_main_fun686(kk_function_t _fself, kk_function_t _b_604, kk_context_t* _ctx) {
  struct kk_prog_assert_main_fun686__t* _self = kk_function_as(struct kk_prog_assert_main_fun686__t*, _fself);
  bool x = _self->x; /* bool */
  kk_drop_match(_self, {;}, {}, _ctx)
  kk_function_t k0_636 = kk_prog_assert_new_main_fun687(_b_604, _ctx); /*(std/core/hnd/resume-result<(),()>) -> console ()*/;
  bool _b_632_595 = x; /*bool*/;
  kk_function_t _b_633_596 = kk_prog_assert_new_main_fun689(_ctx); /*(value : bool, resume : (()) -> console ()) -> console ()*/;
  kk_function_t _b_634_597 = k0_636; /*(std/core/hnd/resume-result<(),()>) -> console ()*/;
  return kk_std_core_hnd_protect(kk_bool_box(_b_632_595), kk_prog_assert_new_main_fun694(_b_633_596, _ctx), kk_prog_assert_new_main_fun699(_b_634_597, _ctx), _ctx);
}
static kk_unit_t kk_prog_assert_main_fun684(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__516__16, bool x, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_std_core_hnd__ev_dropn(___wildcard__516__16, (KK_I32(3)), _ctx);
  kk_box_t _x685 = kk_std_core_hnd_yield_to(m0, kk_prog_assert_new_main_fun686(x, _ctx), _ctx); /*6489*/
  return kk_unit_unbox(_x685);
}


// lift anonymous function
struct kk_prog_assert_main_fun703__t {
  struct kk_function_s _base;
  kk_function_t _b_629_606;
};
static kk_box_t kk_prog_assert_main_fun703(kk_function_t _fself, kk_std_core_hnd__marker _b_607, kk_std_core_hnd__ev _b_608, kk_box_t _b_609, kk_context_t* _ctx);
static kk_function_t kk_prog_assert_new_main_fun703(kk_function_t _b_629_606, kk_context_t* _ctx) {
  struct kk_prog_assert_main_fun703__t* _self = kk_function_alloc_as(struct kk_prog_assert_main_fun703__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_prog_assert_main_fun703, kk_context());
  _self->_b_629_606 = _b_629_606;
  return &_self->_base;
}

static kk_box_t kk_prog_assert_main_fun703(kk_function_t _fself, kk_std_core_hnd__marker _b_607, kk_std_core_hnd__ev _b_608, kk_box_t _b_609, kk_context_t* _ctx) {
  struct kk_prog_assert_main_fun703__t* _self = kk_function_as(struct kk_prog_assert_main_fun703__t*, _fself);
  kk_function_t _b_629_606 = _self->_b_629_606; /* (m0 : std/core/hnd/marker<console,()>, std/core/hnd/ev<prog/assert/.hnd-assert>, x : bool) -> console () */
  kk_drop_match(_self, {kk_function_dup(_b_629_606);}, {}, _ctx)
  kk_unit_t _x704 = kk_Unit;
  bool _x705 = kk_bool_unbox(_b_609); /*bool*/
  kk_function_call(kk_unit_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, bool, kk_context_t*), _b_629_606, (_b_629_606, _b_607, _b_608, _x705, _ctx));
  return kk_unit_box(_x704);
}


// lift anonymous function
struct kk_prog_assert_main_fun706__t {
  struct kk_function_s _base;
};
static kk_box_t kk_prog_assert_main_fun706(kk_function_t _fself, kk_box_t _b_624, kk_context_t* _ctx);
static kk_function_t kk_prog_assert_new_main_fun706(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_prog_assert_main_fun706, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_prog_assert_main_fun706(kk_function_t _fself, kk_box_t _b_624, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_unit_t _x_637 = kk_Unit;
  kk_unit_unbox(_b_624);
  return kk_unit_box(_x_637);
}


// lift anonymous function
struct kk_prog_assert_main_fun707__t {
  struct kk_function_s _base;
};
static kk_box_t kk_prog_assert_main_fun707(kk_function_t _fself, kk_context_t* _ctx);
static kk_function_t kk_prog_assert_new_main_fun707(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_prog_assert_main_fun707, _ctx)
  return kk_function_dup(_fself);
}



// lift anonymous function
struct kk_prog_assert_main_fun714__t {
  struct kk_function_s _base;
};
static kk_box_t kk_prog_assert_main_fun714(kk_function_t _fself, kk_box_t _b_619, kk_context_t* _ctx);
static kk_function_t kk_prog_assert_new_main_fun714(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_prog_assert_main_fun714, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_prog_assert_main_fun714(kk_function_t _fself, kk_box_t _b_619, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_unit_t _x715 = kk_Unit;
  kk_unit_t _x716 = kk_Unit;
  kk_unit_unbox(_b_619);
  kk_prog_assert__mlift523_main(_x716, _ctx);
  return kk_unit_box(_x715);
}
static kk_box_t kk_prog_assert_main_fun707(kk_function_t _fself, kk_context_t* _ctx) {
  kk_unused(_fself);
  bool value0_407 = kk_integer_eq_borrow((kk_integer_from_small(3)),(kk_integer_from_small(3)),kk_context()); /*bool*/;
  kk_std_core_hnd__ev ev_544;
  kk_ssize_t _x708 = (KK_IZ(0)); /*ssize_t*/
  ev_544 = kk_evv_at(_x708,kk_context()); /*std/core/hnd/ev<prog/assert/.hnd-assert>*/
  kk_unit_t x0_542 = kk_Unit;
  kk_box_t _x709;
  {
    struct kk_std_core_hnd_Ev* _con710 = kk_std_core_hnd__as_Ev(ev_544);
    kk_std_core_hnd__marker m00 = _con710->marker;
    kk_box_t _box_x610 = _con710->hnd;
    kk_prog_assert__hnd_assert h = kk_prog_assert__hnd_assert_unbox(_box_x610, NULL);
    kk_prog_assert__hnd_assert_dup(h);
    kk_std_core_hnd__clause1 _match_639;
    kk_std_core_hnd__clause1 _brw_640 = kk_prog_assert__select_assert(h, _ctx); /*std/core/hnd/clause1<bool,(),prog/assert/.hnd-assert,93,94>*/;
    kk_prog_assert__hnd_assert_drop(h, _ctx);
    _match_639 = _brw_640; /*std/core/hnd/clause1<bool,(),prog/assert/.hnd-assert,93,94>*/
    {
      kk_function_t _fun_unbox_x614 = _match_639.clause;
      _x709 = kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x614, (_fun_unbox_x614, m00, ev_544, kk_bool_box(value0_407), _ctx)); /*52*/
    }
  }
  kk_unit_unbox(_x709);
  kk_unit_t _x712 = kk_Unit;
  if (kk_yielding(kk_context())) {
    kk_box_t _x713 = kk_std_core_hnd_yield_extend(kk_prog_assert_new_main_fun714(_ctx), _ctx); /*3926*/
    kk_unit_unbox(_x713);
  }
  else {
    kk_prog_assert__mlift523_main(x0_542, _ctx);
  }
  return kk_unit_box(_x712);
}

kk_unit_t kk_prog_assert_main(kk_context_t* _ctx) { /* () -> console () */ 
  int32_t _b_625_620 = (KK_I32(3)); /*int32*/;
  kk_box_t _x683;
  kk_function_t _b_629_606 = kk_prog_assert_new_main_fun684(_ctx); /*(m0 : std/core/hnd/marker<console,()>, std/core/hnd/ev<prog/assert/.hnd-assert>, x : bool) -> console ()*/;
  kk_prog_assert__hnd_assert _x701;
  kk_std_core_hnd__clause1 _x702 = kk_std_core_hnd__new_Clause1(kk_prog_assert_new_main_fun703(_b_629_606, _ctx), _ctx); /*std/core/hnd/clause1<51,52,53,54,55>*/
  _x701 = kk_prog_assert__new_Hnd_assert(kk_reuse_null, _x702, _ctx); /*prog/assert/.hnd-assert<7,8>*/
  _x683 = kk_prog_assert__handle_assert(_b_625_620, _x701, kk_prog_assert_new_main_fun706(_ctx), kk_prog_assert_new_main_fun707(_ctx), _ctx); /*77*/
  kk_unit_unbox(_x683); return kk_Unit;
}

// main exit
static void _kk_main_exit(void) {
  kk_context_t* _ctx = kk_get_context();
  kk_prog_assert__done(_ctx);
}

// main entry
int main(int argc, char** argv) {
  kk_assert(sizeof(size_t)==8 && sizeof(void*)==8);
  kk_context_t* _ctx = kk_main_start(argc, argv);
  kk_prog_assert__init(_ctx);
  atexit(&_kk_main_exit);
  kk_prog_assert_main(_ctx);
  kk_prog_assert__done(_ctx);
  kk_main_end(_ctx);
  return 0;
}

// initialization
void kk_prog_assert__init(kk_context_t* _ctx){
  static bool _kk_initialized = false;
  if (_kk_initialized) return;
  _kk_initialized = true;
  kk_std_core_types__init(_ctx);
  kk_std_core_hnd__init(_ctx);
  kk_std_core__init(_ctx);
  #if defined(KK_CUSTOM_INIT)
    KK_CUSTOM_INIT (_ctx);
  #endif
  {
    kk_string_t _x655;
    kk_define_string_literal(, _s656, 13, "assert.assert")
    _x655 = kk_string_dup(_s656); /*string*/
    kk_prog_assert__tag_assert = kk_std_core_hnd__new_Htag(_x655, _ctx); /*std/core/hnd/htag<prog/assert/.hnd-assert>*/
  }
}

// termination
void kk_prog_assert__done(kk_context_t* _ctx){
  static bool _kk_done = false;
  if (_kk_done) return;
  _kk_done = true;
  #if defined(KK_CUSTOM_DONE)
    KK_CUSTOM_DONE (_ctx);
  #endif
  kk_std_core_hnd__htag_drop(kk_prog_assert__tag_assert, _ctx);
  kk_std_core__done(_ctx);
  kk_std_core_hnd__done(_ctx);
  kk_std_core_types__done(_ctx);
}
