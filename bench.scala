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

// Probably too many options...

val CFLAGS_MIN = "-g -target patmos-unknown-unknown-elf -O2 " +
        "-mpatmos-disable-vliw " +
        "-mpatmos-method-cache-size=4096 " + //-mpatmos-preferred-subfunction-size=0 " +
        "-mpatmos-stack-base=0x200000 -mpatmos-shadow-stack-base=0x1ff000"

// Partially a big difference between CFLAGS_MIN and CFLAGS
// method cache numbers?
// sqrt is 2 times faster with CFLAGS
// statemate is 2 times faster with CFLAGS_MIN

val CFLAGS = "-g -target patmos-unknown-unknown-elf -O2 " +
        "-mpatmos-disable-stack-cache " +
        "-mpatmos-disable-vliw " +
        "-mpatmos-method-cache-size=4096 -mpatmos-preferred-subfunction-size=0 " +
        "-mpatmos-stack-base=0x200000 -mpatmos-shadow-stack-base=0x1ff000"

val CFLAGS_SC = "-g -target patmos-unknown-unknown-elf -O2 " +
        "-mpatmos-disable-vliw " +
        "-mpatmos-method-cache-size=4096 -mpatmos-preferred-subfunction-size=0 " +
        "-mpatmos-stack-base=0x200000 -mpatmos-shadow-stack-base=0x1f8000 "



val selection = Seq(
"benchmarks/sequential/DSPstone_fixed_point/adpcm_g721_board_test1",
"benchmarks/sequential/DSPstone_fixed_point/adpcm_g721_verify1",
// "benchmarks/sequential/DSPstone_floating_point/matrix1_float1",
"benchmarks/sequential/MISC/codecs_dcodhuff1",
"benchmarks/sequential/MRTC/bsort1001",
"benchmarks/sequential/MRTC/fft11",
"benchmarks/sequential/MRTC/matmult1",
"benchmarks/sequential/MRTC/prime1",
"benchmarks/sequential/MRTC/sqrt1",
"benchmarks/sequential/MRTC/statemate1",
"benchmarks/sequential/MediaBench/cjpeg_jpeg6b_wrbmp1"
)


val dirs = "ls -R benchmarks/sequential" #| "grep :"!!

def fileExists(name: String) = Seq("test", "-f", name).! == 0

def doit(s: String, cflags: String) = {
  val d = s.substring(0, s.length-1)
  // println(d)
  val cmd = Seq("sh", "-c", "patmos-clang "+cflags+" "+d+"/*.c -o "+d+"/a.out")
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
    // if (cycles > 100000 && cycles < 1000000) {
      println(result+" "+d)
    // }
  } else {
    ""
  }
}


val lines = dirs.split("\\n")

// Seq or List?
val some = Seq(lines(0), lines(1), lines(2), lines(3), lines(4))
val abc = Seq("benchmarks/sequential/MISC/codecs_dcodrle1:", "benchmarks/sequential/MISC/g721_encode:")
val mrtc = Seq("benchmarks/sequential/MRTC/adpcm_encoder:", "benchmarks/sequential/MRTC/binarysearch:")

println("CFLAGS_MIN:")
selection.map{ d => doit(d, CFLAGS_MIN) }
println("CFLAGS:")
selection.map{ d => doit(d, CFLAGS) }
println("CFLAGS_SC:")
selection.map{ d => doit(d, CFLAGS_SC) }

