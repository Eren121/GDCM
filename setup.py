from distutils.core import setup
import glob, os, sys, shutil
from distutilsSwigCPlusPlus import *

ThisModule='gdcmPython'
gdcmPythonSrcDir=ThisModule
gdcmSrcDir      ="src"
gdcmDictsDir    ="Dicts"
gdcmDataDir     ="Data"

# Due to a disutil oddity on Unices : see
# http://aspn.activestate.com/ASPN/Mail/Message/distutils-sig/588325
if(os.name=='posix'):
   targetDir=os.path.join('lib','python'+sys.version[:3],'site-packages')
   libraries=["stdc++"]
   macros   =[('__STDC_LIMIT_MACROS', '1')]
else:
   targetDir=os.path.join('lib','site-packages')
   libraries=["WSOCK32"]
   macros   =[]

targetDir=os.path.join(targetDir, ThisModule)

Sources = []
Sources.extend(glob.glob(os.path.join(gdcmSrcDir,"*.cxx")))
Sources.extend(glob.glob(os.path.join(gdcmSrcDir,"*.h")))
Sources.append(os.path.join(gdcmPythonSrcDir,"gdcm.i"))

setup(name=ThisModule,
      version="0.1",
      description="...",
      author="frog",
      author_email="frog@creatis.insa-lyon.fr",
      url="http://www.creatis.insa-lyon.fr/",
      packages=[ gdcmPythonSrcDir,
                 gdcmPythonSrcDir + '.demo' ],
      cmdclass={'build_ext':mybuild_ext}, # redirects default build_ext
      ext_modules=[MyExtension(
                   name='_gdcm',
                   sources=Sources,
                   include_dirs=[gdcmSrcDir],
                   libraries=libraries,
                   define_macros=macros,
                   swig_cpp=1,
                   swig_include=[ gdcmSrcDir] ) ],
      data_files=[(os.path.join(targetDir,"Data"),
                   glob.glob(os.path.join(gdcmDataDir,"*.*"))),
                  (os.path.join(targetDir,"Dicts"),
                   glob.glob(os.path.join(gdcmDictsDir,"*.*"))),
                ]
     )
