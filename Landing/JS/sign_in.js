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
    const format = {
      username: values[0],
      password: values[1]
    }

    console.log(format);

    //API call
    const result = await callApi("login", format);

    if (result.status) {
      redirect("../Home/");
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
