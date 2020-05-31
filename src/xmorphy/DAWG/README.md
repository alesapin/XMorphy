# DAWG
DAWG is highlevel C++ wrapper of <a href="https://code.google.com/archive/p/dawgdic/">dawgdic</a> library. 
It allows you to store template data in very compact and effective 
<a href="https://en.wikipedia.org/wiki/Directed_acyclic_word_graph">Directed acyclic word graph</a>
data structure. 
# Installation
This is <a href = https://en.wikipedia.org/wiki/Header-only>header-only library</a>, so installation is not required. Just put it into your include path or project.
Tested on:
<ul>
<li>g++ 4.8</li>
<li>clang++ 3.5.0</li>
</ul>
# Deps
Library allows you to store and serialize <a href="https://en.wikipedia.org/wiki/Passive_data_structure">POD</a>
or inherited from ISerializable interface classes. This is achived with C++11 &lt; type_traits>, so <i><b>-std=c++0x</b></i>
is required.

# Usage
## Completion example:
```C++
  BuildFactory<double> builder;
  builder.insert("HELLKITIE",23.1);
  builder.insert("HELLO",0.5);
  builder.insert("HELM",23.1);
  shared_ptr<Dictionary<double>> dict = builder.build();
  for(Dictionary<double>::ConstIterator it = dict->begin("HELL");it!=dict->end();++it){
    cout << "[" <<it->key <<","<<it->value <<"]\n";
  }
```
Output
```
[HELLKITIE,23.1]
[HELLO,0.5]
```
## Serialization example:

Example of ISerializable implementation:
```C++
struct FootballTeam : public ISerializable {
  vector<size_t> players;
  FootballTeam(initializer_list<size_t> nums): players(nums) {}
  FootballTeam() {} //necessarily
  virtual bool serialize(ostream& os) const {
    size_t ts = players.size();
    if(!os.write((const char *)(&ts),sizeof(size_t))) return false;
    if(!os.write((const char *)(&players[0]), sizeof(size_t)*ts)) return false;
    return true;
  }
  virtual bool deserialize(istream& is) {
    size_t ts;
    if (!is.read((char *)(&ts), sizeof(size_t))) return false;
    players.resize(ts);
    if (!is.read((char *)(&players[0]),sizeof(size_t)*ts)) return false;
    return true;
  }
};
```
Serialization:
```C++
BuildFactory<FootballTeam> builder;
builder.insert("Bucaneers",{2,12,7,5,3});
builder.insert("Cardinals",{6,43,23,1});
shared_ptr<Dictionary<FootballTeam>> dict = builder.build();
ofstream ofs("bindict",ofstream::binary);
dict->serialize(ofs);
ofs.close();
dict->clear();
ifstream ifs("bindict",ifstream::binary);
dict->deserialize(ifs);
FootballTeam bucaneers = dict->getValue("Bucaneers");
for(int i = 0;i<bucaneers.players.size();++i){
  cout<<"Player number: "<<bucaneers.players[i]<< "\n";
}
ifs.close();
```
Output:
```
Player number:2
Player number:12
Player number:7
Player number:5
Player number:3
```
