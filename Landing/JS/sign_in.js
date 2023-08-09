function collectInfo() {
  const htmlInputs = document.querySelectorAll("input");
  const inputs = new Array(2);

  inputs[0] = htmlInputs[0].value;
  inputs[1] = htmlInputs[1].value;
  return inputs;
}


async function testLoginUP(username, password) {
  const payload = JSON.stringify(
    {
      username: username,
      password: password
    }
  );
  postOptions.body = payload;
  postOptions.headers.Function = "loginUP";

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
    const result = await testLoginUP(values[0], values[1]);
    console.log(result);

    if (result.outcome == SUCCESS) { //no errors and worked
      redirect("/Home/");
      return;
    } else {
      printError(result.outcome);
    }

    unlockButton();
  }
}

function failure() {
  //clear input fields

  //set error message
}
