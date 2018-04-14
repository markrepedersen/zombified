# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.zombie.Debug:
/Users/ppnia/Documents/Winter20172018/CPSC436D/zombified/build_xcode_2/Debug/zombie:\
	/usr/local/lib/libtinyxml2.dylib\
	/usr/local/lib/libglfw.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libz.dylib
	/bin/rm -f /Users/ppnia/Documents/Winter20172018/CPSC436D/zombified/build_xcode_2/Debug/zombie


PostBuild.zombie.Release:
/Users/ppnia/Documents/Winter20172018/CPSC436D/zombified/build_xcode_2/Release/zombie:\
	/usr/local/lib/libtinyxml2.dylib\
	/usr/local/lib/libglfw.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libz.dylib
	/bin/rm -f /Users/ppnia/Documents/Winter20172018/CPSC436D/zombified/build_xcode_2/Release/zombie


PostBuild.zombie.MinSizeRel:
/Users/ppnia/Documents/Winter20172018/CPSC436D/zombified/build_xcode_2/MinSizeRel/zombie:\
	/usr/local/lib/libtinyxml2.dylib\
	/usr/local/lib/libglfw.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libz.dylib
	/bin/rm -f /Users/ppnia/Documents/Winter20172018/CPSC436D/zombified/build_xcode_2/MinSizeRel/zombie


PostBuild.zombie.RelWithDebInfo:
/Users/ppnia/Documents/Winter20172018/CPSC436D/zombified/build_xcode_2/RelWithDebInfo/zombie:\
	/usr/local/lib/libtinyxml2.dylib\
	/usr/local/lib/libglfw.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libz.dylib
	/bin/rm -f /Users/ppnia/Documents/Winter20172018/CPSC436D/zombified/build_xcode_2/RelWithDebInfo/zombie




# For each target create a dummy ruleso the target does not have to exist
/usr/lib/libz.dylib:
/usr/local/lib/libglfw.dylib:
/usr/local/lib/libtinyxml2.dylib:
