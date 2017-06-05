function getSearchParams(k){
	var p={};
	location.search.replace(/[?&]+([^=&]+)=([^&]*)/gi,function(s,k,v){p[k]=v})
	return k?p[k]:p;
}

var ut4waApp = angular.module('ut4waApp', [])
  .controller('GameInfoController', function($scope, $http) {

	var isDebug = (getSearchParams('isdebug') != null);

	var url = "/gameinfo";
	if(isDebug){
		url = "/js/gameinfo-test.json";
	}
	
	$scope.toMinutesSec = function(seconds){
		return new Date(seconds * 1000).toISOString().substr(14, 5);
	};
	
	getGameInstanceHttpUrl = function(GameInstance){
		return "http://" + window.location.hostname + ":" + (GameInstance.InstancePort + 100);
	}
	
	/**
	* Kicks or ban a player
	* @param GameInstance
	* @param PlayerInfo Player to be kicked or banned
	* @param isBan If true player will be banned else kicked
	* @param isRemove If true will remove existing session kick or ban if existing, else will add kick or ban
	*/
	$scope.kickPlayer = function(GameInstance, PlayerInfo, isBan, isRemove) {
		
		var confirmMsg = '';
		
		if(!isRemove){
			confirmMsg = 'Are you sure you want to' + (isBan?' ban ':' kick ') + PlayerInfo.PlayerName + ' ?'
		} else {
			confirmMsg = 'Are you sure you want to remove sesssion ' + (isBan?'ban':'kick') + ' of ' + PlayerInfo.PlayerName + '?'
		}
		
		// TODO add field for reason for kicking
		if(confirm(confirmMsg)){
			
			var urlSuffix = '/gameinfo?action=kick&playerid='+PlayerNetId;
			urlSuffix = '&message=Reason';
			urlSuffix = '&isban=' + isBan;
			urlSuffix = '&isremove=' + isRemove;
			
			var kickUrl = getGameInstanceHttpUrl(GameInstance) + urlSuffix;
			
			if(isDebug){
				kickUrl = 'http://localhost:8080' + urlSuffix;
			}
			
			console.log(kickUrl);

			$http.get(kickUrl).then(function(response) {
				console.log(response);
			}, function(errorResponse){
				console.log(errorResponse);
			});
		}
	}

	$scope.mutePlayer = function(GameInstance, PlayerInfo) {
		
		// TODO add field for reason for mute
		if(confirm('Are you sure you want to mute ' + PlayerInfo.PlayerName + '?')){
			var muteUrl = getGameInstanceHttpUrl(GameInstance) + '/gameinfo?action=mute&playerid='+PlayerInfo.PlayerID;
		
			console.log(muteUrl);
		
			$http.post(url).then(function(response) {
				console.log(response);
			}, function(errorResponse){
				console.log(errorResponse);
			});
		}
	}
  
	$http.get(url).then(function(response) {
		console.log(response.data);
        
		$scope.DataFromLobby = response.data.data.IsLobbyServer;
		$scope.DataFromDedi = !response.data.data.IsLobbyServer;
		$scope.hasMatches = (response.data.data.NumMatches > 0);
		
		$scope.GameInstances = response.data.data.GameInstances;
		
		for(var i=0; i<$scope.GameInstances.length; i++){
			
			var GameInstance = $scope.GameInstances[i];
			
			var mapFoundInMapList = false;
			
			// try found map from map list
			for(var j = 0; j < GameInstance.MapList.length; j++){
				if(GameInstance.MapList[j].MapAssetName == GameInstance.Map.MapAssetName){
					GameInstance.selectedMap = GameInstance.MapList[j];
					mapFoundInMapList = true;
					break;
				}
			}
			
			if(!mapFoundInMapList){
				GameInstance.selectedMap = GameInstance.Map;
			}
			
			if(response.data.IsLobbyServer){
				var dediHttpGameInfoUrl = getGameInstanceHttpUrl(GameInstance) + '/gameinfo';
				if(isDebug){
					dediHttpGameInfoUrl = 'http://localhost:8080/js/gameinfo-dedi-test.json';
				}
				
				$http.get(dediHttpGameInfoUrl).then(
				function(response2) {
					$scope.GameInstances[0].Players = new Array();
					$scope.GameInstances[0].IsDataFromDedi = true;
					for(var j=0; j< response2.data.data.GameInstances[0].Players.length; j++){
						$scope.GameInstances[0].Players.push(response2.data.data.GameInstances[0].Players[j]);
					}
				}, function(error){
					console.log('Something bad happened');
				});
			}
		}
		
    }, function(response) {
        $scope.content = "Something went wrong";
    });
  });