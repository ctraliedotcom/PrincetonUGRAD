//  %Copyright
//  ToolsRel: 8.21.04

//% These are used for cross-domain check...
document.props = new Object();
document.props.location = document.location.href;
document.props.domain = document.domain;

function saveWarning(frameName,form,target,url)
{
var changed=null;
var msg = "You have unsaved data on this page.  OK to go back and save or Cancel to continue without saving.";


if (form)
  changed = checkFormChanged(form);

if (changed==null && top.frames && frameName.length>0 )
{
  objFrame = top.frames[frameName];
  if (objFrame)
    changed=checkFrameChanged(objFrame);
}

if ((changed==null) && top.frames)
  changed = checkAnyFrameChanged(top.frames);

var rtn = true;
if (changed && changed=="1")
{
  if (!confirm(msg))
  {
    if (url)
      open(url,target);
    else
      rtn = true;
  }
  else
    rtn = false;
}
else
{
  if (url)
    open(url,target);
  else
    rtn = true;
}

if (url)
  return;
else
  return rtn;

}

//% See if any frame has a change
function checkAnyFrameChanged(frames)
{
for (var j=0; j < frames.length; ++j)
{
  var objFrame = frames[j];
  if (checkFrameChanged(objFrame))
    return true;
  //% check child frames
  if (objFrame.frames)
    if (checkAnyFrameChanged(objFrame.frames))
      return true;
}
}

//% See if any form in the frame is changed
function checkFrameChanged(objFrame)
{
if (isCrossDomain(objFrame))
  return null;

var objForms = objFrame.document.forms;
if (!objForms)
  return null;

var retval = null;
for (var i=0; i < objForms.length; i++)
{
  var change = checkFormChanged(objForms[i]);
  if (change != null)
  {
    if (change)
      return true;
    retval = change;
  }
}
//% null if we found no form with an ICChanged; else false
return retval;
}

//% See if form changed previously or any field changed since
//% loaded.
function checkFormChanged(form)
{
if (!form.ICChanged)
  return null;          //% no change flag
if (form.ICChanged.value == "-1")
  return false;        //% don't check
if (form.ICChanged.value == "1")
  return true;          //% previous change
for (var j = 0; j < form.length; ++j)
{
  if (isChanged(form.elements[j]))
     return true;
}
return false;
}

//% Check if various types of form fields have been changed from initial value
function isChanged(obj)
{
if (obj.type == "checkbox"  || obj.type == "radio")
  return (obj.checked != obj.defaultChecked);
else if (obj.type == "select-one")
  return !(obj.options[obj.selectedIndex].defaultSelected);
else if (obj.type == "select-multiple")
{
  for (var i =0; i < obj.options.length; ++i)
  {
     if (obj.options[i].defaultSelected != obj.options[i].selected)
        return true;
  }
  return false;
}
else if (obj.type == "hidden" || obj.type == "button")
  return false;
else
  return (obj.value != obj.defaultValue);
}


// function to retrieve a cookies value. Takes the cookie name and returns the value
function getCookieValue(cookieNm) {
 var allcookies = document.cookie;
 var cookiename = cookieNm;
 var start = allcookies.indexOf(cookiename + '=');
    if (start == -1) return "-1";   // Cookie not defined for this page.
    start += cookiename.length + 1;  // Skip name and equals sign.
    var end = allcookies.indexOf(';', start);
    if (end == -1) end = allcookies.length;
    var cookieval = allcookies.substring(start, end);

    if (cookieval != null )
        return cookieval;
    else
        return null;

}


function checkExpireTime()  {

  // ICE 650238007 Comment: This function is commented out so that PIA does not depend on 
  // the browser machine time.
  return false;   // return false immediate so that the rest of the function is skipped.

  var cookieval = getCookieValue("PS_TOKENEXPIRE");

    if (cookieval != null )
    {
        cookieval.search(/(\d*)_(\D*)_(\d*)_([^_]*)/); // extracts out the date values
        var day = RegExp.$1;
        var month = RegExp.$2;
        var year = RegExp.$3;
        var time = RegExp.$4.replace(/(,)/g,":");

        cookieval = new Date(month +" " + day +", "+ year + " " + time);

        var startTime = cookieval.valueOf();
        var currTime = new Date();
        var offset = currTime.getTimezoneOffset();
        currTime= currTime.valueOf();
        offset = offset * 60 * 1000;     // cuttent date is not GMT so we need to add the offset

        if( ((currTime+offset)-startTime) > totalTimeoutMilliseconds)
                return true;    // we have exceeded the time between requests
        else
               return false;
    }
}   

var timeoutWin;
var timeoutWarningID;
var timeoutID;

function setupTimeout()  //called from onLoad.  
{
             if (!isSessionLoggedout())
             {
	  if (!checkExpireTime()) { 
 	  	window.clearTimeout(timeoutWarningID);
		window.clearTimeout(timeoutID);
		timeoutID = window.setTimeout('displayTimeoutMsg()', totalTimeoutMilliseconds);	// Setting up final timeout	
		timeoutWarningID = window.setTimeout('displayTimeoutWarningMsg()', warningTimeoutMilliseconds); //warning timeout
	  } else
              	self.location=timeOutURL;	
	}
}

function displayTimeoutMsg() // alert box displayed when session actually timeout.
{
	timeoutMsg =    "\"Your session has been timed out. As a security precaution"
                +" " + totalTimeoutMilliseconds/60000 +" " + "\" minutes of inactivity.\""; 
	self.location=timeOutURL; // goto sign on page		
}


function displayTimeoutWarningMsg() //warning new window.
{
            timeoutWinOption = "toolbar=0,location=0,directories=0,status=0,menubar=0,scrollbars=0,resizable=0,width=330,height=220";
            timeoutWin =window.open( timeoutWarningPageURL, "timeoutWarning", timeoutWinOption, true); 
          timeoutWin.focus(); 
}

function getForm(document,formname)
{
var objForms = document.forms;
if (!objForms)
  return null;

for (var i=0; i < objForms.length; i++)
{
  if (objForms[i].name==formname)
     return objForms[i];
}
return null;
}

function getLoginCookieValue(cookiename)
{
    var allcookies;
    allcookies = document.cookie;

   if (allcookies == ""){ 
          return ""; //no cookie, not return -1 so not to assume page expired
    }
    // Now extract just the named cookie from that list.

    var posBlank = allcookies.indexOf(cookiename + ';'); //older version set value to blank and IE5 doesn't include '=' in the cookie when blank.
    if (posBlank != -1)
       return "-1"; //cookie define, but blank.
    var  start = allcookies.indexOf(cookiename + '=');
    if (start == -1) return "";   // Cookie not defined for this page. Not return -1 so not to assume page expired
    start += cookiename.length + 1;  // Skip name and equals sign.
    //alert(" looking for " + cookiename +"  in  "+ allcookies +"  start = " + start);     
    
    var end = allcookies.indexOf(';', start);
    if (end == -1) end = allcookies.length;
    var cookieval = allcookies.substring(start, end);
    var a = cookieval.split(' ');
    var winhref=window.location.href;
    winhref=winhref.toLowerCase();
   //add / to pathinfo if it is not ended with /.
   if (winhref.indexOf("?")!=-1)
    {
        var pos = winhref.indexOf("?")-1;
        if (winhref.substr(pos,1) != "/")
           winhref = winhref.substr(0,pos+1)+"/"+winhref.substr(pos+1,winhref.length);
    }
    else
    {
        if (winhref.substr(winhref.length-1,1) != "/")
           winhref = winhref+"/";
    }
    for(var i=0; i < a.length; i++)
    {  
        if (a[i].length>0)
        {
        var urlpath = a[i].substring(0,a[i].lastIndexOf("/"))+"/";
        var pshome = a[i].substring(a[i].lastIndexOf("/"),a[i].length)+"/";
        urlpath = urlpath.toLowerCase();
        pshome = pshome.toLowerCase();
        if ((winhref.indexOf(urlpath) !=-1) && (winhref.indexOf(pshome) !=-1))
           return a[i];
        }
    }
    return "-1";
}

function isSessionLoggedout()
{
    var val =  getLoginCookieValue("PS_LOGINLIST");
    if (val=="-1")
    {
       self.location=timeOutURL;
       return true;
    }
return false;
}

function setupTimeoutMac()  //% called from onLoad.
{
window.setTimeout("setupTimeout();", 1000);
}

function isIE() {
var isIE = navigator.appVersion.indexOf("MSIE")>0;
return isIE;
}

function isMAC() {
var isMAC = navigator.appVersion.indexOf("Mac")>0;
return isMAC;
}