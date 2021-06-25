/*  %CopyRight
    ToolsRel: 8.21.04 */

function getScrollX()
{
  return window.scrollX;
}

function getScrollY()
{
  return window.scrollY;
}

function showhide(obj, bShow)
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
    obj.style.display = "block";
  }
  }