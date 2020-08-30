const EXAMPLE_CODE = `\
int main() {
    return 233;
}
`;

var codeCM;
var irCM;
var asmCM;
var outputCM;

function setOutput(label, type, text) {
  if (type == "error") {
    label.textContent = "Error:";
    label.setAttribute("class", "text-danger");
  } else if (type == "running") {
    label.textContent = "Running...";
    label.setAttribute("class", "text-warning");
  } else {
    label.textContent = "Output:";
    label.setAttribute("class", "text-success");
  }
  outputCM.setValue(text);
  outputCM.addLineWidget(0, label, { above: true });
}

function compileAndRun() {
  var label = document.getElementById('output-label');
  label.setAttribute("style", "");
  setOutput(label, "running", "");

  var input = codeCM.getValue();
  try {
    var ir = MiniDecaf.compile(input, { target: "ir" });
    irCM.setValue(ir.toString());
    var asm = MiniDecaf.compile(ir, { target: "riscv32-asm" });
    asmCM.setValue(asm);
  } catch (err) {
    irCM.setValue("");
    asmCM.setValue("");
    setOutput(label, "error", err.message);
    return;
  }
  setTimeout(() => {
    try {
      var output = MiniDecaf.compile(ir, { target: "executed" });
      setOutput(label, "output", output);
    } catch (err) {
      setOutput(label, "error", err.message);
    }
  }, 1);
}

$(document).ready(function () {
  var extraKeys = {
    "Tab": function (cm) {
      if (cm.somethingSelected()) {
        cm.indentSelection("add");
      } else {
        cm.replaceSelection(cm.getOption("indentWithTabs") ? "\t" :
          Array(cm.getOption("indentUnit") + 1).join(" "), "end", "+input");
      }
    }
  };

  codeCM = CodeMirror(document.getElementById("minidecaf-input"), {
    lineNumbers: true,
    indentUnit: 4,
    matchBrackets: true,
    styleActiveLine: true,
    mode: "text/x-csrc",
    extraKeys,
  });

  irCM = CodeMirror(document.getElementById("minidecaf-ir"), {
    lineNumbers: true,
    indentUnit: 4,
    styleActiveLine: true,
    mode: { name: "gas", architecture: "ir" },
    extraKeys,
  });

  asmCM = CodeMirror(document.getElementById("minidecaf-asm"), {
    lineNumbers: true,
    indentUnit: 4,
    styleActiveLine: true,
    mode: { name: "gas", architecture: "riscv" },
    extraKeys,
  });

  outputCM = CodeMirror(document.getElementById("minidecaf-output"), {
    readOnly: true,
    lineWrapping: true,
    mode: null,
  });

  codeCM.setValue(EXAMPLE_CODE);
  $("#btn-run").click(compileAndRun);
  compileAndRun();
});
