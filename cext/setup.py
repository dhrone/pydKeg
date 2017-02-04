from distutils.core import setup, Extension
import numpy.distutils.misc_util

c_ext = Extension("_winstar_weg", ["_winstar_weg.c", "winstar_weg.c"],libraries=['bcm2835'])

include_list = [
	numpy.distutils.misc_util.get_numpy_include_dirs()
]
setup(
    ext_modules=[c_ext],
    include_dirs=include_list
)
