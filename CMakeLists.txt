project(XMorphy)
cmake_minimum_required(VERSION 3.0.2)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

find_package(Boost COMPONENTS locale program_options regex filesystem REQUIRED)
find_package(ICU REQUIRED)
find_package(TinyXML2 REQUIRED)

#SET (CMAKE_CXX_COMPILER             "/usr/bin/clang++")
#SET (CMAKE_CXX_FLAGS                "-Wall -O3")
#SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
#SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
#SET (CMAKE_CXX_FLAGS_RELEASE        "-O3 -DNDEBUG")
#SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
#
#SET (CMAKE_AR      "/usr/bin/llvm-ar")
#SET (CMAKE_LINKER  "/usr/bin/llvm-link")
#SET (CMAKE_NM      "/usr/bin/llvm-nm")
#SET (CMAKE_OBJDUMP "/usr/bin/llvm-objdump")
#SET (CMAKE_RANLIB  "/usr/bin/llvm-ranlib")
include_directories(${Boost_INCLUDE_DIR})
include_directories(${TinyXML2_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -O3")
set(EXECUTABLE_OUTPUT_PATH bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
SET(BUILD_SHARED_LIBRARIES OFF)

set(LIBRARY_SOURCE_FILES
    utils/easylogging++.h
    utils/UniString.h
    utils/UniString.cpp
    utils/UniCharacter.h
    utils/UniCharacter.cpp
    tag/ITag.h
    tag/ITag.cpp
    graphem/Token.h
    graphem/Token.cpp
    tag/GraphemTag.h
    tag/GraphemTag.cpp
    tag/AnalyzerTag.h
    tag/AnalyzerTag.cpp
    utils/Misc.h
    tag/TokenTypeTag.h
    tag/TokenTypeTag.cpp
    tag/UniSPTag.h
    tag/UniSPTag.cpp
    tag/UniMorphTag.h
    tag/UniMorphTag.cpp
    tag/RusCorpSPTag.h
    tag/RusCorpSPTag.cpp
    tag/RusCorpMorphTag.h
    tag/RusCorpMorphTag.cpp
    graphem/Tokenizer.h
    graphem/Tokenizer.cpp
    tag/MorphTag.h
    tag/MorphTag.cpp
    tag/SpeechPartTag.h
    tag/SpeechPartTag.cpp
    build/ParadigmBuilder.h
    build/ParadigmBuilder.cpp
    build/SuffixDict.h
    build/SuffixDict.cpp
    build/DictBuilder.cpp
    build/DictBuilder.h
    build/MorphDict.h
    build/MorphDict.cpp
    build/BuildDefs.h
    build/BuildDefs.cpp
    morph/DictMorphAnalyzer.h
    morph/DictMorphAnalyzer.cpp
    morph/PrefixAnalyzer.h
    morph/PrefixAnalyzer.cpp
    morph/SuffixDictAnalyzer.h
    morph/SuffixDictAnalyzer.cpp
    morph/HyphenAnalyzer.h
    morph/HyphenAnalyzer.cpp
    morph/WordForm.h
    morph/WordForm.cpp
    tag/AnalyzerTag.h
    tag/AnalyzerTag.cpp
    tag/OpCorporaRusCorporaConverter.h
    tag/OpCorporaRusCorporaConverter.cpp
    tag/OpCorporaUDConverter.h
    tag/OpCorporaUDConverter.cpp
    tag/PhemTag.h
    tag/PhemTag.cpp
    build/DisambDict.h
    build/DisambDict.cpp
    build/PhemDict.h
    build/PhemDict.cpp
    morph/Processor.h
    morph/Processor.cpp
    IO/IIO.h
    IO/OpCorporaIO.h
    IO/OpCorporaIO.cpp
    disamb/SingleWordDisambiguate.h
    disamb/SingleWordDisambiguate.cpp
    ml/CRF.h
    IO/UniDepIO.h
    IO/UniDepIO.cpp
    tag/OpCorporaUDConverter.h
    tag/OpCorporaUDConverter.cpp
    ml/SpeechPartCrfClassifier.h
    ml/SpeechPartCrfClassifier.cpp
    ml/GenderClassifier.h
    ml/GenderClassifier.cpp
    ml/NumberClassifier.h
    ml/NumberClassifier.cpp
    ml/CaseCrfClassifier.h
    ml/CaseCrfClassifier.cpp
    ml/ITrainGenerator.h
    ml/ITrainGenerator.cpp
    ml/CaseCrfClassifier.h
    ml/CaseCrfClassifier.cpp
    ml/PhemClassifier.h
    ml/PhemClassifier.cpp
    disamb/ContextDisambiguate.h
    disamb/ContextDisambiguate.cpp
    phem/Phemmer.h
    phem/Phemmer.cpp
)

set(SOURCE_FILES main.cpp)

add_library(Xmorphy STATIC ${LIBRARY_SOURCE_FILES})
target_link_libraries(Xmorphy ${Boost_LIBRARIES} ${ICU_LIBRARIES} ${TinyXML2_LIBRARIES} crfpp)

add_executable(xmorphy ${SOURCE_FILES})
target_link_libraries(xmorphy Xmorphy)

install(TARGETS xmorphy Xmorphy
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
  )

install(DIRECTORY models DESTINATION share/xmorphy)
install(DIRECTORY dicts DESTINATION share/xmorphy)
