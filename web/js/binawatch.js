

function Binawatch() {

	//---------------
	this.get_allBookTickers = function() {

		$.getJSON("/allBookTickers.json", function( response ) {
			
			if ( response["statuscode"] == 0 ) {
				bnw.allBookTickers = response["data"];
            	bnw.render_market_list();
            	//bnw.get_account();

        	} else {
        		console.log( response );
        	}
            	    	
	    });
	}


	//------------
	this.get_account = function() {

		$.getJSON("/account.json", function( response ) {
			if ( response["statuscode"] == 0 ) {
				
				bnw.account_inventory = response.data.balances;
				bnw.render_inventory_list();

			} else if ( response["statuscode"] == -2014 ) {

				$("#div_warning_header").html("Need to <a href='/apikey.html'>setup API key</a> in order to allow trading and other Binance Account operations.");
				$("#div_warning_header").show();

            } else {
            	$.toaster({ 
						priority : 'danger', 
						message :"Error to get account. " + response["statusmsg"]  
				});
            }	    			
	    });	
	}


	//---------------
	this.render_inventory_list = function() {

		if ( typeof this.account_inventory != 'undefined' ) {
			var data = this.account_inventory;

			for ( var i = 0 ; i < data.length ; i++ ) {

				var symbol 		= data[i]["asset"];
				var free_amt 	= parseFloat( data[i]["free"] );
				var locked_amt 	= parseFloat( data[i]["locked"] );

				var total_amt   = free_amt + locked_amt;
				$("#o" + symbol ).html( total_amt.toFixed(2) );
			}
		}	
	}
	

	//-----
	this.render_market_list = function() {
			
		var mylist = $("#market_list");
			
		if ( typeof this.allBookTickers != 'undefined' ) {

			var data = this.allBookTickers;

			var quote_curr_registered = {};
			
			for ( var i = 0 ; i < data.length ; i++ ) {

				var symbol 		= data[i]["s"];
				if ( symbol == "ETHBTC" || symbol == "BTCUSDT" || symbol == "ETHUSDT" ) {
					
					reciprocal_symbol 	= symbol.slice(3) + symbol.slice(0,3);
					reciprocal_ask 		= 1 / parseFloat( data[i]["a"] );
					reciprocal_bid 		= 1 / parseFloat( data[i]["b"] );
					data.push({
						s : reciprocal_symbol, 
						a : reciprocal_ask.toFixed(8), 
						b : reciprocal_bid.toFixed(8)  
					});
				}
			}		



			for ( var i = 0 ; i < data.length ; i++ ) {

				if (  parseFloat( data[i]["b"] ) > 0.0 ) {

					var symbol 		= data[i]["s"];
					var quote_curr  = symbol.slice(0,-3);
					var base_curr   = symbol.slice(-3);
					
					if ( symbol == "BTCUSDT" || symbol == "ETHUSDT" ) {
						quote_curr = symbol.slice(0,-4);
						base_curr = symbol.slice(-4);
					}

					if ( typeof quote_curr_registered[quote_curr] == 'undefined' ) {
						quote_curr_registered[quote_curr] = {}
						quote_curr_registered[quote_curr]["ask"] = [];
						quote_curr_registered[quote_curr]["bid"] = [];
					}
					quote_curr_registered[quote_curr]["ask"].push( { curr : base_curr, v: data[i]["a"] } );
					quote_curr_registered[quote_curr]["bid"].push( { curr : base_curr, v: data[i]["b"] } );
				}
			}

			var str = ""
			for ( var quote_curr in quote_curr_registered ) {
				str += "<li>";

					str += "<div class='curr_rate_header'>"
						str += sprintf( "<div class='curr_rate_symbol'>%s</div>", quote_curr );

					str += "</div>"
					str +=  sprintf("<div class='curr_rate_row' id='curr_rate_row%s'>", quote_curr);

						str += "<div class='curr_rate_col curr_rate_col_b'>";
						for ( i = 0 ; i < quote_curr_registered[quote_curr]["bid"].length ; i++ ) {
							var obj 		= quote_curr_registered[quote_curr]["bid"][i];
							var base_curr 	= obj.curr; 
							var val  		= obj.v;
							str += sprintf("<div class='curr_rate_inner'>Bid: <div class='curr_rate_val' id='b%s%s'>%s</div> %s</div>", quote_curr, base_curr, val, base_curr );
						}
						str += "</div>";

						str += "<div class='curr_rate_col curr_rate_col_a'>";
						for ( i = 0 ; i < quote_curr_registered[quote_curr]["ask"].length ; i++ ) {
							var obj 		= quote_curr_registered[quote_curr]["ask"][i];
							var base_curr 	= obj.curr; 
							var val  		= obj.v;
							str += sprintf("<div class='curr_rate_inner'>Ask: <div class='curr_rate_val' id='b%s%s'>%s</div> %s</div>", quote_curr, base_curr, val, base_curr );
						}
						str += "</div>";

						str += "<div class='curr_rate_col curr_rate_col_o'>";
							str += sprintf("<div class='curr_rate_inner'>Own: <div class='curr_rate_val' id='o%s'></div>0.00 %s</div>", quote_curr, quote_curr );
						str += "</div>"
						
						

						

					str += "</div>";
					
				str += "</li>";
			}
			mylist.html(str);
		}	

		
	}

	//-------------
	this.init = function() {
		this.get_allBookTickers();
		$("#a_logout").click( function() {
			logout();
		});

	}

}


$(document).ready(function(){
	bnw = new Binawatch();
	bnw.init();
});

	