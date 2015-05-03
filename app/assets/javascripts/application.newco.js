// $Id$
// $(c)$
//

function commafy(nStr) {
  nStr += '';
  x = nStr.split('.');
  x1 = x[0];
  x2 = x.length > 1 ? '.' + x[1] : '';
  var rgx = /(\d+)(\d{3})/;
  while (rgx.test(x1)) {
    x1 = x1.replace(rgx, '$1' + ',' + '$2');
  }
  return x1 + x2;
}

function dateify(n) {
  var d = new Date();
  d.setTime(n);
  return (d.getMonth() + 1) + '/' + d.getDate() + '/' + d.getFullYear();
}

function mergeArray(array1, array2) {
  a_out = {};
  for (item in array1) {
    a_out[item] = array1[item];
  }
  for (item in array2) {
    a_out[item] = array2[item];
  }
  return a_out;
}

//
// addLoadEvent()
// Adds event to window.onload without overwriting currently assigned onload
// functions.
// Function found at Simon Willison's weblog - http://simon.incutio.com/
//
function addLoadEvent(func) {
  var oldonload = window.onload;
  if (typeof window.onload != 'function') {
    window.onload = func;
  }

  else {
    window.onload = function() {
      oldonload();
      func();
    }
  }
}

// found at
// http://stackoverflow.com/questions/3191179/generate-xml-document-in-memory-with-javascript
// and http://www.w3schools.com/Xml/xml_parser.asp

function createXmlDocument(string) {
  var doc;
  if (window.DOMParser) {
    // parser = new DOMParser();
    // doc = parser.parseFromString(string, "text/xml");
    doc = string;
  } else // Internet Explorer
  {
    doc = new ActiveXObject("Microsoft.XMLDOM");
    doc.async = "false";
    doc.loadXML(string);
  }
  return doc;
}

// �From http://phpjs.org/functions/strip_tags/

function strip_tags(input, allowed) {
  // http://kevin.vanzonneveld.net
  // * example 1: strip_tags('<p>Kevin</p> <br /><b>van</b> <i>Zonneveld</i>',
  // '<i><b>');
  // * returns 1: 'Kevin <b>van</b> <i>Zonneveld</i>'
  // * example 2: strip_tags('<p>Kevin <img src="someimage.png"
  // onmouseover="someFunction()">van <i>Zonneveld</i></p>', '<p>');
  // * returns 2: '<p>Kevin van Zonneveld</p>'
  // * example 3: strip_tags("<a href='http://kevin.vanzonneveld.net'>Kevin
  // van Zonneveld</a>", "<a>");
  // * returns 3: '<a href='http://kevin.vanzonneveld.net'>Kevin van
  // Zonneveld</a>'
  // * example 4: strip_tags('1 < 5 5 > 1');
  // * returns 4: '1 < 5 5 > 1'
  // * example 5: strip_tags('1 <br/> 1');
  // * returns 5: '1 1'
  // * example 6: strip_tags('1 <br/> 1', '<br>');
  // * returns 6: '1 1'
  // * example 7: strip_tags('1 <br/> 1', '<br><br/>');
  // * returns 7: '1 <br/> 1'
  allowed = (((allowed || "") + "").toLowerCase().match(/<[a-z][a-z0-9]*>/g) || [])
      .join(''); // making sure the allowed arg is a string containing
            // only tags in lowercase (<a><b><c>)
  var tags = /<\/?([a-z][a-z0-9]*)\b[^>]*>/gi, commentsAndPhpTags = /<!--[\s\S]*?-->|<\?(?:php)?[\s\S]*?\?>/gi;
  return input.replace(commentsAndPhpTags, '').replace(
      tags,
      function($0, $1) {
        return allowed.indexOf('<' + $1.toLowerCase() + '>') > -1 ? $0
            : '';
      });
}