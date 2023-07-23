import java.nio.file.Files
import java.nio.file.Paths

importCode("../../../examples/demo/src/demo.cpp")
println(workspace)


println("Generating graphviz (dot) files")
var dots_dir : String = Paths.get("dots").toAbsolutePath.toString
Files.createDirectories(Paths.get(dots_dir))

var fn : String = "IsEven"
var fn_lower : String = fn.toLowerCase
cpg.method(fn).dotAst.l |> dots_dir + "/" + fn_lower + "-ast.dot"
cpg.method(fn).dotCfg.l |> dots_dir + "/" + fn_lower + "-cfg.dot"
cpg.method(fn).dotCdg.l |> dots_dir + "/" + fn_lower + "-cdg.dot"
cpg.method(fn).dotDdg.l |> dots_dir + "/" + fn_lower + "-ddg.dot"
cpg.method(fn).dotPdg.l |> dots_dir + "/" + fn_lower + "-pdg.dot"
cpg.method(fn).dotCpg14.l |> dots_dir + "/" + fn_lower + "-cpg14.dot"

fn = "ShiftsToZero"
fn_lower = fn.toLowerCase
cpg.method(fn).dotAst.l |> dots_dir + "/" + fn_lower + "-ast.dot"
cpg.method(fn).dotCfg.l |> dots_dir + "/" + fn_lower + "-cfg.dot"
cpg.method(fn).dotCdg.l |> dots_dir + "/" + fn_lower + "-cdg.dot"
cpg.method(fn).dotDdg.l |> dots_dir + "/" + fn_lower + "-ddg.dot"
cpg.method(fn).dotPdg.l |> dots_dir + "/" + fn_lower + "-pdg.dot"
cpg.method(fn).dotCpg14.l |> dots_dir + "/" + fn_lower + "-cpg14.dot"

fn = "Factorial"
fn_lower = fn.toLowerCase
cpg.method(fn).dotAst.l |> dots_dir + "/" + fn_lower + "-ast.dot"
cpg.method(fn).dotCfg.l |> dots_dir + "/" + fn_lower + "-cfg.dot"
cpg.method(fn).dotCdg.l |> dots_dir + "/" + fn_lower + "-cdg.dot"
cpg.method(fn).dotDdg.l |> dots_dir + "/" + fn_lower + "-ddg.dot"
cpg.method(fn).dotPdg.l |> dots_dir + "/" + fn_lower + "-pdg.dot"
cpg.method(fn).dotCpg14.l |> dots_dir + "/" + fn_lower + "-cpg14.dot"

fn = "IsPrime"
fn_lower = fn.toLowerCase
cpg.method(fn).dotAst.l |> dots_dir + "/" + fn_lower + "-ast.dot"
cpg.method(fn).dotCfg.l |> dots_dir + "/" + fn_lower + "-cfg.dot"
cpg.method(fn).dotCdg.l |> dots_dir + "/" + fn_lower + "-cdg.dot"
cpg.method(fn).dotDdg.l |> dots_dir + "/" + fn_lower + "-ddg.dot"
cpg.method(fn).dotPdg.l |> dots_dir + "/" + fn_lower + "-pdg.dot"
cpg.method(fn).dotCpg14.l |> dots_dir + "/" + fn_lower + "-cpg14.dot"

fn = "NByNIncrements"
fn_lower = fn.toLowerCase
cpg.method(fn).dotAst.l |> dots_dir + "/" + fn_lower + "-ast.dot"
cpg.method(fn).dotCfg.l |> dots_dir + "/" + fn_lower + "-cfg.dot"
cpg.method(fn).dotCdg.l |> dots_dir + "/" + fn_lower + "-cdg.dot"
cpg.method(fn).dotDdg.l |> dots_dir + "/" + fn_lower + "-ddg.dot"
cpg.method(fn).dotPdg.l |> dots_dir + "/" + fn_lower + "-pdg.dot"
cpg.method(fn).dotCpg14.l |> dots_dir + "/" + fn_lower + "-cpg14.dot"

fn = "NByNByNIncrements"
fn_lower = fn.toLowerCase
cpg.method(fn).dotAst.l |> dots_dir + "/" + fn_lower + "-ast.dot"
cpg.method(fn).dotCfg.l |> dots_dir + "/" + fn_lower + "-cfg.dot"
cpg.method(fn).dotCdg.l |> dots_dir + "/" + fn_lower + "-cdg.dot"
cpg.method(fn).dotDdg.l |> dots_dir + "/" + fn_lower + "-ddg.dot"
cpg.method(fn).dotPdg.l |> dots_dir + "/" + fn_lower + "-pdg.dot"
cpg.method(fn).dotCpg14.l |> dots_dir + "/" + fn_lower + "-cpg14.dot"
