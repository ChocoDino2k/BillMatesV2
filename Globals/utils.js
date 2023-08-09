const url = "http://127.0.0.1:1025";

//RESPONSE constants
const NORESULT = 0;
const SUCCESS = 1;
const MISSINGPROP = 2;
const BADREQUEST = 3;
const BADPROP = 4;
const BADMETHOD = 5;

const postOptions = {
  method: "POST",
  mode: "cors",
  headers: {
    "Content-Type": "application/json",
    "Function": "undefined"
  },
  body: null
};

const getOptions = {
  method: "GET",
  mode: "cors",
  headers: {
    "Content-Type": "application/json",
    "Function": "undefined"
  }
};

const defaultResponse = {
  outcome: BADREQUEST,
  data: null
}

function redirect(page) {
  window.location.href = page;
}

function closeContainer(container, className) {
  container.parentNode.parentNode.classList = className + " hidden";
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


function getToken() {
  return document.cookie
  .split("; ")
  .find((row) => row.startsWith("token="))
  ?.split("=")[1];
}

function removeToken() {
  document.cookie = "token=; expires=Thu, 01 Jan 1970 00:00:00 GMT; SameSite=None; Secure"
}

function printError( outcome ) {
  switch (outcome) {
    case NORESULT:
      console.log("No result was found");
      break;
    case MISSINGPROP:
      console.log("Missing a property in the input");
      break;
    case BADREQUEST:
      console.log("Made a bad request. Either non-200 response or syntax error while parsing");
      break;
    case BADPROP:
      console.log("Property or value syntax error while parsing");
      break;
    case BADMETHOD:
      console.log("Could not find the method provided in the header");
      break;
    default:
      console.log("unknown error code");
  }
}
