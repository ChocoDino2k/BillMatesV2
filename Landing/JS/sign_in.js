function collectInfo() {
  const htmlInputs = document.querySelectorAll("input");
  const inputs = new Array(2);

  inputs[0] = htmlInputs[0].value;
  inputs[1] = htmlInputs[1].value;
  return inputs;
}

async function submit() {
  if (buttonLock) {
    lockButton();
    const values = collectInfo();
    console.log(values);
    const result = await testLoginUP(values[0], values[1]);
    console.log(result);

    if (result.completed && result.success) { //no errors and worked
      redirect("/Home/");
      return;
    } else {
      console.log("gaah");
    }

    unlockButton();
  }
}

function failure() {
  //clear input fields

  //set error message
}
