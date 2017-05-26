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
  
	$http.get(url).then(function(response) {
		console.debug(response.data);
        
		$scope.IsLobbyServer = response.data.IsLobbyServer;
		$scope.hasMatches = (response.data.ServerInfo.NumMatches > 0);
		
		// lobby does not get full data from instanced servers
		if($scope.IsLobbyServer){
			$scope.ServerInfo = response.data.ServerInfo;
			
			// load full info directly from instanced server and not from parent lobby server
			for(var i=0; i < $scope.ServerInfo.GameInstances.length; i++){
				var urlDedi = "http://" + window.location.hostname + ":" + ($scope.ServerInfo.GameInstances[i].InstancePort + 100) + "/gameinfo";
				if(isDebug){
					urlDedi = "/js/gameinfo-dedi-test.json";
				}
				console.debug(urlDedi);
				
				$http.get(urlDedi).then(function(response2) {
					$scope.ServerInfo.GameInstances[i] = response2.data.ServerInfo.GameInstances[0];
				}, function(response2){
					console.error(response2);
				});
			}
		} 
		// dedicated server or server instanced from parent lobby server 
		else {
			$scope.ServerInfo = response.data.ServerInfo;
		}
		
    }, function(response) {
        //Second function handles error
        $scope.content = "Something went wrong";
    });
  });