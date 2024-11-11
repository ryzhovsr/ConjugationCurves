# ConjugationCurves

ConjugationCurves это модуль по сопряжению кривых Безье в системе полного жизненного цикла (СПЖЦ) "Сарус", реализованный на языке программирования C++.

## Сборка проекта

Сборка происходит с помощью CMake и компилятора MSVC от Visual Studio.
В зависимости от версии Visual Studio необходимо запустить соотвутствующий make_vs**_x64.cmd.

## Установка модуля

Для запуска данного модуля в СПЖЦ "Сарус" необходимо в корневом файле RPLM.Applications указать:

```
[RPLM.CAD.RPLM.CAD.ConjugationCurves]
path=RPLM.CAD.ConjugationCurves.dll
description=Сопряжение кривых
start=true
```
Также необходимо поместить файл .dll с директорией Resourses, которые были собраны ранее, в корень приложения.