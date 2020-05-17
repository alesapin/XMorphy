def pytest_addoption(parser):
    parser.addoption("--corpus-file", action="store", default="Corpus to test")
    parser.addoption("--morphem-file", action="store", default="Morphem dict to test")


def pytest_generate_tests(metafunc):
    option_value = metafunc.config.option.corpus_file
    if 'corpus_file' in metafunc.fixturenames and option_value is not None:
        metafunc.parametrize("corpus_file", [option_value])

    option_value = metafunc.config.option.morphem_file
    if 'morphem_file' in metafunc.fixturenames and option_value is not None:
        metafunc.parametrize("morphem_file", [option_value])

