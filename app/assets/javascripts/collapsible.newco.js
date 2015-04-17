// $Id: collapsible.caplyt.js 5180 2015-01-03 22:03:32Z bmck $
// $(c): Copyright 2008-2015 by Capitalytics $
//

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