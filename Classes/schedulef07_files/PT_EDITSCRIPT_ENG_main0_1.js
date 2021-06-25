/*  %Copyright
    ToolsRel: 8.21.04 */

var oErr_main0 = null;

function PSwarning_main0(msg)
{
msg = PSmessageSubst_main0(msg, PSwarning_main0.arguments, 1);
alert(msg);
}

function PSdeferWarning_main0(msg)
{
sDefMsg_main0 = PSmessageSubst_main0(msg, PSdeferWarning_main0.arguments, 1);
bDefWarning_main0 = true;
}

function PSerror_main0(obj, msg)
{
obj.PSsaveclass = obj.className;
obj.className = "PSERROR";
msg = PSmessageSubst_main0(msg, PSerror_main0.arguments, 2);
alert(msg);
//% Remember the object so we can set focus later (usually from OnFocus processing).
//% Can't do it here because of browser bugs and incompatibilities.
oErr_main0=obj;
}

function PSdeferError_main0(obj, msg)
{
sDefMsg_main0 = PSmessageSubst_main0(msg, PSdeferError_main0.arguments, 2);
oDefErr_main0=obj;
bDefWarning_main0 = false;
}

function PSshowDeferredMsg_main0()
{
if (sDefMsg_main0=="")return;
if (bDefWarning_main0)
   PSwarnng_main0(sDefMsg_main0);
else
   PSerror_main0(oDefErr_main0, sDefMsg_main0);

sDefMsg_main0 = "";
oDefErr_main0 = null;
}

function PSclearError_main0(obj)
{
if (obj == oDefErr_main0)
{
  sDefMsg_main0 = "";
  oDefErr_main0 = null;
}
}

function PSmessageSubst_main0(msg, parms, startparm)
{
var pos;
var res = "";
var startpos = 0;
var nparms = parms.length - startparm;

if (nparms == 0)
  return msg;

while (startpos < msg.length)
{
  pos = msg.indexOf("%", startpos);
  if (pos == -1 || pos == msg.length - 1)
  {
    res += msg.substring(startpos);
    return res;
  }
  res += msg.substring(startpos, pos);

  ++pos;
  if (msg.charAt[pos] == "%")
  {
    res += "%";
    ++pos;
    startpos = pos;
    continue;
  }

  var numstr = "";

  while (pos < msg.length)
  {
    var c = msg.charAt(pos);
    if ("0123456789".indexOf(c) == -1)
      break;
    numstr += c;
    ++pos;
  }

  startpos = pos;

  if (numstr.length == 0)
    continue;

  var num = parseInt(numstr);
  if (num > nparms)
    continue;

  res += parms[startparm + num - 1];
}

return res;
}

function FindString_main0(obj)
{
var s = prompt("Enter search string:","");
if (s == null || s == "")
  return false;
obj.value = s;
return true;
}

function DeleteCheck_main0(form,name)
{
//% MSG_SETPNLRT,  MSG_PNLRTDELETECONF
if(confirm("Delete current/selected rows from this page?  The delete will occur when the transaction is saved."))
  submitAction_main0(document,form.name,name);
}

function DeleteCheck2_main0()
{
return confirm("Delete current/selected rows from this page?  The delete will occur when the transaction is saved.");
}

function AddMultiple_main0(obj)
{
var msg = "Enter number of rows to add:";
var defreply  = "1";
while(true)
{
  var s = prompt(msg,defreply);
  if (s == null || s == "")
    return false;
  if (s.search(/^ *(\d*) *$/) == 0)
  {
    var n = parseInt(RegExp.$1);
    if (n > 0 && n <= 100)
    {
      obj.value = RegExp.$1;
      return true;
    }
  }
  msg = "Invalid value.  Please enter a value between 1 and 100."
        + "\n" + "Enter number of rows to add:";
  defreply = RegExp.$1;
}
}

//% Variable used to remember objects etc. between handlers.  In several situations
//% we have to begin processing in one event handler and finish in another.
var oChange_main0 = null;
var oDefErr_main0 = null;
var sDefMsg_main0 = "";
var bDefWarn_main0;
var oKillFocus_main0 = null;
var oTargetFocus_main0 = null;

function moveFocus_main0(killobj)
{
//% Complicated because of kludges needed to avoid rerunning event handlers
//% on some platforms
if (oErr_main0!=null)
{
  oKillFocus_main0 = killobj;
  oTargetFocus_main0 = oErr_main0;
  oErr_main0.focus();
  oErr_main0 = null;
}
}

function doFocus_main0(obj,bTabonly,bDef)
{

if (obj==oTargetFocus_main0)
{
  oTargetFocus_main0 = null;
  oKillFocus_main0 = null;
  return;
}

//% if we have a calendar control active, close it...
if (typeof dateBoxOpen != "undefined" && dateBoxOpen)
   closeCal2();

if(!bTabonly || getLastKey_main0() == "\t".charCodeAt(0))
{
  if(oChange_main0!=null && oChange_main0!=obj)
  {
    oChange_main0.form.ICFocus.value=obj.name;
    submitAction_main0(document,oChange_main0.form.name,oChange_main0.name);
    oChange_main0=null;  
}
  else if(bDef && oDefErr_main0!=null && oDefErr_main0!=obj)
    PSshowDeferredMsg_main0();
}
moveFocus_main0(obj);
}

function doFocusMac_main0(obj,bTabonly)
{
if (obj==oTargetFocus_main0)
{
  oTartgetFocus_main0 = null;
  oKillFocus_main0 = null;
  return;
}

//% if we have a calendar control active, close it...
if (typeof dateBoxOpen != "undefined" && dateBoxOpen)
   closeCal2();

document.main0.ICFocus.value = obj.name;
if(oDefErr_main0!=null && oDefErr_main0!=obj)
  PSshowDeferredMsg_main0();

moveFocus_main0(obj);

}

function doReqField_main0(obj, bserver, bnumeric)
{
//% Ignore if field losing focus is oKillFocus: browser bug workaround
if (oKillFocus_main0==obj)
  return;

var val;
if(obj.PSsaveclass) obj.className = obj.PSsaveclass;
if(obj.type == "select-one")
  val = obj.options[obj.selectedIndex].value;
else
  val = obj.value;
if (val.search(/^ *$/) == 0 || (bnumeric && val.search(/^[ 0]*$/) == 0))
{
  if (bserver)
    oChange_main0=obj;
  else
    PSdeferError(obj, "Field is required.");  //% MSG_SETPPR, MSG_PPRREQUIRED
}
}

function doEdits_main0(obj,sType,bHist,bEffdtSub,bDate,bYN,bUCase,bImmed, re)
{
//% Process deferred message if any
if(sDefMsg_main0!="")
{
  PSshowDeferredMsg_main0();
  return false;
}

//% Change test... do a submit instead of local edits if we have pending server action from previous field...
if(oChange_main0 != null)
{
  submitAction_main0(document,obj.form.name,obj.name);
  return true;
}

if(obj.PSsaveclass) obj.className = obj.PSsaveclass;

if(bUCase=='Y')
 {
 var temp = obj;
 if(obj.type == "select-one") temp = obj.options[obj.selectedIndex];
 temp.value=temp.value.toUpperCase();
 }

var bErr = false;

if(sType!='') bErr = checkType_main0(obj, sType, re);
if(!bErr && bHist=='Y') bErr = historyUpdated_main0(obj);
if(!bErr && bEffdtSub=='Y') bErr =subscrollUpdated_main0(obj);
if(!bErr && bDate=='Y') bErr = isReasonableDate_main0(obj);
if(!bErr && bYN=='Y') bErr = isYN_main0(obj);

if (bErr && bImmed=='Y') PSshowDeferredMsg_main0();

//% This function now returns true, which doesn't suppress the value change
//% (necessary so we can get out to exit the page etc..)
return true;
}

//% NOTE: these scripts return TRUE if the edit fails.  FALSE
//% if the test is passed!

function historyUpdated_main0(obj)
{
if(obj.type == "select-one")
{
  obj = obj.options[obj.selectedIndex];
  if (obj.defaultSelected)
    return false;
}
else if(obj.value==obj.defaultValue)
   return false;

//% MSG_SETPPR, MSG_PPRNOTFUTURE
PSdeferError_main0(obj, "Cannot change current or history records unless in Correction mode.");
return true;
}

function subscrollUpdated_main0(obj)
{
if(obj.type == "select-one")
  {
  obj = obj.options[obj.selectedIndex];
  if (obj.defaultSelected)
    return false;
  }
else if(obj.value==obj.defaultValue)
  return false;

//% MSG_SETPPR, MSG_PPRSUBKEYEFFDT
PSdeferError_main0(obj, "Cannot change key field with effective dated subordinate data unless in Correction mode.");
return true;
}

//% Reasonable date assumes it has already passed a valid date test
//% Not used currently (server edit)
function isReasonableDate_main0(obj)
{
if(obj.type == "select-one")  obj = obj.options[obj.selectedIndex];
var dateval = new Date(obj.value);
var now = new Date();
var diff = new Date(Math.abs(now.getTime() - dateval.getTime()));
if (diff.getMonth > 0)
  //% MSG_SETPPR, MSG_PPRDATERANGE
  PSdeferWarning_main0("Warning -- date out of range.");
return false;
}

//% ajb what about null value here?
function isYN_main0(obj)
{
var val;
if(obj.type == "select-one")
  val = obj.options[obj.selectedIndex].value;
else
  val = obj.value;
if (val != 'Y' && val != 'N')
{
  //% MSG_SETPPR, MSG_PPRYESNOERR
  PSdeferError_main0(obj, "Value must either be Y for Yes or N for No.");
  return true;
}
return false;
}

//% Typecheck scripts
function isNumOnly_main0(obj)
{
var val = obj.value;
if (val == '') return false;

if (val.search(/^ *\d* *$/) != 0)
{
  //% MSG_SETPPR, MSG_PPRFORMATERRNUMONLY
  PSdeferError_main0(obj, "Numeric Only field format error.  Please reenter.");
  return true;
}
return false;
}

function isNumeric_main0(obj,fmt,re)
{
var val = obj.value;
if (val == '') return false;

if (val.search(re) != 0)
{
  //% MSG_SETPPR, MSG_PPRFORMATERRNUM
  PSdeferError_main0(obj, "Number field format error.  The correct format for this field is %1.", fmt);
  return true;
}
return false;
}

function isTime_main0(obj,fmt)
{
var val = obj.value;
if (val == '') return false;

if (!checkTime(val,fmt))
{
  //% MSG_SETPPR, MSG_PPRFORMATERRTIME
  PSdeferError_main0(obj, "Time field format error.  Please re-enter using the proper format.");
  return true;
}
return false;
}

function isDateTime_main0(obj,fmt)
{
var val = obj.value;
if (val == '') return false;

if (!checkDateTime(val,fmt))
{
  //% MSG_SETPPR, MSG_PPRFORMATERRDATETIME
  PSdeferError_main0(obj, "Datetime field format error.  Please reenter using the proper format.");
  return true;
}
return false;
}

function isDate_main0(obj,fmt)
{
var val = obj.value;
if (val == '') return false;

var dt = getDate(val,fmt);

if (dt==null)
{
  PSdeferError_main0(obj, "Invalid date.");  //% MSG_SETTLS, MSG_TLSINVDATE
  return true;
}
obj.value = formatDate(dt, fmt);
return false;
}

function checkType_main0(obj,sType,re)
{
var cType = sType.substring(0, 1);
var sFmt = sType.substring(1);
switch(cType)
{
case 'N': return isNumOnly_main0(obj);
case 'S': return isNumeric_main0(obj,sFmt,re);
case 'D': return isDate_main0(obj,sFmt);
case 'T': return isTime_main0(obj,sFmt);
case 'X': return isDateTime_main0(obj,sFmt);
}
return false;
}

function getDateRegExpStr(fmt)
{
// Get re for matching the given date format
var dmy = fmt.substring(0, 3);
var sep = fmt.substring(3, 4);
var sepchars = "-/.";
var sepstr = "\\-\/\\.";
if (sepchars.indexOf(sep)==-1)
   sepstr += sep;     // assume not signficant to re
var restr;

if (dmy == "DMY" || dmy == "MDY")
{
  restr = "(\\d{1,2})[";
  restr += sepstr;
  restr += "]?(\\d{1,2})[";
  restr += sepstr;
  restr += "]?(\\d{4}|\\d{2})";
}
else  // YMD
{
  restr = "(\\d{4}|\\d{2})[";
  restr += sepstr;
  restr += "]?(\\d{1,2})[";
  restr += sepstr;
  restr += "]?(\\d{1,2})";
}
return restr;
}

function getTimeRegExpStr(fmt)
{
var restr;

// Accept : or . as separator...
restr = "(\\d{1,2})([:\\.]?(\\d{1,2})([:\\.]?(\\d{1,2})(\\.\\d*)?)?)?";

// AM/PM spec?  Just accept any word chars here -- optional preceding blanks
if (fmt.length != 0)
  restr += " *(\\w*)"

return restr;
}

// Get date object.. validity check everything
function getDate(val,fmt)
{
var re = new  RegExp("^ *" + getDateRegExpStr(fmt) + " *$");
var parts= re.exec(val);
if (parts==null)
  return null;

var dmy = fmt.substring(0, 3);
var defyr;
if (fmt.length >=7)
  defyr = parseInt(fmt.substring(5), 10);
else
  defyr = 50;
var d, m, y;
if (dmy == "DMY")
{
  d = parseInt(parts[1],10);
  m = parseInt(parts[2],10);
  y = parseInt(parts[3],10);
}
else if  (dmy == "MDY")
{
  m = parseInt(parts[1],10);
  d = parseInt(parts[2],10);
  y = parseInt(parts[3],10);
}
else
{
  y = parseInt(parts[1],10);
  m = parseInt(parts[2],10);
  d = parseInt(parts[3],10);
}
if (y<100)
{
  // need to add a century
  if (y<defyr)
    y += 2000;
  else
    y += 1900;
}
--m;

var dt = new Date(y,m,d);

if (dt.getFullYear()!=y||dt.getMonth()!=m||dt.getDate()!=d)
  return null;
return dt;
}

function checkTime(val,fmt)
{
var re = new  RegExp("^ *" + getTimeRegExpStr(fmt) + " *$");
var parts =  re.exec(val);
if (parts==null)
  return false;

var h, m, s, ampm;
h = parseInt(parts[1],10);
if (parts.length>3)
  m = parseInt(parts[3],10);
else
  m = 0;
if (parts.length>5)
  s = parseInt(parts[5],10);
else
  s = 0;
if (parts.length>7)
  ampm = parts[7].toUpperCase();
else
  ampm = "";

if (fmt.length>0 && ampm.length>0)
{
  var i =fmt.indexOf(";");
  var am, pm;
  if (i >0)
  {
    am=fmt.substring(0,i);
    pm=fmt.substring(i+1);
    if (pm.indexOf(ampm)==0)
      h +=12;
    else if (am.indexOf(ampm)!=0)
      return false;
    --h;
   }
}
if (h<0||h>23||m>59||s>59)
  return false;
return true;

}

function checkDateTime(val,fmt)
{
var re = /^ *([^ ]*) +([^ ]*) *$/;
var parts=re.exec(val);
if (parts==null)
  return "";
var dt=parts[1];
var tm=parts[2];
var dtfmt = fmt.substring(0,4);
var tmfmt = fmt.substring(4);

var dtobj = getDate(dt,dtfmt);
if (dtobj == null)
  return "";
if (!checkTime(tm, tmfmt))
  return "";
return formatDate(dtobj, dtfmt) + " " + tm;
}

function formatDate(dt, dateFormat)
{
var format = dateFormat.substring(0,3);
var separator = dateFormat.substring(3,4);
var day = dt.getDate();
var mnt = dt.getMonth() + 1;
var yr = dt.getFullYear();

if (dateFormat.length>=7)
{
  if (dateFormat.substring(4,5) == "2")
  {
    var yrdef = parseInt(dateFormat.substring(5,7), 10);
    var yr2 = yr%100;
    if (yr2 < yrdef)
    {  //% default 2000
      if (yr2 + 2000 == yr)
        yr = yr2;
    }
    else
    {  //% default 1900
      if (yr2 + 1900 == yr)
        yr = yr2;
    }
  }
}

var formattedDate;
if (format == 'MDY')
  formattedDate = '' + padout(mnt) + separator + padout(day) + separator + padout(yr);
else if (format == 'DMY')
  formattedDate = '' + padout(day) + separator +padout(mnt) + separator  + padout(yr);
else if (format == 'YMD')
  formattedDate = '' + padout(yr) + separator + padout(mnt) + separator + padout(day);

//%alert('Date is = ' + formattedDate);

return formattedDate;
}

function padout(number)
{
return (number < 10) ? '0' + number : number;
}