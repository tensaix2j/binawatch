

function Binawatch() {

	//---------------
	this.get_allBookTickers = function() {

		$.getJSON("/allBookTickers.json", function( response ) {
			
			bnw.allBookTickers = response;
            bnw.render_list();
            	    	
	    });
	}

	//--------
	this.logout = function() {

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


	//-----
	this.render_list = function() {
			
		var mylist = $("#threadindex_list");
		
		var str = ""
		if ( typeof this.allBookTickers != 'undefined' ) {
			var data = this.allBookTickers;

			for ( var i = 0 ; i < data.length ; i++ ) {

				if (  data[i]["bidPrice"] > 0.0 ) {

					str += "<li>";

						var symbol 		= data[i]["symbol"];
						var base_curr   = symbol.slice(-3);
						var pts 		= 8;

						if ( base_curr != "BTC" && base_curr != "ETH" ) {
							pts = 2;
						}
						
						str += "<div class='curr_rate_header'>"
							str += sprintf( "<div class='curr_rate_symbol'>%s</div>", symbol );

						str += "</div>"
						str += "<div class='curr_rate'>";
							str += sprintf("<div class='curr_rate_inner'>%s</div>", data[i]["bidPrice"].toFixed(pts) );
							str += sprintf("<div class='curr_rate_inner'>%s</div>", data[i]["askPrice"].toFixed(pts) );
							
						str += "</div>";
						
					str += "</li>";

				}
			}
		}	

		mylist.html(str);
	}

	//-------------
	this.init = function() {
		this.get_allBookTickers();
		$("#a_logout").click( function() {
			bnw.logout();
		});

	}

}


$(document).ready(function(){
	bnw = new Binawatch();
	bnw.init();
});

	