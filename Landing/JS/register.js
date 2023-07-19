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
    const format = {
      fname: values[0],
      lname: values[1],
      username: values[2],
      password: values[3],
      rpassword: values[4]
    }
    console.log(format);

    for (let field in format) {
      if (format[field].trim() == "") {
        unlockButton();
        return {
          status: 0,
          error: "empty",
          info: 0
        }
      }
    }


    //checking on password strength
    //TODO: password strength
    if ( format.password !== format.rpassword) {
      unlockButton();
      return {
        status: 0,
        error: "passwords don't match",
        info: 0
      }
    }

    //API call
    const result = await callApi("register", format);

    if (result.status) {
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
