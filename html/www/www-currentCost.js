      google.load('visualization', '1.0', {'packages':['corechart']});

      var jsonDataWatts0, jsonDataWatts1;
	  var jsonDataDyn0, jsonDataDyn1, jsonDataDynX;
	  var jsonDataH0, jsonDataH1;
	  var jsonDataD0, jsonDataD1;
	  var jsonDataM0, jsonDataM1;
	  var jsDataDailyLoaded = false;
	  var jsDataActualLoaded = false;
	  var jsDataHourlyLoaded = false;
	  var jsDataMonthlyLoaded = false;

function myfailure() {
alert('loading Error');
};

function lastWattsUpdate() {
$.ajax({
     type: "GET",
     url: '_getCCostDataDyn.php?f=LastWattsUpdate',
     success: function(data) {
          $('#last_watts_updated').html(" "+ data + ".");
     }

   });
};

function loaddWattsData () {
    var page = $(':mobile-pagecontainer').pagecontainer('getActivePage')[0].id;
    if (page == "home") {
$.when(
		$.get("_getCCostDataDyn.php?f=Watts0", function(jsonDataWatts0_) {
			jsonDataWatts0 = jsonDataWatts0_;
		}),
		$.get("_getCCostDataDyn.php?f=Watts1", function(jsonDataWatts1_) {
			jsonDataWatts1 = jsonDataWatts1_;
		}) ).then(
		function() {
		drawChartWatts();
		lastWattsUpdate();
		})
		.fail(function(resp) {
		console.log(resp);
		myfailure();
  });
  } else {
  setTimeout(function() {loaddWattsData();}, 10000);
  }
}

function drawChartWatts() {
	var dataW0 = new google.visualization.DataTable(jsonDataWatts0);
	var chartW0 = new google.visualization.Gauge(document.getElementById('chart_div_watts0'));
	//var chartW0 = new google.visualization.AreaChart(document.getElementById('chart_div_watts0'));
  	var optionsW0 = {
          width: 400, height: 120,
          redFrom: 5000, redTo: 9000,
          yellowFrom:1500, yellowTo: 5000,
		  greenFrom:  	0, 	greenTo:	1500,
          minorTicks: 100,
          max: 9000
    };
    chartW0.draw(dataW0, optionsW0);

	var dataW1 = new google.visualization.DataTable(jsonDataWatts1);
	var chartW1 = new google.visualization.Gauge(document.getElementById('chart_div_watts1'));
	//var chartW0 = new google.visualization.AreaChart(document.getElementById('chart_div_watts0'));

  	var optionsW1 = {
          width: 400, height: 120,
          redFrom: 2000, redTo: 5000,
          yellowFrom:500, yellowTo: 2000,
		  greenFrom:  	0, 	greenTo:	500,
          minorTicks: 500,
          max: 5000
    };

	chartW1.draw(dataW1, optionsW1);
	google.visualization.events.addListener(chartW1, 'ready', setTimeout(function() {
          loaddWattsData();
        }, 10000));
}

var data_CC_Dyn0, data_CC_Dyn1, data_CC_DynX;
var chartCC_Dyn0, chartCC_Dyn1, chartCC_DynX;

// Actual Charts
function loadDataA () {
$.when(
		$.get("_getCCostData.php?f=value&u=RAW&c=0", function(jsonDataDyn0_) {
			jsonDataDyn0 = jsonDataDyn0_;
		}),
		$.get("_getCCostData.php?f=value&u=RAW&c=1", function(jsonDataDyn1_) {
			jsonDataDyn1 = jsonDataDyn1_;
		}),
		$.get("_getCCostData.php?f=value&u=RAW&c=0-1", function(jsonDataDynX_) {
			jsonDataDynX = jsonDataDynX_;
		}) ).then(
		function() {
		jsDataActualLoaded = true;
		drawChartA();
});
}

function drawChartA() {

	var optionsCC_Dyn0 = {
          title: 'Compteur Electrique - Conso Dynamique (Watts) [Total]',
		  legend: 'in',
		  chartArea: {left:'60', top:'30', width:'95%'}
        };

	var optionsCC_Dyn1 = {
          title: 'Compteur Electrique - Conso Dynamique (Watts) [HVAC]',
		  legend: 'in',
		  chartArea: {left:'60', top:'30', width:'95%'}
        };

  	var optionsCC_DynX = {
          title: 'Compteur Electrique - Conso Dynamique (Watts) [TOTAL - HVAC]',
		  legend: 'in',
		  chartArea: {left:'60', top:'30', width:'95%'}
        };
	waitdisplay();
	document.getElementById('outer_chart').style.display = 'block';
	data_CC_Dyn0 = new google.visualization.DataTable(jsonDataDyn0);
	chartCC_Dyn0 = new google.visualization.AreaChart(document.getElementById('chart_div_CC_Dyn0'));
	data_CC_Dyn1 = new google.visualization.DataTable(jsonDataDyn1);
	chartCC_Dyn1 = new google.visualization.AreaChart(document.getElementById('chart_div_CC_Dyn1'));
	data_CC_DynX = new google.visualization.DataTable(jsonDataDynX);
	chartCC_DynX = new google.visualization.AreaChart(document.getElementById('chart_div_CC_DynX'));
    chartCC_Dyn0.draw(data_CC_Dyn0, optionsCC_Dyn0);
    chartCC_Dyn1.draw(data_CC_Dyn1, optionsCC_Dyn1);
	chartCC_DynX.draw(data_CC_DynX, optionsCC_DynX);
	google.visualization.events.addListener(chartCC_DynX, 'ready', stoploading());
}

// Hourly Charts
function loadDataH () {
$.when(
		$.get("_getCCostData.php?f=value&u=HOURLY&c=0", function(jsonDataH0_) {
			jsonDataH0 = jsonDataH0_;
		}),
		$.get("_getCCostData.php?f=value&u=HOURLY&c=1", function(jsonDataH1_) {
			jsonDataH1 = jsonDataH1_;
		})
		).then(
		function() {
		drawChartH();
});
}

function drawChartH() {

    var dataH0 = new google.visualization.DataTable(jsonDataH0);
    var chartH0 = new google.visualization.ColumnChart(document.getElementById('chart_div_H0'));
	var optionsH0 = {
          title: 'Compteur Electrique - Conso 2Heures (kWatts) [Total]',
		  legend: 'in',
		  chartArea: {left:'60', top:'30', width:'95%'}
        };

	var dataH1 = new google.visualization.DataTable(jsonDataH1);
    var chartH1 = new google.visualization.ColumnChart(document.getElementById('chart_div_H1'));
	var optionsH1 = {
          title: 'Compteur Electrique - Conso Journalière (kWatts) [HVAC]',
		  legend: 'in',
		  chartArea: {left:'60', top:'30', width:'95%'}
        };
	document.getElementById('outer_chart').style.display = 'block';
	waitdisplay();
	chartH0.draw(dataH0, optionsH0);
	chartH1.draw(dataH1, optionsH1);
	google.visualization.events.addListener(chartH1, 'ready', stoploading());
} // end of drawChartH()

// Monthly Charts
function loadDataM () {
$.when(
		$.get("_getCCostData.php?f=value&u=MONTHLY&c=0", function(jsonDataM0_) {
			jsonDataM0 = jsonDataM0_;
		}),
		$.get("_getCCostData.php?f=value&u=MONTHLY&c=1", function(jsonDataM1_) {
			jsonDataM1 = jsonDataM1_;
		})
		).then(
		function() {
		drawChartM();
});
}

function drawChartM() {

    var dataM0 = new google.visualization.DataTable(jsonDataM0);
    var chartM0 = new google.visualization.ColumnChart(document.getElementById('chart_div_M0'));
	var optionsM0 = {
          title: 'Compteur Electrique - Conso Mensuelle (kWatts) [Total]',
		  legend: 'in',
		  chartArea: {left:'60', top:'30', width:'95%'}
        };

    var dataM1 = new google.visualization.DataTable(jsonDataM1);
    var chartM1 = new google.visualization.ColumnChart(document.getElementById('chart_div_M1'));
	var optionsM1 = {
          title: 'Compteur Electrique - Conso Mensuelle (kWatts) [HVAC]',
		  legend: 'in',
		  chartArea: {left:'60', top:'30', width:'95%'}
        };
	document.getElementById('outer_chart').style.display = 'block';
	waitdisplay();
	chartM0.draw(dataM0, optionsM1);
	chartM1.draw(dataM1, optionsM0);
	google.visualization.events.addListener(chartM1, 'ready', stoploading());
} // end of drawChartM()

// Daily Charts
function loadDataD () {
$.when(
		$.get("_getCCostData.php?f=value&u=DAILY&c=0", function(jsonDataD0_) {
			jsonDataD0 = jsonDataD0_;
		}),
		$.get("_getCCostData.php?f=value&u=DAILY&c=1", function(jsonDataD1_) {
			jsonDataD1 = jsonDataD1_;
		})
		).then(
		function() {
		drawChartD();
});
}

function drawChartD() {
    var dataD0 = new google.visualization.DataTable(jsonDataD0);
    var chartD0 = new google.visualization.ColumnChart(document.getElementById('chart_div_D0'));
	var optionsD0 = {
          title: 'Compteur Electrique - Conso Journalière (kWatts) [Total]',
		  legend: 'in',
		  chartArea: {left:'60', top:'30', width:'95%'}
        };

    var dataD1 = new google.visualization.DataTable(jsonDataD1);
    var chartD1 = new google.visualization.ColumnChart(document.getElementById('chart_div_D1'));
	var optionsD1 = {
          title: 'Compteur Electrique - Conso Journalière (kWatts) [HVAC]',
		  legend: 'in',
		  chartArea: {left:'60', top:'30', width:'95%'}
        };
	document.getElementById('outer_chart').style.display = 'block';
	chartD0.draw(dataD0, optionsD0);
	chartD1.draw(dataD1, optionsD1);
	google.visualization.events.addListener(chartD1, 'ready', stoploading());
} // end of drawChartD()

$(document).ready(function(){
    //loaddatagraph();
	//drawChart();
});

$(document).ready(function(){
  $("#ref_butn").click(function(){
 location.reload();
  });
});

function waitloading() {
$.mobile.loading( "show", {
                text: "loading...",
                textVisible: true,
                theme: 'b',
                textonly: false,
                html: ""
        });
};

function waitdisplay() {
$.mobile.loading( "show", {
                text: "Charts computing...",
                textVisible: true,
                theme: 'b',
                textonly: false,
                html: ""
        });
};

function stoploading() {
  $.mobile.loading( "hide" );
};


$(document).on("pageshow","#home",function(){
	loaddWattsData();
});

$(document).on("pageshow","#actual",function(){
  if (!jsDataActualLoaded) {
	waitloading();
	loadDataA();
  }
});

$(document).on("pageshow","#hourly",function(){
  waitloading();
  loadDataH();
});


$(document).on("pageshow","#daily",function(){
  waitloading();
  loadDataD();
});

$(document).on("pageshow","#monthly",function(){
  waitloading();
  loadDataM();
 });

function updateDisplayCharts() {
    var page = $(':mobile-pagecontainer').pagecontainer('getActivePage')[0].id;
    if (page == "monthly") {
		waitdisplay();
		document.getElementById('outer_chart').style.display = 'none';
		setTimeout(function() {
          drawChartM()();
        }, 500);
	};
	if (page == "daily") {
		waitdisplay();
		document.getElementById('outer_chart').style.display = 'none';
		setTimeout(function() {
          drawChartD()();
        }, 500);
	};
	if (page == "hourly") {
		waitdisplay();
		document.getElementById('outer_chart').style.display = 'none';
		setTimeout(function() {
          drawChartH()();
        }, 500);
	};
	if (page == "actual") {
		waitdisplay();
		document.getElementById('outer_chart').style.display = 'none';
		setTimeout(function() {
          drawChartA()();
        }, 500);
	};
}
 $( window ).resize(function() {
  updateDisplayCharts();
});

$(window).on("orientationchange",function() {
  updateDisplayCharts();
});

$(document).on("pagecontainerloadfailed",function(event,data){
  alert("Sorry, requested page does not exist.");
});
