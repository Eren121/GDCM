from distutils.core import setup
import glob, os, sys
from distutilsSwigCPlusPlus import *

PythonIncludes=os.path.join(sys.exec_prefix, 'include')
PythonIncludes=os.path.join(PythonIncludes, 'python' + sys.version[:3])

ThisModule='gdcmPython'

gdcmSrcDir="src"
gdcmPythonSrcDir=ThisModule

# Due to a disutil oddity on Unices : see
# http://aspn.activestate.com/ASPN/Mail/Message/distutils-sig/588325
if(os.name=='posix'):
	targetDir=os.path.join('lib', 'python' + sys.version[:3])
	targetDir=os.path.join(targetDir, 'site-packages')
	ExtraPath=ThisModule
else:
	targetDir='Lib'
	ExtraPath=os.path.join(targetDir,ThisModule)

targetDir=os.path.join(targetDir, ThisModule)

setup(name=ThisModule,
      version="0.1",
      description="...",
      author="frog",
      author_email="frog@creatis.insa-lyon.fr",
      url="http://www.creatis.insa-lyon.fr/",
      packages=[ gdcmPythonSrcDir,
                 gdcmPythonSrcDir + '.demo',
               ],
      #package_dir={'':'python'},
      #extra_path = "zob",
      cmdclass={'build_ext':mybuild_ext}, # redirects default build_ext
      ext_modules=[MyExtension(
                   name='_gdcm',
                   sources=[os.path.join(gdcmSrcDir, "gdcmDict.cxx"),
                            os.path.join(gdcmSrcDir, "gdcmDictEntry.cxx"),
                            os.path.join(gdcmSrcDir, "gdcmDictSet.cxx"),
                            os.path.join(gdcmSrcDir, "gdcmElValSet.cxx"),
                            os.path.join(gdcmSrcDir, "gdcmElValue.cxx"),
                            os.path.join(gdcmSrcDir, "gdcmFile.cxx"),
                            os.path.join(gdcmSrcDir, "gdcmHeader.cxx"),
                            os.path.join(gdcmSrcDir, "gdcmUtil.cxx"),
                            os.path.join(gdcmPythonSrcDir, "gdcm.i")],
                   include_dirs=[gdcmSrcDir, PythonIncludes],
                   libraries=["stdc++"],
                   #library_dirs=[VTK_LIB_DIR],
                   swig_cpp=1,
                   swig_include=[gdcmSrcDir] ) ],
      data_files=[(os.path.join(targetDir,"Data"),
                   glob.glob(os.path.join("Data","*.*"))),
                  (os.path.join(targetDir,"Dicts"),
                   glob.glob(os.path.join("Dicts","*.*"))),
                ]
     )
