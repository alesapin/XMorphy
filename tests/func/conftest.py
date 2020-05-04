def pytest_addoption(parser):
    parser.addoption("--corpus-file", action="store", default="Corpus to test")


def pytest_generate_tests(metafunc):
    option_value = metafunc.config.option.corpus_file
    if 'corpus_file' in metafunc.fixturenames and option_value is not None:
        metafunc.parametrize("corpus_file", [option_value])
