#!/bin/sh
#
#--------------------------------------------------------------------
# Create symbolic links for some QCan source files
#
# The variable SYM_DIRS holds all directories which need to be linked.
# 
SYM_FILES="qcan_filter.cpp qcan_filter_list.cpp qcan_frame.cpp qcan_socket.cpp qcan_timestamp.cpp"

     

#--------------------------------------------------------------------
# create sym-links
#
cd src
for f in $SYM_FILES
do
   test -h $f && { echo "Link to '../../$f' already exists";} ||
      { echo "Create sym-link for '$f'";
        ln -s ../../$f $f;}
done
cd ..

#--------------------------------------------------------------------
# create a moc file for the QCanSocket class header 
#
echo "Create a moc file for the QCanSocket class header "
moc ../qcan_socket.hpp -o src/qcan_socket_moc.cpp


#--------------------------------------------------------------------
# check if the 'node_modules' directory is already present
#
test -d node_modules && { echo "Directory 'node_modules' already present";} || 
   { echo "Create 'node_modules' directory";
     npm install node-addon-api;} 

echo " "
echo "Start compilation by running: npm run build"
echo " "