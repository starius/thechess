select to_char(created, 'YYYY-MM'), count(id), sum(octet_length(game_moves) / 1.5)
 from thechess_game group by(to_char(created, 'YYYY-MM'))
 order by to_char(created, 'YYYY-MM');
