effect ctl assert(v: bool) : ()
effect ctl yield(v: int) : ()

// part a/b
fun num-coins(coins: list<int>, target)
    coins.foreach fn(elem: int) {
        if elem > target then 
            ()
        else
            val curr_target = target - elem;
            if curr_target == 0 then
                yeild(
    ()

// part c
fun least-coins(coins, tgt)
  0

fun print-coin-counts(coins, tgt)
  with handler
    ctl yield(v)
      println(show(v))
      resume(())
  num-coins([1, 2, 3, 4, 5], 5)

fun main()
  print-coin-counts([1, 2, 3, 4, 5], 5)
  with handler
    ctl assert(v)
        if value == True then 
          println("assertion success")
          resume(())
          ()
        else 
          println("assertion failure") 
          ()
  assert(least-coins([1, 2, 3, 4, 5], 1) == 1)
  assert(least-coins([1, 2, 3, 4, 5], 6) == 2)
