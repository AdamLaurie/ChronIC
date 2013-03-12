import sys
import os
from distutils.core import setup

scripts = ['chronic-cli.py']
modules = ['chronic']

setup  (name        = 'chronic-cli',
        version     = '1.0',
        description = "ChronIC wearable RF hacking tool CLI",
        author = 'Adam Laurie',
        author_email = 'adam@aperturelabs.com',
        scripts = scripts,
	py_modules = modules
       )
