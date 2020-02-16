{
    "targets": [{
        "target_name": "canpie",
        "cflags!": [ "-fno-exceptions" ],
        "sources": [
            'src/qcan_filter.cpp',
            'src/qcan_filter_list.cpp',
            'src/qcan_frame.cpp',
            "<!(moc src/qcan_socket.hpp -o src/qcan_socket_moc.cpp)",
            'src/qcan_socket.cpp',
            'src/qcan_socket_moc.cpp',
            'src/qcan_timestamp.cpp',
            'src/node_canfilter.cpp',
            'src/node_canfilterlist.cpp',
            'src/node_canframe.cpp',
            "<!(moc src/node_cansocket.hpp -o src/node_cansocket_moc.cpp)",
            'src/node_cansocket.cpp',
            'src/node_cansocket_moc.cpp',
            'src/node_init.cpp',
        ],
        'include_dirs': [
            '..',
            '<!@(qmake -query "QT_INSTALL_HEADERS")',
            "<!@(node -p \"require('node-addon-api').include\")"
        ],
        'conditions': [
          ['OS=="linux"',	#---- specific settings for Linux -------------------
            {
              'link_settings': {
		     		 'libraries': [
				   	'-Wl,-rpath,<!@(qmake -query "QT_INSTALL_PREFIX")/lib',
	              ],
               },
              'libraries': [
                  '-F<!@(qmake -query "QT_INSTALL_PREFIX")/lib',
                 ],
            }
          ],
          ['OS=="mac"',    #---- specific settings for macOS -------------------
            {
              'link_settings': {
		     		 'libraries': [
				   	'-Wl,-rpath,<!@(qmake -query "QT_INSTALL_PREFIX")/lib',
					   '-framework QtCore',
					   '-framework QtNetwork'
	              ],
               },
              'libraries': [
                  '-F<!@(qmake -query "QT_INSTALL_PREFIX")/lib',
					   '-framework QtCore',
					   '-framework QtNetwork'
                 ],
             }
          ],
          ['OS=="win"',    #---- specific settings for Windows -----------------
            {
            'include_dirs': [
              # set additional include path for Qt
              '<!@(qmake -query "QT_INSTALL_PREFIX")/include'
            ],
            'libraries': [
              # add required Qt libs
              '<!@(qmake -query "QT_INSTALL_PREFIX")/lib/Qt5Core',
              '<!@(qmake -query "QT_INSTALL_PREFIX")/lib/Qt5Network'
            ]
          }
          ],

        ],
        'dependencies': [
            "<!(node -p \"require('node-addon-api').gyp\")"
        ],
        'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS', 'QT_NO_DEBUG_OUTPUT']
    }]
}
