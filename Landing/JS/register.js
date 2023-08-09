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

async function register(fname, lname, username, password) {
  const payload = JSON.stringify(
    {
      name: fname + " " + lname,
      username: username,
      password: password
    }
  );
  postOptions.body = payload;
  postOptions.headers.Function = "register";

  const result = await fetch(url, postOptions);
  if ( result.status != 200 ) {
    return defaultResponse;
  }
  const response = await result.text();
  return response;
}

async function submit() {
  if (buttonLock) {
    lockButton();
    const values = collectInfo();
    console.log(values);

    for (let field of values) {
      if (field.trim() == "") {
        unlockButton();
        return defaultResponse;
      }
    }


    //checking on password strength
    //TODO: password strength
    if (values[3] !== values[4]) {
      unlockButton();
      return defaultResponse;
    }

    //API call
    const result = await register(values[0], values[1], values[2], values[3]);
    console.log(result);
    if (result.outcome == SUCCESS) {
      redirect("./index.html");
    } else {
      printError(result.outcome);
      failure();
    }
    unlockButton();
  }
}

function failure() {
  //clear input fields

  //set error message
}
