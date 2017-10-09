

function SimpleList() {

	//---------------
	this.get_allBookTickers = function() {

		var sl = this;
		this.loadJSON("/allBookTickers.json", function( obj ) {
			
			sl.allBookTickers = obj;
            sl.render_list();
            	    	
	    }, function(xhr) {
	    	console.log("Error!");
	    });
	}



	//--------------------------
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


	//-------------
	this.get_querystring_value = function( name ) {

		var url = window.location.href;
		name = name.replace(/[\[\]]/g, "\\$&");
		var regex = new RegExp("[?&]" + name + "(=([^&#]*)|&|#|$)");
		results = regex.exec(url);
		if (!results) { 
			return null;
		}
		if (!results[2]) { 
			return '';
		}
		return decodeURIComponent(results[2].replace(/\+/g, " "));
	}


	//--------
	this.sprintf = function( ) {
		
		var str = arguments[0];
		for ( var i = 1 ; i < arguments.length ; i++ ) {
			str = str.replace("%s", arguments[i]);
		}
		return str;
	}


	//-----
	this.render_list = function() {
			
		var mylist = document.getElementById("threadindex_list");
		
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
							str += this.sprintf( "<div class='curr_rate_symbol'>%s</div>", symbol );

						str += "</div>"
						str += "<div class='curr_rate'>";
							str += this.sprintf("<div class='curr_rate_inner'>%s</div>", data[i]["bidPrice"].toFixed(pts) );
							str += this.sprintf("<div class='curr_rate_inner'>%s</div>", data[i]["askPrice"].toFixed(pts) );
							
						str += "</div>";
						
					str += "</li>";

				}
			}
		}	
		mylist.innerHTML = str;
	}


	//--------------
	this.numberWithCommas = function(x) {
    	var parts = x.toString().split(".");
    	parts[0] = parts[0].replace(/\B(?=(\d{3})+(?!\d))/g, ",");
    	return parts.join(".");
	}

	//-------------
	this.init = function() {
		this.get_allBookTickers();

	}

}

document.addEventListener("DOMContentLoaded", function() {
	sl = new SimpleList();
	sl.init();
});

