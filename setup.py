from distutils.core import setup
import glob, os, sys, shutil
from distutilsWrapping import *
from WrapSwig import *
from WrapVTK import *

ThisModule='gdcmPython'
gdcmPythonSrcDir=ThisModule
gdcmSrcDir      ="src"
gdcmvtkSrcDir   ="vtk"
gdcmDictsDir    ="Dicts"
gdcmTestDir     ="Test"

# Due to a disutil oddity on Unices : see
# http://aspn.activestate.com/ASPN/Mail/Message/distutils-sig/588325
if(os.name=='posix'):
	targetDir=os.path.join('lib','python'+sys.version[:3],'site-packages')
	libraries=["stdc++"]
	macros   =[('__STDC_LIMIT_MACROS', '1')]

	VTK_PATH="/usr"
	vtkWrapper="vtkWrapPython"
else:
	targetDir=os.path.join('lib','site-packages')
	libraries=["WSOCK32"]
	macros   =[]

	try:
		VTK_PATH=os.environ['VTK_PATH']
	except KeyError,e:
		err=str(e)
		print "Environment variable",err[err.rfind(':')+1:],'not defined, '\
		       'please fix it!'
		VTK_PATH="c:\\Creatis\\vtkDistrib"
	vtkWrapper=os.path.join(VTK_PATH,"bin","vtkWrapPython")

targetDir=os.path.join(targetDir, ThisModule)

# For the Swig compilation
Sources = []
Sources.extend(glob.glob(os.path.join(gdcmSrcDir,"*.cxx")))
Sources.extend(glob.glob(os.path.join(gdcmSrcDir,"*.h")))
Sources.append(os.path.join(gdcmPythonSrcDir,"gdcm.i"))

# For the VTK compilation
VTK_INCLUDE_DIR=os.path.join(VTK_PATH,"include","vtk")
VTK_LIB_DIR=os.path.join(VTK_PATH,"lib","vtk")

vtkSources = []
vtkSources.extend(glob.glob(os.path.join(gdcmvtkSrcDir,"vtk*.cxx")))
vtkSources.extend(glob.glob(os.path.join(gdcmSrcDir,"*.cxx")))
# vtkSources.extend(glob.glob(os.path.join(gdcmvtkSrcDir,"vtk*.h")))

vtkLibraries=["vtkCommon","vtkCommonPython",
              "vtkIO","vtkIOPython",
              "vtkFiltering","vtkFilteringPython"]

setup(name=ThisModule,
      version="0.2",
      description="...",
      author="frog",
      author_email="frog@creatis.insa-lyon.fr",
      url="http://www.creatis.insa-lyon.fr/",
      packages=[ gdcmPythonSrcDir,
                 gdcmPythonSrcDir + '.demo' ],
      cmdclass={'build_ext':build_extWrap}, # redirects default build_ext
      ext_modules=[SwigExtension(name='_gdcm',
                                 sources=Sources,
                                 include_dirs=[gdcmSrcDir],
                                 libraries=libraries,
                                 define_macros=macros,
                                 swig_cpp=1,
                                 swig_include=[gdcmSrcDir]
                                ),
                   VTKExtension(name='gdcmPython.vtkgdcmPython',
                                sources=vtkSources,
                                include_dirs=[gdcmSrcDir,gdcmvtkSrcDir,VTK_INCLUDE_DIR],
                                libraries=libraries+vtkLibraries,
                                define_macros=macros,
                                library_dirs=[VTK_LIB_DIR],
                                vtkWrapper=vtkWrapper,
                               ),
						],
      data_files=[(os.path.join(targetDir,gdcmTestDir),
                   glob.glob(os.path.join(gdcmTestDir,"*.acr"))),
                  (os.path.join(targetDir,"Dicts"),
                   glob.glob(os.path.join(gdcmDictsDir,"*.*"))),
                ]
     )
