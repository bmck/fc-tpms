// $Id: application.caplyt.js 5189 2015-01-07 23:08:31Z bmck $
// $(c): Copyright 2008-2015 by Capitalytics $
//

// function togglespin(el) {
//   document.getElementById("spinner__" + el.id).style.display = "none";
//   document.getElementById(el.id).style.display = "inline";
// }

function draw_graphs() {
  $('.chartroot').each(function() {
    $.ajax({
      url : $(this).attr('url'),
      datatype : 'script'
    });
  });
}

function showPopup(url) {
  newwindow=window.open(url,'name','height=320,width=470,top=350,left=700,resizable');
  if (window.focus) {newwindow.focus()}
}

function attachCollapsible(collapsible) {
  var div = $('#' + collapsible);
  var heading = $('#' + collapsible + ' > h2');
  var contents = $('#' + collapsible + ' > div');
  if (div && contents && heading) {
    // Initially hide it.
    div.addClass('closed');
    contents.hide();

    // Add an anchor at run time. Non-JavaScript users won't see anchor, but
    // JavaScript users will still be able to use their keyboard for it.
    if ($('#' + collapsible + ' > h2 > a').length) {
      heading
          .html('<a href="#"><img src="/assets/expand.jpg" style="border:0" /></a>'
              + heading.html());
    } else {
      heading
          .html('<a href="#"><img src="/assets/expand.jpg" style="border:0" /> '
              + heading.html() + '</a>');
    }
    $('#' + collapsible + ' > h2 > a:first').click(
        collapsibleOnClick(div, heading, contents));
  }
}

function collapsibleOnClick(div, heading, contents) {
  return function() {
    // Clear the height; it gets messy otherwise if a blind effect is
    // already in progress.
    // It's not perfect, but simple and good enough, and doesn't seem to be
    // able to get into a bad state.
    // contents.css('height','');

    var div_id = div.attr('id');

    // Do we need to blind up or down?
    if (div.hasClass('closed')) {
      $('#' + div_id + ' > h2 > a:first > img:first').attr(
          'src',
          $('h2 > a:first > img:first').attr('src').replace(
              /expand\.jpg/, 'collapse.jpg'));
      contents.show("slow");
    } else {
      $('#' + div_id + ' > h2 > a:first > img:first').attr(
          'src',
          $('h2 > a:first > img:first').attr('src').replace(
              /collapse\.jpg/, 'expand.jpg'));
      contents.hide("slow");
    }
    div.toggleClass('closed');

    // Event has been dealt with.
    return false;
  };
}

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