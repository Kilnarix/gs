from distutils.core import setup, Extension
from Cython.Build import cythonize
#from distutils.extension import Extension
from Cython.Distutils import build_ext
import subprocess
import platform

#will compile a module called SDL in netclient directory
#python linux_SDL_setup.py build_ext --inplace

SYSTEM=platform.system()

SDL_CFLAGS=""
SDL_LDFLAGS=""

try:
    SDL_CFLAGS=subprocess.Popen(['sdl-config', '--cflags'], stdout=subprocess.PIPE).communicate()[0]
    SDL_LDFLAGS=subprocess.Popen(['sdl-config', '--libs'], stdout=subprocess.PIPE).communicate()[0]
except WindowsError:
    # sdl-config is a shell script, windows users will have to provide the path.
    pass

extra_compile_args=[SDL_CFLAGS]
extra_link_args=[SDL_LDFLAGS]

libraries=['SDL', 'SDL_image']

if SYSTEM == 'Windows':
    libraries+=['GLee','opengl32','glu32', 'GLEW',] # 'mega']
    include_dirs = ['/usr/include/SDL']
    runtime_library_dirs = ["./"]
    library_dirs = ["./"]
else:
    libraries+=['GL','GLU', 'GLEW',] # 'mega']
    include_dirs = ['/usr/include/SDL']
    runtime_library_dirs = ["./"]
    library_dirs = ["./"]

debug = True
if debug == True:
    extra_compile_args+=["-g"]
    extra_link_args+=["-g"]


from distutils.unixccompiler import UnixCCompiler

comp = UnixCCompiler()
s_lib=[]

#_tmap: terrain map file
comp.compile(
    sources = [ 'cube_lib/t_map.c',
                'cube_lib/t_properties.c',
                #'cube_lib/t_vbo.c',
                #'cube_lib/t_viz.c'
                ],
    #output_dir="build",
    include_dirs= include_dirs,
    debug=0,
    #extra_preargs= extra_compile_args,
    extra_postargs= extra_compile_args
    )


comp.link_shared_lib(
    objects = [ 'cube_lib/t_map.o',
                'cube_lib/t_properties.o',
                #'cube_lib/t_vbo.o',
                #'cube_lib/t_viz.o'
                ],
    output_libname= "_tmap",
    #output_dir="build",
    libraries=libraries,
    library_dirs=library_dirs,
    #runtime_library_dirs= runtime_library_dirs,
    debug=0,
    extra_preargs= extra_link_args,
    #extra_postargs=None,
)

s_lib += ['_tmap']

#libraries+= ['Mega']

#exit()

SDL_gl = Extension('SDL.gl',
                    include_dirs = include_dirs,
                    libraries = libraries,
                    library_dirs = library_dirs,
                    runtime_library_dirs =  runtime_library_dirs,
                    extra_compile_args = extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = ['SDL/SDL_functions.c',
                                'SDL/camera.c',
                                'SDL/draw_functions.c',
                                'SDL/texture_loader.c',
                                'SDL/particle_functions.c',
                                'SDL/gl.pyx'],
                    )

SDL_input = Extension('SDL.input',
                    include_dirs = include_dirs,
                    libraries = libraries,
                    library_dirs = library_dirs,
                    runtime_library_dirs =  runtime_library_dirs,
                    extra_compile_args = extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = [ 'SDL/input.pyx',
                                'SDL/input_functions.c',
                                'SDL/SDL_functions.c',]
                                )

SDL_hud = Extension('SDL.hud',
                    include_dirs = include_dirs,
                    libraries = libraries,
                    library_dirs = library_dirs,
                    runtime_library_dirs =  runtime_library_dirs,
                    extra_compile_args = extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = [ 'SDL/hud.pyx',
                                'SDL/SDL_text.c',
                                'SDL/draw_functions.c',
                                'SDL/texture_loader.c',
                                #'SDL/hud/block_selector.c',
                                ]
                                )


terrain_map = Extension('cube_lib.terrain_map',
                    include_dirs = include_dirs,
                    libraries = libraries+s_lib ,
                    library_dirs = library_dirs,
                    runtime_library_dirs =  runtime_library_dirs,
                    extra_compile_args = extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = ['cube_lib/terrain_map.pyx',
                            #'cube_lib/t_map.c',
                            #'cube_lib/t_properties.c',
                            'cube_lib/t_vbo.c',
                            'cube_lib/t_viz.c'
                            ]
                    )

vox_lib = Extension('vox_lib',
                    include_dirs = include_dirs,
                    libraries = libraries,
                    library_dirs = library_dirs,
                    runtime_library_dirs =  runtime_library_dirs,
                    extra_compile_args = extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = [
                    'vox_lib/vox_functions.c',
                    'vox_lib/vox_lib.pyx',
                    ]
                                )


#x = distutils.ccompiler.CCompiler()

#import distutils.unixcompiler

'''
from distutils.unixccompiler import UnixCCompiler

comp = UnixCCompiler()

for i in libraries:
    comp.add_library(i)
for i in include_dirs:
    comp.add_include_dir(i)

comp.compile(
    sources = ["vox_lib/vox_functions.c"],
    #output_dir="build",
    include_dirs= include_dirs,
    debug=0,
    extra_preargs= extra_compile_args,
    extra_postargs=None)


comp.link_shared_lib(
    ["vox_lib/vox_functions.o"],
    output_libname= "test2",
    #output_dir="build",
    libraries=None,
    library_dirs=None,
    runtime_library_dirs= ["./",],
    debug=0,
    extra_preargs= extra_link_args,
    extra_postargs=None,
)
'''

'''
comp.link("test", ["vox_lib/vox_functions.o"], "libTest",
    #include_dirs = include_dirs,
    #libraries = libraries, #SDL_image ?
    #extra_compile_args = extra_compile_args,
    #extra_link_args = extra_link_args,

    output_dir="test2/",
    libraries= libraries,
    library_dirs=None,

    runtime_library_dirs=None,
    export_symbols=None,
    debug=0,
    extra_preargs=None,
    extra_postargs=None,
    build_temp=None,
    target_lang=None)
'''

setup(
    cmdclass = {'build_ext': build_ext},
    ext_modules = [vox_lib, SDL_gl, SDL_input, SDL_hud, terrain_map, ], #+ cythonize("*.pyx")
)
