"""
Setup file for the duinp_littlefs module.
"""

from pathlib import Path
import sys
from setuptools import setup, find_packages
from duino_littlefs.version import __version__

if sys.version_info < (3, 9):
    print('duino_littlefs requires Python 3.9 or newer.')
    sys.exit(1)

here = Path(__file__).parent
long_description = (here / "README.md").read_text()

setup(
    name='duino_littlefs',
    version=__version__,
    author='Dave Hylands',
    author_email='dhylands@gmail.com',
    description=(
        'A duino_cli plugin for working with Arduino LittleFs file systems.'),
    license='MIT',
    keywords='cmd cli arduino duino_cli',
    url='https://github.com/dhylands/duino_littlefs',
    download_url=
    f'https://github.com/dhylands/duino_littlefs/shell/tarball/v{__version__}',
    packages=find_packages(),
    long_description=long_description,
    long_description_content_type='text/markdown',
    classifiers=[
        'Development Status :: 3 - Alpha',
        'Environment :: Console',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: MIT License',
        'Natural Language :: English',
        'Operating System :: POSIX :: Linux',
        'Programming Language :: Python',
        'Programming Language :: Python :: 3',
        'Topic :: Software Development :: Embedded Systems',
        'Topic :: System :: Shells',
        'Topic :: Terminals :: Serial',
        'Topic :: Utilities',
    ],
    install_requires=[
            'duino_cli',
    ],
    entry_points={
        'duino_cli.plugin':
        ['littlefs=duino_littlefs.duino_littlefs:LittleFsPlugin'],
    },
)
