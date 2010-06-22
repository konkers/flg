var soma;
soma = soma || {};
soma.page = soma.page || {};
soma.page.player = {};

soma.page.player.PATTERNS = [];
soma.page.player.CHANNEL_DATA = [];
soma.page.player.NUM_CHANNELS = 0;
 
soma.page.player.init = function() {
	
	soma.DEBUGGING = true;
	
	soma.page.player.getNumberOfChannels();
	soma.page.player.getPatterns();
	soma.page.player.getRate();
	
}

soma.page.player.getNumberOfChannels = function() {
	somaWS.serviceCall("channel", null, soma.page.player.getNumberOfChannelsCB, soma.page.player.getNumberOfChannelsCB);
}

soma.page.player.getNumberOfChannelsCB = function(data) {
	
	//trace(data);
	
	if (data.result) {
		//success case
		
		$('.init-channels').html(data.channels);
		
		soma.page.player.getInitialChannelStates(data.channels);
		
		soma.page.player.NUM_CHANNELS = data.channels;
		
	} else {
		//failure case
		
		
	}
	
}

soma.page.player.getInitialChannelStates = function(numChannels) {
	
	soma.page.player.COUNTED_STATES = 0;
	
	for (var i=0; i<numChannels; i++) {
		soma.page.player.getChannelState(i);
	}
	
}

soma.page.player.getChannelState = function(thisChannel) {
	
	var thisAction = 'channel/'+thisChannel+'/state';
	
	somaWS.serviceCall(thisAction, null, function(data) { soma.page.player.getChannelStateCB(thisChannel, data) }, function(data) { soma.page.player.getChannelStateCB(thisChannel, data) });	
}

soma.page.player.getChannelStateCB = function(thisChannel, data) {

	soma.page.player.CHANNEL_DATA[thisChannel] = soma.page.player.CHANNEL_DATA[thisChannel] || {};
	soma.page.player.CHANNEL_DATA[thisChannel].state = data.state;
	
	soma.page.player.getChannelPattern(thisChannel);

}

soma.page.player.getChannelPattern = function(thisChannel) {

	var thisAction = 'channel/'+thisChannel+'/pattern';
	
	somaWS.serviceCall(thisAction, null, function(data) { soma.page.player.getChannelPatternCB(thisChannel, data) }, function(data) { soma.page.player.getChannelPatternCB(thisChannel, data) });	

}

soma.page.player.getChannelPatternCB = function(thisChannel, data) {

	soma.page.player.CHANNEL_DATA[thisChannel] = soma.page.player.CHANNEL_DATA[thisChannel] || {};
	soma.page.player.CHANNEL_DATA[thisChannel].pattern = data.pattern;
	
	soma.page.player.COUNTED_STATES++;
	
	if (soma.page.player.COUNTED_STATES == soma.page.player.NUM_CHANNELS) {
		soma.page.player.displayChannelData();
	}
	

}

soma.page.player.displayChannelData = function() {
	
	$('.init-channel-data').html(JSON.stringify(soma.page.player.CHANNEL_DATA));
	
	
	for (var i = 0; i < soma.page.player.NUM_CHANNELS; i++) {
		$('#channel-table').append(soma.page.player.getChannelMarkup(i));
	}
	
	for (var i = 0; i < soma.page.player.NUM_CHANNELS; i++) {
		soma.page.player.initChannelRow($('#row_'+i));
	}
	
	
}

soma.page.player.getChannelMarkup = function (thisChannel) {
	
	var thisState = soma.page.player.CHANNEL_DATA[thisChannel]['state'];
	var thisPattern = soma.page.player.CHANNEL_DATA[thisChannel]['pattern'];
	
	var thisStateChecked = [];
	thisStateChecked['off'] = '';
	thisStateChecked['single'] = '';
	thisStateChecked['loop'] = '';
	
	thisStateChecked[thisState] = ' checked="checked"';
			
	var markup = '';
	
	markup += '<tr id="row_'+thisChannel+'"><td> channel '+thisChannel+'</td><td>';
	markup += '<input type="radio" name="state_'+thisChannel+'" id="state_'+thisChannel+'_off" value="off"'+thisStateChecked['off']+'/><label for="state_'+thisChannel+'_off" id="state_'+thisChannel+'_off-label">off</label><input type="radio" name="state_'+thisChannel+'" id="state_'+thisChannel+'_single" value="single"'+thisStateChecked['single']+'/><label for="state_'+thisChannel+'_single" id="state_'+thisChannel+'_single-label">single</label><input type="radio" name="state_'+thisChannel+'" id="state_'+thisChannel+'_loop" value="loop"'+thisStateChecked['loop']+'/><label for="state_'+thisChannel+'_loop" id="state_'+thisChannel+'_loop-label">loop</label></td><td>';
	
	
	markup += '<select id="pattern_'+thisChannel+'"><option value="">-            -</option>';
	
	for (var i=0; i < soma.page.player.PATTERNS.length; i++) {
		
		var thisSelected = '';
		if (soma.page.player.PATTERNS[i] == thisPattern) {
			thisSelected = ' SELECTED';
		}
		
		markup += '<option value="'+soma.page.player.PATTERNS[i]+'"'+thisSelected+'>'+soma.page.player.PATTERNS[i]+'</option>';
	}
	
	markup += '</select></td></tr>';
	
	return markup;
 	
}

soma.page.player.initChannelRow = function(thisRow) {
	
	thisRow.find('input[type=radio]').bind('change', function(event) {
		
		soma.page.player.changeState($(this).attr('name').substr(6), $(this).val());
		
	});
	
	thisRow.find('select').bind('change', function(event) {
		
		soma.page.player.changePattern($(this).attr('id').substr(8), $(this).val());
		
	});
	
}

soma.page.player.changeState = function(thisChannel, thisState) {
		
	var thisAction = 'channel/'+thisChannel+'/state/'+thisState;
	
	somaWS.serviceCall(thisAction, null, function(data) { soma.page.player.changeStateCB(thisChannel, data) }, function(data) { soma.page.player.changeStateCB(thisChannel, data) });	
	
	
}

soma.page.player.changeStateCB = function(thisChannel, data) {

	if (data.result) {
		//success case
		
		$('.status').fadeOut('fast', function() {
			$(this).html('changed state of channel '+thisChannel+' to '+data.state).fadeIn();
		})
		
	} else {
		//failure case
		
		
	}

}

soma.page.player.changePattern = function(thisChannel, thisPattern) {
		
	var thisAction = 'channel/'+thisChannel+'/pattern/'+thisPattern;
	
	somaWS.serviceCall(thisAction, null, function(data) { soma.page.player.changePatternCB(thisChannel, data) }, function(data) { soma.page.player.changePatternCB(thisChannel, data) });	
	
	
}

soma.page.player.changePatternCB = function(thisChannel, data) {

	if (data.result) {
		//success case
		
		$('.status').fadeOut('fast', function() {
			$(this).html('changed pattern of channel '+thisChannel+' to '+data.pattern).fadeIn();
		})
		
	} else {
		//failure case
		
		
	}

}

soma.page.player.getPatterns = function() {
	somaWS.serviceCall("pattern", null, soma.page.player.getPatternsCB, soma.page.player.getPatternsCB);	
}

soma.page.player.getPatternsCB = function(data) {

	if (data.result) {
		//success case

		$('.init-patterns').html(data.patterns);		
		
		soma.page.player.PATTERNS = data.patterns.split('\n');
		
		//trace(soma.page.player.PATTERNS);
		
	} else {
		//failure case
		
		
	}
	
}

soma.page.player.getRate = function() {
	somaWS.serviceCall("rate", null, soma.page.player.getRateCB, soma.page.player.getRateCB);	
}

soma.page.player.getRateCB = function(data) {

	if (data.result) {
		//success case
		
		$('.init-rate').html(Math.floor(parseInt(data.rate) / 100));		
		
		
	} else {
		//failure case
		
		
	}
	
}



$(document).ready(function() {
	soma.page.player.init();
});
