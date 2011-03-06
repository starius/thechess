

function thechess_countdown_start(private_wtext, private_secs, 
    std_wtext, std_secs)
{
    $('#' + std_wtext)
        .countdown({until: +std_secs, compact: true, format: 'HMS'});

    setTimeout('thechess_countdown_private_(' +
        '"' + private_wtext + '", "' + private_secs + '")', 
        std_secs * 1000);
}

function thechess_countdown_private_(private_wtext, private_secs)
{
    $('#' + private_wtext)
        .countdown({until: +private_secs, compact: true, format: 'HMS'});
}

function thechess_countdown_stop(private_wtext, std_wtext)
{
}



