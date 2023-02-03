project('todocli', 'cpp',
  version : '1.0.0',
  default_options : ['warning_level=3', 'cpp_std=c++17',])

json = dependency('jsoncpp', required: true)

executable('todocli',
           'main.cpp',
           dependencies : json,
           install : true)
