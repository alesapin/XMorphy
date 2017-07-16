# XMorphy
Анализатор базируется на словаре [OpenCorpora](http://opencorpora.org/) и соответствующей системе морфологических тегов. Для снятия омонимии используется метод CRF++, обученный по открытой части [НКРЯ](http://www.ruscorpora.ru/).

# Возможности
* Морфологический анализ словарных и несловарных слов
* Снятие омонимии по контексту и без контекста
* Разбор слова по составу (морфемный разбор)

# Зависимости
* [Boost](http://www.boost.org/)
* [CRF++](https://taku910.github.io/crfpp/)
* [icu](http://site.icu-project.org/)
* [TinyXML2](http://www.grinninglizard.com/tinyxml2/)

# Сборка
1) cmake .
2) make -j 
3) sudo make install

# Использование бинарника
```
xmorphy -d -p 
мама мыла раму
мама    мама    NOUN    anim|femn|nomn|sing     мам|+а
мыла    мыть    VERB    femn|impf|indc|past|sing|tran   мыл|+а
раму    рама    NOUN    accs|femn|inan|sing     рам|+у
```

# Работа с библиотекой
TODO
