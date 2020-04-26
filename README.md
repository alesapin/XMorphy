![Build and run unit tests](https://github.com/alesapin/XMorphy/workflows/Build%20and%20run%20unit%20tests/badge.svg?branch=master)
# XMorphy
Анализатор базируется на словаре [OpenCorpora](http://opencorpora.org/) и соответствующей системе морфологических тегов. Для снятия омонимии используется метод CNN.

# Возможности
* Морфологический анализ словарных и несловарных слов
* Снятие омонимии по контексту и без контекста
* Разбор слова по составу (морфемный разбор)

# Зависимости
* [Boost](http://www.boost.org/)
* [icu](http://site.icu-project.org/)
* [TinyXML2](http://www.grinninglizard.com/tinyxml2/)

# Сборка
1) cmake .
2) make -j4

# Использование бинарника
```
$ xmorphy -c -m
мама мыла кружку
мама    мама    NOUN    Animacy=Anim|Case=Nom|Gender=Fem|Number=Sing    DICT    1       мам:ROOT/а:END
мыла    мыть    VERB    Gender=Fem|Mood=Ind|Number=Sing|Tense=Past|VerbForm=Fin|Voice=Act       DICT    1       мыл:ROOT/а:END
кружку  кружка  NOUN    Animacy=Inan|Case=Acc|Gender=Fem|Number=Sing    DICT    1       кружк:ROOT/у:SUFF
```

