$(document).ready(function(){
    generate_jqplot();
    $(document).unload(function() {$('*').unbind(); });
});

function generate_jqplot(){
    // Based on work found at http://stackoverflow.com/questions/14926656/jqplot-show-image-on-button-click
    if (!$.jqplot.use_excanvas) {
        $('div.chart-target.jqplot').each(function(){
            var elementId = $(this).attr('id');
            var tag = elementId.slice(3);
            var outerDiv = $(document.createElement('div'));
            var header = $(document.createElement('div'));
            var div = $(document.createElement('div'));

            outerDiv.append(header);
            outerDiv.append(div);

            outerDiv.addClass('jqplot-image-container');
            outerDiv.attr('id','jqp-container-'+elementId);
            header.addClass('jqplot-image-container-header');
            div.addClass('jqplot-image-container-content');

            header.html('Right Click to Save Image, or Open Image in New Tab to Print via Browser<br/>');

            var close = $(document.createElement('a'));
            close.addClass('jqplot-image-container-close');
            close.html('<br/>Close');
            close.attr('href', '#');
            close.on('click', {chart: $(this)}, function(evt) {
                var div = evt.data.chart.parent().children('div.jqplot-image-container').last();
                evt.data.chart.children('div.jqplot-copyright').css('visibility','hidden');
                div.hide(300);
                evt.data.chart.show(300);
                evt.data.chart.parent().find('form').show(10);
                evt.data.chart.parent().parent().find('fieldset').show(10);
                div = null;
                return false;
            });
            header.append(close);

            $(this).after(outerDiv);
            outerDiv.hide();

            outerDiv = header = div = close = null;

            if (!$.jqplot._noToImageButton) {
                var btn = $(document.createElement('input'));
                btn.attr('type','submit').attr('value','View/Save/Print Graph Image').attr('id', "btn__" + elementId);
                btn.addClass('jqplot-image-button');
                btn.on('click', {chart: $(this)}, function(evt) {
                    evt.data.chart.children('div.jqplot-copyright').css('visibility','visible');
                    var imgelem = evt.data.chart.jqplotToImageElem();
                    var div = evt.data.chart.parent().children('div.jqplot-image-container').last();
                    div.children('div.jqplot-image-container-content').empty();
                    div.children('div.jqplot-image-container-content').append(imgelem);
                    evt.data.chart.hide(300);
                    evt.data.chart.parent().find('form').hide(10);
                    evt.data.chart.parent().parent().find('fieldset').hide(10);
                    div.show(300);
                    div = null;
                    return false;
                });

                $('#btns__'+elementId).append(btn);
                btn = null;
            }
        });
    }
}