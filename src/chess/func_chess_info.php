<?


##########################################
# Функции преобразования шахматных строк
##########################################

/*

Base64:
ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/


Как хранится игровое поле:

    1) 64 бита обход сначала по X, затем по Y
        ставится 1, если на этой позиции есть фигура
        
    2) 4 бита * число фигур на доске
        обходим в том же порядке, но пропускаем пустые клеточки
        в каждом квадруплете:
            первый бит: 0 - белые, 1 - черные
            последующие 3 бита кодируют число: 
                1 - слон
                2 - король
                3 - конь
                4 - пешка
                5 - ферзь
                6 - ладья
    
    далее делаем переход на новый байт, если остановились на пол-байте
                      
    3) 1 бит: 1 - ходит черный, 0 - ходит белый
        
    4) 4 бита: возможна ли рокировка с участием ладей:
        a1, h1, a8, h8 
        (a1, h1 - белые; a8, h8 - черные, 
        a1, a8 - длинная; h1, h8 - короткая)
        
        имеется в виду, не ходили соответствующие ладьи или кроли
        
    5) 3 бита - был ли последний ход пешкой на 2 клетки вперед 
        (нужно для возможности установления возможности взятия на проходе)
        если был, то какой (kudax или otkudax) // ?????

        
        
Затем эта бит-последовательность подвергается base64 преобразованию

Изначальная последовательность:

    w8PDw8PDw8NkzjTLFMlUzSTKFMk0y2TOAHg=


*/




/*
переводит строку (base64) в словарь: 
    f - массив [x][y] с инфой о фигуре (1 - слон, 2 - король, 3 - конь, 4 - пешка, 5 - ферзь, 6 - ладья)
    c - массив [x][y] с инфой о цвете (1 - черный, 2 - белый) 
    p - возможность взятия на проходе (был длинный ход пешкой)
    r - список возможностей рокировки (a1, h1, a8, h8)
*/

function chess_str_to_pole($str)
{
    $f = array(); // фигура
    $c = array(); // цвет
    //$p = 0; // взятие на проходе
    $r = array(); // рокировка
    //$h = 0; // кто ходит
    
    
    
    $list = base64_decode($str);
    
   
    
    $second = 16; // номер квадруплета, на котором мы остановились
    
    for ($x = 1; $x <= 8; $x++)
    {
        $t = $list[$x - 1];
        
        for ($y = 1; $y <= 8; $y++)
        {

            
            if (ord($t) & (1 << (8 - $y))) // здесь что-то есть
            {
                // считаем соответствующие координаты из второй секции
                
                $byte = $list[$second >> 1];
                
                $byte = ord($byte);
                

                
                if ((($second >> 1) << 1) == $second)
                {
                    // четный номер квадруплета - первая половина байта
                    $byte = $byte >> 4; // сдвигаем в конец байта
                }
                

                
                // узнаем цвет
                

                
                if ($byte & 8)
                {
                    // если четвертый бит => черный
                    $c[$x][$y] = 1;
                }
                else
                {
                    // белый
                    $c[$x][$y] = 2;
                }
                
                // получаем код фигуры
                
                
                $f[$x][$y] = 7 & $byte;
                
                
                // сдвинем номер квадруплета
                $second++;
            }
            else
            {
                $c[$x][$y] = 0; // пустая клеточка
                $f[$x][$y] = 0;
            }
        }
    }
    
    
    // следующий байт
    
    $second++;
    
    $n = $second >> 1;
    
    //$n++;
    
    
    $byte = $list[$n];
    
    $byte = ord($byte);
    
    
        //$a = array();
        //for ($i = 0; $i < strlen($list); $i++)
        //{
        //    $a []= ord($list[$i]);
        //}
        //echo json($a);
    
    
    
    if ($byte & 128)
    {
        $h = 1; // ходит черный
    }
    else
    {
        $h = 2; // ходит белый
    }    
    
   
    for ($a = 0; $a < 4; $a++)
    {
        if ($byte & (1 << (6 - $a)))
        {
            $r[$a] = 1; // рокировка не запрещена
        }
        else
        {
            $r[$a] = 0; // рокировка запрещена
        }
    }
    
    
    // взятие на проходе
    $p = $byte & 7;


    return array('f' => $f, 'c' => $c, 'p' => $p, 'r' => $r, 'h' => $h);
}










// поле в строку
function chess_pole_to_str($pole)
{
    $f = $pole['f'];
    $c = $pole['c'];
    $p = $pole['p'];
    $r = $pole['r'];
    $h = $pole['h'];
    

    $s1 = ''; // первая секция
    $s2 = array(); // вторая секция
    
    for ($x = 1; $x <= 8; $x++)
    {
        $byte = 0;
        
        for ($y = 1; $y <= 8; $y++)
        {
            
            if ($c[$x][$y])
            {
                // здесь что-то есть
                
                $byte = $byte | (1 << (8 - $y));
                
                
                // элемент для второй секции
                $e = 0;
                
                if ($c[$x][$y] == 1)
                {
                    // цвет
                    $e = $e | 8;
                }
                
                // фигура
                $e = $e | $f[$x][$y];
                
                // добавим элемент в массив для второй секции
                $s2 []= $e;
                
            }
        }
        
        $s1 .= chr($byte);
    }
    
    // теперь скрепим вторую секцию
    
    if ((count($s2) >> 1) << 1 != count($s2))
    {
        // нечетное число членов - добавим нулевой член
        $s2 []= 0x00; 
    }
    
    $L = count($s2) >> 1;
    
    for ($i = 0; $i < $L; $i++)
    {
        $byte1 = $s2[2 * $i];
        $byte = $s2[2 * $i + 1];
        
        $byte1 = $byte1 << 4;
        
        $byte = $byte | $byte1;
        
        $s1 .= chr($byte);
    }
    
    
    // теперь последний байт
    
    $byte = 0x00;
    
    // кто ходит
    if ($h == 1)
    {
        $byte = $byte | 128; // черный ходит
    }
    
    // рокировка
    for ($a = 0; $a < 4; $a++)
    {
        if ($r[$a])
        {
            $byte = $byte | (1 << (6 - $a));
        }
    }
    
    
    // взятие на прходе
    $byte = $byte | $p;
    
    $s1 .= chr($byte);
    
    return base64_encode($s1);
}






/*
    Запись:
        оформляется из расчетв 2 байта на одно передвижение
        
        если ход пешкой на последнюю горизонталь, 
        то на месте kuday пишется код фигуры, 
        в которую надо превратить пешку
        
        взятие на проходе и рокировка - кодируется только ход одной фигуры 
        (берущей пешки или короля)

*/





/*
на вход - base64 строка
*/

$b64 = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/';

function chess_str_to_moves($str)
{
    global $b64;
    
    $moves = array();
    
    $L = strlen($str);
    $L = $L >> 1; // делить на 2 без остатка
    
    for ($i = 0; $i < $L; $i++)
    {
        $otkuda = $str[2 * $i];
        $kuda = $str[2 * $i + 1];
        
        $otkuda = strpos($b64, $otkuda);
        $otkudax = ($otkuda >> 3) + 1;
        $otkuday = ($otkuda & 7) + 1;
        
        $kuda = strpos($b64, $kuda);
        $kudax = ($kuda >> 3) + 1;
        $kuday = ($kuda & 7) + 1;
        
        $moves []= array($otkudax, $otkuday, $kudax, $kuday);
    }
    
    return $moves;
    
}



function chess_move_to_str($move)
{
    global $b64;
    
    $otkudax = $move[0];
    $otkuday = $move[1];
    $kudax = $move[2];
    $kuday = $move[3];
    
    
    $otkuda = ($otkuday - 1) | (($otkudax - 1) << 3);
    $kuda = ($kuday - 1) | (($kudax - 1) << 3);
    
    return $b64[$otkuda] . $b64[$kuda];

}





/*
    производит один ход
        $pole - поле (словарь)
        $move - список координат хода
        
        правильность хода не проверяется
*/

function chess_make_move($pole, $move)
{
    $otkudax = $move[0];
    $otkuday = $move[1];
    $kudax = $move[2];
    $kuday = $move[3];
    
    // изначально снимем возможность взятия на проходе
    $pole['p'] = 0;
    
    
    if ($pole['f'][$otkudax][$otkuday] == 4) // пешка
    {
        // доведение пешки
        
        $ky = 0; // реальное значение kuday для доведенной пешки
        
        if ($pole['h'] == 2 && $otkuday == 7)
        {
            $ky = 8;
        }
        if ($pole['h'] == 1 && $otkuday == 2)
        {
            $ky = 1;
        }
        
        if ($ky)
        {
            // это действительно доведение пешки
            
            if ($kuday != 1 && $kuday != 3 && $kuday != 5 && $kuday != 6)
            {
                // странный тип фигуры, в которую превращают пешку
                $kuday = 5;
            }
            
            
            $pole['f'][$otkudax][$otkuday] = $kuday;
            $kuday = $ky;
        }
        
        //-------//
        
        //взятие на проходе
        
        if ($pole['c'][$kudax][$kuday] == 0 && $kudax != $otkudax)
        {
            // это взятие на проходе: идет наискосок на пустую клетку
            
            $vx = $kudax; // координаты взятой пешки
            $vy = $otkuday;
            
            // убираем взятую пешку
            $pole['f'][$vx][$vy] = 0;
            $pole['c'][$vx][$vy] = 0;
        }
        
        
        //-------//
        
        // длинный ход пешкой
        
        if (abs($otkuday - $kuday) == 2)
        {
            $pole['p'] = $kudax;
        }
        
        
    }
    
    // рокировка
    
    if ($pole['f'][$otkudax][$otkuday] == 2) // король
    {
        if ($otkudax == 5)
        {
            $lx0 = 0; // откуда взять ладью
            $lx1 = 0; // куда поставить ладью
            
            if ($kudax == 3)
            {
                $lx0 = 1;
                $lx1 = 4;
            }
            if ($kudax == 7)
            {
                $lx0 = 8;
                $lx1 = 6;
            }
            
            if ($lx0)
            {
                // это рокировка
                
                // делаем ладью
                $pole['f'][$lx1][$kuday] = 6;
                $pole['c'][$lx1][$kuday] = $pole['c'][$lx0][$kuday];
                
                // убираем ладью
                $pole['f'][$lx0][$kuday] = 0;
                $pole['c'][$lx0][$kuday] = 0;
                
            }
        }
    }
    
    
    
    // передвинем фигуру
    $pole['f'][$kudax][$kuday] = $pole['f'][$otkudax][$otkuday];
    $pole['c'][$kudax][$kuday] = $pole['c'][$otkudax][$otkuday];
    
    // удалим её со старого места
    $pole['f'][$otkudax][$otkuday] = 0;
    $pole['c'][$otkudax][$otkuday] = 0;
    
    
    // если ходил корокль или ладья - запретим рокировку
    //    a1, h1, a8, h8 
    //    (a1, h1 - белые; a8, h8 - черные)  
    
    if ($otkudax == 5 || $otkudax == 1)
    {
        if ($otkuday == 1)
        {
            $pole['r'][0] = 0;
        }
        if ($otkuday == 8)
        {
            $pole['r'][2] = 0;
        }
    }
    
    if ($otkudax == 5 || $otkudax == 8)
    {
        if ($otkuday == 1)
        {
            $pole['r'][1] = 0;
        }
        if ($otkuday == 8)
        {
            $pole['r'][3] = 0;
        }
    }    
    
    
    // ходит теперь другой
    $pole['h'] = 3 - $pole['h'];
    
    return array('pole' => $pole, 'kuday' => $kuday);
    
}




//  $pole = chess_str_to_pole('w8PDw8PDw8NkzjTLFMlUzSTKFMk0y2TOAHg=');
//  
//  $pole['h'] = 2;
//  $pole['p'] = 0;
//  
//  echo chess_pole_to_str($pole);




?>
