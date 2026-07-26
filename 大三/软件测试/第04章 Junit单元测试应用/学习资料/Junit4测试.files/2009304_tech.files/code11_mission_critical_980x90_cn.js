
var mp_swver = 0;
var mp_html = "";
if( navigator.mimeTypes && navigator.mimeTypes["application/x-shockwave-flash"] && navigator.mimeTypes["application/x-shockwave-flash"].enabledPlugin ) {
  if( navigator.plugins && navigator.plugins["Shockwave Flash"] ) {
    mp_swver = (navigator.plugins["Shockwave Flash"].description.split( " " ))[2];
  }
} else if ( navigator.userAgent && navigator.userAgent.indexOf("MSIE") >= 0 && ( navigator.userAgent.indexOf("Windows") >= 0 ) ) {
var mp_axo;
var e;
  for( var mp_i = 11; mp_i > 6; mp_i-- ) {
    try {
    mp_axo = new ActiveXObject("ShockwaveFlash.ShockwaveFlash." + mp_i );
    mp_swver = mp_i;
    break;
    } catch (e) {}
  }
}
if( mp_swver >= 6 ) {
  mp_html =  "<OBJECT classid=\"clsid:D27CDB6E-AE6D-11cf-96B8-444553540000\"";
  mp_html += " codebase=\"\" id=\"10013/70149/code11_mission_critical_980x90_cn.\" NAME=\"movie3115115\" WIDTH=\"980\" HEIGHT=\"90\">";
  if( mp_swver > 5 ) {
    mp_html += "<PARAM NAME=FlashVars VALUE=\"clickTAG=http://altfarm.mediaplex.com/ad/ck/10013-70149-21352-3?mpt=%5bCACHEBUSTER%5d\">";
    mp_html += "<PARAM NAME=movie VALUE=\"http://img-cdn.mediaplex.com/0/10013/70149/code11_mission_critical_980x90_cn.swf\">";
    mp_html += "<PARAM NAME=wmode VALUE=\"opaque\">";
  } else
    mp_html += "<PARAM NAME=movie VALUE=\"http://img-cdn.mediaplex.com/0/10013/70149/code11_mission_critical_980x90_cn.swf?clickTAG=http://altfarm.mediaplex.com/ad/ck/10013-70149-21352-3?mpt=%5bCACHEBUSTER%5d\">";
  mp_html += "<PARAM NAME=wmode VALUE=\"opaque\">";
  if( mp_swver > 5 )
    mp_html += "<EMBED wmode=\"opaque\" NAME=\"10013/70149/code11_mission_critical_980x90_cn.\" src=\"http://img-cdn.mediaplex.com/0/10013/70149/code11_mission_critical_980x90_cn.swf\" FlashVars=\"clickTAG=http://altfarm.mediaplex.com/ad/ck/10013-70149-21352-3?mpt=%5bCACHEBUSTER%5d\"";
  else
    mp_html += "<EMBED wmode=\"opaque\" NAME=\"10013/70149/code11_mission_critical_980x90_cn.\" src=\"http://img-cdn.mediaplex.com/0/10013/70149/code11_mission_critical_980x90_cn.swf?clickTAG=http://altfarm.mediaplex.com/ad/ck/10013-70149-21352-3?mpt=%5bCACHEBUSTER%5d\"";
  mp_html += " swLiveConnect=\"FALSE\" WIDTH=\"980\" HEIGHT=\"90\" TYPE=\"application/x-shockwave-flash\" PLUGINSPAGE=\"\">";
  mp_html += "</EMBED>";
  mp_html += "</OBJECT>";
  if( window.DocumentWrite )
    DocumentWrite( mp_html );
  else
    document.write( mp_html );
} else if( !( navigator.appName && navigator.appName.indexOf("Netscape") >= 0 && navigator.appVersion.indexOf("2.") >= 0 ) ) {
  document.write("<a href=\"http://altfarm.mediaplex.com/ad/ck/10013-70149-21352-3?mpt=[CACHEBUSTER]\" TARGET=\"_blank\">");
  document.write("<IMG SRC=\"http://img-cdn.mediaplex.com/0/10013/70149/code11_mission_critical_980x90_cn.gif\" WIDTH=\"980\" HEIGHT=\"90\" BORDER=0></a>");
}
//-->

