from distutils.command.build_ext import build_ext
from distutils.core import Extension
from types import ListType
import os

class mybuild_ext(build_ext):
	"""
	This distutils command is meant to be used with MyExtension extension, which
	defines a swig_include attribute.
	"""
	build_ext.user_options.append(('swig-include=','S',
											 "list of directories for swig to search in" +
											 "for header files"))
											 
	def initialize_options (self):
		build_ext.initialize_options(self)
		self.swig_include = None
		
	def finalize_options (self):
		build_ext.finalize_options(self)
		if self.swig_include!=None:
			self.swig_include=self.swig_include.split(":")
	
	def build_extension(self, ext):
		# command-line arguments prevail over extension arguments
		# but if no command-line argument is defined, extension argument is
		# taken into account
		if self.swig_include==None:self.swig_include=ext.swig_include
		if self.swig_cpp==None:self.swig_cpp=ext.swig_cpp

		build_ext.build_extension(self,ext)

	def swig_sources (self, sources):

		"""Walk the list of source files in 'sources', looking for SWIG
		interface (.i) files.  Run SWIG on all that are found, and
		return a modified 'sources' list with SWIG source files replaced
		by the generated C (or C++) files.
		"""

		new_sources = []
		swig_sources = []
		swig_targets = {}

		# XXX this drops generated C/C++ files into the source tree, which
		# is fine for developers who want to distribute the generated
		# source -- but there should be an option to put SWIG output in
		# the temp dir.

		## Modified lines (compared to buil_exts.wig_sources original method)
		if self.swig_cpp:
			target_ext = '_wrap.cpp'
		else:
			target_ext = '_wrap.c'
		## End of modification

		for source in sources:
			(base, ext) = os.path.splitext(source)
			if ext == ".i":             # SWIG interface file
				new_sources.append(base + target_ext)
				swig_sources.append(source)
				swig_targets[source] = new_sources[-1]
			elif ext == ".h":
				continue
			else:
				new_sources.append(source)

		if not swig_sources:
			return new_sources

		swig = self.find_swig()

		## Modified lines (compared to buil_exts.wig_sources original method)
		swig_cmd = [swig, "-python"]
		if self.swig_cpp:
			swig_cmd.append("-c++")

		if self.swig_include:
			for pth in self.swig_include:
				swig_cmd.append("-I%s"%pth)
		## End of modification

		for source in swig_sources:
			target = swig_targets[source]
			self.announce("swigging %s to %s" % (source, target))
			self.spawn(swig_cmd + ["-o", target, source])
			## Modified lines (compared to buil_exts.wig_sources original method)
			# When swig generated some shadow classes, place them under
			# self.build_lib (the build directory for Python source).
			if self.swig_cpp:
				# Generate the full pathname of the shadow classes file
				import string
				swig_shadow = string.split(os.path.basename(source), ".")[0]
				swig_shadow = swig_shadow + '.py'
				# On win32 swig places the shadow classes in the directory
				# where it was invoked. This is to be opposed to posix where
				# swig places the shadow classes aside the C++ wrapping code
				# (the target in our context).
				if(os.name=='posix'):
					infile = os.path.join(os.path.dirname(source), swig_shadow)
				else:
					infile = swig_shadow
				if os.path.isfile(infile):
					outfile = [self.build_lib, self.distribution.get_name()]
					outfile.append(swig_shadow)
					outfile = apply(os.path.join, outfile)
					self.copy_file(infile, outfile, preserve_mode=0)
				else:
					self.announce("Warning: swig shadow classes not copied")
			## End of modification

		return new_sources

class MyExtension(Extension):
	"""
	This class extends basic distutils Extension class, adding two keyword
	arguments :
		* swig_cpp, which triggers -c++ mode when swigging
		* swig_include, which specifies -I flag when swigging
	This class is meant to be build with mybuild_ext distutils command.
	"""
	def __init__(self,swig_include=None,swig_cpp=None,**args):
		Extension.__init__(self,**args)

		assert ((swig_include==None or type(swig_include) is ListType),
				  "swig_include must be a list of strings")

		self.swig_include = swig_include or []
		self.swig_cpp = swig_cpp

"""
Example of use of these classes in distutils setup method :

from Transfert.tcDistUtils import mybuild_ext,MyExtension
setup(name="xxx",
		version="X.Y",
		description="blah blah blah",
		author="John Doe",
		author_email="i.hate@spam.com",
		url="http://www.fakeurl.com",
		packages=["yyy"],		
		cmdclass={'build_ext':mybuild_ext}, # redirects default build_ext
		ext_modules=[MyExtension(name="src/xxx, # instance of our Extension class
										 sources=["src/xxx.cpp",
									  				 "src/xxx.i"],
										 include_dirs=["/usr/include/python2.1",
										 					"/usr/include/vtk"],
										 libraries=["vtkGraphics"],
										 swig_cpp=1, # C++ support
										 swig_include=["src"] # SWIG include dirs
										)
						]
		)
		
and then run "python setup.py build"...
"""
