<?php ob_start('ob_gzhandler'); ?>
<!DOCTYPE html>
<html>
<head>

  <!-- (c) 2015 Vincent(at)Cruvellier(dot)eu -->
  <!-- Include meta tag to ensure proper rendering and touch zooming -->
  <meta charset="utf-8">
  <title>DIY - Suivi de Consommation Electrique</title>

  <meta name="viewport" content="width=device-width, initial-scale=1">
  <!-- Include jQuery Mobile stylesheets -->
  <link rel="stylesheet" href="http://code.jquery.com/mobile/1.4.5/jquery.mobile-1.4.5.min.css">
  <!-- Include the jQuery library -->
  <script src="http://code.jquery.com/jquery-1.11.1.min.js"></script>
  <!--Load the AJAX API-->
  <script type="text/javascript" src="https://www.google.com/jsapi"></script>
  <!-- Include the jQuery Mobile library -->
  <script src="http://code.jquery.com/mobile/1.4.5/jquery.mobile-1.4.5.min.js"></script>
  <!-- Include Google Charts library -->
  <script type="text/javascript" src="https://www.google.com/jsapi?autoload={'modules':[{'name':'visualization','version':'1','packages':['gauge']}]}"></script>
  <!-- Include Home Made JavaScript library -->
  <script type="text/javascript" src="www-currentCost.js"></script>
  <!-- Include Home Made CSS library -->
  <link rel="stylesheet" href="www-currentCost.css">
</head>
<body>
<!-- Start Home: #one -->
<div data-role="page" id="home">
  <div data-role="header">
    <a href="#home" class="ui-btn ui-mini ui-corner-all ui-shadow ui-icon-home ui-btn-icon-left ui-btn-icon-notext" data-transition="pop">Home</a>
	<a href="javascript:loaddWattsData();" id="refreshHomeButton" class="ui-btn ui-mini ui-corner-all ui-shadow ui-icon-refresh ui-btn-icon-left ui-btn-icon-notext">Refresh</a>
    <h1> Cons'Elec</h1>
  </div>

  <div data-role="main" class="ui-content">
   <div class="outer_center">
   <center><h2>DIY - Home Energy Monitor</H2></center>
</div>
   <div class="outer-div">
   <div id="current_watts_period"></div>
   <div id="chart_div_watts0" class="inner-div" style="align: center;" ></div>
   <div id="chart_div_watts1" class="inner-div" style="align: center;"></div>
   <div class="last_watts_updated"> WATTS </div>
   <div id="last_watts_updated" class="last_watts_updated"></div>
   </div>
  </div>
  <div data-role="footer" style="text-align:center;">
  <a href="#actual" data-direction="reverse" data-role="button" data-theme="b" data-transition="pop">Actual</a>
  <a href="#hourly" data-direction="reverse" data-role="button" data-theme="b" data-transition="pop">2Hourly</a>
  <a href="#daily" data-direction="reverse" data-role="button" data-theme="b" data-transition="pop">Daily</a>
  <a href="#monthly" data-direction="reverse" data-role="button" data-theme="b" data-transition="pop">Monthly</a>
  <!-- <br><button id="ref_butn">Refresh</button> -->
  </div>
</div>

<div data-role="page" id="dialogPage">
  <div data-role="header">
    <h2>Network Error</h2>
  </div>
  <div role="main" class="ui-content">
    <p>Data Connection Failed.</p>
  </div>
</div>


<!-- Start of second page: #actual -->
<div data-role="page" id="actual">
  <div data-role="header">
    <a href="#home" class="ui-btn ui-mini ui-corner-all ui-shadow ui-icon-home ui-btn-icon-left ui-btn-icon-notext" data-transition="pop">Home</a>
	<a href="javascript:waitloading();loadDataA();" id="refreshHomeButton" class="ui-btn ui-mini ui-corner-all ui-shadow ui-icon-refresh ui-btn-icon-left ui-btn-icon-notext">Refresh</a>
    <h1> Cons'Elec</h1>
  </div>

  <div data-role="main" class="ui-content">
   <div id ="outer_chart" class="outer-div">
     <div id="chart_div_CC_Dyn0"></div>
     <div id="chart_div_CC_Dyn1"></div>
     <div id="chart_div_CC_DynX"></div>
   </div>
  </div>
  <div data-role="footer" style="text-align:center;">
  <a href="#hourly" data-direction="reverse" data-role="button" data-theme="b" data-transition="pop">2Hourly</a>
  <a href="#daily" data-direction="reverse" data-role="button" data-theme="b" data-transition="pop">Daily</a>
  <a href="#monthly" data-direction="reverse" data-role="button" data-theme="b" data-transition="pop">Monthly</a>
  </div>
</div>

<!-- Start of second page: #monthly -->
<div data-role="page" id="monthly" data-theme="a">
  <div data-role="header">
    <a href="#home" class="ui-btn ui-mini ui-corner-all ui-shadow ui-icon-home ui-btn-icon-left ui-btn-icon-notext" data-transition="pop">Home</a>
	<a href="javascript:waitloading();loadDataM();" id="refreshHomeButton" class="ui-btn ui-mini ui-corner-all ui-shadow ui-icon-refresh ui-btn-icon-left ui-btn-icon-notext">Refresh</a>
    <h1> Cons'Elec</h1>
  </div><!-- /header -->
  <div data-role="content" data-direction="reverse" data-theme="a">
   <div id ="outer_chart" class="outer-div">
    <div class="chart_div" id="chart_div_M0"></div>
	<div class="chart_div" id="chart_div_M1"></div>
   </div>
  </div> <!-- /content -->
  <div data-role="footer" style="text-align:center;">
    <a href="#actual" data-direction="reverse" data-role="button" data-theme="b" data-transition="pop">Actual</a>
	<a href="#hourly" data-direction="reverse" data-role="button" data-theme="b" data-transition="pop">2Hourly</a>
    <a href="#daily" data-direction="reverse" data-role="button" data-theme="b" data-transition="pop">Daily</a>
  <!-- <br><button id="ref_butn">Refresh</button>  -->
  </div> <!-- /footer -->
</div><!-- /page monthly -->

<!-- Start of second page: #daily -->
<div data-role="page" id="daily" data-theme="a">
  <div data-role="header">
    <a href="#home" class="ui-btn ui-mini ui-corner-all ui-shadow ui-icon-home ui-btn-icon-left ui-btn-icon-notext" data-transition="pop">Home</a>
	<a href="javascript:waitloading();loadDataD();" id="refreshHomeButton" class="ui-btn ui-mini ui-corner-all ui-shadow ui-icon-refresh ui-btn-icon-left ui-btn-icon-notext">Refresh</a>
    <h1> Cons'Elec</h1>
  </div><!-- /header -->
  <div data-role="content" data-direction="reverse" data-theme="a">
    <div id ="outer_chart" class="outer-div">
	  <div id="chart_div_D0"></div>
	  <div id="chart_div_D1"></div>
	</div>
  </div> <!-- /content -->
  <div data-role="footer" style="text-align:center;">
    <a href="#actual" data-direction="reverse" data-role="button" data-theme="b" data-transition="pop">Actual</a>
    <a href="#hourly" data-direction="reverse" data-role="button" data-theme="b" data-transition="pop">2Hourly</a>
    <a href="#monthly" data-direction="reverse" data-role="button" data-theme="b"data-transition="pop">Monthly</a>
  <!-- <br><button id="ref_butn">Refresh</button>  -->
  </div> <!-- /footer -->
</div><!-- /page daily -->

<!-- Start of second page: #hourly -->
<div data-role="page" id="hourly" data-theme="a">
  <div data-role="header">
    <a href="#home" class="ui-btn ui-mini ui-corner-all ui-shadow ui-icon-home ui-btn-icon-left ui-btn-icon-notext" data-transition="pop">Home</a>
	<a href="javascript:waitloading();loadDataH();" id="refreshHomeButton" class="ui-btn ui-mini ui-corner-all ui-shadow ui-icon-refresh ui-btn-icon-left ui-btn-icon-notext">Refresh</a>
    <h1> Cons'Elec</h1>
  </div><!-- /header -->
  <div data-role="content" data-direction="reverse" data-theme="a">
	<div id ="outer_chart" class="outer-div">
	  <div id="chart_div_H0"></div>
	  <div id="chart_div_H1"></div>
	</div>
  </div> <!-- /content -->
  <div data-role="footer" style="text-align:center;">
    <a href="#actual" data-direction="reverse" data-role="button" data-theme="b" data-transition="pop">Actual</a>
    <a href="#daily" data-direction="reverse" data-role="button" data-theme="b" data-transition="pop">Daily</a>
    <a href="#monthly" data-direction="reverse" data-role="button" data-theme="b" data-transition="pop">Monthly</a>
  </div> <!-- /footer -->
</div><!-- /page hourly -->

</body>
</html>