const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<head>

<meta name="viewport" content="width=device-width, initial-scale=1.0" />
<title>EspressIoT</title>
<!-- Compiled and minified CSS -->
<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/materialize/1.0.0/css/materialize.min.css">
<link href="https://fonts.googleapis.com/css?family=Roboto+Slab&display=swap" rel="stylesheet">

<!-- Compiled and minified JavaScript -->
<script src="https://cdnjs.cloudflare.com/ajax/libs/materialize/1.0.0/js/materialize.min.js"></script>
<script type="text/javascript">
let modeToggle;
let instance;
window.onload = function() {
  instance = M.Tabs.init(document.querySelector(".tabs"), { swipeable: false });
  modeToggle = document.getElementById("modeSelect");
  retrieveAndUpdateState();
  setInterval(() => {
    retrieveAndUpdateState();
  }, 2000);

  modeToggle.addEventListener("change", async function() {
    console.dir(this.selectedOptions[0]);
    switch (this.selectedOptions[0].text) {
      case "Off":
        heaterOff();
        break;
      case "Brew":
        heaterOn();
        break;
      case "Steam":
        console.log("steamin");
        break;
      case "Timer":
        console.log("not implemented");
        break;
      default:
        console.log("oops");
    }

    async function heaterOn() {
      let value;
      try {
        result = await fetch("./heateron");
        value = await result.text();
      } catch (e) {
        console.log("something went wrong");
      }
      if (!value) {
        value = "error toggling heater";
      } else {
        M.toast({ html: value, displayLength: 2000 });
      }
    }

    async function heaterOff() {
      let value;
      try {
        result = await fetch("./heateroff");
        value = await result.text();
      } catch (e) {
        console.log("something went wrong");
      }
      if (!value) {
        value = "error toggling heater";
      } else {
        M.toast({ html: value, displayLength: 2000 });
      }
    }
  });

  async function updateMachineSettings() {
      let newValues = {};
      for (item of document.querySelector('#settingsPageForm').elements){
        if(item.type!='checkbox'){
        newValues[item.id] = item.value;
        }else{
          newValues[item.id] = item.checked;
        }
      }
      try {
        result = await fetch("./set_config", {method: 'POST', headers:{'Content-type':'application/json'}, body:JSON.stringify(newValues)});
        value = await result.text();
      } catch (e) {
        console.log("something went wrong");
      }
      if (!value) {
        value = "error updating settings";
      } else {
        M.toast({ html: value, displayLength: 2000 });
      }
    }

  let setTempSlider = document.getElementById("targetSlider");
  setTempSlider.addEventListener("input", function() {
    document.querySelector("#setTargetValue").innerText = setTempSlider.value + String.fromCharCode(176);
  });
  setTempSlider.addEventListener("change", function() {
    updateMachineSettings();
  });


  let setThresholdSlider = document.getElementById("thresholdSlider");
  setThresholdSlider.addEventListener("input", function() {
    document.querySelector("#setThresholdValue").innerText =
      setThresholdSlider.value + String.fromCharCode(176);
  });
  setThresholdSlider.addEventListener("change", function() {
    updateMachineSettings();
  });

  var elems = document.querySelectorAll("select");
  var instances = M.FormSelect.init(elems, {});
};

async function retrieveAndUpdateState() {
  let values;
  try {
    const response = await fetch("./state");
    values = await response.json();
    // if(values.output>0){
    //   heatToggle.checked = true
    // }
  } catch (e) {
    console.error("failed to get state data ", e);
    values = { current: 0, target: 0, output: 0, threshold: 1.5 };
  }
  document.getElementById("current").innerText =
    values.current + String.fromCharCode(176);

  const stringTarget = values.target + String.fromCharCode(176);
  document.getElementById("target").innerText = stringTarget
  document.querySelector("#setTargetValue").innerText = stringTarget

  document.querySelector("#setThresholdValue").innerText = values.threshold+ String.fromCharCode(176);;
  document.getElementById("output").innerText = values.output + "%";
}

async function toggleSteam() {
  let newColor;
  let buttonColor = document.getElementById("steamButton").style
    .backgroundColor;
  if (buttonColor == "rgb(0, 255, 0)") {
    newColor = "rgb(255,0, 0)";
  } else {
    newColor = "rgb(0, 255, 0)";
  }
  document.getElementById("steamButton").style.backgroundColor = newColor;

  let result = await fetch("./toggleSteam");
}

</script>
<style>
body{
  font-family: 'Roboto Slab', serif;
  display: flex;
    min-height: 100vh;
    flex-direction: column;
  }
.currentState {
  font-size:1.5em;
}
.currentState span{
  font-size:2em;
}

</style>
</head>

<header>
  <div class="row">
    <span class="col s8"><h4>EspressIoT</h4></span>
    <div class="input-field col s3">
     <select id ="modeSelect">
       <option value="1">Off</option>
       <option value="2">Brew</option>
       <option value="3">Steam</option>
       <option value="3">Timer</option>
     </select>
     <!-- <label>Mode</label> -->
     <span class="helper-text" data-error="wrong" data-success="right">Mode</span>
   </div>
   </div>
</header>

<main>
<div class="container">
<div class="row">
  <div class="col s12">
    <ul id="tabs-swipe-demo" class="tabs">
      <li class="tab col s3"><a class="active" href="#currentStateTab">Current</a></li>
      <li class="tab col s3"><a href="#settingsTab">Settings</a></li>
      <li class="tab col s3"><a href="#tuningTab">Tuning</a></li>
    </ul>
  </div>
<div id="currentStateTab" class="currentState">
  <div class="row">
    <div class="col s4 offset-s1">Measured Temperature:</div>

    <div class="col s1 offset-s3"><span id="current"/></div>
  </div>

  <div class="row">
    <div class="col s4 offset-s1">Target Temperature:</div>
    <div class="col s1 offset-s3"><span id="target"/></div>
  </div>
  <div class="row">
    <div class="col s4 offset-s1">Heater Power:</div>
    <div class="col s1 offset-s3"><span id="output"/></div>
  </div>
</div>
</div>
 <!--ends the state swipe area-->

 <div id="settingsTab" class="col s12">
   <form action="#" id="settingsPageForm">
  <ul class="wrapper">
    <li>
      <label for="target" style="font-size: 1em;">Target Temperature: <span id="setTargetValue"></span></label>
      <span class="range-field">
        <input type="range" id="targetSlider" min="0" max="120">
        </span>
  
    </li>
    <li>
      <label for="threshold" style="font-size: 1em;">Threshold: <span id="setThresholdValue"></span></label>
      <span class="range-field">
      <input id="thresholdSlider" type="range" min=".5" max="5" step=".5" />
    </span>
    </li>
  </ul>

  <h6>PID Settings</h6>
  <ul class="wrapper">
    <div class="switch row">
      <label class="col s12">
        Normal
        <input type="checkbox" id="isAdaptivePid">
        <span class="lever"></span>
        Adaptive
      </label>
    </div>
    <li class="row">
      <div class="col s3">
      <label for="P">Progressive</label>
      <input type="number" id="adjustP" />
      </div>
    </li>
    <li class="row">
      <div class="col s3">
      <label for="I">Integral</label>
      <input type="number" id="adjustI"/>
    </div>
    </li>
    <li class="row">
      <div class="col s3">
      <label for="D">Derivative</label>
      <input type="number" id="adjustD"  />
    </div>
    </li>
  </ul>
</form>
</div>


 <div id="tuningTab" class="col s12 wrapper">
  <ul>
    <li class="row">
      <div class="col s8">
      <label for="tuningThreshold">Tuning Threshold (Â°C)</label>
      <input type="number" id="tuningThreshold" />
      </div>
    </li>
    <li class="row">
      <div class="col s8">
      <label for="tuningPower">Tuning Power (heater)</label>
      <input type="number" id="tuningPower"/>
    </div>
    </li>
  </ul>

 <a class="waves-effect waves-light btn">Start PID Tuning Mode</a>
</div>
</main>

 <footer>



</footer>



</body>
</html>
)=====";
