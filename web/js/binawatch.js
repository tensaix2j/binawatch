

function SimpleList() {

	//---------------
	this.get_allBookTickers = function() {

		var sl = this;
		loadJSON("/allBookTickers.json", function( obj ) {
			
			sl.allBookTickers = obj;
            sl.render_list();
            	    	
	    }, function(xhr) {
	    	console.log("Error!");
	    });
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
		mylist.innerHTML = str;
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

