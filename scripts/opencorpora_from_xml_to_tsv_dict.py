#!/usr/bin/env python
# -*- coding: utf-8 -*-
import xml.etree.ElementTree as ET
import sys

# Script allows to convert opencorpora dictionary
# in .xml format to simple tab-separated file
if __name__ == "__main__":
    tree = ET.parse(sys.argv[1])
    root = tree.getroot()
    for text in root:
        print(convert_text(text), end='')
