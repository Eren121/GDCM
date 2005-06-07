#
# This module finds if rsync is installed
#
#  RSYNC_EXECUTABLE         = full path to the pike binary
#

FIND_PROGRAM(RSYNC_EXECUTABLE
  NAMES rsync
  PATHS
  /usr/bin
  /usr/local/bin
  )

MARK_AS_ADVANCED(
  RSYNC_EXECUTABLE
  )
