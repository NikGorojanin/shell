# Conditions

В данной программе реализованы циклы while, for и условие if else для интерпретатора командной строки Shell

## Инициализация

Необходимо подключить заголовочный файл:
```
#include "Conditions.h"
```
Создать экземпляр класса Conditions:
```
Conditions c = Conditions();
```
Для выполнения команд используется метод Start:
```
c.Start( "c = 0 \n if ($c < 10) \n then \n ls \n fi");
```

на вход программы подается строка, разделенна специальным символом '\n', который обозначает новую строку

### Синтаксис команд

На вход подается команда следующего вида

Для цикла For:

for(( ; ; )) \n do \n command1 \n command2 \n done
```
например:
"for ((c = 1; c < 4; c++)) \n do \n ls \n cat test.txt \n done"
```
Для цикла While:

[variables] while ( ) \n do \n command1 \n command2 \n done
```
например:
"c = 5 \n b = 10 \n while ($c < $b) \n then \n ls \n cat test.txt \n ((c++)) \n done"
```

Для условия if else:

[variables] if ( ) \n then \n command1 \n elif( ) \n then \n command2 \n else \n then \n command3 \n fi
```
например:
"c = 1 \n b = 11 \n if ($c < $b) \n then \n ls \n cat test.txt \n fi"
```


### Ограничания
Нельзя использовать вложенные циклы или условия
