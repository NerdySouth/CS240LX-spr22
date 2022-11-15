// Koka generated module: "prog/assert", koka version: 2.4.0, platform: 64-bit
#include "prog_assert.h"
 
// runtime tag for the `:assert` effect

kk_std_core_hnd__htag kk_prog_assert__tag_assert;
 
// handler for the `:assert` effect

kk_box_t kk_prog_assert__handle_assert(int32_t cfc, kk_prog_assert__hnd_assert hnd, kk_function_t ret, kk_function_t action, kk_context_t* _ctx) { /* forall<a,e,b> (cfc : int32, hnd : .hnd-assert<e,b>, ret : (res : a) -> e b, action : () -> <assert|e> a) -> e b */ 
  kk_std_core_hnd__htag _x557 = kk_std_core_hnd__htag_dup(kk_prog_assert__tag_assert); /*std/core/hnd/htag<prog/assert/.hnd-assert>*/
  return kk_std_core_hnd__hhandle(_x557, cfc, kk_prog_assert__hnd_assert_box(hnd, _ctx), ret, action, _ctx);
}
 
// monadic lift

kk_unit_t kk_prog_assert__mlift421_main(kk_unit_t wild__4, kk_context_t* _ctx) { /* (wild_4 : ()) -> assert () */ 
  bool value3_410;
  kk_integer_t _brw_551 = kk_integer_add_small_const(kk_integer_from_small(4), 2, _ctx); /*int*/;
  bool _brw_552 = kk_integer_eq_borrow(_brw_551,(kk_integer_from_small(5)),kk_context()); /*bool*/;
  kk_integer_drop(_brw_551, _ctx);
  value3_410 = _brw_552; /*bool*/
  kk_std_core_hnd__ev ev_428;
  kk_ssize_t _x563 = (KK_IZ(0)); /*ssize_t*/
  ev_428 = kk_evv_at(_x563,kk_context()); /*std/core/hnd/ev<prog/assert/.hnd-assert>*/
  kk_box_t _x564;
  {
    struct kk_std_core_hnd_Ev* _con565 = kk_std_core_hnd__as_Ev(ev_428);
    kk_std_core_hnd__marker m0 = _con565->marker;
    kk_box_t _box_x465 = _con565->hnd;
    kk_prog_assert__hnd_assert h = kk_prog_assert__hnd_assert_unbox(_box_x465, NULL);
    kk_prog_assert__hnd_assert_dup(h);
    kk_std_core_hnd__clause1 _match_549;
    kk_std_core_hnd__clause1 _brw_550 = kk_prog_assert__select_assert(h, _ctx); /*std/core/hnd/clause1<bool,(),prog/assert/.hnd-assert,93,94>*/;
    kk_prog_assert__hnd_assert_drop(h, _ctx);
    _match_549 = _brw_550; /*std/core/hnd/clause1<bool,(),prog/assert/.hnd-assert,93,94>*/
    {
      kk_function_t _fun_unbox_x469 = _match_549.clause;
      _x564 = kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x469, (_fun_unbox_x469, m0, ev_428, kk_bool_box(value3_410), _ctx)); /*1010*/
    }
  }
  kk_unit_unbox(_x564); return kk_Unit;
}
 
// monadic lift


// lift anonymous function
struct kk_prog_assert__mlift422_main_fun572__t {
  struct kk_function_s _base;
};
static kk_box_t kk_prog_assert__mlift422_main_fun572(kk_function_t _fself, kk_box_t _b_482, kk_context_t* _ctx);
static kk_function_t kk_prog_assert__new_mlift422_main_fun572(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_prog_assert__mlift422_main_fun572, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_prog_assert__mlift422_main_fun572(kk_function_t _fself, kk_box_t _b_482, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_unit_t _x573 = kk_Unit;
  kk_unit_t _x574 = kk_Unit;
  kk_unit_unbox(_b_482);
  kk_prog_assert__mlift421_main(_x574, _ctx);
  return kk_unit_box(_x573);
}

kk_unit_t kk_prog_assert__mlift422_main(kk_unit_t wild__3, kk_context_t* _ctx) { /* (wild_3 : ()) -> assert () */ 
  bool value2_409 = kk_integer_eq_borrow((kk_integer_from_small(3)),(kk_integer_from_small(2)),kk_context()); /*bool*/;
  kk_std_core_hnd__ev ev_433;
  kk_ssize_t _x567 = (KK_IZ(0)); /*ssize_t*/
  ev_433 = kk_evv_at(_x567,kk_context()); /*std/core/hnd/ev<prog/assert/.hnd-assert>*/
  kk_unit_t x_431 = kk_Unit;
  kk_box_t _x568;
  {
    struct kk_std_core_hnd_Ev* _con569 = kk_std_core_hnd__as_Ev(ev_433);
    kk_std_core_hnd__marker m0 = _con569->marker;
    kk_box_t _box_x473 = _con569->hnd;
    kk_prog_assert__hnd_assert h = kk_prog_assert__hnd_assert_unbox(_box_x473, NULL);
    kk_prog_assert__hnd_assert_dup(h);
    kk_std_core_hnd__clause1 _match_547;
    kk_std_core_hnd__clause1 _brw_548 = kk_prog_assert__select_assert(h, _ctx); /*std/core/hnd/clause1<bool,(),prog/assert/.hnd-assert,93,94>*/;
    kk_prog_assert__hnd_assert_drop(h, _ctx);
    _match_547 = _brw_548; /*std/core/hnd/clause1<bool,(),prog/assert/.hnd-assert,93,94>*/
    {
      kk_function_t _fun_unbox_x477 = _match_547.clause;
      _x568 = kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x477, (_fun_unbox_x477, m0, ev_433, kk_bool_box(value2_409), _ctx)); /*1010*/
    }
  }
  kk_unit_unbox(_x568);
  if (kk_yielding(kk_context())) {
    kk_box_t _x571 = kk_std_core_hnd_yield_extend(kk_prog_assert__new_mlift422_main_fun572(_ctx), _ctx); /*1001*/
    kk_unit_unbox(_x571); return kk_Unit;
  }
  {
    kk_prog_assert__mlift421_main(x_431, _ctx); return kk_Unit;
  }
}
 
// monadic lift


// lift anonymous function
struct kk_prog_assert__mlift423_main_fun580__t {
  struct kk_function_s _base;
};
static kk_box_t kk_prog_assert__mlift423_main_fun580(kk_function_t _fself, kk_box_t _b_493, kk_context_t* _ctx);
static kk_function_t kk_prog_assert__new_mlift423_main_fun580(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_prog_assert__mlift423_main_fun580, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_prog_assert__mlift423_main_fun580(kk_function_t _fself, kk_box_t _b_493, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_unit_t _x581 = kk_Unit;
  kk_unit_t _x582 = kk_Unit;
  kk_unit_unbox(_b_493);
  kk_prog_assert__mlift422_main(_x582, _ctx);
  return kk_unit_box(_x581);
}

kk_unit_t kk_prog_assert__mlift423_main(kk_unit_t wild__2, kk_context_t* _ctx) { /* (wild_2 : ()) -> assert () */ 
  bool value1_408;
  kk_integer_t _brw_544 = kk_integer_add_small_const(kk_integer_from_small(2), 1, _ctx); /*int*/;
  bool _brw_545 = kk_integer_eq_borrow((kk_integer_from_small(3)),_brw_544,kk_context()); /*bool*/;
  kk_integer_drop(_brw_544, _ctx);
  value1_408 = _brw_545; /*bool*/
  kk_std_core_hnd__ev ev_438;
  kk_ssize_t _x575 = (KK_IZ(0)); /*ssize_t*/
  ev_438 = kk_evv_at(_x575,kk_context()); /*std/core/hnd/ev<prog/assert/.hnd-assert>*/
  kk_unit_t x_436 = kk_Unit;
  kk_box_t _x576;
  {
    struct kk_std_core_hnd_Ev* _con577 = kk_std_core_hnd__as_Ev(ev_438);
    kk_std_core_hnd__marker m0 = _con577->marker;
    kk_box_t _box_x484 = _con577->hnd;
    kk_prog_assert__hnd_assert h = kk_prog_assert__hnd_assert_unbox(_box_x484, NULL);
    kk_prog_assert__hnd_assert_dup(h);
    kk_std_core_hnd__clause1 _match_542;
    kk_std_core_hnd__clause1 _brw_543 = kk_prog_assert__select_assert(h, _ctx); /*std/core/hnd/clause1<bool,(),prog/assert/.hnd-assert,93,94>*/;
    kk_prog_assert__hnd_assert_drop(h, _ctx);
    _match_542 = _brw_543; /*std/core/hnd/clause1<bool,(),prog/assert/.hnd-assert,93,94>*/
    {
      kk_function_t _fun_unbox_x488 = _match_542.clause;
      _x576 = kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x488, (_fun_unbox_x488, m0, ev_438, kk_bool_box(value1_408), _ctx)); /*1010*/
    }
  }
  kk_unit_unbox(_x576);
  if (kk_yielding(kk_context())) {
    kk_box_t _x579 = kk_std_core_hnd_yield_extend(kk_prog_assert__new_mlift423_main_fun580(_ctx), _ctx); /*1001*/
    kk_unit_unbox(_x579); return kk_Unit;
  }
  {
    kk_prog_assert__mlift422_main(x_436, _ctx); return kk_Unit;
  }
}


// lift anonymous function
struct kk_prog_assert_main_fun584__t {
  struct kk_function_s _base;
};
static kk_unit_t kk_prog_assert_main_fun584(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__516__16, bool x, kk_context_t* _ctx);
static kk_function_t kk_prog_assert_new_main_fun584(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_prog_assert_main_fun584, _ctx)
  return kk_function_dup(_fself);
}



// lift anonymous function
struct kk_prog_assert_main_fun586__t {
  struct kk_function_s _base;
  bool x;
};
static kk_box_t kk_prog_assert_main_fun586(kk_function_t _fself, kk_function_t _b_504, kk_context_t* _ctx);
static kk_function_t kk_prog_assert_new_main_fun586(bool x, kk_context_t* _ctx) {
  struct kk_prog_assert_main_fun586__t* _self = kk_function_alloc_as(struct kk_prog_assert_main_fun586__t, 1, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_prog_assert_main_fun586, kk_context());
  _self->x = x;
  return &_self->_base;
}



// lift anonymous function
struct kk_prog_assert_main_fun587__t {
  struct kk_function_s _base;
  kk_function_t _b_504;
};
static kk_unit_t kk_prog_assert_main_fun587(kk_function_t _fself, kk_std_core_hnd__resume_result _b_505, kk_context_t* _ctx);
static kk_function_t kk_prog_assert_new_main_fun587(kk_function_t _b_504, kk_context_t* _ctx) {
  struct kk_prog_assert_main_fun587__t* _self = kk_function_alloc_as(struct kk_prog_assert_main_fun587__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_prog_assert_main_fun587, kk_context());
  _self->_b_504 = _b_504;
  return &_self->_base;
}

static kk_unit_t kk_prog_assert_main_fun587(kk_function_t _fself, kk_std_core_hnd__resume_result _b_505, kk_context_t* _ctx) {
  struct kk_prog_assert_main_fun587__t* _self = kk_function_as(struct kk_prog_assert_main_fun587__t*, _fself);
  kk_function_t _b_504 = _self->_b_504; /* (std/core/hnd/resume-result<1000,1002>) -> 1001 1002 */
  kk_drop_match(_self, {kk_function_dup(_b_504);}, {}, _ctx)
  kk_box_t _x588 = kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__resume_result, kk_context_t*), _b_504, (_b_504, _b_505, _ctx)); /*1002*/
  return kk_unit_unbox(_x588);
}


// lift anonymous function
struct kk_prog_assert_main_fun589__t {
  struct kk_function_s _base;
};
static kk_unit_t kk_prog_assert_main_fun589(kk_function_t _fself, bool value, kk_function_t resume, kk_context_t* _ctx);
static kk_function_t kk_prog_assert_new_main_fun589(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_prog_assert_main_fun589, _ctx)
  return kk_function_dup(_fself);
}

static kk_unit_t kk_prog_assert_main_fun589(kk_function_t _fself, bool value, kk_function_t resume, kk_context_t* _ctx) {
  kk_unused(_fself);
  if (value) {
    kk_unit_t __ = kk_Unit;
    kk_string_t _x590;
    kk_define_string_literal(, _s591, 17, "assertion success")
    _x590 = kk_string_dup(_s591); /*string*/
    kk_std_core_printsln(_x590, _ctx);
    kk_unit_t __0 = kk_Unit;
    kk_function_call(kk_unit_t, (kk_function_t, kk_unit_t, kk_context_t*), resume, (resume, kk_Unit, _ctx));
    return kk_Unit;
  }
  {
    kk_function_drop(resume, _ctx);
    kk_unit_t __1 = kk_Unit;
    kk_string_t _x592;
    kk_define_string_literal(, _s593, 17, "assertion failure")
    _x592 = kk_string_dup(_s593); /*string*/
    kk_std_core_printsln(_x592, _ctx);
    return kk_Unit;
  }
}


// lift anonymous function
struct kk_prog_assert_main_fun594__t {
  struct kk_function_s _base;
  kk_function_t _b_533_496;
};
static kk_box_t kk_prog_assert_main_fun594(kk_function_t _fself, kk_box_t _b_498, kk_function_t _b_499, kk_context_t* _ctx);
static kk_function_t kk_prog_assert_new_main_fun594(kk_function_t _b_533_496, kk_context_t* _ctx) {
  struct kk_prog_assert_main_fun594__t* _self = kk_function_alloc_as(struct kk_prog_assert_main_fun594__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_prog_assert_main_fun594, kk_context());
  _self->_b_533_496 = _b_533_496;
  return &_self->_base;
}



// lift anonymous function
struct kk_prog_assert_main_fun597__t {
  struct kk_function_s _base;
  kk_function_t _b_499;
};
static kk_unit_t kk_prog_assert_main_fun597(kk_function_t _fself, kk_unit_t _b_500, kk_context_t* _ctx);
static kk_function_t kk_prog_assert_new_main_fun597(kk_function_t _b_499, kk_context_t* _ctx) {
  struct kk_prog_assert_main_fun597__t* _self = kk_function_alloc_as(struct kk_prog_assert_main_fun597__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_prog_assert_main_fun597, kk_context());
  _self->_b_499 = _b_499;
  return &_self->_base;
}

static kk_unit_t kk_prog_assert_main_fun597(kk_function_t _fself, kk_unit_t _b_500, kk_context_t* _ctx) {
  struct kk_prog_assert_main_fun597__t* _self = kk_function_as(struct kk_prog_assert_main_fun597__t*, _fself);
  kk_function_t _b_499 = _self->_b_499; /* (1001) -> 1002 1003 */
  kk_drop_match(_self, {kk_function_dup(_b_499);}, {}, _ctx)
  kk_box_t _x598 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _b_499, (_b_499, kk_unit_box(_b_500), _ctx)); /*1003*/
  return kk_unit_unbox(_x598);
}
static kk_box_t kk_prog_assert_main_fun594(kk_function_t _fself, kk_box_t _b_498, kk_function_t _b_499, kk_context_t* _ctx) {
  struct kk_prog_assert_main_fun594__t* _self = kk_function_as(struct kk_prog_assert_main_fun594__t*, _fself);
  kk_function_t _b_533_496 = _self->_b_533_496; /* (value : bool, resume : (()) -> console ()) -> console () */
  kk_drop_match(_self, {kk_function_dup(_b_533_496);}, {}, _ctx)
  kk_unit_t _x595 = kk_Unit;
  bool _x596 = kk_bool_unbox(_b_498); /*bool*/
  kk_function_call(kk_unit_t, (kk_function_t, bool, kk_function_t, kk_context_t*), _b_533_496, (_b_533_496, _x596, kk_prog_assert_new_main_fun597(_b_499, _ctx), _ctx));
  return kk_unit_box(_x595);
}


// lift anonymous function
struct kk_prog_assert_main_fun599__t {
  struct kk_function_s _base;
  kk_function_t _b_534_497;
};
static kk_box_t kk_prog_assert_main_fun599(kk_function_t _fself, kk_std_core_hnd__resume_result _b_501, kk_context_t* _ctx);
static kk_function_t kk_prog_assert_new_main_fun599(kk_function_t _b_534_497, kk_context_t* _ctx) {
  struct kk_prog_assert_main_fun599__t* _self = kk_function_alloc_as(struct kk_prog_assert_main_fun599__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_prog_assert_main_fun599, kk_context());
  _self->_b_534_497 = _b_534_497;
  return &_self->_base;
}

static kk_box_t kk_prog_assert_main_fun599(kk_function_t _fself, kk_std_core_hnd__resume_result _b_501, kk_context_t* _ctx) {
  struct kk_prog_assert_main_fun599__t* _self = kk_function_as(struct kk_prog_assert_main_fun599__t*, _fself);
  kk_function_t _b_534_497 = _self->_b_534_497; /* (std/core/hnd/resume-result<(),()>) -> console () */
  kk_drop_match(_self, {kk_function_dup(_b_534_497);}, {}, _ctx)
  kk_unit_t _x600 = kk_Unit;
  kk_function_call(kk_unit_t, (kk_function_t, kk_std_core_hnd__resume_result, kk_context_t*), _b_534_497, (_b_534_497, _b_501, _ctx));
  return kk_unit_box(_x600);
}
static kk_box_t kk_prog_assert_main_fun586(kk_function_t _fself, kk_function_t _b_504, kk_context_t* _ctx) {
  struct kk_prog_assert_main_fun586__t* _self = kk_function_as(struct kk_prog_assert_main_fun586__t*, _fself);
  bool x = _self->x; /* bool */
  kk_drop_match(_self, {;}, {}, _ctx)
  kk_function_t k0_536 = kk_prog_assert_new_main_fun587(_b_504, _ctx); /*(std/core/hnd/resume-result<(),()>) -> console ()*/;
  bool _b_532_495 = x; /*bool*/;
  kk_function_t _b_533_496 = kk_prog_assert_new_main_fun589(_ctx); /*(value : bool, resume : (()) -> console ()) -> console ()*/;
  kk_function_t _b_534_497 = k0_536; /*(std/core/hnd/resume-result<(),()>) -> console ()*/;
  return kk_std_core_hnd_protect(kk_bool_box(_b_532_495), kk_prog_assert_new_main_fun594(_b_533_496, _ctx), kk_prog_assert_new_main_fun599(_b_534_497, _ctx), _ctx);
}
static kk_unit_t kk_prog_assert_main_fun584(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ___wildcard__516__16, bool x, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_std_core_hnd__ev_dropn(___wildcard__516__16, (KK_I32(3)), _ctx);
  kk_box_t _x585 = kk_std_core_hnd_yield_to(m0, kk_prog_assert_new_main_fun586(x, _ctx), _ctx); /*1000*/
  return kk_unit_unbox(_x585);
}


// lift anonymous function
struct kk_prog_assert_main_fun603__t {
  struct kk_function_s _base;
  kk_function_t _b_529_506;
};
static kk_box_t kk_prog_assert_main_fun603(kk_function_t _fself, kk_std_core_hnd__marker _b_507, kk_std_core_hnd__ev _b_508, kk_box_t _b_509, kk_context_t* _ctx);
static kk_function_t kk_prog_assert_new_main_fun603(kk_function_t _b_529_506, kk_context_t* _ctx) {
  struct kk_prog_assert_main_fun603__t* _self = kk_function_alloc_as(struct kk_prog_assert_main_fun603__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_prog_assert_main_fun603, kk_context());
  _self->_b_529_506 = _b_529_506;
  return &_self->_base;
}

static kk_box_t kk_prog_assert_main_fun603(kk_function_t _fself, kk_std_core_hnd__marker _b_507, kk_std_core_hnd__ev _b_508, kk_box_t _b_509, kk_context_t* _ctx) {
  struct kk_prog_assert_main_fun603__t* _self = kk_function_as(struct kk_prog_assert_main_fun603__t*, _fself);
  kk_function_t _b_529_506 = _self->_b_529_506; /* (m0 : std/core/hnd/marker<console,()>, std/core/hnd/ev<prog/assert/.hnd-assert>, x : bool) -> console () */
  kk_drop_match(_self, {kk_function_dup(_b_529_506);}, {}, _ctx)
  kk_unit_t _x604 = kk_Unit;
  bool _x605 = kk_bool_unbox(_b_509); /*bool*/
  kk_function_call(kk_unit_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, bool, kk_context_t*), _b_529_506, (_b_529_506, _b_507, _b_508, _x605, _ctx));
  return kk_unit_box(_x604);
}


// lift anonymous function
struct kk_prog_assert_main_fun606__t {
  struct kk_function_s _base;
};
static kk_box_t kk_prog_assert_main_fun606(kk_function_t _fself, kk_box_t _b_524, kk_context_t* _ctx);
static kk_function_t kk_prog_assert_new_main_fun606(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_prog_assert_main_fun606, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_prog_assert_main_fun606(kk_function_t _fself, kk_box_t _b_524, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_unit_t _x_537 = kk_Unit;
  kk_unit_unbox(_b_524);
  return kk_unit_box(_x_537);
}


// lift anonymous function
struct kk_prog_assert_main_fun607__t {
  struct kk_function_s _base;
};
static kk_box_t kk_prog_assert_main_fun607(kk_function_t _fself, kk_context_t* _ctx);
static kk_function_t kk_prog_assert_new_main_fun607(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_prog_assert_main_fun607, _ctx)
  return kk_function_dup(_fself);
}



// lift anonymous function
struct kk_prog_assert_main_fun614__t {
  struct kk_function_s _base;
};
static kk_box_t kk_prog_assert_main_fun614(kk_function_t _fself, kk_box_t _b_519, kk_context_t* _ctx);
static kk_function_t kk_prog_assert_new_main_fun614(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_prog_assert_main_fun614, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_prog_assert_main_fun614(kk_function_t _fself, kk_box_t _b_519, kk_context_t* _ctx) {
  kk_unused(_fself);
  kk_unit_t _x615 = kk_Unit;
  kk_unit_t _x616 = kk_Unit;
  kk_unit_unbox(_b_519);
  kk_prog_assert__mlift423_main(_x616, _ctx);
  return kk_unit_box(_x615);
}
static kk_box_t kk_prog_assert_main_fun607(kk_function_t _fself, kk_context_t* _ctx) {
  kk_unused(_fself);
  bool value0_407 = kk_integer_eq_borrow((kk_integer_from_small(3)),(kk_integer_from_small(3)),kk_context()); /*bool*/;
  kk_std_core_hnd__ev ev_444;
  kk_ssize_t _x608 = (KK_IZ(0)); /*ssize_t*/
  ev_444 = kk_evv_at(_x608,kk_context()); /*std/core/hnd/ev<prog/assert/.hnd-assert>*/
  kk_unit_t x0_442 = kk_Unit;
  kk_box_t _x609;
  {
    struct kk_std_core_hnd_Ev* _con610 = kk_std_core_hnd__as_Ev(ev_444);
    kk_std_core_hnd__marker m00 = _con610->marker;
    kk_box_t _box_x510 = _con610->hnd;
    kk_prog_assert__hnd_assert h = kk_prog_assert__hnd_assert_unbox(_box_x510, NULL);
    kk_prog_assert__hnd_assert_dup(h);
    kk_std_core_hnd__clause1 _match_539;
    kk_std_core_hnd__clause1 _brw_540 = kk_prog_assert__select_assert(h, _ctx); /*std/core/hnd/clause1<bool,(),prog/assert/.hnd-assert,93,94>*/;
    kk_prog_assert__hnd_assert_drop(h, _ctx);
    _match_539 = _brw_540; /*std/core/hnd/clause1<bool,(),prog/assert/.hnd-assert,93,94>*/
    {
      kk_function_t _fun_unbox_x514 = _match_539.clause;
      _x609 = kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x514, (_fun_unbox_x514, m00, ev_444, kk_bool_box(value0_407), _ctx)); /*1010*/
    }
  }
  kk_unit_unbox(_x609);
  kk_unit_t _x612 = kk_Unit;
  if (kk_yielding(kk_context())) {
    kk_box_t _x613 = kk_std_core_hnd_yield_extend(kk_prog_assert_new_main_fun614(_ctx), _ctx); /*1001*/
    kk_unit_unbox(_x613);
  }
  else {
    kk_prog_assert__mlift423_main(x0_442, _ctx);
  }
  return kk_unit_box(_x612);
}

kk_unit_t kk_prog_assert_main(kk_context_t* _ctx) { /* () -> console () */ 
  int32_t _b_525_520 = (KK_I32(3)); /*int32*/;
  kk_box_t _x583;
  kk_function_t _b_529_506 = kk_prog_assert_new_main_fun584(_ctx); /*(m0 : std/core/hnd/marker<console,()>, std/core/hnd/ev<prog/assert/.hnd-assert>, x : bool) -> console ()*/;
  kk_prog_assert__hnd_assert _x601;
  kk_std_core_hnd__clause1 _x602 = kk_std_core_hnd__new_Clause1(kk_prog_assert_new_main_fun603(_b_529_506, _ctx), _ctx); /*std/core/hnd/clause1<1015,1016,1017,1018,1019>*/
  _x601 = kk_prog_assert__new_Hnd_assert(kk_reuse_null, _x602, _ctx); /*prog/assert/.hnd-assert<7,8>*/
  _x583 = kk_prog_assert__handle_assert(_b_525_520, _x601, kk_prog_assert_new_main_fun606(_ctx), kk_prog_assert_new_main_fun607(_ctx), _ctx); /*77*/
  kk_unit_unbox(_x583); return kk_Unit;
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
    kk_string_t _x555;
    kk_define_string_literal(, _s556, 13, "assert.assert")
    _x555 = kk_string_dup(_s556); /*string*/
    kk_prog_assert__tag_assert = kk_std_core_hnd__new_Htag(_x555, _ctx); /*std/core/hnd/htag<prog/assert/.hnd-assert>*/
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
