static const char* html_msg =

"<!DOCTYPE html>\n"
"<html><head><style>\n"
"html, body {\n"
"font-family: Arial, Helvetica, sans-serif;\n"
"}\n"
".title {\n"
"text-align: left;\n"
"font-family: Arial, Helvetica, sans-serif;\n"
"font-weight: bold;\n"
"font-size: 2em;\n"
"background-color: #525682;\n"
"box-shadow: 3px 3px #333761;\n"
"color: #ddee00;\n"
"padding:3px 6px\n"
"}\n"
"</style>\n"
"<meta name=\"viewport\" content=\"width=device-width, initial-scale=2\"/>\n"
"<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">\n"
"<title>tick-led</title>\n"
"</head>\n"
"<body text=\"#BABABA\" bgcolor=\"#3c406d\" >\n"
"<div class=\"title\">$msgt</div>\n"
"<p/>$msgb<p/>\n"
"</body></html>\n"

;


static const char* html_root =

"<!DOCTYPE html>\n"
"<html><head>\n"
"<style>\n"
"html, body {\n"
"font-family: Arial, Helvetica, sans-serif;\n"
"top:0;\n"
"bottom: 0;\n"
"left: 0;\n"
"right: 0;\n"
"}\n"
"table, th, td {\n"
"border: 0px solid black;\n"
"border-collapse: collapse;\n"
"background-color: #333761;\n"
"}\n"
".title {\n"
"text-align: left;\n"
"font-family: Arial, Helvetica, sans-serif;\n"
"font-weight: bold;\n"
"font-size: 2em;\n"
"background-color: #525682;\n"
"box-shadow: 3px 3px #333761;\n"
"color: #ddee00;\n"
"padding:3px 6px\n"
"}\n"
".version {\n"
"float: right;\n"
"}\n"
".fileToUpload {\n"
"color: #ddee00;\n"
"font-weight: bold;\n"
"font-size: 1em;\n"
"}\n"
".uplButton {\n"
"font-weight: bold;\n"
"font-size: 1em;\n"
"}\n"
".upl {\n"
"font-size: 1em;\n"
"}\n"
".groupHeader {\n"
"background-color: #525682;\n"
"box-shadow: 3px 3px #333761;\n"
"color: #ddee00;\n"
"}\n"
".pointer {\n"
"cursor: pointer;\n"
"}\n"
".inline {\n"
"display: inline;\n"
"}\n"
".right {\n"
"float: right;\n"
"}\n"
".one-line {\n"
"display: table-cell;\n"
"top: 0;\n"
"padding:0px 0px;\n"
"}\n"
"</style>\n"
"<meta name=\"viewport\" content=\"width=device-width, initial-scale=2\"/>\n"
"<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">\n"
"<title>tick-led</title>\n"
"<script type=\"text/javascript\">\n"
"const ACTION_CHECK_STATUS = 100;\n"
"const STATE_IDLE = 0;\n"
"const STATE_MONITOR = 1;\n"
"var inAction = 0;\n"
"var state = STATE_IDLE;\n"
"function log(str) {\n"
"document.getElementById('log').innerHTML += str + \"<BR>\";\n"
"}\n"
"async function fetchWithTimeout(resource) {\n"
"const controller = new AbortController();\n"
"const id = setTimeout(() => controller.abort(), 1800);\n"
"const response = await fetch(resource, {method: \"GET\", signal: controller.signal});\n"
"clearTimeout(id);\n"
"return response;\n"
"}\n"
"window.postAction  = function(act) {\n"
"if (inAction != 0) {\n"
"} else\n"
"if (act == \"reload\") {\n"
"console.log(\"reload!\");\n"
"history.go(0);\n"
"} else {\n"
"var retData;\n"
"inAction = 1;\n"
"console.log(\"post action: \" + act);\n"
"fetchWithTimeout(\"status\")\n"
".then((response) => response.text())\n"
".then((text) => {retData = text;})\n"
".then(() => {\n"
"console.log(\" received: \"  + retData);\n"
"if (retData.startsWith(\"finishedOK\")) {\n"
"console.log(\"Finished\");\n"
"document.getElementById('prog0').value = 100;\n"
"document.getElementById('upl').innerHTML = '<br/>Flashing finished OK.' +\n"
"'<br/><b>Power cycle the console.</b>'\n"
"state = STATE_IDLE;\n"
"} else\n"
"if (retData.startsWith(\"E:\")) {\n"
"document.getElementById('progLabel').innerHTML = \"Erasing\";\n"
"document.getElementById('prog0').value = retData.substring(2);\n"
"} else\n"
"if (retData.startsWith(\"F:\")) {\n"
"document.getElementById('progLabel').innerHTML = \"Writing\";\n"
"document.getElementById('prog0').value = retData.substring(2);\n"
"if (retData == \"F:100\") document.getElementById('upl').innerHTML = '<br/>Flashing finished.';\n"
"} else\n"
"else {\n"
"document.getElementById('upl').innerHTML = '<br/>Unknown message received:' + retData ;\n"
"console.log(\"Unknown message: \" +  retData);\n"
"state = STATE_IDLE;\n"
"}\n"
"inAction = 0;\n"
"})\n"
".catch (error => {console.log(\" recv failed\"); inAction = 0;});\n"
"}\n"
"}\n"
"function autoLoop() {\n"
"if (state == STATE_MONITOR) {\n"
"console.log(\"Monitor Loop...\");\n"
"postAction(ACTION_CHECK_STATUS);\n"
"}\n"
"}\n"
"function submitRequest() {\n"
"console.log(\"Sumbit request\");\n"
"fetchWithTimeout(\"apply\")\n"
".then((response) => response.text())\n"
".then((text) => {retData = text;})\n"
".then(() => {\n"
"console.log(\" received: \"  + retData);\n"
"if (retData.startsWith(\"OK\")) {\n"
"state = STATE_MONITOR;\n"
"document.getElementById('upl').innerHTML = '<br/>Writing in progress...';\n"
"} else {\n"
"state = STATE_IDLE;\n"
"document.getElementById('upl').innerHTML = '<br/>' + retData ;\n"
"}\n"
"})\n"
".catch (error => {console.log(\"recv failed\"); inAction = 0;});\n"
"}\n"
"function init() {\n"
"console.log(\"init called\");\n"
"inAction = 0;\n"
"state = STATE_IDLE;\n"
"history.pushState(null, document.title, location.href);\n"
"setInterval(autoLoop, 1000);\n"
"}\n"
"</script>\n"
"</head>\n"
"<body onload=\"init()\" text=\"#BABABA\" bgcolor=\"#3c406d\" >\n"
"<div class=\"title\">tick-led</div>\n"
"<div class=\"version\">$ver</div>\n"
"<p/>$txt_000<p/>\n"
"<form id=\"form1\" action=\"/lang\">\n"
"<label for=\"lang\">$txt_001</label><br>\n"
"<select id=\"lang\" name=\"lang\">\n"
"<option $lang0 value=\"0\">English</option>\n"
"<option $lang1 value=\"1\">Cestina</option>\n"
"</select>\n"
"<input type=\"submit\" value=\"$txt_002\">\n"
"</form>\n"
"<p>\n"
"<form id=\"form2\" action=\"/apply\">\n"
"<div class=\"groupHeader\">\n"
"$txt_003\n"
"</div>\n"
"<div>\n"
"<p>\n"
"<label for=\"cfgssid\">$txt_004</label><br>\n"
"<input type=\"text\" id=\"cfgssid\" name=\"cfgssid\" value=\"$cfs\" maxlength=\"47\"><br>\n"
"<p>\n"
"<label for=\"cfgpwd\">$txt_005</label><br>\n"
"<input type=\"password\" id=\"cfgpwd\" name=\"cfgpwd\" maxlength=\"47\" ><br>\n"
"</div>\n"
"<div class=\"groupHeader\">\n"
"$txt_006\n"
"</div>\n"
"<div>\n"
"<p>\n"
"<label for=\"ntpssid\">$txt_004</label><br>\n"
"<input type=\"text\" id=\"ntpssid\" name=\"ntpssid\" value=\"$nts\" maxlength=\"47\"><br>\n"
"<p>\n"
"<label for=\"ntppwd\">$txt_005</label><br>\n"
"<input type=\"password\" id=\"ntppwd\" name=\"ntppwd\" maxlength=\"47\"><br>\n"
"<p>\n"
"<label for=\"ntphost\">$txt_007</label><br>\n"
"<input type=\"text\" id=\"ntphost\" name=\"ntphost\" value=\"$nth\" maxlength=\"47\"><br>\n"
"<p>\n"
"<label for=\"ntprate\">$txt_008</label><br>\n"
"<select id=\"ntprate\" name=\"ntprate\">\n"
"<option $ntprat0 value=\"0\">$txt_009</option>\n"
"<option $ntprat1 value=\"1\">$txt_010</option>\n"
"<option $ntprat2 value=\"2\">$txt_011</option>\n"
"<option $ntprat3 value=\"3\">$txt_012</option>\n"
"</select>\n"
"<p>\n"
"</div>\n"
"<div class=\"groupHeader\">\n"
"$txt_013\n"
"</div>\n"
"<div>\n"
"<p>\n"
"<label for=\"tzoffs\">$txt_014</label><br>\n"
"<input type=\"text\" id=\"tzofs\" name=\"tzofs\" value=\"$tzof\" maxlength=\"8\"><br>\n"
"<p>\n"
"<label for=\"tzods\">$txt_015</label><br>\n"
"<input type=\"text\" id=\"tzods\" name=\"tzods\" value=\"$tzds\" maxlength=\"8\"><br>\n"
"<p>\n"
"<label for=\"tzdsr\">$txt_016</label><br>\n"
"<select id=\"tzdsr\" name=\"tzdsr\">\n"
"<option $tzdsr0 value=\"0\">$txt_017</option>\n"
"<option $tzdsr1 value=\"1\">$txt_018</option>\n"
"<option $tzdsr2 value=\"2\">$txt_019</option>\n"
"<option $tzdsr3 value=\"3\">$txt_020</option>\n"
"<option $tzdsr4 value=\"4\">$txt_021</option>\n"
"<option $tzdsr5 value=\"5\">$txt_022</option>\n"
"<option $tzdsr6 value=\"6\">$txt_023</option>\n"
"<option $tzdsr7 value=\"7\">$txt_024</option>\n"
"<option $tzdsr8 value=\"8\">$txt_025</option>\n"
"<option $tzdsr9 value=\"9\">$txt_026</option>\n"
"<option $tzdsr10 value=\"10\">$txt_027</option>\n"
"<option $tzdsr11 value=\"11\">$txt_028</option>\n"
"<option $tzdsr12 value=\"12\">$txt_029</option>\n"
"<option $tzdsr13 value=\"13\">$txt_030</option>\n"
"<option $tzdsr14 value=\"14\">$txt_031</option>\n"
"<option $tzdsr15 value=\"15\">$txt_032</option>\n"
"<option $tzdsr16 value=\"16\">$txt_033</option>\n"
"<option $tzdsr17 value=\"17\">$txt_034</option>\n"
"<option $tzdsr18 value=\"18\">$txt_035</option>\n"
"<option $tzdsr19 value=\"19\">$txt_036</option>\n"
"</select>\n"
"<p>\n"
"</div>\n"
"<div class=\"groupHeader\">\n"
"$txt_037\n"
"</div>\n"
"<div>\n"
"<p>\n"
"<label for=\"colsch\">$txt_038</label><br>\n"
"<select id=\"colsch\" name=\"colsch\">\n"
"<option $csc0 value=\"0\">$txt_039</option>\n"
"<option $csc1 value=\"1\">$txt_040</option>\n"
"<option $csc2 value=\"2\">$txt_041</option>\n"
"<option $csc3 value=\"3\">$txt_042</option>\n"
"<option $csc4 value=\"4\">$txt_043</option>\n"
"<option $csc5 value=\"5\">$txt_044</option>\n"
"<option $csc6 value=\"6\">$txt_045</option>\n"
"<option $csc7 value=\"7\">$txt_046</option>\n"
"<option $csc8 value=\"8\">$txt_047</option>\n"
"<option $csc9 value=\"9\">$txt_048</option>\n"
"</select>\n"
"<p>\n"
"<label for=\"minstl\">$txt_049</label><br>\n"
"<select id=\"minstl\" name=\"minstl\">\n"
"<option $minstl0 value=\"0\">$txt_050</option>\n"
"<option $minstl1 value=\"1\">$txt_051</option>\n"
"<option $minstl2 value=\"2\">$txt_052</option>\n"
"</select>\n"
"<p>\n"
"<label for=\"secstl\">$txt_053</label><br>\n"
"<select id=\"secstl\" name=\"secstl\">\n"
"<option $secstl0 value=\"0\">$txt_054</option>\n"
"<option $secstl1 value=\"1\">$txt_055</option>\n"
"<option $secstl2 value=\"2\">$txt_056</option>\n"
"</select>\n"
"<p>\n"
"<label for=\"oristl\">$txt_057</label><br>\n"
"<select id=\"oristl\" name=\"oristl\">\n"
"<option $oristl0 value=\"0\">$txt_058</option>\n"
"<option $oristl1 value=\"1\">$txt_059</option>\n"
"<option $oristl2 value=\"2\">$txt_060</option>\n"
"</select>\n"
"</div>\n"
"<div class=\"groupHeader\">\n"
"$txt_061\n"
"</div>\n"
"<div>\n"
"<p>\n"
"<label for=\"vebrig\">$txt_062</label><br>\n"
"<select id=\"vebrig\" name=\"vebrig\">\n"
"<option $vebrig0 value=\"0\">100%</option>\n"
"<option $vebrig1 value=\"1\" >80%</option>\n"
"<option $vebrig2 value=\"2\" >60%</option>\n"
"<option $vebrig3 value=\"3\" >40%</option>\n"
"<option $vebrig4 value=\"4\" >20%</option>\n"
"</select>\n"
"<p>\n"
"<label for=\"vedimg\">$txt_063</label><br>\n"
"<select id=\"vedimg\" name=\"vedimg\">\n"
"<option $vedimg0 value=\"0\">$txt_064</option>\n"
"<option $vedimg1 value=\"1\">$txt_065</option>\n"
"<option $vedimg2 value=\"2\">$txt_066</option>\n"
"<option $vedimg3 value=\"3\">$txt_067</option>\n"
"<option $vedimg4 value=\"4\">$txt_068</option>\n"
"</select>\n"
"<p>\n"
"<div >$txt_069</label><br>\n"
"<div class=\"one-line\">\n"
"<select id=\"vedimfh\" name=\"vedimfh\">\n"
"<option $vedimfh0 value=\"0\">00</option>\n"
"<option $vedimfh1 value=\"1\">01</option>\n"
"<option $vedimfh2 value=\"2\">02</option>\n"
"<option $vedimfh3 value=\"3\">03</option>\n"
"<option $vedimfh4 value=\"4\">04</option>\n"
"<option $vedimfh5 value=\"5\">05</option>\n"
"<option $vedimfh6 value=\"6\">06</option>\n"
"<option $vedimfh7 value=\"7\">07</option>\n"
"<option $vedimfh8 value=\"8\">08</option>\n"
"<option $vedimfh9 value=\"9\">09</option>\n"
"<option $vedimfh10 value=\"10\">10</option>\n"
"<option $vedimfh11 value=\"11\">11</option>\n"
"<option $vedimfh12 value=\"12\">12</option>\n"
"<option $vedimfh13 value=\"13\">13</option>\n"
"<option $vedimfh14 value=\"14\">14</option>\n"
"<option $vedimfh15 value=\"15\">15</option>\n"
"<option $vedimfh16 value=\"16\">16</option>\n"
"<option $vedimfh17 value=\"17\">17</option>\n"
"<option $vedimfh18 value=\"18\">18</option>\n"
"<option $vedimfh19 value=\"19\">19</option>\n"
"<option $vedimfh20 value=\"20\">20</option>\n"
"<option $vedimfh21 value=\"21\">21</option>\n"
"<option $vedimfh22 value=\"22\">22</option>\n"
"<option $vedimfh23 value=\"23\">23</option>\n"
"</select>\n"
"</div>\n"
"<div class=\"one-line\">:</div>\n"
"<div class=\"one-line\">\n"
"<select id=\"vedimfm\" name=\"vedimfm\">\n"
"<option $vedimfm0 value=\"0\">00</option>\n"
"<option $vedimfm1 value=\"5\">05</option>\n"
"<option $vedimfm2 value=\"10\">10</option>\n"
"<option $vedimfm3 value=\"15\">15</option>\n"
"<option $vedimfm4 value=\"20\">20</option>\n"
"<option $vedimfm5 value=\"25\">25</option>\n"
"<option $vedimfm6 value=\"30\">30</option>\n"
"<option $vedimfm7 value=\"35\">35</option>\n"
"<option $vedimfm8 value=\"40\">40</option>\n"
"<option $vedimfm9 value=\"45\">45</option>\n"
"<option $vedimfm10 value=\"50\">50</option>\n"
"<option $vedimfm11 value=\"55\">55</option>\n"
"</select>\n"
"</div>\n"
"</div>\n"
"<p>\n"
"<div>$txt_070</label><br>\n"
"<div class=\"one-line\">\n"
"<select id=\"vedimth\" name=\"vedimth\">\n"
"<option $vedimth0 value=\"0\">00</option>\n"
"<option $vedimth1 value=\"1\">01</option>\n"
"<option $vedimth2 value=\"2\">02</option>\n"
"<option $vedimth3 value=\"3\">03</option>\n"
"<option $vedimth4 value=\"4\">04</option>\n"
"<option $vedimth5 value=\"5\">05</option>\n"
"<option $vedimth6 value=\"6\">06</option>\n"
"<option $vedimth7 value=\"7\">07</option>\n"
"<option $vedimth8 value=\"8\">08</option>\n"
"<option $vedimth9 value=\"9\">09</option>\n"
"<option $vedimth10 value=\"10\">10</option>\n"
"<option $vedimth11 value=\"11\">11</option>\n"
"<option $vedimth12 value=\"12\">12</option>\n"
"<option $vedimth13 value=\"13\">13</option>\n"
"<option $vedimth14 value=\"14\">14</option>\n"
"<option $vedimth15 value=\"15\">15</option>\n"
"<option $vedimth16 value=\"16\">16</option>\n"
"<option $vedimth17 value=\"17\">17</option>\n"
"<option $vedimth18 value=\"18\">18</option>\n"
"<option $vedimth19 value=\"19\">19</option>\n"
"<option $vedimth20 value=\"20\">20</option>\n"
"<option $vedimth21 value=\"21\">21</option>\n"
"<option $vedimth22 value=\"22\">22</option>\n"
"<option $vedimth23 value=\"23\">23</option>\n"
"</select>\n"
"</div>\n"
"<div class=\"one-line\">:</div>\n"
"<div class=\"one-line\">\n"
"<select id=\"vedimtm\" name=\"vedimtm\">\n"
"<option $vedimtm0 value=\"0\">00</option>\n"
"<option $vedimtm1 value=\"5\">05</option>\n"
"<option $vedimtm2 value=\"10\">10</option>\n"
"<option $vedimtm3 value=\"15\">15</option>\n"
"<option $vedimtm4 value=\"20\">20</option>\n"
"<option $vedimtm5 value=\"25\">25</option>\n"
"<option $vedimtm6 value=\"30\">30</option>\n"
"<option $vedimtm7 value=\"35\">35</option>\n"
"<option $vedimtm8 value=\"40\">40</option>\n"
"<option $vedimtm9 value=\"45\">45</option>\n"
"<option $vedimtm10 value=\"50\">50</option>\n"
"<option $vedimtm11 value=\"55\">55</option>\n"
"</select>\n"
"</div>\n"
"</div>\n"
"<p>\n"
"<label for=\"vehana\">$txt_071</label><br>\n"
"<select id=\"vehana\" name=\"vehana\">\n"
"<option $vehana0 value=\"0\">$txt_072</option>\n"
"<option $vehana1 value=\"1\">$txt_073</option>\n"
"<option $vehana2 value=\"2\">$txt_074</option>\n"
"<option $vehana3 value=\"3\">$txt_075</option>\n"
"</select>\n"
"</div>\n"
"<div class=\"groupHeader\">\n"
"$txt_076\n"
"</div>\n"
"<div>\n"
"<p>\n"
"<label for=\"alapit\">$txt_077</label><br>\n"
"<select id=\"alapit\" name=\"alapit\">\n"
"<option $alapit0 value=\"0\">$txt_066</option>\n"
"<option $alapit1 value=\"1\">$txt_065</option>\n"
"<option $alapit2 value=\"2\">$txt_078</option>\n"
"</select>\n"
"<!-- ######################## ALARM 1 ##################### -->\n"
"<p>\n"
"<div id=\"ala1\">$txt_079<br>\n"
"<div class=\"one-line\" >\n"
"<select id=\"ala1day\" name=\"ala1day\">\n"
"<option $ala1day0 value=\"0\">$txt_080</option>\n"
"<option $ala1day1 value=\"1\">$txt_081</option>\n"
"<option $ala1day2 value=\"2\">$txt_082</option>\n"
"<option $ala1day3 value=\"3\">$txt_083</option>\n"
"<option $ala1day4 value=\"4\">$txt_084</option>\n"
"<option $ala1day5 value=\"5\">$txt_085</option>\n"
"<option $ala1day6 value=\"6\">$txt_086</option>\n"
"<option $ala1day7 value=\"7\">$txt_087</option>\n"
"<option $ala1day8 value=\"8\">$txt_088</option>\n"
"<option $ala1day9 value=\"9\">$txt_089</option>\n"
"<option $ala1day10 value=\"10\">$txt_090</option>\n"
"</select>\n"
"</div>\n"
"<div class=\"one-line\">&nbsp;</div>\n"
"<div class=\"one-line\">\n"
"<select id=\"ala1hr\" name=\"ala1hr\">\n"
"<option $ala1hr0 value=\"0\">0</option>\n"
"<option $ala1hr1 value=\"1\">1</option>\n"
"<option $ala1hr2 value=\"2\">2</option>\n"
"<option $ala1hr3 value=\"3\">3</option>\n"
"<option $ala1hr4 value=\"4\">4</option>\n"
"<option $ala1hr5 value=\"5\">5</option>\n"
"<option $ala1hr6 value=\"6\">6</option>\n"
"<option $ala1hr7 value=\"7\">7</option>\n"
"<option $ala1hr8 value=\"8\">8</option>\n"
"<option $ala1hr9 value=\"9\">9</option>\n"
"<option $ala1hr10 value=\"10\">10</option>\n"
"<option $ala1hr11 value=\"11\">11</option>\n"
"<option $ala1hr12 value=\"12\">12</option>\n"
"<option $ala1hr13 value=\"13\">13</option>\n"
"<option $ala1hr14 value=\"14\">14</option>\n"
"<option $ala1hr15 value=\"15\">15</option>\n"
"<option $ala1hr16 value=\"16\">16</option>\n"
"<option $ala1hr17 value=\"17\">17</option>\n"
"<option $ala1hr18 value=\"18\">18</option>\n"
"<option $ala1hr19 value=\"19\">19</option>\n"
"<option $ala1hr20 value=\"20\">20</option>\n"
"<option $ala1hr21 value=\"21\">21</option>\n"
"<option $ala1hr22 value=\"22\">22</option>\n"
"<option $ala1hr23 value=\"23\">23</option>\n"
"</select>\n"
"</div>\n"
"<div class=\"one-line\">&nbsp;</div>\n"
"<div class=\"one-line\">\n"
"<select id=\"ala1min\" name=\"ala1min\">\n"
"<option $ala1min0 value=\"0\">00</option>\n"
"<option $ala1min1 value=\"5\">05</option>\n"
"<option $ala1min2 value=\"10\">10</option>\n"
"<option $ala1min3 value=\"15\">15</option>\n"
"<option $ala1min4 value=\"20\">20</option>\n"
"<option $ala1min5 value=\"25\">25</option>\n"
"<option $ala1min6 value=\"30\">30</option>\n"
"<option $ala1min7 value=\"35\">35</option>\n"
"<option $ala1min8 value=\"40\">40</option>\n"
"<option $ala1min9 value=\"45\">45</option>\n"
"<option $ala1min10 value=\"50\">50</option>\n"
"<option $ala1min11 value=\"55\">55</option>\n"
"</select>\n"
"</div>\n"
"<div class=\"one-line\">&nbsp;</div>\n"
"<div class=\"one-line\">\n"
"<select id=\"ala1bep\" name=\"ala1bep\">\n"
"<option $ala1bep0 value=\"0\">$txt_091</option>\n"
"<option $ala1bep1 value=\"1\">$txt_092</option>\n"
"<option $ala1bep2 value=\"2\">$txt_093</option>\n"
"<option $ala1bep3 value=\"3\">A-Team</option>\n"
"<option $ala1bep4 value=\"4\">Rich World</option>\n"
"<option $ala1bep5 value=\"5\">Indiana</option>\n"
"<option $ala1bep6 value=\"6\">Staw Wars</option>\n"
"<option $ala1bep7 value=\"7\">Adams Family</option>\n"
"<option $ala1bep8 value=\"8\">Bod</option>\n"
"<option $ala1bep9 value=\"9\">Duel Banjos</option>\n"
"<option $ala1bep10 value=\"10\">Entertainer</option>\n"
"<option $ala1bep11 value=\"11\">G-Busters</option>\n"
"<option $ala1bep12 value=\"12\">Jingle Bells</option>\n"
"<option $ala1bep13 value=\"13\">Knight Rider</option>\n"
"<option $ala1bep14 value=\"14\">12th day</option>\n"
"<option $ala1bep15 value=\"15\">Popcorn</option>\n"
"<option $ala1bep16 value=\"16\">YMCA</option>\n"
"</select>\n"
"</div>\n"
"<div class=\"one-line\">&nbsp;</div>\n"
"<div class=\"one-line\">\n"
"<select id=\"ala1lop\" name=\"ala1lop\">\n"
"<option $ala1lop0 value=\"0\">$txt_094</option>\n"
"<option $ala1lop1 value=\"1\">$txt_095</option>\n"
"<option $ala1lop2 value=\"2\">$txt_096</option>\n"
"<option $ala1lop3 value=\"3\">$txt_097</option>\n"
"<option $ala1lop4 value=\"5\">$txt_098</option>\n"
"<option $ala1lop5 value=\"10\">$txt_099</option>\n"
"<option $ala1lop6 value=\"20\">$txt_100</option>\n"
"<option $ala1lop7 value=\"30\">$txt_101</option>\n"
"<option $ala1lop8 value=\"40\">$txt_102</option>\n"
"<option $ala1lop9 value=\"50\">$txt_103</option>\n"
"<option $ala1lop10 value=\"100\">$txt_104</option>\n"
"<option $ala1lop11 value=\"300\">$txt_105</option>\n"
"</select>\n"
"</div>\n"
"</div>\n"
"<p>\n"
"<!-- ######################## ALARM 2 ##################### -->\n"
"<p>\n"
"<div id=\"ala2\">$txt_106<br>\n"
"<div class=\"one-line\" >\n"
"<select id=\"ala2day\" name=\"ala2day\">\n"
"<option $ala2day0 value=\"0\">$txt_080</option>\n"
"<option $ala2day1 value=\"1\">$txt_081</option>\n"
"<option $ala2day2 value=\"2\">$txt_082</option>\n"
"<option $ala2day3 value=\"3\">$txt_083</option>\n"
"<option $ala2day4 value=\"4\">$txt_084</option>\n"
"<option $ala2day5 value=\"5\">$txt_085</option>\n"
"<option $ala2day6 value=\"6\">$txt_086</option>\n"
"<option $ala2day7 value=\"7\">$txt_087</option>\n"
"<option $ala2day8 value=\"8\">$txt_088</option>\n"
"<option $ala2day9 value=\"9\">$txt_089</option>\n"
"<option $ala2day10 value=\"10\">$txt_090</option>\n"
"</select>\n"
"</div>\n"
"<div class=\"one-line\">&nbsp;</div>\n"
"<div class=\"one-line\">\n"
"<select id=\"ala2hr\" name=\"ala2hr\">\n"
"<option $ala2hr0 value=\"0\">0</option>\n"
"<option $ala2hr1 value=\"1\">1</option>\n"
"<option $ala2hr2 value=\"2\">2</option>\n"
"<option $ala2hr3 value=\"3\">3</option>\n"
"<option $ala2hr4 value=\"4\">4</option>\n"
"<option $ala2hr5 value=\"5\">5</option>\n"
"<option $ala2hr6 value=\"6\">6</option>\n"
"<option $ala2hr7 value=\"7\">7</option>\n"
"<option $ala2hr8 value=\"8\">8</option>\n"
"<option $ala2hr9 value=\"9\">9</option>\n"
"<option $ala2hr10 value=\"10\">10</option>\n"
"<option $ala2hr11 value=\"11\">11</option>\n"
"<option $ala2hr12 value=\"12\">12</option>\n"
"<option $ala2hr13 value=\"13\">13</option>\n"
"<option $ala2hr14 value=\"14\">14</option>\n"
"<option $ala2hr15 value=\"15\">15</option>\n"
"<option $ala2hr16 value=\"16\">16</option>\n"
"<option $ala2hr17 value=\"17\">17</option>\n"
"<option $ala2hr18 value=\"18\">18</option>\n"
"<option $ala2hr19 value=\"19\">19</option>\n"
"<option $ala2hr20 value=\"20\">20</option>\n"
"<option $ala2hr21 value=\"21\">21</option>\n"
"<option $ala2hr22 value=\"22\">22</option>\n"
"<option $ala2hr23 value=\"23\">23</option>\n"
"</select>\n"
"</div>\n"
"<div class=\"one-line\">&nbsp;</div>\n"
"<div class=\"one-line\">\n"
"<select id=\"ala2min\" name=\"ala2min\">\n"
"<option $ala2min0 value=\"0\">00</option>\n"
"<option $ala2min1 value=\"5\">05</option>\n"
"<option $ala2min2 value=\"10\">10</option>\n"
"<option $ala2min3 value=\"15\">15</option>\n"
"<option $ala2min4 value=\"20\">20</option>\n"
"<option $ala2min5 value=\"25\">25</option>\n"
"<option $ala2min6 value=\"30\">30</option>\n"
"<option $ala2min7 value=\"35\">35</option>\n"
"<option $ala2min8 value=\"40\">40</option>\n"
"<option $ala2min9 value=\"45\">45</option>\n"
"<option $ala2min10 value=\"50\">50</option>\n"
"<option $ala2min11 value=\"55\">55</option>\n"
"</select>\n"
"</div>\n"
"<div class=\"one-line\">&nbsp;</div>\n"
"<div class=\"one-line\">\n"
"<select id=\"ala2bep\" name=\"ala2bep\">\n"
"<option $ala2bep0 value=\"0\">$txt_091</option>\n"
"<option $ala2bep1 value=\"1\">$txt_092</option>\n"
"<option $ala2bep2 value=\"2\">$txt_093</option>\n"
"<option $ala2bep3 value=\"3\">A-Team</option>\n"
"<option $ala2bep4 value=\"4\">Rich World</option>\n"
"<option $ala2bep5 value=\"5\">Indiana</option>\n"
"<option $ala2bep6 value=\"6\">Staw Wars</option>\n"
"<option $ala2bep7 value=\"7\">Adams Family</option>\n"
"<option $ala2bep8 value=\"8\">Bod</option>\n"
"<option $ala2bep9 value=\"9\">Duel Banjos</option>\n"
"<option $ala2bep10 value=\"10\">Entertainer</option>\n"
"<option $ala2bep11 value=\"11\">G-Busters</option>\n"
"<option $ala2bep12 value=\"12\">Jingle Bells</option>\n"
"<option $ala2bep13 value=\"13\">Knight Rider</option>\n"
"<option $ala2bep14 value=\"14\">12th day</option>\n"
"<option $ala2bep15 value=\"15\">Popcorn</option>\n"
"<option $ala2bep16 value=\"16\">YMCA</option>\n"
"</select>\n"
"</div>\n"
"<div class=\"one-line\">&nbsp;</div>\n"
"<div class=\"one-line\">\n"
"<select id=\"ala2lop\" name=\"ala2lop\">\n"
"<option $ala2lop0 value=\"0\">$txt_094</option>\n"
"<option $ala2lop1 value=\"1\">$txt_095</option>\n"
"<option $ala2lop2 value=\"2\">$txt_096</option>\n"
"<option $ala2lop3 value=\"3\">$txt_097</option>\n"
"<option $ala2lop4 value=\"5\">$txt_098</option>\n"
"<option $ala2lop5 value=\"10\">$txt_099</option>\n"
"<option $ala2lop6 value=\"20\">$txt_100</option>\n"
"<option $ala2lop7 value=\"30\">$txt_101</option>\n"
"<option $ala2lop8 value=\"40\">$txt_102</option>\n"
"<option $ala2lop9 value=\"50\">$txt_103</option>\n"
"<option $ala2lop10 value=\"100\">$txt_104</option>\n"
"<option $ala2lop11 value=\"300\">$txt_105</option>\n"
"</select>\n"
"</div>\n"
"</div>\n"
"<p>\n"
"<!-- ######################## ALARM 3 ##################### -->\n"
"<p>\n"
"<div id=\"ala3\">$txt_107<br>\n"
"<div class=\"one-line\" >\n"
"<select id=\"ala3day\" name=\"ala3day\">\n"
"<option $ala3day0 value=\"0\">$txt_080</option>\n"
"<option $ala3day1 value=\"1\">$txt_081</option>\n"
"<option $ala3day2 value=\"2\">$txt_082</option>\n"
"<option $ala3day3 value=\"3\">$txt_083</option>\n"
"<option $ala3day4 value=\"4\">$txt_084</option>\n"
"<option $ala3day5 value=\"5\">$txt_085</option>\n"
"<option $ala3day6 value=\"6\">$txt_086</option>\n"
"<option $ala3day7 value=\"7\">$txt_087</option>\n"
"<option $ala3day8 value=\"8\">$txt_088</option>\n"
"<option $ala3day9 value=\"9\">$txt_089</option>\n"
"<option $ala3day10 value=\"10\">$txt_090</option>\n"
"</select>\n"
"</div>\n"
"<div class=\"one-line\">&nbsp;</div>\n"
"<div class=\"one-line\">\n"
"<select id=\"ala3hr\" name=\"ala3hr\">\n"
"<option $ala3hr0 value=\"0\">0</option>\n"
"<option $ala3hr1 value=\"1\">1</option>\n"
"<option $ala3hr2 value=\"2\">2</option>\n"
"<option $ala3hr3 value=\"3\">3</option>\n"
"<option $ala3hr4 value=\"4\">4</option>\n"
"<option $ala3hr5 value=\"5\">5</option>\n"
"<option $ala3hr6 value=\"6\">6</option>\n"
"<option $ala3hr7 value=\"7\">7</option>\n"
"<option $ala3hr8 value=\"8\">8</option>\n"
"<option $ala3hr9 value=\"9\">9</option>\n"
"<option $ala3hr10 value=\"10\">10</option>\n"
"<option $ala3hr11 value=\"11\">11</option>\n"
"<option $ala3hr12 value=\"12\">12</option>\n"
"<option $ala3hr13 value=\"13\">13</option>\n"
"<option $ala3hr14 value=\"14\">14</option>\n"
"<option $ala3hr15 value=\"15\">15</option>\n"
"<option $ala3hr16 value=\"16\">16</option>\n"
"<option $ala3hr17 value=\"17\">17</option>\n"
"<option $ala3hr18 value=\"18\">18</option>\n"
"<option $ala3hr19 value=\"19\">19</option>\n"
"<option $ala3hr20 value=\"20\">20</option>\n"
"<option $ala3hr21 value=\"21\">21</option>\n"
"<option $ala3hr22 value=\"22\">22</option>\n"
"<option $ala3hr23 value=\"23\">23</option>\n"
"</select>\n"
"</div>\n"
"<div class=\"one-line\">&nbsp;</div>\n"
"<div class=\"one-line\">\n"
"<select id=\"ala3min\" name=\"ala3min\">\n"
"<option $ala3min0 value=\"0\">00</option>\n"
"<option $ala3min1 value=\"5\">05</option>\n"
"<option $ala3min2 value=\"10\">10</option>\n"
"<option $ala3min3 value=\"15\">15</option>\n"
"<option $ala3min4 value=\"20\">20</option>\n"
"<option $ala3min5 value=\"25\">25</option>\n"
"<option $ala3min6 value=\"30\">30</option>\n"
"<option $ala3min7 value=\"35\">35</option>\n"
"<option $ala3min8 value=\"40\">40</option>\n"
"<option $ala3min9 value=\"45\">45</option>\n"
"<option $ala3min10 value=\"50\">50</option>\n"
"<option $ala3min11 value=\"55\">55</option>\n"
"</select>\n"
"</div>\n"
"<div class=\"one-line\">&nbsp;</div>\n"
"<div class=\"one-line\">\n"
"<select id=\"ala3bep\" name=\"ala3bep\">\n"
"<option $ala3bep0 value=\"0\">$txt_091</option>\n"
"<option $ala3bep1 value=\"1\">$txt_092</option>\n"
"<option $ala3bep2 value=\"2\">$txt_093</option>\n"
"<option $ala3bep3 value=\"3\">A-Team</option>\n"
"<option $ala3bep4 value=\"4\">Rich World</option>\n"
"<option $ala3bep5 value=\"5\">Indiana</option>\n"
"<option $ala3bep6 value=\"6\">Staw Wars</option>\n"
"<option $ala3bep7 value=\"7\">Adams Family</option>\n"
"<option $ala3bep8 value=\"8\">Bod</option>\n"
"<option $ala3bep9 value=\"9\">Duel Banjos</option>\n"
"<option $ala3bep10 value=\"10\">Entertainer</option>\n"
"<option $ala3bep11 value=\"11\">G-Busters</option>\n"
"<option $ala3bep12 value=\"12\">Jingle Bells</option>\n"
"<option $ala3bep13 value=\"13\">Knight Rider</option>\n"
"<option $ala3bep14 value=\"14\">12th day</option>\n"
"<option $ala3bep15 value=\"15\">Popcorn</option>\n"
"<option $ala3bep16 value=\"16\">YMCA</option>\n"
"</select>\n"
"</div>\n"
"<div class=\"one-line\">&nbsp;</div>\n"
"<div class=\"one-line\">\n"
"<select id=\"ala3lop\" name=\"ala3lop\">\n"
"<option $ala3lop0 value=\"0\">$txt_094</option>\n"
"<option $ala3lop1 value=\"1\">$txt_095</option>\n"
"<option $ala3lop2 value=\"2\">$txt_096</option>\n"
"<option $ala3lop3 value=\"3\">$txt_097</option>\n"
"<option $ala3lop4 value=\"5\">$txt_098</option>\n"
"<option $ala3lop5 value=\"10\">$txt_099</option>\n"
"<option $ala3lop6 value=\"20\">$txt_100</option>\n"
"<option $ala3lop7 value=\"30\">$txt_101</option>\n"
"<option $ala3lop8 value=\"40\">$txt_102</option>\n"
"<option $ala3lop9 value=\"50\">$txt_103</option>\n"
"<option $ala3lop10 value=\"100\">$txt_104</option>\n"
"<option $ala3lop11 value=\"300\">$txt_105</option>\n"
"</select>\n"
"</div>\n"
"</div>\n"
"<p>\n"
"<!-- ######################## ALARM 4 ##################### -->\n"
"<p>\n"
"<div id=\"ala4\">$txt_108<br>\n"
"<div class=\"one-line\" >\n"
"<select id=\"ala4day\" name=\"ala4day\">\n"
"<option $ala4day0 value=\"0\">$txt_080</option>\n"
"<option $ala4day1 value=\"1\">$txt_081</option>\n"
"<option $ala4day2 value=\"2\">$txt_082</option>\n"
"<option $ala4day3 value=\"3\">$txt_083</option>\n"
"<option $ala4day4 value=\"4\">$txt_084</option>\n"
"<option $ala4day5 value=\"5\">$txt_085</option>\n"
"<option $ala4day6 value=\"6\">$txt_086</option>\n"
"<option $ala4day7 value=\"7\">$txt_087</option>\n"
"<option $ala4day8 value=\"8\">$txt_088</option>\n"
"<option $ala4day9 value=\"9\">$txt_089</option>\n"
"<option $ala4day10 value=\"10\">$txt_090</option>\n"
"</select>\n"
"</div>\n"
"<div class=\"one-line\">&nbsp;</div>\n"
"<div class=\"one-line\">\n"
"<select id=\"ala4hr\" name=\"ala4hr\">\n"
"<option $ala4hr0 value=\"0\">0</option>\n"
"<option $ala4hr1 value=\"1\">1</option>\n"
"<option $ala4hr2 value=\"2\">2</option>\n"
"<option $ala4hr3 value=\"3\">3</option>\n"
"<option $ala4hr4 value=\"4\">4</option>\n"
"<option $ala4hr5 value=\"5\">5</option>\n"
"<option $ala4hr6 value=\"6\">6</option>\n"
"<option $ala4hr7 value=\"7\">7</option>\n"
"<option $ala4hr8 value=\"8\">8</option>\n"
"<option $ala4hr9 value=\"9\">9</option>\n"
"<option $ala4hr10 value=\"10\">10</option>\n"
"<option $ala4hr11 value=\"11\">11</option>\n"
"<option $ala4hr12 value=\"12\">12</option>\n"
"<option $ala4hr13 value=\"13\">13</option>\n"
"<option $ala4hr14 value=\"14\">14</option>\n"
"<option $ala4hr15 value=\"15\">15</option>\n"
"<option $ala4hr16 value=\"16\">16</option>\n"
"<option $ala4hr17 value=\"17\">17</option>\n"
"<option $ala4hr18 value=\"18\">18</option>\n"
"<option $ala4hr19 value=\"19\">19</option>\n"
"<option $ala4hr20 value=\"20\">20</option>\n"
"<option $ala4hr21 value=\"21\">21</option>\n"
"<option $ala4hr22 value=\"22\">22</option>\n"
"<option $ala4hr23 value=\"23\">23</option>\n"
"</select>\n"
"</div>\n"
"<div class=\"one-line\">&nbsp;</div>\n"
"<div class=\"one-line\">\n"
"<select id=\"ala4min\" name=\"ala4min\">\n"
"<option $ala4min0 value=\"0\">00</option>\n"
"<option $ala4min1 value=\"5\">05</option>\n"
"<option $ala4min2 value=\"10\">10</option>\n"
"<option $ala4min3 value=\"15\">15</option>\n"
"<option $ala4min4 value=\"20\">20</option>\n"
"<option $ala4min5 value=\"25\">25</option>\n"
"<option $ala4min6 value=\"30\">30</option>\n"
"<option $ala4min7 value=\"35\">35</option>\n"
"<option $ala4min8 value=\"40\">40</option>\n"
"<option $ala4min9 value=\"45\">45</option>\n"
"<option $ala4min10 value=\"50\">50</option>\n"
"<option $ala4min11 value=\"55\">55</option>\n"
"</select>\n"
"</div>\n"
"<div class=\"one-line\">&nbsp;</div>\n"
"<div class=\"one-line\">\n"
"<select id=\"ala4bep\" name=\"ala4bep\">\n"
"<option $ala4bep0 value=\"0\">$txt_091</option>\n"
"<option $ala4bep1 value=\"1\">$txt_092</option>\n"
"<option $ala4bep2 value=\"2\">$txt_093</option>\n"
"<option $ala4bep3 value=\"3\">A-Team</option>\n"
"<option $ala4bep4 value=\"4\">Rich World</option>\n"
"<option $ala4bep5 value=\"5\">Indiana</option>\n"
"<option $ala4bep6 value=\"6\">Staw Wars</option>\n"
"<option $ala4bep7 value=\"7\">Adams Family</option>\n"
"<option $ala4bep8 value=\"8\">Bod</option>\n"
"<option $ala4bep9 value=\"9\">Duel Banjos</option>\n"
"<option $ala4bep10 value=\"10\">Entertainer</option>\n"
"<option $ala4bep11 value=\"11\">G-Busters</option>\n"
"<option $ala4bep12 value=\"12\">Jingle Bells</option>\n"
"<option $ala4bep13 value=\"13\">Knight Rider</option>\n"
"<option $ala4bep14 value=\"14\">12th day</option>\n"
"<option $ala4bep15 value=\"15\">Popcorn</option>\n"
"<option $ala4bep16 value=\"16\">YMCA</option>\n"
"</select>\n"
"</div>\n"
"<div class=\"one-line\">&nbsp;</div>\n"
"<div class=\"one-line\">\n"
"<select id=\"ala4lop\" name=\"ala4lop\">\n"
"<option $ala4lop0 value=\"0\">$txt_094</option>\n"
"<option $ala4lop1 value=\"1\">$txt_095</option>\n"
"<option $ala4lop2 value=\"2\">$txt_096</option>\n"
"<option $ala4lop3 value=\"3\">$txt_097</option>\n"
"<option $ala4lop4 value=\"5\">$txt_098</option>\n"
"<option $ala4lop5 value=\"10\">$txt_099</option>\n"
"<option $ala4lop6 value=\"20\">$txt_100</option>\n"
"<option $ala4lop7 value=\"30\">$txt_101</option>\n"
"<option $ala4lop8 value=\"40\">$txt_102</option>\n"
"<option $ala4lop9 value=\"50\">$txt_103</option>\n"
"<option $ala4lop10 value=\"100\">$txt_104</option>\n"
"<option $ala4lop11 value=\"300\">$txt_105</option>\n"
"</select>\n"
"</div>\n"
"</div>\n"
"<p>\n"
"<!-- ######################## ALARM 5 ##################### -->\n"
"<p>\n"
"<div id=\"ala5\">$txt_109<br>\n"
"<div class=\"one-line\" >\n"
"<select id=\"ala5day\" name=\"ala5day\">\n"
"<option $ala5day0 value=\"0\">$txt_080</option>\n"
"<option $ala5day1 value=\"1\">$txt_081</option>\n"
"<option $ala5day2 value=\"2\">$txt_082</option>\n"
"<option $ala5day3 value=\"3\">$txt_083</option>\n"
"<option $ala5day4 value=\"4\">$txt_084</option>\n"
"<option $ala5day5 value=\"5\">$txt_085</option>\n"
"<option $ala5day6 value=\"6\">$txt_086</option>\n"
"<option $ala5day7 value=\"7\">$txt_087</option>\n"
"<option $ala5day8 value=\"8\">$txt_088</option>\n"
"<option $ala5day9 value=\"9\">$txt_089</option>\n"
"<option $ala5day10 value=\"10\">$txt_090</option>\n"
"</select>\n"
"</div>\n"
"<div class=\"one-line\">&nbsp;</div>\n"
"<div class=\"one-line\">\n"
"<select id=\"ala5hr\" name=\"ala5hr\">\n"
"<option $ala5hr0 value=\"0\">0</option>\n"
"<option $ala5hr1 value=\"1\">1</option>\n"
"<option $ala5hr2 value=\"2\">2</option>\n"
"<option $ala5hr3 value=\"3\">3</option>\n"
"<option $ala5hr4 value=\"4\">4</option>\n"
"<option $ala5hr5 value=\"5\">5</option>\n"
"<option $ala5hr6 value=\"6\">6</option>\n"
"<option $ala5hr7 value=\"7\">7</option>\n"
"<option $ala5hr8 value=\"8\">8</option>\n"
"<option $ala5hr9 value=\"9\">9</option>\n"
"<option $ala5hr10 value=\"10\">10</option>\n"
"<option $ala5hr11 value=\"11\">11</option>\n"
"<option $ala5hr12 value=\"12\">12</option>\n"
"<option $ala5hr13 value=\"13\">13</option>\n"
"<option $ala5hr14 value=\"14\">14</option>\n"
"<option $ala5hr15 value=\"15\">15</option>\n"
"<option $ala5hr16 value=\"16\">16</option>\n"
"<option $ala5hr17 value=\"17\">17</option>\n"
"<option $ala5hr18 value=\"18\">18</option>\n"
"<option $ala5hr19 value=\"19\">19</option>\n"
"<option $ala5hr20 value=\"20\">20</option>\n"
"<option $ala5hr21 value=\"21\">21</option>\n"
"<option $ala5hr22 value=\"22\">22</option>\n"
"<option $ala5hr23 value=\"23\">23</option>\n"
"</select>\n"
"</div>\n"
"<div class=\"one-line\">&nbsp;</div>\n"
"<div class=\"one-line\">\n"
"<select id=\"ala5min\" name=\"ala5min\">\n"
"<option $ala5min0 value=\"0\">00</option>\n"
"<option $ala5min1 value=\"5\">05</option>\n"
"<option $ala5min2 value=\"10\">10</option>\n"
"<option $ala5min3 value=\"15\">15</option>\n"
"<option $ala5min4 value=\"20\">20</option>\n"
"<option $ala5min5 value=\"25\">25</option>\n"
"<option $ala5min6 value=\"30\">30</option>\n"
"<option $ala5min7 value=\"35\">35</option>\n"
"<option $ala5min8 value=\"40\">40</option>\n"
"<option $ala5min9 value=\"45\">45</option>\n"
"<option $ala5min10 value=\"50\">50</option>\n"
"<option $ala5min11 value=\"55\">55</option>\n"
"</select>\n"
"</div>\n"
"<div class=\"one-line\">&nbsp;</div>\n"
"<div class=\"one-line\">\n"
"<select id=\"ala5bep\" name=\"ala5bep\">\n"
"<option $ala5bep0 value=\"0\">$txt_091</option>\n"
"<option $ala5bep1 value=\"1\">$txt_092</option>\n"
"<option $ala5bep2 value=\"2\">$txt_093</option>\n"
"<option $ala5bep3 value=\"3\">A-Team</option>\n"
"<option $ala5bep4 value=\"4\">Rich World</option>\n"
"<option $ala5bep5 value=\"5\">Indiana</option>\n"
"<option $ala5bep6 value=\"6\">Staw Wars</option>\n"
"<option $ala5bep7 value=\"7\">Adams Family</option>\n"
"<option $ala5bep8 value=\"8\">Bod</option>\n"
"<option $ala5bep9 value=\"9\">Duel Banjos</option>\n"
"<option $ala5bep10 value=\"10\">Entertainer</option>\n"
"<option $ala5bep11 value=\"11\">G-Busters</option>\n"
"<option $ala5bep12 value=\"12\">Jingle Bells</option>\n"
"<option $ala5bep13 value=\"13\">Knight Rider</option>\n"
"<option $ala5bep14 value=\"14\">12th day</option>\n"
"<option $ala5bep15 value=\"15\">Popcorn</option>\n"
"<option $ala5bep16 value=\"16\">YMCA</option>\n"
"</select>\n"
"</div>\n"
"<div class=\"one-line\">&nbsp;</div>\n"
"<div class=\"one-line\">\n"
"<select id=\"ala5lop\" name=\"ala5lop\">\n"
"<option $ala5lop0 value=\"0\">$txt_094</option>\n"
"<option $ala5lop1 value=\"1\">$txt_095</option>\n"
"<option $ala5lop2 value=\"2\">$txt_096</option>\n"
"<option $ala5lop3 value=\"3\">$txt_097</option>\n"
"<option $ala5lop4 value=\"5\">$txt_098</option>\n"
"<option $ala5lop5 value=\"10\">$txt_099</option>\n"
"<option $ala5lop6 value=\"20\">$txt_100</option>\n"
"<option $ala5lop7 value=\"30\">$txt_101</option>\n"
"<option $ala5lop8 value=\"40\">$txt_102</option>\n"
"<option $ala5lop9 value=\"50\">$txt_103</option>\n"
"<option $ala5lop10 value=\"100\">$txt_104</option>\n"
"<option $ala5lop11 value=\"300\">$txt_105</option>\n"
"</select>\n"
"</div>\n"
"</div>\n"
"<p>\n"
"</div>\n"
"<div class=\"groupHeader\">\n"
"$txt_110\n"
"</div>\n"
"<div>\n"
"<p>\n"
"<label for=\"bushp\">$txt_111</label><br>\n"
"<select id=\"bushp\" name=\"bushp\">\n"
"<option $bushp0 value=\"0\">$txt_112</option>\n"
"<option $bushp1 value=\"1\">$txt_113</option>\n"
"<option $bushp2 value=\"2\">$txt_114</option>\n"
"</select>\n"
"<p>\n"
"<label for=\"bumep\">$txt_115</label><br>\n"
"<select id=\"bumep\" name=\"bumep\">\n"
"<option $bumep0 value=\"0\">$txt_112</option>\n"
"<option $bumep1 value=\"1\">$txt_113</option>\n"
"<option $bumep2 value=\"2\">$txt_114</option>\n"
"</select>\n"
"</div>\n"
"<hr>\n"
"<input type=\"submit\" value=\"$txt_116\">\n"
"</form>\n"
"<div id=\"log\">\n"
"<p/>\n"
"</div>\n"
"</body></html>\n"


;
