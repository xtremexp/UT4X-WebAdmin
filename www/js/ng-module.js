var ut4waApp = angular.module('ut4waApp', [])
  .controller('GameInfoController', function($scope, $http) {

	var url = "/gameinfo";
	url = "/js/gameinfo-test.json";
  
	$http.get(url).then(function(response) {
		console.log(response);
        $scope.lobbyInfo = response.data.LobbyInfo;
		$scope.hasMatches = ($scope.lobbyInfo.NumMatches > 0);
    }, function(response) {
        //Second function handles error
        $scope.content = "Something went wrong";
    });
  });

 /*
ut4waApp.directive('GameInstance', function () {
    return {
        restrict : 'E',
        replace : true,
        scope : { 
			instance : '@instance'
		},
        template : '<div>Some instance</div>' 
    }
});*/