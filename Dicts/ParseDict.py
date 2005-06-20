#! /usr/bin/env python
"""
Let's write our own python parser to clean up the pdf (after 
pdftotext of course). 
Instructions: run pdftotext like this:

$ pdftotext -f 9 -l 81 -raw -nopgbrk 04_06PU.PDF 04_06PU-3.txt

then run the python parser like this:

$ python ParseDict.py 04_06PU.txt dicomV3.dic
"""
import re,os

"""
PdfTextParser takes as input a text file (produced by pdftotext)
and create as output a clean file (ready to be processed) by
DicomV3Expander
Warning: PdfTextParser does not expand:
- (xxxx,xxxx to xxxx) xxxxxxxxxxxx
or
- (12xx, 3456) comment...

"""
class PdfTextParser:
  # Cstor
  def __init__(self):
    self._InputFilename = ''
    self._OutputFilename = ''
    self._Infile = 0
    self._OutLines = []
    self._PreviousBuffers = []

  def SetInputFileName(self,s):
    self._InputFilename = s

  def SetOutputFileName(self,s):
    self._OutputFilename = s
  
  # Function returning if s is a comment for sure
  def IsAComment(self,s):
    #print s,  len(s)
    if s == "Tag Name VR VM":
      return True
    elif s == "PS 3.6-2003":
      return True
    elif s == "PS 3.6-2004":
      return True
    patt = re.compile('^Page [0-9]+$') 
    if( patt.match(s) ):
      return True
    return False

  def IsAStartingLine(self,s):
    patt = re.compile('^\\([0-9a-fA-Fx]+,[0-9a-fA-F]+\\) (.*)$') 
    if( patt.match(s) ):
      return True
    return False

  def IsAFullLine(self,s):
    patt = re.compile('^\\([0-9a-fA-Fx]+,[0-9a-fA-F]+\\) (.*) [A-Z][A-Z] [0-9]$')
    if( patt.match(s) ):
      return True
    return False

  # FIXME this function could we avoided...
  def IsSuspicious(self,s):
    l = len(s)
    if l > 80:
      return True
    return False

  def AddOutputLine(self,s):
    assert not self.IsAComment(s)
    self._OutLines.append(s + '\n')

  def Open(self):
    self._Infile = file(self._InputFilename, 'r')
    for line in self._Infile.readlines():
      line = line[:-1] # remove '\n'
      if not self.IsAComment( line ):
        if self.IsAStartingLine(line):
          #print "Previous buffer:",self._PreviousBuffers
          previousbuffer = ' '.join(self._PreviousBuffers)
          if self.IsAStartingLine(previousbuffer):
            if not self.IsSuspicious(previousbuffer):
              self.AddOutputLine(previousbuffer)
            else:
              # this case should not happen if I were to rewrite the
              # thing I should be able to clean that
              #print "Suspicious:", previousbuffer
              #print "List is:", self._PreviousBuffers
              s = self._PreviousBuffers[0]
              if self.IsAFullLine(s):
                # That means we have a weird line that does not start
                # as usual (xxxx,xxxx) therefore we tried constructing
                # a buffer using a the complete previous line...
                #print "Full line:", s
                self.AddOutputLine(s)
                s2 = ' '.join(self._PreviousBuffers[1:])
                #print "Other Full line:", s2
                self.AddOutputLine(s2)
              else:
                # we have a suspicioulsy long line, so what that could
                # happen, let's check:
                if self.IsAFullLine(previousbuffer):
                  self.AddOutputLine(previousbuffer)
                else:
                  # This is the only case where we do not add
                  # previousbuffer to the _OutLines
                  print "Suspicious and Not a full line:", s
          else:
            if previousbuffer:
              print "Not a buffer:", previousbuffer
          # We can clean buffer, since only the case 'suspicious' +
          # 'Not a full line' has not added buffer to the list
          self._PreviousBuffers = []
          # In all cases save the line for potentially growing this line
          assert not self.IsAComment(line)
          self._PreviousBuffers.append(line)
        else:
          #print "Not a line",line
          assert not self.IsAComment(line)
          self._PreviousBuffers.append(line)
      else:
        #print "Comment:",line
        previousbuffer = ' '.join(self._PreviousBuffers)
        if previousbuffer and self.IsAStartingLine(previousbuffer):
          #print "This line is added:", previousbuffer
          self.AddOutputLine( previousbuffer )
        else:
          #print "Line is comment:", line
          print "Buffer is:", previousbuffer
        # Ok this is a comment we can safely clean the buffer:
        self._PreviousBuffers = []
    self.Write()

  def Write(self):
    outfile = file(self._OutputFilename, 'w')
    outfile.writelines( self._OutLines )
    outfile.close()
    self._Infile.close()
    
  # Main function to call for parsing
  def Parse(self):
    self.Open()

"""
This class is meant to expand line like:
- (xxxx,xxxx to xxxx) xxxxxxxxxxxx
or
- (12xx, 3456) comment...

"""
class DicomV3Expander:
  def __init__(self):
    self._InputFilename = ''
    self._OutputFilename = ''
    self._OutLines = []

  def SetInputFileName(self,s):
    self._InputFilename = s

  def SetOutputFileName(self,s):
    self._OutputFilename = s
 
  # Function to turn into lower case a tag:
  # ex: (ABCD, EF01) -> (abcd, ef01)
  def LowerCaseTag(self,s):
    #print "Before:", s[:-1]
    patt = re.compile('^(\\([0-9a-fA-F]+,[0-9a-fA-F]+\\))(.*)$')
    m = patt.match(s)
    if m:
      s1 = m.group(1)
      s2 = m.group(2)
      return s1.lower() + s2
    else:
      print "Impossible case:", s
      os.sys.exit(1)

  def AddOutputLine(self,s):
    if s.__class__ == list:
      for i in s:
        self._OutLines.append(i + '\n')
    else:
      self._OutLines.append(s + '\n')

  # Expand the line approriaetkly and also add it to the
  # _OutLines list
  def ExpandLine(self, s):
    assert s[-1] == '\n'
    s = s[:-1]  # remove \n
    list = []
    if self.NeedToExpansion(s, list):
      self.AddOutputLine(list) # list != []
    elif self.NeedXXExpansion(s, list):
      self.AddOutputLine(list) # list != []
    else:
      self.AddOutputLine(self.LowerCaseTag(s))

  # If line is like:
  # (0020,3100 to 31FF) Source Image Ids RET
  def NeedToExpansion(self,s, list):
    patt = re.compile('^\\(([0-9a-fA-F]+),([0-9a-fA-F]+) to ([0-9a-fA-F]+)\\)(.*)$')
    m = patt.match(s)
    if m:
      #print m.groups()
      gr = m.group(1)
      el_start = '0x'+m.group(2)
      el_end = '0x'+m.group(3)
      for i in range(eval(el_start), eval(el_end)):
        el = hex(i)[2:]
        l = '('+gr+','+el+')'+m.group(4)
        list.append(l)
      return True
    return False

  # If line is like:
  # (50xx,1200) Number of Patient Related Studies IS 1
  def NeedXXExpansion(self,s,list):
    patt = re.compile('^\\(([0-9a-fA-F]+)xx,([0-9a-fA-F]+)\\)(.*)$')
    m = patt.match(s)
    if m:
      #print m.groups()
      gr_start = m.group(1)
      el = m.group(2)
      #el_start = '0x'+m.group(2)
      #el_end = '0x'+m.group(3)
      start = '0x'+gr_start+'00'
      end   = '0x'+gr_start+'FF'
      for i in range(eval(start), eval(end)):
        gr = hex(i)[2:]
        l = '('+gr+','+el+')'+m.group(3)
        #print l
        list.append(l)
      return True
    return False

  def Write(self):
    outfile = file(self._OutputFilename, 'w')
    outfile.writelines( self._OutLines )
    outfile.close()

  def Expand(self):
    infile = file(self._InputFilename,'r')
    for line in infile.readlines():
      # ExpandLine also LowerCase the line
      self.ExpandLine(line) # l is [1,n] lines
    self.Write()
    infile.close()

if __name__ == "__main__":
  argc = len(os.sys.argv )
  if ( argc < 3 ):
    print "Sorry, wrong list of args"
    os.sys.exit(1) #error

  inputfilename = os.sys.argv[1]
  outputfilename = os.sys.argv[2]
  tempfile = "/tmp/mytemp"
  dp = PdfTextParser()
  dp.SetInputFileName( inputfilename )
  #dp.SetOutputFileName( outputfilename )
  dp.SetOutputFileName( tempfile )
  dp.Parse()

  exp = DicomV3Expander()
  exp.SetInputFileName( tempfile )
  exp.SetOutputFileName( outputfilename )
  exp.Expand()


  #print dp.IsAStartingLine( "(0004,1212) File-set Consistency Flag US 1\n" )
