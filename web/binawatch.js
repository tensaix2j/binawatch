

function SimpleList() {

	//---------------
	this.get_exchange_rates = function() {

		var sl = this;
		this.loadJSON("/binawatch_tickers_json", function( obj ) {
			sl.exchange_rates = obj;
            sl.render_list();
            var profile = sl.get_querystring_value("profile") ;
            if ( profile ) {
				sl.get_holdings(profile);
			}	    	
	    }, function(xhr) {
	    	console.log("Error!");
	    });
	}

	//-----------
	// Get user's holding from profile
	this.get_holdings = function( profile_name ) {
			
		var sl = this;
		this.profile_name = profile_name;
		this.loadJSON("/" + profile_name + ".json", function( obj ) {
			sl.profile = obj;
            sl.render_profile();
            	    	
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
		this.coin_usdval = {};
		this.coin_sgdval = {};

		var str = ""
		if ( typeof this.exchange_rates != 'undefined' ) {
			var data = this.exchange_rates;

			for ( var i = 0 ; i < data.length ; i++ ) {

				if (  data[i]["bidPrice"] > 0.0 ) {

					str += "<li>";

						var symbol 		= data[i]["symbol"];
						var pair   		= this.sprintf("%sBTC", symbol );
						
						var chart_url = this.sprintf("https://www.binance.com/tradeDetail.html?symbol=%s_%s", symbol.slice(0,-3) , symbol.slice(-3)  );	
						
						str += "<div class='curr_rate_header'>"
							str += this.sprintf( "<div class='curr_rate_symbol'><a href='%s' target='_blank'>%s</a></div>", chart_url, symbol );

						str += "</div>"
						str += "<div class='curr_rate'>";
							str += this.sprintf("<div class='curr_rate_inner'>%s BTC</div>", data[i]["bidPrice"].toFixed(8) );
							str += this.sprintf("<div class='curr_rate_inner'>%s BTC</div>", data[i]["askPrice"].toFixed(8) );
							
						str += "</div>";
						
					str += "</li>";

				}
			}
		}	
		mylist.innerHTML = str;
	}

	//------------
	this.render_profile = function() {
		
		if ( typeof this.profile != 'undefined' ) {
			
			this.total_usd = 0.0;
			this.total_sgd = 0.0;

			for ( var key in this.profile ) {
				
				var symbol 		= key;
				var own 		= this.profile[key];

				var dom = document.getElementById("curr_rate_holding_symbol_"+ symbol ) ;
				if ( dom ) {
					dom.innerHTML = "You Own : " + own + " " + symbol;
				
					var own_usd 	= own * this.coin_usdval[symbol];
					this.total_usd += own_usd;
					var dom_usd 	= document.getElementById("curr_rate_holding_usd_" + symbol );
					if ( dom_usd ) {
						dom_usd.innerHTML = this.numberWithCommas( own_usd.toFixed(2)  ) + " USD";
					} 

					var own_sgd 	= own * this.coin_sgdval[symbol];
					this.total_sgd += own_sgd;
					var dom_sgd 	= document.getElementById("curr_rate_holding_sgd_" + symbol );
					if ( dom_sgd ) {
						dom_sgd.innerHTML = this.numberWithCommas( own_sgd.toFixed(2) ) + " SGD";
					} 

				}		
			}

			var str = "<div>";
				str += this.sprintf( "<div class='profile_header'>%s</div>", this.profile_name );
				str += this.sprintf( "<div class='curr_rate_holding'>Total USD: %s </div>", this.numberWithCommas( this.total_usd.toFixed(2) ));
				str += this.sprintf( "<div class='curr_rate_holding'>Total SGD: %s </div>", this.numberWithCommas( this.total_sgd.toFixed(2) ));
			str += "</div>";

			document.getElementById("li_profile").innerHTML = str;
		}
	}

	//--------------
	this.numberWithCommas = function(x) {
    	var parts = x.toString().split(".");
    	parts[0] = parts[0].replace(/\B(?=(\d{3})+(?!\d))/g, ",");
    	return parts.join(".");
	}

	//-------------
	this.init = function() {
		this.get_exchange_rates();

	}

}

document.addEventListener("DOMContentLoaded", function() {
	sl = new SimpleList();
	sl.init();
});

