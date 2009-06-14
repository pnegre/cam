env = Environment()
env.Append(CCFLAGS = '-g')
 
# We should check that gcc and g++ are installed
# HOW ??
 
# We should check that 'pkg-config' is installed also
 
deps = (
  ['SDL',]
)
 
conf = Configure(env)
for l in deps:
  if not conf.CheckLib(l): exit(1)
 
env = conf.Finish()
 
 
libs = (
	['sdl','opencv']
)
 
for l in libs:
  env.ParseConfig('pkg-config ' + l + ' --cflags --libs')
 
 
env.Program('cam', Glob('*.c*'))