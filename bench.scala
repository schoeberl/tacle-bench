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

val CFLAGS = "-g -target patmos-unknown-unknown-elf -O2 " +
        "-mpatmos-disable-stack-cache " +
        "-mpatmos-disable-vliw " +
        "-mpatmos-method-cache-size=4096 " + // -mpatmos-preferred-subfunction-size=0 " +
        "-mpatmos-stack-base=0x200000 -mpatmos-shadow-stack-base=0x1f8000"

val CFLAGS_SC = "-g -target patmos-unknown-unknown-elf -O2 " +
        "-mpatmos-disable-vliw " +
        "-mpatmos-method-cache-size=4096 " + // -mpatmos-preferred-subfunction-size=0 " +
        "-mpatmos-stack-base=0x200000 -mpatmos-shadow-stack-base=0x1f8000 "

        /*
         * paemu-2kwb-0ksc-2kmsc    paemu-2kwb-2ksc-0kmscdo  paemu-4kwb-0ksc-0kmscdo
paemu-2kwb-0ksc-2kmscd   paemu-2kwt-0ksc-2kmscdo  paemu-4kwt-0ksc-0kmscdo
paemu-2kwb-0ksc-2kmscdo  paemu-2kwt-2ksc-0kmscdo
         */
val EMU1 = "../patmos/hardware/build/paemu-4kwt-0ksc-0kmscdo"
val EMU2 = "../patmos/hardware/build/paemu-4kwb-0ksc-0kmscdo"
val EMU3 = "../patmos/hardware/build/paemu-2kwt-0ksc-2kmscdo"
val EMU4 = "../patmos/hardware/build/paemu-2kwb-0ksc-2kmscdo"
val EMU5 = "../patmos/hardware/build/paemu-2kwb-0ksc-2kmsc"

val PATSC = "../patmos/hardware/build/paemu-2kwb-2ksc-0kmscdo"

val selection = Seq(
"benchmarks/sequential/DSPstone_fixed_point/adpcm_g721_board_test1",
"benchmarks/sequential/DSPstone_fixed_point/adpcm_g721_verify1",
"benchmarks/sequential/DSPstone_floating_point/matrix1_float1",
// "benchmarks/sequential/MISC/codecs_dcodhuff1", - broken with SC optimization
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

// just compile a benchmark
def compile(s: String, cflags: String) = {
  val d = s.substring(0, s.length-1)
  val cmd = Seq("sh", "-c", "patmos-clang "+cflags+" "+d+"/*.c -o "+d+"/a.out")
  cmd!
  
}

// compile and run in the simulator
def doit(s: String, cflags: String) = {
  
  compile(s, cflags)
  val d = s.substring(0, s.length-1)
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

def runEmu(s: String, e: String) = {
  val d = s.substring(0, s.length-1)
  val run = Seq("sh", "-c", e+" -p "+d+"/a.out")
  val res: String = run.!!
  val p1 = res.indexOf(":")
  val p2 = res.indexOf("\n")
  val cycles = res.substring(p1+1, p2).trim.toInt
  println(cycles+" "+d)
}

val lines = dirs.split("\\n")

// Seq or List?
val some = Seq(lines(0), lines(1), lines(2), lines(3), lines(4))
val abc = Seq("benchmarks/sequential/MISC/codecs_dcodrle1:", "benchmarks/sequential/MISC/g721_encode:")
val mrtc = Seq("benchmarks/sequential/MRTC/adpcm_encoder:", "benchmarks/sequential/MRTC/binarysearch:")

// run all benchmarks (and maybe select the output to restrict to useful cycles
// println("CFLAGS:")
// lines.map{ d => doit(d, CFLAGS) }

// run a selection
//println("CFLAGS:")
//selection.map{ d => doit(d, CFLAGS) }
//println("CFLAGS_SC:")
//selection.map{ d => doit(d, CFLAGS_SC) }

// compile
selection.map{ d => compile(d, CFLAGS)}
// run emulator
println(EMU1)
selection.map{ d => runEmu(d, EMU1)}
println(EMU2)
selection.map{ d => runEmu(d, EMU2)}
println(EMU3)
selection.map{ d => runEmu(d, EMU3)}
println(EMU4)
selection.map{ d => runEmu(d, EMU4)}
println(EMU5)
selection.map{ d => runEmu(d, EMU5)}

// compile
selection.map{ d => compile(d, CFLAGS_SC)}
// run emulator
println(PATSC)
selection.map{ d => runEmu(d, PATSC)}
