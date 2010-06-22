var ajaxManager = $.manageAjax.create('cacheQueue', { 
    queue: false,  
    cacheResponse: false 
});
var somaWS = {};
//	somaWS.serviceQueue = [];
//	somaWS.queueLocked = false;
	somaWS.queueFlag = true;
  	somaWS.serviceMessageURL = "";


// Common function used by all pages that wish to communicate with
// the soma web services. 
//
// Pass:
//
// package    : which web service package contains the action processor for the requested action.
// json-data  : data object (which minimally needs to contain the action property).
// successsCB : this callback is called on success and will be passed the result reponse object that 
//              comes back from the server.
// failureCB  : called when either the server fails or the Ajax call failes.  Any Ajax errors will be 
//              converted into a classic "unexpected error" as the client can't do anything different 
//              between Ajax or server errors (i.e. it's an 'oops! page.').
// 
somaWS.serviceCall = function(thisPackage, json_data, successCB, failureCB) {

 //  soma.DEBUGGING = true;
//	trace('service call '+thisPackage);
//	soma.DEBUGGING = false;

   var url = somaWS.serviceMessageURL+'/'+thisPackage;
   
 
   // Map the package to it's corresponding service URL.
   //
   //switch (thisPackage) {

     //	case "messages":
	//	   url = somaWS.serviceMessageURL;
	//	break;

	//	default:
		//	alert("somaWS: no URL found for package: " + package); // Should never happen!
	//	return;
   //}


   //json_data["valid"] = undefined; // Remove this temporary property...
	var json_text = '';
	
	if (json_data) {
		json_text = JSON.stringify(json_data);
	}

	if (soma.SERVICES) {
	   //trace("Request Object: " + json_text);
		
	//	soma.DEBUGGING = true;
	//	trace(this.serviceQueue);
	//	soma.DEBUGGING = false;
		
		if (somaWS.queueFlag) {
		
						//soma.DEBUGGING = true;
						//trace('here');
						//soma.DEBUGGING = false;
						ajaxManager.add({
							url     : url, 
							data    : json_text,
							type    : "GET",
							dataType: "json",

							//beforeSend: function(xhr) {
								//setting "Connection" to "close" doesn't seem to work on some (modern?) browsers
								//xhr.setRequestHeader("Connection", "close");
								//xhr.setRequestHeader("Keep-Alive", "10");
								//xhr.setRequestHeader("Content-Type", "text/xml");
							//},

							success: function(data, stat) {

								if (data.result) successCB(data);
								else             failureCB(data);

							},
							error: function(xhr, stat, err) {

								var syntheticResponse = {result: false, reasonMap: {error:"unexpectedError"}};
								failureCB(syntheticResponse);

							}
						});
						/*
						if (!this.queueLocked) {
		
								this.queueLocked = true;
								var serviceObj = {};
								
								var xmr = $.ajax({
									url     : url, 
									data    : json_text,
									type    : "POST",
									dataType: "json",
			
									//beforeSend: function(xhr) {
										//setting "Connection" to "close" doesn't seem to work on some (modern?) browsers
										//xhr.setRequestHeader("Connection", "close");
										//xhr.setRequestHeader("Keep-Alive", "10");
										//xhr.setRequestHeader("Content-Type", "text/xml");
									//},
			
									success: function(data, stat) {

										if (data.result) successCB(data);
										else             failureCB(data);

									},
									error: function(xhr, stat, err) {
					
										var syntheticResponse = {result: false, reasonMap: {error:"unexpectedError"}};
										failureCB(syntheticResponse);

									},
									complete: function() {
											somaWS.queueLocked = false;

											if (somaWS.serviceQueue.length > 0) {

												serviceObj = somaWS.serviceQueue.shift();

													soma.DEBUGGING = true;
													trace('success pop '+serviceObj.package);
													trace(serviceObj);
													soma.DEBUGGING = false;

												//somaWS.serviceCall(serviceObj.package, serviceObj.data, serviceObj.successCB, serviceObj.failureCB);

											}
										
									}
		

								});
								
								
								
						} else {
							
							//this.enQueue(thisPackage, json_data, successCB, failureCB);
							soma.DEBUGGING = true;
							trace('queue locked '+thisPackage);
							soma.DEBUGGING = false;

							var pushObj = {};
							pushObj.package = thisPackage;
							pushObj.data = json_data;
							pushObj.successCB = successCB;
							pushObj.failureCB = failureCB;

							somaWS.serviceQueue.push(pushObj);
							
						}
						*/
					
		} else {
			
				var xmr = $.ajax({
					url     : url, 
					data    : json_text,
					type    : "GET",
					dataType: "json",

					//beforeSend: function(xhr) {
						//setting "Connection" to "close" doesn't seem to work on some (modern?) browsers
						//xhr.setRequestHeader("Connection", "close");
						//xhr.setRequestHeader("Keep-Alive", "10");
						//xhr.setRequestHeader("Content-Type", "text/xml");
					//},

					success: function(data, stat) {

						if (data.result) successCB(data);
						else             failureCB(data);

					},
					error: function(xhr, stat, err) {

						var syntheticResponse = {result: false, reasonMap: {error:"unexpectedError"}};
						failureCB(syntheticResponse);

					}


				});
			
		}
 	}
}

/*
somaWS.deQueue = function() {
	
	if (this.serviceQueue.length > 0) {
		
		var serviceObj = this.serviceQueue.shift();
		this.serviceCall(serviceObj.package, serviceObj.data, serviceObj.successCB, serviceObj.failureCB);
		
	}
	
}
somaWS.enQueue = function(thisPackage, json_data, successCB, failureCB) {
	var pushObj = {};
	pushObj.package = thisPackage;
	pushObj.data = json_data;
	pushObj.successCB = successCB;
	pushObj.failureCB = failureCB;
	
	this.serviceQueue.push(pushObj);
}
*/

