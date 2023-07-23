function collectInfo() {
  const htmlInputs = document.querySelectorAll("input");
  const inputs = new Array(5);

  inputs[0] = htmlInputs[0].value;
  inputs[1] = htmlInputs[1].value;
  inputs[2] = htmlInputs[2].value;
  inputs[3] = htmlInputs[3].value;
  inputs[4] = htmlInputs[4].value;

  return inputs;
}

async function submit() {
  if (buttonLock) {
    lockButton();
    const values = collectInfo();
    console.log(values);

    for (let field of values) {
      if (field.trim() == "") {
        unlockButton();
        return {
          completed: 0,
          error: "empty",
          info: 0
        }
      }
    }


    //checking on password strength
    //TODO: password strength
    if (values[3] !== values[4]) {
      unlockButton();
      return {
        completed: 0,
        error: "passwords don't match",
        info: 0
      }
    }

    //API call
    const result = await register(values[0], values[1], values[2], values[3]);
    console.log(result);
    if (result.completed && result.success) {
      redirect("./index.html");
    } else {
      failure();
    }
    unlockButton();
  }
}

function failure() {
  //clear input fields

  //set error message
}
