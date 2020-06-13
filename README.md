![Build and run unit tests](https://github.com/alesapin/XMorphy/workflows/Build%20and%20run%20unit%20tests/badge.svg?branch=master)
# XMorphy
Анализатор базируется на словаре [OpenCorpora](http://opencorpora.org/). Для снятия омонимии используется метод CNN.

# Возможности
* Морфологический анализ словарных и несловарных слов
* Снятие омонимии по контексту и без контекста
* Разбор слова по составу (морфемный разбор)

# Зависимости
* [Boost](http://www.boost.org/)
* [icu](http://site.icu-project.org/)
* [git-lfs](https://github.com/git-lfs/git-lfs/wiki/Installation)
* Компилятор с поддержкой C++17

# Сборка
1) `mkdir build`
2) `cd build`
3) `cmake ..`
4) `make -j4`

# Использование бинарника
```
$ xmorphy -c -m
дизельный полярный ледокол<CTRL+d>
дизельный       дизельный       ADJ     Case=Nom|Degree=Pos|Gender=Masc|Number=Sing     DICT    1       дизель:ROOT/н:SUFF/ый:END
полярный        полярный        ADJ     Case=Nom|Degree=Pos|Gender=Masc|Number=Sing     DICT    1       пол:ROOT/ярн:SUFF/ый:END
ледокол ледокол NOUN    Animacy=Inan|Case=Nom|Gender=Masc|Number=Sing   DICT    1       лед:ROOT/о:LINK/кол:ROOT
```

