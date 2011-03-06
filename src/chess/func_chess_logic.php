<?

###############################
# логические шахматные функции
###############################




/*
 функция "есть ли такой ход?". 

 эта функция не предусматривает возможность рокировки и взятия на проходе
 не проверяет на удар короля
 не учитывает очередности хода
 
 $move - массив координат (без учета замены координат у доведенной пешки)
*/

function chess_estlihod ($pole, $move)
{
    // 0 - нет хода, 1 - есть ход
    
    $otkudax = $move[0];
    $otkuday = $move[1];
    $kudax = $move[2];
    $kuday = $move[3];
    
    $f = $pole['f'];
    $c = $pole['c'];
    

    $kemhod = $f[$otkudax][$otkuday];
    $mycolor = $c[$otkudax][$otkuday];
    
    $deltay = $kuday - $otkuday;
    $deltax = $kudax - $otkudax;

    // проверка, существует ли вообже фигура, 
    // не съедает ли она свою фигуру и 
    // не совпадает ли начальная и конечная точки
    
    if ($mycolor == $c[$kudax][$kuday])
    {
        return 0;
    }
    
    if ($mycolor != 1 && $mycolor != 2)
    {
        return 0;
    }
    
    
    if ($kudax < 1 || $kudax > 8 || $kuday < 1 || $kuday > 8)
    {
        return 0;
    }
    
    if ($otkudax < 1 || $otkudax > 8 || $otkuday < 1 || $otkuday > 8)
    {
        return 0;
    }
    
    if ($otkuday == $kuday && $otkudax == $kudax)
    {
        return 0;
    }
    
    
    // проверка, если ходит ПЕШКА
    if ($kemhod == 4)
    {
        
        // ход вперед на 1 или на 2
        if (!$c[$kudax][$kuday] && $otkudax == $kudax)
        {
            if ($mycolor * 2 - 3 == $deltay)
            {
                return 1;
            }
            
            $middley = $otkuday + $deltay / 2;
            

            if ($mycolor * 4 - 6 == $deltay && !$c[$otkudax][$middley])
            {
                if ($otkuday ==2 || $otkuday == 7)
                {
                    return 1;
                }
            }
        
        }
        
        if ($c[$kudax][$kuday] && abs($deltax) == 1 && $kuday == $otkuday+$mycolor*2-3)
        {
            return 1;
        }
        
        return 0;
    }


    // провека, если ходит КОРОЛЬ
    if ($kemhod==2)
    {
        if (abs($deltax) <= 1 && abs($deltay) <= 1)
        {
            return 1;
        }
        
        return 0;
    }


    // проверка, если ходит ФЕРЗЬ, ЛАДЬЯ или СЛОН
    if ($kemhod == 5 || $kemhod == 6 || $kemhod == 1)
    {
        
        if ($deltax == 0 || $deltay == 0)
        {
            $lad = 1;
        }
        
        if (abs($deltax) == abs($deltay))
        {
            $slon = 1;
        }
        
        if ($kemhod == 6 && !$lad) // ЛАДЬЯ
        {
            return 0;
        }
        
        if ($kemhod == 1 && !$slon) // СЛОН
        {
            return 0;
        }
        
        if ($kemhod == 5 && !$slon && !$lad) // ФЕРЗЬ
        {
            return 0;
        }         
        
        

        
        if ($deltax > 0)
        {
            $vectorX = 1;
        }
        if ($deltax == 0)
        {
            $vectorX = 0;
        }
        if ($deltax < 0)
        {
            $vectorX = -1;
        }
        
        
        if ($deltay > 0)
        {
            $vectorY = 1;
        }
        if ($deltay == 0)
        {
            $vectorY = 0;
        }
        if ($deltay < 0)
        {
            $vectorY = -1;
        }
        

        
        for ($x = $otkudax, $y = $otkuday; ($x != $kudax || $y != $kuday); $x += $vectorX, $y += $vectorY)
        {
            
            if ($x == $otkudax && $y == $otkuday)
            {
                continue;
            }

            if ($c[$x][$y])
            {
                return 0;
            }
        }
        return 1;
    }
    
    

    // проверка, если ходит КОНЬ
    if ($kemhod==3)
    {
        if (abs($deltax) == 2 && abs($deltay) == 1)
        {
            return 1;
        }
        
        if (abs($deltax) == 1 && abs($deltay) == 2)
        {
            return 1;
        }
        
    }


    return 0;
}











function chess_udarli ($pole, $jx, $jy)
{
    // 0 - нет удара, 1 - есть удар
    
    $f = $pole['f'];
    $c = $pole['c'];
        
    $mycolor = $c[$jx][$jy];

    for ($x = 1; $x <= 8; $x++)
    {
        for ($y = 1; $y <= 8; $y++)
        {
            if ($c[$x][$y] + $mycolor == 3)
            {
                if (chess_estlihod($pole, array($x, $y, $jx, $jy)) == 1)
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}






// функция проверяет, не объявлен ли шах королю $mycolor
function chess_shachli ($pole, $mycol)
{
    // 0 - нет шаха, 1 - есть шах
    
    $f = $pole['f'];
    $c = $pole['c'];
    
    for ($x = 1; $x <= 8; $x++)
    {
        for ($y = 1; $y <= 8; $y++)
        {
            if ($c[$x][$y] == $mycol && $f[$x][$y] == 2)
            {
                return chess_udarli($pole, $x, $y);
            }
        }
    }

}






function chess_bitoli ($pole, $jx, $jy, $mycolor)
{
    // 0 - не бито, 1 - бито
    
    $f = $pole['f'];
    $c = $pole['c'];
    
    // мысленно ставим на это поле пешку моего цвета и проверяем, под ударом ли ОНА.
    $f[$jx][$jy] = 4;
    $c[$jx][$jy] = $mycolor;
    
    return chess_udarli($pole, $jx, $jy);
}











// функция "есть ли такой ход?". Учитывает всё
// Если хода нет - вернет 0
// если есть - новое $pole
// move - измененный (kuday доведенной пешки заменен)
function chess_realhod($pole, $move)
{
    // 0 - нет хода, 1 - есть ход
    
    $rokirovka = 0; // учитывает всё, кроме того, что король не должен оказаться под ударом
    
    $otkudax = $move[0];
    $otkuday = $move[1];
    $kudax = $move[2];
    $kuday = $move[3];
    
    $p = $pole['p'];
    $r = $pole['r'];
    $h = $pole['h'];
    $f = $pole['f'];
    $c = $pole['c'];

    $mycolor = $c[$otkudax][$otkuday];
    $kemhod = $f[$otkudax][$otkuday];
    
    
    if ($mycolor != $h)
    {
        return 0; // сейчас ходит не этот игрок
    }
    


    // Проверим критерием рокировки
    if ($kemhod == 2 && $otkudax == 5 && ($kudax == 3 || $kudax == 7))
    {

        // Это может быть только рокировкой
    
        if ($otkuday != 1 && $otkuday != 8)
        {
            return 0;
        }
        
        if ($otkuday != $kuday)
        {
            return 0;
        }        
    
        if (($otkuday != 1 && $mycolor == 2) || ($otkuday != 8 && $mycolor == 1))
        {
            return 0; // проверка не нужна
        }
        
        
        //    a1, h1, a8, h8 
        //    (a1, h1 - белые; a8, h8 - черные) 
        // [kudax][kuday] 
        $R = array(
            3 => array(1 => 0, 8 => 2),
            7 => array(1 => 1, 8 => 3));
        
        if (!$r[$R[$kudax][$kuday]])
        {
            return 0; // король или ладья уже двигались
        }
        
        
        
        // проверим поле, через которое пролетает ладья
        
        if ($kudax == 3)
        {
            if ($f[2][$kuday])
            {
                return 0; // посередине кто-то есть
            }
        }
        
        
        // проверим клетку, на которую должен встать король
        if ($f[$kudax][$kuday])
        {
            return 0; // занято
        }
//        if (chess_bitoli($pole, $kudax, $kuday, $mycolor))
//        {
//            return 0; // это поле пробито
//        }        
        
        
        // проверим клетку, на которую должна встать ладья
        if ($f[($kudax + $otkudax)/2][$kuday])
        {
            return 0; // занято
        }
        if (chess_bitoli($pole, ($kudax + $otkudax)/2, $kuday, $mycolor))
        {
            return 0; // это поле пробито
        }
        
        
        
        // проверим клетку, на которой стоял король
        if (chess_bitoli($pole, $otkudax, $otkuday, $mycolor))
        {
            return 0; // это поле пробито
        }
        
        
        $rokirovka = 1;

    }


    // сюда попадаем, если это точно не рокировка
  
  
    
    // проверим критерием взятия на проходе
    
    $deltax = $kudax - $otkudax;  
    $deltay = $kuday - $otkuday;
  
    
    if ($kemhod == 4 && abs($deltay) == 1 && abs($deltax) == 1 && !$c[$kudax][$kuday])
    {
        // ничем иным, кроме взятия на проходе, это быть не может
        
        $vx = $kudax; // координаты взятой пешки
        $vy = $otkuday;
        
        if ($vx != $pole['p'])
        {
            return 0; // сейчас не было хода пешкой в нужную клуточку
        }
        
        if ($f[$vx][$vy] != 4)
        {
            return 0; // там нет пешки
        }
        
        if ($c[$vx][$vy] == $mycolor)
        {
            return 0;
        }
        
        // мысленное перемещенеие пешки  
        $pole['f'][$vx][$vy] = 0;
        $pole['c'][$vx][$vy] = 0;
        
        $pole['f'][$kudax][$kuday] = 4;
        $pole['c'][$kudax][$kuday] = 3 - $mycolor;
        
    }
        
    
    $a = chess_make_move($pole, $move);
    
    $move_1 = $move;
    $move_1[3] = $a['kuday']; // kuday
    
    if (!$rokirovka)
    {
        if (!chess_estlihod($pole, $move_1))
        {
            return 0;
        }
    }
    
    $pole = $a['pole']; // новое pole
    
    
    // а теперь проверим, не под ударом ли король
    if (chess_shachli($pole, $mycolor))
    {
        return 0;
    }
    
    return array('pole' => $pole, 'kuday' => $move_1[3]);
}




/* 
функция проверяет, наступил ли конец игры, то есть мат или пат. 
	
информация о том, кто ходит, получается из pole
*/
function chess_konecli ($pole)
{
    $c = $pole['c'];
    $mycolor = $pole['h'];
    
    # 0 - нет конца, 1 - мат Вам, 2 - пат
    for ($x = 1; $x <= 8; $x++)
    {
        for ($y = 1; $y <= 8; $y++)
        {
            if ($c[$x][$y] == $mycolor)
            {
                for ($x1 = 1; $x1 <= 8; $x1++)
                {
                    for ($y1 = 1; $y1 <= 8; $y1++)
                    {
                        if (chess_realhod($pole, array($x, $y, $x1, $y1)))
                        {
                            return 0; // есть ход
                        }

                    }
                }
            }
        }
    }


    if (chess_shachli($pole, $mycolor))
    {

        //если нет такого моего хода, после которого мой король не под ударом, 
        //И сейчас он под ударом, то значит, МАТ.
        return 1;
    }

    # если нет такого моего хода, после которого мой король не под ударом, 
    //И сейчас он НЕ под ударом, то значит, ПАТ.
    return 2;
}








/*
 moves - список ходов
 проводит их до конца
 если prover = 1, то проверяет правильность каждого хода
 если ошибка - возвращает 0
 иначе - итоговое положение
*/
function chess_zapis_prover($moves, $prover)
{
    global $startposition;
    
    $pole = chess_str_to_pole($startposition);
    

    foreach ($moves as $move)
    {
        if ($prover)
        {
            $p = chess_realhod($pole, $move);
            
            if (!$p)
            {
                return 0;
            }
            
            $pole = $p['pole'];
        }
        else
        {
            $p = chess_make_move($pole, $move);
            $pole = $p['pole'];
        }
    }
    
    return $pole;

}







?>
