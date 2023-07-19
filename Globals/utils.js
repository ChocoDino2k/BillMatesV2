function redirect(page) {
  window.location.href = page;
}

//prevent user from making multiple api calls before another has finished
var buttonLock = 1;
function lockButton() {
  buttonLock = 0;
}
function unlockButton() {
  buttonLock = 1;
}
function isButtonLocked() {
  return buttonLock;
}

//acts as a wrapper for error logging, exception handling, and return formats
async function callApi(apiName, infoObject) {

  //switch on which api to call
  let result;
  switch (apiName) {
    case "login":
      result = await testLogin(infoObject.username, infoObject.password);
      return result;
      break;
    case "register":
      result = await testRegister(infoObject.fname, infoObject.lname,
        infoObject.username, infoObject.password);
      return result;
      break;
    default:
      return {
        status: 0,
        error: "could not find API: " + apiName,
        info: []
      };
  }
}


async function testLogin(username, password) {
  //make API call

  //test data for now

  const response = {
    status: 1,
    error: "",
    info: 0
  }
  return response;
}

async function testRegister(firstName, lastName, username, password) {
  //make API call

  //test data for now

  const response = {
    status: 1,
    error: "",
    info: 0
  }
  return response;
}
