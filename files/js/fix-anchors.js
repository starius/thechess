setInterval(function() {
    $('a').each(function() {
        var a = $(this);
        if (a.attr("href") && !a.attr("onclick")) {
            var external = a[0].hostname.replace(/^www\./, '') !=
                location.hostname.replace(/^www\./, '');
            if (!a.attr("href").match(/^mailto\:/) && external) {
                a.attr('target','_blank');
            } else if (!external) {
                a.attr("onclick", "$.noop()");
                a.click(function() {
                    var e = event || window.event, o = this;
                    var WT = Wt.WT;
                    if (e.ctrlKey || e.metaKey || (WT.button(e) > 1)) {
                        return true;
                    } else {
                        {
                            var f = function() {
                                WT.history.navigate(a[0].pathname, true);
                            };
                            f(o, e);
                        }
                        WT.cancelEvent(e,0x2);
                        // Wt._p_.update(o,'s22b9d',e,true); // FIXME ???
                    }
                });
            }
        }
    });
}, 500);

