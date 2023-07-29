const url = "http://127.0.0.1:1025";

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


// call to test for a valid login using a username and password
async function testLoginUP(username, password) {
  const request = JSON.stringify({
    username: username,
    password: password
  });
  const options = {
    method: "POST",
    mode: "no-cors",
    headers: {
      "Content-Type": "application/json",
      "function": "login"
    },
    body: request
  };

  try {
    const result = await fetch(url, options);
    if (result.status == 200) {
      const data = await result.text();
      return data;
    } else {
      return {
        completed: 0,
        reason: result.statusText
      };
    }
  } catch (e) {
    console.log(e);
    return {
      completed: 0,
      reason: e
    };
  }
}


// call to test for a valid login using a username and password
async function register(username, fname, lname, password) {
  const request = JSON.stringify({
    username: username,
    name: fname + " " + lname,
    password: password
  });
  const options = {
    method: "POST",
    mode: "no-cors",
    headers: {
      "Content-Type": "application/json",
      "function": "login"
    },
    body: request
  };

  try {
    const result = await fetch(url, options);
    if (result.status == 200) {
      const data = await result.text();
      return data;
    } else {
      return {
        completed: 0,
        reason: result.statusText
      };
    }
  } catch (e) {
    console.log(e);
    return {
      completed: 0,
      reason: e
    };
  }
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
