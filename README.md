# Проект wifi-radio на базе mpd с выводом на дисплей PD-2600 #

![Внешний вид](https://github.com/dlinyj/wifi-radio/tree/master/images/0001.jpg)

Проект в стадии разработки вывода на дисплей покупателя PD-2600 и управление станциями с помощью энкодера.

## На данный момент реализовано:

  - Вывод на экран и поддержка русских симвлов
  - Управление с помощью энкодера, или любого UART-устройства, которое посылает в порт "L" - станция в верх по листу, "R" - станция вниз по листу, "P" - пауза
  - Реализовано управление mpd-демоном без использования программы mpc и библиотеки libmpdclient. Реализовано на окетах

# Проблемы и требует тестирования

1. Проблемы с поиском станций, надо провести проверку высчитывания положения символа "|" на экране
2. Подвисает на долго и теряет соединение, когда станции нет. Справедливости ради такая же проблема у [ympd](https://www.ympd.org/)
3. Необходимо длительное тестирование в боевых условиях, на это пока нет времени :(

Связаться с автором можно <dlinyj@gmail.com>
 
License
----
MIT
**Free Software, Hell Yeah!**

