	

	function logout() {

		$.getJSON("/logout.json", function( response ) {

			if ( response["statuscode"] == 0 ) {
				
				$.toaster({ 
					priority : 'success', 
					message :"Logout successfully"  
				});
				setTimeout( function() {
					window.location.href = "/login.html";
				}, 300);
					
			} else {
				$.toaster({ 
						priority : 'danger', 
						message :"Failed to logout. " + response["statusmsg"] + " . Please retry."  
				});
			}

	    });
	}
