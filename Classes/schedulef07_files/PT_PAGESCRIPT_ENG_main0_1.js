/*  %CopyRight
    ToolsRel: 8.21.04 */

//%  Define showhide if we haven't already
if (window.showhide == null)
{
  window.showhide = function (obj, bShow)
  {
  if (obj == null)
     return;
  if (bShow)
  {
    var y = getScrollY();
    if (y != 0)
      obj.style.top = y;
    obj.style.visibility = "visible";
    obj.style.display = "block";
  }
  else
  {
    obj.style.visibility = "hidden";
    obj.style.display = "none";
  }
  }
}

function processing_main0(opt, waittime)
{
var waitobj = document.getElementById("WAIT_main0");
var saveobj = document.getElementById("SAVED_main0");

if (opt == 0)
{
  showhide(waitobj, false);
  showhide(saveobj, false);
  return;
}

if (opt == 1)
{
   showhide(waitobj, false); 
   showhide(saveobj, false);
   showhide(waitobj, true);
   return;
}

if (opt == 2)
{
   showhide(waitobj, false);
   showhide(saveobj, true);
   setTimeout("processing_main0(0);",waittime);
}
}

//% key event handling
function isAltKey(evt)
{
if (!evt && window.event)
  evt = window.event;
if (!evt)
  return false;

if (evt.altKey)
  return true;
if (evt.modifiers)
  return (evt.modifiers & Event.ALT_MASK) != 0;
return false;
}

function isCtrlKey(evt)
{
if (!evt && window.event)
  evt = window.event;
if (!evt)
  return false;

if (evt.ctrlKey)
  return true;
if (evt.modifiers)
  return (evt.modifiers & Event.CONTROL_MASK) != 0;
return false;
}

function isShiftKey(evt)
{
if (!evt && window.event)
  evt = window.event;
if (!evt)
  return false;

if (evt.shiftKey)
  return true;
if (evt.modifiers)
  return (evt.modifiers & Event.SHIFT_MASK) != 0;
return false;
}

function getKeyCode(evt)
{
if (!evt && window.event)
  evt = window.event;
if (!evt)
  return 0;
if (evt.keyCode)
  return evt.keyCode;
if (evt.which)
  return evt.which;
return 0;
}

function cancelBubble(evt)
{
if (!evt && window.event)
  evt = window.event;
if (!evt)
  return;
if (typeof evt.cancelBubble != "undefined")
  evt.cancelBubble = true;
if (typeof evt.preventBubble != "undefined" && evt.preventBubble)  //% ie4 gives wrong result for typeof
  evt.preventBubble();
}

function isPromptKey(evt)
{
if (isAltKey(evt) && getKeyCode(evt) == "5".charCodeAt(0))
{
  cancelBubble(evt);   //% if processed don't pass on (IE/NS6)
  return true;
}
return false;
}

function getEventTarget(evt)
{
if (!evt && window.event)
  evt = window.event;
if (!evt)
  return null;
if (evt.srcElement)
  return evt.srcElement;
if (evt.target)
  return evt.target;
return null;
}


function getModifiers(evt)
{
var res = "";
if (isAltKey(evt))
  res += "A";
if (isCtrlKey(evt))
  res += "C";
if (isShiftKey(evt))
  res += "S";
return res;
}


var nLastKey_main0 = 0;

function doKeyDown_main0(evt)
{
nLastKey_main0 = getKeyCode(evt);

//% Tab processing...
if (nLastKey_main0 != "\t".charCodeAt(0))
  return true;

if (isCtrlKey(evt))
  return tryFocus(oFirstTab_main0);

if (isAltKey(evt) || isShiftKey(evt))
  return true;

var obj = getEventTarget(evt);

if (obj == oLastTab_main0)
{
  if (oFirstTB_main0)
  {
    if (!bTabOverTB_main0)
      return true;
  }
  if (oFirstPg_main0)
  {
    if (!bTabOverPg_main0)
       return tryFocus(oFirstPg_main0);
  }
  if (bTabOverNonPS_main0 || oFirstTB_main0 != null || oFirstPg_main0 != null)
    return tryFocus(oFirstTab_main0);
  return true;

}

if (obj == oLastTB_main0)
{
  if (oFirstPg_main0)
  {
    if (!bTabOverPg_main0)
       return tryFocus(oFirstPg_main0);
  }
  if (bTabOverNonPS_main0 || oFirstPg_main0 != null)
    return tryFocus(oFirstTab_main0);
  return true;
}

if (obj == oLastPg_main0)
{
  if (bTabOverNonPS_main0)
    return tryFocus(oFirstTab_main0);
  return true;
}

}

function getLastKey_main0()
{
var nTemp = nLastKey_main0;
nLastKey_main0 = 0;
return nTemp;
}

function doKeyUp_main0(evt)
{
var key = getKeyCode(evt);
var keyChar = String.fromCharCode(key);

//% Enter key is done in keypress, to avoid IME problems
if (keyChar == "\r" || key == 27)
  return true;

//% Netcape 4 routing..
if (!routeKeyEvent(evt))
  return false;

//% Check if needed.   Currently assumes no compound modifiers (ctrl-shift-x)
if (isAltKey(evt))
{
  if (isCtrlKey(evt) || altKey_main0.indexOf(keyChar) < 0)
    return true;
}
else if (isCtrlKey(evt))
{
  //% NS converts to ctrl instead of passing base key:  convert back
  key |= 0x40;
  keyChar = String.fromCharCode(key);
  if (ctrlKey_main0.indexOf(keyChar) < 0)
    return true;
}
else
{
  if (baseKey_main0.indexOf(keyChar) < 0)
    return true;
}

var target = getEventTarget(evt);
var code = getModifiers(evt) + keyChar;

if (target && target.name)
  document.main0.ICFocus.value = target.name;

//% If it is delete, check if valid,  confirm.
if (code == "A8")
   if (!doDeleteKey_main0(target))
      return false;

//% If it is find key (alt/) process here...
if (code == "A\xbf")
{
  if (window.FindString_main0
      && document.main0.ICFind
      && findScroll(target))
  {
    if (!FindString_main0(document.main0.ICFind))
      return false;
  }
  else
    return false
}

//% insert key (alt-7) process here to check if allowed for current field
//% and if multiple or single.
if (code == "A7")
   if (!doInsertKey_main0(target))
      return false;


 	submitAction_main0(document,'main0', "#KEY" + code);
return false;
}

//% Delete key processing.   Need to determine if valid, and get confirmation if
//% it is.
function doDeleteKey_main0(obj)
{
if (!window.DeleteCheck2_main0)
  return false;

var scroll = findScroll(obj);
if (!scroll)
   return false;                        //% not on a scroll

//% See if there is a delete button on the footer.  If so, delete is valid for
//% any field. (Generate id of delete button from id of table header).
var buttonid = scroll.id.replace(/\$scroll[im]?\$/,"\$fdelete\$")
if (document.getElementById(buttonid))
  return DeleteCheck2_main0();

//% See if we have a delete button for the row containing the focus.
//% Name is either name$occ or name$occ$$occ -- want the first
if (obj.name.search(/\$(\d*)(\$\$\d*)?$/) < 0)
  return false;         //% can't interpret name
var row = RegExp.$1;    //% get row

//% id for delete button on this row...
if (buttonid.search(/^(.*)(\$fdelete\$)(.*)$/) < 0)
  return false;
buttonid = RegExp["$1"] + "$delete$" + row + "$$" + RegExp["$3"];

if (document.getElementById(buttonid))
  return DeleteCheck2_main0();

return false;

}

//% insert key processing.   Find the <table> tag for the scroll, and
//% check if allowed and if multiple
function doInsertKey_main0(obj)
{
if (!window.AddMultiple_main0)
  return false;

var scroll = findScroll(obj);
if (!scroll)
   return false;                        //% not on a scroll

if (scroll.id.match(/\$scrollm\$/))     //% multiple insert
  return AddMultiple_main0(document.main0.ICAddCount);

if (scroll.id.match(/\$scrolli\$/))     //% single insert
   return true;

//% no insert allowed
return false;

}

//% Look up the parent chain for an element with an id that indicates it is
//% a scroll.
function findScroll(obj)
{
while (obj)
{
  if (typeof obj.id  != "undefined")
     if (obj.id.match(/\$scroll/))
        return obj;
  if (typeof obj.parentNode != "undefined")
    obj = obj.parentNode;
  else   //% IE 4...
  if (typeof obj.offsetParent != "undefined")
    obj = obj.offsetParent;
  else
    return null;
}
}

//% define keypress handler if not already defined.
//% Note for IE 4.5 there is a different version
if (window.doKeyPress_main0 == null)
{
  window.doKeyPress_main0 = function (evt)
  {
  var key = getKeyCode(evt);
  var keyChar = String.fromCharCode(key);

  //% Currently used only for enter key (to avoid IME problems) -- change this
  //% if need to process others also
  if (keyChar != "\r" && key != 27)
    return true;

  //% Netcape 4 routing..
  if (!routeKeyEvent(evt))
    return false;

  //% If it is return key and target field has onclick or href, assume
  //% normal processing
  var target = getEventTarget(evt);

  if (target && key != 27)
  {
    if (typeof target.onclick == "function" || target.href)
      return true;
    if (target.type == "textarea")
      return true;
  }

  var code = getModifiers(evt) + keyChar;

  if (target && target.name)
    document.main0.ICFocus.value = target.name;

	submitAction_main0(document,'main0', "#KEY" + code);
  return false;
  }
}

var oFirstTab_main0 = null;
var oLastTab_main0 = null;
var oFirstTB_main0 = null;
var oLastTB_main0 = null;
var oFirstPg_main0 = null;
var oLastPg_main0 = null;

var nFirstTBIndex = 5000;        //% must agree with page generator
var nFirstPgIndex = 10000;

function checkTabIndex(obj)
{
if (obj.tabIndex && obj.tabIndex >= 0)
{
  if (obj.tabIndex < nFirstTBIndex)
  {
    if (oLastTab_main0 == null || obj.tabIndex > oLastTab_main0.tabIndex)
        oLastTab_main0 = obj;
    if (oFirstTab_main0 == null || obj.tabIndex < oFirstTab_main0.tabIndex)
        oFirstTab_main0 = obj;
  }
  else if (obj.tabIndex < nFirstPgIndex)
  {
    if (oLastTB_main0 == null || obj.tabIndex > oLastTB_main0.tabIndex)
        oLastTB_main0 = obj;
    if (oFirstTB_main0 == null || obj.tabIndex < oFirstTB_main0.tabIndex)
        oFirstTB_main0 = obj;
  }
  else
  {
    if (oLastPg_main0 == null || obj.tabIndex > oLastPg_main0.tabIndex)
        oLastPg_main0 = obj;
    if (oFirstPg_main0 == null || obj.tabIndex < oFirstPg_main0.tabIndex)
        oFirstPg_main0 = obj;
  }
}
}

function setEventHandlers_main0(sFirst, sLast, bMac)
{
var focus1, focus2;
if (bMac)
{
  focus1 = function (evt) {doFocusMac_main0(this, true, true);};
  focus2 = function (evt) {doFocusMac_main0(this, false, true);};
}
else
{
  focus1 = function (evt) {doFocus_main0(this, true, true);};
  focus2 = function (evt) {doFocus_main0(this, false, true);};
}

var i;

if (sFirst!="")
{
  for (i = 0; i < document.anchors.length; ++i)
  {
    if (document.anchors[i].name == sFirst) break;
  }
  for (++i; i < document.anchors.length; ++i)
  {
    var anc =  document.anchors[i];
    if (anc.name == sLast) break;
    checkTabIndex(anc);
    //% following test doesn't work with Netscape
    if  (typeof anc.onfocus != "function")
        document.anchors[i].onfocus = focus1;
  }
}
var frm = document.main0;
if (!frm) return;
for (i = 0; i < frm.length; ++i)
{
  var frmi  =  frm[i];
  if (frmi.type=="hidden") continue;
  checkTabIndex(frmi);
  if (typeof frmi.onfocus != "function")
  {
    if (typeof frmi.onclick == "function")
      frmi.onfocus = focus1;
    else
      frmi.onfocus = focus2;
  }
}

}

//%  define a couple of functions if we haven't already
if (window.setKeyEventHandler_main0 == null)
{
  window.setKeyEventHandler_main0 = function ()
    {
    if (typeof(baseKey_main0) != "undefined")
    {
      document.onkeyup = doKeyUp_main0;
      if (baseKey_main0.indexOf("\r") >= 0 || baseKey_main0.indexOf("\x1b") >= 0)
         document.onkeypress = doKeyPress_main0;
    }
    document.onkeydown = doKeyDown_main0;
    }
}

if (window.routeKeyEvent == null)
{
  window.routeKeyEvent = function(evt)
    {
    return true;
    }
}

function setFocus_main0(fldname, indx)
{
var obj = null;
if (document.main0)
{
  obj = document.main0.elements[fldname];
  if (!obj)
     obj = document.main0[fldname];
}
if (!obj)
    obj = document.getElementById(fldname);
if (!obj) return;
if (indx >= 0 && obj.length && indx < obj.length)
    obj = obj[indx];
tryFocus(obj);
}

function tryFocus(obj)
{
if (obj && typeof obj.focus != "undefined" && !obj.disabled)
{
   obj.focus();
   return false;
}
return true;
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