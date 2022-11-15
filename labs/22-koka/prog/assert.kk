effect ctl assert(value: bool) : ()

fun main() : console ()
  //----------------your code here--------------------
  with handler
    ctl assert(value)
        if value == True then 
          println("assertion success")
          resume(())
          ()
        else 
          println("assertion failure") 
          ()


  //----------------end your code--------------------
  assert(3 == 3)
  assert(3 == 2 + 1)
  assert(3 == 2)
  assert(4 + 2 == 5)
  // Should print:q

  // assertion success
  // assertion success
  // assertion failure
