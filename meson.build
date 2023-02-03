project('taskstui', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++14'])

json = dependency('jsoncpp', required: true)

executable('taskstui',
           'main.cpp',
           dependencies : json,
           install : true)
