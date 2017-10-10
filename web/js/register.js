

function Register() {

	//-----------
	this.submit = function() {

		var username = document.getElementById("txtUsername").value;
		var password = document.getElementById("txtPassword").value;

		loadJSON( sprintf("/register.json?username=%s&password=%s", username, password ) , function( obj ) {
			

	    }, function(xhr) {
	    	console.log("Error!");
	    });
	}

	//------------------------
	this.loadJSON = function( path, success, error ) {
	    
	    var xhr = new XMLHttpRequest();
	    xhr.onreadystatechange = function() {
	        if (xhr.readyState === XMLHttpRequest.DONE) {
	            if (xhr.status === 200) {
	                if (success)
	                    success(JSON.parse(xhr.responseText));
	            } else {
	                if (error)
	                    error(xhr);
	            }
	        }
	    };
	    xhr.open("GET", path, true);
	    xhr.send();
	}


	//--------------------
	this.validate = function() {

		var ret = 0;

		v_arr = ["Username", "Password", "RetypePassword"]

		for ( i = 0 ; i < v_arr.length ; i++ ) {

			var v = v_arr[i];
			if ( document.getElementById("txt" + v).value == "" ) {
				document.getElementById("vdl" + v).style.display = "block";
				document.getElementById("vdl" + v).innerHTML = "Cannot be Empty!"; 
				ret = -1;
			} else {
				document.getElementById("vdl"+ v ).style.display = "none";
			}
		}
		if ( ret == 0 ) {
			if ( document.getElementById("txtPassword").value != document.getElementById("txtRetypePassword").value ) {
				document.getElementById("vdlRetypePassword").style.display = "block";
				document.getElementById("vdlRetypePassword").innerHTML = "Repeat Password does not match the password!";
				ret = -1;
			}
		}
		return ret;
	}

	//--------------------
	this.init = function() {

		var rg = this;
		document.getElementById("chkAgree").onclick = function(ev) {
			document.getElementById('butSignup').disabled = !this.checked;
		}
		document.getElementById('butSignup').onclick = function(ev) {
			
			if ( rg.validate() == 0 ) {
				rg.submit();
			}
		}
	}

}

document.addEventListener("DOMContentLoaded", function() {
	rg = new Register();
	rg.init();
});

