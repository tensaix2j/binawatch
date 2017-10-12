

function Login() {

	//-----------
	this.submit = function() {

		var params = {
			username : $("#txtUsername").val() ,
			password : $("#txtPassword").val()
		}

		$.getJSON( "/login.json", params, function(response,status) {

			if ( response["statuscode"] == 0 ) {
				
				$.toaster({ 
					priority : 'success', 
					message :"Login successfully"  
				});
				setTimeout( function() {
					window.location.href = "/";
				}, 800);

				
			} else {
				$.toaster({ 
						priority : 'danger', 
						message :"Failed to login. " + response["statusmsg"]  
				});
				if ( response["statuscode"] < 0 ) {
					$("#igrpUsername").addClass("has-error"); 
					$("#vdlUsername").html(response["statusmsg"]); 
					$("#vdlUsername").show();
					
				}
			}
		});
	}

	
	//--------------------
	this.validate = function() {

		var ret = 0;

		var v_arr = ["Username", "Password"]

		for ( i = 0 ; i < v_arr.length ; i++ ) {

			var v = v_arr[i];
			if ( $("#txt" + v).val() == "" ) {
				$("#igrp" + v).addClass("has-error");
				$("#vdl" + v).html("Cannot be empty.");
				$("#vdl" + v).show();
				
				ret = -1;
			} else {
				$("#igrp"+ v ).removeClass("has-error");
				$("#vdl" + v ).hide();

			}
		}
		return ret;
	}

	//--------------------
	this.init = function() {

		var lgn = this;
		$('#butLogin').click( function(ev) {
			if ( lgn.validate() == 0 ) {
				lgn.submit();
			}
		});
	}

}

$(document).ready(function(){
	lgn = new Login();
	lgn.init();
});

