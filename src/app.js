Pebble.addEventListener("showConfiguration", function(e) {
    //Load the remote config page
    Pebble.openURL("http://kristof.willen.be/pebble/skylines/config-v3.html");
  }
);


Pebble.addEventListener("webviewclosed",
  function(e) {
    //Get JSON dictionary
    var configuration = JSON.parse(decodeURIComponent(e.response));
    console.log("[DBUG] Configuration window returned: " + JSON.stringify(configuration));
 
    //Send to Pebble, persist there
    Pebble.sendAppMessage(
      {'KEY_CITY': configuration.KEY_CITY, 'KEY_RANDOM': configuration.KEY_RANDOM},
      function(e) {
        console.log("[DBUG] Settings data sent successfully ");
      },
      function(e) {
        console.log("[FAIL] Settings feedback failed!");
      }
    );
  }
);

