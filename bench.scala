/**
 * Benchmark the new stack cache with the TACLe benchmark suit.
 * 
 * Author: Martin Schoeberl
 * 
 * License: Simplified BSD License
 */

/*
 * All benchmarks with pasim and -O0 takes 30'
 * with pasim and -O2 takes 9'
 */
import scala.sys.process._
// why the below?
import scala.language.postfixOps

// println("Benchmarking Patmos")


val dirs = "ls -R benchmarks/sequential" #| "grep :"!!

def fileExists(name: String) = Seq("test", "-f", name).! == 0

def doit(s: String) = {
  val d = s.substring(0, s.length-1)
  // println(d)
  val cmd = Seq("sh", "-c", "patmos-clang -O2 "+d+"/*.c -o "+d+"/a.out")
  cmd!
  // pasim prints results to stderr...
  val run = Seq("sh", "-c", "pasim -v "+d+"/a.out 2>tmp.txt >/dev/null")
  if (fileExists(d+"/a.out")) {
    run!
    val grepTest = Seq("sh", "-c", "grep Cycles: tmp.txt >/dev/null")
    val grep = "grep Cycles: tmp.txt"
    val tim = if (grepTest.! == 0) grep!! else "Crash:"
    val result = tim.substring(0, tim.length-1)
    val cycles = if (result.length > 8) 
      result.substring(8, result.length).trim.toInt
      else 0
    // following does not work when the simulator exits != 0
    // val tim = (run #| "grep Cycles:")!!
    if (cycles > 100000 && cycles < 1000000) {
      println(result+" "+d)
    }
  } else {
    ""
  }
}


val lines = dirs.split("\\n")

// Seq or List?
val some = Seq(lines(0), lines(1), lines(2), lines(3), lines(4))
val abc = Seq("benchmarks/sequential/MISC/codecs_dcodrle1:", "benchmarks/sequential/MISC/g721_encode:")
val mrtc = Seq("benchmarks/sequential/MRTC/adpcm_encoder:", "benchmarks/sequential/MRTC/binarysearch:")
lines.map{ d => doit(d) }

