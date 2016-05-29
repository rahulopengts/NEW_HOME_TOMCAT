  //var ref = new Firebase('https://project-8241275639547446807.firebaseio.com/HOME_2/');
// define our app and dependencies (remember to include firebase!)
var app = angular.module("sampleApp", ["firebase"]);
// inject $firebaseObject into our controller
app.controller("SampleCtrl", ["$scope", "$firebaseObject",
  function($scope, $firebaseObject) {
    var ref = new Firebase('https://project-8241275639547446807.firebaseio.com/');
    // download physicsmarie's profile data into a local object
    // all server changes are applied in realtime
    $scope.profile = $firebaseObject(ref.child('HOME_2').child('deviceTypeMap').child('dev2').child('deviceType'));
  }
]);