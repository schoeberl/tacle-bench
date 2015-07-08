
import scala.sys.process._
// why the below?
import scala.language.postfixOps

val dirs = "ls -R benchmarks/sequential" #| "grep :"!!

def fileExists(name: String) = Seq("test", "-f", name).! == 0

def doit(s: String) = {
  val d = s.substring(0, s.length-1)
  // println(d)
  val cmd = Seq("sh", "-c", "patmos-clang "+d+"/*.c -o "+d+"/a.out")
  cmd!
  // pasim prints results to stderr...
  val run = Seq("sh", "-c", "pasim -v "+d+"/a.out 2>&1 >/dev/null")
  if (fileExists(d+"/a.out")) {
   val tim = (run #| "grep Cycles:")!!

    println(tim.substring(0, tim.length-1)+" "+d)
  } else {
    ""
  }
}


// Seq("sh", "-c", "ls benchmarks/sequential/DSPstone_fixed_point/adpcm_g721_board_test/*.c")!

val lines = dirs.split("\\n")

// Seq or List?
val some = Seq(lines(0), lines(1), lines(2), lines(3), lines(4))

lines.map{ dir => doit(dir) }

//println(dirs)
