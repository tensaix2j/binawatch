
function Apikey() {

	//-----------
	this.submit = function() {

		var params = {
			apikey : $("#txtApikey").val() ,
			secretkey : $("#txtSecretkey").val()
		}

		$.getJSON( "/save_apikey.json", params, function(response,status) {

			if ( response["statuscode"] == 0 ) {
				
				$.toaster({ 
					priority : 'success', 
					message :"API and Secret keys saved"  
				});

			} else {
				$.toaster({ 
						priority : 'danger', 
						message :"Failed to save info. " + response["statusmsg"] + ". Please retry."  
				});
				if ( response["statuscode"] == -5 ) {
					setTimeout( function() {
						window.location.href = "/login.html";
					}, 800);
				}
			}
		});
	}


	
	//--------------------
	this.validate = function() {

		var ret = 0;

		var v_arr = ["Apikey", "Secretkey"]

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

	//-----------------------------
	this.display_apikey = function() {

		var params = {};
		$.getJSON( "/get_apikey.json", params, function(response,status) {

			if ( response["statuscode"] == 0 ) {
				$("#txtApikey").val( response["apikey"] );
				$("#txtSecretkey").val( response["secretkey"] );
				
			} else {
				$.toaster({ 
						priority : 'danger', 
						message :"Failed to get apikey " + response["statusmsg"] + ". Please retry."  
				});
				if ( response["statuscode"] == -5 ) {
					setTimeout( function() {
						window.location.href = "/login.html";
					}, 800);
				}
			}
		});
	}

	//--------------------
	this.init = function() {

		var apky = this;
		$('#butSave').click( function(ev) {
			if ( apky.validate() == 0 ) {
				apky.submit();
			}
		});
		$("#a_logout").click( function() {
			logout();
		});
		this.display_apikey();
	}

}

$(document).ready(function(){
	apky = new Apikey();
	apky.init();
});

