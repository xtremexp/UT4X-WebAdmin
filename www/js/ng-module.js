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
	
	$scope.getGameInstanceHttpUrl = function(GameInstance){
		return "http://" + window.location.hostname + ":" + (GameInstance.InstancePort + 100);
	}
	
	/**
	* Kicks or ban a player
	* @param GameInstance
	* @param PlayerNetId UniqueNetId of player
	* @param isBan If true player will be banned else kicked
	* @param isRemove If true will remove existing session kick or ban if existing, else will add kick or ban
	*/
	$scope.kickPlayer = function(GameInstance, PlayerNetId, isBan, isRemove) {
		
		var confirmMsg = '';
		
		if(!isRemove){
			confirmMsg = 'Are you sure you want to ' + (isBan?'ban':'kick') + PlayerInfo.PlayerName + '?'
		} else {
			confirmMsg = 'Are you sure you want to remove sesssion ' + (isBan?'ban':'kick') + ' of ' + PlayerInfo.PlayerName + '?'
		}
		
		// TODO add field for reason for kicking
		if(confirm(confirmMsg)){
			
			var urlSuffix = '/gameinfo?action=kick&playerid='+PlayerNetId;
			urlSuffix = '&message=Reason';
			urlSuffix = '&isban=' + isBan;
			urlSuffix = '&isremove=' + isRemove;
			
			var kickUrl = $scope.getGameInstanceHttpUrl(GameInstance) + urlSuffix;
			
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
			var muteUrl = $scope.getGameInstanceHttpUrl(GameInstance) + '/gameinfo?action=mute&playerid='+PlayerInfo.PlayerID;
		
			console.log(muteUrl);
		
			$http.post(url).then(function(response) {
				console.log(response);
			}, function(errorResponse){
				console.log(errorResponse);
			});
		}
	}
  
	$http.get(url).then(function(response) {
		console.debug(response.data);
        
		$scope.IsLobbyServer = response.data.IsLobbyServer;
		$scope.hasMatches = (response.data.ServerInfo.NumMatches > 0);
		
		$scope.GameInstancesLightData = new Array();
		$scope.GameInstancesFullData = new Array();
		
		// lobby does not get full data from instanced servers
		if($scope.IsLobbyServer){
			//$scope.GameInstancesLightData = response.data.ServerInfo.GameInstances;
			// TODO load extra data directly from instanced server rather than lobby
		} 
		// dedicated server or server instanced from parent lobby server 
		else {
			$scope.GameInstancesFullData.push(response.data.ServerInfo.GameInstances[0]);
		}
		
    }, function(response) {
        $scope.content = "Something went wrong";
    });
  });